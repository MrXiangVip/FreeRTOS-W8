#include <string.h>
#include <stdio.h>
#include "mqtt-common.h"
#include "fsl_log.h"
#include "config.h"
#include "mqtt_mcu_mgr.h"
#include "mqtt_topic_mgr.h"
#include "mqtt_conn_mgr.h"
#include "MCU_UART5_Layer.h"
#include "PriorityQueue.h"
#include "mqtt-mcu.h"
#include "mqtt_feature_mgr.h"
#include "util.h"
#include "util_crc16.h"

/**** MCU -> 106 ***/

/**** 106 -> MCU ***/

int MqttMcuMgr::syncTimeToMCU(char *tsStr) {
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

// 保持工作模式
int MqttMcuMgr::notifyKeepAlive() {
    LOGD("系统任务执行中，保持智能系统运作\r\n");
    return sendStatusToMCU(0x06, 0);
}

// 关闭电源
int MqttMcuMgr::notifyShutdown() {
    LOGD("系统任务已完成，关闭智能系统\r\n");
    return sendStatusToMCU(0x04, 0);
}

// 重新启动设备
int MqttMcuMgr::notifyReboot() {
    LOGD("系统遇到异常，重新启动智能系统\r\n");
    return sendStatusToMCU(0x05, 0);
}
// int sendStatusToMCU(int biz, int ret);
int MqttMcuMgr::sendStatusToMCU(int biz, int ret) {
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
//    char payload_str[15];
//    HexToStr(payload_str, reinterpret_cast<unsigned char*>(&payload_bin), 7);
    //LOGD("sendStatusToMCU %s", payload_str);
    int result = SendMsgToMCU((uint8_t *)payload_bin, 7);
    return result;
}


//// WIFI已初始化
//int MqttMcuMgr::notifyWifiInitialized(int code) {
//    LOGD("MQTT 通知MCU WIFI 初始化 %s\r\n", (code==0 ?"成功":"失败"));
//    return sendStatusToMCU(0x00, code);
//}
//
//// WIFI已连接
//int MqttMcuMgr::notifyWifiConnected(int code) {
//    LOGD("MQTT 通知MCU WIFI 连接 %s\r\n", (code==0 ?"成功":"失败"));
//    return sendStatusToMCU(0x01, code);
//}
//
//// MQTT已连接
//int MqttMcuMgr::notifyMqttConnected(int code) {
//    LOGD("MQTT 通知MCU MQTT 连接 %s\r\n", (code==0 ?"成功":"失败"));
//    return sendStatusToMCU(0x02, code);
//}
//
//// TOPIC已订阅
//// int notifyTopicSubscribed(int code) {
//// 心跳已经发送
//int MqttMcuMgr::notifyHeartBeat(int code) {
//    LOGD("MQTT 通知MCU 心跳已发送 \r\n");
//    return sendStatusToMCU(0x03, code);
//}
//
//// 命令已执行
//int MqttMcuMgr::notifyCommandExecuted(int code) {
//    LOGD("MQTT 任务完成,通知MCU下电 \r\n");
//    return sendStatusToMCU(0x04, code);
//}
