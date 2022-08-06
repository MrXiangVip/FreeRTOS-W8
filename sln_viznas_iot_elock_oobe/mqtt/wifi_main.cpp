#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <string.h>

#include "fsl_log.h"
#include "wifi_main.h"
//#include "mqtt_config.h"
#include "mqtt-common.h"
#include "config.h"
#include "mqtt_util.h"
#include "mqtt-api.h"
#include "mqtt-topic.h"

#include "commondef.h"
#include <ctype.h>
#include <stdio.h>
#include "MCU_UART5.h"
#include "MCU_UART5_Layer.h"
#include "cJSON.h"
#include "util.h"
#include "util_crc16.h"
#include "mqtt-instruction-pool.h"
#include "mqtt-mcu.h"
#include "DBManager.h"

#include "UART_FAKER.h"
#include "md5.h"
#include "base64.h"
#include "database.h"
#include "oasis.h"
#include "mqtt_dev_esp32.h"
#include "mqtt_conn_mgr.h"
#include "mqtt_cmd_mgr.h"
#include "mqtt_topic_mgr.h"
#include "mqtt_test_mgr.h"
#include "mqtt_feature_mgr.h"
#include "sln_cli.h"

#define MQTT_AT_LEN             128

int mqtt_init_done = 0;
// 当前pub执行优先级，0为最低优先级，9为最高优先级
int g_priority = 0;

bool bOasisRecordUpload = false;


MqttInstructionPool *mqtt_instruction_pool = MqttInstructionPool::getInstance();
int battery_level = -1;

#if (configSUPPORT_STATIC_ALLOCATION == 1)
DTC_BSS static StackType_t Uart8RecvStack[UART8RECVTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart8RecvTaskTCB;

DTC_BSS static StackType_t MqttTaskStack[MQTTTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_MqttTaskTCB;

DTC_BSS static StackType_t Uart8MsgHandleTaskStack[UART8MSGHANDLETASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart8MsgHandleTaskTCB;

DTC_BSS static StackType_t Uart8SendheartbeatTaskStack[UART8SENDHEARTBEATTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart8SendheartbeatTaskTCB;

DTC_BSS static StackType_t Uart8UploadDataTaskStack[UART8UPLOADDATATASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart8UploadDataTaskTCB;

DTC_BSS static StackType_t TestTaskStack[TESTTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_TestTaskTCB;
#endif

void remove_mqtt_instruction_from_pool(char instruction_dest, char cmd_code) {
    int cmd_index = MqttInstruction::getCmdIndex(instruction_dest, cmd_code);
    mqtt_instruction_pool->removeMqttInstruction(cmd_index);
}

int run_at_cmd(char const *cmd, int retry_times, int cmd_timeout_usec)
{
    int result = MqttDevEsp32::getInstance()->sendATCmd(cmd, cmd_timeout_usec, retry_times);
    return result;
}

int run_at_raw_cmd(char const *cmd, char *data, int data_len, int retry_times, int cmd_timeout_usec)
{
    int result = MqttDevEsp32::getInstance()->sendRawATCmd(cmd, data, data_len, cmd_timeout_usec, retry_times);
    return result;
}

static void mqtt_conn_task(void *pvParameters) {
    char const *logTag = "[UART8_WIFI]:mqtt_conn_task-";
    LOGD("%s start...\r\n", logTag);
    MqttConnMgr::getInstance()->keepConnectionAlive();
    // record error to flash
    // upload error to network
    // notify MCU to reset system
    vTaskDelete(NULL);
}

static void mqtt_recv_task(void *pvParameters)
{
    MqttDevEsp32::getInstance()->receiveMqtt();
    return;
}

int SendMsgToMQTT(char *mqtt_payload, int len) {
    vSetFakeCommandBuffer(mqtt_payload);
    return 0;
    // it fianlly execute doSendMsgToMQTT
}

int doSendMsgToMQTT(char *mqtt_payload, int len) {
    char mqtt_payload_str[MQTT_AT_LEN];
    memset(mqtt_payload_str, '\0', MQTT_AT_LEN);
    strncpy(mqtt_payload_str, mqtt_payload, len);
    char msg[MQTT_AT_LEN];
    memset(msg, '\0', MQTT_AT_LEN);
    char first[MQTT_AT_LEN];
    char *pub_topic = NULL;
    char pub_msg[MQTT_AT_LEN];
    memset(pub_msg, '\0', MQTT_AT_LEN);
    LOGD("--- mqttpayload len is %d\r\n", len);
    // 主动上报功能
    if (mqtt_payload_str != NULL && strncmp(mqtt_payload_str, DEFAULT_HEADER, DEFAULT_HEADER_LEN) == 0) {
        // 内部异步发布消息
        mysplit(mqtt_payload_str, first, msg, (char *)":");
        LOGD("=================== msg 1 is %s\r\n", msg);
        if (msg != NULL && strncmp(msg, "heartbeat", 9) == 0) {
            return 0;
		} else if (msg != NULL && strncmp(msg, "disconnect", 9) == 0) {
            int ret = disconnectMQTT(MQTT_LINK_ID_DEFAULT);
            return ret;
        } else {
            LOGD("--------- start send message %s\r\n", msg);
            pub_topic = get_pub_topic_cmd_res();
            LOGD("--------- pub to topic %s\r\n", pub_topic);
            int ret = quickPublishMQTTWithPriority(pub_topic, msg, 1);
            //freePointer(&pub_topic);
            return 0;
        }
    } else {
        char mqtt_msg_id[256];
        memset(mqtt_msg_id, '\0', 256);
        strcpy(mqtt_msg_id, mqtt_instruction_pool->getMsgId((char) (mqtt_payload[0]), (char) (mqtt_payload[1])));
        LOGD("out mqtt_msg_id is %s", mqtt_msg_id);
        if ((int) (char) (mqtt_payload[0]) == 0x24) {
            remove_mqtt_instruction_from_pool((char) (mqtt_payload[0]), (char) (mqtt_payload[1]));
            LOGD("in mqtt_msg_id is %s", mqtt_msg_id);
            pub_topic = get_pub_topic_cmd_res();
            LOGD("-------- pub_topic is %s\n", pub_topic);
            if ((int) (char) (mqtt_payload[1]) == 0x09 && (int) (char) (mqtt_payload[2]) == 0x01) {
                // 远程删除用户指令反馈
                // 远程删除用户成功
                if ((int) (char) (mqtt_payload[3]) == 0x00) {
                    printf("---- delete/clear user successfully\n");
                    // sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"result\":\"%d\"}", mqtt_msg_id, btWifiConfig.wifi_mac, 0);
                    sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"result\":\"%d\"}",
                            mqtt_msg_id, btWifiConfig.bt_mac, 0);
                } else {
                    // 远程删除用户失败
                    printf("---- delete/clear user fail\n");
                    // sprintf(pub_msg, "{\"msgId\":\"%s\",\"mqtt_payloadult\":\"%d\"}", mqtt_msg_id, 1);
                    // sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"result\":\"%d\"}", mqtt_msg_id, btWifiConfig.wifi_mac, 500);
                    sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"result\":\"%d\"}",
                            mqtt_msg_id, btWifiConfig.bt_mac, 1);
                }
                int ret = quickPublishMQTTWithPriority(pub_topic, pub_msg, 1);
                //freePointer(&pub_topic);
                return 0;
            } else {
            	LOGD("mqtt_payload unidentified");
            }
        } else if ((int) (char) (mqtt_payload[0]) == 0x23) {
            remove_mqtt_instruction_from_pool((char) (mqtt_payload[0]), (char) (mqtt_payload[1]));
            LOGD("in mqtt_msg_id is %s", mqtt_msg_id);
            pub_topic = get_pub_topic_cmd_res();
            LOGD("-------- pub_topic is %s\n", pub_topic);
            LOGD("---- from MCU\n");
			if ((int)(char)(mqtt_payload[1]) == 0x83 && (int)(char)(mqtt_payload[2]) == 0x01) {
                // 远程开锁指令反馈
                // 远程开锁成功
                if ((int) (char) (mqtt_payload[3]) == 0x00) {
                	LOGD("---- unlock door successfully\n");
                    // sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"result\":\"%d\"}", mqtt_msg_id, btWifiConfig.wifi_mac, 0);
                    sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"result\":\"%d\"}",
                            mqtt_msg_id, btWifiConfig.bt_mac, 0);
                } else {
                    // 远程开锁失败
                	LOGD("---- unlock door fail\n");
                    // sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"result\":\"%d\"}", mqtt_msg_id, btWifiConfig.wifi_mac, 500);
                    sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"result\":\"%d\"}",
                            mqtt_msg_id, btWifiConfig.bt_mac, 1);
                }
                int ret = quickPublishMQTTWithPriority(pub_topic, pub_msg, 1);
                //freePointer(&pub_topic);
                return 0;
            } else if ((int) (char) (mqtt_payload[1]) == 0x15 && (int) (char) (mqtt_payload[2]) == 0x01) {
                // 远程设置合同时间指令反馈
                // 远程设置合同时间成功
                if ((int) (char) (mqtt_payload[3]) == 0x00) {
                	LOGD("---- setup contract successfully\n");
                    // sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"result\":\"%d\"}", mqtt_msg_id, btWifiConfig.wifi_mac, 0);
                    sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"result\":\"%d\"}",
                            mqtt_msg_id, btWifiConfig.bt_mac, 0);
                } else {
                    // 远程设置合同时间失败
                	LOGD("---- setup contract fail\n");
                    // sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"result\":\"%d\"}", mqtt_msg_id, btWifiConfig.wifi_mac, 500);
                    sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"result\":\"%d\"}",
                            mqtt_msg_id, btWifiConfig.bt_mac, 1);
                }
                int ret = quickPublishMQTTWithPriority(pub_topic, pub_msg, 1);
                LOGD("------- before sendStatus ToMCU ret %d\n", ret);
                // 远程设置合同时间成功，并不能立即要求下电，有可能要等待其他指令，比如远程开锁
                // sendStatusToMCU(0x04, ret);
                //freePointer(&pub_topic);
                return 0;
            } else if ((int) (char) (mqtt_payload[1]) == CMD_MQTT_UPLOAD) {
                // 注册/开门记录
                int id = (int) (mqtt_payload[3]);
                // TODO:
                LOGD("请求MQTT 上传记录 start id %d \r\n", id);
                Record record;
                int ret = DBManager::getInstance()->getRecordByID(id, &record);
                if (ret == 0) {
                	LOGD("register/unlcok record is not NULL start upload record/image \r\n");
#if REMOTE_FEATURE != 0
                    //char *msgId = gen_msgId();
//					ret = doFeatureUpload(msgId, record.UUID);
					ret = MqttFeatureMgr::getInstance()->uploadFeature(record.UUID);
#endif
                    if( record.upload == BOTH_UNUPLOAD ){//有可能被当做历史记录先上传,先检测防止重复上传
                        ret = MqttCmdMgr::getInstance()->pushRecord(&record, CMD_TYPE_RECORD_TEXT, PRIORITY_HIGH, 1);
                        ret = MqttCmdMgr::getInstance()->pushRecord(&record, CMD_TYPE_RECORD_IMAGE, PRIORITY_HIGH, 1);
                    }else{
                        LOGD("实时记录已经被上传  \r\n");
                    }
                } else {
                	LOGD("register/unlcok record is NULL");
                }
            } else if ((int) (char) (mqtt_payload[1]) == CMD_OPEN_LOCK_REC && (int) (char) (mqtt_payload[2]) == CMD_WIFI_SSID) {
                // 开门记录上报指令
                if ((int) (char) (mqtt_payload[2]) == 0x0f) {
                	LOGD("unlock record upload to server\r\n");
                    char uid[17];
                    HexToStr(uid, (unsigned char *) (&mqtt_payload[3]), 8);
                    LOGD("unlock record uid %s\r\n", uid);
                    int unlockType = (int) (char) mqtt_payload[11];
                    unsigned int unlockTime = (unsigned int) StrGetUInt32((unsigned char *) (&mqtt_payload[12]));
                    int batteryLevel = (int) mqtt_payload[16];
                    int unlockStatus = (int) (char) mqtt_payload[17];
                    LOGD("unlock record uid %s, unlockType %d, unlockTime %u, batteryLevel %d, unlockStatus %d\r\n",
                             uid, unlockType, unlockTime, batteryLevel, unlockStatus);
                    // sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"userId\":\"%s\", \"unlockMethod\":%d,\"unlockTime\":%u,\"batteryPower\":%d,\"unlockState\":%d}", gen_msgId(), btWifiConfig.wifi_mac, uid, unlockType, unlockTime, batteryLevel, unlockStatus);
                    sprintf(pub_msg,
                            "{\"msgId\":\"%s\",\"mac\":\"%s\",\"userId\":\"%s\", \"unlockMethod\":%d,\"unlockTime\":%u,\"batteryPower\":%d,\"unlockState\":%d}",
                            gen_msgId(), btWifiConfig.bt_mac, uid, unlockType, unlockTime, batteryLevel, unlockStatus);
                    pub_topic = get_pub_topic_record_request();
                    int ret = quickPublishMQTTWithPriority(pub_topic, pub_msg, 1);
                    //freePointer(&pub_topic);
                    return ret;
                } else {
                    LOGD("unlock record length is %d instead of 15\r\n", (int) (char) (mqtt_payload[2]));
                }
            } else if ((int) (char) (mqtt_payload[1]) == CMD_MECHANICAL_LOCK) {
                // 机械开锁记录
                int id = (int) (mqtt_payload[3]);
                // TODO:
                LOGD("DBManager::getInstance()->getRecord start id %d\r\n", id);
                Record record;
                int ret = DBManager::getInstance()->getRecordByID(id, &record);
                if (ret == 0) {
                	LOGD("mechnical record is not NULL start upload record/image\r\n");
                    int ret = MqttCmdMgr::getInstance()->pushRecord(&record, CMD_TYPE_RECORD_TEXT, PRIORITY_HIGH, 1);
                    ret = MqttCmdMgr::getInstance()->pushRecord(&record, CMD_TYPE_RECORD_IMAGE, PRIORITY_HIGH, 1);
                    // TODO:
                } else {
                	LOGD("mechnical record is NULL");
                }
            } else {
            }
        }
        return -1;
    }

}

static void mqtt_send_task(void *pvParameters)
{
    char const *logTag = "[UART8_WIFI]:mqtt_send_task-";
    LOGD("%s start...\r\n", logTag);
    MqttCmdMgr::getInstance()->loopSendMqttMsgs();
    vTaskDelete(NULL);
}

static void test_task(void *pvParameters)
{
    char const *logTag = "[UART8_WIFI]:mqtt_send_task-";
    LOGD("%s start...\r\n", logTag);
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (g_test_argc > 1) {
            for (int i = 0; i < g_test_argc; i++) {
                LOGD("test argv %d is %s\r\n", i, g_test_argv[i]);
            }
            MqttTestMgr::getInstance()->doTest(g_test_argc, g_test_argv[1], g_test_argv[2], g_test_argv[3]);
            reset_test_args();
        }
    }
    vTaskDelete(NULL);
}

static void uploaddata_task(void *pvParameters)
{
    char const *logTag = "[UART8_WIFI]:uploaddata_task-";
    LOGD("%s start...\r\n", logTag);
    MqttCmdMgr::getInstance()->uploadRecords();
    vTaskDelete(NULL);
    LOGD("\r\n%s end...\r\n", logTag);
}

int WIFI_Start()
{
	char const *logTag = "[UART8_WIFI]:main-";
//#if !MQTT_SUPPORT
//    LOGD("%s stopped...\r\n", logTag);
//	return 0;
//#endif
    LOGD("%s starting...\r\n", logTag);

    MqttDevEsp32::getInstance()->initUart();

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(mqtt_conn_task, "mqtt_conn_task", MQTTTASK_STACKSIZE, NULL, MQTTTASK_PRIORITY,
                                        MqttTaskStack, &s_MqttTaskTCB))
#else
    if (xTaskCreate(mqtt_conn_task, "mqtt_conn_task", MQTTTASK_STACKSIZE, NULL, MQTTTASK_PRIORITY, NULL) != pdPASS)
#endif
    {
        LOGD("%s failed to create mqtt_conn_task\r\n", logTag);
        while (1);
    }
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(mqtt_recv_task, "mqtt_recv_task", UART8RECVTASK_STACKSIZE, NULL, UART8RECVTASK_PRIORITY,
                                            Uart8RecvStack, &s_Uart8RecvTaskTCB))
#else
    if (xTaskCreate(uartrecv_task, "mqtt_recv_task", UART8RECVTASK_STACKSIZE, NULL, UART8RECVTASK_PRIORITY, NULL) != pdPASS)
#endif
    {
        LOGD("%s failed to create mqtt_recv_task\r\n", logTag);
        while (1);
    }
#ifndef TIMEOUT_TEST
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(mqtt_send_task, "mqtt_send_task", UART8MSGHANDLETASK_STACKSIZE, NULL, UART8MSGHANDLETASK_PRIORITY,
                                            Uart8MsgHandleTaskStack, &s_Uart8MsgHandleTaskTCB))
#else
    if (xTaskCreate(mqtt_send_task, "mqtt_send_task", UART8RECVTASK_STACKSIZE, NULL, UART8MSGHANDLETASK_PRIORITY, NULL) != pdPASS)
#endif
    {
    	LOGD("%s failed to create mqtt_send_task\r\n", logTag);
    	while (1);
    }
#endif

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(uploaddata_task, "uploaddata_task", UART8UPLOADDATATASK_STACKSIZE, NULL, UART8UPLOADDATATASK_PRIORITY,
                                            Uart8UploadDataTaskStack, &s_Uart8UploadDataTaskTCB))
#else
    if (xTaskCreate(uploaddata_task, "uploaddata_task", UART8UPLOADDATATASK_STACKSIZE, NULL, UART8UPLOADDATATASK_PRIORITY, NULL) != pdPASS)
#endif
    {
    	LOGD("%s failed to create uploaddata_task\r\n", logTag);
    	while (1);
    }

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(test_task, "test_task", TESTTASK_STACKSIZE, NULL, TESTTASK_PRIORITY,
                                            TestTaskStack, &s_TestTaskTCB))
#else
    if (xTaskCreate(test_task, "test_task", TESTTASK_STACKSIZE, NULL, TESTTASK_PRIORITY, NULL) != pdPASS)
#endif
    {
        LOGD("%s failed to create test_task\r\n", logTag);
        while (1);
    }

    LOGD("%s started...\r\n", logTag);
    return 0;
}
