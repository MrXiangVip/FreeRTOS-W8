//
// Created by wszgx on 2022/7/29.
//

#ifndef _MQTT_STATE_H_
#define _MQTT_STATE_H_

#include "mqtt-common.h"

typedef enum {
    WIFI_DISCONNECTED = 0x00,
    WIFI_CONNECTED = 0x01,
    MQTT_CONNECTED = 0x02,
} MQTT_CONN_STATE;

class MqttConnMgr {
private:
    int m_wifi_rssi;
    char m_at_cmd[MQTT_AT_CMD_LEN];

    MqttConnMgr() {
        m_mqtt_conn_state = WIFI_DISCONNECTED;
    };
    MQTT_CONN_STATE m_mqtt_conn_state;
public:
    static MqttConnMgr *getInstance() {
        static MqttConnMgr m_instance;
        return &m_instance;
    };

    void setMqttConnState(MQTT_CONN_STATE mqttConnState);
    MQTT_CONN_STATE getMqttConnState();
    bool isWifiConnected();
    bool isMqttConnected();

    int resetWifi();
    void setWifiRSSI(int wifiRSSI);
    int getWifiRSSI();
    int updateWifiRSSI();
    int connectWifi(const char* ssid, const char* password);
};

#endif //_MQTT_STATE_H_
