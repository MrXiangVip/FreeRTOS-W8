#ifndef _MQTT_COMMON_H_
#define _MQTT_COMMON_H_

#define CODE_SUCCESS 0
#define CODE_FAILURE 1

#define DEFAULT_HEADER "IN:"
#define DEFAULT_HEADER_LEN (strlen(DEFAULT_HEADER))
#define DEFAULT_SEPARATOR ":"
#define TIME_OUT_RES 2

#define CMD_FEATURE_UP "fu"

#define MQTT_MSG_ID_LEN 50
#define MQTT_AT_LEN 256
#define MQTT_AT_CMD_LEN 256
#define MQTT_AT_TOPIC_LEN 64
#define AT_SEND_BUF_LEN 256
#define UART_RX_BUF_LEN 1536 //2048//8292
#define MQTT_AT_CMD_DEFAULT_RETRY_TIME 2
#define MQTT_AT_CMD_DEFAULT_TIMEOUT 8000

#define SLEEP_INTERVER_USEC 10000
#define DEFAULT_CMD_TIMEOUT_USEC 1000000


#include "board_rt106f_elock.h"


#ifdef __cplusplus
extern "C" {
inline char *getFirmwareVersion() {
	return  FIRMWARE_VERSION;
}
#endif
#ifdef __cplusplus
}
#endif

#endif
