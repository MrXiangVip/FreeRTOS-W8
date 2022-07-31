//
// Created by wszgx on 2022/7/29.
//

#ifndef _MQTT_STATE_H_
#define _MQTT_STATE_H_

typedef enum {
    WIFI_DISCONNECTED = 0x00,
    WIFI_CONNECTED = 0x01,
    MQTT_CONNECTED = 0x02,
} MQTT_CONN_STATE;

class MqttConnMgr {
private:
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
};

#endif //_MQTT_STATE_H_
