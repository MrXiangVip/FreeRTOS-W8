#ifndef _MQTT_CMD_MGR_H_
#define _MQTT_CMD_MGR_H_

//#ifdef __cplusplus
//extern "C" {
//#endif

#include "mqtt_cmd.h"

#include "PriorityQueue.h"
#include "DBManager.h"
#include <vector>
#include <stdio.h>

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
    bool m_is_heart_beating = false;
    int m_hb_fail_count = 0;

    PriorityQueue m_mqtt_cmds;
    int pushRecord(int uploadStatus, int cmdType);
    vector<int> m_uploading_records;

    int uploadRecordText(Record *record);
    int uploadRecordImage(Record *record);
    int pushRecords(int uploadStatus, int cmdType, int maxCount = 20);
    void doHeartBeat();
public:
    static MqttCmdMgr *getInstance() {
        static MqttCmdMgr m_instance;
        return &m_instance;
    };
    char *genMsgId();

    /****** 远程指令处理 ******/
    // 持续监控并向远程发送mqtt指令，核心任务
    void loopSendMqttMsgs();

    /****** 设备->后台指令处理 ******/
    // 发送实时记录，比如注册记录/开门记录等
    int pushRecord(Record *record, int cmdType, int priority = PRIORITY_LOW, int force = 0);
    // 发送历史记录
    void uploadRecords();

    // AT指令回复
    void atCmdResponse(int result, char *msgId, char *rspMsg = NULL, int priority = PRIORITY_HIGH);

    // 请求上传用户特征值
    void requestFeature(char *uuid);
    // 请求心跳
    void heartBeat();
};

//#ifdef __cplusplus
//}
//#endif

#endif
