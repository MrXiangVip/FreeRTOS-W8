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

char jsonData[UART_RX_BUF_LEN];
int MqttManager::analyzeMqttMsg(char *msg) {
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
//                        int result = requestFeatureUpload(g_rfd_data, (char*)&msgId);
                        return 0;
                    } else {
                        LOGE("fr data is not formatted with JSON\r\n");
                        MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_ERROR, get_short_str(data));
                        return -1;
                    }
                }

                // 下载feature到设备
                char *fd_topic = MqttTopicMgr::getInstance()->getSubTopicFeatureDownload();
                if (strncmp(topic, fd_topic, strlen(fd_topic)) == 0) {
                    LOGD("do feature download\r\n");
                    if (data[0] == '{' && data[data_len - 1] == '}') {
//                        int result = requestFeatureUpload(g_rfd_data, (char*)&msgId);
                        return 0;
                    } else {
                        LOGE("fd data is not formatted with JSON\r\n");
                        MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_ERROR, get_short_str(data));
                        return -1;
                    }
                }

                char *cr_topic = MqttTopicMgr::getInstance()->getSubTopicCmdRequest();
                if (strncmp(topic, cr_topic, strlen(cr_topic)) == 0) {
                    LOGD("do cmd request\r\n");
                    if (data[0] == '{' && data[data_len - 1] == '}') {
//                        int result = handleJsonMsg(g_rfd_data, (char*)&msgId);
                        return 0;
                    } else {
                        LOGE("cr data is not formatted with JSON\r\n");
                        MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_ERROR, get_short_str(data));
                        return -1;
                    }
                }

                LOGD("analyze topic %s is not supported\r\n", topic);
                MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_NOT_SUPPORT, get_short_str(data));
                return -1;
            } else {
                LOGD("analyze topic %s supposed at data_len is %d is greater than strlen(data) is %d\r\n", topic, data_len, strlen(data));
                MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_ERROR, get_short_str(data));
                return -1;
            }
        }
    } else {
        LOGE("[ERROR] not mqtt sub recv, should not get here\r\n");
        return -1;
    }
}

int MqttManager::handleJsonMsg(char *jsonMsg) {
    cJSON *mqtt = NULL;
    cJSON *msg_id = NULL;
    cJSON *data = NULL;
    cJSON *timestamp = NULL;
    char *msg_idStr = NULL;
    char *dataStr = NULL;
    char *tsStr = NULL;

    mqtt = cJSON_Parse(jsonMsg);
    msg_id = cJSON_GetObjectItem(mqtt, "msgId");
    msg_idStr = cJSON_GetStringValue(msg_id);

    int result = 0;

    data = cJSON_GetObjectItem(mqtt, "data");
    if (data != NULL) {
        dataStr = cJSON_GetStringValue(data);
        LOGD("---JSON data is %s\r\n", dataStr);
        if ((dataStr != NULL) && (strlen(dataStr) > 0)) {
            // 来源于服务器
            if (strncmp(dataStr, "sf:", 3) == 0) {
                if (strncmp(dataStr, "sf:nodata", 9) == 0) {
                    LOGD("no more data to download from server by server");
                } else {
                    char pub_msg[80];
                    memset(pub_msg, '\0', 80);
                    sprintf(pub_msg, "%s{\"msgId\":\"%s\",\"result\":4}", DEFAULT_HEADER, msg_idStr);
                    // NOTE: 此处必须异步操作
                    // TODO:
//                    SendMsgToMQTT(pub_msg, strlen(pub_msg));
                }
            } else {
                // TODO:
//                result = handlePayload(dataStr, msg_idStr);
            }
        }
    }

    timestamp = cJSON_GetObjectItem(mqtt, "time");
    if (timestamp != NULL) {
        tsStr = cJSON_GetStringValue(timestamp);
        LOGD("--- timestamp is %s len is %d\n", tsStr, strlen(tsStr));
        if (tsStr != NULL && strlen(tsStr) > 0) {

            int currentSec = atoi(tsStr);
            if (currentSec > 1618965299) {
                LOGD("__network time sync networkTime is %d can setTimestamp\n", currentSec);
                setTimestamp(currentSec);
                //mqtt_log_init();
            } else {
                LOGD("__network time sync networkTime is %d don't setTimestamp\n", currentSec);
            }

            result = syncTimeToMCU(tsStr);
        }
    }
    if (mqtt != NULL) {
        cJSON_Delete(mqtt);
        mqtt = NULL;
    }
    return result;
}

