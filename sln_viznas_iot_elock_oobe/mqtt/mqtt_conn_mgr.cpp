//
// Created by wszgx on 2022/7/29.
//

#include <stdio.h>

#include "FreeRTOS.h"
#include "mqtt_conn_mgr.h"
#include "mqtt-mcu.h"
#include "mqtt-topic.h"
#include "mqtt_topic_mgr.h"
#include "mqtt_dev_esp32.h"
#include "fsl_log.h"
#include "config.h"
#include "mqtt_util.h"
#include "MCU_UART5_Layer.h"
#include "mqtt_cmd_mgr.h"

/**************** Connection State start********************/
int MqttConnMgr::initWifiConnection(const char* ssid, const char* password) {
    MqttDevEsp32::getInstance()->setSysLog(1);
    MqttDevEsp32::getInstance()->setEcho(0);
    int result = MqttDevEsp32::getInstance()->setWifiMode(1);

    int wifi_count = 0;
    // 尽量3s以内判断wifi是否已自动连接，若已经自动连接，则无需处理下面的操作
    while (!isWifiConnected() && wifi_count < 10) {
        updateWifiRSSI();
        LOGD("initConnection connect to wifi %d %d\r\n", MqttConnMgr::getInstance()->isWifiConnected(), MqttConnMgr::getInstance()->getMqttConnState());
        // 睡眠300ms
        vTaskDelay(pdMS_TO_TICKS(300));
        wifi_count++;
    }

    if (!isWifiConnected()) {
        // 连接WIFI
        result = connectWifi(ssid, password);
        if (result < 0) {
            LOGD("initConnection Failed to connect to WIFI\r\n");
            return -1;
        }
        updateWifiRSSI();
        LOGD("initConnection connect to WIFI done\r\n");
    } else {
        LOGD("initConnection auto connect to WIFI done\r\n");
    }
    notifyWifiConnected(0);

    return result;
}

int MqttConnMgr::initMqttConnection(const char* clientId, const char* username, const char* password, const char* serverIp, const char* serverPort) {
    disconnectMQTT();
    // 连接MQTT
    int result = quickConnectMQTT(clientId, username, password, serverIp, serverPort);
    if (result != 0) {
        LOGD("--------- Failed to connect to MQTT\r\n");
        return -1;
    }
    LOGD("--------- connect to mqtt done\r\n");

    char *crTopic = MqttTopicMgr::getInstance()->getSubTopicCmdRequest();
    result = subscribeMQTT(crTopic);
    char *frTopic = MqttTopicMgr::getInstance()->getSubTopicFeatureRequest();
    result = subscribeMQTT(frTopic);
    char *fdTopic = MqttTopicMgr::getInstance()->getSubTopicFeatureDownload();
    result = subscribeMQTT(fdTopic);

    notifyMqttConnected(result);
    setMqttConnState(MQTT_CONNECTED);
    return result;
}

void MqttConnMgr::keepConnectionAlive() {
    int wifi_fail_count = 0;
    int mqtt_fail_count = 0;
    int heart_beat_count = 0;
    do {
        if (!isWifiConnected()) {
            initWifiConnection(btWifiConfig.ssid, btWifiConfig.password);
            if (isWifiConnected()) {
                wifi_fail_count = 0;
                initMqttConnection(mqttConfig.client_id,
                                   mqttConfig.username,
                                   mqttConfig.password,
                                   mqttConfig.server_ip,
                                   mqttConfig.server_port);
                if (!isMqttConnected()) {
                    mqtt_fail_count++;
                } else {
                    mqtt_fail_count = 0;
                    wifi_fail_count = 0;
                }
            } else {
                wifi_fail_count++;
            }
        } else {
            if (!isMqttConnected()) {
                initMqttConnection(mqttConfig.client_id,
                        mqttConfig.username,
                        mqttConfig.password,
                        mqttConfig.server_ip,
                        mqttConfig.server_port);
                if (!isMqttConnected()) {
                    mqtt_fail_count++;
                } else {
                    mqtt_fail_count = 0;
                    wifi_fail_count = 0;
                }
            }
        }
        if (isMqttConnected() && heart_beat_count++ % 6 == 0) {
            // TODO: insert into command queue
            heartbeat();
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
//        LOGD("keepConnectionAlive heart_beat_count %d mqtt_fail_count %d wifi_fail_count %d\r\n", heart_beat_count, mqtt_fail_count, wifi_fail_count);
//        do heartbeat every 60s
        if (mqtt_fail_count > 1) {
            LOGD("keepConnectionAlive disconnectMQTT mqtt_fail_count %d wifi_fail_count %d\r\n", mqtt_fail_count, wifi_fail_count);
            disconnectMQTT();
        } else if (mqtt_fail_count > 5) {
            LOGD("keepConnectionAlive setMqttConnState WIFI_DISCONNECTED mqtt_fail_count %d wifi_fail_count %d\r\n", mqtt_fail_count, wifi_fail_count);
//            setMqttConnState(WIFI_DISCONNECTED);
            reconnectWifiAsync();
        } else if (mqtt_fail_count > 10 || wifi_fail_count > 2) {
            LOGD("keepConnectionAlive resetWifi mqtt_fail_count %d wifi_fail_count %d\r\n", mqtt_fail_count, wifi_fail_count);
            resetWifi();
        }
    } while(1 > 0);
}

void MqttConnMgr::heartbeat() {
    static int heartbeatIdx = 1;
    static int heartbeatFailCount = 0;
    char pubMsg[MQTT_AT_CMD_LEN] = {0};
    char *msgId = MqttCmdMgr::getInstance()->genMsgId();
//    LOGD("heartbeat msgId %s\r\n", msgId);
    sprintf(pubMsg,
//            "{\"id\":\"%s\",\"ts\":%d,\"wr\":%d,\"idx\":%d,\"ver\":\"%s\"}",
            "{\\\"id\\\":\\\"%s\\\"\\,\\\"ts\\\":%d\\,\\\"wr\\\":%d\\,\\\"idx\\\":%d\\,\\\"ver\\\":\\\"%s\\\"}",
            msgId, ws_systime, m_wifi_rssi, heartbeatIdx++, "");
    char *topic = MqttTopicMgr::getInstance()->getPubTopicHeartBeat();
//    LOGD("do heartbeat topic %s %s\r\n", topic, pubMsg);
    int result = 0;
    result = publishMQTT(topic, pubMsg);
//    LOGD("do heartbeat result %d\r\n", result);
    if (result != 0) {
        if (heartbeatFailCount++ > 2) {
//            this->setMqttConnState(WIFI_CONNECTED);
            reconnectMqttAsync();
        }
    }
}

void MqttConnMgr::reconnectWifiAsync() {
    setMqttConnState(WIFI_DISCONNECTED);
}

void MqttConnMgr::reconnectMqttAsync() {
    setMqttConnState(WIFI_CONNECTED);
}

void MqttConnMgr::setMqttConnState(MQTT_CONN_STATE mqttConnState) {
    m_mqtt_conn_state = mqttConnState;
}

MQTT_CONN_STATE MqttConnMgr::getMqttConnState() {
    return m_mqtt_conn_state;
}

bool MqttConnMgr::isWifiConnected() {
//    LOGD("isWifiConnected %d\r\n", m_mqtt_conn_state);
    return m_mqtt_conn_state != WIFI_DISCONNECTED;
}

bool MqttConnMgr::isMqttConnected() {
//    LOGD("isMqttConnected %d\r\n", m_mqtt_conn_state);
    return m_mqtt_conn_state == MQTT_CONNECTED;
}
/**************** Connection State end ********************/

/**************** WIFI start ********************/
int MqttConnMgr::resetWifi() {
    int result = MqttDevEsp32::getInstance()->sendATCmd("AT+RST", 2, 1200);
    LOGD("resetWifi result %d\r\n", result);
    notifyWifiInitialized(result);
    return result;
}

void MqttConnMgr::setWifiRSSI(int wifiRSSI) {
    m_wifi_rssi = wifiRSSI;
}

int MqttConnMgr::getWifiRSSI() {
    return m_wifi_rssi;
}

int MqttConnMgr::updateWifiRSSI() {
    int result = MqttDevEsp32::getInstance()->sendATCmd("AT+CWJAP?", 1, 1000);
    LOGD("getWifiRSSI result %d\r\n", result);
    return result;
}

int MqttConnMgr::connectWifi(const char* ssid, const char* password) {
    memset(m_at_cmd, '\0', sizeof(m_at_cmd));
    sprintf(m_at_cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
    int result = MqttDevEsp32::getInstance()->sendATCmd(m_at_cmd, 2, 10000);
    LOGD("connectWifi result %d\r\n", result);
    notifyWifiConnected(result);
    // TODO: 此处仅仅是wifi配置了，但是是否获取到IP不确定，也许需要updateWifiRSSI?
    return result;
}
/**************** WIFI end ********************/

/**************** MQTT start ********************/
// 配置MQTT UserConfig参数
int MqttConnMgr::setupMQTTUserConfig(const char* clientId, const char* username, const char* password,
        int scheme, char* path, int certKeyId, int caId, int linkId) {
    memset(m_at_cmd, 0, sizeof(m_at_cmd));
    sprintf(m_at_cmd, "AT+MQTTUSERCFG=%d,%d,\"%s\",\"%s\",\"%s\",%d,%d,\"%s\"", linkId, scheme, clientId, username, password, certKeyId, caId, path);
    int result = MqttDevEsp32::getInstance()->sendATCmd(m_at_cmd);
    return result;
}

// 配置MQTT ConnConfig参数
int MqttConnMgr::setupMQTTConnConfig(char* lwtTopic, char* lwtMsg, int keepAlive,
        int disableCleanSession, int lwtQos, int lwtRetain, int linkId) {
    memset(m_at_cmd, 0, sizeof(m_at_cmd));
    sprintf(m_at_cmd, "AT+MQTTCONNCFG=%d,%d,%d,\"%s\",\"%s\",%d,%d", linkId, keepAlive, disableCleanSession, lwtTopic, lwtMsg, lwtQos, lwtRetain);
    int result = MqttDevEsp32::getInstance()->sendATCmd(m_at_cmd);
    return result;
}

// 连接MQTT服务器
int MqttConnMgr::connectMQTT(const char* hostIp, const char* port, int reconnect, int linkId) {
    memset(m_at_cmd, 0, sizeof(m_at_cmd));
    sprintf(m_at_cmd, "AT+MQTTCONN=%d,\"%s\",%s,%d", linkId, hostIp, port, reconnect);
    int result = MqttDevEsp32::getInstance()->sendATCmd(m_at_cmd);
    return result;
}

// 断开MQTT连接
int MqttConnMgr::disconnectMQTT(int linkId) {
    memset(m_at_cmd, 0, sizeof(m_at_cmd));
    sprintf(m_at_cmd, "AT+MQTTCLEAN=%d", linkId);
    int result = MqttDevEsp32::getInstance()->sendATCmd(m_at_cmd);
    return result;
}

// 订阅MQTT主题
int MqttConnMgr::subscribeMQTT(char* topic, int qos, int linkId) {
    memset(m_at_cmd, 0, sizeof(m_at_cmd));
    sprintf(m_at_cmd, "AT+MQTTSUB=%d,\"%s\",%d", linkId, topic, qos);
    int result = MqttDevEsp32::getInstance()->sendATCmd(m_at_cmd);
//    vPortFree(topic);
    return result;
}

// 取消订阅MQTT主题
int MqttConnMgr::unsubscribeMQTT(char* topic, int linkId) {
    memset(m_at_cmd, 0, sizeof(m_at_cmd));
    sprintf(m_at_cmd, "AT+MQTTUNSUB=%d,\"%s\"", linkId, topic);
    int result = MqttDevEsp32::getInstance()->sendATCmd(m_at_cmd);
//    vPortFree(topic);
    return result;
}

// 发布消息
int MqttConnMgr::publishMQTT(char* topic, const char* data, int qos, int retain, int linkId) {
    memset(m_at_cmd, 0, sizeof(m_at_cmd));
    sprintf(m_at_cmd, "AT+MQTTPUB=%d,\"%s\",\"%s\",%d,%d", linkId, topic, data, qos, retain);
    int result = MqttDevEsp32::getInstance()->sendATCmd(m_at_cmd);
//    vPortFree(topic);
    return result;
}

// 发布Raw消息
int MqttConnMgr::publishRawMQTT(char* topic, char* data, int data_len, int qos, int retain, int linkId) {
    sprintf(m_at_cmd, "AT+MQTTPUBRAW=%d,\"%s\",%d,%d,%d", linkId, topic, data_len, qos, retain);
    int result = MqttDevEsp32::getInstance()->sendRawATCmd(m_at_cmd, data, data_len, 15000);
//    vPortFree(topic);
    return result;
}

int MqttConnMgr::quickConnectMQTT(const char* clientId, const char* username, const char* password, const char* hostIp, const char* port, char* lwtTopic, char* lwtMsg) {
    int resSetup = setupMQTTUserConfig(clientId, username, password);
    if (resSetup) {
        return resSetup;
    }
    // 作用：服务器必须在客户端断开之后继续存储/保持客户端的订阅状态，包括以下状态
    // 存储订阅的消息Qos1和Qos2消息，当客户端重新订阅时发送
    // 服务端正在发送消息给客户端期间连接丢失导致发送失败的消息
    // disable_clean_session 为0代表会session丢失
    // 对于门锁来说是合理的，否则万一保留了一个开锁的命令，结果下次连接就直接开锁了。这不合理
    resSetup = setupMQTTConnConfig(lwtTopic, lwtMsg);
    if (resSetup) {
        return resSetup;
    }

//    vTaskDelay(pdMS_TO_TICKS(50));
    int resConn = connectMQTT(hostIp, port);
    return resConn;
}

//int MqttConnMgr::quickSubscribeMQTT(const char* topic) {
//    return subscribeMQTT(MQTT_LINK_ID_DEFAULT, topic, MQTT_QOS_AT_MOST_ONCE);
//}
//
//int MqttConnMgr::quickUnsubscribeMQTT(const char* topic) {
//    return unsubscribeMQTT(MQTT_LINK_ID_DEFAULT, topic);
//}
//
//int MqttConnMgr::quickPublishMQTT(const char* topic, const char* data) {
//    return publishMQTT(MQTT_LINK_ID_DEFAULT, topic, data, MQTT_QOS_AT_LEAST_ONCE, MQTT_RETAIN_OFF);
//}
//
//int MqttConnMgr::quickPublishRawMQTT(const char* topic, const char* data, int data_len) {
//    return publishRawMQTT(MQTT_LINK_ID_DEFAULT, topic, (char*)data, data_len, MQTT_QOS_AT_LEAST_ONCE, MQTT_RETAIN_OFF);
//}
//
//int MqttConnMgr::quickPublishOasisMQTT(const char* topic, const char* data) {
//    return publishOasisMQTT(MQTT_LINK_ID_DEFAULT, topic, data, MQTT_QOS_AT_LEAST_ONCE, MQTT_RETAIN_OFF);
//}
//
//int MqttConnMgr::quickPublishMQTTWithPriority(const char* topic, const char* data, int priority) {
//    int ret = publishRawMQTT(MQTT_LINK_ID_DEFAULT, topic, (char*)data, strlen(data), MQTT_QOS_AT_LEAST_ONCE, MQTT_RETAIN_OFF);
//    return ret;
//}
//
//int MqttConnMgr::quickDisconnectMQTT() {
//    return disconnectMQTT(MQTT_LINK_ID_DEFAULT);
//}
/**************** MQTT end ********************/
