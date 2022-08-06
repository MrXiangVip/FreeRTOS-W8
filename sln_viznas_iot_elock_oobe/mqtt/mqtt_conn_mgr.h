//
// Created by wszgx on 2022/7/29.
//

#ifndef _MQTT_STATE_H_
#define _MQTT_STATE_H_

#include "mqtt-common.h"

#define MQTT_LINK_ID_DEFAULT 		0

#define MQTT_SCHEME_TCP 						1
#define MQTT_SCHEME_TLS_NO_CERT					2
#define MQTT_SCHEME_TLS_SERVER_CERT				3
#define MQTT_SCHEME_TLS_CLIENT_CERT				4
#define MQTT_SCHEME_TLS_BOTH_CERT				5
#define MQTT_SCHEME_WEBSOCKET_TCP				6
#define MQTT_SCHEME_WEBSOCKET_TLS_NO_CERT		7
#define MQTT_SCHEME_WEBSOCKET_TLS_SERVER_CERT	8
#define MQTT_SCHEME_WEBSOCKET_TLS_CLIENT_CERT	9
#define MQTT_SCHEME_WEBSOCKET_TLS_BOTH_CERT		10

#define MQTT_CERT_KEY_ID_DEFAULT	0
#define MQTT_CA_ID_DEFAULT			0

#define MQTT_STATE_UNINITIALIZED		0
#define MQTT_STATE_SET_BY_MQTTUSERCFG	1
#define MQTT_STATE_SET_BY_MQTTCONNCFG	2
#define MQTT_STATE_DISCONNECTED			3
#define MQTT_STATE_ESTABLISHED			4
#define MQTT_STATE_CONNECTED_TOPIC_NONE	5
#define MQTT_STATE_CONNECTED_TOPIC_SUB	6

#define MQTT_QOS_AT_MOST_ONCE			0
#define MQTT_QOS_AT_LEAST_ONCE			1
#define MQTT_QOS_EXACTLY_ONCE			2

#define MQTT_RETAIN_OFF		0
#define MQTT_RETAIN_ON		1

#define MQTT_AT_CMD_SUCCESS	0
typedef enum {
    WIFI_DISCONNECTED = 0x00,
    WIFI_CONNECTED = 0x01,
    MQTT_CONNECTED = 0x02,
} MQTT_CONN_STATE;

class MqttConnMgr {
private:
    // WIFI RSSI
    int m_wifi_rssi;
    char m_at_cmd[MQTT_AT_CMD_LEN];

    MqttConnMgr() {
        m_mqtt_conn_state = WIFI_DISCONNECTED;
    };
    MQTT_CONN_STATE m_mqtt_conn_state;

    // MQTT
    // Connection State
    int initWifiConnection(const char* ssid, const char* password);
    int initMqttConnection(const char* clientId, const char* username, const char* password, const char* serverIp, const char* serverPort);
public:
    static MqttConnMgr *getInstance() {
        static MqttConnMgr m_instance;
        return &m_instance;
    };
    // Connection State
    void keepConnectionAlive();

    void setMqttConnState(MQTT_CONN_STATE mqttConnState);
    MQTT_CONN_STATE getMqttConnState();
    bool isWifiConnected();
    bool isMqttConnected();

    // WIFI
    int resetWifi();    // reset wifi module
    void setWifiRSSI(int wifiRSSI);
    int getWifiRSSI();
    int updateWifiRSSI();
    int connectWifi(const char* ssid, const char* password);
    void reconnectWifiAsync();

    // MQTT
    void reconnectMqttAsync();
    // 配置MQTT UserConfig参数
    int setupMQTTUserConfig(const char* clientId, const char* username, const char* password,
            int scheme = MQTT_SCHEME_TCP,
            char* path = "",
            int certKeyId = MQTT_CERT_KEY_ID_DEFAULT,
            int caId = MQTT_CA_ID_DEFAULT,
            int linkId = MQTT_LINK_ID_DEFAULT);
    // 配置MQTT ConnConfig参数
    int setupMQTTConnConfig(char* lwtTopic = "",
            char* lwtMsg = "",
            int keepAlive = 180,
            int disableCleanSession = 0,
            int lwtQos = MQTT_QOS_AT_LEAST_ONCE,
            int lwtRetain = MQTT_RETAIN_ON,
            int linkId = MQTT_LINK_ID_DEFAULT);
    // 连接MQTT服务器
    int connectMQTT(const char* hostIp, const char* port, int reconnect = 0, int linkId = MQTT_LINK_ID_DEFAULT);
    // 断开MQTT连接
    int disconnectMQTT(int linkId = MQTT_LINK_ID_DEFAULT);
    // 订阅MQTT主题
    int subscribeMQTT(char* topic, int qos = MQTT_QOS_AT_MOST_ONCE, int linkId = MQTT_LINK_ID_DEFAULT);
    // 取消订阅MQTT主题
    int unsubscribeMQTT(char* topic, int linkId = MQTT_LINK_ID_DEFAULT);
    // 发布消息
    int publishMQTT(char* topic, const char* data, int qos = MQTT_QOS_AT_LEAST_ONCE, int retain = MQTT_RETAIN_OFF, int linkId = MQTT_LINK_ID_DEFAULT);
    // 发布Raw消息
    int publishRawMQTT(char* topic, char* data, int data_len, int qos = MQTT_QOS_AT_LEAST_ONCE, int retain = MQTT_RETAIN_OFF, int linkId = MQTT_LINK_ID_DEFAULT);
    // 快速连接MQTT
    int quickConnectMQTT(const char* clientId, const char* username, const char* password, const char* hostIp, const char* port, char* lwtTopic = "", char* lwtMsg = "");
};

#endif //_MQTT_STATE_H_
