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

void MqttCmdMgr::atCmdResponse(int result, char *msgId, char *rspMsg) {
    char pubMsg[MQTT_AT_CMD_LEN] = {0};
    sprintf(pubMsg,
            "{\\\"id\\\":\\\"%s\\\"\\,\\\"res\\\":%d\\,\\\"msg\\\":\\\"%s\\\"}",
            msgId, result, rspMsg);
    char *topic = MqttTopicMgr::getInstance()->getPubTopicCmdResponse();
    MqttCmd *mqttCmd = new MqttCmd(1, 0, topic, pubMsg);
    m_mqtt_cmds.Push(mqttCmd);
    LOGD("push size %d\r\n", m_mqtt_cmds.Size());
//    int result = MqttConnMgr::getInstance()->publishMQTT(topic, pubMsg);
//    LOGD("do atCmdResponse result %d\r\n", result);
}

void MqttCmdMgr::loopSendMqttMsgs() {
    for(;;) {
        while (!m_mqtt_cmds.Empty())
        {
            MqttCmd mqttCmd = m_mqtt_cmds.Top();
            m_mqtt_cmds.Pop();
            int result = MqttConnMgr::getInstance()->publishMQTT(mqttCmd.getTopic(), mqttCmd.getData());
            LOGD("do topic %s result %d\r\n", mqttCmd.getTopic(), result);
            mqttCmd.free();
        }
        vTaskDelay(pdMS_TO_TICKS(50));
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
