#include <string.h>
#include <stdio.h>
#include "mqtt-common.h"
#include "fsl_log.h"
#include "config.h"
#include "mqtt_cmd_mgr.h"
#include "mqtt_topic_mgr.h"
#include "mqtt_conn_mgr.h"
#include "MCU_UART5_Layer.h"
#include "PriorityQueue.h"
#include "mqtt-mcu.h"

char* MqttCmdMgr::genMsgId() {
    struct timeval tv;
    tv.tv_sec = ws_systime;
    tv.tv_usec = 0;
    long a = tv.tv_sec % 1000000000;
    char bt_mac_string[MSG_BT_MAC_LEN + 1];
    memset(bt_mac_string, '\0', sizeof(bt_mac_string));
    memcpy(bt_mac_string, btWifiConfig.bt_mac + 2, MSG_BT_MAC_LEN);
    if (++random_gen >= 10) {
        random_gen = 1;
    }
    sprintf(m_msg_id, "%s%09d%d", bt_mac_string, a, random_gen);
    return m_msg_id;
}

void MqttCmdMgr::atCmdResponse(int result, char *msgId, char *rspMsg, int priority) {
    char pubMsg[MQTT_AT_CMD_LEN] = {0};
    sprintf(pubMsg,
            "{\\\"id\\\":\\\"%s\\\"\\,\\\"res\\\":%d\\,\\\"msg\\\":\\\"%s\\\"}",
            msgId, result, rspMsg);
    char *topic = MqttTopicMgr::getInstance()->getPubTopicCmdResponse();
    // TODO: 防止内存泄漏
    MqttCmd *mqttCmd = new MqttCmd(priority, CMD_TYPE_RAW, topic, pubMsg);
    m_mqtt_cmds.Push(mqttCmd);
    // TODO: don't try to use Top().getData() or it will make this program crash
//    LOGD("push size %d %s\r\n", m_mqtt_cmds.Size(), m_mqtt_cmds.Top().getData());
    LOGD("push size %d\r\n", m_mqtt_cmds.Size());
//    int result = MqttConnMgr::getInstance()->publishMQTT(topic, pubMsg);
//    LOGD("do atCmdResponse result %d\r\n", result);
}

void MqttCmdMgr::loopSendMqttMsgs() {
    for(;;) {
        while (MqttConnMgr::getInstance()->isMqttConnected() && !m_mqtt_cmds.Empty())
        {
            MqttCmd mqttCmd = m_mqtt_cmds.Top();
            m_mqtt_cmds.Pop();

            int cmdType = mqttCmd.getCmdType();
            if (cmdType == CMD_TYPE_RAW) {
                int result = MqttConnMgr::getInstance()->publishMQTT(mqttCmd.getTopic(), mqttCmd.getData());
                LOGD("do topic %s result %d\r\n", mqttCmd.getTopic(), result);
            } else if (cmdType == CMD_TYPE_RECORD_TEXT || cmdType == CMD_TYPE_RECORD_IMAGE) {
                char *recordId = mqttCmd.getTopic();
                int rid = atoi(recordId);
                Record record;
                int ret = DBManager::getInstance()->getRecordByID(rid, &record);
            }
            mqttCmd.free();
        }
        vTaskDelay(pdMS_TO_TICKS(50));
//        vTaskDelay(pdMS_TO_TICKS(25000));
    }
}

int MqttCmdMgr::timeSync(char *ts) {
    if (ts != NULL && strlen(ts) > 0) {
        int currentSec = atoi(ts);
        if (currentSec > 1618965299) {
            LOGD("__network time sync networkTime is %d can setTimestamp\r\n", currentSec);
            setTimestamp(currentSec);
        } else {
            LOGD("__network time sync networkTime is %d don't setTimestamp\r\n", currentSec);
            return AT_RSP_ERROR;
        }

        int result = syncTimeToMCU(ts);
        return result == 0 ? AT_RSP_SUCCESS : AT_RSP_ERROR;
    }
    return AT_RSP_ERROR;
}

int MqttCmdMgr::pushRecord(int uploadStatus, int cmdType, int maxCount) {
    list<Record*> records = DBManager::getInstance()->getAllUnuploadRecord();
    list <Record*>::iterator it;
    int count = 0;
    LOGD("pushRecord %d\r\n", records.size());
    for ( it = records.begin( ); it != records.end( ); it++ ) {
        Record *record = (Record * ) * it;
        if (record->upload == uploadStatus) {
            char rid[12] = {0};
            sprintf(rid, "%d", record->ID);
            MqttCmd *mqttCmd = new MqttCmd(PRIORITY_LOW, cmdType, rid, rid);
            m_mqtt_cmds.Push(mqttCmd);
            count++;
            if (count >= maxCount) {
                break;
            }
        }
    }
    return count;
}

void MqttCmdMgr::uploadRecords() {
    int timeoutCount = 0;
    for(;;) {
        int recordCount = pushRecord(BOTH_UNUPLOAD, CMD_TYPE_RECORD_TEXT, 20);
        if (recordCount > 0) {
            vTaskDelay(pdMS_TO_TICKS(100*recordCount));
            continue;
        }

        recordCount = pushRecord(BOTH_UPLOAD, CMD_TYPE_RECORD_IMAGE, 10);
        if (recordCount > 0) {
            vTaskDelay(pdMS_TO_TICKS(1000*recordCount));
            continue;
        }

        if (recordCount == 0) {
            if (timeoutCount < 10) {
                timeoutCount++;
            }
            vTaskDelay(pdMS_TO_TICKS(6000*timeoutCount));
        } else {
            timeoutCount = 1;
        }
    }
}
