//
// Created by wszgx on 2022/7/29.
//

#ifndef _MQTT_TEST_MGR_H_
#define _MQTT_TEST_MGR_H_

#include "mqtt-instruction-pool.h"
#include <vector>
#include <string.h>

#define TEST_CMD_LEN 16
#define TEST_HELP_LEN 64
typedef void (*TestHandler)(char *cmd, char *usage, int argc, char *data, char *extra);

//void testAddRecord(char *cmd, char *usage, int argc, char *data, char *extra);

class MqttTest {
private:
    char m_cmd[TEST_CMD_LEN];
    int m_argc;
    char m_usage[TEST_HELP_LEN];
//    void (*handler)(int argc, char *data, char *extra);
    TestHandler m_handler;
public:
    MqttTest() {}
    MqttTest(char *cmd, int argc, char *usage, TestHandler handler) {
        strcpy(m_cmd, cmd);
        m_argc = argc;
        strcpy(m_usage, usage);
        m_handler = handler;
    }
    void setCmd(char *cmd) { strcpy(m_cmd, cmd); }
    void setArgc(int argc) { m_argc = argc; }
    void setHelp(char *usage) { strcpy(m_usage, usage); }
    void setHandler(TestHandler handler) { m_handler = handler; }
    char* getCmd() { return m_cmd; }
    int getArgc() { return m_argc; }
    char* getUsage() { return m_usage; }
    TestHandler getHandler() { return m_handler; }
};

class MqttTestMgr {
private:
    MqttTestMgr();
    vector<MqttTest> m_mqtt_tests;
public:
    static MqttTestMgr *getInstance() {
        static MqttTestMgr m_instance;
        return &m_instance;
    };

    // Test Entry
    void doTest(int argc, char *cmd, char *data, char *extra);
    vector<MqttTest> getCmds() { return m_mqtt_tests; }

    static void help(char *cmd = NULL, char *usage = NULL, int argc = 0, char *data = NULL, char *extra = NULL);
    static void addRecord(char *cmd, char *usage, int argc, char *data, char *extra);
    static void listRecords(char *cmd, char *usage, int argc, char *data, char *extra);
    static void pubRaw(char *cmd, char *usage, int argc, char *data, char *extra);
    static void setWifi(char *cmd, char *usage, int argc, char *data, char *extra);
    static void setMqtt(char *cmd, char *usage, int argc, char *data, char *extra);
    static void reconn(char *cmd, char *usage, int argc, char *data, char *extra);
    static void printConfig(char *cmd, char *usage, int argc, char *data, char *extra);
};

#endif //_MQTT_TEST_MGR_H_
