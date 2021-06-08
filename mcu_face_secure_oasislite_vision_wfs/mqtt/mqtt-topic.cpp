#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "mqtt-common.h"
//#include "log.h"
#include "fsl_log.h"
#include "config.h"
#include "mqtt-topic.h"

#include <ctype.h>

char *generate_topic(char *cmd) {
	char *topic = (char*)malloc(MQTT_AT_LEN);
	memset(topic, '\0', MQTT_AT_LEN);
	//sprintf(topic, cmd, mqttConfig.username);
    sprintf(topic, cmd, "CECADED19DB9");
	//LOGD("generate topic is %s\n", topic);
	return topic;
}

/*** 订阅TOPIC ***/
// 后台命令
char *get_sub_topic_cmd() {
	return generate_topic("WAVE/DEVICE/%s/INSTRUCTION/ISSUE");
}

// 时间同步订阅
char *get_sub_topic_time_sync() {
	return generate_topic("WAVE/DEVICE/%s/TIME/RESPONSE");
}

/*** 发布TOPIC ***/
// 时间同步发布
char *get_pub_topic_time_sync() {
	return generate_topic("WAVE/DEVICE/%s/TIME/REQUEST");
}

// 图片上传
char *get_pub_topic_pic_report() {
	return generate_topic("WAVE/W7/%s/P/R"); 
}

// 图片用户
char *get_pub_topic_pic_report_u() {
	return generate_topic("WAVE/W7/%s/U/R");
}

// 心跳
char *get_pub_topic_heart_beat() {
	return generate_topic("WAVE/DEVICE/%s/HEARTBEAT");
}

// 遗愿消息
char *get_pub_topic_last_will() {
	return generate_topic("WAVE/USER/%s/WILL");
}

// 命令返回给mqtt后台
char *get_pub_topic_cmd_res() {
	return generate_topic("WAVE/DEVICE/%s/INSTRUCTION/RESPONSE");
}

// 开门记录上报
char *get_pub_topic_record_request() {
	return generate_topic("WAVE/DEVICE/%s/W7/RECORD/REQUEST");
}

#ifdef HOST5
int main() {
	char *topic = get_sub_topic_cmd();

	printf("--- msg is %s", topic); 
	if (topic != NULL) {
		free(topic);
		topic = NULL;
	}

}
#endif
