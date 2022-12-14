/**************************************************************************
 * 	FileName:	 	MCU_UART5.cpp
 *	Description:	This file is including the function interface which is used to processing
 					uart5 communication with mcu
 *	Copyright(C):	2018-2020 Wave Group Inc.
 *	Version:		V 1.0
 *	Author:			tanqw
 *	Created:		2020-10-26
 *	Updated:        xshx 2022/5/20.
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
#include "../mqtt/mqtt-remote-feature.h"
#include "../mqtt/base64.h"
#include "database.h"
#include "DBManager.h"
#include "UserExtendManager.h"
#include "oasis.h"
#include "display.h"
#include "aw_wave_logo_v3.h"
#include "MCU_UART5.h"
#include "wifi_main.h"
#include "mqtt_util.h"
#include "mqtt-common.h"

static const char *logtag ="[UART5] ";

QueueHandle_t Uart5MsgQ = NULL;

/*!< Queue used by Uart5 Task to receive messages*/
enum {
    UART5_RX_MSG_STATUS_WAITING_HEADER,
    UART5_RX_MSG_STATUS_WAITING_LENGTH,
    UART5_RX_MSG_STATUS_WAITING_DATA,

};
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

uint8_t background_buffer5[UART5_DRV_RX_RING_BUF_SIZE] = {0};


//static face_info_t gFaceInfo; //?????????oasis_rt106f_elcok.cpp????????????????????????????????????????????????????????????
//????????????????????????
static uint32_t g_reg_start = 0;    //?????????????????????????????????

struct _lpuart_handle t_handle5;


lpuart_rtos_config_t lpuart_config5 = {
        .baudrate    = 115200,
        .parity      = kLPUART_ParityDisabled,
        .stopbits    = kLPUART_OneStopBit,
        .buffer      = background_buffer5,
        .buffer_size = sizeof(background_buffer5),
};


int SendMsgToSelf(unsigned char *MsgBuf, unsigned char MsgLen) {

    unsigned char szBuffer[128] = {0};

    const unsigned char *pszMsgInfo = NULL;
    unsigned char CmdId = 0;
    unsigned char HeadMark;

    memset(szBuffer, 0, sizeof(szBuffer));
    memcpy(szBuffer, &MsgBuf[0], MsgLen);

    pszMsgInfo = MsgHead_Unpacket(
            szBuffer,
            MsgLen,
            &HeadMark,
            &CmdId,
            &MsgLen);

    if (HeadMark != HEAD_MARK_MQTT) {
        LOGD("msg headmark error: 0x%x!\n", HeadMark);
        return -1;
    }
    //ProcMessage(CmdId, MsgLen, pszMsgInfo);
    ProcMessageFromMQTT(CmdId, MsgLen, pszMsgInfo);
    return 0;
}
int ProcMessage(
        unsigned char nCommand,
        unsigned char nMessageLen,
        const unsigned char *pszMessage) {
    ProcMessageByHead(HEAD_MARK, nCommand, nMessageLen, pszMessage);
    return 0;
}

int ProcMessageFromMQTT(
        unsigned char nCommand,
        unsigned char nMessageLen,
        const unsigned char *pszMessage) {
    ProcMessageByHead((unsigned char) (HEAD_MARK_MQTT), nCommand, nMessageLen, pszMessage);
    return 0;
}

// source: 0x23 MCU 0x24 MQTT
int ProcMessageByHead(
        unsigned char nHead,
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
        case CMD_TEMPER_DATA:{//??????????????????
            cmdTemperRsp( nMessageLen, pszMessage );
            break;
        }
        case CMD_CLOSE_FACEBOARD: {
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
            cmdDeleteUserReqProcByHead(/*HEAD_MARK*/nHead, nMessageLen, pszMessage);
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
        case CMD_WIFI_TIME_SYNC: {
            cmdWifiTimeSyncRsp(nMessageLen, pszMessage);
            break;
        }
        case CMD_ORDER_TIME_SYNC: {
            cmdWifiOrderTimeSyncRsp(nMessageLen, pszMessage);
            break;
        }
//        ???????????????????????????
        case CMD_LOCK_MODE: {
            cmdLockModeRsp(nMessageLen, pszMessage);
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
        case CMD_REQ_POWER_DOWN:{//MCU ??????116 MCU ????????????
            cmdReqPoweDown(nMessageLen, pszMessage);
        }
        default:
            LOGD("No effective cmd from MCU!\n");
            break;
    }

    return 0;
}


int Uart5_SendDeinitCameraMsg(void) {
#if  SUPPORT_POWEROFF
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
#endif
    return 0;
}

//
static void vReceiveOasisTask(void *pvParameters) {
    BaseType_t  ret;
    face_info_t *faceInfo;
    int recognize_times = 0;
    char image_path[16] = {0};

    while (1) {
        //LOGD("vReceiveOasisTask()  -> xQueueReceive()!\n");
        // pick up message
        int REG_RESULT_FLG = -1;
        ret = xQueueReceive(Uart5MsgQ, (void *) &faceInfo, portMAX_DELAY);
        if (ret == pdTRUE) {
            switch( faceInfo->evt ){
                case OASISLT_EVT_REG_COMPLETE:
                    if( boot_mode == BOOT_MODE_REGIST ){
                        UserExtendClass  *pUserExtendType = UserExtendManager::getInstance()->getCurrentUser();
                        if( faceInfo->enrolment_result == OASIS_REG_RESULT_OK){
                            LOGD( "%s ???????????? ???????????????????????? %s  ,??????????????? %d   \r\n",logtag, pUserExtendType->UUID, ws_systime );
//                            UserExtend userExtend;
//                            memset( &userExtend, 0, sizeof(UserExtend) );
//                            vConvertUserExtendType2Json( &userExtendType,  &userExtend );
//                            int result = UserExtendManager::getInstance()->saveUserJsonToFlash(   &userExtend );
//                            LOGD( "%s ???????????????????????? %d\r\n",logtag, result);
                            //????????????????????????  ??????????????????
                            LOGD("%s?????????????????? \r\n", logtag);
                            Record *record = (Record *) pvPortMalloc(sizeof(Record));
                            memset(record, 0, sizeof(Record));
//                            strcpy(record->UUID, username);
                            strcpy(record->UUID, pUserExtendType->UUID);
                            record->action = REGISTE;// 0 ????????????
                            record->time_stamp = ws_systime;//????????????
                            memset(image_path, 0, sizeof(image_path)); // ????????????????????????????????????????????????jpeg??????

                            snprintf(image_path, sizeof(image_path), "%x.jpg", record->time_stamp);
                            memcpy(record->image_path, image_path, sizeof(image_path));//image_path
                            //record->status = SUCCESS;// ??????????????????
    //                        record->power = 0xFFFF;
                            record->data[0]=0xFF;
                            record->data[1]=0xFF;

                            record->upload = BOTH_UNUPLOAD;// 0??????????????? 1????????????????????????????????? 2????????????
    //                        record->action_upload = 0;	//????????????????????????
                            LOGD("%s??????????????????????????????????????? \r\n", logtag);
                            DBManager::getInstance()->addRecord(record);
//
                            Oasis_SetOasisFileName(record->image_path);
                            Oasis_WriteJpeg();
                            REG_RESULT_FLG = 0; //???????????????, 0 ????????????
                            cmdRegResultNotifyReq( pUserExtendType, REG_RESULT_FLG);
//                           ??????????????????
                            LOGD("????????????,??????MQTT?????????????????????????????? \n");
                            int ID = DBManager::getInstance()->getLastRecordID();
                            cmdRequestMqttUpload(ID);
                        }else if( faceInfo->enrolment_result == OASIS_REG_RESULT_DUP){
                            LOGD("???????????? \r\n");
                            REG_RESULT_FLG = 9;// ???????????????, 9 ??????????????????
                            cmdRegResultNotifyReq( pUserExtendType, REG_RESULT_FLG);
                        }else{
                            LOGD("???????????? \r\n");
                            REG_RESULT_FLG = 1; //???????????????, 1 ????????????
                            cmdRegResultNotifyReq( pUserExtendType, REG_RESULT_FLG);
                        }
                        if( xSemaphoreTake( workCountSemaphore,0 ) ==pdTRUE ){
                            LOGD("xSemaphoreTake ok! \r\n");
                        }else{
                            LOGD("xSemaphoreTake failed! \r\n");
                        }
                        cmdCloseFaceBoardReq();//???????????????
                        boot_mode = BOOT_MODE_RECOGNIZE;
                    } else{
                        LOGD("evt %d, boot_mode %d \r\n", faceInfo->evt, boot_mode );
                    }
                    break;
                case OASISLT_EVT_REC_COMPLETE:
                    if( boot_mode == BOOT_MODE_RECOGNIZE ) {
                        if (faceInfo->recognize_result == R60_REC_ATTSUCC_ACCFAIL_FACE || faceInfo->recognize_result==R60_REC_ATTSUCC_ACCSUCC_FACE ) {// ????????????
                            LOGD("UUID %s ,??????????????????  %d\r\n", faceInfo->name.c_str(), faceInfo->recognize_result );

//                          ??????????????????, ?????????1 ????????????,??????????????? 2. ????????????, ?????????????????????????????????
                            UserExtendClass *pUserExtendClass = UserExtendManager::getInstance()->getCurrentUser( );
                            LOGD("?????????????????? \r\n");
//                          ????????????
                            Record *record = (Record *) pvPortMalloc(sizeof(Record));
                            strcpy(record->UUID, pUserExtendClass->UUID);
//        record->action = FACE_UNLOCK;//  ???????????????0???????????? 1: ???????????? 2???????????????  3 ??????????????????
                            record->action = ATTENDANCE_MODE;// R60 ?????? ???????????????0?????? 1: ?????? 2?????????  3 ?????? +??????
                            char image_path[16] = {0};
                            //record->status = 0; // 0,???????????? 1,????????????.
                            record->time_stamp = ws_systime; //????????? ???1970??????????????????
//        record->power = power * 256 + power2;
                            record->data[0] = 0xff;//????????????
                            record->data[1] = 0xff;//????????????
                            //sprintf(power_msg, "{\\\"batteryA\\\":%d\\,\\\"batteryB\\\":%d}", record->power, record->power2);
                            //LOGD("power_msg is %s \r\n", power_msg);

                            record->upload = BOTH_UNUPLOAD; //   0??????????????? 1????????????????????????????????? 2????????????
//        record->action_upload = 0x300;
                            memset(image_path, 0, sizeof(image_path)); // ????????????????????????????????????????????????jpeg??????
                            //snprintf(image_path, sizeof(image_path), "REC_%d_%d_%s.jpg", 0, record->time_stamp, record->UUID);
                            snprintf(image_path, sizeof(image_path), "%x.jpg", record->time_stamp);

                            memcpy(record->image_path, image_path, sizeof(image_path));//image_path

                            DBManager::getInstance()->addRecord(record);

                            Oasis_SetOasisFileName(record->image_path);
                            Oasis_WriteJpeg();
                            int ID = DBManager::getInstance()->getLastRecordID();
                            LOGD("????????????, ???????????????????????????.??????MQTT???????????????????????? \r\n");
                            cmdRequestMqttUpload(ID);
                            recognize_times = 0;
                        }
                        if( (faceInfo->recognize_result == R60_REC_ATTFAIL_ACCSUCC_FACE) || (faceInfo->recognize_result == R60_REC_ATTSUCC_ACCSUCC_FACE)){
                                // ??????????????????
                            LOGD("UUID %s ,?????????????????? %d, ?????????????????? \r\n", faceInfo->name.c_str(), faceInfo->recognize_result);

                            cmdOpenDoorReq();
                            recognize_times = 0;

                        }
                        if(faceInfo->recognize_result == OASIS_REC_RESULT_UNKNOWN_FACE){
                            LOGD("????????? \r\n");
                            recognize_times = 0;
                        }
                        if(true){// ??????40??? ????????????
                            recognize_times++;
                            if (recognize_times > 40 ) {
                                LOGD("User face recognize timeout \r\n");
                                recognize_times = 0;
                                CloseLcdBackground();
                                Uart5_SendDeinitCameraMsg();
                                cmdCloseFaceBoardReq();//???????????????
                            }
                        }
                    }else{
                        LOGD("evt %d, boot_mode %d \r\n", faceInfo->evt, boot_mode );
                    }
                    break;
                default:
                    break;
            }
            vPortFree(faceInfo);

        } else{
            LOGD("Receive Message Error \r\n ");
        }
    }
}
//static void uart5_sync_task(void *pvParameters) {
static void vUartInitTask(void *pvParameters) {
    //vTaskDelay(pdMS_TO_TICKS(200));
    //check_config();
    vTaskDelay(pdMS_TO_TICKS(300));
    cmdSysInitOKSyncReq(SYS_VERSION);
    vTaskDelete(NULL);
}


static void vReceiveUartTask(void *pvParameters) {
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
    LOGD("[vReceiveUartTask]:starting...\r\n");
    lpuart_config5.srcclk = DEMO_LPUART_CLK_FREQ;
    lpuart_config5.base = DEMO_LPUART;

    if (kStatus_Success != LPUART_RTOS_Init(&handle5, &t_handle5, &lpuart_config5)) {
        LOGD("[vReceiveUartTask]:Error,LPUART_RTOS_Init failed!\r\n");
        vTaskSuspend(NULL);
    }

    /* Send introduction message. */
    //tell mcu that mainboard has Initialized OK.
    //cmdSysInitOKSyncReq(SYS_VERSION);

    /* Receive user input and send it back to terminal. */
    do {

        LOGD("[vReceiveUartTask]:Recv New Msg ...\r\n");
        memset(recv_buffer, 0, sizeof(recv_buffer));
        uint8_t rx_status = UART5_RX_MSG_STATUS_WAITING_HEADER;
        msglen = 0;
        uint8_t req_len = 0;

        while (1) {
            LOGD("[vReceiveUartTask]:rx_status -> %d \r\n", rx_status);
            switch (rx_status) {
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

            if (error == kStatus_Success) {
                //LOGD("[vReceiveUartTask]:receive return!\r\n");
                if (req_len != rcvlen) {
                    LOGD("[vReceiveUartTask]:part of data received, expect:%d received:%d!\r\n", req_len, rcvlen);
                    rx_status = UART5_RX_MSG_STATUS_WAITING_HEADER;


                } else if (UART5_RX_MSG_STATUS_WAITING_HEADER == rx_status) {
                    if (HEAD_MARK != recv_buffer[0]) {
                        LOGD("[vReceiveUartTask]:wrong header received, expect:0x%x received:0x%x!\r\n", HEAD_MARK,
                             recv_buffer[0]);
                        rx_status = UART5_RX_MSG_STATUS_WAITING_HEADER;
                    } else {
                        rx_status = UART5_RX_MSG_STATUS_WAITING_LENGTH;
                    }
                } else if (UART5_RX_MSG_STATUS_WAITING_LENGTH == rx_status) {
                    if (recv_buffer[2] > UART5_RX_MAX_DATA_PACKAGE_SIZE) {
                        LOGD("[vReceiveUartTask]:wrong msg length received, length:%d\r\n", recv_buffer[2]);
                        rx_status = UART5_RX_MSG_STATUS_WAITING_HEADER;
                    } else {
                        rx_status = UART5_RX_MSG_STATUS_WAITING_DATA;
                    }
                } else {
                    //a whole package received
                    msglen = rcvlen + 3;
                    break;
                }

            } else {
                LOGD("[vReceiveUartTask]:LPUART_RTOS_Receive error:%d sts:%d!\r\n", error, rx_status);
                rx_status = UART5_RX_MSG_STATUS_WAITING_HEADER;
                continue;
            }
        }

        if (msglen > 0) {
            //LOGD("\n===rcv msg<len:%d>:", msglen);

            memset(message_buffer, 0, sizeof(message_buffer));
            HexToStr(message_buffer, recv_buffer, msglen);
            LOGD("\n===receive msg<len:%d %s>: \r\n", msglen, message_buffer);
            /*for(int i=0; i<msglen; i++)
            {
                LOGD("0x%02x	", recv_buffer[i]);
            }
            LOGD("\n");*/
        }
//        ??????????????????HEAD_MARK??????????????????????????????
        memcpy(&Msg_CRC16, recv_buffer + msglen - CRC16_LEN, CRC16_LEN);
        Cal_CRC16 = CRC16_X25(recv_buffer, msglen - CRC16_LEN);
        if (Msg_CRC16 != Cal_CRC16) {
            LOGD("CRC ?????? error: Msg_CRC16<0x%02X>, Cal_CRC16<0x%02X>!\r\n", Msg_CRC16, Cal_CRC16);
            continue;
        }
//        if( bCheckSum( recv_buffer, msglen )==false ){
//            LOGD("%s check sum failed \r\n", logtag);
//            continue;
//        }

        pszMsgInfo = MsgHead_Unpacket(
                recv_buffer,
                msglen,
                &HeadMark,
                &CmdId,
                &datalen);

        /*????????????*/
        ProcMessage(CmdId,
                    datalen,
                    pszMsgInfo);

    } while (1);// (kStatus_Success == error);
    LPUART_RTOS_Deinit(&handle5);
    vTaskSuspend(NULL);
}
static void vReceiveFakeMessageTask( void *pvParameters){

    int msglen = 0;
    uint8_t recv_buffer[128] = {0};
    unsigned char HeadMark;
    unsigned char CmdId = 0;
    unsigned char datalen = 0;
    const unsigned char *pszMsgInfo = NULL;

    printf("%s ?????? ReceiveFakeMessageTask  \r\n", logtag);
    while (1) {
        //LOGD("vReceiveOasisTask()  -> xQueueReceive()!\n");
        // pick up message
        UartMessage message;
        memset( &message, 0, sizeof(UartMessage));
        int ret = xQueueReceive( Uart5FromFakeUartMsgQueue, (void *) &message, portMAX_DELAY);
        if (ret == pdTRUE) {
            LOGD("%s  fake data: %s \r\n", logtag,  message.Data);
//            1. ???message.Data ?????? 16??????
            int msglen = strlen( message.Data )/2;
            StrToHex(recv_buffer, message.Data, msglen);
            for(int i=0; i<msglen; i++)
            {
                LOGD("0x%02x	", recv_buffer[i]);
            }
            LOGD("\r\n");
//            2. ??????
            pszMsgInfo = MsgHead_Unpacket(
                    recv_buffer,
                    msglen,
                    &HeadMark,
                    &CmdId,
                    &datalen);
            LOGD("%s ?????? HeadMark 0x%02x ,CmdId 0x%02x ,datalen 0x%02x   \r\n",logtag, HeadMark,CmdId, datalen );
//            3.????????????
            ProcMessage(CmdId,
                        datalen,
                        pszMsgInfo);
        }
    }
}

int MCU_UART5_Start() {
    LOGD("%s:starting...\r\n", logtag);

    NVIC_SetPriority(LPUART5_IRQn, 5);

    //??????uart5??????????????????task
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(vReceiveUartTask, "ReceiveUartTask", UART5TASK_STACKSIZE, NULL, UART5TASK_PRIORITY,
                                        Uart5TaskStack, &s_Uart5TaskTCB))
#else
    if (xTaskCreate(vReceiveUartTask, "ReceiveUartTask", UART5TASK_STACKSIZE, NULL, UART5TASK_PRIORITY, NULL) != pdPASS)
#endif
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }

    //??????uart5 ????????????task
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(vUartInitTask, "UartInitTask", UART5SYNCTASK_STACKSIZE, NULL, UART5SYNCTASK_PRIORITY,
                                        Uart5SyncTaskStack, &s_Uart5SyncTaskTCB))
#else
    if (xTaskCreate(vUartInitTask, "UartInitTask", UART5SYNCTASK_STACKSIZE, NULL, UART5SYNCTASK_PRIORITY, NULL) !=
        pdPASS)
#endif
    {
        PRINTF("Task vUartInitTask creation failed!.\r\n");
        while (1);
    }
    //??????UART5 Task?????????????????????QMSG
//    Uart5MsgQ = xQueueCreate(UART5_MSG_Q_COUNT, sizeof(QMsg *));
    Uart5MsgQ = xQueueCreate(UART5_MSG_Q_COUNT, sizeof(face_info_t *));
    if (Uart5MsgQ == NULL) {
        LOGE("[ERROR]:xQueueCreate uart5 queue\r\n");
        return -1;
    }

    //??????uart5???QMsg??????task
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(vReceiveOasisTask, "ReceiveOasisTask", UART5QMSGTASK_STACKSIZE, NULL, UART5QMSGTASK_PRIORITY,
                                        Uart5QmsgTaskStack, &s_Uart5QmsgTaskTCB))
#else
    if (xTaskCreate(vReceiveOasisTask, "ReceiveOasisTask", UART5QMSGTASK_STACKSIZE, NULL, UART5QMSGTASK_PRIORITY, NULL) !=
        pdPASS)
#endif
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }

// ???????????????????????????task  xshx add 20220524
    Uart5FromFakeUartMsgQueue = xQueueCreate(UART5_MSG_Q_COUNT, sizeof(UartMessage));
    if (Uart5FromFakeUartMsgQueue == NULL) {
        LOGE("[ERROR]:xQueueCreate Uart5FromFakeUartMsgQueue queue\r\n");
        return -1;
    }
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(  vReceiveFakeMessageTask, "ReceiveFakeMessageTask", FakeUartTASK_STACKSIZE, NULL, FakeUartTASK_PRIORITY,
                                        UartFakeTaskStack, &s_UartFakeTaskTCB))
#else
    if (xTaskCreate(vReceiveFakeMessageTask, "ReceiveFakeMessageTask", FakeUartTASK_STACKSIZE, NULL, FakeUartTASK_PRIORITY, NULL) !=
        pdPASS)
#endif
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }
    LOGD("%s:started...\r\n", logtag);

    return 0;
}


// build FaceInfoExtend  and send  to uart5 task when regist or recognize over
int Uart5_GetFaceInfo(  void  *pFaceInfo ){
    face_info_t *faceInfo= (face_info_t*)pvPortMalloc(sizeof(face_info_t));
    memset( faceInfo, 0, sizeof(face_info_t) );
    if( faceInfo ==NULL ){
        LOGD(" ERROR: pvPortMalloc failed \r\n");
    }
    memcpy(  faceInfo, pFaceInfo, sizeof(face_info_t) );
    if (Uart5MsgQ) {
        int ret = xQueueSend(Uart5MsgQ, &faceInfo, (TickType_t) 0);
        if (ret != pdPASS) {
            LOGE("[ERROR]:Send Uart5MsgQ failed %d\r\n", ret);
            return -1;
        }
    } else {
        LOGE("[ERROR]:Uart5MsgQ is NULL\r\n");
        return -1;
    }
    return  0;
}
