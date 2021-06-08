/**************************************************************************
 * 	FileName:	 	MCU_UART5.cpp
 *	Description:	This file is including the function interface which is used to processing
 					uart5 communication with mcu 
 *	Copyright(C):	2018-2020 Wave Group Inc.
 *	Version:		V 1.0
 *	Author:			tanqw
 *	Created:		2020-10-26
 *	Updated:		
 *					
**************************************************************************/
#include <vector>
#include <string>

#include "board.h"
#include "pin_mux.h"
#include "fsl_lpuart_freertos.h"
#include "fsl_lpuart.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "sln_vizn_api.h"
#include "MCU_UART5.h"
#include "util_crc16.h"
#include "commondef.h"
#include "user_info.h"

#include "fsl_log.h"
#if defined(FSL_RTOS_FREE_RTOS)
#include "FreeRTOS.h"
#endif

#include "cJSON.h"
#include "util.h"
#include "fsl_pjdisp.h"
#include "fatfs_op.h"
#include "camera_rt106f_elock.h"

#include "../mqtt/config.h"

// 20201120 wszgx added for display correct date/time information in the screen
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* The software timer period. */
#define SW_TIMER_PERIOD_MS (1000 / portTICK_PERIOD_MS)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* The callback function. */
static void SwTimerCallback(TimerHandle_t xTimer);
ws_time_t ws_systime = 0;
static bool timer_started = false;
// 20201120 wszgx end


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_USER_NUM		50
#define UART5_DRV_RX_RING_BUF_SIZE 256
#define DEMO_LPUART          LPUART5
#define DEMO_LPUART_CLK_FREQ BOARD_DebugConsoleSrcFreq()
#define DEMO_LPUART_IRQn     LPUART5_IRQn
/* Task priorities. */
#define uart5_task_PRIORITY (configMAX_PRIORITIES - 1)

uint8_t background_buffer5[UART5_DRV_RX_RING_BUF_SIZE] = {0};

lpuart_rtos_handle_t handle5;
struct _lpuart_handle t_handle5;

lpuart_rtos_config_t lpuart_config5 = {
    .baudrate    = 115200,
    .parity      = kLPUART_ParityDisabled,
    .stopbits    = kLPUART_OneStopBit,
    .buffer      = background_buffer5,
    .buffer_size = sizeof(background_buffer5),
};

uUID g_uu_id;  //记录当前应用的uuid
InitSyncInfo	stInitSyncInfo;		//记录初始化同步信息(电量、MCU版本等)
bool	bInitSyncInfo = false;
bool	bSysTimeSynProc = false;

/*!< Queue used by Uart5 Task to receive messages*/
static QueueHandle_t Uart5MsgQ = NULL;
//人脸注册等待激活
static uint32_t g_reg_start=0;	//记录注册响应发出的时间

static Reg_Status g_reging_flg = REG_STATUS_WAIT;//正在注册流程标记,等待被激活，对识别task发过来的识别结果进行过滤
static face_info_t gFaceInfo; //记录从oasis_rt106f_elcok.cpp中获取当前人脸识别或者注册的相关人脸信息
//userInfo_data_t userInfoList[MAX_USER_NUM] = {0};
static char username[17] = {0}; //用于存放传入NXP提供的人脸注册于识别相关的API的username

static bool lcd_back_ground = true;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void uart5_task(void *pvParameters);
static int Uart5_SendQMsg(void* msg);

int SendMsgToMqtt(unsigned char *MsgBuf, unsigned char MsgLen);

/*******************************************************************************
 * Code
 ******************************************************************************/
uint8_t StrGetUInt8( const uint8_t * i_pSrc )
{
    uint8_t u8Rtn = 0;
    memcpy(&u8Rtn, i_pSrc, 1);
    return u8Rtn;
}

uint16_t StrGetUInt16( const uint8_t * i_pSrc )
{
    uint16_t u16Rtn = 0;
    for ( uint8_t i=0; i<2; i++ )
    {
        uint16_t u16Temp = 0;
        memcpy(&u16Temp, i_pSrc, 1);
        u16Rtn += u16Temp << (2-1-i)*8;
        i_pSrc++;
    }
    return u16Rtn;
}

uint32_t StrGetUInt32( const uint8_t * i_pSrc )
{
    uint32_t u32Rtn = 0;
    for ( uint8_t i=0; i<4; i++ )
    {
        uint32_t u32Temp = 0;
        memcpy(&u32Temp, i_pSrc, 1);
        u32Rtn += u32Temp << (4-1-i)*8;
        i_pSrc++;
    }
    return u32Rtn;
}

void StrSetUInt8( uint8_t * io_pDst, const uint8_t i_u8Src )
{
    memcpy(io_pDst, &i_u8Src, 1);
    io_pDst++;
}

void StrSetUInt16( uint8_t * io_pDst, const uint16_t i_u16Src )
{
    for ( uint8_t i=0; i<2; i++ )
    {
        uint8_t u8Temp = (i_u16Src >> (2-1-i)*8) & 0xFF;
        memcpy(io_pDst, &u8Temp, 1);
        io_pDst++;
    }    
}

void StrSetUInt32( uint8_t * io_pDst, const uint32_t i_u32Src )
{
    for ( uint8_t i=0; i<4; i++ )
    {
        uint8_t u8Temp = (i_u32Src >> (4-1-i)*8) & 0xFF;
        memcpy(io_pDst, &u8Temp, 1);
        io_pDst++;
    }    
}	

/*
// C prototype : void StrToHex(byte *pbDest, char *pszSrc, int nLen)
// parameter(s): [OUT] pbDest - 输出缓冲区
//	[IN] pszSrc - 字符串
//	[IN] nLen - 16进制数的字节数(字符串的长度/2)
// return value:
// remarks : 将字符串转化为16进制数
*/
void StrToHex(unsigned char *pbDest, char *pszSrc, int nLen)
{
	int i;
	char h1, h2;
	unsigned char s1, s2;
	for (i = 0; i < nLen; i++)
	{
		h1 = pszSrc[2 * i];
		h2 = pszSrc[2 * i + 1];
 
		s1 = toupper(h1) - 0x30;
		if (s1 > 9)
			s1 -= 7;
 
		s2 = toupper(h2) - 0x30;
		if (s2 > 9)
			s2 -= 7;
 
		pbDest[i] = s1 * 16 + s2;
	}
}
 
/*
// C prototype : void HexToStr(char *pszDest, byte *pbSrc, int nLen)
// parameter(s): [OUT] pszDest - 存放目标字符串
//	[IN] pbSrc - 输入16进制数的起始地址
//	[IN] nLen - 16进制数的字节数
// return value:
// remarks : 将16进制数转化为字符串
*/
void HexToStr(char *pszDest, unsigned char *pbSrc, int nLen)
{
	char	ddl, ddh;
	int i;
	for (i = 0; i < nLen; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pszDest[i * 2] = ddh;
		pszDest[i * 2 + 1] = ddl;
	}
 
	pszDest[nLen * 2] = '\0';

}

int FileExist(const char* filename)
{
    if (filename && access(filename, F_OK) == 0) {
        return 1;
    }

    return 0;
}

/****************************************************************************************
函数名称：MsgHead_Packet
函数功能：组包
入口参数：pszBuffer--数据
		HeadMark--消息标记
		CmdId--命令
		MsgLen--消息长度
出口参数：无
返回值：成功返回消息的总长度(消息头+消息体+FCS长度)，失败返回-1
****************************************************************************************/
int MsgHead_Packet(
	char *pszBuffer,
	unsigned char HeadMark,
	unsigned char CmdId,
	unsigned char MsgLen)
{
	if (!pszBuffer)
	{
		LOGD("pszBuffer is NULL\n");
		return -1;
	}
	char *pTemp = pszBuffer;

	StrSetUInt8( (uint8_t *)pTemp, HeadMark );
	pTemp += sizeof(uint8_t);
	StrSetUInt8( (uint8_t *)pTemp, CmdId );
	pTemp += sizeof(uint8_t);
	StrSetUInt8( (uint8_t *)pTemp, MsgLen );
	pTemp += sizeof(uint8_t);

	return MsgLen+ sizeof(MESSAGE_HEAD);
}

/****************************************************************************************
函数名称：MsgHead_Unpacket
函数功能：解包
入口参数：pszBuffer--数据
		iBufferSize--数据大小
		HeadMark--标记头
		CmdID--命令
		MsgLen--消息长度
出口参数：无
返回值：成功返回消息内容的指针，失败返回-1
****************************************************************************************/
const unsigned char *MsgHead_Unpacket(
	const unsigned char *pszBuffer,
	unsigned  char iBufferSize,
	unsigned char *HeadMark,
	unsigned char *CmdId,
	unsigned char *MsgLen)
{
	if (!pszBuffer)
	{
		LOGD("pszBuffer is NULL\n");
		return NULL;
	}
	const unsigned char *pTemp = pszBuffer;

	*HeadMark = StrGetUInt8(pTemp);	
	pTemp += sizeof(uint8_t);
	*CmdId = StrGetUInt8(pTemp);
	pTemp += sizeof(uint8_t);
	*MsgLen = StrGetUInt8(pTemp);
	pTemp += sizeof(uint8_t);
	if (*HeadMark != HEAD_MARK && *HeadMark != HEAD_MARK_MQTT)
	{
		LOGD("byVersion[0x%x] != MESSAGE_VERSION[0x%x|0x%x]\n", \
			*HeadMark, HEAD_MARK, HEAD_MARK_MQTT);
		return NULL;
	}

	if ((int)*MsgLen + sizeof(MESSAGE_HEAD) + CRC16_LEN> iBufferSize)
	{
		LOGD("pstMessageHead->MsgLen + sizeof(MESSAGE_HEAD) + CRC16_LEN > iBufferSize\n");
		return NULL;
	}

	return pszBuffer + sizeof(MESSAGE_HEAD);
}

//106F->MCU:通用响应
int cmdCommRsp2MCU(unsigned char CmdId, uint8_t ret)
{
	char szBuffer[128]={0};
	int iBufferSize;
	char *pop = NULL;
	unsigned char MsgLen = 0;

	memset(szBuffer, 0, sizeof(szBuffer));	
	pop = szBuffer+sizeof(MESSAGE_HEAD);
	
	/* 填充消息体 */
	StrSetUInt8((uint8_t*)pop, ret);
	MsgLen += sizeof(uint8_t);
	pop += sizeof(uint8_t);

	/* 填充消息头 */
	iBufferSize = MsgHead_Packet(
		szBuffer,
		HEAD_MARK,
		CmdId,
		MsgLen);
	
	/* 计算FCS */
	unsigned short cal_crc16 = CRC16_X25((uint8_t*)szBuffer, MsgLen+sizeof(MESSAGE_HEAD));
	memcpy((uint8_t*)pop, &cal_crc16, sizeof(cal_crc16));
	
	SendMsgToMCU((uint8_t*)szBuffer, iBufferSize+CRC16_LEN);
	//usleep(10);
	vTaskDelay(pdMS_TO_TICKS(1));

	return 0;
}

int SendMsgToMCU(unsigned char *MsgBuf, unsigned char MsgLen)
{
	int ret = kStatus_Success; 

	int i = 0;
	char message_buffer[64];

	memset(message_buffer, 0, sizeof(message_buffer));
	HexToStr(message_buffer, MsgBuf, MsgLen);
	LOGD("\n===send msg<len:%d %s>:", MsgLen, message_buffer);	
	/*for(i; i<MsgLen; i++)	
	{		
		LOGD("0x%02x	", MsgBuf[i]);	
	}	
	LOGD("\n");*/

	/*
	by tanqw 20200722
	如果后期出现串口数据丢失，需要重发的情况，可以在此增加一个全局UartMsgList，
	把需要发送的消息都先存放到这个List的节点中，并把发送标记置最大重发次数Num，
	然后在face_loop中另外开一个线程，定时从这个List中去取消息发送，然后Num--，当取到的
	消息的发送标记位为0时，删除该节点；当收到对方该消息的响应时，把该消息从List中直接删除。
	*/

	ret = LPUART_RTOS_Send(&handle5, (uint8_t *)MsgBuf, MsgLen);
	if (kStatus_Success != ret)
	{ 
		LOGD("LPUART_RTOS_Send() error,errno<%d>!\n", ret);
		//vTaskSuspend(NULL);
	}

	return ret;
}

//主控发送测试指令:  开机同步请求
int cmdSysInitOKSyncReq(const char *strVersion)
{
	char szBuffer[32]={0};
	int iBufferSize;
	char *pop = NULL;
	unsigned char MsgLen = 0;

	memset(szBuffer, 0, sizeof(szBuffer));	
	pop = szBuffer+sizeof(MESSAGE_HEAD);

	/*填充消息体*/
	memcpy(pop, strVersion, Version_LEN);
	MsgLen += Version_LEN;
	pop += MsgLen;

	/*填充消息头*/
	iBufferSize = MsgHead_Packet(
		szBuffer,
		HEAD_MARK,
		CMD_INITOK_SYNC,
		MsgLen);
	
	/*计算FCS*/
	unsigned short cal_crc16 = CRC16_X25((uint8_t*)szBuffer, MsgLen+sizeof(MESSAGE_HEAD));
	memcpy((uint8_t*)pop, &cal_crc16, sizeof(cal_crc16));
	//LOGD("cal_crc16<0x%X>\n", cal_crc16);
	
	SendMsgToMCU((uint8_t*)szBuffer, iBufferSize+CRC16_LEN);

	return 0;
}

//主控接收指令:  开机同步响应
int cmdSysInitOKSyncRsp(unsigned char nMessageLen, const unsigned char *pszMessage)
{
	unsigned char szBuffer[128]={0};

	if((nMessageLen < sizeof(szBuffer)) && nMessageLen == 4)
	{
		memcpy(szBuffer, pszMessage, nMessageLen);
	}else {
        LOGD("%s : error\n", __FUNCTION__);
		return 0;
	}

	//解析设置参数
	pInitSyncInfo pt_InitSyncInfo;
	pt_InitSyncInfo = (pInitSyncInfo)pszMessage;
	stInitSyncInfo.Mcu_Ver = pt_InitSyncInfo->Mcu_Ver;
	stInitSyncInfo.PowerVal = (pt_InitSyncInfo->PowerVal>100) ? 100 : pt_InitSyncInfo->PowerVal;
	stInitSyncInfo.status = pt_InitSyncInfo->status;	
	stInitSyncInfo.LightVal = pt_InitSyncInfo->LightVal;
	bInitSyncInfo = true;
	LOGD("Mcu_Ver<0x%02x>,PowerVal<%d>, status<%d>, LightVal<%d>\n", stInitSyncInfo.Mcu_Ver, stInitSyncInfo.PowerVal, stInitSyncInfo.status, stInitSyncInfo.LightVal);
	
	//MCU消息处理
	//更新电量显示
	//status中bit 的处理
	if(stInitSyncInfo.status & 0x01) // 防撬开关单次出发(MCU上传后置0)
	{
		//通知UI做出反应
	}
	if(stInitSyncInfo.status & 0x02)// 恢复出厂设置按钮单次触发(MCU上传后置0)
	{
		// 界面跳转到恢复出厂设置或者直接调用恢复出厂设置功能模块
	}
	if(stInitSyncInfo.status & 0x04)// 老化模式(MCU掉电后清0, 由A5配置)
	{
		//运行老化测试程序
	}

#if	0
	if(stInitSyncInfo.LightVal == 2) {
		OpenCameraPWM();
	}else {
		CloseCameraPWM();
	}
#endif
#if	0
	{		
		char verbuf[4] = {0}; 
		char ver_tmp[32] = {0}; 
		
		/* 保存x7 信息到系统配置文件 */
		// 获取字符串格式的版本号
		memset(verbuf, 0, sizeof(verbuf));
		memset(ver_tmp, 0, sizeof(ver_tmp));
		sprintf(ver_tmp, "%s", FIRMWARE_VERSION);
        update_sys_info(ver_tmp);
        //LOGD("SYS_VERSION :<%s>.\n", ver_tmp);

		// 保存设置到系统配置文件
        memset(ver_tmp, 0, sizeof(ver_tmp));
        sprintf(ver_tmp, "%s", PROJECT_VERSION);
		update_project_info(ver_tmp);
		//LOGD("OASIS_VERSION :<%s>.\n", ver_tmp);
	

		/* 保存MCU 信息到系统配置文件 */
		// 获取字符串格式的版本号
		memset(verbuf, 0, sizeof(verbuf));
		memset(ver_tmp, 0, sizeof(ver_tmp));
		sprintf(verbuf, "%03d", stInitSyncInfo.Mcu_Ver);
		sprintf(ver_tmp, "W8_HC130_106F_V%c.%c.%c", verbuf[0], verbuf[1], verbuf[2]);

		// 保存设置到系统配置文件
		update_mcu_info(ver_tmp);
		//read_config("./config.ini");
		//LOGD("MCU_VERSION:<%s>.\n", ver_tmp);
	
		//system("sync");
	}
#endif
#if	0
	// 通知MQTT 上传
    char version[12]={0};
	memcpy(version, VERSION, sizeof(VERSION));
    char subVersion[4]={0};
    subVersion[0]=version[0];
    subVersion[1]=version[2];
    subVersion[2]=version[4];
    LOGD("%s , %s \n", version, subVersion);


    uint16_t sVersion= atoi( subVersion );
    LOGD("%d \n",sVersion);

	sendSysInit2MQTT(sVersion, stInitSyncInfo.PowerVal);
#endif
	return 0;
}

void SetSysToFactory()
{
	VIZN_DelUser (NULL);
}

//主控返回响应指令: 请求恢复出厂设置响应
int cmdReqResumeFactoryRsp(uint8_t ret)
{
	char szBuffer[32]={0};
	int iBufferSize;
	char *pop = NULL;
	unsigned char MsgLen = 0;

	memset(szBuffer, 0, sizeof(szBuffer));	
	pop = szBuffer+sizeof(MESSAGE_HEAD);

	/*填充消息体*/
	StrSetUInt8((uint8_t*)pop, ret);
	MsgLen += sizeof(uint8_t);
	pop += sizeof(uint8_t);	

	/*填充消息头*/
	iBufferSize = MsgHead_Packet(
		szBuffer,
		HEAD_MARK,
		CMD_REQ_RESUME_FACTORY,
		MsgLen);
	
	/*计算FCS*/
	unsigned short cal_crc16 = CRC16_X25((uint8_t*)szBuffer, MsgLen+sizeof(MESSAGE_HEAD));
	memcpy((uint8_t*)pop, &cal_crc16, sizeof(cal_crc16));
	
	SendMsgToMCU((uint8_t*)szBuffer, iBufferSize+CRC16_LEN);

	return 0;
}

//主控接收指令:  请求恢复出厂设置请求
int cmdReqResumeFactoryProc(unsigned char nMessageLen, const unsigned char *pszMessage)
{
	uint8_t ret = SUCCESS;

	//返回响应
	cmdReqResumeFactoryRsp(ret);

	//消息处理
	SetSysToFactory();	

	return 0;
}

//主控返回响应指令: 用户注册
int cmdUserRegRsp(uint8_t ret)
{
	char szBuffer[32]={0};
	int iBufferSize;
	char *pop = NULL;
	unsigned char MsgLen = 0;
	uint8_t status = 0;

	memset(szBuffer, 0, sizeof(szBuffer));	
	pop = szBuffer+sizeof(MESSAGE_HEAD);
	
	/*填充消息体*/
	StrSetUInt8((uint8_t*)pop, status);
	MsgLen += sizeof(uint8_t);
	pop += sizeof(uint8_t);	

	/*填充消息头*/
	iBufferSize = MsgHead_Packet(
		szBuffer,
		HEAD_MARK,
		CMD_FACE_REG,
		MsgLen);
	
	/*计算FCS*/
	unsigned short cal_crc16 = CRC16_X25((uint8_t*)szBuffer, MsgLen+sizeof(MESSAGE_HEAD));
	memcpy((uint8_t*)pop, &cal_crc16, sizeof(cal_crc16));
	
	SendMsgToMCU((uint8_t*)szBuffer, iBufferSize+CRC16_LEN);

	return 0;
}


//主控接收指令:  用户注册请求
int cmdUserRegReqProc(unsigned char nMessageLen, const unsigned char *pszMessage)
{
	uint8_t ret = SUCCESS, len=0;	
	unsigned char szBuffer[32]={0};
	unsigned char *pos = szBuffer;	
	cJSON *root = NULL;
	memcpy(szBuffer, pszMessage, nMessageLen);
	
	//解析指令
	if((nMessageLen < sizeof(szBuffer)) && nMessageLen == 8)
	{
		memcpy(&g_uu_id, pos+len, 8);
		len += 8;
	}

	g_reging_flg = REG_STATUS_ING;
	if(lcd_back_ground == false) {
		OpenLcdBackground();
	}
	
	//LOGD("reg uuid<len=%d> : L<0x%08x>, H<0x%08x>.\n", sizeof(g_uu_id), g_uu_id.tUID.L, g_uu_id.tUID.H);
	
	memset(username, 0, sizeof(username));
	HexToStr(username,g_uu_id.UID,sizeof(g_uu_id.UID));
	username[16] = '\0';//NXP的人脸注册API的username最大只能16byte
	LOGD("=====UUID<len:%d>:%s.\n", sizeof(username), username); 

	root = cJSON_CreateObject();
	if(root != NULL) {
		cJSON_AddItemToObject(root, "UUID", cJSON_CreateString((const char *)username));

		char* buf = NULL;

		buf = cJSON_Print(root);
		LOGD("%s buf is %s\n", __FUNCTION__, buf);

		fatfs_write("user.json", buf, 0, strlen(buf));

		cJSON_Delete(root);
	}
		
	vizn_api_status_t status;	
	status = VIZN_AddUser(NULL, username);
	switch (status)
	{
		case kStatus_API_Layer_AddUser_NoAddCommand:
			LOGD("No add command registered\r\n");
			break;
		case kStatus_API_Layer_AddUser_AddCommandStopped:
			LOGD("Stopped adding %s \r\n");
			break;
		case kStatus_API_Layer_AddUser_InvalidUserName:
			LOGD("Invalid User Name\r\n");
			break;
		case kStatus_API_Layer_AddUser_MultipleAddCommand:
			LOGD("Add command already in pending. Please complete the pending registration or stop the adding\r\n");
			break;
		case kStatus_API_Layer_Success:
			LOGD("Start registering...\r\n");
			break;
		default:
			LOGD("ERROR API ENGINE, status<%d>.\r\n",status);
			break;
	}

	if(kStatus_API_Layer_Success != status)
	{
		ret = FAILED;
	}

	//返回响应消息
	cmdUserRegRsp(ret);
	return 0;
}

//主控发送测试指令: 注册结果通知请求
/* status:
0-完成注册,待激活
1-注册失败
*/
int cmdRegResultNotifyReq(uUID uu_id, uint8_t regResult)
{
	char szBuffer[32]={0};
	int iBufferSize;
	char *pop = NULL;
	unsigned char MsgLen = 0;

	memset(szBuffer, 0, sizeof(szBuffer));	
	pop = szBuffer+sizeof(MESSAGE_HEAD);

	/*填充消息体*/	
	memcpy(pop, uu_id.UID, sizeof(uUID));
	MsgLen += sizeof(uUID);
	pop += sizeof(uUID);
	StrSetUInt8((uint8_t*)pop, regResult);
	MsgLen += sizeof(uint8_t);
	pop += sizeof(uint8_t);

	/*填充消息头*/
	iBufferSize = MsgHead_Packet(
		szBuffer,
		HEAD_MARK,
		CMD_FACE_REG_RLT,
		MsgLen);
	
	/*计算FCS*/
	unsigned short cal_crc16 = CRC16_X25((uint8_t*)szBuffer, MsgLen+sizeof(MESSAGE_HEAD));
	memcpy((uint8_t*)pop, &cal_crc16, sizeof(cal_crc16));
	
	SendMsgToMCU((uint8_t*)szBuffer, iBufferSize+CRC16_LEN);

	return 0;
}

//主控发送测试指令:  开门请求
int cmdOpenDoorReq(uUID uu_id)
{
	char szBuffer[32]={0};
	int iBufferSize;
	char *pop = NULL;
	unsigned char MsgLen = 0;

	memset(szBuffer, 0, sizeof(szBuffer));	
	pop = szBuffer+sizeof(MESSAGE_HEAD);
	
	/*填充消息体*/
	memcpy(pop, uu_id.UID, sizeof(uUID));
	MsgLen += sizeof(uUID);
	pop += sizeof(uUID);

	/*填充消息头*/
	iBufferSize = MsgHead_Packet(
		szBuffer,
		HEAD_MARK,
		CMD_OPEN_DOOR,
		MsgLen);
	
	/*计算FCS*/
	unsigned short cal_crc16 = CRC16_X25((uint8_t*)szBuffer, MsgLen+sizeof(MESSAGE_HEAD));
	memcpy((uint8_t*)pop, &cal_crc16, sizeof(cal_crc16));
	
	SendMsgToMCU((uint8_t*)szBuffer, iBufferSize+CRC16_LEN);

	return 0;
}

//主控发送: 关机请求
int cmdCloseFaceBoardReq()
{
	char szBuffer[32]={0};
	int iBufferSize;
	char *pop = NULL;
	unsigned char MsgLen = 0;

	memset(szBuffer, 0, sizeof(szBuffer));	
	pop = szBuffer+sizeof(MESSAGE_HEAD);

	/*填充消息体*/

	/*填充消息头*/
	iBufferSize = MsgHead_Packet(
		szBuffer,
		HEAD_MARK,
		CMD_CLOSE_FACEBOARD,
		MsgLen);
	
	/*计算FCS*/
	unsigned short cal_crc16 = CRC16_X25((uint8_t*)szBuffer, MsgLen+sizeof(MESSAGE_HEAD));
	memcpy((uint8_t*)pop, &cal_crc16, sizeof(cal_crc16));
	
	SendMsgToMCU((uint8_t*)szBuffer, iBufferSize+CRC16_LEN);

	return 0;
}

//主控返回响应指令: 手机APP请求注册激活响应
int cmdReqActiveByPhoneRsp(uint8_t ret)
{
	char szBuffer[32]={0};
	int iBufferSize;
	char *pop = NULL;
	unsigned char MsgLen = 0;

	memset(szBuffer, 0, sizeof(szBuffer));	
	pop = szBuffer+sizeof(MESSAGE_HEAD);

	/*填充消息体*/
	StrSetUInt8((uint8_t*)pop, ret);
	MsgLen += sizeof(uint8_t);
	pop += sizeof(uint8_t);

	/*填充消息头*/
	iBufferSize = MsgHead_Packet(
		szBuffer,
		HEAD_MARK,
		CMD_REG_ACTIVE_BY_PHONE,
		MsgLen);
	
	/*计算FCS*/
	unsigned short cal_crc16 = CRC16_X25((uint8_t*)szBuffer, MsgLen+sizeof(MESSAGE_HEAD));
	memcpy((uint8_t*)pop, &cal_crc16, sizeof(cal_crc16));
	
	SendMsgToMCU((uint8_t*)szBuffer, iBufferSize+CRC16_LEN);

	return 0;
}

//主控接收指令: 手机APP请求注册激活请求
int cmdReqActiveByPhoneProc(unsigned char nMessageLen, const unsigned char *pszMessage)
{
	uint8_t ret = SUCCESS;
	int result = 0;

	if(REG_STATUS_OK==g_reging_flg)
	{
		//返回响应
		cmdReqActiveByPhoneRsp(ret);
		vTaskDelay(pdMS_TO_TICKS(20));
		
		/*注册激活后关机*/
		cmdCloseFaceBoardReq();
	}
	else
	{//借助APP激活按钮无注册时触发，显示IR图像
		vizn_api_status_t status;	
		status = VIZN_SetDispMode (NULL, DISPALY_MODE_IR);
		if(kStatus_API_Layer_Success != status)
		{
			ret = FAILED;
		}		
	}

	return 0;
}

//主控返回响应指令: 删除用户响应
int cmdUserDeleteRsp(uint8_t result)
{
	char szBuffer[32]={0};
	int iBufferSize;
	char *pop = NULL;
	unsigned char MsgLen = 0;
	
	memset(szBuffer, 0, sizeof(szBuffer));	
	pop = szBuffer+sizeof(MESSAGE_HEAD);

	/*填充消息体*/
	StrSetUInt8((uint8_t*)pop, result);
	MsgLen += sizeof(uint8_t);
	pop += sizeof(uint8_t);

	/*填充消息头*/
	iBufferSize = MsgHead_Packet(
		szBuffer,
		HEAD_MARK,
		CMD_FACE_DELETE_USER,
		MsgLen);
	
	/*计算FCS*/
	unsigned short cal_crc16 = CRC16_X25((uint8_t*)szBuffer, MsgLen+sizeof(MESSAGE_HEAD));
	memcpy((uint8_t*)pop, &cal_crc16, sizeof(cal_crc16));
	
	SendMsgToMCU((uint8_t*)szBuffer, iBufferSize+CRC16_LEN);

	return 0;
}

//主控接收指令:  删除用户请求
int cmdDeleteUserReqProc(unsigned char nMessageLen, const unsigned char *pszMessage)
{
	uUID	uu_id;
	uint8_t ret = SUCCESS, len=0;	
	unsigned char szBuffer[32]={0};
	char username[17] = {0}; //用于存放传入NXP提供的人脸注册于识别相关的API的username
	unsigned char *pos = szBuffer;	
	memcpy(szBuffer, pszMessage, nMessageLen);
	
	//解析指令
	if((nMessageLen < sizeof(szBuffer)) && nMessageLen == 8)
	{
		memcpy(&uu_id, pos+len, 8);
		len += 8;
	}	
	//LOGD("reg uuid<len=%d> : L<0x%08x>, H<0x%08x>.\n", sizeof(g_uu_id), g_uu_id.tUID.L, g_uu_id.tUID.H);
	
	memset(username, 0, sizeof(username));
	HexToStr(username,uu_id.UID,sizeof(uu_id.UID));
	username[16] = '\0';//NXP的人脸注册API的username最大只能16byte
	LOGD("=====UUID<len:%d>:%s.\n", sizeof(username), username); 
	
	vizn_api_status_t status;	
	status = VIZN_DelUser(NULL, username);
	if(kStatus_API_Layer_Success != status)
	{
		ret = FAILED;
	}

	//发送响应
	cmdUserDeleteRsp(ret);

	return 0;
}

static void uart5_startUITimeUpdateTimer()
{	
	if (!timer_started) {
		TimerHandle_t SwTimerHandle = NULL;
		/* Create the software timer. */
		SwTimerHandle = xTimerCreate("SwTimer",          /* Text name. */
                SW_TIMER_PERIOD_MS, /* Timer period. */
				pdTRUE,             /* Enable auto reload. */
				0,                  /* ID is not used. */
				SwTimerCallback);   /* The callback function. */

		/* Start timer. */
		xTimerStart(SwTimerHandle, 0);
		timer_started = true;
		//LOGD("Start timer by wszgx current %d\r\n", ws_systime);
	}
}

//set current time
void SysTimeSet(uint8_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t min,uint8_t sec)
{
	// 20201120 wszgx added for display correct date/time information in the screen
	struct ws_tm t;
	t.tm_year = year + 2000;
	t.tm_mon = month;
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = min;
	t.tm_sec = sec;
	ws_systime = ws_mktime(t);
		
	//启动UI上时间更新定时器
	uart5_startUITimeUpdateTimer();
}

//主控返回响应指令: 时间同步
int cmdSetSysTimeSynRsp(uint8_t ret)
{
	char szBuffer[128]={0};
	int iBufferSize;
	char *pop = NULL;
	unsigned char MsgLen = 0;

	memset(szBuffer, 0, sizeof(szBuffer));	
	pop = szBuffer+sizeof(MESSAGE_HEAD);
	
	/*填充消息体*/
	StrSetUInt8((uint8_t*)pop, ret);
	MsgLen += sizeof(uint8_t);
	pop += sizeof(uint8_t);

	/*填充消息头*/
	iBufferSize = MsgHead_Packet(
		szBuffer,
		HEAD_MARK,
		CMD_NTP_SYS_TIME,
		MsgLen);
	
	/*计算FCS*/
	unsigned short cal_crc16 = CRC16_X25((uint8_t*)szBuffer, MsgLen+sizeof(MESSAGE_HEAD));
	memcpy((uint8_t*)pop, &cal_crc16, sizeof(cal_crc16));
	
	SendMsgToMCU((uint8_t*)szBuffer, iBufferSize+CRC16_LEN);

	return 0;
}

//主控接收指令:  时间同步请求
int cmdSetSysTimeSynProc(unsigned char nMessageLen, const unsigned char *pszMessage)
{
	uint8_t year=0, month=0,day=0,hour=0,min=0,sec=0, i=0;
	uint8_t ret = SUCCESS;
	if(nMessageLen == 6)
	{
		year = StrGetUInt8( pszMessage+i );i++;
		month = StrGetUInt8( pszMessage+i );i++;
		day = StrGetUInt8( pszMessage+i );i++;
		hour = StrGetUInt8( pszMessage+i );i++;
		min = StrGetUInt8( pszMessage+i );i++;
		sec = StrGetUInt8( pszMessage+i );i++;				
	}
	else
	{
		ret = FAILED;
	}
	LOGD("setTime:%04d-%02d-%02d %02d:%02d:%02d\n", 2000+year, month, day, hour, min, sec);
	
	if((month==0||month>12) || (day==0 || day>31) || hour>24 || min>60 || sec>60)
	{
		ret = FAILED;
		LOGD("Set Time value error!\n"); 				
	}
	else
	{		
		/*系统时间设置*/
		SysTimeSet(year, month, day, hour, min, sec);
	}

	bSysTimeSynProc = true;
	
	cmdSetSysTimeSynRsp(ret);

	return 0;
}


//串口接收消息处理
// 主控接收指令: WIFI SSID 设置请求
int cmdWifiSSIDProc(unsigned char nMessageLen, const unsigned char *pszMessage)
{
	uint8_t ret = FAILED;
	char wifi_ssid[WIFI_SSID_LEN_MAX+1]={0};

	// 解析指令
	if((nMessageLen < sizeof(wifi_ssid)) && nMessageLen < WIFI_SSID_LEN_MAX)
	{
		memset(wifi_ssid, 0, sizeof(wifi_ssid));
		memcpy(wifi_ssid, pszMessage, nMessageLen);
		
		// 保存设置到系统配置文件
		LOGD("wifi ssid : <%s>.\n", wifi_ssid);
		update_wifi_ssid("./config.ini", wifi_ssid);	
		//read_config("./config.ini");
		
		ret = SUCCESS;
	}	

	cmdCommRsp2MCU(CMD_WIFI_SSID, ret);
	return 0;
}

// 主控接收指令: WIFI SSID 设置请求
int cmdWifiPwdProc(unsigned char nMessageLen, const unsigned char *pszMessage)
{
	uint8_t ret = FAILED;
	char  wifi_pwd[WIFI_PWD_LEN_MAX+1]={0};

	// 解析指令
	if((nMessageLen < sizeof(wifi_pwd)) && nMessageLen < WIFI_SSID_LEN_MAX)
	{
		memset(wifi_pwd, 0, sizeof(wifi_pwd));
		memcpy(wifi_pwd, pszMessage, nMessageLen);
		
		// 保存设置到系统配置文件
		LOGD("wifi pwd : <%s>.\n", wifi_pwd);
		update_wifi_pwd("./config.ini", wifi_pwd);	
		//read_config("./config.ini");
		ret = SUCCESS;
	}

	cmdCommRsp2MCU(CMD_WIFI_PWD, ret);
	return 0;
}


// 主控接收指令: 蓝牙模块状态信息上报
int cmdBTInfoRptProc(unsigned char nMessageLen, const unsigned char *pszMessage)
{
	uint8_t ret = FAILED;
	uint8_t bt_ver = 0; 
	uint8_t bt_mac[6] = {0};	
	char bt_verbuf[4] = {0}; 
	char bt_ver_tmp[32] = {0}; 
	char bt_mac_tmp[16] = {0}; 
	
	//解析指令
	if(nMessageLen == 1+6)
	{
		bt_ver = StrGetUInt8( pszMessage );
		memset(bt_mac, 0, sizeof(bt_mac));
		memcpy(bt_mac, pszMessage+1, 6);

		// 获取字符串格式的版本号
		memset(bt_verbuf, 0, sizeof(bt_verbuf));		
		memset(bt_ver_tmp, 0, sizeof(bt_ver_tmp));
		sprintf(bt_verbuf, "%03d", bt_ver);
		sprintf(bt_ver_tmp, "W8_52832_V%c.%c.%c", bt_verbuf[0], bt_verbuf[1], bt_verbuf[2]);

		// 转换为字符格式的BT mac
		memset(bt_mac_tmp, 0, sizeof(bt_mac_tmp));
		sprintf(bt_mac_tmp, "%02x%02x%02x%02x%02x%02x",	\
			bt_mac[0],bt_mac[1],bt_mac[2],bt_mac[3],bt_mac[4],bt_mac[5]);

		// 保存设置到系统配置文件
		update_bt_info(bt_ver_tmp, bt_mac_tmp);
		//read_config("./config.ini");
		LOGD("bt_ver :<%s>, bt_mac:<%s>.\n", bt_ver_tmp, bt_mac_tmp);
		
		//system("sync");
		ret = SUCCESS;
	}

	//cmdCommRsp2MCU(CMD_BT_INFO, ret);
	return 0;
}

//======= 需要通过MQTT来转发的消息=====
int SendMsgToMqtt(unsigned char *MsgBuf, unsigned char MsgLen)
{
	int ret; 
#if 1
	if(1/*stSysLogCfg.log_level < LOG_DEBUG*/)
	{
		int i = 0;
		LOGD("\n<<<send to MQTT MsgQueue<len:%d>:", MsgLen);
		for(i; i<MsgLen; i++)	
		{		
			LOGD("0x%02x   ", MsgBuf[i]);
		}	
		LOGD("\n");
	}
#endif
	//ret = MqttQMsgSend(MsgBuf, MsgLen);

	return ret;
}
//开机同步完成后, 请求MQTT上传版本号， 电量，状态
int sendSysInit2MQTT(uint16_t version, uint8_t powerValue)
{
	//LOGD("请求MQTT上传版本号,电量,状态\n");
    LOGD("%s\n", __FUNCTION__);
#if	0
    char szBuffer[32]={0};
    int iBufferSize;
    memset(szBuffer, 0, sizeof(szBuffer));
    char *pop = NULL;
    unsigned char MsgLen = 0;

    pop = szBuffer+sizeof(MESSAGE_HEAD);
    memcpy(pop, &version, 2);
    pop+=2;
    MsgLen+=2;
    memcpy(pop, &powerValue, 1);
    pop+=1;
    MsgLen+=1;

    /* 填充消息头 */
    iBufferSize = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_INITOK_SYNC,
            MsgLen);
    unsigned short cal_crc16 = CRC16_X25((uint8_t*)szBuffer, MsgLen+sizeof(MESSAGE_HEAD));
    memcpy((uint8_t*)pop, &cal_crc16, sizeof(cal_crc16));
    SendMsgToMqtt((uint8_t*)szBuffer, iBufferSize+CRC16_LEN);
#endif
}
int ProcMessage(
	unsigned char nCommand,
	unsigned char nMessageLen,
	const unsigned char *pszMessage)
{ 
	LOGD("======Command[0x%X], nMessageLen<%d>\n", nCommand, nMessageLen);	
	switch (nCommand)
	{
		case CMD_INITOK_SYNC:
		{
			cmdSysInitOKSyncRsp(nMessageLen, pszMessage);
			break;
		}	
		case CMD_OPEN_DOOR:
		{
#if 0
			vizn_api_status_t status;	
			status = VIZN_SetDispMode(NULL,DISPALY_MODE_IR);
			if(status == kStatus_API_Layer_Success){
				LOGD("display ir frame\r\n");
			}	
			else{
				LOGD("display mode not supported\r\n");
			}
#endif
			break;			
		}
		case CMD_FACE_REG:
		{			
			cmdUserRegReqProc(nMessageLen, pszMessage);			
			break;
		}	
		case CMD_FACE_REG_RLT:
		{			
			break;
		}			
		case CMD_NTP_SYS_TIME:
		{
			cmdSetSysTimeSynProc(nMessageLen, pszMessage);
			break;
		}
		case CMD_FACE_DELETE_USER:	
		{
			cmdDeleteUserReqProc(nMessageLen, pszMessage);
			break;
		}
		case CMD_REQ_RESUME_FACTORY:	
		{
			cmdReqResumeFactoryProc(nMessageLen, pszMessage);
			break;
		}	
		case CMD_REG_ACTIVE_BY_PHONE:	
		{
			cmdReqActiveByPhoneProc(nMessageLen, pszMessage);
			break;
		}		

		case CMD_WIFI_SSID:	
		{
			cmdWifiSSIDProc(nMessageLen, pszMessage);
			break;
		}	
		case CMD_WIFI_PWD:	
		{
			cmdWifiPwdProc(nMessageLen, pszMessage);
			break;
		}	

		case CMD_BT_INFO:
		{
			cmdBTInfoRptProc(nMessageLen, pszMessage);
			
			break;
		}
		
		default:
			LOGD("No effective cmd from MCU!\n");
			break;				
	}

	return 0;
}

static char buf[1024] = {0};

static void uart5_QMsg_task(void *pvParameters)
{
	int i = 0;
    BaseType_t ret;
    QMsg* pQMsg;
    int recognize_times = 0;
	cJSON *root ;
    while (1)
    {
		LOGD("uart5_QMsg_task()  -> xQueueReceive()!\n");
        // pick up message
        ret = xQueueReceive(Uart5MsgQ, (void*)&pQMsg, portMAX_DELAY);
        if (ret == pdTRUE) 
		{
        	LOGD("%s cmd.id is %d!\n", __FUNCTION__, pQMsg->msg.cmd.id);
            switch (pQMsg->msg.cmd.id) 
			{
	            case QCMD_ADD_FACE:
				{//处理人脸注册结果
					if(pQMsg->msg.cmd.data.val)
					{//success
						if(gFaceInfo.enrolment && (OASIS_REG_RESULT_DUP==gFaceInfo.rt))
						{//判断是否是重复注册
							LOGD("User face duple register!\n");
							g_reging_flg = REG_STATUS_DUP;
						}
						else
						{
							LOGD("User face register succuss!\n");
							g_reging_flg = REG_STATUS_OK;
						}

						g_reg_start = Time_Now(); //记录注册成功响应发出的时间，并开始计时
					}
					else
					{//failed
						LOGD("User face register failed!\n");
						g_reging_flg = REG_STATUS_FAILED;
					}
					StrToHex(g_uu_id.UID,gFaceInfo.name,sizeof(g_uu_id.UID));
					/*LOGD("\n<<<UUID<len:%d>:%s.\n", sizeof(g_uu_id.UID),gFaceInfo.name);
					for(i = 0; i<sizeof(g_uu_id.UID); i++)
					{		
						LOGD("0x%02x   ", g_uu_id.UID[i]);	
					}
					LOGD("\n"); */
					cmdRegResultNotifyReq(g_uu_id, g_reging_flg);
					vTaskDelay(pdMS_TO_TICKS(200));
					cmdCloseFaceBoardReq();
	            }
				CloseLcdBackground();
				break;
				
				case QCMD_REC_FACE:
				{//处理人脸识别结果
					LOGD("%s g_reging_flg is %d lcd_back_ground is %d\n", __FUNCTION__, g_reging_flg, lcd_back_ground);
					if(REG_STATUS_WAIT != g_reging_flg)//如果正在注册流程，就过滤掉该识别结果
					{
						continue;
					}
					if(lcd_back_ground == false) {
						continue;
					}
					//char buf[1024] = {0};
					memset(buf, 0, sizeof(buf));

					fatfs_read("user.json", buf, 0, sizeof(buf));
					LOGD("buf is %s\n", buf);


					root = cJSON_Parse(buf);
					
					if(root != NULL) {
						cJSON *uuid = cJSON_GetObjectItem(root, "UUID");
						if(uuid != NULL) {
							char *uuid_string = cJSON_GetStringValue(uuid);
					
							LOGD("uuid_string is %s\n", uuid_string);
						}
						cJSON_Delete(root);
					}

					if(pQMsg->msg.cmd.data.val)
					{//success
						LOGD("User face recognize succuss!\n");
						//LOGD("gFaceInfo.name is %s!\n", gFaceInfo.name);
						StrToHex(g_uu_id.UID,gFaceInfo.name,sizeof(g_uu_id.UID));
						//LOGD("\n<<<UUID<len:%d>:%s.\n", sizeof(g_uu_id.UID),gFaceInfo.name);	
						/*for(i; i<sizeof(g_uu_id.UID); i++)	
						{		
							LOGD("0x%02x   ", g_uu_id.UID[i]);	
						}	
						LOGD("\n"); */
						cmdOpenDoorReq(g_uu_id);
						CloseLcdBackground();
					}
					else
					{//failed
						LOGD("User face recognize failed!\n");
						recognize_times ++;
						LOGD("recognize_times is %d\n", recognize_times);
						if(recognize_times > 30) {
							recognize_times = 0;
							cmdCloseFaceBoardReq();//关主控电源
							CloseLcdBackground();
						}
					}
				}
				break;
				default:
					break;
            }
        }

		vPortFree(pQMsg);
    }

}

static void uart5_sync_task(void *pvParameters)
{
	vTaskDelay(pdMS_TO_TICKS(1500));
	check_config();
	cmdSysInitOKSyncReq(SYS_VERSION);
	vTaskDelete(NULL);
}

static void uart5_task(void *pvParameters)
{
    int error;
	int i = 0;
    size_t rcvlen = 0;
    int msglen = 0;
	uint16_t  Msg_CRC16 = 0,Cal_CRC16= 0;
	const unsigned char *pszMsgInfo=NULL;	
	unsigned char HeadMark;
	unsigned char CmdId=0;
	unsigned char datalen = 0;
	uint8_t recv_buffer[64] = {0};
	uint8_t data_tmp;
    LOGD("[uart5_task]:starting...\r\n");
    lpuart_config5.srcclk = DEMO_LPUART_CLK_FREQ;
    lpuart_config5.base   = DEMO_LPUART;

    if (kStatus_Success != LPUART_RTOS_Init(&handle5, &t_handle5, &lpuart_config5))
    {
		LOGD("[uart5_task]:Error,LPUART_RTOS_Init failed!\r\n");
        vTaskSuspend(NULL);
    }

    /* Send introduction message. */	
	//tell mcu that mainboard has Initialized OK.
	//cmdSysInitOKSyncReq(SYS_VERSION);
	
    /* Receive user input and send it back to terminal. */
    do
    {
        //LOGD("[uart5_task]:Recv New Msg ...\r\n");
		memset(recv_buffer, 0, sizeof(recv_buffer));
		msglen=0;
		while(1)
		{
	        error = LPUART_RTOS_Receive(&handle5, &data_tmp, 1, &rcvlen);
	        if (error == kStatus_LPUART_RxHardwareOverrun)
	        {
	            /* Notify about hardware buffer overrun */
	        	LOGD("[uart5_task]:Hardware buffer overrun!\r\n");
				//vTaskSuspend(NULL);
	        }
	        if (error == kStatus_LPUART_RxRingBufferOverrun)
	        {
	            /* Notify about ring buffer overrun */
			    LOGD("[uart5_task]:Ring buffer overrun!\r\n");
				//vTaskSuspend(NULL);
	        }
			//LOGD("[uart5_task]:<0x%02x>	", data_tmp);
			recv_buffer[msglen] = data_tmp;
			msglen++;

			if(HEAD_MARK == recv_buffer[0])
			{
				if((msglen>=5) && (msglen==recv_buffer[2]+5))
				{
					break;
				}
			}
			else
			{//如果不是以HEAD_MARK开头的消息则自动丢弃
				memset(recv_buffer, 0, sizeof(recv_buffer));
				msglen=0;
			}
		}
#if 1		
        if (msglen > 0)
        {
			//LOGD("\n===rcv msg<len:%d>:", msglen);	
			
			char message_buffer[64];
			
			memset(message_buffer, 0, sizeof(message_buffer));
			HexToStr(message_buffer, recv_buffer, msglen);
			LOGD("\n===receive msg<len:%d %s>:", msglen, message_buffer);
			/*for(i=0; i<msglen; i++)	
			{		
				LOGD("0x%02x	", recv_buffer[i]); 
			}	
			LOGD("\n");*/
        }
#endif		
		//收到完整的以HEAD_MARK开头的消息后校验处理
		memcpy(&Msg_CRC16, recv_buffer+msglen-CRC16_LEN, CRC16_LEN);
		Cal_CRC16 = CRC16_X25(recv_buffer, msglen-CRC16_LEN);
		if(Msg_CRC16 != Cal_CRC16)
		{
			LOGD("msg CRC error: Msg_CRC16<0x%02X>, Cal_CRC16<0x%02X>!\n", Msg_CRC16, Cal_CRC16);	
			continue;
		}

		pszMsgInfo = MsgHead_Unpacket(
					recv_buffer,
					msglen,
					&HeadMark,
					&CmdId,
					&datalen);
		
		/*处理消息*/
		ProcMessage(CmdId,
					datalen,
					pszMsgInfo);


    } while (kStatus_Success == error);
    LPUART_RTOS_Deinit(&handle5);
    vTaskSuspend(NULL);
}

static void uart5_Loop_task(void *pvParameters)
{
    uint32_t timeNow = 0;
	uint32_t timeNum = 0;
	
    while (1)
    {
#if 1
		timeNow = Time_Now();
		//LOGD("===name<%s>, recognize<%d>, enrolment<%d>, rt<%d>\n", gFaceInfo.name, gFaceInfo.recognize, gFaceInfo.enrolment, gFaceInfo.rt);
		if(gFaceInfo.enrolment && (OASIS_REG_RESULT_OK==gFaceInfo.rt))
		{
			uint32_t diff = g_reg_start - timeNow;				
			//LOGD("===diff:%u,g_reg_start:%u,timeStop:%u.\n", diff, g_reg_start, timeNow); 
#if	0
			if (diff > (uint32_t)REG_ACTIVE_OUTTIME)
			{
				memset(username, 0, sizeof(username));
				HexToStr(username,g_uu_id.UID,sizeof(g_uu_id.UID));
				username[16] = '\0';//NXP的人脸注册API的username最大只能16byte
				LOGD("===delete UUID<len:%d>:%s.\n", sizeof(username), username); 
		
				vizn_api_status_t status;	
				status = VIZN_DelUser(NULL, username);
				if(kStatus_API_Layer_Success != status)
				{
					LOGD("VIZN_DelUser<%s> failed.\n", username); 
					continue;
				}
				
				break;
			}
#endif			
		}
#endif
#if 0
		//人脸模块工作到最大时间后关机
		if(timeNum>=RT106F_MAX_WORKTIME)
		{
			cmdCloseFaceBoardReq();			
		}
#endif
    	timeNum++;
		vTaskDelay(1000);//delay 1s
    }
	
    vTaskDelete (NULL);

}

int MCU_UART5_Start()
{
    LOGD("[MCU_UART5_Start]:starting...\r\n");	
	NVIC_SetPriority(LPUART5_IRQn, 5);
	
	//创建uart5串口数据通信task
	if (xTaskCreate(uart5_task, "Uart5_task", configMINIMAL_STACK_SIZE + 100, NULL, uart5_task_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }    

	//创建uart5 开机同步task
	if (xTaskCreate(uart5_sync_task, "Uart5_sync_task", configMINIMAL_STACK_SIZE + 100, NULL, uart5_task_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Task uart5_sync_task creation failed!.\r\n");
        while (1);
    }
	//创建UART5 Task的消息接收队列QMSG
	Uart5MsgQ = xQueueCreate(UART5_MSG_Q_COUNT, sizeof(QMsg*));
    if (Uart5MsgQ == NULL)
    {
        LOGE("[ERROR]:xQueueCreate uart5 queue\r\n");
        return -1;
    }

	//创建uart5的QMsg接收task
    if (xTaskCreate(uart5_QMsg_task, "Uart5_QMsg_task", configMINIMAL_STACK_SIZE + 100, NULL, uart5_task_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }  

	//创建循环定时处理的task
    if (xTaskCreate(uart5_Loop_task, "Uart5_Loop_task", configMINIMAL_STACK_SIZE + 100, NULL, uart5_task_PRIORITY-2, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }  

    //LOGD("[MCU_UART5_Start]:started...\r\n");

	return 0;
}

//send qMsg to uart5 task when face register over		
int  Uart5_GetFaceRegResult(uint8_t result)
{
    int status;
    QMsg* pQMsg = (QMsg*)pvPortMalloc(sizeof(QMsg));
    pQMsg->id = QMSG_CMD;
    pQMsg->msg.cmd.id = QCMD_ADD_FACE;
    pQMsg->msg.cmd.data.val = result;
    status = Uart5_SendQMsg((void*)&pQMsg);
	
    return status;
}

//send qMsg to uart5 task when face recognize over			
int  Uart5_GetFaceRecResult(uint8_t result)
{
    int status;
    QMsg* pQMsg = (QMsg*)pvPortMalloc(sizeof(QMsg));
    pQMsg->id = QMSG_CMD;
    pQMsg->msg.cmd.id = QCMD_REC_FACE;
    pQMsg->msg.cmd.data.val = result;
    status = Uart5_SendQMsg((void*)&pQMsg);
	
    return status;
}

static int Uart5_SendQMsg(void* msg)
{
    BaseType_t ret;

    ret = xQueueSend(Uart5MsgQ, msg, (TickType_t)0);

    if (ret != pdPASS) 
    {
        LOGE("[ERROR]:Uart5_SendQMsg failed %d\r\n", ret);
        return -1;
    }

    return 0;
}

//从oasis_rt106f_elcok.cpp中获取识别或者注册时的face_info
void Set_curFaceInfo(char *name, bool recognize, bool enrolment, int rt)
{
	LOGD(">>>name<%s>, recognize<%d>, enrolment<%d>, rt<%d>\n", name, recognize, enrolment, rt);
	memset(gFaceInfo.name, 0, sizeof(gFaceInfo.name));
	memcpy(gFaceInfo.name, name, strlen(name));
	gFaceInfo.recognize = recognize;
	gFaceInfo.enrolment = enrolment;
	gFaceInfo.rt = rt;
}


// 20201120 wszgx added for display correct date/time information in the screen
/*!
 * @brief Software timer callback.
 */
static void SwTimerCallback(TimerHandle_t xTimer)
{
	ws_systime++;
}
// 20201120 wszgx end

void OpenLcdBackground() {
	LOGD("[%s]:\r\n", __FUNCTION__);
	lcd_back_ground = true;
	//if(stInitSyncInfo.LightVal == 2) {
	//	OpenCameraPWM();
	//}
	PJDisp_TurnOnBacklight();
}

void CloseLcdBackground() {
	LOGD("[%s]:\r\n", __FUNCTION__);
	lcd_back_ground = false;
	//if(stInitSyncInfo.LightVal == 2) {
		CloseCameraPWM();
	//}
	PJDisp_TurnOffBacklight();
}
