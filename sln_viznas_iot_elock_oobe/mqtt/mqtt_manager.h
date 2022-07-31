//
// Created by wszgx on 2022/7/29.
//

#ifndef _MQTT_MANAGER_H_
#define _MQTT_MANAGER_H_

class MqttManager {
private:
    MqttManager() {};
public:
    static MqttManager *getInstance() {
        static MqttManager m_instance;
        return &m_instance;
    };

    int analyzeMqttMsg(char *msg);
    int handleJsonMsg(char *jsonMsg);
};

#endif //_MQTT_MANAGER_H_
