//
// Created by wszgx on 2022/7/29.
//

#include <stdio.h>
#include "mqtt_test_mgr.h"

#include "aw_wstime.h"
#include "mqtt-common.h"
#include "mqtt_util.h"
#include "fsl_log.h"
#include "mqtt-topic.h"
#include "cJSON.h"
#include "mqtt-mcu.h"
#include "mqtt_topic_mgr.h"
#include "mqtt_cmd_mgr.h"
#include "mqtt_conn_mgr.h"
#include "mqtt_feature_mgr.h"
#include "mqtt_manager.h"
#include "mqtt-remote-feature.h"
#include "base64.h"
#include "util.h"
#include "MCU_UART5.h"
#include "MCU_UART5_Layer.h"
#include "DBManager.h"
#include "config.h"

#define TEST_CMD_DEFINE(cmd, argc, usage, callback) { MqttTest mqttTest_##cmd((#cmd), (argc), (usage), (callback)); m_mqtt_tests.push_back(mqttTest_##cmd); }

//void testAddRecord(char *cmd, char *usage, int argc, char *data, char *extra) {
//    LOGE("%s\r\n", usage);
//}

MqttTestMgr::MqttTestMgr() {
    // utils
    TEST_CMD_DEFINE(help, 2, (char*)("test help"), &MqttTestMgr::help);
    TEST_CMD_DEFINE(gettime, 2, (char*)("test gettime"), &MqttTestMgr::getTime);
    TEST_CMD_DEFINE(settime, 3, (char*)("test settime timestamp"), &MqttTestMgr::setTime);

    // records
    TEST_CMD_DEFINE(addrecord, 4, (char*)("test addrecord UUID [0 REG|1 UNLOCK]"), &MqttTestMgr::addRecord);
    TEST_CMD_DEFINE(listrecords, 2, (char*)("test listrecords"), &MqttTestMgr::listRecords);

    // mqtt
    TEST_CMD_DEFINE(pubraw, 3, (char*)("test pubraw [SIZE|data]"), &MqttTestMgr::pubRaw);
    TEST_CMD_DEFINE(upload, 3, (char*)("test upload uuid"), &MqttTestMgr::uploadFeature);
    TEST_CMD_DEFINE(pubimg , 3, (char*)("test pubimg filename"), &MqttTestMgr::pubImage);

    // connectivity
    TEST_CMD_DEFINE(setwifi, 4, (char*)("test setwifi ssid password"), &MqttTestMgr::setWifi);
    TEST_CMD_DEFINE(setmqtt, 4, (char*)("test setmqtt [id|port|id|username|password] data"), &MqttTestMgr::setMqtt);
    TEST_CMD_DEFINE(reconn, 3, (char*)("test reconn [wifi|mqtt]"), &MqttTestMgr::reconn);

    // config
    TEST_CMD_DEFINE(printconfig, 2, (char*)("test printconfig"), &MqttTestMgr::printConfig);
    TEST_CMD_DEFINE(saveconfig, 2, (char*)("test saveconfig"), &MqttTestMgr::saveConfig);
    TEST_CMD_DEFINE(switchconfig, 3, (char*)("test switchconfig [w8|r60]"), &MqttTestMgr::switchConfig);

#if 0
//    MqttTest mqttTest((char*)("addrecord"), (char*)("help"), &testAddRecord);
//    m_mqtt_tests.push_back(mqttTest);
//    LOGD("size is %d\r\n", m_mqtt_tests.size());
//
//    MqttTest mqttTest2((char*)("addrecord"), (char*)("help"), &MqttTestMgr::addRecord);
//    m_mqtt_tests.push_back(mqttTest2);
//    LOGD("size is %d\r\n", m_mqtt_tests.size());

//    MqttTest *mqttTest = new MqttTest((char*)("addrecord"), (char*)("help"), testAddRecord);

//    MqttTest *mqttTest = new MqttTest();
//    mqttTest->setCmd((char*)"addrecord");
//    mqttTest->setHelp((char*)"addrecord");
//    mqttTest->setHandler(testAddRecord);
#endif
}

void MqttTestMgr::doTest(int argc, char *cmd, char *data, char *extra) {
    for (std::vector<MqttTest>::iterator it = m_mqtt_tests.begin(); it != m_mqtt_tests.end(); it++)
    {
        if (strcmp(cmd, (*it).getCmd()) == 0) {
            if ((*it).getArgc() > argc) {
                LOGE("%s USAGE:\r\n", cmd);
                LOGE("\t\t%s\r\n", (*it).getUsage());
            } else {
                (*it).getHandler()((*it).getCmd(), (*it).getUsage(), argc, data, extra);
            }
            return;
        }
    }
    MqttTestMgr::help();
}

void MqttTestMgr::setWifi(char *cmd, char *usage, int argc, char *data, char *extra) {
//    strcpy(btWifiConfig.ssid, data);
    update_wifi_ssid(data);
//    strcpy(btWifiConfig.password, extra);
    update_wifi_pwd(extra);
//    reconn(NULL, NULL, 3, "wifi");
}

void MqttTestMgr::setMqtt(char *cmd, char *usage, int argc, char *data, char *extra) {
    if (strcmp("ip", data) == 0) {
        update_mqtt_ip(extra);
//        reconn(NULL, NULL, 3, "mqtt");
//        strcpy(mqttConfig.server_ip, extra);
    } else if (strcmp("port", data) == 0) {
        update_mqtt_port(extra);
//        reconn(NULL, NULL, 3, "mqtt");
//        strcpy(mqttConfig.server_port, extra);
    } else if (strcmp("id", data) == 0) {
        update_mqtt_id(extra);
//        reconn(NULL, NULL, 3, "mqtt");
//        strcpy(mqttConfig.client_id, extra);
    } else if (strcmp("username", data) == 0) {
        update_mqtt_username(extra);
        MqttTopicMgr::getInstance()->reloadTopic();
//        reconn(NULL, NULL, 3, "mqtt");
//        strcpy(mqttConfig.username, extra);
    } else if (strcmp("password", data) == 0) {
        update_mqtt_password(extra);
//        reconn(NULL, NULL, 3, "mqtt");
//        strcpy(mqttConfig.password, extra);
    } else {
        help(cmd);
//        LOGE("%s USAGE:\r\n", cmd);
//        LOGE("\t\t%s\r\n", usage);
    }
}

void MqttTestMgr::help(char *cmd, char *usage, int argc, char *data, char *extra) {
    vector <MqttTest> mqttCmds = MqttTestMgr::getInstance()->getCmds();
    // test / test help help / test help setwifi
    if (cmd != NULL || (strcmp(cmd, "help") == 0 && argc == 3)) {
        for (std::vector<MqttTest>::iterator it = mqttCmds.begin(); it != mqttCmds.end(); it++)
        {
            if (strcmp(cmd, (*it).getCmd()) == 0) {
                LOGE("%s USAGE:\r\n", (*it).getCmd());
                LOGE("\t\t%s\r\n", (*it).getUsage());
                return;
            }
        }

    }

    LOGD("Test Commands:\r\n");
    for (std::vector<MqttTest>::iterator it = mqttCmds.begin(); it != mqttCmds.end(); it++) {
        LOGD("\t%s\r\n", (*it).getCmd());
    }
    for (std::vector<MqttTest>::iterator it = mqttCmds.begin(); it != mqttCmds.end(); it++)
    {
        LOGE("%s USAGE:\r\n", (*it).getCmd());
        LOGE("\t\t%s\r\n", (*it).getUsage());
    }
}

void MqttTestMgr::getTime(char *cmd, char *usage, int argc, char *data, char *extra) {
    struct ws_tm current;
    memset((void*)&current,0x00,sizeof(current));
    ws_localtime(ws_systime + 8*60*60, &current);
    char timeStr[32];
    memset(timeStr, 0, sizeof(timeStr));
    sprintf(timeStr, "%d(%04d-%02d-%02d %02d:%02d:%02d)", ws_systime, current.tm_year, current.tm_mon + 1, current.tm_mday, current.tm_hour, current.tm_min, current.tm_sec);
//    LOGD("%d(%04d-%02d-%02d %02d:%02d:%02d)", ws_systime, current.tm_year, current.tm_mon + 1, current.tm_mday, current.tm_hour, current.tm_min, current.tm_sec);
    LOGD("%s\r\n", timeStr);
}

void MqttTestMgr::setTime(char *cmd, char *usage, int argc, char *data, char *extra) {
    LOGD("before setTime\r\n");
    getTime(cmd, usage, argc, data, extra);
    int result = MqttManager::getInstance()->timeSync(data);
    LOGD("after setTime %s result %d\r\n", data, result);
    getTime(cmd, usage, argc, data, extra);
}

// add a user register/recognize record
void MqttTestMgr::addRecord(char *cmd, char *usage, int argc, char *data, char *extra) {
    Record *record = (Record *) pvPortMalloc(sizeof(Record));
    memset(record, 0, sizeof(Record));
    strcpy(record->UUID, data);
    record->action = atoi(extra);// 0 代表注册
    record->time_stamp = ws_systime;//当前时间
//        memset(image_path, 0, sizeof(image_path)); // 对注册成功的用户保存一张压缩过的jpeg图片
//        snprintf(image_path, sizeof(image_path), "%x.jpg", record->time_stamp);
//        memcpy(record->image_path, image_path, sizeof(image_path));//image_path
    record->data[0]=0xFF;
    record->data[1]=0xFF;

    record->upload = BOTH_UNUPLOAD;// 0代表没上传 1代表记录上传图片未上传 2代表均已
    LOGD("%s往数据库中插入本次注册记录\r\n", "do_test");
    DBManager::getInstance()->addRecord(record);
}

void MqttTestMgr::listRecords(char *cmd, char *usage, int argc, char *data, char *extra) {
    LOGD("List Records start\r\n");
    list<Record*> records = DBManager::getInstance()->getAllUnuploadRecord();
    LOGD("idx %8s %20s %6s %16s %10s %6s %32s \r\n", "id", "UUID", "action", "image", "timestamp", "upload", "data");
    // 第一步，只上传未上传的注册记录以及图片，涉及到可能存在的重复上传问题: 注册优先
    list <Record*>::iterator it;
    //for (int i = 0; i < recordNum; i++) {
    int i = 1;
    for ( it = records.begin( ); it != records.end( ); it++ ) {
        Record *record = (Record *) *it;
        LOGD("%3d %8d %20s %06d %d %s %6d %s\r\n", i++, record->ID, record->UUID, record->action, record->time_stamp,
             record->image_path, record->upload, record->data);
    }
}

void MqttTestMgr::pubRaw(char *cmd, char *usage, int argc, char *data, char *extra) {
    int result = 0;
    int size = atoi(data);
    char *pubTopic = MqttTopicMgr::getInstance()->getPubTopicActionRecord();
    if (size == 0) {
        result = MqttConnMgr::getInstance()->publishRawMQTT(pubTopic, data, strlen(data));
    } else {
        char *dataGen = (char*)pvPortMalloc(size + 1);
        memset(dataGen, '1', size);
        dataGen[size] = '\0';
        result = MqttConnMgr::getInstance()->publishRawMQTT(pubTopic, dataGen, strlen(dataGen));
    }
    LOGD("do pubRaw %s result %d\r\n", data, result);
}

void MqttTestMgr::uploadFeature(char *cmd, char *usage, int argc, char *data, char *extra) {
    int result = MqttFeatureMgr::getInstance()->uploadFeature(data);
    LOGD("do uploadFeature result %d\r\n", result);
}

void MqttTestMgr::pubImage(char *cmd, char *usage, int argc, char *data, char *extra) {
    int result = 0;
    LOGD("do pubImage %s result %d\r\n", data, result);
    int fileSize = fatfs_getsize(data);
    LOGD("%s size is %d\r\n", pubImage, fileSize);
    if(fileSize == 0) {
        return;
    }
    int size = 18000;
    int sentLen = 0;
    int leftLen = fileSize;
    while (sentLen < fileSize) {
        //分配内存存储整个图片
        char *buffer = (char *) pvPortMalloc((size / 4 + 1) * 4);
        if (NULL == buffer) {
            LOGD("memory_error1");
            return;
        }
        result = fatfs_read(data, buffer, sentLen, size);
        sentLen += size;
        leftLen -= size;

        //base64编码
        char *buffer1 = (char *) pvPortMalloc((size / 3 + 1) * 4 + 1);
        if (NULL == buffer1) {
            LOGD("memory_error2");
            vPortFree(buffer);
            return;
        }
        char *ret1 = base64_encode(buffer, buffer1, size);
        char *pubTopic = MqttTopicMgr::getInstance()->getPubTopicActionRecord();
        MqttConnMgr::getInstance()->publishRawMQTT(pubTopic, buffer1, strlen(buffer1));
        vPortFree(buffer1);
        vPortFree(buffer);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void MqttTestMgr::reconn(char *cmd, char *usage, int argc, char *data, char *extra) {
    if (strcmp(data, "wifi") == 0) {
        MqttConnMgr::getInstance()->reconnectWifiAsync();
    } else if (strcmp(data, "mqtt") == 0) {
        MqttConnMgr::getInstance()->reconnectMqttAsync();
    } else {
        help(cmd);
    }
}

void MqttTestMgr::printConfig(char *cmd, char *usage, int argc, char *data, char *extra) {
    print_project_config();
}

void MqttTestMgr::saveConfig(char *cmd, char *usage, int argc, char *data, char *extra) {
    save_json_config_file();
}

void MqttTestMgr::switchConfig(char *cmd, char *usage, int argc, char *data, char *extra) {
    if (strcmp(data, "w8") == 0) {
        setWifi(NULL, NULL, 4, "guest", "wave202205");
//        strcpy(btWifiConfig.ssid, "guest");
//        strcpy(btWifiConfig.password, "wave202205");

        setMqtt(NULL, NULL, 4, "ip", "mqtt.wavewisdom.com");
        setMqtt(NULL, NULL, 4, "port", "2883");
        setMqtt(NULL, NULL, 4, "id", "F9BA971B147D");
        setMqtt(NULL, NULL, 4, "username", "F9BA971B147D");
        setMqtt(NULL, NULL, 4, "password", "dQEtim6M");
//        reconn(NULL, NULL, 3, "wifi");
//        strcpy(mqttConfig.server_ip, "mqtt.wavewisdom.com");
//        strcpy(mqttConfig.server_port, "2883");
//        strcpy(mqttConfig.client_id, "F9BA971B147D");
//        strcpy(mqttConfig.username, "F9BA971B147D");
//        strcpy(mqttConfig.password, "dQEtim6M");
    } else if (strcmp(data, "r60") == 0) {
        setWifi(NULL, NULL, 4, "wave-soft", "wave.123456");
//        strcpy(btWifiConfig.ssid, "wave-soft");
//        strcpy(btWifiConfig.password, "wave.123456");

        setMqtt(NULL, NULL, 4, "ip", "10.0.14.90");
        setMqtt(NULL, NULL, 4, "port", "1883");
        setMqtt(NULL, NULL, 4, "id", "F9BA971B147D");
        setMqtt(NULL, NULL, 4, "username", "F9BA971B147D");
        setMqtt(NULL, NULL, 4, "password", "dQEtim6M");
//        reconn(NULL, NULL, 3, "wifi");
//        strcpy(mqttConfig.server_ip, "10.0.14.90");
//        strcpy(mqttConfig.server_port, "1883");
//        strcpy(mqttConfig.client_id, "F9BA971B147D");
//        strcpy(mqttConfig.username, "F9BA971B147D");
//        strcpy(mqttConfig.password, "dQEtim6M");
    } else {
        help(cmd);
    }
}
