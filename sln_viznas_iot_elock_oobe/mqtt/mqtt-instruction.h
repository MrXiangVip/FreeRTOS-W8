//
// Created by wszgx on 2021/4/21.
//

#ifndef CJSON_MQTT_INSTRUCTION_H
#define CJSON_MQTT_INSTRUCTION_H

#include <iostream>
#include <string>
using namespace::std;

#define	MQTT_INSTRUCTION_MSG_ID_LEN		64

class MqttInstruction {
    char m_type_code; // 0x23 for mcu; 0x24 for face_loop; 0x25 for mqtt
    char m_cmd_code;
    int m_timeout;
    int m_cmd_index;
    // string m_msg_id;
    char m_msg_id[MQTT_INSTRUCTION_MSG_ID_LEN];

public:
    MqttInstruction(char type_code, char cmd_code, int timeout, char *msg_id);
    static int getCmdIndex(char type_code, char cmd_code);
    int getCmdIndex();
    void tick();
    int getTimeout();
    char *getMsgId();

    friend ostream& operator<<(ostream & os,const MqttInstruction &mqttInstruction) {
        os << "MqttInstruction:" << endl;
        os << "\tcmd_index: "<< mqttInstruction.m_cmd_index << endl;
        os << "\ttimeout: "<< mqttInstruction.m_timeout << endl;
        os << "\tmsg_id: "<< mqttInstruction.m_msg_id << endl;
        return os;
    }

};


#endif //CJSON_MQTT_INSTRUCTION_H
