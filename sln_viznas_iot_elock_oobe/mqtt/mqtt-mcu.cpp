#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "mqtt-mcu.h"
#include "base64.h"
//#include "log.h"
#include "fsl_log.h"
//#include "mqtt-mq.h"
#include "mqtt-common.h"
#include "util.h"
//#include "../SHM/SHM.h"
#include "util_crc16.h"
#include "MCU_UART5.h"

#include <ctype.h>

// WIFI已初始化
int notifyWifiInitialized(int code) {
	return sendStatusToMCU(0x00, code);
}

// WIFI已连接
int notifyWifiConnected(int code) {
	return sendStatusToMCU(0x01, code);
}

// MQTT已连接
int notifyMqttConnected(int code) {
	return sendStatusToMCU(0x02, code);
}

// TOPIC已订阅
// int notifyTopicSubscribed(int code) {
// 心跳已经发送
int notifyHeartBeat(int code) {
	return sendStatusToMCU(0x03, code);
}

// 命令已执行 
int notifyCommandExecuted(int code) {
	return sendStatusToMCU(0x04, code);
}

// 心跳发送保持不下电
// int notifyKeepAlive(int timeout) {
int notifyKeepAlive() {
	return sendStatusToMCU(0x06, 0);
}

// 发送下电
int notifyShutdown() {
	// TODO: 后续可以使用下电指令来代替
	return sendStatusToMCU(0x05, 0);
}

// int sendStatusToMCU(int biz, int ret);
int sendStatusToMCU(int biz, int ret) {
	if (ret == 0 && biz == 2) {
		//SHM::getInstance()->setMQTTStatus(biz);
	}
	char payload_bin[16];
    memset(payload_bin, '\0', sizeof(payload_bin));
	payload_bin[0] = 0x23;
	payload_bin[1] = 0x12;
	payload_bin[2] = 0x02;
	payload_bin[3] = biz;
	payload_bin[4] = (ret == 0 ? 0x00 : 0x01);
	unsigned short cal_crc16 = CRC16_X25(reinterpret_cast<unsigned char*>(payload_bin), 5);
	payload_bin[5] = (char)cal_crc16;
	payload_bin[6] = (char)(cal_crc16 >> 8);
	// for (int i = 0; i < 7; i++) {
	// 	log_debug("sendStatusToMCU %d 0x%x", i, (char)payload_bin[i]);
	// }
	char payload_str[15];
	HexToStr(payload_str, reinterpret_cast<unsigned char*>(&payload_bin), 7);
	LOGD("sendStatusToMCU %s", payload_str);
	//int result = MessageSend(1234, payload_bin, 7);
	int result = SendMsgToMCU((uint8_t *)payload_bin, 7);
	return result;
}
