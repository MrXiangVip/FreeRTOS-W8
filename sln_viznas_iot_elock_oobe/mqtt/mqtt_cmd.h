#ifndef _MQTT_CMD_H_
#define _MQTT_CMD_H_

#include "stdio.h"

//#ifdef __cplusplus
//extern "C" {
//#endif

class MqttCmd {
private:
    int m_priority;
    int m_cmd_type;
    char *m_topic = NULL;
    char *m_data = NULL;
public:
    MqttCmd(int priority, int cmdType, char *topic, char *data);
    void free();
    ~MqttCmd() { free(); }
    bool operator > (const MqttCmd& mc);
    bool operator < (const MqttCmd& mc);

    int getCmdType() { return m_cmd_type; }
    char *getTopic() { return m_topic; }
    char *getData() { return m_data; }
};


//#ifdef __cplusplus
//}
//#endif

#endif
