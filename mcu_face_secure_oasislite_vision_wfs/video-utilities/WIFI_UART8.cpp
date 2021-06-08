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
#include "../mqtt/mqtt-common.h"
#include "../mqtt/config.h"
#include "../mqtt/mqtt_util.h"
#include "../mqtt/mqtt-api.h"
#include "../mqtt/mqtt-topic.h"
#include "commondef.h"
#include <ctype.h>
#include <stdio.h>
#include "MCU_UART5.h"
#include "cJSON.h"

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
int mqtt_init_done = 0;
// 本次开机心跳序列
static int g_heartbeat_index = 0;
// 是否已经确认服务器在线
static int g_is_online = 0;
// 是否已经确认服务器没有更多数据需要下载，默认认为需要有更多数据需要下载
static int g_has_more_download_data = 1;
// 当前pub执行优先级，0为最低优先级，9为最高优先级
int g_priority = 0;


#define AT_CMD_RESULT_OK 		0
#define AT_CMD_RESULT_ERROR 	1
#define AT_CMD_RESULT_TIMEOUT 	2
#define AT_CMD_RESULT_BUSY		3
#define AT_CMD_RESULT_UNDEF	4

#define AT_CMD_MODE_INACTIVE	0
#define AT_CMD_MODE_ACTIVE 	1

#define MQTT_AT_LEN 128
#define MQTT_AT_LONG_LEN 256
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

int run_at_cmd(char const *cmd, int retry_times, int cmd_timeout_usec)
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
				LOGD("run command %s %s\r\n", cmd, at_cmd_result ? AT_CMD_RESULT_OK : "OK", "ERROR");
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

int run_at_long_cmd(char const *cmd, int retry_times, int cmd_timeout_usec)
{
#if 1
    char at_cmd[MQTT_AT_LONG_LEN];
    memset(at_cmd, '\0', MQTT_AT_LONG_LEN);
    sprintf(at_cmd, "%s\r\n", cmd);
#else
    char at_cmd1[MQTT_AT_LEN];
    memset(at_cmd1, '\0', MQTT_AT_LEN);
    memcpy(at_cmd1, cmd, MQTT_AT_LEN);
    char at_cmd2[MQTT_AT_LEN];
    memset(at_cmd2, '\0', MQTT_AT_LEN);
    memcpy(at_cmd2, cmd + MQTT_AT_LEN, MQTT_AT_LEN);
#endif
    LOGD("start AT long command %s\r\n", cmd);
    LOGD("start AT long strlen(at_cmd) is %d\r\n", strlen(cmd));
    for (int i = 0; i < retry_times; i++) {
#if 1
        if (kStatus_Success != LPUART_RTOS_Send(&handle8, (uint8_t *)at_cmd, strlen(at_cmd))) {
            LOGD("Failed to run long command %s\r\n", cmd);
            return -1;
        } else {
            LOGD("Succeed to run long command %s\r\n", cmd);
            at_cmd_mode = AT_CMD_MODE_ACTIVE;
        }
#else
        if (kStatus_Success != LPUART_RTOS_Send(&handle8, (uint8_t *)at_cmd1, strlen(at_cmd1))) {
            LOGD("Failed to run long command1 %s\r\n", at_cmd1);
            return -1;
        } else {
            LOGD("Succeed to run long command1 %s\r\n", at_cmd1);
            at_cmd_mode = AT_CMD_MODE_ACTIVE;
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (kStatus_Success != LPUART_RTOS_Send(&handle8, (uint8_t *)at_cmd2, strlen(at_cmd2))) {
            LOGD("Failed to run long command2 %s\r\n", at_cmd2);
            return -1;
        } else {
            LOGD("Succeed to run long command2 %s\r\n", at_cmd2);
            at_cmd_mode = AT_CMD_MODE_ACTIVE;
        }
#endif
        int timeout_usec = 0;
        int delay_usec = 1000;
        do {
            vTaskDelay(pdMS_TO_TICKS(delay_usec));
            timeout_usec += delay_usec;
            if (AT_CMD_RESULT_OK == at_cmd_result || AT_CMD_RESULT_ERROR == at_cmd_result) {
                at_cmd_mode = AT_CMD_MODE_INACTIVE;
                LOGD("run long command %s %s\r\n", cmd, at_cmd_result ? AT_CMD_RESULT_OK : "OK", "ERROR");
                return at_cmd_result;
            }
            if (timeout_usec >= cmd_timeout_usec) {
                LOGD("run long command %s timeout\r\n", cmd);
                break;
            }
        } while (0);
//		xEventGroupSetBits(handle8.rxEvent, RTOS_LPUART_RX_TIMEOUT);
    }
    LOGD("run long command %s timeout end\r\n", cmd);
    return AT_CMD_RESULT_TIMEOUT;
}

static void mqttinit_task(void *pvParameters) {
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
    result = run_at_cmd("ATE0", 1, 1200);
    //result = run_at_cmd("AT+CIPSTAMAC?", 1, 1200);
    result = run_at_cmd("AT+CWJAP=\"wireless_052E81\",\"12345678\"", 2, 5000);

    vTaskDelay(pdMS_TO_TICKS(3000));

    char lwtMsg[50];
    // sprintf(lwtMsg, "{\\\"username\\\":\\\"%s\\\"\\,\\\"state\\\":\\\"0\\\"}", btWifiConfig.wifi_mac);
    //sprintf(lwtMsg, "{\\\"username\\\":\\\"%s\\\"\\,\\\"state\\\":\\\"0\\\"}", btWifiConfig.bt_mac);
    sprintf(lwtMsg, "{\\\"username\\\":\\\"%s\\\"\\,\\\"state\\\":\\\"0\\\"}", "7CDFA102AB68");

    char *pub_topic_last_will = get_pub_topic_last_will();
    // 连接MQTT
    //result = quickConnectMQTT(mqttConfig.client_id, mqttConfig.username, mqttConfig.password, mqttConfig.server_ip,
    //                       mqttConfig.server_port, pub_topic_last_will, lwtMsg);
    result = quickConnectMQTT("CECADED19DB9", "7CDFA102AB68", "0000000000000000", "10.0.14.61",
                              "9883", pub_topic_last_will, lwtMsg);
    //result = quickConnectMQTT("f3eb0aee1eb7", "7CDFA102AB68", "0000000000000000", "47.107.126.154",
    //                          "1883", pub_topic_last_will, lwtMsg);
    //notifyMqttConnected(result);
    if (result < 0) {
        LOGD("--------- Failed to connect to MQTT\n");
        return;
    }
    LOGD("--------- connect to mqtt done\n");

    vTaskDelay(pdMS_TO_TICKS(1000));

    result = quickPublishMQTT(pub_topic_last_will, "{}");

    freePointer(&pub_topic_last_will);


    vTaskDelay(pdMS_TO_TICKS(1000));
    char *sub_topic_cmd = get_sub_topic_cmd();
    result = quickSubscribeMQTT(sub_topic_cmd);
    if (result < 0) {
        //notifyHeartBeat(CODE_FAILURE);
        freePointer(&sub_topic_cmd);
        LOGD("--------- Failed to subscribe topic\n");
        //return;
    }else {
        freePointer(&sub_topic_cmd);
        LOGD("--------- subscribe topic done\n");
    }
#endif

    vTaskDelay(pdMS_TO_TICKS(1000));
    mqtt_init_done = 1;
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
	char const *logTag = "[UART8_WIFI]:uartrecv_timeout_task-";
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
        /*PRINTF("\r\n--- receive bytes ---\r\n");
        for (int i = 0; i < sizeof(recv_buffer8); i ++) {
        	PRINTF("0x%02x ", recv_buffer8[i]);
        }
        PRINTF("\r\n--- receive end ---\r\n");*/

        if (error == kStatus_LPUART_RxHardwareOverrun)
        {
            /* Notify about hardware buffer overrun */
        	LOGD("%s RX timeout hardware overrun\r\n", logTag);
        }
        if (error == kStatus_LPUART_RxRingBufferOverrun)
        {
            /* Notify about ring buffer overrun */
        	LOGD("%s RX timeout ring buffer overrun\r\n", logTag);
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
        	}else {
                //LOGD("%s receive %d bytes and message is %s\r\n",logTag, n, recv_buffer8);
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

int handleJsonMsg(char *jsonMsg) {
	cJSON *mqtt = NULL;
	cJSON *msg_id = NULL;
	cJSON *data = NULL;
	cJSON *timestamp = NULL;
	char *msg_idStr = NULL;
	char *dataStr = NULL;
	char *tsStr = NULL;

	mqtt = cJSON_Parse(jsonMsg);
	msg_id = cJSON_GetObjectItem(mqtt, "msgId");
	msg_idStr = cJSON_GetStringValue(msg_id);

	int result = 0;

	data = cJSON_GetObjectItem(mqtt, "data");
	if (data != NULL) { 
		dataStr = cJSON_GetStringValue(data);
		LOGD("---JSON data is %s\n", dataStr);
		if (dataStr != NULL && strlen(dataStr)) {
			// 来源于服务器
			if (strncmp(dataStr, "sf:", 3) == 0) {
				if (strncmp(dataStr, "sf:nodata", 9) == 0) {
					g_has_more_download_data = 0;
					LOGD("no more data to download from server by server");
				} else {
					char pub_msg[80];
					memset(pub_msg, '\0', 80);
					sprintf(pub_msg, "%s{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":4}", DEFAULT_HEADER, msg_idStr, btWifiConfig.bt_mac);
					// NOTE: 此处必须异步操作
					//MessageSend(1883, pub_msg, strlen(pub_msg));
				}
			} else {
				result = handlePayload(dataStr, msg_idStr);
			}
		}
	}

	timestamp = cJSON_GetObjectItem(mqtt, "time");
	if (timestamp != NULL) {
		tsStr = cJSON_GetStringValue(timestamp);
		LOGD("--- timestamp is %s len is %d\n", tsStr, strlen(tsStr));
		if (tsStr != NULL && strlen(tsStr) > 0) {

			int currentSec = atoi(tsStr);
			if (currentSec > 1618965299) {
				LOGD("__network time sync networkTime is %d can setTimestamp\n", currentSec);
				//setTimestamp(currentSec);
				//mqtt_log_init();
			} else {
				LOGD("__network time sync networkTime is %d don't setTimestamp\n", currentSec);
			}

			char payload_bin[MQTT_AT_LEN];
			memset(payload_bin, '\0', MQTT_AT_LEN);
			payload_bin[0] = 0x23;
			payload_bin[1] = 0x8a;
			payload_bin[2] = 0x0a;
			int len = strlen(tsStr);
			strncpy(&payload_bin[3], tsStr, len);
			//unsigned short cal_crc16 = CRC16_X25(reinterpret_cast<unsigned char*>(payload_bin), 3 + len);
			//payload_bin[len + 3] = (char)cal_crc16;
			//payload_bin[len + 4] = (char)(cal_crc16 >> 8);
			//result = MessageSend(1234, payload_bin, 5 + len);
		}
	}
	if (mqtt != NULL) {
		cJSON_Delete(mqtt);
		mqtt = NULL;
	}
	return result;
}

#define MQTT_LINKID_LEN     16
#define MQTT_TOPIC_LEN      64
#define MQTT_DATALEN_LEN    16
#define MQTT_DATA_LEN       128
#define MQTT_LASTTOP_LEN    64
#define MQTT_LASTDATA_LEN   16
int analyzeMQTTMsgInternal(char *msg) {
	static int ana_timeout_count = 0;
	int result = -1;
	char *pNext;

	//+MQTTSUBRECV:0,"WAVE/DEVICE/CECADED19DB9/INSTRUCTION/ISSUE",84,{"data":"sf:nodata","mac":"CECADED19DB9","msgId":"19625d782a174188965b1c4129accc60"}
	pNext = (char *)strstr(msg,"+MQTTSUBRECV:"); //必须使用(char *)进行强制类型转换(虽然不写有的编译器中不会出现指针错误)
	if (pNext != NULL) {

		// TODO: 只要服务器下发指令下来，我们就认为服务器已经确认在线
		g_is_online = 1;

		char firstWithLinkId[MQTT_LINKID_LEN];
		char topic[MQTT_TOPIC_LEN];
		char dataLen[MQTT_DATALEN_LEN];
		char data[MQTT_DATA_LEN];
		char lastWithTopic[MQTT_LASTTOP_LEN];
		char lastWithDataLen[MQTT_LASTDATA_LEN];
		mysplit(msg, firstWithLinkId, lastWithTopic, ",\"");

		mysplit(lastWithTopic, topic, lastWithDataLen, "\",");

		mysplit(lastWithDataLen, dataLen, data, ",");

		if (dataLen == NULL || data == NULL) {
			LOGD("analyzeMQTTMsgInternal dataLen is NULL or data is is NULL");
			return false;
		}
#if 1
		int data_len = atoi(dataLen);

		if (data != NULL) {
			if (strlen(data) >= data_len) {
				char payload[MQTT_AT_LEN];
				strncpy(payload, data, data_len);
				payload[data_len] = '\0';
				ana_timeout_count = 0;
				LOGD("payload is %s len is %d\n", payload, strlen(payload));
				int ret = handleJsonMsg(payload);
				if (ret < 0)  {
					result = -3; // command fail
				} else {
					result = 0;
				}
				// freePointer(&payload);
			} else {
				if (++ana_timeout_count > 10) {
					LOGD("analyze timeout\n");
					result = -2;
					ana_timeout_count = 0;
				}
			}
		}
#endif
	}
	// 下面的初始化会导致strtok 段错误
	// char *data = "+MQTTSUBRECV:0,\"testtopic\",26,{ \"msg\": \"Hello, World!\" }";

	return result;
}

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
                    char *second = (char *) strstr((const char *) recv_msg_lines[current_handle_line], MQTT_CWJAP);
                    // TODO: 确认是否有OK
                    // GET RSSI

                    rx_len = 0;
                    LOGD("----------------");
                    LOGD("uart rx get wifi RSSI:\n%s\n", second);
                    LOGD("----------------");
                    // +CWJAP:"wireless_052E81","c8:ee:a6:05:2e:81",1,-40,0,0,3,0
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
                    }
				} else if (strncmp((const char*)recv_msg_lines[current_handle_line], "+CIPSTAMAC:", 11) == 0) {
					LOGD("\r\n----------------- RUN COMMAND CIPSTAMAC %s---------- \r\n", curr_line);
					if (0/*btWifiConfig.wifi_mac == NULL || strlen(btWifiConfig.wifi_mac) == 0*/) {
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
                                update_mac(DEFAULT_CONFIG_FILE, device_mac_from_module);
                            //}
                        }
					}
					// TODO: 确认是否有OK
					// GET MAC
				}else if (strncmp((const char*)recv_msg_lines[current_handle_line], "+MQTTSUBRECV:", 13) == 0) {
					LOGD("################## receive subscribe message from mqtt server %s", (const char*)recv_msg_lines[current_handle_line]);
                    int ret = analyzeMQTTMsgInternal((char*)recv_msg_lines[current_handle_line]);
                }else if (strncmp((const char*)recv_msg_lines[current_handle_line], "+MQTTDISCONNECTED:0", 19) == 0 && mqtt_init_done == 1) {
                    LOGD("################## receive disconnect message from wifi_module");
                    char pub_msg[50];
                    memset(pub_msg, '\0', 50);
                    sprintf(pub_msg, "%s%s", DEFAULT_HEADER, "reconnect");
                    // NOTE: 此处必须异步操作
                    //MessageSend(1883, pub_msg, strlen(pub_msg));
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

    char msg[MQTT_AT_LEN];
    memset(msg, '\0', MQTT_AT_LEN);
    char *pub_topic = NULL;
    char pub_msg[MQTT_AT_LEN];
    memset(pub_msg, '\0', MQTT_AT_LEN);

	//mqtt_init_done = 1;
    do {
        vTaskDelay(pdMS_TO_TICKS(1000));

        if (count % 30 == 0) {
            if (mqtt_init_done == 1) {
                update_rssi();
            }
        }

        if(count % 5 == 0) {
            if (mqtt_init_done == 1) {
                // sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"btmac\\\":\\\"%s\\\"\\,\\\"wifi_rssi\\\":%s\\,\\\"battery\\\":%d\\,\\\"version\\\":\\\"%s\\\"}", msgId, btWifiConfig.wifi_mac, btWifiConfig.bt_mac, wifi_rssi, w7_battery_level, getFirmwareVersion());
                //sprintf(pub_msg,
                //        "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"wifi_rssi\\\":%s\\,\\\"battery\\\":%d\\,\\\"index\\\":%d\\,\\\"version\\\":\\\"%s\\\"}",
                //        msgId, btWifiConfig.bt_mac, wifi_rssi, w7_battery_level, g_heartbeat_index++,
                //        getFirmwareVersion());

                sprintf(pub_msg,
                        "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"wifi_rssi\\\":%s\\,\\\"battery\\\":%d\\,\\\"index\\\":%d\\,\\\"version\\\":\\\"%s\\\"}",
                        "0", "CECADED19DB9", "-55", 50, g_heartbeat_index++,
                        "fw"/*getFirmwareVersion()*/);


                pub_topic = get_pub_topic_heart_beat();
                //LOGD("%s pub_msg is %s\n", __FUNCTION__, pub_msg);
                //int ret = quickPublishMQTTWithPriority(pub_topic, pub_msg, 1);
                int ret = quickPublishMQTTWithPriority(pub_topic, "{\\\"msgId\\\":\\\"0\\\"\\,\\\"mac\\\":\\\"CECADED19DB9\\\"\\,\\\"index\\\":2\\}", 1);
            }
        }
        count++;

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



int  MqttQMsgSend(unsigned char *cmd, unsigned char msgLen)
{
	LOGD("发送消息到MQTT 的消息队列 \n");
    int ret = LPUART_RTOS_Send(&handle8, (uint8_t *)cmd, msgLen);
    if (kStatus_Success != ret)
    {
        LOGD("LPUART_RTOS_Send() error,errno<%d>!\n", ret);
        //vTaskSuspend(NULL);
    }else {
        LOGD("LPUART_RTOS_Send() succeed!\n");
    }

    return ret;
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

    if (xTaskCreate(mqttinit_task, "mqttinit_task", MQTTTASK_STACKSIZE, NULL, MQTTTASK_PRIORITY, NULL) != pdPASS)
    {
//        PRINTF("Task mqttinit_task creation failed!.\r\n");
        LOGD("%s failed to create mqttinit_task\r\n", logTag);
        while (1);
    }
#ifndef TIMEOUT_TEST
    if (xTaskCreate(uartrecv_task, "uartrecv_task", MQTTTASK_STACKSIZE, NULL, MQTTTASK_PRIORITY, NULL) != pdPASS)
#else
    if (xTaskCreate(uartrecv_timeout_task, "uartrecv_timeout_task", configMINIMAL_STACK_SIZE + 100, NULL, uart8_task_PRIORITY, NULL) != pdPASS)
#endif
    {
        LOGD("%s failed to create uartrecv_task\r\n", logTag);
        while (1);
    }
#ifndef TIMEOUT_TEST
    if (xTaskCreate(msghandle_task, "msghandle_task", MQTTTASK_STACKSIZE + 100, NULL, MQTTTASK_PRIORITY - 2, NULL) != pdPASS)
    {
    	LOGD("%s failed to create msghandle_task\r\n", logTag);
    	while (1);
    }
#endif

    LOGD("%s started...\r\n", logTag);
    return 0;
}
