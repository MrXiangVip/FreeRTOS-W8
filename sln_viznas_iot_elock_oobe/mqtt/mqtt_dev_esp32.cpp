//
// Created by wszgx on 2022/7/29.
//

#include "stdio.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "fsl_log.h"

#include "mqtt_dev_esp32.h"

MqttDevEsp32::MqttDevEsp32() {
    if (NULL == m_send_at_cmd_lock) {
        m_send_at_cmd_lock = xSemaphoreCreateMutex();
        if (NULL == m_send_at_cmd_lock) {
            LOGE("[ERROR]:Fail to create MqttDevEsp32 mutex\r\n");
        }
        LOGD("[OK]:Succeed to create MqttDevEsp32 mutex\r\n");
    }
}

MqttDevEsp32* MqttDevEsp32::getInstance() {
    if (NULL == m_instance) {
        m_instance = new MqttDevEsp32();
    }
    return m_instance;
}

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
