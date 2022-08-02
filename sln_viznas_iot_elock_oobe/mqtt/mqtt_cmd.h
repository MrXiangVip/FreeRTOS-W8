#ifndef _MQTT_CMD_H_
#define _MQTT_CMD_H_

#include "stdio.h"

//#ifdef __cplusplus
//extern "C" {
//#endif

typedef enum {
    PRIORITY_HIGHEST    = 0x01,
    PRIORITY_HIGH       = 0x02,
    PRIORITY_MEDIUM     = 0x03,
    PRIORITY_LOW        = 0x04,
    PRIORITY_LOWEST     = 0x05,
} AT_RSP_PRIORITY;

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
