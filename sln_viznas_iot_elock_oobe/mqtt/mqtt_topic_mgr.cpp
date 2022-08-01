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

/*** 订阅TOPIC ***/
// 后台命令
char* MqttTopicMgr::getSubTopicCmdRequest() {
    return generateTopic("WAVE/S/%s/C/REQ");
}

// face feature 人脸特征上传请求
char* MqttTopicMgr::getSubTopicFeatureRequest() {
    return generateTopic("WAVE/S/%s/F/R");
}

// face feature 人脸特征下发命令
char* MqttTopicMgr::getSubTopicFeatureDownload() {
    return generateTopic("WAVE/S/%s/F/D");
}

/*** 发布TOPIC ***/
// 心跳
char* MqttTopicMgr::getPubTopicHeartBeat() {
    return generateTopic("WAVE/S/%s/D/HB");
}

// 命令返回给mqtt后台
char* MqttTopicMgr::getPubTopicCmdResponse() {
    return generateTopic("WAVE/S/%s/C/RSP");
}

// face feature 人脸特征上传后台
char* MqttTopicMgr::getPubTopicFeatureUpload() {
    return generateTopic("WAVE/S/%s/F/U");
}

// 人脸操作记录上报
char* MqttTopicMgr::getPubTopicActionRecord() {
    return generateTopic("WAVE/S/%s/A/R");
}

// 人脸操作图片上传
char* MqttTopicMgr::getPubTopicActionImage() {
    return generateTopic("WAVE/S/%s/A/I");
}

char* MqttTopicMgr::generateTopic(char *cmd) {
	memset(m_topic, '\0', sizeof(m_topic));
	sprintf(m_topic, cmd, mqttConfig.username);
	LOGD("Topic %s generated\r\n", m_topic);
	return m_topic;
}
