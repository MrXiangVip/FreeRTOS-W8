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
#include "sln_api.h"
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
#include "WIFI_UART8.h"
#include "database.h"
#include "DBManager.h"
#include "oasis.h"
#include "display.h"
#include "aw_wave_logo_v3.h"
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
#define MAX_USER_NUM        50
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
bool bInitSyncInfo = false;
bool bSysTimeSynProc = false;

/*!< Queue used by Uart5 Task to receive messages*/
static QueueHandle_t Uart5MsgQ = NULL;
//人脸注册等待激活
static uint32_t g_reg_start = 0;    //记录注册响应发出的时间

static Reg_Status g_reging_flg = REG_STATUS_WAIT;//正在注册流程标记,等待被激活，对识别task发过来的识别结果进行过滤
static face_info_t gFaceInfo; //记录从oasis_rt106f_elcok.cpp中获取当前人脸识别或者注册的相关人脸信息
//userInfo_data_t userInfoList[MAX_USER_NUM] = {0};
static char username[17] = {0}; //用于存放传入NXP提供的人脸注册于识别相关的API的username

bool lcd_back_ground = true;
extern int battery_level;
static bool saving_file = false;
static bool saving_db = false;
extern int g_is_shutdown;
bool g_is_save_file = false;
bool bDeleteUser = false;
extern int mqtt_init_done;
int boot_mode = BOOT_MODE_INVALID;  //0:短按;1：长按;2:蓝牙设置;3:蓝牙人脸注册;4:睡眠状态
bool oasis_task_start = false;
#if (configSUPPORT_STATIC_ALLOCATION == 1)
DTC_BSS static StackType_t Uart5TaskStack[UART5TASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart5TaskTCB;

DTC_BSS static StackType_t Uart5SyncTaskStack[UART5SYNCTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart5SyncTaskTCB;

DTC_BSS static StackType_t Uart5QmsgTaskStack[UART5QMSGTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart5QmsgTaskTCB;

#if	0
DTC_BSS static StackType_t Uart5LoopTaskStack[UART5LOOPTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart5LoopTaskTCB;
#endif
#endif


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void uart5_task(void *pvParameters);

static int Uart5_SendQMsg(void *msg);

/*******************************************************************************
 * Code
 ******************************************************************************/

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
        unsigned char MsgLen) {
    if (!pszBuffer) {
        LOGD("pszBuffer is NULL\n");
        return -1;
    }
    char *pTemp = pszBuffer;

    StrSetUInt8((uint8_t *) pTemp, HeadMark);
    pTemp += sizeof(uint8_t);
    StrSetUInt8((uint8_t *) pTemp, CmdId);
    pTemp += sizeof(uint8_t);
    StrSetUInt8((uint8_t *) pTemp, MsgLen);
    pTemp += sizeof(uint8_t);

    return MsgLen + sizeof(MESSAGE_HEAD);
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
        unsigned char iBufferSize,
        unsigned char *HeadMark,
        unsigned char *CmdId,
        unsigned char *MsgLen) {
    if (!pszBuffer) {
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
    if (*HeadMark != HEAD_MARK && *HeadMark != HEAD_MARK_MQTT) {
        LOGD("byVersion[0x%x] != MESSAGE_VERSION[0x%x|0x%x]\n", \
            *HeadMark, HEAD_MARK, HEAD_MARK_MQTT);
        return NULL;
    }

    if ((int) *MsgLen + sizeof(MESSAGE_HEAD) + CRC16_LEN > iBufferSize) {
        LOGD("pstMessageHead->MsgLen + sizeof(MESSAGE_HEAD) + CRC16_LEN > iBufferSize\n");
        return NULL;
    }

    return pszBuffer + sizeof(MESSAGE_HEAD);
}

//106F->MCU:通用响应
int cmdCommRsp2MCU(unsigned char CmdId, uint8_t ret) {
    char szBuffer[32] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /* 填充消息体 */
    StrSetUInt8((uint8_t *) pop, ret);
    MsgLen += sizeof(uint8_t);
    pop += sizeof(uint8_t);

    /* 填充消息头 */
    iBufferSize = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CmdId,
            MsgLen);

    /* 计算FCS */
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));

    SendMsgToMCU((uint8_t *) szBuffer, iBufferSize + CRC16_LEN);
    //usleep(10);
    vTaskDelay(pdMS_TO_TICKS(1));

    return 0;
}

int  SendMsgToSelf(unsigned char *MsgBuf, unsigned char MsgLen)
{
    unsigned char szBuffer[128] = {0};

    const unsigned char *pszMsgInfo=NULL;
    unsigned char CmdId=0;
    unsigned char HeadMark;

    memset(szBuffer, 0, sizeof(szBuffer));
	memcpy(szBuffer, &MsgBuf[0], MsgLen);

	pszMsgInfo = MsgHead_Unpacket(
				szBuffer,
				MsgLen,
				&HeadMark,
				&CmdId,
				&MsgLen);

	if(HeadMark != HEAD_MARK_MQTT)
	{
		LOGD("msg headmark error: 0x%x!\n", HeadMark);
		return -1;
	}
	ProcMessage(CmdId, MsgLen, pszMsgInfo);

    return 0;
}

int SendMsgToMCU(unsigned char *MsgBuf, unsigned char MsgLen) {
    if (g_is_shutdown) {
        return 0;
    }
    int ret = kStatus_Success;

    char message_buffer[64];

    memset(message_buffer, 0, sizeof(message_buffer));
    HexToStr(message_buffer, MsgBuf, MsgLen);
    LOGD("\n===send msg<len:%d %s>: \r\n", MsgLen, message_buffer);

    /*
    by tanqw 20200722
    如果后期出现串口数据丢失，需要重发的情况，可以在此增加一个全局UartMsgList，
    把需要发送的消息都先存放到这个List的节点中，并把发送标记置最大重发次数Num，
    然后在face_loop中另外开一个线程，定时从这个List中去取消息发送，然后Num--，当取到的
    消息的发送标记位为0时，删除该节点；当收到对方该消息的响应时，把该消息从List中直接删除。
    */

    ret = LPUART_RTOS_Send(&handle5, (uint8_t *) MsgBuf, MsgLen);
    if (kStatus_Success != ret) {
        LOGD("LPUART_RTOS_Send() error,errno<%d>!\r\n", ret);
        //vTaskSuspend(NULL);
    }

    return ret;
}

//主控发送测试指令:  开机同步请求
int cmdSysInitOKSyncReq(const char *strVersion) {
    LOGD(" 发送开机同步请求 \r\n");
    char szBuffer[32] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

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
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));
    //LOGD("cal_crc16<0x%X>\n", cal_crc16);

    SendMsgToMCU((uint8_t *) szBuffer, iBufferSize + CRC16_LEN);

    return 0;
}

int cmdRemoteRequestRsp(unsigned char nHead, unsigned char CmdId, uint8_t ret) {
    LOGD("%s\r\n", __func__);
    char szBuffer[32] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /* 填充消息体 */
    StrSetUInt8((uint8_t *) pop, ret);
    MsgLen += sizeof(uint8_t);
    pop += sizeof(uint8_t);

    /* 填充消息头 */
    iBufferSize = MsgHead_Packet(
            szBuffer,
            nHead,
            CmdId,
            MsgLen);

    /* 计算FCS */
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));

    LOGD("%s send to 0x%02x cmd 0x%02x \r\n", __FUNCTION__, nHead, CmdId);

    int count = 0;
    while (1) {
        // 发送指令给 wifi 相关的mqtt 模块
        //usleep(100000);//0.1秒
        vTaskDelay(pdMS_TO_TICKS(100));
        if (count++ > 150) {//如果超过15秒 还没有连接上则退出
            LOGD("WIFI 未连接  \r\n");
            break;
        }
        //判断MQTT是否可用
        if (mqtt_init_done == 1) {// 1 代表MQTT连接成功
            LOGD("WIFI 连接成功,请求mqtt发送msg  \r\n");
            SendMsgToMQTT(szBuffer, iBufferSize + CRC16_LEN);
            break;
        }
    }

    return 0;
}
//主控接收指令:  开机同步响应
int cmdSysInitOKSyncRsp(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("接收开机同步响应 \r\n");
    unsigned char szBuffer[32] = {0};

	memcpy(szBuffer, pszMessage, nMessageLen);

    uint8_t year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0, i = 0;
	year = StrGetUInt8(pszMessage + i);
	i++;
	month = StrGetUInt8(pszMessage + i);
	i++;
	day = StrGetUInt8(pszMessage + i);
	i++;
	hour = StrGetUInt8(pszMessage + i);
	i++;
	min = StrGetUInt8(pszMessage + i);
	i++;
	sec = StrGetUInt8(pszMessage + i);
	i++;
    LOGD("setTime:%04d-%02d-%02d %02d:%02d:%02d \r\n", 2000 + year, month, day, hour, min, sec);

    if ((month == 0 || month > 12) || (day == 0 || day > 31) || hour > 24 || min > 60 || sec > 60) {
        LOGD("Set Time value error!\r\n");
    } else {
        /*系统时间设置*/
        SysTimeSet(year, month, day, hour, min, sec);
    }

    bSysTimeSynProc = true;

    //解析设置参数
    battery_level = StrGetUInt8(pszMessage + i);
    i++;
	LOGD("battery_level:<%d>.\r\n", battery_level);
	bInitSyncInfo = true;


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
        //sprintf(verbuf, "%03d", stInitSyncInfo.Mcu_Ver);
        //sprintf(ver_tmp, "W8_HC130_106F_V%c.%c.%c", verbuf[0], verbuf[1], verbuf[2]);
        verbuf[1]=StrGetUInt8(pszMessage + i);
        i++;
		verbuf[0]=StrGetUInt8(pszMessage + i);
		i++;
		sprintf(ver_tmp, "W8_HC130_106F_V%d.%d.%d", verbuf[1], verbuf[0]>>4, verbuf[0]&0x0f );
		LOGD("MCU_VERSION:<%s>.\r\n", ver_tmp);

        // 保存设置到系统配置文件
        update_mcu_info(ver_tmp);
        //read_config("./config.ini");
        //LOGD("MCU_VERSION:<%s>.\n", ver_tmp);

        //system("sync");
    }

    boot_mode = StrGetUInt8(pszMessage + i);
    if (boot_mode > BOOT_MODE_MECHANICAL_LOCK) {
        boot_mode = BOOT_MODE_INVALID;
    }
    LOGD("boot_mode: %d\r\n", boot_mode);
    if((boot_mode == BOOT_MODE_NORMAL) || (boot_mode == BOOT_MODE_REG)) {
        if(oasis_task_start == false) {
        	oasis_task_start = true;
        	OpenLcdBackground();
        	Display_Start();
            Oasis_Start();

            //Display_Update((uint32_t)wave_logo_v3);
        }
    }else if(boot_mode == BOOT_MODE_REMOTE) {
    	cmdRemoteRequestRsp(HEAD_MARK, CMD_BOOT_MODE, 1);
    }

    return 0;
}

void SetSysToFactory() {
    list < Record * > recordList = DBManager::getInstance()->getRecord();
    list<Record *>::iterator it;
    for (it = recordList.begin(); it != recordList.end(); it++) {
        Record *tmpRecord = (Record * ) * it;
        fatfs_delete(tmpRecord->image_path);
    }

    // 清空操作记录
    DBManager::getInstance()->clearRecord();

	VIZN_DelUser(NULL);
    DB_Save(0);
}

//主控返回响应指令: 请求恢复出厂设置响应
int cmdReqResumeFactoryRsp(uint8_t ret) {
    char szBuffer[32] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /*填充消息体*/
    StrSetUInt8((uint8_t *) pop, ret);
    MsgLen += sizeof(uint8_t);
    pop += sizeof(uint8_t);

    /*填充消息头*/
    iBufferSize = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_REQ_RESUME_FACTORY,
            MsgLen);

    /*计算FCS*/
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));

    SendMsgToMCU((uint8_t *) szBuffer, iBufferSize + CRC16_LEN);

    return 0;
}

//主控接收指令:  请求恢复出厂设置请求
int cmdReqResumeFactoryProc(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("请求恢复出厂设置请求 \r\n");

    uint8_t ret = SUCCESS;

    CloseLcdBackground();
    //返回响应
    cmdReqResumeFactoryRsp(ret);

    vTaskDelay(pdMS_TO_TICKS(10));

    /*关机 */
    cmdCloseFaceBoardReq();


    //消息处理
    SetSysToFactory();
    return 0;
}

//主控返回响应指令: 用户注册
int cmdUserRegRsp(uint8_t ret) {
    LOGD("用户注册回复 \r\n");
    char szBuffer[32] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = 0;
    uint8_t status = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /*填充消息体*/
    StrSetUInt8((uint8_t *) pop, status);
    MsgLen += sizeof(uint8_t);
    pop += sizeof(uint8_t);

    /*填充消息头*/
    iBufferSize = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_FACE_REG,
            MsgLen);

    /*计算FCS*/
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));

    SendMsgToMCU((uint8_t *) szBuffer, iBufferSize + CRC16_LEN);

    return 0;
}


//主控接收指令:  用户注册请求
int cmdUserRegReqProc(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("用户注册请求 \r\n");
    uint8_t ret = SUCCESS, len = 0;
    unsigned char szBuffer[32] = {0};
    unsigned char *pos = szBuffer;
    cJSON *root = NULL;
    memcpy(szBuffer, pszMessage, nMessageLen);

    //解析指令
    if ((nMessageLen < sizeof(szBuffer)) && nMessageLen == 8) {
        memcpy(&g_uu_id, pos + len, 8);
        len += 8;
    }

    g_reging_flg = REG_STATUS_ING;
    boot_mode = BOOT_MODE_REG;
    if (lcd_back_ground == false) {
        OpenLcdBackground();
    }

    //LOGD("reg uuid<len=%d> : L<0x%08x>, H<0x%08x>.\n", sizeof(g_uu_id), g_uu_id.tUID.L, g_uu_id.tUID.H);

    memset(username, 0, sizeof(username));
    HexToStr(username, g_uu_id.UID, sizeof(g_uu_id.UID));
    username[16] = '\0';//NXP的人脸注册API的username最大只能16byte
    LOGD("=====UUID<len:%d>:%s.\r\n", sizeof(username), username);


    vizn_api_status_t status;
    status = VIZN_AddUser(NULL, username);
    switch (status) {
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
            LOGD("ERROR API ENGINE, status<%d>.\r\n", status);
            break;
    }

    if (kStatus_API_Layer_Success != status) {
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
int cmdRegResultNotifyReq(uUID uu_id, uint8_t regResult) {
    char szBuffer[32] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /*填充消息体*/
    memcpy(pop, uu_id.UID, sizeof(uUID));
    MsgLen += sizeof(uUID);
    pop += sizeof(uUID);
    StrSetUInt8((uint8_t *) pop, regResult);
    MsgLen += sizeof(uint8_t);
    pop += sizeof(uint8_t);

    /*填充消息头*/
    iBufferSize = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_FACE_REG_RLT,
            MsgLen);

    /*计算FCS*/
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));

    SendMsgToMCU((uint8_t *) szBuffer, iBufferSize + CRC16_LEN);

    return 0;
}

//主控发送测试指令:  开门请求
int cmdOpenDoorReq(uUID uu_id) {
    LOGD("向mcu发送开门请求 \r\n \n");
    char szBuffer[32] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

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
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));

    SendMsgToMCU((uint8_t *) szBuffer, iBufferSize + CRC16_LEN);

    return 0;
}

// 主控接收指令:开门响应
int cmdOpenDoorRsp(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("x7 收到mcu 的开门响应 \n");
    uint8_t ret = -1;
    unsigned char *pop = NULL;
    unsigned char szBuffer[32]={0};

    // MCU到face_loop，0代表开锁成功，1代表开锁失败
    memcpy(szBuffer, pszMessage, nMessageLen);

    pop = szBuffer;

    // MCU到face_loop，0代表开锁成功，1代表开锁失败
    ret = StrGetUInt8(pszMessage);
    // 如果开锁成功 更新数据库状态 ,请求mqtt上传本次操作记录。
    if (ret == 0) {
        //					xshx add
        //char power_msg[32] = {0};
        pop += 1;
        uint8_t power = StrGetUInt8(pop);
        pop += 1;
        uint8_t  power2 = StrGetUInt8(pop);

        Record *record = (Record *) pvPortMalloc(sizeof(Record));
        HexToStr(username, g_uu_id.UID, sizeof(g_uu_id.UID));
        strcpy(record->UUID, username);
        //record->action = 3;//  操作类型：0代表注册 1: 一键开锁 2：钥匙开锁  3 人脸识别开锁
        char image_path[16];
        //record->status = 0; // 0,操作成功 1,操作失败.
        record->time_stamp = ws_systime; //时间戳 从1970年开始的秒数
        record->power = power * 256 + power2;
        //sprintf(power_msg, "{\\\"batteryA\\\":%d\\,\\\"batteryB\\\":%d}", record->power, record->power2);
        //LOGD("power_msg is %s \r\n", power_msg);

        //record->upload = 0; //   0代表没上传 1代表记录上传图片未上传 2代表均已
        record->action_upload = 0x300;
        memset(image_path, 0, sizeof(image_path)); // 对注册成功的用户保存一张压缩过的jpeg图片
        //snprintf(image_path, sizeof(image_path), "REC_%d_%d_%s.jpg", 0, record->time_stamp, record->UUID);
        snprintf(image_path, sizeof(image_path), "C%d", record->time_stamp);
        memcpy(record->image_path, image_path, sizeof(image_path));//image_path

        DBManager::getInstance()->addRecord(record);

        Oasis_SetOasisFileName(record->image_path);
		//Oasis_WriteJpeg();

        int ID = DBManager::getInstance()->getLastRecordID();
        LOGD("开锁成功, 更新数据库状态.请求MQTT上传本次开门的记录 \n");
        cmdRequestMqttUpload(ID);
    } else {
        LOGD("开锁失败,不更新数据库状态. 不上传记录\n");
    }

    return 0;
}

int cmdPowerDownRsp(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("收到MCU发来的下电回复 \r\n");
    for( int i=0; i<nMessageLen ; i++){
        LOGD("0x%02x	\r\n", pszMessage[i]);
    }
    return  0;
}
// 主控接收指令:机械开锁响应
int cmdMechicalLockRsp(unsigned char nMessageLen, const unsigned char *pszMessage) {
    uint8_t ret = -1;
    unsigned char *pop = NULL;
    unsigned char szBuffer[32]={0};

    // MCU到face_loop，0代表开锁成功，1代表开锁失败
    memcpy(szBuffer, pszMessage, nMessageLen);

    pop = szBuffer;

    // MCU到face_loop，0代表开锁成功，1代表开锁失败
    ret = StrGetUInt8(pszMessage);
    // 如果开锁成功 更新数据库状态 ,请求mqtt上传本次操作记录。
    if (ret == 0) {
        //					xshx add
        //char power_msg[32] = {0};
        pop += 1;
        uint8_t power = StrGetUInt8(pop);
        pop += 1;
        uint8_t  power2 = StrGetUInt8(pop);

        Record *record = (Record *) pvPortMalloc(sizeof(Record));
        HexToStr(username, g_uu_id.UID, sizeof(g_uu_id.UID));
        strcpy(record->UUID, username);
        //record->action = 3;//  操作类型：0代表注册 1: 一键开锁 2：钥匙开锁  3 人脸识别开锁
        char image_path[16];
        //record->status = 0; // 0,操作成功 1,操作失败.
        record->time_stamp = ws_systime; //时间戳 从1970年开始的秒数
        record->power = power * 256 + power2;
        //sprintf(power_msg, "{\\\"batteryA\\\":%d\\,\\\"batteryB\\\":%d}", record->power, record->power2);
        //LOGD("power_msg is %s \r\n", power_msg);

        //record->upload = 0; //   0代表没上传 1代表记录上传图片未上传 2代表均已
        record->action_upload = 0xB00;
        memset(image_path, 0, sizeof(image_path)); // 对注册成功的用户保存一张压缩过的jpeg图片
        //snprintf(image_path, sizeof(image_path), "REC_%d_%d_%s.jpg", 0, record->time_stamp, record->UUID);
        //snprintf(image_path, sizeof(image_path), "C%d", record->time_stamp);
        //memcpy(record->image_path, image_path, sizeof(image_path));//image_path

        DBManager::getInstance()->addRecord(record);

        //Oasis_SetOasisFileName(record->image_path);
		//Oasis_WriteJpeg();

        int ID = DBManager::getInstance()->getLastRecordID();
        LOGD("开锁成功, 更新数据库状态.请求MQTT上传本次开门的记录 \n");
        cmdRequestMqttUpload(ID);
    } else {
        LOGD("开锁失败,不更新数据库状态. 不上传记录\n");
    }

    return 0;
}


// 主控接收指令:远程wifi开门响应
int cmdWifiOpenDoorRsp(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("远程开门回复 \r\n");

    uint8_t ret = SUCCESS;

    ret = StrGetUInt8(pszMessage);

    if (1/*stSystemCfg.flag_wifi_enable*/) {
        // 转发响应给 Mqtt 模块
        cmdCommRsp2Mqtt(CMD_WIFI_OPEN_DOOR, ret);
        //usleep(200);
    }

    return 0;
}

int save_config_feature_file() {
    LOGD("save config, feature start\r\n");
    g_is_save_file = true;
    if (saving_file == false) {
        DB_Save(0);
        save_json_config_file();
        saving_file = true;
    }
    g_is_save_file = false;
    LOGD("save config, feature end\r\n");

    return 0;
}

int save_files_before_pwd() {
    LOGD("开始保存config, db, jpg 文件 \r\n");
    g_is_save_file = true;
    if (saving_file == false) {
        DB_Save(0);
        save_json_config_file();
        saving_file = true;
    }
// save record list  to file
    if (saving_db == false) {
        saving_db = true;
        DBManager::getInstance()->flushRecordList();
    }
    Oasis_WriteJpeg();
    g_is_save_file = false;
    LOGD("保存config, db, jpg 文件结束 \r\n");

    return 0;
}

//主控发送: 关机请求
int cmdCloseFaceBoardReq() {
    save_files_before_pwd();
    LOGD("发送关机请求 \r\n");
    char szBuffer[32] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /*填充消息体*/

    /*填充消息头*/
    iBufferSize = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_CLOSE_FACEBOARD,
            MsgLen);

    /*计算FCS*/
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));

    SendMsgToMCU((uint8_t *) szBuffer, iBufferSize + CRC16_LEN);
    g_is_shutdown = true;
    vTaskDelay(pdMS_TO_TICKS(100));


    return 0;
}

//主控返回响应指令: 手机APP请求注册激活响应
int cmdReqActiveByPhoneRsp(uint8_t ret) {
    char szBuffer[32] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /*填充消息体*/
    StrSetUInt8((uint8_t *) pop, ret);
    MsgLen += sizeof(uint8_t);
    pop += sizeof(uint8_t);

    /*填充消息头*/
    iBufferSize = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_REG_ACTIVE_BY_PHONE,
            MsgLen);

    /*计算FCS*/
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));

    SendMsgToMCU((uint8_t *) szBuffer, iBufferSize + CRC16_LEN);

    return 0;
}

//主控接收指令: 手机APP请求注册激活请求
int cmdReqActiveByPhoneProc(unsigned char nMessageLen, const unsigned char *pszMessage) {
    uint8_t ret = SUCCESS;

    if (REG_STATUS_OK == g_reging_flg) {
        //返回响应
        cmdReqActiveByPhoneRsp(ret);
        vTaskDelay(pdMS_TO_TICKS(20));

        /*注册激活后关机*/
        cmdCloseFaceBoardReq();
    } else {//借助APP激活按钮无注册时触发，显示IR图像
        vizn_api_status_t status;
        status = VIZN_SetDispMode(NULL, DISPLAY_MODE_IR);
        if (kStatus_API_Layer_Success != status) {
            ret = FAILED;
        }
    }

    return 0;
}

//主控返回响应指令: 删除用户响应
int cmdUserDeleteRsp(uint8_t result) {
    char szBuffer[32] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /*填充消息体*/
    StrSetUInt8((uint8_t *) pop, result);
    MsgLen += sizeof(uint8_t);
    pop += sizeof(uint8_t);

    /*填充消息头*/
    iBufferSize = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_FACE_DELETE_USER,
            MsgLen);

    /*计算FCS*/
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));

    SendMsgToMCU((uint8_t *) szBuffer, iBufferSize + CRC16_LEN);

    return 0;
}

static void uart5_startUITimeUpdateTimer() {
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
void SysTimeSet(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
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
int cmdSetSysTimeSynRsp(uint8_t ret) {
    LOGD("时间同步回复 \r\n");
    char szBuffer[128] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /*填充消息体*/
    StrSetUInt8((uint8_t *) pop, ret);
    MsgLen += sizeof(uint8_t);
    pop += sizeof(uint8_t);

    /*填充消息头*/
    iBufferSize = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_NTP_SYS_TIME,
            MsgLen);

    /*计算FCS*/
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));

    SendMsgToMCU((uint8_t *) szBuffer, iBufferSize + CRC16_LEN);

    return 0;
}

//主控接收指令:  时间同步请求
int cmdSetSysTimeSynProc(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("时间同步请求 \r\n");
    uint8_t year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0, i = 0;
    uint8_t ret = SUCCESS;
    if (nMessageLen == 6) {
        year = StrGetUInt8(pszMessage + i);
        i++;
        month = StrGetUInt8(pszMessage + i);
        i++;
        day = StrGetUInt8(pszMessage + i);
        i++;
        hour = StrGetUInt8(pszMessage + i);
        i++;
        min = StrGetUInt8(pszMessage + i);
        i++;
        sec = StrGetUInt8(pszMessage + i);
        i++;
    } else {
        ret = FAILED;
    }
    LOGD("setTime:%04d-%02d-%02d %02d:%02d:%02d \r\n", 2000 + year, month, day, hour, min, sec);

    if ((month == 0 || month > 12) || (day == 0 || day > 31) || hour > 24 || min > 60 || sec > 60) {
        ret = FAILED;
        LOGD("Set Time value error!\n");
    } else {
        /*系统时间设置*/
        SysTimeSet(year, month, day, hour, min, sec);
    }

    bSysTimeSynProc = true;

    cmdSetSysTimeSynRsp(ret);

    return 0;
}

// 主控接收指令: 删除用户请求
int cmdDeleteUserReqProcByHead(unsigned char nHead, unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD(" 删除用户请求 \r\n");
    uint8_t ret = FAILED;
    char szBuffer[32] = {0};
    uUID uu_id;

    // 解析指令
    if ((nMessageLen < sizeof(szBuffer)) && nMessageLen == UUID_LEN) {
        memcpy(&uu_id, pszMessage, nMessageLen);
    }

    LOGD("delete uuid : <0x%08x>, <0x%08x>.\r\n", uu_id.tUID.H, uu_id.tUID.L);
    if (uu_id.tUID.H == 0xFFFFFFFF && uu_id.tUID.L == 0xFFFFFFFF) {
        // 清空所有用户 和操作记录
        char strUUID[20] = {0};
        HexToStr(strUUID, uu_id.UID, UUID_LEN);
        // 清空用户
        //ret =DBManager::getInstance()->clearUser();
        // 清空操作记录
        ret = DBManager::getInstance()->clearRecord();

        vizn_api_status_t status;
        status = VIZN_DelUser(NULL);
        if (kStatus_API_Layer_Success == status) {
            ret = SUCCESS;
        } else {
            ret = FAILED;
        }
        DB_Save(0);
    } else {
        // 删除单个用户 和其操作记录
        LOGD("delete single user start");
        //删除单个用户
        char strUUID[20] = {0};
        HexToStr(strUUID, uu_id.UID, UUID_LEN);
        //ret = DBManager::getInstance()->deleteUserByUUID( strUUID );
        //删除用户的操作记录
        ret = DBManager::getInstance()->deleteRecordByUUID(strUUID);

        memset(username, 0, sizeof(username));
        HexToStr(username, uu_id.UID, sizeof(uu_id.UID));
        username[16] = '\0';//NXP的人脸注册API的username最大只能16byte
        LOGD("=====UUID<len:%d>:%s.\r\n", sizeof(username), username);

        vizn_api_status_t status;
        status = VIZN_DelUser(NULL, username);
        if (kStatus_API_Layer_Success == status) {
            ret = SUCCESS;
        } else {
            ret = FAILED;
        }
        DB_Save(0);
    }

    LOGD("delete uuid : <0x%08x>, <0x%08x> result %d nHead 0x%2x.\r\n", uu_id.tUID.H, uu_id.tUID.L, ret, nHead);
    if (nHead == HEAD_MARK_MQTT) {
        cmdCommRsp2MqttByHead(HEAD_MARK_MQTT, CMD_FACE_DELETE_USER, ret);
    } else {
        cmdCommRsp2MCU(CMD_FACE_DELETE_USER, ret);
    }

    bDeleteUser = true;

    return 0;
}

//串口接收消息处理
// 主控接收指令: WIFI SSID 设置请求
int cmdWifiSSIDProc(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("接收指令: WIFI SSID 设置 \r\n");

    uint8_t ret = FAILED;
    char wifi_ssid[WIFI_SSID_LEN_MAX + 1] = {0};

    CloseLcdBackground();
    // 解析指令
    if ((nMessageLen < sizeof(wifi_ssid)) && nMessageLen < WIFI_SSID_LEN_MAX) {
        memset(wifi_ssid, 0, sizeof(wifi_ssid));
        memcpy(wifi_ssid, pszMessage, nMessageLen);

        // 保存设置到系统配置文件
        LOGD("wifi ssid : <%s>.\r\n", wifi_ssid);
        update_wifi_ssid(wifi_ssid);
		update_need_reconnect("true");
        //read_config("./config.ini");

        ret = SUCCESS;
    }

    cmdCommRsp2MCU(CMD_WIFI_SSID, ret);
    return 0;
}

// 主控接收指令: WIFI 密码 设置请求
int cmdWifiPwdProc(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("接收指令: WIFI 密码 设置 \r\n");

    uint8_t ret = FAILED;
    char wifi_pwd[WIFI_PWD_LEN_MAX + 1] = {0};

    CloseLcdBackground();
    // 解析指令
    if ((nMessageLen < sizeof(wifi_pwd)) && nMessageLen < WIFI_SSID_LEN_MAX) {
        memset(wifi_pwd, 0, sizeof(wifi_pwd));
        memcpy(wifi_pwd, pszMessage, nMessageLen);

        // 保存设置到系统配置文件
        LOGD("wifi pwd : <%s>.\r\n", wifi_pwd);
        update_wifi_pwd(wifi_pwd);
		update_need_reconnect("true");
        //read_config("./config.ini");
        ret = SUCCESS;
    }

    cmdCommRsp2MCU(CMD_WIFI_PWD, ret);
    return 0;
}

// 主控接收指令: 设置MQTT 参数
int cmdMqttParamSetProc(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("设置MQTT 参数  \r\n");
    uint8_t ret = FAILED;
    uint8_t mqtt_user[MQTT_USER_LEN + 1] = {0};
    uint8_t mqtt_pwd[MQTT_PWD_LEN + 1] = {0};
    char mqtt_user_tmp[32] = {0};
    char mqtt_pwd_tmp[32] = {0};

    // 解析指令
    if (nMessageLen == MQTT_USER_LEN + MQTT_PWD_LEN) {
        memset(mqtt_user, 0, sizeof(mqtt_user));
        memcpy(mqtt_user, pszMessage, MQTT_USER_LEN);
        memset(mqtt_pwd, 0, sizeof(mqtt_pwd));
        memcpy(mqtt_pwd, pszMessage + MQTT_USER_LEN, MQTT_PWD_LEN);
        LOGD("mqtt_user<0x%02x%02x%02x%02x%02x%02x>,mqtt_pwd<0x%02x%02x%02x%02x%02x%02x%02x%02x>!\r\n", \
        mqtt_user[0], mqtt_user[1], mqtt_user[2], mqtt_user[3], mqtt_user[4], mqtt_user[5], \
                mqtt_pwd[0], mqtt_pwd[1], mqtt_pwd[2], mqtt_pwd[3], mqtt_pwd[4], mqtt_pwd[5], mqtt_pwd[6], mqtt_pwd[7]);

        // 保存设置到系统配置文件
        memset(mqtt_pwd_tmp, 0, sizeof(mqtt_user_tmp));
        HexToStr(mqtt_user_tmp, mqtt_user, MQTT_USER_LEN);
        memset(mqtt_pwd_tmp, 0, sizeof(mqtt_pwd_tmp));
//        HexToStr(mqtt_pwd_tmp, mqtt_pwd, MQTT_PWD_LEN);
        update_mqtt_opt(mqtt_user_tmp,  (char*)mqtt_pwd);
        //read_config("./config.ini");
        LOGD("mqtt user :<%s>, mqtt_pwd:<%s>.\r\n", mqtt_user_tmp, mqtt_pwd_tmp);

        //system("sync");
        ret = SUCCESS;
    }

    // 重启MQTT 以新的参数启动
    //system("killall mqtt");
    //system("/opt/smartlocker/mqtt &");

    cmdCommRsp2MCU(CMD_WIFI_MQTT, ret);
    return 0;
}

// 主控接收指令: 蓝牙模块状态信息上报
int cmdBTInfoRptProc(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("主控接收指令: 蓝牙模块状态信息上报 \r\n");
    uint8_t ret = FAILED;
    uint8_t bt_ver = 0;
    uint8_t bt_mac[6] = {0};
    char bt_verbuf[4] = {0};
    char bt_ver_tmp[32] = {0};
    char bt_mac_tmp[16] = {0};

    //解析指令
    if (nMessageLen == 1 + 6) {
        bt_ver = StrGetUInt8(pszMessage);
        memset(bt_mac, 0, sizeof(bt_mac));
        memcpy(bt_mac, pszMessage + 1, 6);

        // 获取字符串格式的版本号
        memset(bt_verbuf, 0, sizeof(bt_verbuf));
        memset(bt_ver_tmp, 0, sizeof(bt_ver_tmp));
        sprintf(bt_verbuf, "%03d", bt_ver);
        sprintf(bt_ver_tmp, "W8_52832_V%c.%c.%c", bt_verbuf[0], bt_verbuf[1], bt_verbuf[2]);

        // 转换为字符格式的BT mac
        memset(bt_mac_tmp, 0, sizeof(bt_mac_tmp));
        sprintf(bt_mac_tmp, "%02X%02X%02X%02X%02X%02X", \
            bt_mac[0], bt_mac[1], bt_mac[2], bt_mac[3], bt_mac[4], bt_mac[5]);

        // 保存设置到系统配置文件
        update_bt_info(bt_ver_tmp, bt_mac_tmp);
        //read_config("./config.ini");
        LOGD("bt_ver :<%s>, bt_mac:<%s>.\r\n", bt_ver_tmp, bt_mac_tmp);

        //system("sync");
        ret = SUCCESS;
    }

    //cmdCommRsp2MCU(CMD_BT_INFO, ret);
    return 0;
}

// 主控接收指令: 设置wifi的MQTT server 登录URL(可能是IP+PORT, 可能是域名+PORT)
int cmdMqttSvrURLProc(unsigned char nMessageLen, const unsigned char *pszMessage) {
    uint8_t ret = FAILED;
    char MqttSvr_Url[MQTT_SVR_URL_LEN_MAX + 1] = {0};

    CloseLcdBackground();
    //system("killall face_recgnize");
    // 解析指令
    if ((nMessageLen < sizeof(MqttSvr_Url)) && nMessageLen < MQTT_SVR_URL_LEN_MAX) {
        memset(MqttSvr_Url, 0, sizeof(MqttSvr_Url));
        memcpy(MqttSvr_Url, pszMessage, nMessageLen);

        // 保存设置到系统配置文件
        LOGD("MqttSvr_Url : <%s>.\r\n", MqttSvr_Url);
        update_MqttSvr_opt(MqttSvr_Url);
        //read_config("./config.ini");

        ret = SUCCESS;
    }
    cmdCommRsp2MCU(CMD_WIFI_MQTTSER_URL, ret);
    return 0;
}

//106F->MQTT: 通用响应
int cmdCommRsp2Mqtt(unsigned char CmdId, uint8_t ret) {
    int result = 0;
    result = cmdCommRsp2MqttByHead(HEAD_MARK, CmdId, ret);
    return result;
}

int cmdCommRsp2MqttByHead(unsigned char nHead, unsigned char CmdId, uint8_t ret) {
    char szBuffer[32] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /* 填充消息体 */
    StrSetUInt8((uint8_t *) pop, ret);
    MsgLen += sizeof(uint8_t);
    pop += sizeof(uint8_t);

    /* 填充消息头 */
    iBufferSize = MsgHead_Packet(
            szBuffer,
            nHead,
            CmdId,
            MsgLen);

    /* 计算FCS */
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));

    LOGD("%s send to 0x%02x cmd 0x%02x result %d\r\n", __FUNCTION__, nHead, CmdId, ret);
    //���Ϳ�����Ӧ��wifi��ص�mqttģ��
    SendMsgToMQTT(szBuffer, iBufferSize + CRC16_LEN);

    return 0;
}

// 主控接收指令: wifi 网络时间同步响应
int cmdWifiTimeSyncRsp(unsigned char nMessageLen, const unsigned char *pszMessage)
{
    LOGD("网络时间同步响应 \r\n");
	uint8_t ret = SUCCESS;	

	ret = StrGetUInt8( pszMessage );

	{
		cmdCommRsp2Mqtt(CMD_WIFI_TIME_SYNC, ret);
	}
	
	return 0;
}

//MCU->106F->MQTT:  WIFI 同步订单时间响应
int cmdWifiOrderTimeSyncRsp(unsigned char nMessageLen, const unsigned char *pszMessage)
{
    uint8_t ret = StrGetUInt8( pszMessage );
    LOGD("远程同步订单请求  %d \r\n", (uint8_t )nMessageLen);

	cmdCommRsp2Mqtt(CMD_ORDER_TIME_SYNC, ret);
	return 0;
}
int ProcMessage(
        unsigned char nCommand,
        unsigned char nMessageLen,
        const unsigned char *pszMessage) {
    LOGD("======Command[0x%X], nMessageLen<%d>\r\n", nCommand, nMessageLen);
    switch (nCommand) {
        case CMD_INITOK_SYNC: {
            cmdSysInitOKSyncRsp(nMessageLen, pszMessage);
            break;
        }
        case CMD_OPEN_DOOR: {
            cmdOpenDoorRsp(nMessageLen, pszMessage);
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
        case CMD_CLOSE_FACEBOARD:{
            cmdPowerDownRsp(nMessageLen, pszMessage);
            break;
        }
        case CMD_FACE_REG: {
            cmdUserRegReqProc(nMessageLen, pszMessage);
            break;
        }
        case CMD_FACE_REG_RLT: {
            break;
        }
        case CMD_NTP_SYS_TIME: {
            cmdSetSysTimeSynProc(nMessageLen, pszMessage);
            break;
        }
        case CMD_FACE_DELETE_USER: {
            cmdDeleteUserReqProcByHead(/*HEAD_MARK*/HEAD_MARK_MQTT, nMessageLen, pszMessage);
            break;
        }
        case CMD_REQ_RESUME_FACTORY: {
            cmdReqResumeFactoryProc(nMessageLen, pszMessage);
            break;
        }
        case CMD_REG_ACTIVE_BY_PHONE: {
            cmdReqActiveByPhoneProc(nMessageLen, pszMessage);
            break;
        }
        case CMD_WIFI_SSID: {
            cmdWifiSSIDProc(nMessageLen, pszMessage);
            break;
        }
        case CMD_WIFI_PWD: {
            cmdWifiPwdProc(nMessageLen, pszMessage);
            break;
        }
        case CMD_WIFI_MQTT: {
            cmdMqttParamSetProc(nMessageLen, pszMessage);
            break;
        }
		case CMD_WIFI_TIME_SYNC:	
		{
			cmdWifiTimeSyncRsp(nMessageLen, pszMessage);
			break;
		}	
		case CMD_ORDER_TIME_SYNC:
		{
			cmdWifiOrderTimeSyncRsp(nMessageLen, pszMessage);
			break;
		}
        case CMD_WIFI_OPEN_DOOR: {
            cmdWifiOpenDoorRsp(nMessageLen, pszMessage);
            break;
        }

        case CMD_BT_INFO: {
            cmdBTInfoRptProc(nMessageLen, pszMessage);
            break;
        }
        case CMD_WIFI_MQTTSER_URL: {
            cmdMqttSvrURLProc(nMessageLen, pszMessage);
            break;
        }
        case CMD_MECHANICAL_LOCK: {
        	cmdMechicalLockRsp(nMessageLen, pszMessage);
        	break;
        }
        default:
            LOGD("No effective cmd from MCU!\n");
            break;
    }

    return 0;
}

// 主控发送指令:请求MQTT 上传记录
/*status:
 * Op: 注册、开门
 * */
int cmdRequestMqttUpload(int id) {
    LOGD("%s\r\n", __func__);
    char szBuffer[32] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /* 填充消息体 */
    memcpy(pop, &id, 4);
    MsgLen += 4;
    pop += MsgLen;
    /* 填充消息头 */
    iBufferSize = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_MQTT_UPLOAD,
            MsgLen);

    /* 计算 FCS */
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));

    int count = 0;
    while (1) {
        // 发送指令给 wifi 相关的mqtt 模块
        //usleep(100000);//0.1秒
        vTaskDelay(pdMS_TO_TICKS(100));
        if (count++ > 150) {//如果超过15秒 还没有连接上则退出
            LOGD("WIFI 未连接  \r\n");
            break;
        }
        //判断MQTT是否可用
        if (mqtt_init_done == 1) {// 1 代表MQTT连接成功
            LOGD("WIFI 连接成功,请求mqtt发送msg  \r\n");
            SendMsgToMQTT(szBuffer, iBufferSize + CRC16_LEN);
            break;
        }
    }

    return 0;
}

int Uart5_SendDeinitCameraMsg(void)
{
    QMsg *pQMsgCamera;
    /* Camera */
    pQMsgCamera             = (QMsg *)pvPortMalloc(sizeof(QMsg));
    if (NULL == pQMsgCamera) {
        LOGE("[ERROR]: pQMsg pvPortMalloc failed\r\n");
        return -1;
    }
    pQMsgCamera->id         = QMSG_CMD;
    pQMsgCamera->msg.cmd.id = QCMD_DEINIT_CAMERA;
    int status =  Camera_SendQMsg((void *)&pQMsgCamera);

    if (status) {
        vPortFree(pQMsgCamera);
    }
    return 0;
}

static void uart5_QMsg_task(void *pvParameters) {
    BaseType_t ret;
    QMsg *pQMsg;
    int recognize_times = 0;
    char image_path[16] = {0};

    while (1) {
        //LOGD("uart5_QMsg_task()  -> xQueueReceive()!\n");
        // pick up message
        ret = xQueueReceive(Uart5MsgQ, (void *) &pQMsg, portMAX_DELAY);
        if (ret == pdTRUE) {
            //LOGD("%s pQMsg->id is %d!\r\n", __FUNCTION__, pQMsg->id);
            switch (pQMsg->id) {
                case QMSG_FACEREC_ADDNEWFACE: {//处理人脸注册结果
                    //LOGD("处理人脸注册结果 %d\r\n", pQMsg->msg.val);
                    if (pQMsg->msg.val) {//success
                        if (gFaceInfo.enrolment && (OASIS_REG_RESULT_DUP == gFaceInfo.rt)) {//判断是否是重复注册
                            LOGD("User face duplicate register!\r\n");
                            g_reging_flg = REG_STATUS_DUP;
                        } else {
                            LOGD("User face register success!\r\n");
                            g_reging_flg = REG_STATUS_OK;
                        }

                        g_reg_start = Time_Now(); //记录注册成功响应发出的时间，并开始计时
//						增加注册记录
                        //增加本次操作记录
                        LOGD("增加注册记录");
                        Record *record = (Record *) pvPortMalloc(sizeof(Record));
                        memset(record, 0, sizeof(Record));
                        strcpy(record->UUID, username);
                        //record->action = 0;// 0 代表注册
                        record->time_stamp = ws_systime;//当前时间
                        memset(image_path, 0, sizeof(image_path)); // 对注册成功的用户保存一张压缩过的jpeg图片
                        //snprintf(image_path, sizeof(image_path), "REG_%d_%d_%s.jpg", g_reging_flg, record->time_stamp,
                        //         record->UUID);
                        snprintf(image_path, sizeof(image_path), "G%d", record->time_stamp);
                        memcpy(record->image_path, image_path, sizeof(image_path));//image_path
                        //record->status = SUCCESS;// 本次注册成功
                        record->power = 0xFFFF;
                        //record->power1 = -1;//当前电池电量
                        //record->power2 = -1;//当前电池电量
                        //record->upload = 0;// 0代表没上传 1代表记录上传图片未上传 2代表均已
                        record->action_upload = 0;	//代表注册且未上传
                        LOGD("往数据库中插入本次注册记录 \n");
                        DBManager::getInstance()->addRecord(record);

                        Oasis_SetOasisFileName(record->image_path);
                        //Oasis_WriteJpeg();
//                        g_face.WriteJPG(image_path, faceBuf.color_buf, CAM_HEIGHT,CAM_WIDTH, 3, 50);
//                        log_info("保存 UID<%s> 注册图片到 path<%s>!\n", record.UID, image_path);
                    } else {//failed
                        LOGD("User face register failed!\r\n");
                        g_reging_flg = REG_STATUS_FAILED;
                    }
                    //StrToHex(g_uu_id.UID,(char*)gFaceInfo.name.c_str(),sizeof(g_uu_id.UID));
                    StrToHex(g_uu_id.UID, (char *) username, sizeof(g_uu_id.UID));

                    cmdRegResultNotifyReq(g_uu_id, g_reging_flg);
                    if (g_reging_flg == REG_STATUS_FAILED) {
                        CloseLcdBackground();
                        vTaskDelay(pdMS_TO_TICKS(1000));
                        Uart5_SendDeinitCameraMsg();
                        vTaskDelay(pdMS_TO_TICKS(200));
                        cmdCloseFaceBoardReq();
                    } else {
                        //shut_down = true;
                        CloseLcdBackground();
                        vTaskDelay(pdMS_TO_TICKS(1000));
                        Uart5_SendDeinitCameraMsg();
                        vTaskDelay(pdMS_TO_TICKS(200));

                        save_config_feature_file();
                        vTaskDelay(pdMS_TO_TICKS(100));

                    	LOGD("注册成功,请求MQTT上传本次用户注册记录 \n");
						int ID = DBManager::getInstance()->getLastRecordID();
						cmdRequestMqttUpload( ID );
                    }
                }
                    break;

                case QMSG_FACEREC_RECFACE: {//处理人脸识别结果
                    //LOGD("%s g_reging_flg is %d lcd_back_ground is %d\r\n", __FUNCTION__, g_reging_flg, lcd_back_ground);
                    //LOGD("处理人脸识别结果 %d flg %d\r\n", pQMsg->msg.val, g_reging_flg);
                    if ( (boot_mode != BOOT_MODE_NORMAL) || (REG_STATUS_WAIT != g_reging_flg)
                        || (lcd_back_ground == false))//如果正在注册流程，就过滤掉该识别结果
                    {
                        vPortFree(pQMsg);
                        continue;
                    }

                    if (pQMsg->msg.val) {//success
                        LOGD("User face recognize success!\r\n");
                        CloseLcdBackground();
                        vTaskDelay(pdMS_TO_TICKS(1000));
						Uart5_SendDeinitCameraMsg();
                        //LOGD("gFaceInfo.name is %s!\n", gFaceInfo.name);
                        LOGD("pQMsg->msg.info.name is %s!\r\n", pQMsg->msg.info.name);
                        char name[64];
                        //memcpy(name, gFaceInfo.name.c_str(), gFaceInfo.name.size());
                        memcpy(name, pQMsg->msg.info.name, 64);
                        StrToHex(g_uu_id.UID, name, sizeof(g_uu_id.UID));

                        cmdOpenDoorReq(g_uu_id);
                    } else {//failed
                        recognize_times++;
                        LOGD("User face recognize failed %d times\r\n", recognize_times);
                        if (recognize_times > 100) {
                            recognize_times = 0;
                            CloseLcdBackground();
                            vTaskDelay(pdMS_TO_TICKS(1000));
							Uart5_SendDeinitCameraMsg();
                            cmdCloseFaceBoardReq();//关主控电源
                            break;
                        }

                    }

                }
                    break;
                default:
                    assert(0);
                    break;
            }

            vPortFree(pQMsg);
        } else {
            //message receive error
            assert(0);
        }


    }

}

static void uart5_sync_task(void *pvParameters) {
    //vTaskDelay(pdMS_TO_TICKS(200));
    //check_config();
    vTaskDelay(pdMS_TO_TICKS(300));
    cmdSysInitOKSyncReq(SYS_VERSION);
    vTaskDelete(NULL);
}


enum{
	UART5_RX_MSG_STATUS_WAITING_HEADER,
	UART5_RX_MSG_STATUS_WAITING_LENGTH,
	UART5_RX_MSG_STATUS_WAITING_DATA,

};
#define UART5_RX_MAX_DATA_PACKAGE_SIZE 32	//20

static void uart5_task(void *pvParameters) {
    int error;
    size_t rcvlen = 0;
    int msglen = 0;
    uint16_t Msg_CRC16 = 0, Cal_CRC16 = 0;
    const unsigned char *pszMsgInfo = NULL;
    unsigned char HeadMark;
    unsigned char CmdId = 0;
    unsigned char datalen = 0;
    uint8_t recv_buffer[64] = {0};
    char message_buffer[64] = {0};

//    uint8_t data_tmp;
    LOGD("[uart5_task]:starting...\r\n");
    lpuart_config5.srcclk = DEMO_LPUART_CLK_FREQ;
    lpuart_config5.base = DEMO_LPUART;

    if (kStatus_Success != LPUART_RTOS_Init(&handle5, &t_handle5, &lpuart_config5)) {
        LOGD("[uart5_task]:Error,LPUART_RTOS_Init failed!\r\n");
        vTaskSuspend(NULL);
    }

    /* Send introduction message. */
    //tell mcu that mainboard has Initialized OK.
    //cmdSysInitOKSyncReq(SYS_VERSION);

    /* Receive user input and send it back to terminal. */
    do {

        //LOGD("[uart5_task]:Recv New Msg ...\r\n");
        memset(recv_buffer, 0, sizeof(recv_buffer));
        uint8_t rx_status = UART5_RX_MSG_STATUS_WAITING_HEADER;
        msglen = 0;
        uint8_t req_len = 0;

        while (1) {
        	switch(rx_status)
        	{
        	case UART5_RX_MSG_STATUS_WAITING_HEADER:
        		req_len = 1;
                error = LPUART_RTOS_Receive(&handle5, &recv_buffer[0], req_len, &rcvlen);
                break;
        	case UART5_RX_MSG_STATUS_WAITING_LENGTH:
        		req_len = 2;
        		error = LPUART_RTOS_Receive(&handle5, &recv_buffer[1], req_len, &rcvlen);
        		break;
        	case UART5_RX_MSG_STATUS_WAITING_DATA:
        		req_len = recv_buffer[2] + 2;
        		error = LPUART_RTOS_Receive(&handle5, &recv_buffer[3], req_len, &rcvlen);
        		break;
        	default:
        		assert(0);
        	}

        	if (error == kStatus_Success)
        	{
        		//LOGD("[uart5_task]:receive return!\r\n");
        		if (req_len != rcvlen)
        		{
        			LOGD("[uart5_task]:part of data received, expect:%d received:%d!\r\n",req_len,rcvlen);
					rx_status = UART5_RX_MSG_STATUS_WAITING_HEADER;


        		}else if (UART5_RX_MSG_STATUS_WAITING_HEADER == rx_status)
        		{
        			if (HEAD_MARK != recv_buffer[0]) {
        				LOGD("[uart5_task]:wrong header received, expect:0x%x received:0x%x!\r\n",HEAD_MARK,recv_buffer[0]);
						rx_status = UART5_RX_MSG_STATUS_WAITING_HEADER;
        			}else
        			{
        				rx_status = UART5_RX_MSG_STATUS_WAITING_LENGTH;
        			}
        		}else if (UART5_RX_MSG_STATUS_WAITING_LENGTH == rx_status)
        		{
        			if (recv_buffer[2] > UART5_RX_MAX_DATA_PACKAGE_SIZE)
        			{
        				LOGD("[uart5_task]:wrong msg length received, length:%d\r\n",recv_buffer[2]);
						rx_status = UART5_RX_MSG_STATUS_WAITING_HEADER;
        			}else
        			{
        				rx_status = UART5_RX_MSG_STATUS_WAITING_DATA;
        			}
        		}else
        		{
        			//a whole package received
        			msglen = rcvlen + 3;
        			break;
        		}

        	}else
        	{
        		LOGD("[uart5_task]:LPUART_RTOS_Receive error:%d sts:%d!\r\n",error,rx_status);
        		rx_status = UART5_RX_MSG_STATUS_WAITING_HEADER;
        		continue;
        	}
        }

        if (msglen > 0) {
            //LOGD("\n===rcv msg<len:%d>:", msglen);

            memset(message_buffer, 0, sizeof(message_buffer));
            HexToStr(message_buffer, recv_buffer, msglen);
            LOGD("\n===receive msg<len:%d %s>: \r\n", msglen, message_buffer);
            /*for(i=0; i<msglen; i++)
            {
                LOGD("0x%02x	", recv_buffer[i]);
            }
            LOGD("\n");*/
        }
        //收到完整的以HEAD_MARK开头的消息后校验处理
        memcpy(&Msg_CRC16, recv_buffer + msglen - CRC16_LEN, CRC16_LEN);
        Cal_CRC16 = CRC16_X25(recv_buffer, msglen - CRC16_LEN);
        if (Msg_CRC16 != Cal_CRC16) {
            LOGD("msg CRC error: Msg_CRC16<0x%02X>, Cal_CRC16<0x%02X>!\r\n", Msg_CRC16, Cal_CRC16);
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

    } while (1);// (kStatus_Success == error);
    LPUART_RTOS_Deinit(&handle5);
    vTaskSuspend(NULL);
}

#if    0
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
#endif

int MCU_UART5_Start() {
    LOGD("[MCU_UART5_Start]:starting...\r\n");

    NVIC_SetPriority(LPUART5_IRQn, 5);

    //创建uart5串口数据通信task
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(uart5_task, "Uart5_task", UART5TASK_STACKSIZE, NULL, UART5TASK_PRIORITY,
                                        Uart5TaskStack, &s_Uart5TaskTCB))
#else
    if (xTaskCreate(uart5_task, "Uart5_task", UART5TASK_STACKSIZE, NULL, UART5TASK_PRIORITY, NULL) != pdPASS)
#endif
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }

    //创建uart5 开机同步task
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(uart5_sync_task, "Uart5_sync_task", UART5SYNCTASK_STACKSIZE, NULL, UART5SYNCTASK_PRIORITY,
                                        Uart5SyncTaskStack, &s_Uart5SyncTaskTCB))
#else
    if (xTaskCreate(uart5_sync_task, "Uart5_sync_task", UART5SYNCTASK_STACKSIZE, NULL, UART5SYNCTASK_PRIORITY, NULL) !=
        pdPASS)
#endif
    {
        PRINTF("Task uart5_sync_task creation failed!.\r\n");
        while (1);
    }
    //创建UART5 Task的消息接收队列QMSG
    Uart5MsgQ = xQueueCreate(UART5_MSG_Q_COUNT, sizeof(QMsg *));
    if (Uart5MsgQ == NULL) {
        LOGE("[ERROR]:xQueueCreate uart5 queue\r\n");
        return -1;
    }

    //创建uart5的QMsg接收task
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(uart5_QMsg_task, "Uart5_QMsg_task", UART5QMSGTASK_STACKSIZE, NULL, UART5QMSGTASK_PRIORITY,
                                        Uart5QmsgTaskStack, &s_Uart5QmsgTaskTCB))
#else
    if (xTaskCreate(uart5_QMsg_task, "Uart5_QMsg_task", UART5QMSGTASK_STACKSIZE, NULL, UART5QMSGTASK_PRIORITY, NULL) !=
        pdPASS)
#endif
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }

    //创建循环定时处理的task
#if    0
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(uart5_Loop_task, "Uart5_Loop_task", UART5LOOPTASK_STACKSIZE, NULL, UART5LOOPTASK_PRIORITY,
                                        Uart5LoopTaskStack, &s_Uart5LoopTaskTCB))
#else
    if (xTaskCreate(uart5_Loop_task, "Uart5_Loop_task", UART5LOOPTASK_STACKSIZE, NULL, UART5LOOPTASK_PRIORITY, NULL) != pdPASS)
#endif
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }
#endif
    //LOGD("[MCU_UART5_Start]:started...\r\n");

    return 0;
}

//send qMsg to uart5 task when face register over		
int Uart5_GetFaceRegResult(uint8_t result, char *pszMessage) {
    int status;
    QMsg *pQMsg = (QMsg *) pvPortMalloc(sizeof(QMsg));
    if (NULL == pQMsg) {
        LOGE("[ERROR]: pQMsg pvPortMalloc failed\r\n");
        return -1;
    }
    pQMsg->id = QMSG_FACEREC_ADDNEWFACE;
    pQMsg->msg.val = result;
    memcpy(pQMsg->msg.info.name, pszMessage, sizeof(pQMsg->msg.info.name));
    status = Uart5_SendQMsg((void *) &pQMsg);

    if (status) {
        vPortFree(pQMsg);
    }

    return status;
}

//send qMsg to uart5 task when face recognize over			
int Uart5_GetFaceRecResult(uint8_t result, char *pszMessage) {
    int status;
    QMsg *pQMsg = (QMsg *) pvPortMalloc(sizeof(QMsg));
    if (NULL == pQMsg) {
        LOGE("[ERROR]: pQMsg pvPortMalloc failed\r\n");
        return -1;
    }
    pQMsg->id = QMSG_FACEREC_RECFACE;
    pQMsg->msg.val = result;
    memcpy(pQMsg->msg.info.name, pszMessage, sizeof(pQMsg->msg.info.name));
    status = Uart5_SendQMsg((void *) &pQMsg);

    if (status) {
        vPortFree(pQMsg);
    }

    return status;
}

static int Uart5_SendQMsg(void *msg) {
    BaseType_t ret;

    if (g_is_shutdown) {
        return -3;
    }

    if (Uart5MsgQ)
    {
        ret = xQueueSend(Uart5MsgQ, msg, (TickType_t) 0);
    }else
    {
    	LOGE("[ERROR]:Uart5MsgQ is NULL\r\n");
    	return -2;
    }

    if (ret != pdPASS) {
        LOGE("[ERROR]:Uart5_SendQMsg failed %d\r\n", ret);
        return -1;
    }

    return 0;
}

//从oasis_rt106f_elcok.cpp中获取识别或者注册时的face_info
void Set_curFaceInfo(char *name, bool recognize, bool enrolment, int rt) {
    LOGD(">>>name<%s>, recognize<%d>, enrolment<%d>, rt<%d>\r\n", name, recognize, enrolment, rt);
    gFaceInfo.name = std::string(name);
    gFaceInfo.recognize = recognize;
    gFaceInfo.enrolment = enrolment;
    gFaceInfo.rt = rt;
}


// 20201120 wszgx added for display correct date/time information in the screen
/*!
 * @brief Software timer callback.
 */
static void SwTimerCallback(TimerHandle_t xTimer) {
    ws_systime++;
}
// 20201120 wszgx end

void OpenLcdBackground() {
    if (!lcd_back_ground) {
        LOGD("[%s]:\r\n", __FUNCTION__);
        lcd_back_ground = true;
        //if(stInitSyncInfo.LightVal == 2) {
        //	OpenCameraPWM();
        //}
        PJDisp_TurnOnBacklight();
    }
}

void CloseLcdBackground() {
    if (lcd_back_ground) {
        LOGD("[%s]:\r\n", __FUNCTION__);
        lcd_back_ground = false;
        //if(stInitSyncInfo.LightVal == 2) {
        CloseCameraPWM();
        //}
        PJDisp_TurnOffBacklight();
    }
}
