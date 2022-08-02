//
// Created by wszgx on 2022/7/29.
//

#ifndef _MQTT_MANAGER_H_
#define _MQTT_MANAGER_H_

#include "mqtt-instruction-pool.h"

class MqttManager {
private:
    MqttManager() {};
public:
    static MqttManager *getInstance() {
        static MqttManager m_instance;
        return &m_instance;
    };

    // 分析最原始的MQTT AT消息
    int analyzeMqttRecvLine(char *msg);
    // 解包之后的MQTT AT数据包
    int handleMqttMsgData(char *jsonMsg);
    // 透传指令解析
    int handlePassThroughPayload(char *payload, char *idStr);
};

#endif //_MQTT_MANAGER_H_
