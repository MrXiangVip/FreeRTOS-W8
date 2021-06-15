//
// Created by wszgx on 2021/4/21.
//
#include <stdlib.h>
#include <string.h>

#include "mqtt-instruction.h"
//#include "log.h"
#include "fsl_log.h"
MqttInstruction::MqttInstruction(char type_code, char cmd_code, int timeout, char *msg_id) {
    this->m_type_code = type_code;
    this->m_cmd_code = cmd_code;
    this->m_timeout = timeout;
    this->m_cmd_index = MqttInstruction::getCmdIndex(type_code, cmd_code);
//    this->m_msg_id = msg_id;
    memset(this->m_msg_id, '0', sizeof(this->m_msg_id));
    strcpy(this->m_msg_id, msg_id);
}

int MqttInstruction::getCmdIndex(char type_code, char cmd_code) {
    return (int)type_code * 1000 + (int)cmd_code;
}

int MqttInstruction::getCmdIndex() {
    return this->m_cmd_index;
}

int MqttInstruction::getTimeout() {
    return this->m_timeout;
}

void MqttInstruction::tick() {
    this->m_timeout = this->m_timeout - 1;
}

char *MqttInstruction::getMsgId() {
	LOGD("MqttInstruction getMsg Id %s", this->m_msg_id);
    return this->m_msg_id;
}
