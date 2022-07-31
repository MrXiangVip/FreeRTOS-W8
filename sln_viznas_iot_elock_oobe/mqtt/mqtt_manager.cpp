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

char jsonData[UART_RX_BUF_LEN];
int MqttManager::analyzeMqttMsg(char *msg) {
    static int ana_timeout_count = 0;
    int result = -1;
    char *pNext;

    pNext = (char *)strstr(msg,"+MQTTSUBRECV:"); //必须使用(char *)进行强制类型转换(虽然不写有的编译器中不会出现指针错误)
    if (pNext != NULL) {
        LOGD("---- it is sub recv\r\n");

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
                // 请求上传feature
                char *rfr_topic= get_sub_topic_feature_request();
                LOGD("analyze rfr_topic %s topic %s\r\n", rfr_topic, topic);
                if (strncmp(topic, rfr_topic, strlen(rfr_topic)) == 0) {
                    if (data[0] == '{') {
                        memset(jsonData, '\0', sizeof(jsonData));
                        strcpy(jsonData, data);
                        LOGD("analyzeRemoteFeature g_rfd_data %s", jsonData);
                    } else {
                        int len = strlen(jsonData);
                        strcpy(&jsonData[len], data);
                        LOGD("analyzeRemoteFeature g_rfd_data %s", jsonData);
                    }
                    if (data[data_len - 1] == '}') {
                        // log_info("get rfd data %d: %s", strlen(g_rfd_data), g_rfd_data);
                        char msgId[MQTT_AT_LEN];
                        memset(msgId, '\0', MQTT_AT_LEN);
                        LOGD("requestFeatureUpload g_rfd_data %s", jsonData);
//                        int ret = requestFeatureUpload(g_rfd_data, (char*)&msgId);
//                        LOGD("requestFeatureUpload ret %d", ret);
                    }
                    freePointer(&rfr_topic);
                    return 0;
                }

                // 下载feature到设备
                char *rfd_topic= get_sub_topic_feature_download();
                LOGD("analyze rfd_topic %s topic %s\r\n", rfd_topic, topic);
				if (strncmp(topic, rfd_topic, strlen(rfd_topic)) == 0) {
					if (data[0] == '{') {
						memset(jsonData, '\0', sizeof(jsonData));
						strcpy(jsonData, data);
                        LOGD("analyzeRemoteFeature g_rfd_data %s", jsonData);
					} else {
						int len = strlen(jsonData);
						strcpy(&jsonData[len], data);
                        LOGD("analyzeRemoteFeature g_rfd_data %s", jsonData);
					}
					if (data[data_len - 1] == '}') {
						// log_info("get rfd data %d: %s", strlen(g_rfd_data), g_rfd_data);
						char msgId[MQTT_AT_LEN];
						memset(msgId, '\0', MQTT_AT_LEN);
                        LOGD("analyzeRemoteFeature g_rfd_data %s", jsonData);
//						int ret = analyzeRemoteFeature(g_rfd_data, (char*)&msgId);
//						LOGD("analyzeRemoteFeature ret %d", ret);
					}
					freePointer(&rfd_topic);
					return 0;
				}
				//freePointer(&ffd_topic_cmd);

                char payload[MQTT_AT_LEN];
                strncpy(payload, data, data_len);
                payload[data_len] = '\0';
                ana_timeout_count = 0;
                LOGD("payload is %s len is %d\r\n", payload, strlen(payload));
                int ret = 0;
                ret = handleJsonMsg(payload);
                if (ret < 0)  {
                    result = -3; // command fail
                } else {
                    result = 0;
                }
                // freePointer(&payload);
            } else {
                if (++ana_timeout_count > 10) {
                    LOGD("analyze timeout\n");
                    result = -2;
                    ana_timeout_count = 0;
                }
            }
        }

    } else {
        LOGD("---- it is not sub recv\r\n");
    }
    // 下面的初始化会导致strtok 段错误
    // char *data = "+MQTTSUBRECV:0,\"testtopic\",26,{ \"msg\": \"Hello, World!\" }";

    return result;
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

