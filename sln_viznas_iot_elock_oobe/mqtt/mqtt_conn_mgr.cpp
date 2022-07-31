//
// Created by wszgx on 2022/7/29.
//

#include <stdio.h>

#include "mqtt_conn_mgr.h"
#include "mqtt-mcu.h"
#include "mqtt_dev_esp32.h"
#include "fsl_log.h"

void MqttConnMgr::setMqttConnState(MQTT_CONN_STATE mqttConnState) {
    m_mqtt_conn_state = mqttConnState;
}

MQTT_CONN_STATE MqttConnMgr::getMqttConnState() {
    return m_mqtt_conn_state;
}

bool MqttConnMgr::isWifiConnected() {
    LOGD("isWifiConnected %d\r\n", m_mqtt_conn_state);
    return m_mqtt_conn_state != WIFI_DISCONNECTED;
}

bool MqttConnMgr::isMqttConnected() {
    LOGD("isMqttConnected %d\r\n", m_mqtt_conn_state);
    return m_mqtt_conn_state == MQTT_CONNECTED;
}

int MqttConnMgr::resetWifi() {
    int result = MqttDevEsp32::getInstance()->sendATCmd("AT+RST", 2, 1200);
    LOGD("resetWifi result %d\r\n", result);
    notifyWifiInitialized(result);
    return result;
}
