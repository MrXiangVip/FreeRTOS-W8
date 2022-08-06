#ifndef _MQTT_MCU_MGR_H_
#define _MQTT_MCU_MGR_H_

//#ifdef __cplusplus
//extern "C" {
//#endif

#include "mqtt_cmd.h"

#include "PriorityQueue.h"
#include "DBManager.h"
#include <vector>

class MqttMcuMgr {
private:
    MqttMcuMgr() {};

    // 更新106状态，或者说通知MCU控制106，比如不下电，下电，重启
    int sendStatusToMCU(int biz, int ret);
    void sendMcuRspToMqtt(char res, char *msgId);
public:
    static MqttMcuMgr *getInstance() {
        static MqttMcuMgr m_instance;
        return &m_instance;
    };

    int mcuToMqtt(char *mcuData, int len);
    void reportRealTimeRecord(int id);

    int syncTimeToMCU(char *tsStr);

// 保持不下电
    int notifyKeepAlive();
// 发送下电
    int notifyShutdown();
// 发送重启106的指令，当系统检测到异常的时候，尽量上报到后台告警，保存异常到系统存储之后，发起重新启动的指令
    int notifyReboot();

#if 0
//// WIFI已初始化 0x00
//    int notifyWifiInitialized(int code);
//// WIFI已连接 0x01
//    int notifyWifiConnected(int code);
//// MQTT已连接 0x02
//    int notifyMqttConnected(int code);
// TOPIC已订阅 0x03
// int notifyTopicSubscribed(int code);
//// 改为心跳已发送 0x03
//    int notifyHeartBeat(int code);
//// 命令已执行 0x04
//    int notifyCommandExecuted(int code);
#endif
};

//#ifdef __cplusplus
//}
//#endif

#endif
