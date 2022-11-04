//
// Created by wszgx on 2021/4/21.
//

#include "mqtt-instruction-pool.h"
#include <stdio.h>
//#include "mqtt-mq.h"
#include <stdlib.h>
#include <string.h>
//#include "log.h"
#include "fsl_log.h"
#include "wifi_main.h"
#include "mqtt_cmd_mgr.h"

int MqttInstructionPool::insertMqttInstruction(MqttInstruction mqtt_instruction) {
    int cmd_index = mqtt_instruction.getCmdIndex();
	map<int, MqttInstruction>::iterator iter = m_mqtt_instructions.find(cmd_index);
	if(iter != m_mqtt_instructions.end()){
		LOGD("key %d exist\r\n", cmd_index);
        return -1;
    }
    m_mqtt_instructions.insert(pair<int, MqttInstruction>(cmd_index, mqtt_instruction));
    LOGD("insert key %d success\r\n", cmd_index);
	return 0;
}

int MqttInstructionPool::insertOrUpdateMqttInstruction(MqttInstruction mqtt_instruction) {
    int cmd_index = mqtt_instruction.getCmdIndex();
	map<int, MqttInstruction>::iterator iter = m_mqtt_instructions.find(cmd_index);
	if(iter != m_mqtt_instructions.end()){
		LOGD("key %d exist\r\n", cmd_index);
		iter->second = mqtt_instruction;
        return -1;
    }
	LOGD("insert key %d success\r\n", cmd_index);
    m_mqtt_instructions.insert(pair<int, MqttInstruction>(cmd_index, mqtt_instruction));
	return 0;
}

int MqttInstructionPool::removeMqttInstruction(char instruction_dest, char cmd_code) {
    int cmd_index = MqttInstruction::getCmdIndex(instruction_dest, cmd_code);
    return removeMqttInstruction(cmd_index);
}

int MqttInstructionPool::removeMqttInstruction(int cmd_index) {
    map<int, MqttInstruction>::iterator key = m_mqtt_instructions.find(cmd_index);
    if(key!=m_mqtt_instructions.end()) {
        m_mqtt_instructions.erase(key);
        LOGD("remove key %d success\r\n", cmd_index);
        return 0;
    }
    LOGD("remove key %d not found\r\n", cmd_index);
    return -1;
}

char* MqttInstructionPool::getMsgId(char type_code, char cmd_code) {
    int cmd_index = MqttInstruction::getCmdIndex(type_code, cmd_code);
    LOGD("mqtt_instruction_pool getMsgId %d start\n", cmd_index);
    map<int, MqttInstruction>::iterator key;// = m_mqtt_instructions.find(cmd_index);
    for(key = m_mqtt_instructions.begin(); key != m_mqtt_instructions.end(); key ++) {
        int index = key->first;
        //LOGD("%s key->first is %d\n", __FUNCTION__, index);
        if (index == cmd_index) {
            //LOGD("mqtt_instruction_pool getMsgId %d found\n", cmd_index);
            MqttInstruction mqttInstruction = key->second;
            LOGD("getMsgId %d found msgId %s\n", cmd_index, mqttInstruction.getMsgId());
            return mqttInstruction.getMsgId();
        }
    }
    /*if(key!=m_mqtt_instructions.end()) {
    	LOGD("mqtt_instruction_pool getMsgId %d found\n", cmd_index);
        MqttInstruction mqttInstruction = key->second;
        LOGD("mqtt_instruction_pool getMsgId %d found msgId %s\n", cmd_index, mqttInstruction.getMsgId());
        return mqttInstruction.getMsgId();
    }*/
    LOGD("getMsgId %d not found\r\n", cmd_index);
    return "";
}

void MqttInstructionPool::tick() {
    map<int, MqttInstruction>::iterator it;
    int i = 0;
    for(it = m_mqtt_instructions.begin(); it != m_mqtt_instructions.end();)// ++it)
    {
        MqttInstruction mqttInstruction = it->second;
        mqttInstruction.tick();
        cout<<mqttInstruction<<endl;
        int timeout = mqttInstruction.getTimeout();
        it->second = mqttInstruction;
        if (timeout <= 0) {
            MqttCmdMgr::getInstance()->atCmdResponse(AT_RSP_TIMEOUT, mqttInstruction.getMsgId());
			LOGD("key %d timeout\n", mqttInstruction.getCmdIndex());
            m_mqtt_instructions.erase(it++);
        } else {
            LOGD("++id\n");
            ++it;
        }
    }
}

