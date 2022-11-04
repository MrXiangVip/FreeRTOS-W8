//
// Created by wszgx on 2021/4/21.
//

#ifndef CJSON_MQTT_INSTRUCTION_POOL_H
#define CJSON_MQTT_INSTRUCTION_POOL_H

#include <map>
#include "mqtt-instruction.h"

using namespace::std;

class MqttInstructionPool {
private:
    map<int, MqttInstruction> m_mqtt_instructions;
public:
    static MqttInstructionPool *getInstance() {
        static MqttInstructionPool m_instance;
        return &m_instance;
    };
    int insertMqttInstruction(MqttInstruction mqtt_instruction);
    int insertOrUpdateMqttInstruction(MqttInstruction mqtt_instruction);
    int removeMqttInstruction(int cmd_index);
    int removeMqttInstruction(char instruction_dest, char cmd_code);
    void tick();
    char* getMsgId(char type_code, char cmd_code);
};


#endif //CJSON_MQTT_INSTRUCTION_POOL_H
