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
    LOGD("MQTT 通知MCU WIFI 初始化 %s\r\n", (code==0 ?"成功":"失败"));
	return sendStatusToMCU(0x00, code);
}

// WIFI已连接
int notifyWifiConnected(int code) {
    LOGD("MQTT 通知MCU WIFI 连接 %s\r\n", (code==0 ?"成功":"失败"));
	return sendStatusToMCU(0x01, code);
}

// MQTT已连接
int notifyMqttConnected(int code) {
    LOGD("MQTT 通知MCU MQTT 连接 %s\r\n", (code==0 ?"成功":"失败"));
    return sendStatusToMCU(0x02, code);
}

// TOPIC已订阅
// int notifyTopicSubscribed(int code) {
// 心跳已经发送
int notifyHeartBeat(int code) {
    LOGD("MQTT 通知MCU 心跳已发送 \r\n");
    return sendStatusToMCU(0x03, code);
}

// 命令已执行 
int notifyCommandExecuted(int code) {
    LOGD("MQTT 任务完成,通知MCU下电 \r\n");
	return sendStatusToMCU(0x04, code);
}

// 心跳发送保持不下电
// int notifyKeepAlive(int timeout) {
int notifyKeepAlive() {
    LOGD("MQTT 发送心跳 MCU保持不下电 \r\n");
	return sendStatusToMCU(0x06, 0);
}

// 发送下电
int notifyShutdown() {
	// TODO: 后续可以使用下电指令来代替
    LOGD("MQTT 没有任务,通知MCU下电 \r\n");
	return sendStatusToMCU(0x05, 0);
}

int syncTimeToMCU(char *tsStr) {
    char payload_bin[20];
    memset(payload_bin, '\0', sizeof(payload_bin));
    payload_bin[0] = 0x23;
    payload_bin[1] = 0x8a;
    payload_bin[2] = 0x0a;
    int len = strlen(tsStr);
    strncpy(&payload_bin[3], tsStr, len);
    unsigned short cal_crc16 = CRC16_X25(reinterpret_cast<unsigned char*>(payload_bin), 3 + len);
    payload_bin[len + 3] = (char)cal_crc16;
    payload_bin[len + 4] = (char)(cal_crc16 >> 8);
    int result = SendMsgToMCU((unsigned char *)payload_bin, 5 + len);
    return result;
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
	//LOGD("sendStatusToMCU %s", payload_str);
	//int result = MessageSend(1234, payload_bin, 7);
	int result = SendMsgToMCU((uint8_t *)payload_bin, 7);
	return result;
}

//xshx add 20221024
int sendLockModeToMCU(int lockMode ) {
    LOGD("发送门锁模式 %d 给MCU \r\n", lockMode);
    char payload_bin[16];
    memset(payload_bin, '\0', sizeof(payload_bin));
    payload_bin[0] = 0x23;
    payload_bin[1] = CMD_LOCK_MODE;
    payload_bin[2] = 0x01;
    payload_bin[3] = lockMode;
    unsigned short cal_crc16 = CRC16_X25(reinterpret_cast<unsigned char*>(payload_bin), 4);
    payload_bin[4] = (char)cal_crc16;
    payload_bin[5] = (char)(cal_crc16 >> 8);

    char payload_str[15];
    HexToStr(payload_str, reinterpret_cast<unsigned char*>(&payload_bin), 6);
    //LOGD("sendStatusToMCU %s", payload_str);
    int result = SendMsgToMCU((uint8_t *)payload_bin, strlen(payload_bin));
    return result;
}