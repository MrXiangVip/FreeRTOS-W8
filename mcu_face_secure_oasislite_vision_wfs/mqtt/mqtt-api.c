#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "mqtt-api.h"
#include <unistd.h>
//#include "log.h"
#include "fsl_log.h"
#include "WIFI_UART8.h"

#define CMD_EXTRA_LEN 30
//#define MQTT_MAX_LEN 256
#define MQTT_MAX_LEN 128
#define MQTT_MAX_LONG_LEN 256

//extern int runATCmd(const char *at_cmd, int retry_times, int cmd_timeout_usec);
//int run_at_cmd(char const *cmd, int retry_times, int cmd_timeout_usec);

static int sendATCmd(const char *cmd) {
	LOGD("--- send AT CMD %s\n", cmd);
	int res = run_at_cmd(cmd, 2, 8000000);
	while (res == -5) {
		//sleep(1);
        vTaskDelay(pdMS_TO_TICKS(1000));
		res = run_at_cmd(cmd, 2, 8000000);
	}
	return res;
}

static int sendATLongCmd(const char *cmd) {
    LOGD("--- send AT Long CMD %s\n", cmd);
    int res = run_at_long_cmd(cmd, 1, 8000000);
    while (res == -5) {
        //sleep(1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        res = run_at_long_cmd(cmd, 1, 8000000);
    }
    return res;
}
// --------------------------- 主要接口 start ------------------------
// 连接WIFI
int connectWifi(const char* ssid, const char* password) {
	// int cmd_len = CMD_EXTRA_LEN + strlen(ssid) + strlen(password);

	// char *cmd = (char*)malloc(cmd_len);
	char cmd[MQTT_MAX_LEN];
	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);

	LOGD("--- send AT CMD %s\n", cmd);
	int res = run_at_cmd(cmd, 2, 10000000);
	// free(cmd);
	return res;
}

// 配置MQTT UserConfig参数
int setupMQTTUserConfig(int linkId, int scheme, const char* clientId, const char* username, const char* password, int certKeyId, int caId, const char* path) {
	// int cmd_len = CMD_EXTRA_LEN + strlen(clientId) + strlen(username) + strlen(password) + strlen(path);

	// char *cmd = (char*)malloc(cmd_len);
	char cmd[MQTT_MAX_LEN];
    memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+MQTTUSERCFG=%d,%d,\"%s\",\"%s\",\"%s\",%d,%d,\"%s\"", linkId, scheme, clientId, username, password, certKeyId, caId, path);

	int res = sendATCmd(cmd);
	// free(cmd);
	return res;
}

// 配置MQTT ConnConfig参数
int setupMQTTConnConfig(int linkId, int keepAlive, int disableCleanSession, const char* lwtTopic, const char* lwtMsg, int lwtQos, int lwtRetain) {
	// int cmd_len = CMD_EXTRA_LEN + strlen(lwtTopic) + strlen(lwtMsg);

	// char *cmd = (char*)malloc(cmd_len);
	char cmd[MQTT_MAX_LEN];
    memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+MQTTCONNCFG=%d,%d,%d,\"%s\",\"%s\",%d,%d", linkId, keepAlive, disableCleanSession, lwtTopic, lwtMsg, lwtQos, lwtRetain);

	int res = sendATCmd(cmd);
	// free(cmd);
	return res;
}

// 连接MQTT服务器
int connectMQTT(int linkId, const char* hostIp, const char* port, int reconnect) {
	// int cmd_len = CMD_EXTRA_LEN + strlen(hostIp);

	// char *cmd = (char*)malloc(cmd_len);
	char cmd[MQTT_MAX_LEN];
    memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+MQTTCONN=%d,\"%s\",%s,%d", linkId, hostIp, port, reconnect);

	int res = sendATCmd(cmd);
	// free(cmd);
	return res;
}

// 订阅MQTT主题
int subscribeMQTT(int linkId, const char* topic, int qos) {
	// int cmd_len = CMD_EXTRA_LEN + strlen(topic);

	// char *cmd = (char*)malloc(cmd_len);
	char cmd[MQTT_MAX_LEN];
    memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+MQTTSUB=%d,\"%s\",%d", linkId, topic, qos);

	int res = sendATCmd(cmd);
	// free(cmd);
	return res;
}

// 取消订阅MQTT主题
int unsubscribeMQTT(int linkId, const char* topic) {
	// int cmd_len = CMD_EXTRA_LEN + strlen(topic);

	// char *cmd = (char*)malloc(cmd_len);
	char cmd[MQTT_MAX_LEN];
    memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+MQTTUNSUB=%d,\"%s\"", linkId, topic);

	int res = sendATCmd(cmd);
	// free(cmd);
	return res;
}

// 发布MQTT主题
int publishMQTT(int linkId, const char* topic, const char* data, int qos, int retain) {
	// int cmd_len = CMD_EXTRA_LEN + strlen(topic) + strlen(data);

	// printf("publishMQTT 1 %d\n", cmd_len);
	// char *cmd = (char*)malloc(cmd_len);
	char cmd[MQTT_MAX_LONG_LEN];
    memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+MQTTPUB=%d,\"%s\",\"%s\",%d,%d", linkId, topic, data, qos, retain);

	//LOGD("%s cmd is %s\n", __FUNCTION__, cmd);
	//int res = sendATLongCmd(cmd);
    int res = sendATCmd(cmd);
	// free(cmd);
	return res;
}

int publishMQTT2(int linkId, const char* topic, const char* data, int qos, int retain) {
    // int cmd_len = CMD_EXTRA_LEN + strlen(topic) + strlen(data);

    // printf("publishMQTT 1 %d\n", cmd_len);
    // char *cmd = (char*)malloc(cmd_len);
    char cmd[MQTT_MAX_LONG_LEN];
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "AT+MQTTPUB=%d,\"%s\",\"%s\",%d,%d", linkId, topic, data, qos, retain);

    //LOGD("%s cmd is %s\n", __FUNCTION__, cmd);
    int res = 0;
    if (strlen(cmd) > 128) {
        res = sendATLongCmd(cmd);
    } else {
        res = sendATCmd(cmd);
    }
    // free(cmd);
    return res;
}
// 断开MQTT连接
int disconnectMQTT(int linkId) {
	// int cmd_len = CMD_EXTRA_LEN; 

	// char *cmd = (char*)malloc(cmd_len);
	char cmd[MQTT_MAX_LEN];
    memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+MQTTCLEAN=%d", linkId);

	int res = sendATCmd(cmd);
	// free(cmd);
	return res;
}

// --------------------------- 主要接口 end ------------------------

// --------------------------- 次要接口 start ------------------------
/*
// 配置MQTT ClientId
int setupMQTTClientId(int linkId, const char* clientId);
// 配置MQTT username 
int setupMQTTUsername(int linkId, const char* username);
// 配置MQTT password
int setupMQTTPassword(int linkId, const char* password);
// 配置MQTT Lastwill Connection
int setupMQTTConnection(int linkId, int keepAlive, int disableCleanSession, const char* lwtTopic, const char* lwtMsg, int lwtQos, int lwtRetain);
 */
// --------------------------- 次要接口 end ------------------------

// --------------------------- 辅助接口 start ------------------------
int quickConnectMQTT(const char* clientId, const char* username, const char* password, const char* hostIp, const char* port, const char* lwtTopic, const char* lwtMsg) {
	int resSetup = setupMQTTUserConfig(MQTT_LINK_ID_DEFAULT, MQTT_SCHEME_TCP, clientId, username, password, MQTT_CERT_KEY_ID_DEFAULT, MQTT_CA_ID_DEFAULT, "");
	if (resSetup) {
		return resSetup;
	}
	// 作用：服务器必须在客户端断开之后继续存储/保持客户端的订阅状态，包括以下状态
	// 存储订阅的消息Qos1和Qos2消息，当客户端重新订阅时发送
	// 服务端正在发送消息给客户端期间连接丢失导致发送失败的消息
	// disable_clean_session 为0代表会session丢失
	// 对于门锁来说是合理的，否则万一保留了一个开锁的命令，结果下次连接就直接开锁了。这不合理
	resSetup = setupMQTTConnConfig(MQTT_LINK_ID_DEFAULT, 180, 0, lwtTopic, lwtMsg, MQTT_QOS_AT_LEAST_ONCE, MQTT_RETAIN_ON);
	if (resSetup) {
		return resSetup;
	}

	int resConn = connectMQTT(MQTT_LINK_ID_DEFAULT, hostIp, port, 0);
	return resConn;
}

int quickSubscribeMQTT(const char* topic) {
	return subscribeMQTT(MQTT_LINK_ID_DEFAULT, topic, MQTT_QOS_AT_MOST_ONCE);
}

int quickUnsubscribeMQTT(const char* topic) {
	return unsubscribeMQTT(MQTT_LINK_ID_DEFAULT, topic);
}

int quickPublishMQTT(const char* topic, const char* data) {
	return publishMQTT(MQTT_LINK_ID_DEFAULT, topic, data, MQTT_QOS_AT_LEAST_ONCE, MQTT_RETAIN_OFF);
}

extern int g_priority;
int quickPublishMQTTWithPriority(const char* topic, const char* data, int priority) {
	g_priority = priority;
	int ret = publishMQTT2(MQTT_LINK_ID_DEFAULT, topic, data, MQTT_QOS_AT_LEAST_ONCE, MQTT_RETAIN_OFF);
	g_priority = 0;
	return ret;
}

int quickDisconnectMQTT() {
	return disconnectMQTT(MQTT_LINK_ID_DEFAULT);
}
// --------------------------- 辅助接口 end ------------------------
