//
// Created by wszgx on 2022/7/29.
//

#include "stdio.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "fsl_log.h"

#include "mqtt_dev_esp32.h"
#include "wifi_main.h"
#include "mqtt_util.h"
#include "mqtt_manager.h"
#include "mqtt_conn_mgr.h"

lpuart_rtos_handle_t m_uart_handle_esp32;
struct _lpuart_handle m_t_handle_esp32;

MqttDevEsp32::MqttDevEsp32() {
    if (NULL == m_send_at_cmd_lock) {
        m_send_at_cmd_lock = xSemaphoreCreateMutex();
        if (NULL == m_send_at_cmd_lock) {
            LOGE("[ERROR]:Fail to create MqttDevEsp32 mutex\r\n");
        }
        LOGD("[OK]:Succeed to create MqttDevEsp32 mutex\r\n");
    }

    for (int i = 0; i < MAX_MSG_LINES; i++) {
        memset(recv_msg_lines[i], '\0', MAX_MSG_LEN_OF_LINE);
    }
    // initUart();
}

/*
MqttDevEsp32* MqttDevEsp32::getInstance() {
    if (NULL == m_instance) {
        m_instance = new MqttDevEsp32();
    }
    return m_instance;
}
*/

int MqttDevEsp32::lockSendATCmd(TickType_t delayMs) {
    if (NULL == m_send_at_cmd_lock) {
        return SEND_AT_CMD_ENOLOCK;
    }

//    if (pdTRUE != xSemaphoreTake(m_send_at_cmd_lock, portMAX_DELAY)) {
    if (pdTRUE != xSemaphoreTake(m_send_at_cmd_lock, delayMs)) {
        return SEND_AT_CMD_ERETRY;
    }

    LOGD("Try lock MqttDevEsp32 successfully\r\n");
    return SEND_AT_CMD_OK;
}

void MqttDevEsp32::unlockSendATCmd() {
    xSemaphoreGive(m_send_at_cmd_lock);
    LOGD("Try unlock MqttDevEsp32 successfully\r\n");
}

int MqttDevEsp32::initUart() {
    NVIC_SetPriority(ESP32_LPUART_IRQn, 4);

    lpuart_rtos_config_t lpuart_config_esp32 = {
            .baudrate    = 115200,
            .parity      = kLPUART_ParityDisabled,
            .stopbits    = kLPUART_OneStopBit,
            .buffer      = m_background_buffer_esp32,
            .buffer_size = sizeof(m_background_buffer_esp32),
    };
    lpuart_config_esp32.srcclk = ESP32_LPUART_CLK_FREQ;
    lpuart_config_esp32.base   = ESP32_LPUART;
    if (kStatus_Success != LPUART_RTOS_Init(&m_uart_handle_esp32, &m_t_handle_esp32, &lpuart_config_esp32))
    {
        LOGD("[ERROR]:fail to initialize uart ESP32\r\n");
        return -1;
    }
    LOGD("[OK]:succeed to initialize uart ESP32\r\n");

    return 0;
}

void MqttDevEsp32::receiveMqtt() {
    char const *logTag = "[MQTT Receive]:";
    LOGD("%s start...\r\n", logTag);
    int error;
    size_t n = 0;
    uint8_t esp32RecvBuf[1] = {0};
    do
    {
        error = LPUART_RTOS_Receive(&m_uart_handle_esp32, esp32RecvBuf, 1, &n);
        if ( error == kStatus_Success)
        {
            if (current_recv_line_len >= MAX_MSG_LEN_OF_LINE) {
                LOGD("\r\n--- receive line length is %d greater than %d, discard---\r\n", current_recv_line_len, MAX_MSG_LEN_OF_LINE);
            } else {
                char lastest_char = esp32RecvBuf[0];
                recv_msg_lines[current_recv_line][current_recv_line_len++] = lastest_char;
                if (lastest_char == 0x0a) {
                    if (current_recv_line_len > 1) {
                        char lastest_char2 = recv_msg_lines[current_recv_line][current_recv_line_len-2];
                        if (lastest_char2 == 0x0d) {
                            recv_msg_lines[current_recv_line][current_recv_line_len - 2] = '\0';
                            if (current_recv_line_len == 2) {
                                // 当只有0x0d0x0a的时候，忽略此行，并且重置current_recv_len为0
                                current_recv_line_len = 0;
                                recv_msg_lines[current_recv_line][1] = '\0';
                                continue;
                            }
                        }
                        recv_msg_lines[current_recv_line][current_recv_line_len - 1] = '\0';
                        LOGD("%s recv_msg_line is %d %d %s\r\n", logTag, current_recv_line, current_handle_line, ((const char *)recv_msg_lines[current_recv_line]));
                        handleLine((const char *)recv_msg_lines[current_handle_line]);
                        current_recv_line++;
                        current_recv_line_len = 0;
                        if (current_recv_line >= MAX_MSG_LINES) {
                            current_recv_line = 0;
                        }
                        memset(recv_msg_lines[current_handle_line], '\0', MAX_MSG_LEN_OF_LINE);
                        current_handle_line++;
                        if (current_handle_line >= MAX_MSG_LINES) {
                            current_handle_line = 0;
                        }
                    } else {
                        // 当只有一个0x0a的时候，忽略此行，并且重置current_recv_len为0
                        current_recv_line_len = 0;
                        recv_msg_lines[current_recv_line][current_recv_line_len] = '\0';
                    }
                }else {
                    //LOGD("%s receive %d bytes and message is %s\r\n",logTag, n, esp32RecvBuf);
                }
            }
        }

        if (error == kStatus_LPUART_RxHardwareOverrun)
        {
            /* Notify about hardware buffer overrun */
            LOGD("%s RX hardware overrun\r\n", logTag);
        }
        if (error == kStatus_LPUART_RxRingBufferOverrun)
        {
            /* Notify about ring buffer overrun */
            LOGD("%s RX ring buffer overrun\r\n", logTag);
        }
    } while (1); //(kStatus_Success == error);
    // TODO: LPUART_RTOS_Deinit(&m_uart_handle_esp32);
}

int MqttDevEsp32::sendATCmd(char const *cmd, int retry_times, int cmd_timeout_usec) {
    lockSendATCmd();

    char at_cmd[MQTT_AT_LEN];
    memset(at_cmd, '\0', MQTT_AT_LEN);
    sprintf(at_cmd, "%s\r\n", cmd);
    LOGD("\r\n\r\n");
    LOGD(">>>>>> start AT command %s\r\n", cmd);
    m_at_cmd_result = AT_CMD_RESULT_UNDEF;
    for (int i = 0; i < retry_times; i++) {
        if (kStatus_Success != LPUART_RTOS_Send(&m_uart_handle_esp32, (uint8_t *)at_cmd, strlen(at_cmd))) {
            LOGD("Failed to run command %s\r\n", cmd);
            m_at_cmd_result = AT_CMD_RESULT_ERROR;
            break;
        }
        int timeout_usec = 0;
        int delay_usec = 10;
        do {
            vTaskDelay(pdMS_TO_TICKS(delay_usec));
            timeout_usec += delay_usec;
            if (AT_CMD_RESULT_OK == m_at_cmd_result || AT_CMD_RESULT_ERROR == m_at_cmd_result) {
                // at_cmd_mode = AT_CMD_MODE_INACTIVE;
                LOGD("<<<<<< run command %s %s\r\n\r\n", cmd, (m_at_cmd_result == AT_CMD_RESULT_OK) ? "OK": "ERROR");
                unlockSendATCmd();
                return m_at_cmd_result;
//            } else {
//                LOGD("run command %s %d\r\n", cmd, m_at_cmd_result);
            }
            if (timeout_usec >= cmd_timeout_usec) {
                LOGD("run command %s timeout index %d\r\n", cmd, i);
                break;
            }
        } while (1);
    }

    LOGD("<<<<<< run command %s timeout end\r\n\r\n", cmd);
    m_at_cmd_result = AT_CMD_RESULT_TIMEOUT;
    unlockSendATCmd();
    return m_at_cmd_result;
}

int MqttDevEsp32::handleLine(const char *curr_line) {
    LOGD("handleLine %d is : %d %s\r\n", current_handle_line, strlen(curr_line), curr_line);
    if (strncmp(curr_line, "OK", 2) == 0) {
        LOGD("AT CMD OK\r\n");
        m_at_cmd_result = AT_CMD_RESULT_OK;
    } else if (strncmp(curr_line, "ERROR", 5) == 0) {
        LOGE("AT CMD ERROR\r\n");
        m_at_cmd_result = AT_CMD_RESULT_ERROR;
    } else if (strncmp(curr_line, ">+MQTTPUB:OK", 12) == 0 || strncmp((const char*)recv_msg_lines[current_handle_line], "+MQTTPUB:OK", 11) == 0) {
        m_at_cmd_result = AT_CMD_RESULT_OK;
        LOGD("RAW AT CMD OK\r\n");
    } else if (strncmp(curr_line, ">+MQTTPUB:ERROR", 15) == 0 || strncmp((const char*)recv_msg_lines[current_handle_line], "+MQTTPUB:ERROR", 14) == 0) {
        m_at_cmd_result = AT_CMD_RESULT_ERROR;
        LOGE("RAW AT CMD ERROR\r\n");
    }else if (strncmp(curr_line, MQTT_CWJAP, MQTT_CWJAP_SIZE) == 0) {
        LOGD("CWJAP OK\r\n");
        char *second = (char *) strstr((const char *) recv_msg_lines[current_handle_line], MQTT_CWJAP);

        LOGD("----------------");
        LOGD("uart rx get wifi RSSI:%s\r\n", second);
        LOGD("----------------");
        if (second != NULL) {
            char field11[MQTT_RSSI_LEN];
            char field21[MQTT_RSSI_LEN];
            char field22[MQTT_RSSI_LEN];
            char field31[MQTT_RSSI_LEN];
            char field32[MQTT_RSSI_LEN];
            char field41[MQTT_RSSI_LEN];
            char field42[MQTT_RSSI_LEN];
            char field5[MQTT_RSSI_LEN];
            memset(field42, '\0', MQTT_RSSI_LEN);
            mysplit(second, field11, field21, (char *)",");
            //LOGD("field21 is %s\n", field21);
            mysplit(field21, field22, field31, (char *)",");
            //LOGD("field31 is %s\n", field31);
            mysplit(field31, field32, field41, (char *)",");
            //LOGD("field41 is %s\n", field41);
            mysplit(field41, field42, field5, (char *)",");
            //LOGD("field42 is %s\n", field42);
            if (field42 != NULL && strlen(field42) > 0) {
                LOGD("RSSI is %s\r\n", field42);
                m_wifi_rssi = atoi(field42);
                //at_state = AT_CMD_OK;
                m_at_cmd_result = AT_CMD_RESULT_OK;
                MqttConnMgr::getInstance()->setMqttConnState(WIFI_CONNECTED);
//                g_is_wifi_connected = 1;
            }
        }
    } else if (strncmp(curr_line, "+CIPSTAMAC:", 11) == 0) {
        LOGD("\r\n----------------- RUN COMMAND CIPSTAMAC %s---------- \r\n", curr_line);
    }else if (strncmp(curr_line, "+MQTTSUBRECV:", 13) == 0) {
        LOGD("####receive subscribe message from mqtt server %s \r\n", curr_line);
        int ret = MqttManager::getInstance()->analyzeMqttMsg((char*)curr_line);
//    }else if (((strncmp(curr_line, MQTT_DISCONNECT, MQTT_DISCONNECT_SIZE) == 0) ||
//               (strncmp(curr_line, MQTT_RAW_DISCONNECT, MQTT_RAW_DISCONNECT_SIZE) == 0)) && (mqtt_init_done == 1)) {
    }else if (strncmp(curr_line, MQTT_DISCONNECT, MQTT_DISCONNECT_SIZE) == 0 ||
               strncmp(curr_line, MQTT_RAW_DISCONNECT, MQTT_RAW_DISCONNECT_SIZE) == 0) {
        LOGD("###receive mqtt disconnect message from wifi_module \r\n");
    }
    LOGD("=============== handle_line finished =================\r\n");
    return 0;
}

int MqttDevEsp32::sendRawATCmd(char const *cmd, char *data, int data_len, int retry_times, int cmd_timeout_usec) {
    int ret = 0;
    LOGD("start AT raw command %s data_len is %d\r\n", cmd, data_len);

    ret = sendATCmd(cmd, retry_times, cmd_timeout_usec);

    lockSendATCmd();

    vTaskDelay(pdMS_TO_TICKS(2));

    if(ret == AT_CMD_RESULT_OK) {
        if (kStatus_Success != LPUART_RTOS_Send(&m_uart_handle_esp32, (uint8_t *) data, data_len)) {
            LOGD("Failed to run raw command\r\n");
            m_at_cmd_result = AT_CMD_RESULT_ERROR;
            unlockSendATCmd();
            return m_at_cmd_result;
        } else {
            LOGD("Succeed to run raw command\r\n");
            m_at_cmd_result = AT_CMD_RESULT_UNDEF;
            int timeout_usec = 0;
            int delay_usec = 10;
            do {
                vTaskDelay(pdMS_TO_TICKS(delay_usec));
                timeout_usec += delay_usec;
                if (AT_CMD_RESULT_OK == m_at_cmd_result || AT_CMD_RESULT_ERROR == m_at_cmd_result) {
                    LOGD("run raw %s\r\n", m_at_cmd_result == AT_CMD_RESULT_OK ? "OK" : "ERROR");
                    unlockSendATCmd();
                    return m_at_cmd_result;
                }
                if (timeout_usec >= cmd_timeout_usec) {
                    LOGD("run raw timeout\r\n");
                    break;
                }
            } while (1);
            LOGD("run raw at cmd error\r\n");
            m_at_cmd_result = AT_CMD_RESULT_TIMEOUT;
            unlockSendATCmd();

            return m_at_cmd_result;
        }
    }
    LOGD("run raw at cmd error\r\n");
    m_at_cmd_result = ret;
    unlockSendATCmd();

    return m_at_cmd_result;
}
