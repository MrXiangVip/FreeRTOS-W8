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
int WIFI_UART8_Start();

extern void mqtt_task(void *pvParameters);
extern void mqtt_init(void);

extern int run_at_cmd(char const *cmd, int retry_times, int cmd_timeout_usec);

#ifdef __cplusplus
}

#endif



#endif /* _FACEREC_MCU_UART5L_H_ */
