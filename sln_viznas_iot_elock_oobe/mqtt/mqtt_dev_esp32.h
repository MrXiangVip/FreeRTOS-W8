//
// Created by wszgx on 2022/7/29.
//

#ifndef _MQTT_DEV_ESP32_H_
#define _MQTT_DEV_ESP32_H_

#include "fsl_lpuart_freertos.h"
#include "fsl_lpuart.h"
#include "board.h"

#define ESP32_LPUART          LPUART8
#define ESP32_LPUART_CLK_FREQ BOARD_DebugConsoleSrcFreq()
#define ESP32_LPUART_IRQn     LPUART8_IRQn

typedef enum {
    SEND_AT_CMD_OK      = 0x00,
    SEND_AT_CMD_FAILED  = -0x01,
    SEND_AT_CMD_NODB    = -0x02,
    SEND_AT_CMD_ENOLOCK = -0x03,
    SEND_AT_CMD_ERETRY  = -0x04,
    SEND_AT_CMD_NOSPACE = -0x05,
} SEND_AT_CMD_LOCK_STATUS;

class MqttDevEsp32 {
private:
    static MqttDevEsp32 *m_instance;
    static SemaphoreHandle_t m_send_at_cmd_lock;
    static lpuart_rtos_handle_t m_uart_handle_esp32;
    static struct _lpuart_handle m_t_handle_esp32;
    static uint8_t m_background_buffer_esp32[256];

    MqttDevEsp32();

    static int lockSendATCmd(TickType_t delayMs = portMAX_DELAY);
    static void unlockSendATCmd();

    static int initUart();
public:
    static MqttDevEsp32 *getInstance();
};


#endif //_MQTT_DEV_ESP32_H_
