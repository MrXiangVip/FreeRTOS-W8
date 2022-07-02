/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.d
 *
 * Created by: NXP China Solution Team.
 */

#ifndef _WIFI_UART8_H_
#define _WIFI_UART8_H_


#ifdef __cplusplus
extern "C" {
#endif

#define	MQTT_OK	"OK"
#define	MQTT_OK_SIZE	(sizeof(MQTT_OK) - 1)

#define	MQTT_CWJAP	"+CWJAP:"
#define	MQTT_CWJAP_SIZE	(sizeof(MQTT_CWJAP) - 1)

#define	MQTT_DISCONNECT	            "+MQTTDISCONNECTED:0"
#define	MQTT_DISCONNECT_SIZE	    (sizeof(MQTT_DISCONNECT) - 1)
#define	MQTT_RAW_DISCONNECT	        ">+MQTTDISCONNECTED:0"
#define	MQTT_RAW_DISCONNECT_SIZE	(sizeof(MQTT_RAW_DISCONNECT) - 1)


#define AT_CMD_RESULT_OK 		0
#define AT_CMD_RESULT_ERROR 	1
#define AT_CMD_RESULT_TIMEOUT 	2
#define AT_CMD_RESULT_BUSY		3
#define AT_CMD_RESULT_UNDEF	    4

#define AT_CMD_MODE_INACTIVE	0
#define AT_CMD_MODE_ACTIVE 	    1


int WIFI_UART8_Start();

extern void mqtt_task(void *pvParameters);
extern void mqtt_init(void);

extern int run_at_cmd(char const *cmd, int retry_times, int cmd_timeout_usec);
extern int run_at_long_cmd(char const *cmd, int retry_times, int cmd_timeout_usec);
extern int run_at_raw_cmd(char const *cmd, char *data, int data_len, int retry_times, int cmd_timeout_usec);
extern int SendMsgToMQTT(char *mqtt_payload, int len);
extern int doSendMsgToMQTT(char *mqtt_payload, int len);

extern char *gen_msgId();
extern void freePointer(char **p);

extern int fakeWifiCmd(char *wifi_cmd);

#ifdef __cplusplus
}

#endif



#endif /* _FACEREC_MCU_UART5L_H_ */
