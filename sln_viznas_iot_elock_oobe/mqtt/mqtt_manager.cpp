//
// Created by wszgx on 2022/7/29.
//

#include <stdio.h>

#include "aw_wstime.h"
#include "mqtt_manager.h"
#include "mqtt-common.h"
#include "mqtt_util.h"
#include "fsl_log.h"
#include "mqtt-topic.h"
#include "cJSON.h"
#include "mqtt-mcu.h"
#include "mqtt_topic_mgr.h"
#include "mqtt_cmd_mgr.h"
#include "mqtt_mcu_mgr.h"
#include "mqtt_feature_mgr.h"
#include "mqtt-remote-feature.h"
#include "base64.h"
#include "util.h"
#include "MCU_UART5.h"
#include "MCU_UART5_Layer.h"
#include "DBManager.h"
#include "sln_api.h"

// 分析最原始的MQTT AT消息
int MqttManager::analyzeMqttRecvLine(char *msg) {
    static int ana_timeout_count = 0;
    int result = -1;
    char *pNext;

    pNext = (char *)strstr(msg,"+MQTTSUBRECV:"); //必须使用(char *)进行强制类型转换(虽然不写有的编译器中不会出现指针错误)
    if (pNext != NULL) {
        LOGD("mqtt sub recv\r\n");

        // TODO: 只要服务器下发指令下来，我们就认为服务器已经确认在线
//        g_is_online = 1;
// TODO: WifiState->setState(MQTT_CONNECTED);
        char firstWithLinkId[MQTT_AT_LEN];
		char topic[MQTT_AT_LEN];
		char dataLen[MQTT_AT_LEN];
		char data[UART_RX_BUF_LEN];
		char lastWithTopic[UART_RX_BUF_LEN];
		char lastWithDataLen[UART_RX_BUF_LEN];
        mysplit(msg, firstWithLinkId, lastWithTopic, (char *)",\"");
        mysplit(lastWithTopic, topic, lastWithDataLen, (char *)"\",");
        mysplit(lastWithDataLen, dataLen, data, (char *)",");
        if (dataLen == NULL || data == NULL) {
            LOGD("analyzeMQTTMsgInternal dataLen is NULL or data is is NULL");
            return false;
        }
        int data_len = atoi(dataLen);

        if (data != NULL) {
            if (strlen(data) >= data_len) {
                LOGD("analyze topic %s data %s\r\n", topic, data);
                // 请求上传feature
                char *fr_topic = MqttTopicMgr::getInstance()->getSubTopicFeatureRequest();
                if (strncmp(topic, fr_topic, strlen(fr_topic)) == 0) {
                    LOGD("do feature upload request\r\n");
                    if (data[0] == '{' && data[data_len - 1] == '}') {
                        int result = MqttFeatureMgr::getInstance()->requestFeature(data);
                        return 0;
                    } else {
                        LOGE("fr data is not formatted with JSON\r\n");
                        MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_ERROR, NULL, get_short_str(data));
                        return -1;
                    }
                }

                // 下载feature到设备
                char *fd_topic = MqttTopicMgr::getInstance()->getSubTopicFeatureDownload();
                if (strncmp(topic, fd_topic, strlen(fd_topic)) == 0) {
                    LOGD("do feature download\r\n");
                    if (data[0] == '{' && data[data_len - 1] == '}') {
                        int result = MqttFeatureMgr::getInstance()->downloadFeature(data);
                        return 0;
                    } else {
                        LOGE("fd data is not formatted with pure JSON\r\n");
                        MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_ERROR, NULL, get_short_str(data));
                        return -1;
                    }
                }

                char *cr_topic = MqttTopicMgr::getInstance()->getSubTopicCmdRequest();
                if (strncmp(topic, cr_topic, strlen(cr_topic)) == 0) {
                    LOGD("do cmd request\r\n");
                    if (data[0] == '{' && data[data_len - 1] == '}') {
                        int result = handleMqttMsgData(data);
                        return 0;
                    } else {
                        LOGE("cr data is not formatted with pure JSON\r\n");
                        MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_ERROR, NULL, get_short_str(data));
                        return -1;
                    }
                }

                LOGD("analyze topic %s is not supported\r\n", topic);
                // TODO: may need to transfer " to \" for getShortData to send the MQTT msg
                MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_NOT_SUPPORT, NULL, get_short_str(data));
                return -1;
            } else {
                LOGD("analyze topic %s supposed at data_len is %d is greater than strlen(data) is %d\r\n", topic, data_len, strlen(data));
                char *msgId = MqttCmdMgr::getInstance()->genMsgId();
                MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_ERROR, NULL, get_short_str(data));
                return -1;
            }
        }
    } else {
        LOGE("[ERROR] not mqtt sub recv, should not get here\r\n");
        return -1;
    }
}

// 解包之后的MQTT AT数据包
int MqttManager::handleMqttMsgData(char *jsonMsg) {
    cJSON *mqtt = NULL;
    cJSON *msg_id = NULL;
    cJSON *cmd_type = NULL;
    cJSON *cmd_data = NULL;
    cJSON *timestamp = NULL;
    char *idStr = NULL;
    char *typeStr = NULL;
    char *dataStr = NULL;
    char *tsStr = NULL;
    int result = 0;

    mqtt = cJSON_Parse(jsonMsg);
    msg_id = cJSON_GetObjectItem(mqtt, "id");
    idStr = cJSON_GetStringValue(msg_id);
    cmd_type = cJSON_GetObjectItem(mqtt, "ct");
    typeStr = cJSON_GetStringValue(cmd_type);
    cmd_data = cJSON_GetObjectItem(mqtt, "cd");
    dataStr = cJSON_GetStringValue(cmd_data);
    if (idStr == NULL || typeStr == NULL || dataStr == NULL) {
        // TODO: invalid command
        MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_NOT_SUPPORT, (idStr == NULL || strlen(idStr) == 0 ? MqttCmdMgr::getInstance()->genMsgId() : idStr), "Paramater Missing");
        return -1;
    }
    LOGD("MQTT MSG: id: %s, ct: %s, cd: %s\r\n", idStr, typeStr, get_short_str(dataStr));
    if (strcmp("pt", typeStr) == 0) {
        result = handlePassThroughPayload(dataStr, idStr);
    } else if (strcmp("ts", typeStr) == 0) {
        result = timeSync(dataStr);
        MqttCmdMgr::getInstance()->atCmdResponse(result, idStr);
    } else if (strcmp("du", typeStr) == 0) {
        DBManager::getInstance()->deleteRecordByUUID(dataStr);
        UserExtendManager::getInstance()->delUserJsonByUUID( dataStr );
        vizn_api_status_t status = VIZN_DelUser(NULL, dataStr);
        result =AT_RSP_SUCCESS;
        MqttCmdMgr::getInstance()->atCmdResponse(result, idStr);
    } else if (strcmp("cu", typeStr) == 0) {
        DBManager::getInstance()->clearRecord();
        UserExtendManager::getInstance()->clearAllUserJson();
        VIZN_DelUser(NULL);
        result =AT_RSP_SUCCESS;
        MqttCmdMgr::getInstance()->atCmdResponse(result, idStr);
    } else if (strcmp("ua", typeStr) == 0) {
        // TODO:
        LOGD("dataStr: %s \r\n", dataStr);
        UserExtendManager::getInstance()->addStrUser(dataStr);
        result = AT_RSP_SUCCESS;
        MqttCmdMgr::getInstance()->atCmdResponse(result, idStr);

    } else if (strcmp("um", typeStr) == 0) {
        // xshx TODO:
//        result = DBManager::getInstance()->setUserMode(dataStr);
        result = AT_RSP_SUCCESS;
        MqttCmdMgr::getInstance()->atCmdResponse(result, idStr);
    } else if (strcmp("fr", typeStr) == 0) {
        // Feature request
        MqttCmdMgr::getInstance()->requestFeature(dataStr);
    } else {
        MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_NOT_SUPPORT, idStr, "Command Type Invalid");
        result = -1;
    }
    if (mqtt != NULL) {
        cJSON_Delete(mqtt);
        mqtt = NULL;
    }
    return result;
}

// 透传指令解析
int MqttManager::handlePassThroughPayload(char *payload, char *idStr) {
    if (payload != NULL) {
        int payload_len = strlen(payload);
        int payload_bin_len = (payload_len / 4 - 1) * 3 + 1;
        char payload_bin[MQTT_AT_LEN];
        int ret1 = base64_decode(payload, payload_bin);
        //LOGD("decode payload_bin_len is %d ret1 is %d", payload_bin_len, ret1);

        char payload_str[MQTT_AT_LEN];
        HexToStr(payload_str, reinterpret_cast<unsigned char*>(&payload_bin), payload_bin_len);
        LOGD("decode payload_bin<len:%d %s> ret1 is %d\r\n", payload_bin_len, payload_str, ret1);

        // TODO:
        unsigned char x7_cmd = payload_bin[0];
        unsigned char x7_cmd_code = payload_bin[1];
        int timeout = 5;
        if (x7_cmd == 0x23 && x7_cmd_code == 0x83) {
            // 远程开门
            timeout = 25;
        } else if (x7_cmd == 0x23 && x7_cmd_code == 0x8a) {
            // 时间同步
            int len = (int) (char) payload_bin[2];
            if (len != 10) {
                return AT_RSP_ERROR;
            }
            char tsStr[11];
            memset(tsStr, '\0', 11);
            strncpy(tsStr, &payload_bin[3], 10);
            // 时间同步
            LOGD("--- timestamp is %s len is %d\n", tsStr, strlen(tsStr));
            if (tsStr != NULL && strlen(tsStr) > 0) {
                int result = timeSync(tsStr);
                MqttCmdMgr::getInstance()->atCmdResponse(result, idStr, "Error: Resource Busy");
                return result;
            } else {
                MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_ERROR, idStr, "Error: Time Sync");
                return AT_RSP_ERROR;
            }
        }
        MqttInstruction instruction(x7_cmd, x7_cmd_code, timeout, idStr);
        int ret = MqttInstructionPool::getInstance()->insertMqttInstruction(instruction);
        if (ret == -1) {
            MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_BUSY, idStr, "Error: Resource Busy");
            return AT_RSP_BUSY;
        }
        if (x7_cmd == 0x24) {
            //MessageSend(1235, payload_bin, ret1);
            // TODO:
            SendMsgToSelf((unsigned char *)payload_bin, payload_bin_len);
        } else {
            //MessageSend(1234, payload_bin, ret1);
            // TODO:
            SendMsgToMCU((unsigned char *)payload_bin, payload_bin_len);
        }
        return AT_RSP_SUCCESS;
    } else {
        return AT_RSP_ERROR;
    }
}

int MqttManager::timeSync(char *ts) {
    if (ts != NULL && strlen(ts) > 0) {
        int currentSec = atoi(ts);
        if (currentSec > 1618965299) {
            LOGD("__network time sync networkTime is %d can setTimestamp\r\n", currentSec);
            int result = MqttMcuMgr::getInstance()->syncTimeToMCU(ts);
            if (result == 0) {
                setTimestamp(currentSec);
                ws_systime = currentSec;
                return AT_RSP_SUCCESS;
            }
            return AT_RSP_ERROR;
        } else {
            LOGD("__network time sync networkTime is %d don't setTimestamp\r\n", currentSec);
            return AT_RSP_ERROR;
        }
    }
    return AT_RSP_ERROR;
}

