#ifndef _MQTT_CMD_MGR_H_
#define _MQTT_CMD_MGR_H_

//#ifdef __cplusplus
//extern "C" {
//#endif

#include "mqtt_cmd.h"

#include "PriorityQueue.h"

#define MSG_ID_LEN 20
#define MSG_BT_MAC_LEN	10

typedef enum {
    AT_RSP_SUCCESS      = 0x00,
    AT_RSP_ERROR        = 0x01,
    AT_RSP_TIMEOUT      = 0x02,
    AT_RSP_BUSY         = 0x03,
    AT_RSP_NOT_SUPPORT  = 0x04,
} AT_RSP_RESULT;

class MqttCmdMgr {
private:
    MqttCmdMgr() {};
    char m_msg_id[MSG_ID_LEN + 1];
    int random_gen = 1;

    PriorityQueue m_mqtt_cmds;

public:
    static MqttCmdMgr *getInstance() {
        static MqttCmdMgr m_instance;
        return &m_instance;
    };
    char *genMsgId();

    void atCmdResponse(int result, char *msgId, char *rspMsg, int priority = PRIORITY_HIGH);

    void loopSendMqttMsgs();
    void uploadRecords();

    int timeSync(char *ts);
};

//#ifdef __cplusplus
//}
//#endif

#endif
