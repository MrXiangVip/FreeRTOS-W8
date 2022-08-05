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
#include "mqtt-remote-feature.h"
#include "base64.h"
#include "util.h"
#include "MCU_UART5.h"
#include "MCU_UART5_Layer.h"
#include "DBManager.h"

#define TEST_ADD_RECORD     "addrecord"
#define TEST_LIST_RECORD    "listrecord"
#define TEST_PUB_RAW        "pub_raw"

void MqttTestMgr::doTest(int argc, char *cmd, char *data, char *extra) {
    if (strcmp(cmd, TEST_ADD_RECORD) == 0) {
        addRecord(argc, data, extra);
    } else if (strcmp(cmd, TEST_LIST_RECORD) == 0) {
        listRecords();
    } else if (strcmp(cmd, TEST_PUB_RAW) == 0) {
        pubRaw(argc, data);
    } else {
        testIdle();
    }
}

void MqttTestMgr::testIdle() {
    char *data = (char*)pvPortMalloc(24);
    strcpy(data, "hello");
    LOGD("test idle pvPortMalloc %s\r\n", data);
    vPortFree(data);
}

// add a user register/recognize record
void MqttTestMgr::addRecord(int argc, char *data, char *extra) {
    if (argc != 4) {
        LOGE("test addrecord UUID [0 REG|1 UNLOCK]\r\n");
        return;
    }
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

void MqttTestMgr::listRecords() {
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

void MqttTestMgr::pubRaw(int argc, char *data) {
    if (argc < 3 || data == NULL) {
        LOGE("test pubraw [data]\r\n");
        return;
    }
    char *pubTopic = MqttTopicMgr::getInstance()->getPubTopicActionRecord();
    int result = MqttConnMgr::getInstance()->publishRawMQTT(pubTopic, data, strlen(data));
    LOGD("do pubRaw result %d\r\n", result);
}
