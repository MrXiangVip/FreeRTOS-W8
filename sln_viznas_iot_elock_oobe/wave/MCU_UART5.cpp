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
#include "../mqtt/base64.h"
//#include "WIFI_UART8.h"
#include "database.h"
#include "DBManager.h"
#include "UserExtendManager.h"
#include "oasis.h"
#include "display.h"
#include "aw_wave_logo_v3.h"
#include "MCU_UART5.h"


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


static face_info_t gFaceInfo; //记录从oasis_rt106f_elcok.cpp中获取当前人脸识别或者注册的相关人脸信息
//人脸注册等待激活
static uint32_t g_reg_start = 0;    //记录注册响应发出的时间

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
        case CMD_TEMPER_DATA:{//测温请求回复
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
        case CMD_REQ_POWER_DOWN:{//MCU 通知116 MCU 即将下单
            cmdReqPoweDown(nMessageLen, pszMessage);
        }
        default:
            LOGD("No effective cmd from MCU!\n");
            break;
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
                        record->action = REGISTE;// 0 代表注册
                        record->time_stamp = ws_systime;//当前时间
                        memset(image_path, 0, sizeof(image_path)); // 对注册成功的用户保存一张压缩过的jpeg图片
                        //snprintf(image_path, sizeof(image_path), "REG_%d_%d_%s.jpg", g_reging_flg, record->time_stamp,
                        //         record->UUID);
                        snprintf(image_path, sizeof(image_path), "%x.jpg", record->time_stamp);
                        memcpy(record->image_path, image_path, sizeof(image_path));//image_path
                        //record->status = SUCCESS;// 本次注册成功
//                        record->power = 0xFFFF;
                        record->data[0]=0xFF;
                        record->data[1]=0xFF;
                        //record->power1 = -1;//当前电池电量
                        //record->power2 = -1;//当前电池电量
                        record->upload = BOTH_UNUPLOAD;// 0代表没上传 1代表记录上传图片未上传 2代表均已
//                        record->action_upload = 0;	//代表注册且未上传
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
//                        CloseLcdBackground();
                        vTaskDelay(pdMS_TO_TICKS(1000));
//                        Uart5_SendDeinitCameraMsg();
                        vTaskDelay(pdMS_TO_TICKS(200));
                        cmdCloseFaceBoardReq();
                    } else {
                        //shut_down = true;
//                        CloseLcdBackground();
                        vTaskDelay(pdMS_TO_TICKS(1000));
//                        Uart5_SendDeinitCameraMsg();
                        vTaskDelay(pdMS_TO_TICKS(200));

                        save_config_feature_file();
                        vTaskDelay(pdMS_TO_TICKS(100));

                        LOGD("注册成功,请求MQTT上传本次用户注册记录 \n");
                        int ID = DBManager::getInstance()->getLastRecordID();
                        cmdRequestMqttUpload(ID);
                    }
                }
                    break;

                case QMSG_FACEREC_RECFACE: {//处理人脸识别结果
                    //LOGD("%s g_reging_flg is %d lcd_back_ground is %d\r\n", __FUNCTION__, g_reging_flg, lcd_back_ground);
                    //LOGD("处理人脸识别结果 %d flg %d\r\n", pQMsg->msg.val, g_reging_flg);
                    if ((boot_mode != BOOT_MODE_NORMAL) || (REG_STATUS_WAIT != g_reging_flg)
                        || (lcd_back_ground == false))//如果正在注册流程，就过滤掉该识别结果
                    {
                        LOGD("Face rec continue!\r\n");
                        vPortFree(pQMsg);

                        continue;
                    }
#ifdef TEST_ANY_FACE_REC
//                    if (recognize_times == 2) {
                        pQMsg->msg.val = true;
                        strcpy(pQMsg->msg.info.name, "00000000");
//                    }
#endif

                    if (pQMsg->msg.val) {//success
                        LOGD("User face recognize success!\r\n");
//                        CloseLcdBackground();
                        vTaskDelay(pdMS_TO_TICKS(1000));
//                        Uart5_SendDeinitCameraMsg();
                        //LOGD("gFaceInfo.name is %s!\n", gFaceInfo.name);
                        LOGD("pQMsg->msg.info.name is %s!\r\n", pQMsg->msg.info.name);
                        char name[64];
                        //memcpy(name, gFaceInfo.name.c_str(), gFaceInfo.name.size());
                        memcpy(name, pQMsg->msg.info.name, 64);
                        StrToHex(g_uu_id.UID, name, sizeof(g_uu_id.UID));

#if    SUPPORT_PRESSURE_TEST != 0
                        int record_count = DBManager::getInstance()->getRecordCount();
                        LOGD("record_count is %d \r\n", record_count);
                        if (record_count <= 180) {
                        	pressure_test = 0;
                            for (int i = 0; i < 20; i++) {
                                vTaskDelay(pdMS_TO_TICKS(10));
                                Record *record = (Record *) pvPortMalloc(sizeof(Record));
                                HexToStr(username, g_uu_id.UID, sizeof(g_uu_id.UID));
                                strcpy(record->UUID, username);
                                //record->status = 0; // 0,操作成功 1,操作失败.
                                record->time_stamp = ws_systime; //时间戳 从1970年开始的秒数
//                                record->power = 100 * 256 + 0;
                                record->data[0]=0xFF;
                                record->data[1]=0xFF;
                                //sprintf(power_msg, "{\\\"batteryA\\\":%d\\,\\\"batteryB\\\":%d}", record->power, record->power2);
                                //LOGD("power_msg is %s \r\n", power_msg);

                                //record->upload = 0; //   0代表没上传 1代表记录上传图片未上传 2代表均已
//                                record->action_upload = 0x300;
                                record->action = FACE_UNLOCK;//  操作类型：0代表注册 1: 一键开锁 2：钥匙开锁  3 人脸识别开锁
                                record->upload = BOTH_UNUPLOAD; //   0代表没上传 1代表记录上传图片未上传 2代表均已

                                memset(image_path, 0, sizeof(image_path)); // 对注册成功的用户保存一张压缩过的jpeg图片
                                //snprintf(image_path, sizeof(image_path), "REC_%d_%d_%s.jpg", 0, record->time_stamp, record->UUID);

                                snprintf(image_path, sizeof(image_path), "%x%02d.jpg", record->time_stamp & 0x00FFFFFF, i);
                                memcpy(record->image_path, image_path, sizeof(image_path));//image_path

                                LOGD("[%d] record->image_path is %s \r\n", i, record->image_path);

                                Oasis_SetOasisFileName(record->image_path);

                                DBManager *dbManager = DBManager::getInstance();
                                dbManager->addRecord(record);

                                //LOGD("feature.map 1size:%d\r\n", fatfs_getsize("feature.map"));
                                if (!SUPPORT_POWEROFF || g_is_shutdown == 0) {
                                    Oasis_WriteJpeg();
                                }

                                notifyKeepAlive();
                                vTaskDelay(pdMS_TO_TICKS(20));
#if	0
                                vTaskDelay(pdMS_TO_TICKS(100));

//                                char *buffer = (char *) pvPortMalloc(10 * 1024);
//                                for (int j = 0; j < 1; j++) {
//                                    int status = fatfs_read(record->image_path, buffer, 0, 10 * 1024);
//                                }
//                                vPortFree(buffer);

                                //LOGD("feature.map 2size:%d\r\n", fatfs_getsize("feature.map"));
//                                vTaskDelay(pdMS_TO_TICKS(100));
                                fatfs_delete(record->image_path);
#endif
//                                LOGD("feature.map size:%d\r\n", fatfs_getsize("feature.map"));
//                                LOGD("config size:%d\r\n", fatfs_getsize("config.jsn"));
//                                LOGD("record size:%d\r\n", fatfs_getsize("record.jsn"));

                            }
                            pressure_test = 1;
                            vTaskDelay(pdMS_TO_TICKS(100));
                        }
#endif
                        cmdOpenDoorReq(g_uu_id);
                    } else {//failed
                        recognize_times++;
                        LOGD("User face recognize failed %d times\r\n", recognize_times);
//#if    SUPPORT_POWEROFF
                        if (recognize_times > 30) {
                            recognize_times = 0;
//                            CloseLcdBackground();
                            vTaskDelay(pdMS_TO_TICKS(1000));
//                            Uart5_SendDeinitCameraMsg();
                            cmdCloseFaceBoardReq();//关主控电源
                            break;
                        }
//#endif
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
                //LOGD("[uart5_task]:receive return!\r\n");
                if (req_len != rcvlen) {
                    LOGD("[uart5_task]:part of data received, expect:%d received:%d!\r\n", req_len, rcvlen);
                    rx_status = UART5_RX_MSG_STATUS_WAITING_HEADER;


                } else if (UART5_RX_MSG_STATUS_WAITING_HEADER == rx_status) {
                    if (HEAD_MARK != recv_buffer[0]) {
                        LOGD("[uart5_task]:wrong header received, expect:0x%x received:0x%x!\r\n", HEAD_MARK,
                             recv_buffer[0]);
                        rx_status = UART5_RX_MSG_STATUS_WAITING_HEADER;
                    } else {
                        rx_status = UART5_RX_MSG_STATUS_WAITING_LENGTH;
                    }
                } else if (UART5_RX_MSG_STATUS_WAITING_LENGTH == rx_status) {
                    if (recv_buffer[2] > UART5_RX_MAX_DATA_PACKAGE_SIZE) {
                        LOGD("[uart5_task]:wrong msg length received, length:%d\r\n", recv_buffer[2]);
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
                LOGD("[uart5_task]:LPUART_RTOS_Receive error:%d sts:%d!\r\n", error, rx_status);
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