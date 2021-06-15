//
// Created by wszgx on 2021/4/21.
//

#ifndef CJSON_MQTT_INSTRUCTION_POOL_H
#define CJSON_MQTT_INSTRUCTION_POOL_H

#include <map>
#include "mqtt-instruction.h"

using namespace::std;

class MqttInstructionPool {
    map<int, MqttInstruction> m_mqtt_instructions;
public:
    int insertMqttInstruction(MqttInstruction mqtt_instruction);
    int insertOrUpdateMqttInstruction(MqttInstruction mqtt_instruction);
    int removeMqttInstruction(int cmd_index);
    int tick();
    char* getMsgId(char type_code, char cmd_code);
};


#endif //CJSON_MQTT_INSTRUCTION_POOL_H
