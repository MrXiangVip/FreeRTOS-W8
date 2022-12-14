/**************************************************************************
 * 	FileName:	 	MCU_UART5_Layer.cpp
 *	Description:	This file is including the function interface which is used to processing
 					uart5 communication with mcu
 *	Copyright(C):	2018-2020 Wave Group Inc.
 *	Version:		V 1.0
 *	Author:			tanqw
 *	Created:		2020-10-26
 *	Updated:        xshx on 2022/5/20.
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
#include "../mqtt/mqtt-mcu.h"
#include "../mqtt/base64.h"
#include "wifi_main.h"
#include "database.h"
#include "DBManager.h"
#include "UserExtendManager.h"
#include "oasis.h"
#include "display.h"
#include "aw_wave_logo_v3.h"
#include "MCU_UART5_Layer.h"
#include "mqtt_mcu_mgr.h"

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

//ws_time_t ws_systime = 0;
static bool timer_started = false;
// 20201120 wszgx end

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_USER_NUM        50

/* Task priorities. */
#define uart5_task_PRIORITY (configMAX_PRIORITIES - 1)

#define SUPPORT_PRESSURE_TEST   0
//xshx add
#define WORK_MAX_COUNT          6

static const char *logtag ="[MCU_UART5_Layer] ";
int pressure_test = 1;
//计数性信号量
SemaphoreHandle_t  workCountSemaphore = xSemaphoreCreateCounting(WORK_MAX_COUNT,0);

lpuart_rtos_handle_t handle5;

bool bInitSyncInfo = false;
bool bSysTimeSynProc = false;


Reg_Status g_reging_flg = REG_STATUS_WAIT;//正在注册流程标记,等待被激活，对识别task发过来的识别结果进行过滤
//userInfo_data_t userInfoList[MAX_USER_NUM] = {0};

extern int battery_level;
static bool saving_file = false;
static bool saving_db = false;
bool g_is_save_file = false;
int boot_mode = BOOT_MODE_INVALID;  //0:短按;1：长按;2:蓝牙设置;3:蓝牙人脸注册;4:睡眠状态
int receive_boot_mode = 0;
bool oasis_task_start = false;


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void uart5_task(void *pvParameters);


/*******************************************************************************
 * Code
 *
 *______________________________________________________
|   |      |    |       |                |      |      |
|帧头|指令码 |方向|数据长度|内容             | 序列号|校验码|
|___|______|____|_______|________________|______|______|
*
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
//    StrSetUInt8( (uint8_t*)pTemp, DIRECT_SEND);
//    pTemp += sizeof(uint8_t);
    StrSetUInt8((uint8_t *) pTemp, MsgLen);
    pTemp += sizeof(uint8_t);

    return MsgLen + sizeof(MESSAGE_HEAD);
}
///****************************************************************************************
//函数名称：RspHead_Packet
//函数功能：组包
//入口参数：pszBuffer--数据
//		HeadMark--消息标记
//		CmdId--命令
//		MsgLen--消息长度
//出口参数：无
//返回值：成功返回消息的总长度(消息头+消息体+FCS长度)，失败返回-1
//****************************************************************************************/
//int RspHead_Packet(
//        char *pszBuffer,
//        unsigned char HeadMark,
//        unsigned char MsgLen) {
//    if (!pszBuffer) {
//        LOGD("pszBuffer is NULL\n");
//        return -1;
//    }
//    char *pTemp = pszBuffer;
//
//    StrSetUInt8((uint8_t *) pTemp, HeadMark);
//    pTemp += sizeof(uint8_t);
//    StrSetUInt8((uint8_t *) pTemp, 0xFE);
//    pTemp += sizeof(uint8_t);
////    StrSetUInt8( (uint8_t*)pTemp, DIRECT_SEND);
////    pTemp += sizeof(uint8_t);
//    StrSetUInt8((uint8_t *) pTemp, MsgLen);
//    pTemp += sizeof(uint8_t);
////    StrSetUInt8((uint8_t *) pTemp, CmdId);
////    pTemp += sizeof(uint8_t);
//
//    return MsgLen + sizeof(RESPONSE_HEAD);
//}
/*******************************************************************************
 *
 *函数功能: 消息尾拼接
 * 入口参数: Message   整包数据的起始地址
 *          iTailIndex   到尾部的下标计数
 * 出口参数: Message 整包数据的总长度
 *tailIndex = head+ data
 ******************************************************************************/
int MsgTail_Pack(char *Message, int iTailIndex ){
    char *pop = Message+iTailIndex;
#if 0
//  序列号
    uint8_t orderIndex =0;
    memcpy( pop, &orderIndex, sizeof(orderIndex));
    pop += sizeof(orderIndex);
//  校验码 =  不算帧头 和 检验位求和
    uint8_t checkSum = uGetCheckSum( (uint8_t*)Message+1, iTailIndex  );
    LOGD( "%s 校验码 %x \r\n",logtag, checkSum);
    memcpy( pop, &checkSum, sizeof(checkSum));
    return  iTailIndex+2;
#else
    uint16_t Msg_CRC16 = 0, Cal_CRC16 = 0;
    Cal_CRC16 = CRC16_X25((uint8_t *)Message, iTailIndex);
    memcpy(Message+iTailIndex, &Cal_CRC16, CRC16_LEN);
    LOGD("CRC 检验码 %x\r\n", Cal_CRC16);
    return  iTailIndex+CRC16_LEN;
#endif
};
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
//   跳过方向
//    pTemp += sizeof(uint8_t);

    pTemp += sizeof(uint8_t);
    *MsgLen = StrGetUInt8(pTemp);
    pTemp += sizeof(uint8_t);
    if (*HeadMark != HEAD_MARK && *HeadMark != HEAD_MARK_MQTT) {
        LOGD("byVersion[0x%x] != MESSAGE_VERSION[0x%x|0x%x]\n", \
            *HeadMark, HEAD_MARK, HEAD_MARK_MQTT);
//        return NULL;
    }

    if ((int) *MsgLen + sizeof(MESSAGE_HEAD) + CRC16_LEN > iBufferSize) {
        LOGD("pstMessageHead->MsgLen + sizeof(MESSAGE_HEAD) + CRC16_LEN > iBufferSize\r\n");
//        return NULL;
    }

    return pszBuffer + sizeof(MESSAGE_HEAD);
}

//106F->MCU:通用响应
int cmdCommRsp2MCU(unsigned char CmdId, uint8_t ret) {
    LOGD("通用响应 0x%02x\r\n", CmdId);
    char szBuffer[32] = {0};
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /* 填充消息体 */
//    StrSetUInt8((uint8_t *) pop, CmdId);
//    MsgLen += sizeof(uint8_t);
//    pop += sizeof(uint8_t);
    StrSetUInt8((uint8_t *) pop, ret);
    MsgLen += sizeof(uint8_t);
    pop += sizeof(uint8_t);

    /*填充消息头*/
    int iTailIndex = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CmdId,
            MsgLen);

    //  拼装消息尾
    int iTotalLen =MsgTail_Pack( szBuffer, iTailIndex);
    if( iTotalLen > sizeof(szBuffer) ){
        LOGD("%s ERROR  %d must <  %d \r\n",iTotalLen, sizeof(szBuffer) );
    }

    SendMsgToMCU((uint8_t *) szBuffer, iTotalLen);
    vTaskDelay(pdMS_TO_TICKS(1));

    return 0;
}



int SendMsgToMCU(unsigned char *MsgBuf, unsigned char MsgLen) {
    // 由于下电指令在存文件之后，而g_is_shutdown在存文件之前已经置为true，当前写文件不影响串口，所以注释掉下面几行
//    if (g_is_shutdown) {
//        return 0;
//    }
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
    int iTailIndex = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_INITOK_SYNC,
            MsgLen);


    int iTotalLen =MsgTail_Pack( szBuffer, iTailIndex);
    if( iTotalLen > sizeof(szBuffer) ){
        LOGD("%s ERROR  %d must <  %d \r\n",iTotalLen, sizeof(szBuffer) );
    }
    SendMsgToMCU((uint8_t *) szBuffer, iTotalLen);

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

//    SendMsgToMQTT(szBuffer, iBufferSize + CRC16_LEN);
    MqttMcuMgr::getInstance()->mcuToMqtt(szBuffer, iBufferSize + CRC16_LEN);

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
        verbuf[1] = StrGetUInt8(pszMessage + i);
        i++;
        verbuf[0] = StrGetUInt8(pszMessage + i);
        i++;
        sprintf(ver_tmp, "W8_HC130_106F_V%d.%d.%d", verbuf[1], verbuf[0] >> 4, verbuf[0] & 0x0f);
        LOGD("MCU_VERSION:<%s>.\r\n", ver_tmp);

        // 保存设置到系统配置文件
        update_mcu_info(ver_tmp);
        //read_config("./config.ini");
        //LOGD("MCU_VERSION:<%s>.\n", ver_tmp);

        //system("sync");
    }

    boot_mode = StrGetUInt8(pszMessage + i);
    if (boot_mode != BOOT_MODE_INVALID) {
//      长供电版本 模式都设置为识别
        boot_mode = BOOT_MODE_RECOGNIZE;
    }
    LOGD("boot_mode: %d\r\n", boot_mode);
//    receive_boot_mode = 1;
    if ( boot_mode == BOOT_MODE_RECOGNIZE ) {
        if (oasis_task_start == false) {
            oasis_task_start = true;
            OpenLcdBackground();
            Display_Start();
            Oasis_Start();

            //Display_Update((uint32_t)wave_logo_v3);
        }
    } else if (boot_mode == BOOT_MODE_REMOTE) {
//        cmdRemoteRequestRsp(HEAD_MARK, CMD_BOOT_MODE, 1);
        cmdRemoteRequestRsp(HEAD_MARK, CMD_WIFI_OPEN_DOOR, 1);
    } else if (boot_mode == BOOT_MODE_PREVIEW) {
        if (oasis_task_start == false) {
            oasis_task_start = true;
            OpenLcdBackground();
            Display_Start();
//            Oasis_Start();
            //Display_Update((uint32_t)wave_logo_v3);
        }
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

//  清空操作记录
    int clear_status = DBManager::getInstance()->clearRecord();
    LOGD("Clear Records status is %d\r\n", clear_status);

//  清空用戶附加信息
    int ret= UserExtendManager::getInstance()->clearAllUserJson();
    LOGD("%s 清空用户附加信息 result: %s \r\n", logtag, ret);
//    清空用户注册表
    clear_status = VIZN_DelUser(NULL);
    LOGD("Clear Users status is %d\r\n", clear_status);

//  清除配置文件
    clear_json_config_file();
    //DB_Save(0);
    return;
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
//    cmdCommRsp2MCU(CMD_REQ_RESUME_FACTORY, ret);

    vTaskDelay(pdMS_TO_TICKS(10));

    //消息处理
    SetSysToFactory();
    /*关机 */

    cmdCloseFaceBoardReq();
    return 0;
}

//主控返回响应指令: 用户注册回复
int cmdUserRegRsp(uint8_t ret) {
    LOGD("用户注册回复 \r\n");
    char szBuffer[32] = {0};
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
    int iTailIndex = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_FACE_REG,
            MsgLen);


//  拼装消息尾
    int iTotalLen =MsgTail_Pack( szBuffer, iTailIndex);
    if( iTotalLen > sizeof(szBuffer) ){
        LOGD("%s ERROR  %d must <  %d \r\n",iTotalLen, sizeof(szBuffer) );
    }
    SendMsgToMCU((uint8_t *) szBuffer, iTotalLen);

    return 0;
}


//主控接收指令:  用户注册请求
int cmdUserRegReqProc(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("用户注册请求 \r\n");
//    信号量加一
    if(xSemaphoreGive(workCountSemaphore) ==pdTRUE ){
        LOGD("xSemaphoreGive ok! \r\n");
    }else{
        LOGD("xSemaphoreGive failed! \r\n");
    }

    uint8_t ret = SUCCESS, len = 0;
    const unsigned char *pos = pszMessage;

//  1.将当前状态设置为 注册
    boot_mode = BOOT_MODE_REGIST;

//  2. 解析指令, 获取UUID, 开始时间,结束时间,柜门编号
    UserExtendClass *pUserExtendType = UserExtendManager::getInstance()->getCurrentUser();
    memset( pUserExtendType, 0, sizeof(UserExtendClass) );
    if (  nMessageLen >= 8 ) {
//        memcpy(&g_uu_id, pos + len, 8);
        memcpy( (void*)pUserExtendType->HexUID, pos+len, 8);
        len += 8;
        pos+=8;
    }

    g_reging_flg = REG_STATUS_ING;
    if (lcd_back_ground == false) {
        OpenLcdBackground();
    }

    HexToStr(pUserExtendType->UUID, pUserExtendType->HexUID, sizeof(pUserExtendType->HexUID)  );
    LOGD("%s UUID<len:%d>:%s.\r\n",logtag, sizeof(pUserExtendType->UUID), pUserExtendType->UUID);


//2.发起注册
    vizn_api_status_t status;
    status = VIZN_AddUser(NULL, pUserExtendType->UUID);
    LOGD("%s, VIZN AddUser %s\r\n",logtag, pUserExtendType->UUID);
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
    }else{
        //注册成功, 则往人员拓展表中增加拓展的记录
        LOGD( "%s VIZEN_ADD_User Success\r\n",logtag);
    }

    //返回响应消息
    // cmdUserRegRsp(ret);
    return 0;
}

//主控发送测试指令: 注册结果通知请求
/* status:
0-完成注册,待激活
1-注册失败
*/
int cmdRegResultNotifyReq(UserExtendClass *userExtendClass, uint8_t regResult) {
    LOGD("%s 注册结果通知 \r\n", logtag);
    char szBuffer[32] = {0};
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /*填充消息体*/
    memcpy(pop, userExtendClass->HexUID, sizeof(userExtendClass->HexUID));
    MsgLen += sizeof( userExtendClass->HexUID);
    pop += sizeof( userExtendClass->HexUID);
//    StrSetUInt8((uint8_t *) pop, CMD_FACE_REG);
//    MsgLen += sizeof(uint8_t);
//    pop += sizeof(uint8_t);

    StrSetUInt8((uint8_t *) pop, regResult);
    MsgLen += sizeof(uint8_t);
    pop += sizeof(uint8_t);

    /*填充消息头*/
    int iTailIndex = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_FACE_REG_RLT,
            MsgLen);


//  拼装消息尾
    int iTotalLen =MsgTail_Pack( szBuffer, iTailIndex);
    if( iTotalLen > sizeof(szBuffer) ){
        LOGD("%s ERROR  %d must <  %d \r\n",iTotalLen, sizeof(szBuffer) );
    }

    SendMsgToMCU((uint8_t *) szBuffer, iTotalLen);

    return 0;
}

//主控发送测试指令:  开门请求
int cmdOpenDoorReq( ) {
    LOGD("%s  向mcu发送开门请求 \r\n", logtag);
    char szBuffer[64] = {0};
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /*填充消息体*/
//    uint8_t L25MAC[6]={0};//十六进制uuid
//    memcpy( pop, L25MAC, sizeof(L25MAC));
//    MsgLen += sizeof(L25MAC);
//    pop += sizeof(L25MAC);
//    填入uuid
    UserExtendClass *pUserExtendType = UserExtendManager::getInstance()->getCurrentUser();
    LOGD("%s UUID %s\r\n",logtag, pUserExtendType->UUID);
    memcpy(pop, pUserExtendType->HexUID, sizeof(pUserExtendType->HexUID));
    MsgLen += sizeof(pUserExtendType->HexUID);
    pop += sizeof(pUserExtendType->HexUID);

//    填入device列表
//    LOGD( "%s DeviceList %s\r\n",logtag, userExtendType->cDeviceId);
//    unsigned char deviceList[48]={0};
//    int  iHexDeviceLen = strlen(userExtendType->cDeviceId)/2;
//    StrToHex( deviceList, userExtendType->cDeviceId, iHexDeviceLen);
//    memcpy(pop , deviceList, iHexDeviceLen);
//    MsgLen += iHexDeviceLen;
//    pop += iHexDeviceLen;
    /*填充消息头*/
    int iTailIndex = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_OPEN_DOOR,
            MsgLen);

    int iTotalLen = MsgTail_Pack( szBuffer, iTailIndex);
    if( iTotalLen > sizeof(szBuffer) ){
        LOGD("%s ERROR  %d must <  %d \r\n",iTotalLen, sizeof(szBuffer) );
    }

    SendMsgToMCU((uint8_t *) szBuffer, iTotalLen );

    return 0;
}

// 主控接收指令:开门响应
int cmdOpenDoorRsp(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("w8 收到mcu 的开门响应 \r\n");
    uint8_t ret = -1;
    unsigned char *pop = NULL;
    unsigned char szBuffer[32] = {0};

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
        uint8_t power2 = StrGetUInt8(pop);

        UserExtendClass *pUserExtendClass = UserExtendManager::getInstance()->getCurrentUser();

        LOGD("%s 创建门禁记录\r\n",logtag);
        Record *record = (Record *) pvPortMalloc(sizeof(Record));
        strcpy(record->UUID, pUserExtendClass->UUID);
//        record->action = FACE_UNLOCK;//  操作类型：0代表注册 1: 一键开锁 2：钥匙开锁  3 人脸识别开锁
        record->action = ACCESS_MODE;// R60 项目 操作类型：0代表 1: 考勤 2：门禁  3 门禁 +考勤
        char image_path[16];
        //record->status = 0; // 0,操作成功 1,操作失败.
        record->time_stamp = ws_systime; //时间戳 从1970年开始的秒数
//        record->power = power * 256 + power2;
        record->data[0]=power;
        record->data[1]=power2;
        //sprintf(power_msg, "{\\\"batteryA\\\":%d\\,\\\"batteryB\\\":%d}", record->power, record->power2);
        //LOGD("power_msg is %s \r\n", power_msg);

        record->upload = BOTH_UNUPLOAD; //   0代表没上传 1代表记录上传图片未上传 2代表均已
//        record->action_upload = 0x300;
        memset(image_path, 0, sizeof(image_path)); // 对注册成功的用户保存一张压缩过的jpeg图片
        //snprintf(image_path, sizeof(image_path), "REC_%d_%d_%s.jpg", 0, record->time_stamp, record->UUID);
        snprintf(image_path, sizeof(image_path), "%x.jpg", record->time_stamp);

        memcpy(record->image_path, image_path, sizeof(image_path));//image_path

        DBManager::getInstance()->addRecord(record);

        Oasis_SetOasisFileName(record->image_path);
        Oasis_WriteJpeg();

        int ID = DBManager::getInstance()->getLastRecordID();
        LOGD("开锁成功, 更新数据库状态.请求MQTT上传本次开门的记录 \r\n");
        cmdRequestMqttUpload(ID);
    } else {
        LOGD("开锁失败,不更新数据库状态. 不上传记录,请求下电\r\n");
        cmdCloseFaceBoardReq();

    }

    return 0;
}

// 主控接收指令:测温响应
int cmdTemperRsp(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("x7 收到mcu 的测温响应 \r\n");
    uint16_t ret = -1;
//    unsigned char *pop = NULL;
    char szBuffer[128]={0};

    // MCU到face_loop，0代表开锁成功，1代表开锁失败
    memcpy(szBuffer, pszMessage, nMessageLen);
    LOGD("收到的测温数据  \r\n");
    for(int i=0; i<nMessageLen; i++){
        LOGD( "%d  0x%02x \r\n", i, szBuffer[i]);
    }
    //    pop = pszMessage;

    // MCU到face_loop，0代表开锁成功，1代表开锁失败
//    ret = StrGetUInt16(pszMessage);
    // 如果开锁成功 更新数据库状态 ,请求mqtt上传本次操作记录。
    if (nMessageLen != 0) {
//  xshx add
//        char power_msg[32] = {0};
//        pop += 1;
//        uint8_t power = StrGetUInt8(pop);
//        pop += 1;
//        uint8_t  power2 = StrGetUInt8(pop);

        Record *record = (Record *) pvPortMalloc(sizeof(Record));
        memset( record, 0, sizeof(Record));
        UserExtendClass *pUserExtendType = UserExtendManager::getInstance()->getCurrentUser();
        strcpy(record->UUID, pUserExtendType->UUID);
        record->action = FACE_TEMPER;//  操作类型：0代表注册 1: 一键开锁 2：钥匙开锁  3 人脸识别开锁 10 测温
        char image_path[16];
        //record->status = 0; // 0,操作成功 1,操作失败.
        record->time_stamp = ws_systime; //时间戳 从1970年开始的秒数

//        record->power = power * 256 + power2;
        //sprintf(power_msg, "{\\\"batteryA\\\":%d\\,\\\"batteryB\\\":%d}", record->power, record->power2);
        //LOGD("power_msg is %s \r\n", power_msg);

        record->upload = BOTH_UNUPLOAD; //   0代表没上传 1代表记录上传图片未上传 2代表均已
//        record->action_upload = 0x1000;
        memset(image_path, 0, sizeof(image_path)); // 对注册成功的用户保存一张压缩过的jpeg图片
        snprintf(image_path, sizeof(image_path), "%x.jpg", record->time_stamp);
        memcpy(record->image_path, image_path, sizeof(image_path));//image_path

//        蓝牙测温
        char* ret = base64_encode( szBuffer, record->data, nMessageLen );
        LOGD(" record->data %s \r\n",  record->data);
        DBManager::getInstance()->addRecord(record);

        Oasis_SetOasisFileName(record->image_path);
        Oasis_WriteJpeg();

//        WriteTemper(record->image_path, (char *)szBuffer, sizeof(szBuffer) );//写温度文件

        int ID = DBManager::getInstance()->getLastRecordID();
        LOGD("测温成功, 更新数据库状态.请求MQTT上传本次测温的记录 \r\n");
        cmdRequestMqttUpload(ID);
    } else {
        LOGD("测温失败,不更新数据库状态. 不上传记录,请求下电 \r\n");
        cmdCloseFaceBoardReq();

    }

    return 0;
}
int cmdPowerDownRsp(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("收到MCU发来的下电回复 \r\n");
    for (int i = 0; i < nMessageLen; i++) {
        LOGD("0x%02x	\r\n", pszMessage[i]);
    }
    return 0;
}

// 主控接收指令:机械开锁响应
int cmdMechicalLockRsp(unsigned char nMessageLen, const unsigned char *pszMessage) {
    uint8_t ret = -1;
    unsigned char *pop = NULL;
    unsigned char szBuffer[32] = {0};

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
        uint8_t power2 = StrGetUInt8(pop);

        Record *record = (Record *) pvPortMalloc(sizeof(Record));
        UserExtendClass *pUserExtendType = UserExtendManager::getInstance()->getCurrentUser();
        strcpy(record->UUID, pUserExtendType->UUID );
        record->action = MECH_UNLOCK;//  操作类型：0代表注册 1: 一键开锁 2：钥匙开锁  3 人脸识别开锁  机械开锁
        char image_path[16];
        //record->status = 0; // 0,操作成功 1,操作失败.
        record->time_stamp = ws_systime; //时间戳 从1970年开始的秒数
//        record->power = power * 256 + power2;
        record->data[0]=power;
        record->data[1]=power2;
        //sprintf(power_msg, "{\\\"batteryA\\\":%d\\,\\\"batteryB\\\":%d}", record->power, record->power2);
        //LOGD("power_msg is %s \r\n", power_msg);

        record->upload = BOTH_UNUPLOAD; //   0代表没上传 1代表记录上传图片未上传 2代表均已
//        record->action_upload = 0xB00;
        memset(image_path, 0, sizeof(image_path)); // 对注册成功的用户保存一张压缩过的jpeg图片
        //snprintf(image_path, sizeof(image_path), "REC_%d_%d_%s.jpg", 0, record->time_stamp, record->UUID);
        //snprintf(image_path, sizeof(image_path), "C%d", record->time_stamp);
        //memcpy(record->image_path, image_path, sizeof(image_path));//image_path

        DBManager::getInstance()->addRecord(record);

        //Oasis_SetOasisFileName(record->image_path);
        //Oasis_WriteJpeg();

        int ID = DBManager::getInstance()->getLastRecordID();
        LOGD("机械开锁成功, 更新数据库状态.请求MQTT上传本次开门的记录 \r\n");
        cmdRequestMqttUpload(ID);
    } else {
        cmdCloseFaceBoardReq();
        LOGD("开锁失败,不更新数据库状态. 不上传记录\r\n");
    }

    return 0;
}
//MCU通知主机,MCU即将下电
int cmdReqPoweDown(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("MCU通知主机,MCU请求下电");

    char szBuffer[64] = {0};
    int iBufferSize;
    char *pop = NULL;
    unsigned char MsgLen = nMessageLen;

    pop = szBuffer+sizeof(MESSAGE_HEAD);
    /*填充消息体*/
    memcpy(pop, pszMessage, nMessageLen);
    pop+=nMessageLen;
    /*填充消息头*/
    iBufferSize = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_REQ_POWER_DOWN,
            MsgLen);

    /*计算FCS*/
    unsigned short cal_crc16 = CRC16_X25((uint8_t *) szBuffer, MsgLen + sizeof(MESSAGE_HEAD));
    memcpy((uint8_t *) pop, &cal_crc16, sizeof(cal_crc16));

    save_files_before_pwd();

    vTaskDelay(pdMS_TO_TICKS(100));

    SendMsgToMCU((uint8_t *) szBuffer, iBufferSize + CRC16_LEN);

    return 0;
}

// 主控接收指令: 锁模式响应
int cmdLockModeRsp(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("锁模式下发,回复MQTT \r\n");

    uint8_t ret = SUCCESS;

    ret = StrGetUInt8(pszMessage);

    // 转发响应给 Mqtt 模块
    cmdCommRsp2Mqtt(CMD_LOCK_MODE, ret);
    //usleep(200);
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
        //DB_Save(0);
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
        //DB_Save(0);
        save_json_config_file();
        saving_file = true;
    }
// save record list  to file
    if (saving_db == false) {
        saving_db = true;
        DBManager::getInstance()->flushRecordList();
    }
    g_is_save_file = false;
    LOGD("保存config, db, jpg 文件结束 \r\n");

    return 0;
}


//主控发送: 关机请求
int cmdCloseFaceBoardReqExt(bool save_file) {
    LOGD("发送关机请求 \r\n");
    char szBuffer[32] = {0};
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /*填充消息体*/

    /*填充消息头*/
    int iTailIndex = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_CLOSE_FACEBOARD,
            MsgLen);

//  拼装消息尾
    int iTotalLen =MsgTail_Pack( szBuffer, iTailIndex);
    if( iTotalLen > sizeof(szBuffer) ){
        LOGD("%s ERROR  %d must <  %d \r\n",iTotalLen, sizeof(szBuffer) );
    }

    if (save_file) {
        save_files_before_pwd();
    } else {
        LOGD("No need to save file before shutdown the device\r\n");
    }

//    xshx add
    vTaskDelay(pdMS_TO_TICKS(100));
    if( uxSemaphoreGetCount(workCountSemaphore) == 0  ){
        LOGD("没有未完成的任务 \r\n");
        SendMsgToMCU((uint8_t *) szBuffer, iTotalLen);
    }else{
        LOGD("等待未完成的任务 \r\n");
    }

    return 0;
}

//主控发送: 关机请求
int cmdCloseFaceBoardReq() {
#if 0
    return cmdCloseFaceBoardReqExt(true);
#endif
    LOGD("不下电版本 此处不下电\r\n");
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
    char *pop = NULL;
    unsigned char MsgLen = 0;

    memset(szBuffer, 0, sizeof(szBuffer));
    pop = szBuffer + sizeof(MESSAGE_HEAD);

    /*填充消息体*/
//    StrSetUInt8((uint8_t *) pop, CMD_FACE_DELETE_USER);
//    MsgLen += sizeof(uint8_t);
//    pop += sizeof(uint8_t);
    StrSetUInt8((uint8_t *) pop, result);
    MsgLen += sizeof(uint8_t);
    pop += sizeof(uint8_t);

    /*填充消息头*/
    int iTailIndex = MsgHead_Packet(
            szBuffer,
            HEAD_MARK,
            CMD_FACE_DELETE_USER,
            MsgLen);

//  拼装消息尾
    int iTotalLen =MsgTail_Pack( szBuffer, iTailIndex);
    if( iTotalLen > sizeof(szBuffer) ){
        LOGD("%s ERROR  %d must <  %d \r\n",iTotalLen, sizeof(szBuffer) );
    }

    SendMsgToMCU((uint8_t *) szBuffer, iTotalLen);

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
    const unsigned char *pos = pszMessage;

    UserExtendClass *pUserExtendType = UserExtendManager::getInstance()->getCurrentUser();
    memset( pUserExtendType,0, sizeof(UserExtendClass) );
    memcpy( pUserExtendType->HexUID, pos, sizeof(pUserExtendType->HexUID));
    HexToStr(pUserExtendType->UUID, pUserExtendType->HexUID, sizeof(pUserExtendType->HexUID));
    LOGD("current uuid %s \r\n", pUserExtendType->UUID);
//    删除用户的操作记录
    int ret = DBManager::getInstance()->deleteRecordByUUID( pUserExtendType->UUID );
    LOGD("删除用户操作记录 %d\r\n", ret);
//    删除用户附加信息
    ret = UserExtendManager::getInstance()->delUserJsonByUUID( pUserExtendType->UUID );
    LOGD("删除用户 %s 附加信息 返回码 %d \r\n", pUserExtendType->UUID, ret);
//    删除用户
    vizn_api_status_t status = VIZN_DelUser(NULL, pUserExtendType->UUID);
    LOGD("删除用户注册表中用户 %d \r\n", status);

//    只有一种返回失败的情况 即用户不存在  ,这种情况也返回成功
//    if (kStatus_API_Layer_Success == status) {
//        ret = SUCCESS;
//    }else {
//        ret = FAILED;
//    }
    ret = SUCCESS;

    DBManager::getInstance()->flushRecordList();//写回 记录文件
    LOGD("delete uuid :  %s nHead 0x%2x.\r\n", pUserExtendType->UUID, nHead);
    if (nHead == HEAD_MARK_MQTT) {
        cmdCommRsp2MqttByHead(HEAD_MARK_MQTT, CMD_FACE_DELETE_USER, ret);
    } else {
        cmdCommRsp2MCU(CMD_FACE_DELETE_USER, ret);
    }

//
    LOGD("删除用户后请求下电 \r\n");
    cmdCloseFaceBoardReq();

    return  0;

}

//串口接收消息处理
// 主控接收指令: WIFI SSID 设置请求
int cmdWifiSSIDProc(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("接收指令:设置 WIFI SSID  \r\n");

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
    LOGD("接收指令:设置 WIFI 密码  \r\n");

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
        update_mqtt_opt(mqtt_user_tmp, (char *) mqtt_pwd);
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
    LOGD("设置MQTT URL \r\n");
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
//    SendMsgToMQTT(szBuffer, iBufferSize + CRC16_LEN);
    MqttMcuMgr::getInstance()->mcuToMqtt(szBuffer, iBufferSize + CRC16_LEN);

    return 0;
}

// 主控接收指令: wifi 网络时间同步响应
int cmdWifiTimeSyncRsp(unsigned char nMessageLen, const unsigned char *pszMessage) {
    LOGD("网络时间同步响应 \r\n");
    uint8_t ret = SUCCESS;

    ret = StrGetUInt8(pszMessage);

    {
        cmdCommRsp2Mqtt(CMD_WIFI_TIME_SYNC, ret);
    }

    return 0;
}

//MCU->106F->MQTT:  WIFI 同步订单时间响应
int cmdWifiOrderTimeSyncRsp(unsigned char nMessageLen, const unsigned char *pszMessage) {
    uint8_t ret = StrGetUInt8(pszMessage);
    LOGD("远程同步订单请求  %d \r\n", (uint8_t) nMessageLen);

    cmdCommRsp2Mqtt(CMD_ORDER_TIME_SYNC, ret);
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

    MqttMcuMgr::getInstance()->mcuToMqtt(szBuffer, iBufferSize + CRC16_LEN);

    return 0;
}

// 20201120 wszgx added for display correct date/time information in the screen
/*!
 * @brief Software timer callback.
 */
void SwTimerCallback(TimerHandle_t xTimer) {
    ws_systime++;
}




