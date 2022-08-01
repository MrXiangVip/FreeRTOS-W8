#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "mqtt-common.h"
#include "fsl_log.h"
#include "config.h"
#include "mqtt_cmd.h"
#include "mqtt_topic_mgr.h"
#include "mqtt_conn_mgr.h"
#include "MCU_UART5_Layer.h"
#include "PriorityQueue.h"

MqttCmd::MqttCmd(int priority, int cmdType, char *topic, char *data) {
    m_priority = priority;
    m_cmd_type = cmdType;
    m_topic = (char*)pvPortMalloc(strlen(topic) + 1);
    m_data = (char*)pvPortMalloc(strlen(data) + 1);
    strcpy(m_topic, topic);
    strcpy(m_data, data);
}

void MqttCmd::free() {
    if (m_topic) {
        vPortFree(m_topic);
        m_topic = NULL;
    }
    if (m_data) {
        vPortFree(m_data);
        m_data = NULL;
    }
}

bool MqttCmd::operator > (const MqttCmd& mc)
{
    if(this->m_priority > mc.m_priority) {
        return true;
    } else {
        return false;
    }
}

bool MqttCmd::operator < (const MqttCmd& mc)
{
    if(this->m_priority < mc.m_priority) {
        return true;
    } else {
        return false;
    }
}
