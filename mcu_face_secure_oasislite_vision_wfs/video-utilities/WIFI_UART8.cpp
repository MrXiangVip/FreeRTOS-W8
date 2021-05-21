#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <string.h>

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_lpuart_freertos.h"
#include "fsl_lpuart.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_log.h"
#include "WIFI_UART8.h"
//#include "mqtt_config.h"
#include "../mqtt/config.h"
#include "../mqtt/mqtt_util.h"
#include <ctype.h>
#include <stdio.h>
#include "MCU_UART5.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_LPUART          LPUART8
#define DEMO_LPUART_CLK_FREQ BOARD_DebugConsoleSrcFreq()
#define DEMO_LPUART_IRQn     LPUART8_IRQn
/* Task priorities. */
#define uart8_task_PRIORITY (configMAX_PRIORITIES - 1)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void uart_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
const char *to_send               = "AT\r\n";//"FreeRTOS LPUART driver example!\r\n";
const char *send_ring_overrun     = "\r\nAT\r\n";
const char *send_hardware_overrun = "\r\nAT\r\n";
uint8_t background_buffer8[256];

#define TIMEOUT_TEST
#undef TIMEOUT_TEST

#ifdef TIMEOUT_TEST
uint8_t recv_buffer8[8];
#else
uint8_t recv_buffer8[1];
#endif

#define MAX_MSG_LINES 20
#define MAX_MSG_LEN_OF_LINE 256
//unsigned char **recv_msg_lines;              // received messages
//unsigned char *recv_crrent_line;     // line to receive
//unsigned char *handle_current_line; // line to handle
uint8_t recv_msg_lines[MAX_MSG_LINES][MAX_MSG_LEN_OF_LINE];
static int current_recv_line = 0;
static int current_recv_line_len = 0;
static int current_handle_line = 0;


#define AT_CMD_RESULT_OK 		0
#define AT_CMD_RESULT_ERROR 	1
#define AT_CMD_RESULT_TIMEOUT 	2
#define AT_CMD_RESULT_BUSY		3
#define AT_CMD_RESULT_UNDEF	4

#define AT_CMD_MODE_INACTIVE	0
#define AT_CMD_MODE_ACTIVE 	1

#define MQTT_AT_LEN 128
#define MQTT_MAC_LEN 32
#define MQTT_RSSI_LEN 32

static int at_cmd_mode = AT_CMD_MODE_INACTIVE;
static int at_cmd_result = AT_CMD_RESULT_UNDEF;

static int wifi_ready = 0;

lpuart_rtos_handle_t handle8;
struct _lpuart_handle t_handle8;

lpuart_rtos_config_t lpuart_config8 = {
    .baudrate    = 115200,
    .parity      = kLPUART_ParityDisabled,
    .stopbits    = kLPUART_OneStopBit,
    .buffer      = background_buffer8,
    .buffer_size = sizeof(background_buffer8),
};

static int handle_line();

static int run_at_cmd(char const *cmd, int retry_times, int cmd_timeout_usec)
{
//	if (AT_CMD_MODE_ACTIVE == at_cmd_mode) {
//		return AT_CMD_RESULT_BUSY;
//	}
	char at_cmd[MQTT_AT_LEN];
	memset(at_cmd, '\0', MQTT_AT_LEN);
	sprintf(at_cmd, "%s\r\n", cmd);
	LOGD("start AT command %s\r\n", cmd);
	for (int i = 0; i < retry_times; i++) {
		if (kStatus_Success != LPUART_RTOS_Send(&handle8, (uint8_t *)at_cmd, strlen(at_cmd))) {
			LOGD("Failed to run command %s\r\n", cmd);
			return -1;
		} else {
			LOGD("Succeed to run command %s\r\n", cmd);
			at_cmd_mode = AT_CMD_MODE_ACTIVE;
		}
		int timeout_usec = 0;
		int delay_usec = 1000;
		do {
			vTaskDelay(pdMS_TO_TICKS(delay_usec));
			timeout_usec += delay_usec;
			if (AT_CMD_RESULT_OK == at_cmd_result || AT_CMD_RESULT_ERROR == at_cmd_result) {
				at_cmd_mode = AT_CMD_MODE_INACTIVE;
				LOGD("run command %s OK/ERROR\r\n", cmd);
				return at_cmd_result;
			}
			if (timeout_usec >= cmd_timeout_usec) {
				LOGD("run command %s timeout\r\n", cmd);
				break;
			}
		} while (0);
//		xEventGroupSetBits(handle8.rxEvent, RTOS_LPUART_RX_TIMEOUT);
	}
	LOGD("run command %s timeout end\r\n", cmd);
	return AT_CMD_RESULT_TIMEOUT;
}

static void mqttinit_task(void *pvParameters)
{
	char const *logTag = "[UART8_WIFI]:mqttinit_task-";
    LOGD("%s start...\r\n", logTag);
    int error;
    size_t n = 0;
#ifdef TIMEOUT_TEST
    int result = run_at_cmd("AT", 1, 3000);
    vTaskDelay(pdMS_TO_TICKS(1000));
    for (int i = 0; i < 9; i++) {
    	result = run_at_cmd("AT", 1, 3000);
    	vTaskDelay(pdMS_TO_TICKS(1000));
    }
#else
    int result = AT_CMD_RESULT_OK;
//    result = run_at_cmd("AT+GMR\r\n", 1, 1000);
//    LOGD("\r\nrun AT+GMR result %d\r\n", result);
//    if (AT_CMD_RESULT_OK != result) {
//    	LOGD("\r\n%sFailed to get WiFi module version\r\n", logTag);
//    	// TODO: try to notify MCU
//    	vTaskDelete(NULL);
//    	return;
//    }

    result = run_at_cmd("AT+RST", 2, 1200);
    LOGD("run AT+RST result %d\r\n", result);
    if (AT_CMD_RESULT_OK != result) {
    	LOGD("%sFailed to reset WiFi module\r\n", logTag);
    	// TODO: try to notify MCU
    	vTaskDelete(NULL);
    	return;
    }
	//cmdSysInitOKSyncReq(SYS_VERSION);

//    LOGD("\r\n%sSuccess to reset WiFi module\r\n", logTag);
    for (int i = 0; i < 10; i++) {
    	if (wifi_ready == 1) {
    		LOGD("%sSuccess to reset WiFi module\r\n", logTag);
    		break;
    	}
    	vTaskDelay(pdMS_TO_TICKS(100));
    }


    result = run_at_cmd("AT+SYSLOG=1", 2, 1200);
    result = run_at_cmd("AT+CWMODE=3", 1, 1200);
    result = run_at_cmd("AT+CIPSTAMAC?", 1, 1200);
    result = run_at_cmd("AT+CWJAP=\"wireless_052E81\",\"12345678\"", 2, 5000);
#endif

//    run_at_cmd("AT\r\n", 2, 3000);

//    run_at_cmd("AT+RST", 2, 3000);
//    run_at_cmd("AT\r\n", 2, 3000);

//    do {
//    	LOGD("\r\n%s run mqttinit_task\r\n", logTag);
//    	vTaskDelay(pdMS_TO_TICKS(2000));
//    } while (1);

//    vTaskSuspend(NULL);
    LOGD("\r\n%s end...\r\n", logTag);
    vTaskDelete(NULL);
}

static void uartrecv_timeout_task(void *pvParameters)
{
	char const *logTag = "[UART8_WIFI]:uartrecv_task-";
    LOGD("\r\n%s start...\r\n", logTag);
    int error;
    size_t n = 0;
    do
    {
    	memset(recv_buffer8, 0, sizeof(recv_buffer8));
//        error = LPUART_RTOS_Receive(&handle8, recv_buffer8, sizeof(recv_buffer8), &n);
        LOGD("%s ------------ before receive\r\n", logTag);
        error = LPUART_RTOS_ReceiveTimeout(&handle8, recv_buffer8, sizeof(recv_buffer8), &n, 1000);
//        error = LPUART_RTOS_ReceiveTimeout(&handle8, recv_buffer8, 8, &n, 1000);
        LOGD("%s receive %d bytes and message is %s\r\n",logTag, n, recv_buffer8);
        PRINTF("\r\n--- receive bytes ---\r\n");
        for (int i = 0; i < sizeof(recv_buffer8); i ++) {
        	PRINTF("0x%02x ", recv_buffer8[i]);
        }
        PRINTF("\r\n--- receive end ---\r\n");

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
    } while (kStatus_Success == error || kStatus_Timeout == error);
    LPUART_RTOS_Deinit(&handle8);
    vTaskSuspend(NULL);

    LOGD("\r\n%s end...\r\n", logTag);
}

static void uartrecv_task(void *pvParameters)
{
	char const *logTag = "[UART8_WIFI]:uartrecv_task-";
    LOGD("%s start...\r\n", logTag);
    int error;
    size_t n = 0;
    memset(recv_buffer8, 0, sizeof(recv_buffer8));
    do
    {
//        error = LPUART_RTOS_Receive(&handle8, recv_buffer8, sizeof(recv_buffer8), &n);
        error = LPUART_RTOS_Receive(&handle8, recv_buffer8, 1, &n);
        if (current_recv_line_len >= MAX_MSG_LEN_OF_LINE) {
        	LOGD("\r\n--- receive line length is %d greater than 255, discard---\r\n", current_recv_line_len);
        } else {
        	char lastest_char = recv_buffer8[0];
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
        			LOGD("--- recv_msg_line is %s\r\n", recv_msg_lines[current_recv_line]);
        			current_recv_line++;
        			current_recv_line_len = 0;
        			if (current_recv_line >= MAX_MSG_LINES) {
        				current_recv_line = 0;
        			}
        			handle_line();
        		} else {
        			// 当只有一个0x0a的时候，忽略此行，并且重置current_recv_len为0
        			current_recv_line_len = 0;
        			recv_msg_lines[current_recv_line][current_recv_line_len] = '\0';
        		}
        	}
        }
//        LOGD("%s receive %d bytes and message is %s\r\n",logTag, n, recv_buffer8);
//        PRINTF("\r\n--- receive bytes ---\r\n");
//        for (int i = 0; i < sizeof(recv_buffer8); i ++) {
//        	PRINTF("0x%02x ", recv_buffer8[i]);
//        }
//        PRINTF("\r\n--- receive end ---\r\n");

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
    } while (kStatus_Success == error);
    LPUART_RTOS_Deinit(&handle8);
    vTaskSuspend(NULL);

//    do {
//    	LOGD("\r\n%s run uartrecv_task\r\n", logTag);
//    } while (1);
    LOGD("\r\n%s end...\r\n", logTag);
}

// 防止重入
static int is_handling_line = 0;
static int handle_line()
{
	int rx_len = 0;

	if (is_handling_line) {
		return 0;
	}
	is_handling_line = 1;
	do {
    	if (current_handle_line == current_recv_line) {
    		break;
    	}
    	const char *curr_line = (const char*)recv_msg_lines[current_handle_line];
    	LOGD("---------------- line %d is : %s\r\n", current_handle_line, recv_msg_lines[current_handle_line]);
#if 0
		// 如果是MQTT的TOPIC RECEIVE
		if (strncmp((const char*)recv_msg_lines[current_handle_line], "+MQTTSUBRECV:", strlen("+MQTTSUBRECV:")) == 0) {
			LOGD("\r\n----------------- message from mqtt server : %s -------------- \r\n", recv_msg_lines[current_handle_line]);
			// TODO: handle message from mqtt server
		} else if (strncmp(curr_line, "ready", 5) == 0) {
			wifi_ready = 1;
		} else {
#endif
#if 0
			// 如果是处于AT指令执行状态，则需要判断AT指令的执行情况
			if (AT_CMD_MODE_ACTIVE == at_cmd_mode) {
#endif
				if (strncmp((const char*)recv_msg_lines[current_handle_line], "OK", 2) == 0) {
					LOGD("\r\n----------------- RUN COMMAND OK ---------- \r\n");
					at_cmd_result = AT_CMD_RESULT_OK;
				} else if (strncmp((const char*)recv_msg_lines[current_handle_line], "ERROR", 5) == 0) {
					LOGD("\r\n----------------- RUN COMMAND FAIL ---------- \r\n");
					at_cmd_result = AT_CMD_RESULT_ERROR;
				} else if (strncmp((const char*)recv_msg_lines[current_handle_line], MQTT_CWJAP, MQTT_CWJAP_SIZE) == 0) {
					LOGD("\r\n----------------- RUN COMMAND CWJAP ---------- \r\n");
					char *second = (char*)strstr((const char *)recv_msg_lines[current_handle_line], MQTT_CWJAP);
					// TODO: 确认是否有OK
					// GET RSSI

					rx_len = 0;
					LOGD("----------------");
					LOGD("uart rx get wifi RSSI:\n%s\n", second);
					LOGD("----------------");
					// +CWJAP:"wireless_052E81","c8:ee:a6:05:2e:81",1,-40,0,0,3,0
#if 1
					char field11[MQTT_RSSI_LEN];
					char field21[MQTT_RSSI_LEN];
					char field22[MQTT_RSSI_LEN];
					char field31[MQTT_RSSI_LEN];
					char field32[MQTT_RSSI_LEN];
					char field41[MQTT_RSSI_LEN];
					char field42[MQTT_RSSI_LEN];
					char field5[MQTT_RSSI_LEN];
					memset(field42, '\0', MQTT_RSSI_LEN);
					mysplit(second, field11, field21, ",");
					//LOGD("field21 is %s\n", field21);
					mysplit(field21, field22, field31, ",");
					//LOGD("field31 is %s\n", field31);
					mysplit(field31, field32, field41, ",");
					//LOGD("field41 is %s\n", field41);
					mysplit(field41, field42, field5, ",");
					//LOGD("field42 is %s\n", field42);
					if (field42 != NULL && strlen(field42) > 0) {
						LOGD("RSSI is %s\n", field42);
						//strcpy(wifi_rssi, field42);
						//at_state = AT_CMD_OK;
						rx_len = 0;
					}
#endif
				} else if (strncmp((const char*)recv_msg_lines[current_handle_line], "+CIPSTAMAC:", 11) == 0) {
					LOGD("\r\n----------------- RUN COMMAND CIPSTAMAC %s---------- \r\n", curr_line);
					if (1/*btWifiConfig.wifi_mac == NULL || strlen(btWifiConfig.wifi_mac) == 0*/) {
                        char device_mac_from_module[MQTT_MAC_LEN];
                        char cmd_atmac[MQTT_MAC_LEN];
                        char mac_atmacstr[MQTT_MAC_LEN];
                        char mac_atmac[MQTT_MAC_LEN];
                        //char dumpstr[MQTT_MAC_LEN];
						//memset(device_mac_from_module, '\0', MQTT_MAC_LEN);
						//memset(cmd_atmac, '\0', MQTT_MAC_LEN);
						//memset(mac_atmacstr, '\0', MQTT_MAC_LEN);
						//memset(mac_atmac, '\0', MQTT_MAC_LEN);
                        mysplit((char*)curr_line, cmd_atmac, mac_atmacstr, ":\"");
                        LOGD("\r\n***** %s, %s\r\n", cmd_atmac, mac_atmacstr);
						//mysplit(mac_atmacstr, mac_atmac, dumpstr, "\"");
                        //LOGD("\r\n***** %s, %s\r\n", mac_atmac, dumpstr);
						mysplit2(mac_atmacstr, mac_atmac, "\"");
                        LOGD("\r\n***** %s\r\n", mac_atmac);
#if	1						

						for (char* ptr = mac_atmac; *ptr; ptr++) {
                            // *ptr = tolower(*ptr);  //转小写
                            *ptr = toupper(*ptr);  //转大写
                        }

                        LOGD("\r\n***** %s\r\n", mac_atmac);

                        if (mac_atmac != NULL && strlen(mac_atmac) > 0) {
                            del_char(mac_atmac, ':');
                            strcpy(device_mac_from_module, mac_atmac);
							LOGD("\r\n***** device_mac_from_module is %s\r\n", device_mac_from_module);
                            //if (strncmp(device_mac_from_module, btWifiConfig.wifi_mac, 12) != 0) {
                                //update_mac(DEFAULT_CONFIG_FILE, device_mac_from_module);
                            //}
                        }
#endif
					}
					// TODO: 确认是否有OK
					// GET MAC
				}
#if 0
			}
#endif
#if 0
		}
#endif
		memset(recv_msg_lines[current_handle_line], '\0', MAX_MSG_LEN_OF_LINE);
		current_handle_line++;
		if (current_handle_line >= MAX_MSG_LINES) {
			current_handle_line = 0;
		}
	} while (1);
	LOGD("=============== handle_line finished =================\r\n");
	is_handling_line = 0;
	return 0;
}

void update_rssi() {
	run_at_cmd("AT+CWJAP?", 1, 1000000);
}

static void msghandle_task(void *pvParameters)
{
    char const *logTag = "[UART8_WIFI]:msghandle_task-";
    LOGD("%s start...\r\n", logTag);
	int count = 0;
    do {
    	vTaskDelay(pdMS_TO_TICKS(1000));

		if(count % 30 == 0) {
			update_rssi();
		}

		count ++;

//    	LOGD("\r\n%s current_handle_line is %d current_recv_line is %d...\r\n", logTag, current_handle_line, current_recv_line);

//    	handle_line();

//    	if (current_handle_line != current_recv_line) {
//    		PRINTF("line %d is : %s\r\n", current_handle_line, recv_msg_lines[current_handle_line]);
//    		// TODO:handle line
////    		memset(recv_msg_lines[current_handle_line], '\0', MAX_MSG_LEN_OF_LINE);
//    		// TODO:end
//    	}
    } while (1);
    vTaskDelete(NULL);
    LOGD("\r\n%s end...\r\n", logTag);
}


int WIFI_UART8_Start()
{
	char const *logTag = "[UART8_WIFI]:main-";
    LOGD("%s starting...\r\n", logTag);
    NVIC_SetPriority(LPUART8_IRQn, 4);
//    if (xTaskCreate(uart8_task, "uart8_task", configMINIMAL_STACK_SIZE + 100, NULL, uart8_task_PRIORITY, NULL) != pdPASS)
//    {
//        PRINTF("Task creation failed!.\r\n");
//        while (1);
//    }

    lpuart_config8.srcclk = DEMO_LPUART_CLK_FREQ;
    lpuart_config8.base   = DEMO_LPUART;
    if (kStatus_Success != LPUART_RTOS_Init(&handle8, &t_handle8, &lpuart_config8))
    {
    	LOGD("%s failed to initialize uart8\r\n", logTag);
        vTaskSuspend(NULL);
    }
    LOGD("%s succeed to initialize uart8\r\n", logTag);

    for (int i = 0; i < MAX_MSG_LINES; i++) {
    	memset(recv_msg_lines[i], '\0', MAX_MSG_LEN_OF_LINE);
    }

    if (xTaskCreate(mqttinit_task, "mqttinit_task", configMINIMAL_STACK_SIZE + 100, NULL, uart8_task_PRIORITY, NULL) != pdPASS)
    {
//        PRINTF("Task mqttinit_task creation failed!.\r\n");
        LOGD("%s failed to create mqttinit_task\r\n", logTag);
        while (1);
    }
#ifndef TIMEOUT_TEST
    if (xTaskCreate(uartrecv_task, "uartrecv_task", configMINIMAL_STACK_SIZE + 100, NULL, uart8_task_PRIORITY, NULL) != pdPASS)
#else
    if (xTaskCreate(uartrecv_timeout_task, "uartrecv_timeout_task", configMINIMAL_STACK_SIZE + 100, NULL, uart8_task_PRIORITY, NULL) != pdPASS)
#endif
    {
        LOGD("%s failed to create uartrecv_task\r\n", logTag);
        while (1);
    }
#ifndef TIMEOUT_TEST
    if (xTaskCreate(msghandle_task, "msghandle_task", configMINIMAL_STACK_SIZE + 100, NULL, uart8_task_PRIORITY, NULL) != pdPASS)
    {
    	LOGD("%s failed to create msghandle_task\r\n", logTag);
    	while (1);
    }
#endif

    LOGD("%s started...\r\n", logTag);
    return 0;
}
