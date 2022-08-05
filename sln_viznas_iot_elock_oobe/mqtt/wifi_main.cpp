#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <string.h>

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_lpuart_freertos.h"
#include "fsl_lpuart.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_log.h"
#include "wifi_main.h"
//#include "mqtt_config.h"
#include "../mqtt/mqtt-common.h"
#include "../mqtt/config.h"
#include "../mqtt/mqtt_util.h"
#include "../mqtt/mqtt-api.h"
#include "../mqtt/mqtt-topic.h"
#include "../mqtt/mqtt-ota.h"
#include "../mqtt/mqtt-ff.h"
#include "../mqtt/mqtt-remote-feature.h"

#include "commondef.h"
#include <ctype.h>
#include <stdio.h>
#include "MCU_UART5.h"
#include "../wave/MCU_UART5_Layer.h"
#include "cJSON.h"
#include "util.h"
#include "util_crc16.h"
#include "../mqtt/base64.h"
#include "../mqtt/mqtt-instruction-pool.h"
#include "../mqtt/mqtt-mcu.h"
#include "../db/DBManager.h"

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
#include "sln_cli.h"


#define MQTT_AT_LEN             128

int mqtt_init_done = 0;
// 命令是否已经执行完成
int g_command_executed = 0;
// 当前pub执行优先级，0为最低优先级，9为最高优先级
int g_priority = 0;


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

extern bool bPubOasisImage;
extern int boot_mode;
extern int receive_boot_mode;
bool bOasisRecordUpload = false;
extern bool g_is_save_file;
extern int pressure_test;

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

// xshx TODO: 获取特征值
int getFeatureData(char *uuid, char featureData[]) {
    int featureLen = getOasisFeatureSize();
    LOGD("--- get Feature Data size %d\r\n", featureLen);

    float feature[400];
    memset(feature, '\0', sizeof(feature));
    int res = DB_GetFeature_ByName(uuid, feature);
    LOGD("--- DB_GetFeature_ByName res %d\r\n", res);
    uint8_t *feature_hex_str = (uint8_t *)pvPortMalloc(featureLen * 2 + 1);
    feature_hex_str[featureLen * 2] = '\0';
    Remote_convertInt2ascii(feature, featureLen, feature_hex_str);
    LOGD("feature str %s \r\n",feature_hex_str);
    vPortFree(feature_hex_str);

//    char *featureP = (char*)feature;
    memcpy(featureData, (char*)feature, featureLen);
    return featureLen;
//    uint8_t* feature_hex_str                       = (uint8_t *)pvPortMalloc(OASISLT_getFaceItemSize() * 2 + 1);
//    feature_hex_str[OASISLT_getFaceItemSize() * 2] = '\0';
//
//    char featureData1[800];
//    HexToStr(featureData1, (const unsigned char*)feature, 100);
//    LOGD("--- featureData is %s\r\n", featureData1);

//    strcpy(featureData, "123456");
//    return strlen(featureData);
}

int getFeatureJson(char *msgId, char *uuid, char featureJson[]) {
    char featureData[400];
    memset(featureData, '\0', sizeof(featureData));
    int featureSize = getFeatureData(uuid, featureData);
    LOGD("--- featureData %s len is %d\r\n", featureData, featureSize);
    char featureBase64[1000];
    base64_encode(featureData, featureBase64, featureSize);
    LOGD("--- featureBase64 %s len is %d\r\n", featureBase64, strlen(featureBase64));

    // verify sign
    unsigned char md5_value[MD5_SIZE];
    unsigned char md5_str[MD5_STR_LEN + 1];
    MD5_CTX md5;
    MD5Init(&md5);
    MD5Update(&md5, (unsigned char*)featureData, featureSize);
    MD5Final(&md5, md5_value);
    for(int i = 0; i < MD5_SIZE; i++)
    {
        snprintf((char*)md5_str + i*2, 2+1, "%02x", md5_value[i]);
    }
    md5_str[MD5_STR_LEN] = '\0'; // add end

//    sprintf(featureJson,
//            "{\"msgId\":\"%s\",\"u\":\"%s\",\"s\":%s,\"l\":%d,\"d\":\"%s\"}",
//            msgId, uuid, md5_str, featureSize, featureBase64);
    sprintf(featureJson,
            "{\"msgId\":\"%s\",\"u\":\"%s\",\"s\":%s,\"l\":%d,\"d\":\"%s\"}",
            msgId, uuid, md5_str, featureSize, featureBase64);
    LOGD("--- featureJson %d %s\r\n", strlen(featureJson), featureJson);
    return strlen(featureJson);
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
        } else if (msg != NULL && strncmp(msg, CMD_FEATURE_UP, strlen(CMD_FEATURE_UP)) == 0) {
            mysplit(msg, first, pub_msg, ":");
            char msgId[64];
            char uuid[32];
            memset(msgId, '\0', sizeof(msgId));
            memset(uuid, '\0', sizeof(uuid));
            mysplit(pub_msg, msgId, uuid, ":");
            LOGD("---- send face of %s with msgId %s\r\n", uuid, msgId);
            char *pub_topic_feature_up = get_pub_topic_feature_upload();
//            int ret = quickPublishMQTTWithPriority(pub_topic_feature_up, pub_msg, 1);
            char *mymsg="123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
            char featureJson[1200];
            memset(featureJson, '\0', sizeof(featureJson));
            int len = getFeatureJson(msgId, uuid, featureJson);
            // xshx TODO: 获取特征值
//            int featureLen = getFeatureByUUID(char *uuid, &featureData);
            // TODO: base64
            // TODO: do sign with md5
            // TODO: set data with format {"msgId": msgId, "u": uuid, "s": sign, "d": base64}

            int ret = quickPublishRawMQTT(pub_topic_feature_up, featureJson, strlen(featureJson));
//            int ret = quickPublishMQTT(pub_topic_feature_up, "123456");
            //freePointer(&pub_topic_ota_request);
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
			if ((int)(char)(mqtt_payload[1]) == 0x82 && (int)(char)(mqtt_payload[2]) == 0x01 &&(int)(char)(mqtt_payload[3]) == 0x01) {
				//freePointer(&pub_topic);
				pub_topic = get_pub_topic_heart_beat();
				// 远程开锁模式心跳上报
				LOGD("---- remote unlock door mode");
				char *msgId = gen_msgId();
				if (versionConfig.sys_ver!= NULL && strlen(versionConfig.sys_ver) > 0) {
//					sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"wifi_rssi\":%s,\"battery\":%d,\"index\":%d,\"version\":\"%s\"}", msgId, btWifiConfig.bt_mac, wifi_rssi, battery_level, -1, versionConfig.sys_ver);
                    sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"wifi_rssi\":%s,\"battery\":%d,\"index\":%d,\"version\":\"%s\"}", msgId, btWifiConfig.bt_mac, 0, battery_level, -1, versionConfig.sys_ver);
				} else {
//					sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"wifi_rssi\":%s,\"battery\":%d,\"index\":%d,\"version\":\"%s\"}", msgId, btWifiConfig.bt_mac, wifi_rssi, battery_level, -1, getFirmwareVersion());
                    sprintf(pub_msg, "{\"msgId\":\"%s\",\"mac\":\"%s\",\"wifi_rssi\":%s,\"battery\":%d,\"index\":%d,\"version\":\"%s\"}", msgId, btWifiConfig.bt_mac, 0, battery_level, -1, getFirmwareVersion());
				}
				freePointer(&msgId);
				pub_topic = get_pub_topic_heart_beat();
				int ret = quickPublishMQTTWithPriority(pub_topic, pub_msg, 1);
				//freePointer(&pub_topic);
				return 0;
			} else if ((int)(char)(mqtt_payload[1]) == 0x83 && (int)(char)(mqtt_payload[2]) == 0x01) {
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
                // notifyCommandExecuted(ret);
                g_command_executed = 1;
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
                    char *msgId = gen_msgId();
					ret = doFeatureUpload(msgId, record.UUID);
#endif
                    if( record.upload == BOTH_UNUPLOAD ){//有可能被当做历史记录先上传,先检测防止重复上传
                        ret = MqttCmdMgr::getInstance()->pushRecord(&record, CMD_TYPE_RECORD_TEXT, PRIORITY_HIGH, 1);
                        ret = MqttCmdMgr::getInstance()->pushRecord(&record, CMD_TYPE_RECORD_IMAGE, PRIORITY_HIGH, 1);
                    }else{
                        LOGD("实时记录已经被上传  \r\n");
                    }
                    g_command_executed = 1;
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
                    // notifyCommandExecuted(ret);
                    g_command_executed = 1;
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
                    g_command_executed = 1;
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

void testFeatureDownload(char *featureJson) {
    char msgId[MQTT_AT_LEN];
    memset(msgId, '\0', MQTT_AT_LEN);
//    LOGD("testFeatureDownload %s\r\n", featureJson);
    LOGD("testFeatureDownload %d\r\n", strlen(featureJson));
    int ret = analyzeRemoteFeature(featureJson, (char*)&msgId);
    LOGD("testFeatureDownload ret %d\r\n", ret);
    return;
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
