#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mqtt-common.h"
//#include "log.h"
#include "fsl_log.h"
#include "config.h"
#include "mqtt_topic_mgr.h"

#include <ctype.h>

MqttTopicMgr::MqttTopicMgr() {
    reloadTopic();
}

void MqttTopicMgr::reloadTopic() {
    memset(m_sub_cmd_request, 0, MQTT_AT_TOPIC_LEN);
    memset(m_sub_feature_request, 0, MQTT_AT_TOPIC_LEN);
    memset(m_sub_feature_download, 0, MQTT_AT_TOPIC_LEN);
    memset(m_pub_heart_beat, 0, MQTT_AT_TOPIC_LEN);
    memset(m_pub_cmd_response, 0, MQTT_AT_TOPIC_LEN);
    memset(m_pub_feature_upload, 0, MQTT_AT_TOPIC_LEN);
    memset(m_pub_action_record, 0, MQTT_AT_TOPIC_LEN);
    memset(m_pub_action_image, 0, MQTT_AT_TOPIC_LEN);

    sprintf(m_sub_cmd_request, "WAVE/S/%s/C/REQ", mqttConfig.username);
    sprintf(m_sub_feature_request, "WAVE/S/%s/F/R", mqttConfig.username);
    sprintf(m_sub_feature_download, "WAVE/S/%s/F/D", mqttConfig.username);
    sprintf(m_pub_heart_beat, "WAVE/S/%s/D/HB", mqttConfig.username);
    sprintf(m_pub_cmd_response, "WAVE/S/%s/C/RSP", mqttConfig.username);
    sprintf(m_pub_feature_upload, "WAVE/S/%s/F/U", mqttConfig.username);
    sprintf(m_pub_action_record, "WAVE/S/%s/A/R", mqttConfig.username);
    sprintf(m_pub_action_image, "WAVE/S/%s/A/I", mqttConfig.username);
}

/*** 订阅TOPIC ***/
// 后台命令
char* MqttTopicMgr::getSubTopicCmdRequest() {
//    return generateTopic("WAVE/S/%s/C/REQ");
    return m_sub_cmd_request;
}

// face feature 人脸特征上传请求
char* MqttTopicMgr::getSubTopicFeatureRequest() {
//    return generateTopic("WAVE/S/%s/F/R");
    return m_sub_feature_request;
}

// face feature 人脸特征下发命令
char* MqttTopicMgr::getSubTopicFeatureDownload() {
//    return generateTopic("WAVE/S/%s/F/D");
    return m_sub_feature_download;
}

/*** 发布TOPIC ***/
// 心跳
char* MqttTopicMgr::getPubTopicHeartBeat() {
//    return generateTopic("WAVE/S/%s/D/HB");
    return m_pub_heart_beat;
}

// 命令返回给mqtt后台
char* MqttTopicMgr::getPubTopicCmdResponse() {
//    return generateTopic("WAVE/S/%s/C/RSP");
    return m_pub_cmd_response;
}

// face feature 人脸特征上传后台
char* MqttTopicMgr::getPubTopicFeatureUpload() {
//    return generateTopic("WAVE/S/%s/F/U");
    return m_pub_feature_upload;
}

// 人脸操作记录上报
char* MqttTopicMgr::getPubTopicActionRecord() {
//    return generateTopic("WAVE/S/%s/A/R");
    return m_pub_action_record;
}

// 人脸操作图片上传
char* MqttTopicMgr::getPubTopicActionImage() {
//    return generateTopic("WAVE/S/%s/A/I");
    return m_pub_action_image;
}

