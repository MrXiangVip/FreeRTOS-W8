//
// Created by wszgx on 2022/7/29.
//

#ifndef _MQTT_TEST_MGR_H_
#define _MQTT_TEST_MGR_H_

#include "mqtt-instruction-pool.h"

class MqttTestMgr {
private:
    MqttTestMgr() {};
public:
    static MqttTestMgr *getInstance() {
        static MqttTestMgr m_instance;
        return &m_instance;
    };

    // Test Entry
    void doTest(int argc, char *cmd, char *data, char *extra);
    void testIdle();

    void addRecord(int argc, char *data, char *extra);
    void listRecords();
    void pubRaw(int argc, char *data);
};

#endif //_MQTT_TEST_MGR_H_
