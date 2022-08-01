#ifndef _MQTT_CMD_MGR_H_
#define _MQTT_CMD_MGR_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MSG_ID_LEN 20
#define MSG_BT_MAC_LEN	10

class MqttCmdMgr {
private:
    MqttCmdMgr() {};
    char m_msg_id[MSG_ID_LEN + 1];
    int random_gen = 1;
public:
    static MqttCmdMgr *getInstance() {
        static MqttCmdMgr m_instance;
        return &m_instance;
    };
    char *genMsgId();
};

#ifdef __cplusplus
}
#endif

#endif
