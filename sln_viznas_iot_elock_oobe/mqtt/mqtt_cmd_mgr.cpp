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
#include "mqtt_feature_mgr.h"
#include "mqtt_mcu_mgr.h"
#include "fatfs_op.h"
#include "base64.h"

char* MqttCmdMgr::genMsgId() {
    static int random_gen = 1;
    struct timeval tv;
    tv.tv_sec = ws_systime;
    tv.tv_usec = 0;
    long a = tv.tv_sec % 1000000000;
    char bt_mac_string[MSG_BT_MAC_LEN + 1];
    memset(bt_mac_string, '\0', sizeof(bt_mac_string));
    memcpy(bt_mac_string, btWifiConfig.bt_mac + 2, MSG_BT_MAC_LEN);
    sprintf(m_msg_id, "%s%09d%d", bt_mac_string, a, random_gen);
    if (++random_gen >= 10) {
        random_gen = 1;
    }
    return m_msg_id;
}

void MqttCmdMgr::atCmdResponse(int result, char *msgId, char *rspMsg, int priority) {
    char pubMsg[MQTT_AT_CMD_LEN] = {0};
    sprintf(pubMsg,
            "{\\\"id\\\":\\\"%s\\\"\\,\\\"res\\\":%d\\,\\\"msg\\\":\\\"%s\\\"}",
            (msgId != NULL ? msgId : genMsgId()), result, (rspMsg != NULL ? rspMsg : ((result == AT_RSP_SUCCESS) ? "OK" : "Error")));
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
            } else if (cmdType == CMD_TYPE_HEART_BEAT) {
                doHeartBeat();
            } else if (cmdType == CMD_TYPE_RECORD_TEXT || cmdType == CMD_TYPE_RECORD_IMAGE) {
                char *recordId = mqttCmd.getData();
                int rid = atoi(recordId);
                Record record;
                int ret = DBManager::getInstance()->getRecordByID(rid, &record);
                if (cmdType == CMD_TYPE_RECORD_TEXT) {
                    uploadRecordText(&record);
                } else {
                    uploadRecordImage(&record);
                }
            } else if (cmdType == CMD_TYPE_FEATURE_UPLOAD) {
                int result = MqttFeatureMgr::getInstance()->uploadFeature(mqttCmd.getData());
            }
            mqttCmd.free();
        }
        vTaskDelay(pdMS_TO_TICKS(50));
        m_is_heart_beating = false;
//        vTaskDelay(pdMS_TO_TICKS(25000));
    }
}

int MqttCmdMgr::uploadRecordText(Record *record) {
    LOGD("uploadRecordText record id %d\r\n", record->ID);
    char *msgId = MqttCmdMgr::getInstance()->genMsgId();
    char pubMsg[MQTT_AT_CMD_LEN]={0};
//    char pubMsg[8000]={0};
    char *pubTopic = MqttTopicMgr::getInstance()->getPubTopicActionRecord();
    sprintf(pubMsg, "{\\\"id\\\":\\\"%s\\\"\\,\\\"u\\\":\\\"%s\\\"\\,\\\"t\\\":%d}", msgId, record->UUID, record->time_stamp);
//    sprintf(pubMsg, "{\"id\":\"%s\",\"p\":\"%s\",\"t\":%d}", msgId, record->UUID, record->time_stamp);
//    sprintf(pubMsg, "{\"id\":\"%s\",\"p\":\"%s\",\"t\":%d,\"i\":\"%s\"}", msgId, record->UUID, record->time_stamp, imageData);
    LOGD("上传record pub_msg %s \r\n", pubMsg);
    int result = MqttConnMgr::getInstance()->publishMQTT(pubTopic, pubMsg);
//    LOGD("上传record pub_msg %d \r\n", strlen(pubMsg));
//    int result = MqttConnMgr::getInstance()->publishRawMQTT(pubTopic, pubMsg, strlen(pubMsg));
    LOGD("do topic %s result %d\r\n", pubTopic, result);
    if (result == 0) {
        record->upload = RECORD_UPLOAD;
        DBManager::getInstance()->updateRecordByID(record);
        for (int i = 0; i < m_uploading_records.size(); i++) {
            if (m_uploading_records[i] == record->ID) {
                m_uploading_records.erase(m_uploading_records.begin() + i);
                LOGD("uploadRecordText erase id %d uploading_records size %d\r\n", record->ID, m_uploading_records.size());
                break;
            }
        }
    }
    return result;
}

// TODO: image publish
char *imageData = "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";
int MqttCmdMgr::uploadRecordImage(Record *record) {
    LOGD("uploadRecordImage record id %d\r\n", record->ID);
    char *msgId = MqttCmdMgr::getInstance()->genMsgId();
    char *fileName = (char*)(record->image_path);
    int imgFileSize = fatfs_getsize(fileName);
    int imgBufferSize = (imgFileSize / 4 + 1) * 4;
    char *imgBuffer = (char*)pvPortMalloc(imgBufferSize);
    if (NULL == imgBuffer) {
        LOGD("Failed to alloc buffer for image\r\n");
        return -1;
    }
    int result = fatfs_read(fileName, imgBuffer, 0, imgFileSize);

    int base64BufferSize = (imgFileSize / 3 + 1) * 4 + 1;
    char *base64Buffer = (char*)pvPortMalloc(base64BufferSize);
    if (NULL == base64Buffer) {
        LOGD("Failed to alloc base64 buffer for image\r\n");
        return -1;
    }
    base64_encode(imgBuffer, base64Buffer, imgFileSize);
    int base64Len = strlen(base64Buffer);
    LOGD("img size %d base64 size %d base64 len %d\r\n", imgFileSize, base64BufferSize, base64Len);

//    char pubMsg[8000]={0};
    char *pubMsg = (char*)pvPortMalloc(base64BufferSize + 100);
    char *pubTopic = MqttTopicMgr::getInstance()->getPubTopicActionRecord();
    sprintf(pubMsg, "{\"id\":\"%s\",\"u\":\"%s\",\"t\":%d,\"i\":\"%s\"}", msgId, record->UUID, record->time_stamp, base64Buffer);
    LOGD("上传record pub_msg %d \r\n", strlen(pubMsg));
    result = MqttConnMgr::getInstance()->publishRawMQTT(pubTopic, pubMsg, strlen(pubMsg));
    LOGD("do topic %s result %d\r\n", pubTopic, result);
    if (result == 0) {
        record->upload = BOTH_UPLOAD;
        DBManager::getInstance()->updateRecordByID(record);
    }

    vPortFree(pubMsg);
    vPortFree(base64Buffer);
    vPortFree(imgBuffer);
    return 0;
}

int MqttCmdMgr::pushRecord(Record *record, int cmdType, int priority, int force) {
    int id = record->ID;
    if (!force) {
        for (int i = 0; i < m_uploading_records.size(); i++) {
            if (m_uploading_records[i] == id) {
                LOGD("pushRecrod find id %d\r\n", id);
                return -1;
            }
        }
    }
    m_uploading_records.push_back(id);
    char rid[12] = {0};
    sprintf(rid, "%d", id);
    MqttCmd *mqttCmd = new MqttCmd(priority, cmdType, rid, rid);
    m_mqtt_cmds.Push(mqttCmd);
    return 0;
}

int MqttCmdMgr::pushRecords(int uploadStatus, int cmdType, int maxCount) {
    list<Record*> records = DBManager::getInstance()->getAllUnuploadRecord();
    list <Record*>::iterator it;
    int count = 0;
    LOGD("pushRecords uploadStatus %d cmdType %d size %d\r\n", uploadStatus, cmdType, records.size());
    for ( it = records.begin( ); it != records.end( ); it++ ) {
        Record *record = (Record * ) * it;
        if (record->upload == uploadStatus) {
            int result = pushRecord(record, cmdType);
            if (result != 0) {
                continue;
            }
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
        if (!MqttConnMgr::getInstance()->isMqttConnected()) {
            // If MQTT is not connected, just delay 1 sec
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        int recordCount = pushRecords(BOTH_UNUPLOAD, CMD_TYPE_RECORD_TEXT, 20);
        if (recordCount > 0) {
            vTaskDelay(pdMS_TO_TICKS(20*recordCount));
            continue;
        }

        recordCount = pushRecords(RECORD_UPLOAD, CMD_TYPE_RECORD_IMAGE, 10);
        if (recordCount > 0) {
            vTaskDelay(pdMS_TO_TICKS(150*recordCount));
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

void MqttCmdMgr::requestFeature(char *uuid) {
    MqttCmd *mqttCmd = new MqttCmd(PRIORITY_HIGH, CMD_TYPE_FEATURE_UPLOAD, uuid, uuid);
    m_mqtt_cmds.Push(mqttCmd);
}

void MqttCmdMgr::doHeartBeat() {
    m_is_heart_beating = true;
    MqttConnMgr::getInstance()->updateWifiRSSI();

    static int heartbeatIdx = 1;
    char pubMsg[MQTT_AT_CMD_LEN] = {0};
    char *msgId = MqttCmdMgr::getInstance()->genMsgId();
    sprintf(pubMsg,
            "{\\\"id\\\":\\\"%s\\\"\\,\\\"ts\\\":%d\\,\\\"wr\\\":%d\\,\\\"idx\\\":%d\\,\\\"ver\\\":\\\"%s\\\"}",
            msgId, ws_systime, MqttConnMgr::getInstance()->getWifiRSSI(), heartbeatIdx++, "");
    char *topic = MqttTopicMgr::getInstance()->getPubTopicHeartBeat();
    int result = MqttConnMgr::getInstance()->publishMQTT(topic, pubMsg);
    if (result != 0) {
        if (m_hb_fail_count++ > 2) {
            MqttConnMgr::getInstance()->reconnectMqttAsync();
        }
    } else {
        m_hb_fail_count = 0;
    }
}

void MqttCmdMgr::heartBeat() {
    if (m_is_heart_beating) {
        return;
    }
    MqttCmd *mqttCmd = new MqttCmd(PRIORITY_LOWEST, CMD_TYPE_HEART_BEAT, "", "");
    m_mqtt_cmds.Push(mqttCmd);
}
