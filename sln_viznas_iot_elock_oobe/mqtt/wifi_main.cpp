//
// Created by wszgx on 2022/7/29.
//

#include "commondef.h"
#include "wifi_main.h"
#include "mqtt_dev_esp32.h"

MqttDevEsp32 mqttDevEsp32 = NULL;

static void uartrecv_task(void *pvParameters)
{
    mqttDevEsp32.receiveMqtt();
    vTaskDelete(NULL);
}

int WIFI_Start()
{
    char const *logTag = "[MQTT Main]:";
//#if !MQTT_SUPPORT
//    LOGD("%s stopped...\r\n", logTag);
//    return 0;
//#endif
    LOGD("%s starting...\r\n", logTag);
    mqttDevEsp32 = MqttDevEsp32::getInstance();

//#if (configSUPPORT_STATIC_ALLOCATION == 1)
//    if (NULL == xTaskCreateStatic(mqttinit_task, "mqttinit_task", MQTTTASK_STACKSIZE, NULL, MQTTTASK_PRIORITY,
//                                        MqttTaskStack, &s_MqttTaskTCB))
//#else
//    if (xTaskCreate(mqttinit_task, "mqttinit_task", MQTTTASK_STACKSIZE, NULL, MQTTTASK_PRIORITY, NULL) != pdPASS)
//#endif
//    {
//        LOGD("%s failed to create mqttinit_task\r\n", logTag);
//        while (1);
//    }
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(uartrecv_task, "uartrecv_task", UART8RECVTASK_STACKSIZE, NULL, UART8RECVTASK_PRIORITY,
                                            Uart8RecvStack, &s_Uart8RecvTaskTCB))
#else
    if (xTaskCreate(uartrecv_task, "uartrecv_task", UART8RECVTASK_STACKSIZE, NULL, UART8RECVTASK_PRIORITY, NULL) != pdPASS)
#endif
    {
        LOGD("%s failed to create uartrecv_task\r\n", logTag);
        while (1);
    }
//#if (configSUPPORT_STATIC_ALLOCATION == 1)
//    if (NULL == xTaskCreateStatic(msghandle_task, "msghandle_task", UART8MSGHANDLETASK_STACKSIZE, NULL, UART8MSGHANDLETASK_PRIORITY,
//                                            Uart8MsgHandleTaskStack, &s_Uart8MsgHandleTaskTCB))
//#else
//    if (xTaskCreate(msghandle_task, "msghandle_task", UART8RECVTASK_STACKSIZE, NULL, UART8MSGHANDLETASK_PRIORITY, NULL) != pdPASS)
//#endif
//    {
//        LOGD("%s failed to create msghandle_task\r\n", logTag);
//        while (1);
//    }

//#if (configSUPPORT_STATIC_ALLOCATION == 1)
//    if (NULL == xTaskCreateStatic(send_heartbeat_task, "send_heartbeat_task", UART8SENDHEARTBEATTASK_STACKSIZE, NULL, UART8SENDHEARTBEATTASK_PRIORITY,
//                                            Uart8SendheartbeatTaskStack, &s_Uart8SendheartbeatTaskTCB))
//#else
//    if (xTaskCreate(send_heartbeat_task, "send_heartbeat_task", UART8SENDHEARTBEATTASK_STACKSIZE, NULL, UART8SENDHEARTBEATTASK_PRIORITY, NULL) != pdPASS)
//#endif
//    {
//        LOGD("%s failed to create send_heartbeat_task\r\n", logTag);
//        while (1);
//    }
//
//#if (configSUPPORT_STATIC_ALLOCATION == 1)
//    if (NULL == xTaskCreateStatic(uploaddata_task, "uploaddata_task", UART8UPLOADDATATASK_STACKSIZE, NULL, UART8UPLOADDATATASK_PRIORITY,
//                                            Uart8UploadDataTaskStack, &s_Uart8UploadDataTaskTCB))
//#else
//    if (xTaskCreate(uploaddata_task, "uploaddata_task", UART8UPLOADDATATASK_STACKSIZE, NULL, UART8UPLOADDATATASK_PRIORITY, NULL) != pdPASS)
//#endif
//    {
//        LOGD("%s failed to create uploaddata_task\r\n", logTag);
//        while (1);
//    }

    LOGD("%s started...\r\n", logTag);
    return 0;
}
