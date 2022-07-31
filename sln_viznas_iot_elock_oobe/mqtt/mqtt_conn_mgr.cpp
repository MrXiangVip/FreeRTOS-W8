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
