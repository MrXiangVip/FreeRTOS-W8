#ifndef _MQTT_TOPIC_MGR_H_
#define _MQTT_TOPIC_MGR_H_

#include "mqtt-topic.h"
#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

class MqttTopicMgr {
private:
    MqttTopicMgr();
    char m_sub_cmd_request[MQTT_AT_TOPIC_LEN];
    char m_sub_feature_request[MQTT_AT_TOPIC_LEN];
    char m_sub_feature_download[MQTT_AT_TOPIC_LEN];
    char m_pub_heart_beat[MQTT_AT_TOPIC_LEN];
    char m_pub_cmd_response[MQTT_AT_TOPIC_LEN];
    char m_pub_feature_upload[MQTT_AT_TOPIC_LEN];
    char m_pub_action_record[MQTT_AT_TOPIC_LEN];
    char m_pub_action_image[MQTT_AT_TOPIC_LEN];
public:
    static MqttTopicMgr *getInstance() {
        static MqttTopicMgr m_instance;
        return &m_instance;
    };

    void reloadTopic();

    /*** 订阅TOPIC ***/
    // 后台命令
    char *getSubTopicCmdRequest();
    // face feature 人脸特征上传请求
    char *getSubTopicFeatureRequest();
    // face feature 人脸特征下发命令
    char *getSubTopicFeatureDownload();

    /*** 发布TOPIC ***/
    // 心跳
    char *getPubTopicHeartBeat();
    // 命令返回给mqtt后台
    char *getPubTopicCmdResponse();
    // face feature 人脸特征上传后台
    char *getPubTopicFeatureUpload();
    // 人脸操作记录上报
    char *getPubTopicActionRecord();
    // 人脸操作图片上传
    char *getPubTopicActionImage();
};

#ifdef __cplusplus
}
#endif

#endif
