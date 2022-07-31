#ifndef _MQTT_MCU_H_
#define _MQTT_MCU_H_

//#define CODE_SUCCESS 0
//#define CODE_FAILURE 1

#ifdef __cplusplus
extern "C" {
#endif
int syncTimeToMCU(char *tsStr);
int sendStatusToMCU(int biz, int ret);
// WIFI已初始化 0x00
int notifyWifiInitialized(int code);
// WIFI已连接 0x01
int notifyWifiConnected(int code);
// MQTT已连接 0x02
int notifyMqttConnected(int code);
// TOPIC已订阅 0x03
// int notifyTopicSubscribed(int code);
// 改为心跳已发送 0x03
int notifyHeartBeat(int code);
// 命令已执行 0x04
int notifyCommandExecuted(int code);
// 保持不下电
// int notifyKeepAlive(int code);
int notifyKeepAlive();
// 发送下电
int notifyShutdown();
#ifdef __cplusplus
}
#endif

#endif
