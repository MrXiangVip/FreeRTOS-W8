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
#include "mqtt_util.h"
#include "mqtt_cmd_mgr.h"
#include "DBManager.h"

void MqttMcuMgr::sendMcuRspToMqtt(char res, char *msgId) {
    int result = AT_RSP_SUCCESS;
    if ((int)res != 0x00) {
        result = AT_RSP_ERROR;
    }
    MqttCmdMgr::getInstance()->atCmdResponse(result, msgId);
}

void MqttMcuMgr::reportRealTimeRecord(int id) {
    LOGD("请求MQTT 上传记录 start id %d \r\n", id);
    Record record;
    int ret = DBManager::getInstance()->getRecordByID(id, &record);
    if (ret == 0) {
        LOGD("register/unlcok record is not NULL start upload record/image \r\n");
        if( record.upload == BOTH_UNUPLOAD ) {//有可能被当做历史记录先上传,先检测防止重复上传
            ret = MqttCmdMgr::getInstance()->pushRecord(&record, CMD_TYPE_RECORD_TEXT, PRIORITY_HIGH, 1);
            ret = MqttCmdMgr::getInstance()->pushRecord(&record, CMD_TYPE_RECORD_IMAGE, PRIORITY_HIGH, 1);
        } else if (record.upload == RECORD_UPLOAD) {
            ret = MqttCmdMgr::getInstance()->pushRecord(&record, CMD_TYPE_RECORD_IMAGE, PRIORITY_HIGH, 1);
        }else{
            LOGD("实时记录已经被上传  \r\n");
        }
    } else {
        LOGD("开门记录未存在");
        // TODO: 可以上报给后台
    }
}

/**** MCU -> 106 ***/
int MqttMcuMgr::mcuToMqtt(char *mcuData, int len) {
    char msgId[MSG_ID_LEN];
    memset(msgId, '\0', sizeof(msgId));
    strcpy(msgId, MqttInstructionPool::getInstance()->getMsgId((char) (mcuData[0]), (char) (mcuData[1])));
    LOGD("out msgId is %s", msgId);
    if ((int) (char) (mcuData[0]) == 0x24) {
        int cmd_index = MqttInstruction::getCmdIndex((char) (mcuData[0]), (char) (mcuData[1]));
        MqttInstructionPool::getInstance()->removeMqttInstruction(cmd_index);
        sendMcuRspToMqtt(mcuData[3], msgId);
    } else if ((int) (char) (mcuData[0]) == 0x23) {
        int cmd_index = MqttInstruction::getCmdIndex((char) (mcuData[0]), (char) (mcuData[1]));
        MqttInstructionPool::getInstance()->removeMqttInstruction(cmd_index);
        if ((int)(char)(mcuData[1]) == 0x83 && (int)(char)(mcuData[2]) == 0x01) {
            // 远程开锁指令反馈
            // 远程开锁成功
            sendMcuRspToMqtt(mcuData[3], msgId);
        } else if ((int) (char) (mcuData[1]) == 0x15 && (int) (char) (mcuData[2]) == 0x01) {
            // 远程设置合同时间指令反馈
            // 远程设置合同时间成功
            sendMcuRspToMqtt(mcuData[3], msgId);
        } else if ((int) (char) (mcuData[1]) == CMD_MQTT_UPLOAD) {
            // 注册/开门记录
            int id = (int) (mcuData[3]);
            reportRealTimeRecord(id);
        } else if ((int) (char) (mcuData[1]) == CMD_OPEN_LOCK_REC) {
            // 开门记录上报指令
            int id = (int) (mcuData[3]);
            reportRealTimeRecord(id);
        } else if ((int) (char) (mcuData[1]) == CMD_MECHANICAL_LOCK) {
            // 机械开锁记录
            int id = (int) (mcuData[3]);
            reportRealTimeRecord(id);
        } else {
        }
    }
    return 0;
}

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
