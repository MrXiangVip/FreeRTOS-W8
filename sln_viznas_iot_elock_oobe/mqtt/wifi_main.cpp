#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <string.h>

#include "fsl_log.h"
#include "wifi_main.h"
//#include "mqtt_config.h"
#include "mqtt-common.h"
#include "config.h"
#include "mqtt_util.h"
#include "mqtt-api.h"
#include "mqtt-topic.h"

#include "commondef.h"
#include <ctype.h>
#include <stdio.h>
#include "MCU_UART5.h"
#include "MCU_UART5_Layer.h"
#include "cJSON.h"
#include "util.h"
#include "util_crc16.h"
#include "mqtt-instruction-pool.h"
#include "mqtt-mcu.h"
#include "DBManager.h"

#include "UART_FAKER.h"
#include "md5.h"
#include "base64.h"
#include "database.h"
#include "oasis.h"
#include "mqtt_dev_esp32.h"
#include "mqtt_conn_mgr.h"
#include "mqtt_cmd_mgr.h"
#include "mqtt_topic_mgr.h"
#include "mqtt_test_mgr.h"
#include "mqtt_feature_mgr.h"
#include "sln_cli.h"

#define MQTT_AT_LEN             128

// 当前pub执行优先级，0为最低优先级，9为最高优先级
int g_priority = 0;

//bool bOasisRecordUpload = false;

int battery_level = -1;

#if (configSUPPORT_STATIC_ALLOCATION == 1)
DTC_BSS static StackType_t Uart8RecvStack[UART8RECVTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart8RecvTaskTCB;

DTC_BSS static StackType_t MqttTaskStack[MQTTTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_MqttTaskTCB;

DTC_BSS static StackType_t Uart8MsgHandleTaskStack[UART8MSGHANDLETASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart8MsgHandleTaskTCB;

DTC_BSS static StackType_t Uart8SendheartbeatTaskStack[UART8SENDHEARTBEATTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart8SendheartbeatTaskTCB;

DTC_BSS static StackType_t Uart8UploadDataTaskStack[UART8UPLOADDATATASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart8UploadDataTaskTCB;

DTC_BSS static StackType_t TestTaskStack[TESTTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_TestTaskTCB;
#endif

int run_at_cmd(char const *cmd, int retry_times, int cmd_timeout_usec)
{
    int result = MqttDevEsp32::getInstance()->sendATCmd(cmd, cmd_timeout_usec, retry_times);
    return result;
}

int run_at_raw_cmd(char const *cmd, char *data, int data_len, int retry_times, int cmd_timeout_usec)
{
    int result = MqttDevEsp32::getInstance()->sendRawATCmd(cmd, data, data_len, cmd_timeout_usec, retry_times);
    return result;
}

static void mqtt_conn_task(void *pvParameters) {
    char const *logTag = "[UART8_WIFI]:mqtt_conn_task-";
    LOGD("%s start...\r\n", logTag);
    MqttConnMgr::getInstance()->keepConnectionAlive();
    // TODO:
    // record error to flash
    // upload error to network
    // notify MCU to reset system
    vTaskDelete(NULL);
}

static void mqtt_recv_task(void *pvParameters)
{
    MqttDevEsp32::getInstance()->receiveMqtt();
    return;
}

static void mqtt_send_task(void *pvParameters)
{
    char const *logTag = "[UART8_WIFI]:mqtt_send_task-";
    LOGD("%s start...\r\n", logTag);
    MqttCmdMgr::getInstance()->loopSendMqttMsgs();
    vTaskDelete(NULL);
}

static void test_task(void *pvParameters)
{
    char const *logTag = "[UART8_WIFI]:test_task-";
    LOGD("%s start...\r\n", logTag);
    MqttTestMgr *mqttTestMgr = MqttTestMgr::getInstance();
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        MqttInstructionPool::getInstance()->tick();
        if (g_test_argc > 0) {
//            for (int i = 0; i < g_test_argc; i++) {
//                LOGD("test argv %d is %s\r\n", i, g_test_argv[i]);
//            }
            mqttTestMgr->doTest(g_test_argc, g_test_argv[1], g_test_argv[2], g_test_argv[3]);
            reset_test_args();
        }
    }
    vTaskDelete(NULL);
}

static void uploaddata_task(void *pvParameters)
{
    char const *logTag = "[UART8_WIFI]:uploaddata_task-";
    LOGD("%s start...\r\n", logTag);
    MqttCmdMgr::getInstance()->uploadRecords();
    vTaskDelete(NULL);
    LOGD("\r\n%s end...\r\n", logTag);
}

int WIFI_Start()
{
	char const *logTag = "[UART8_WIFI]:main-";
//#if !MQTT_SUPPORT
//    LOGD("%s stopped...\r\n", logTag);
//	return 0;
//#endif
    LOGD("%s starting...\r\n", logTag);

    MqttDevEsp32::getInstance()->initUart();

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(mqtt_conn_task, "mqtt_conn_task", MQTTTASK_STACKSIZE, NULL, MQTTTASK_PRIORITY,
                                        MqttTaskStack, &s_MqttTaskTCB))
#else
    if (xTaskCreate(mqtt_conn_task, "mqtt_conn_task", MQTTTASK_STACKSIZE, NULL, MQTTTASK_PRIORITY, NULL) != pdPASS)
#endif
    {
        LOGD("%s failed to create mqtt_conn_task\r\n", logTag);
        while (1);
    }
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(mqtt_recv_task, "mqtt_recv_task", UART8RECVTASK_STACKSIZE, NULL, UART8RECVTASK_PRIORITY,
                                            Uart8RecvStack, &s_Uart8RecvTaskTCB))
#else
    if (xTaskCreate(uartrecv_task, "mqtt_recv_task", UART8RECVTASK_STACKSIZE, NULL, UART8RECVTASK_PRIORITY, NULL) != pdPASS)
#endif
    {
        LOGD("%s failed to create mqtt_recv_task\r\n", logTag);
        while (1);
    }
#ifndef TIMEOUT_TEST
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(mqtt_send_task, "mqtt_send_task", UART8MSGHANDLETASK_STACKSIZE, NULL, UART8MSGHANDLETASK_PRIORITY,
                                            Uart8MsgHandleTaskStack, &s_Uart8MsgHandleTaskTCB))
#else
    if (xTaskCreate(mqtt_send_task, "mqtt_send_task", UART8RECVTASK_STACKSIZE, NULL, UART8MSGHANDLETASK_PRIORITY, NULL) != pdPASS)
#endif
    {
    	LOGD("%s failed to create mqtt_send_task\r\n", logTag);
    	while (1);
    }
#endif

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(uploaddata_task, "uploaddata_task", UART8UPLOADDATATASK_STACKSIZE, NULL, UART8UPLOADDATATASK_PRIORITY,
                                            Uart8UploadDataTaskStack, &s_Uart8UploadDataTaskTCB))
#else
    if (xTaskCreate(uploaddata_task, "uploaddata_task", UART8UPLOADDATATASK_STACKSIZE, NULL, UART8UPLOADDATATASK_PRIORITY, NULL) != pdPASS)
#endif
    {
    	LOGD("%s failed to create uploaddata_task\r\n", logTag);
    	while (1);
    }

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(test_task, "test_task", TESTTASK_STACKSIZE, NULL, TESTTASK_PRIORITY,
                                            TestTaskStack, &s_TestTaskTCB))
#else
    if (xTaskCreate(test_task, "test_task", TESTTASK_STACKSIZE, NULL, TESTTASK_PRIORITY, NULL) != pdPASS)
#endif
    {
        LOGD("%s failed to create test_task\r\n", logTag);
        while (1);
    }

    LOGD("%s started...\r\n", logTag);
    return 0;
}
