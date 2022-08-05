#ifndef _MQTT_CMD_H_
#define _MQTT_CMD_H_

#include "stdio.h"

//#ifdef __cplusplus
//extern "C" {
//#endif

// 返回命令是HIGH，心跳是LOW，记录上传是MEDIUM，保留HIGHEST/LOWEST扩展，默认HIGH
typedef enum {
    PRIORITY_HIGHEST    = 0x01,
    PRIORITY_HIGH       = 0x02,
    PRIORITY_MEDIUM     = 0x03,
    PRIORITY_LOW        = 0x04,
    PRIORITY_LOWEST     = 0x05,
} AT_RSP_PRIORITY;

// 命令类型，RAW是裸数据，直接上传，RECORD_TEXT是注册/开门记录，需要解析上传，RECORD_IMAGE是注册/开门记录带图片
typedef enum {
    CMD_TYPE_RAW            = 0x01,
    CMD_TYPE_RECORD_TEXT    = 0x02,
    CMD_TYPE_RECORD_IMAGE   = 0x03,
    CMD_TYPE_FEATURE_UPLOAD = 0x04,
};

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
