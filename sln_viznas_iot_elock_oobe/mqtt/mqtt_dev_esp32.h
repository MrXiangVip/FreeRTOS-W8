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

#define MAX_MSG_LINES           20
#if	FFD_SUPPORT != 0 || REMOTE_FEATURE != 0
#define MAX_MSG_LEN_OF_LINE     1536
#else
#define MAX_MSG_LEN_OF_LINE     256
#endif

#define MQTT_AT_LEN 256

typedef enum {
    SEND_AT_CMD_OK      = 0x00,
    SEND_AT_CMD_FAILED  = -0x01,
    SEND_AT_CMD_NODB    = -0x02,
    SEND_AT_CMD_ENOLOCK = -0x03,
    SEND_AT_CMD_ERETRY  = -0x04,
    SEND_AT_CMD_NOSPACE = -0x05,
} SEND_AT_CMD_LOCK_STATUS;

//typedef enum {
//    AT_CMD_RESULT_OK 	    =	0x00,
//    AT_CMD_RESULT_ERROR     =	-0x01,
//    AT_CMD_RESULT_TIMEOUT 	=	-0x02,
//    AT_CMD_RESULT_BUSY 	    =	-0x03,
//    AT_CMD_RESULT_UNDEF 	=	-0x04,
//} AT_CMD_RESULT;

class MqttDevEsp32 {
private:
    SemaphoreHandle_t m_send_at_cmd_lock;

//    lpuart_rtos_handle_t *m_uart_handle_esp32;
//    struct _lpuart_handle *m_t_handle_esp32;
    uint8_t m_background_buffer_esp32[256];

    uint8_t recv_msg_lines[MAX_MSG_LINES][MAX_MSG_LEN_OF_LINE];
    int current_recv_line = 0;
    int current_recv_line_len = 0;
    int current_handle_line = 0;

    MqttDevEsp32();

    int lockSendATCmd(TickType_t delayMs = portMAX_DELAY);
    void unlockSendATCmd();

public:
    static MqttDevEsp32 *getInstance() {
        static MqttDevEsp32 m_instance;
        return &m_instance;
    }

    void receiveMqtt();
    int sendATCmd(char const *cmd, int retry_times, int cmd_timeout_usec, int &m_at_cmd_result);
    int initUart();
//    int initUart(lpuart_rtos_handle_t *handle, lpuart_handle_t *t_handle, const lpuart_rtos_config_t *cfg);
};

#endif //_MQTT_DEV_ESP32_H_
