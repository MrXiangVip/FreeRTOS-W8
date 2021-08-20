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
#include "../mqtt/mqtt-ota.h"
#include "commondef.h"
#include <ctype.h>
#include <stdio.h>
#include "MCU_UART5.h"
#include "cJSON.h"
#include "util.h"
#include "util_crc16.h"
#include "../mqtt/base64.h"
#include "../mqtt/mqtt-instruction-pool.h"
#include "../mqtt/mqtt-mcu.h"
#include "../db/DBManager.h"


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

#define MAX_MSG_LINES           20
#define MAX_MSG_LEN_OF_LINE     256

#define MQTT_AT_LEN             128
#define MQTT_AT_LONG_LEN        256
#define MQTT_MAC_LEN            32
#define MQTT_RSSI_LEN           32

//unsigned char **recv_msg_lines;              // received messages
//unsigned char *recv_crrent_line;     // line to receive
//unsigned char *handle_current_line; // line to handle
uint8_t recv_msg_lines[MAX_MSG_LINES][MAX_MSG_LEN_OF_LINE];
static int current_recv_line = 0;
static int current_recv_line_len = 0;
static int current_handle_line = 0;
int mqtt_init_done = 0;
static int at_is_running = 0;
// 本次开机心跳序列
static int g_heartbeat_index = 0;
// 是否已经确认服务器在线
static int g_is_online = 0;
// 是否已经确认服务器没有更多数据需要下载，默认认为需要有更多数据需要下载
static int g_has_more_download_data = 1;
// 是否已经确认服务器没有更多数据需要上传，默认认为需要更多数据需要上传
static int g_has_more_upload_data = 1;
// 是否正在上传数据
static int g_is_uploading_data = 0;
// 命令是否已经执行完成
static int g_command_executed = 0;
// 关机通知是否已经执行完成
static int g_shutdown_notified = 0;
// 当前pub执行优先级，0为最低优先级，9为最高优先级
int g_priority = 0;
// 是否是自动上传过程中	: 0 未上传/上传完成 1 上传中 2 上传中断
static int g_is_auto_uploading = 0;
// wifi是否已自动连接
static int g_is_wifi_connected = 0;
// 心跳是否立即执行
static int g_heart_beat_executed = 0;

int g_is_shutdown = 0;



MqttInstructionPool mqtt_instruction_pool;
int battery_level = -1;

char wifi_rssi[MQTT_RSSI_LEN];


#if (configSUPPORT_STATIC_ALLOCATION == 1)
DTC_BSS static StackType_t Uart8RecvStack[UART8RECVTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart8RecvTaskTCB;

DTC_BSS static StackType_t MqttTaskStack[MQTTTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_MqttTaskTCB;

DTC_BSS static StackType_t Uart8MsgHandleTaskStack[UART8MSGHANDLETASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart8MsgHandleTaskTCB;

DTC_BSS static StackType_t Uart8SendheartbeatTaskStack[UART8SENDHEARTBEATTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart8SendheartbeatTaskTCB;

DTC_BSS static StackType_t Uart8UploadDataTaskStack[UART8UPLOADDATATASK_STACKSIZE];
DTC_BSS static StaticTask_t s_Uart8UploadDataTaskTCB;
#endif

static int at_cmd_mode = AT_CMD_MODE_INACTIVE;
static int at_cmd_result = AT_CMD_RESULT_UNDEF;

static int wifi_ready = 0;

int uploadRecordImage(Record *record, bool online);

int g_uploading_id = -1;

extern bool bPubOasisImage;
extern int boot_mode;
bool bOasisRecordUpload = false;
lpuart_rtos_handle_t handle8;
struct _lpuart_handle t_handle8;

lpuart_rtos_config_t lpuart_config8 = {
    .baudrate    = 921600,
    .parity      = kLPUART_ParityDisabled,
    .stopbits    = kLPUART_OneStopBit,
    .buffer      = background_buffer8,
    .buffer_size = sizeof(background_buffer8),
};

lpuart_rtos_config_t lpuart_config8_wifi = {
        .baudrate    = 921600,
        .parity      = kLPUART_ParityDisabled,
        .stopbits    = kLPUART_OneStopBit,
        .buffer      = background_buffer8,
        .buffer_size = sizeof(background_buffer8),
};

static int handle_line();
static int random_gen = 1;

void freePointer(char **p) {
    if (*p != NULL) {
        vPortFree(*p);
        *p = NULL;
    }
}

char *gen_msgId() {
    char *msgId = (char *) pvPortMalloc(64);
    memset(msgId, '\0', 64);
    // mac
    struct timeval tv;
    tv.tv_sec = ws_systime;
    tv.tv_usec = 0;
    //gettimeofday(&tv, NULL);
    // 可能秒就够了
    // long id = tv.tv_sec*1000000 + tv.tv_usec;
    // sprintf(msgId, "%s%d%06d%03d", btWifiConfig.wifi_mac, tv.tv_sec, tv.tv_usec, random_gen);
    // sprintf(msgId, "%s%d%d", btWifiConfig.wifi_mac, tv.tv_sec, random_gen);
    sprintf(msgId, "%d%06d%03d", tv.tv_sec, tv.tv_usec, random_gen);
    //LOGD("generate msgId is %s\n", msgId);
    // 3位random
    // if (++random_gen >= 1000) {
    // 1位random
    if (++random_gen >= 1000) {
        random_gen = 1;
    }
    return msgId;
}

void remove_mqtt_instruction_from_pool(char instruction_dest, char cmd_code) {
    int cmd_index = MqttInstruction::getCmdIndex(instruction_dest, cmd_code);
    mqtt_instruction_pool.removeMqttInstruction(cmd_index);
}

int run_at_cmd(char const *cmd, int retry_times, int cmd_timeout_usec)
{
//	if (AT_CMD_MODE_ACTIVE == at_cmd_mode) {
//		return AT_CMD_RESULT_BUSY;
//	}
	if(g_is_shutdown) {
        at_cmd_mode = AT_CMD_MODE_INACTIVE;
        return -1;
    }
    if (at_is_running >= 1) {
        LOGD("AT cmd is running %d so can't run %s\r\n", at_is_running, cmd);
        at_is_running++;
        if (at_is_running > 10) {
            // 防止死锁，10s无论如何解锁
            at_is_running = 0;
        }
        return AT_CMD_RESULT_BUSY;
    }
    at_is_running = 1;

	char at_cmd[MQTT_AT_LEN];
	memset(at_cmd, '\0', MQTT_AT_LEN);
	sprintf(at_cmd, "%s\r\n", cmd);
	LOGD("start AT command %s\r\n", cmd);
	for (int i = 0; i < retry_times; i++) {
		if (kStatus_Success != LPUART_RTOS_Send(&handle8, (uint8_t *)at_cmd, strlen(at_cmd))) {
			LOGD("Failed to run command %s\r\n", cmd);
            at_is_running = 0;
			return -1;
		} else {
			LOGD("Succeed to run command %s\r\n", cmd);
			at_cmd_mode = AT_CMD_MODE_ACTIVE;
			at_cmd_result = AT_CMD_RESULT_UNDEF;
		}
		int timeout_usec = 0;
		int delay_usec = 10;
		do {
			vTaskDelay(pdMS_TO_TICKS(delay_usec));
			timeout_usec += delay_usec;
			if (AT_CMD_RESULT_OK == at_cmd_result || AT_CMD_RESULT_ERROR == at_cmd_result) {
				at_cmd_mode = AT_CMD_MODE_INACTIVE;
				LOGD("run command %s %s\r\n", cmd, at_cmd_result ? AT_CMD_RESULT_OK : "OK", "ERROR");
                at_is_running = 0;
				return at_cmd_result;
			}
			if (timeout_usec >= cmd_timeout_usec) {
				LOGD("run command %s timeout\r\n", cmd);
				break;
			}
		} while (1);
//		xEventGroupSetBits(handle8.rxEvent, RTOS_LPUART_RX_TIMEOUT);
	}
	LOGD("run command %s timeout end\r\n", cmd);
    at_is_running = 0;
	return AT_CMD_RESULT_TIMEOUT;
}

char at_long_cmd[MQTT_AT_LONG_LEN];
int run_at_long_cmd(char const *cmd, int retry_times, int cmd_timeout_usec)
{
    if(g_is_shutdown) {
        at_cmd_mode = AT_CMD_MODE_INACTIVE;
        return -1;
    }
    if (at_is_running >= 1) {
        LOGD("AT long cmd is running %d so can't run %s\r\n", at_is_running, cmd);
        at_is_running++;
        if (at_is_running > 10) {
            // 防止死锁，10s无论如何解锁
            at_is_running = 0;
        }
        return AT_CMD_RESULT_BUSY;
    }
    at_is_running = 1;

    //char at_long_cmd[MQTT_AT_LONG_LEN];
    memset(at_long_cmd, '\0', MQTT_AT_LONG_LEN);
    sprintf(at_long_cmd, "%s\r\n", cmd);
    LOGD("start AT long command %s\r\n", cmd);
    //LOGD("start AT long strlen(at_cmd) is %d\r\n", strlen(cmd));
    for (int i = 0; i < retry_times; i++) {
        if (kStatus_Success != LPUART_RTOS_Send(&handle8, (uint8_t *)at_long_cmd, strlen(at_long_cmd))) {
            //LOGD("Failed to run long command %s\r\n", cmd);
            LOGD("Failed to run long command\r\n");
            at_is_running = 0;
            return -1;
        } else {
            //LOGD("Succeed to run long command %s\r\n", cmd);
            LOGD("Succeed to run long command\r\n");
            at_cmd_mode = AT_CMD_MODE_ACTIVE;
            at_cmd_result = AT_CMD_RESULT_UNDEF;
        }
        int timeout_usec = 0;
        int delay_usec = 10;
        do {
            vTaskDelay(pdMS_TO_TICKS(delay_usec));
            timeout_usec += delay_usec;
            if (AT_CMD_RESULT_OK == at_cmd_result || AT_CMD_RESULT_ERROR == at_cmd_result) {
                at_cmd_mode = AT_CMD_MODE_INACTIVE;
                //LOGD("run long command %s %s\r\n", cmd, at_cmd_result ? AT_CMD_RESULT_OK : "OK", "ERROR");
                LOGD("run long command %s\r\n", at_cmd_result ? AT_CMD_RESULT_OK : "OK", "ERROR");
                at_is_running = 0;
                return at_cmd_result;
            }
            if (timeout_usec >= cmd_timeout_usec) {
                //LOGD("run long command %s timeout\r\n", cmd);
                LOGD("run long command timeout\r\n");
                break;
            }
        } while (1);
//		xEventGroupSetBits(handle8.rxEvent, RTOS_LPUART_RX_TIMEOUT);
    }
    //LOGD("run long command %s timeout end\r\n", cmd);
    LOGD("run long command timeout end\r\n");
    at_is_running = 0;
    return AT_CMD_RESULT_TIMEOUT;
}

int run_at_raw_cmd(char const *cmd, char *data, int data_len, int retry_times, int cmd_timeout_usec)
{
    if(g_is_shutdown) {
        at_cmd_mode = AT_CMD_MODE_INACTIVE;
        return -1;
    }

    int ret = 0;
    //LOGD("start AT raw command %s\r\n", cmd);
    //LOGD("start AT raw data_len is %d\r\n", data_len);
    LOGD("start AT raw command %s data_len is %d\r\n", cmd, data_len);
    //LOGD("start AT raw data %s\r\n", data);

    ret = run_at_cmd(cmd, retry_times, cmd_timeout_usec);

    if (at_is_running >= 1) {
        LOGD("AT raw cmd is running %d so can't run %s\r\n", at_is_running, cmd);
        at_is_running++;
        if (at_is_running > 10) {
            // 防止死锁，10s无论如何解锁
            at_is_running = 0;
        }
        return AT_CMD_RESULT_BUSY;
    }
    at_is_running = 1;

    vTaskDelay(pdMS_TO_TICKS(2));

    if(ret == 0) {
        for (int i = 0; i < 1; i++) {
            if (kStatus_Success != LPUART_RTOS_Send(&handle8, (uint8_t *) data, data_len)) {
                //LOGD("Failed to run long command %s\r\n", cmd);
                LOGD("Failed to run raw command\r\n");
                at_is_running = 0;
                return -1;
            } else {
                //LOGD("Succeed to run long command %s\r\n", cmd);
                LOGD("Succeed to run raw command\r\n");
                at_cmd_mode = AT_CMD_MODE_ACTIVE;
                at_cmd_result = AT_CMD_RESULT_UNDEF;
            }
            int timeout_usec = 0;
            int delay_usec = 10;
            do {
                vTaskDelay(pdMS_TO_TICKS(delay_usec));
                timeout_usec += delay_usec;
                if (AT_CMD_RESULT_OK == at_cmd_result || AT_CMD_RESULT_ERROR == at_cmd_result) {
                    at_cmd_mode = AT_CMD_MODE_INACTIVE;
                    //LOGD("run long command %s %s\r\n", cmd, at_cmd_result ? AT_CMD_RESULT_OK : "OK", "ERROR");
                    LOGD("run long raw %s\r\n", at_cmd_result ? AT_CMD_RESULT_OK : "OK", "ERROR");
                    at_is_running = 0;
                    return at_cmd_result;
                }
                if (timeout_usec >= cmd_timeout_usec) {
                    //LOGD("run long command %s timeout\r\n", cmd);
                    LOGD("run long raw timeout\r\n");
                    break;
                }
            } while (1);
        }
    }
    //LOGD("run long command %s timeout end\r\n", cmd);
    LOGD("run long raw timeout end\r\n");
    at_is_running = 0;

    return AT_CMD_RESULT_TIMEOUT;
}

void update_rssi() {
    run_at_cmd("AT+CWJAP?", 1, 1000);
}

static void mqttinit_task(void *pvParameters) {
    char const *logTag = "[UART8_WIFI]:mqttinit_task-";
    LOGD("%s start...\r\n", logTag);

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
    if (strncmp("true", btWifiConfig.need_reset, 4) == 0 ) {
        result = run_at_cmd("AT+RST", 2, 1200);
        LOGD("run AT+RST result %d\r\n", result);
        notifyWifiInitialized(result);
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

        int result1 = run_at_cmd("AT+SYSLOG=1", 2, 1200);
        int result2 = run_at_cmd("AT+CWMODE=1", 1, 1200);
        int result3 = run_at_cmd("ATE0", 1, 1200);

        if (result == 0 && result1 == 0 && result2 == 0 && result3 == 0) {
            update_need_reset("false");
        }
    }else {
        int result3 = run_at_cmd("ATE0", 2, 1200);
    }

#if	0
    vTaskDelay(pdMS_TO_TICKS(300));
    //result = run_at_cmd("AT+UART_CUR=921600,8,1,0,1", 1, 1000);
    //result = run_at_cmd("AT+UART_DEF=2000000,8,1,0,0", 2, 5000);
    //result = run_at_cmd("AT+UART_DEF=115200,8,1,0,0", 2, 5000);
    result = run_at_cmd("AT+UART_DEF=921600,8,1,0,0", 2, 5000);
    if (result == 0) {
        // 关闭WIFI串口
        LOGD("%s 1\r\n", __FUNCTION__);
    	//vTaskDelete(NULL);

        //result = LPUART_RTOS_Deinit(&handle8);
        // 打开WIFI串口
        result = LPUART_RTOS_Init(&handle8, &t_handle8, &lpuart_config8_wifi);
        if (result < 0) {
            LOGD("open serial port %s failed!\n", "UART8");
        	vTaskDelete(NULL);
            return;
        }
        LOGD("open serial port %s with B921600\n", "UART8");
    	vTaskDelete(NULL);

    	vTaskDelay(pdMS_TO_TICKS(1000));

        return;
    }
    LOGD("--------- connect to URART8\n");
#endif

    int wifi_count = 0;
    // 尽量3s以内判断wifi是否已自动连接，若已经自动连接，则无需处理下面的操作
    while (g_is_wifi_connected == 0 && wifi_count < 10) {
        run_at_cmd("AT+CWJAP?", 1, 1000);
        // 睡眠300ms
        //usleep(300000);
        vTaskDelay(pdMS_TO_TICKS(300));
        wifi_count++;
    }

    if (g_is_wifi_connected == 0 || strncmp(btWifiConfig.need_reconnect, "true", 4) == 0) {
        // 连接WIFI
        result = connectWifi(btWifiConfig.ssid, btWifiConfig.password);
        // sendStatusToMCU(0x01, ret);
        notifyWifiConnected(result);
        update_rssi();
        if (result < 0) {
            LOGD("--------- Failed to connect to WIFI\r\n");
            vTaskDelete(NULL);
            return;
        } else {
            update_need_reconnect("false");
        }
        LOGD("--------- connect to WIFI done\r\n");
    } else {
        LOGD("--------- auto connect to WIFI done\r\n");
    }
    vTaskDelay(pdMS_TO_TICKS(300));

    char lwtMsg[50];
    // sprintf(lwtMsg, "{\\\"username\\\":\\\"%s\\\"\\,\\\"state\\\":\\\"0\\\"}", btWifiConfig.wifi_mac);
    //sprintf(lwtMsg, "{\\\"username\\\":\\\"%s\\\"\\,\\\"state\\\":\\\"0\\\"}", btWifiConfig.bt_mac);
    sprintf(lwtMsg, "{\\\"username\\\":\\\"%s\\\"\\,\\\"state\\\":\\\"0\\\"}", "7CDFA102AB68");

    char *pub_topic_last_will = get_pub_topic_last_will();
    // 连接MQTT
    result = quickConnectMQTT(mqttConfig.client_id, mqttConfig.username, mqttConfig.password, mqttConfig.server_ip,
                           mqttConfig.server_port, pub_topic_last_will, lwtMsg);
    //result = quickConnectMQTT(mqttConfig.client_id, "7CDFA102AB68", "0000000000000000", "10.0.14.61",
    //                          "9883", pub_topic_last_will, lwtMsg);


    //result = quickConnectMQTT(mqttConfig.client_id, "7CDFA102AB68", "0000000000000000", mqttConfig.server_ip,
    //                          mqttConfig.server_port, pub_topic_last_will, lwtMsg);
    notifyMqttConnected(result);
    if (result != 0) {
        LOGD("--------- Failed to connect to MQTT\r\n");
        vTaskDelete(NULL);
        return;
    }
    LOGD("--------- connect to mqtt done\r\n");

    vTaskDelay(pdMS_TO_TICKS(200));

    result = quickPublishMQTT(pub_topic_last_will, "{}");

    //freePointer(&pub_topic_last_will);


    vTaskDelay(pdMS_TO_TICKS(200));
    char *sub_topic_cmd = get_sub_topic_cmd();
    result = quickSubscribeMQTT(sub_topic_cmd);
    if (result != 0) {
        notifyHeartBeat(CODE_FAILURE);
        //freePointer(&sub_topic_cmd);
        LOGD("--------- Failed to subscribe topic\r\n");
        //return;
    }else {
        //freePointer(&sub_topic_cmd);
        LOGD("--------- subscribe topic done\r\n");
    }
#endif

    vTaskDelay(pdMS_TO_TICKS(200));
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

#ifdef TIMEOUT_TEST
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
#endif

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
        if ( error == kStatus_Success)
        {
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
    } while (1); //(kStatus_Success == error);
    LPUART_RTOS_Deinit(&handle8);
    vTaskSuspend(NULL);

//    do {
//    	LOGD("\r\n%s run uartrecv_task\r\n", logTag);
//    } while (1);
    LOGD("\r\n%s end...\r\n", logTag);
}

int handlePayload(char *payload, char *msg_idStr) {
    if (payload != NULL) {
        int payload_len = strlen(payload);
        int payload_bin_len = (payload_len / 4 - 1) * 3 + 1;
        char payload_bin[MQTT_AT_LEN];
        int ret1 = base64_decode(payload, payload_bin);
        //LOGD("decode payload_bin_len is %d ret1 is %d", payload_bin_len, ret1);

        char payload_str[MQTT_AT_LEN];
        HexToStr(payload_str, reinterpret_cast<unsigned char*>(&payload_bin), payload_bin_len);
        LOGD("\ndecode payload_bin<len:%d %s> ret1 is %d", payload_bin_len, payload_str, ret1);

        // TODO:
        unsigned char x7_cmd = payload_bin[0];
        unsigned char x7_cmd_code = payload_bin[1];
        int timeout = 5;
        if (x7_cmd == 0x23 && x7_cmd_code == 0x83) {
            // 远程开门
            timeout = 25;
        } else if (x7_cmd == 0x23 && x7_cmd_code == 0x8a) {
            int err = 0;
            int len = (int) (char) payload_bin[2];
            if (len != 10) {
                err = 1;
            }
            char tsStr[11];
            memset(tsStr, '\0', 11);
            strncpy(tsStr, &payload_bin[3], 10);
            // 时间同步
            LOGD("--- timestamp is %s len is %d\n", tsStr, strlen(tsStr));
            if (0 == err && tsStr != NULL && strlen(tsStr) > 0) {
                int currentSec = atoi(tsStr);
                if (currentSec > 1618965299) {
                	LOGD("__network time sync networkTime is %d can setTimestamp\n", currentSec);
                    setTimestamp(currentSec);

                    char pub_msg[80];
                    memset(pub_msg, '\0', 80);
                    sprintf(pub_msg, "%s{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":0}",
                            DEFAULT_HEADER, msg_idStr, btWifiConfig.bt_mac);
                    // NOTE: 此处必须异步操作
                    //MessageSend(1883, pub_msg, strlen(pub_msg));
                    SendMsgToMQTT(pub_msg, strlen(pub_msg));
                    //MessageSend(1234, payload_bin, ret1);
					SendMsgToMCU((unsigned char *)payload_bin, payload_bin_len);
                    return 0;
                } else {
                    LOGD("__network time sync networkTime is %d don't setTimestamp\n", currentSec);
                    err = 1;
                }
            } else {
                err = 1;
            }
            if (1 == err) {
                // 失败
                char pub_msg[80];
                memset(pub_msg, '\0', 80);
                sprintf(pub_msg, "%s{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":1}",
                        DEFAULT_HEADER, msg_idStr, btWifiConfig.bt_mac);
                // NOTE: 此处必须异步操作
                //MessageSend(1883, pub_msg, strlen(pub_msg));
                SendMsgToMQTT(pub_msg, strlen(pub_msg));
                return -1;
            }
        }
        MqttInstruction instruction(x7_cmd, x7_cmd_code, timeout, msg_idStr);
        int ret = mqtt_instruction_pool.insertMqttInstruction(instruction);
        if (ret == -1) {
            char pub_msg[80];
            memset(pub_msg, '\0', 80);
            // sprintf(pub_msg, "%s{\"data\":\"%s\"}", DEFAULT_HEADER, "Resource Busy");
            // sprintf(pub_msg, "%s{\\\"msgId\\\":\\\"%s\\\", }", DEFAULT_HEADER, "Resource Busy");
            // 设备繁忙
            sprintf(pub_msg, "%s{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":3}", DEFAULT_HEADER,
                    msg_idStr, btWifiConfig.bt_mac);
            // NOTE: 此处必须异步操作
            //MessageSend(1883, pub_msg, strlen(pub_msg));
            SendMsgToMQTT(pub_msg, strlen(pub_msg));
            return -1;
        }
        if (x7_cmd == 0x24) {
            //MessageSend(1235, payload_bin, ret1);
            SendMsgToSelf((unsigned char *)payload_bin, payload_bin_len);
        } else {
            //MessageSend(1234, payload_bin, ret1);
        	SendMsgToMCU((unsigned char *)payload_bin, payload_bin_len);
        }
    } else {
        char pub_msg[50];
        memset(pub_msg, '\0', 50);
        sprintf(pub_msg, "%s{\"data\":\"%s\"}", DEFAULT_HEADER, "No Payload");
        // NOTE: 此处必须异步操作
        //MessageSend(1883, pub_msg, strlen(pub_msg));
        SendMsgToMQTT(pub_msg, strlen(pub_msg));
    }

    return 0;
}

int SendMsgToMQTT(char *mqtt_payload, int len) {
    char mqtt_payload_str[MQTT_AT_LEN];
    memset(mqtt_payload_str, '\0', MQTT_AT_LEN);
    strncpy(mqtt_payload_str, mqtt_payload, len);
    char msg[MQTT_AT_LEN];
    memset(msg, '\0', MQTT_AT_LEN);
    char first[MQTT_AT_LEN];
    char *pub_topic = NULL;
    char pub_msg[MQTT_AT_LEN];
    memset(pub_msg, '\0', MQTT_AT_LEN);
    LOGD("--- mqttpayload len is %d\n", len);
    // 主动上报功能
    if (mqtt_payload_str != NULL && strncmp(mqtt_payload_str, DEFAULT_HEADER, DEFAULT_HEADER_LEN) == 0) {
        // 内部异步发布消息
        mysplit(mqtt_payload_str, first, msg, (char *)":");
        LOGD("========================================= msg 1 is %s\n", msg);
        if (msg != NULL && strncmp(msg, "heartbeat", 9) == 0) {
            char *msgId = gen_msgId();
            if (versionConfig.sys_ver != NULL && strlen(versionConfig.sys_ver) > 0) {
                // sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"btmac\\\":\\\"%s\\\"\\,\\\"wifi_rssi\\\":%s\\,\\\"battery\\\":%d\\,\\\"version\\\":\\\"%s\\\"}", msgId, btWifiConfig.wifi_mac, btWifiConfig.bt_mac, wifi_rssi, battery_level, versionConfig.sys_ver);
                sprintf(pub_msg,
                        "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"wifi_rssi\\\":%s\\,\\\"battery\\\":%d\\,\\\"index\\\":%d\\,\\\"version\\\":\\\"%s\\\"}",
                        msgId, btWifiConfig.bt_mac, wifi_rssi, battery_level, g_heartbeat_index++,
                        versionConfig.sys_ver);
            } else {
                // sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"btmac\\\":\\\"%s\\\"\\,\\\"wifi_rssi\\\":%s\\,\\\"battery\\\":%d\\,\\\"version\\\":\\\"%s\\\"}", msgId, btWifiConfig.wifi_mac, btWifiConfig.bt_mac, wifi_rssi, battery_level, getFirmwareVersion());
                sprintf(pub_msg,
                        "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"wifi_rssi\\\":%s\\,\\\"battery\\\":%d\\,\\\"index\\\":%d\\,\\\"version\\\":\\\"%s\\\"}",
                        msgId, btWifiConfig.bt_mac, wifi_rssi, battery_level, g_heartbeat_index++,
                        getFirmwareVersion());
            }
            freePointer(&msgId);
            pub_topic = get_pub_topic_heart_beat();
            int ret = quickPublishMQTTWithPriority(pub_topic, pub_msg, 1);
            //freePointer(&pub_topic);
            return 0;
        } else if (msg != NULL && strncmp(msg, "sf:nodata", 9) == 0) {
            // 来源于pool，超时
            // TODO: check if need to shutdown
            g_has_more_download_data = 0;
            LOGD("no more data to download from server by timeout");
            return 0;
        } else if (msg != NULL && strncmp(msg, "startota:", 9) == 0) {
			mysplit(msg, first, pub_msg, ":");
			char *pub_topic_ota_request = get_pub_topic_ota_request();
			int ret = quickPublishMQTTWithPriority(pub_topic_ota_request, pub_msg, 1);
			//freePointer(&pub_topic_ota_request);
			return ret;
		} else if (msg != NULL && strncmp(msg, "ota", 3) == 0) {
			char *msgId = gen_msgId();
			sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"curr_ver\\\":\\\"%s\\\"}", msgId, versionConfig.oasis_ver);
			freePointer(&msgId);
			char *pub_topic_ota_request = get_pub_topic_ota_request();
			int ret = quickPublishMQTTWithPriority(pub_topic_ota_request, pub_msg, 1);
			//freePointer(&pub_topic_ota_request);
			return ret;
		} else if (msg != NULL && strncmp(msg, "disconnect", 9) == 0) {
            int ret = disconnectMQTT(MQTT_LINK_ID_DEFAULT);
            g_is_online = 0;
            return ret;
        } else if (msg != NULL && strncmp(msg, "reconnect", 9) == 0) {
            char lwtMsg[50];
            // sprintf(lwtMsg, "{\\\"username\\\":\\\"%s\\\"\\,\\\"state\\\":\\\"0\\\"}", btWifiConfig.wifi_mac);
            sprintf(lwtMsg, "{\\\"username\\\":\\\"%s\\\"\\,\\\"state\\\":\\\"0\\\"}", btWifiConfig.bt_mac);
            char *pub_topic_last_will = get_pub_topic_last_will();
            int ret = quickConnectMQTT(mqttConfig.client_id, mqttConfig.username, mqttConfig.password,
                                       mqttConfig.server_ip, mqttConfig.server_port, pub_topic_last_will, lwtMsg);
            //freePointer(&pub_topic_last_will);

            notifyMqttConnected(ret);
            if (ret < 0) {
            	LOGD("--------- Failed to reconnect to MQTT\n");
                return -1;
            }
            LOGD("--------- reconnect to mqtt done\n");
            // 订阅Topic
            char *sub_topic_cmd = get_sub_topic_cmd();
            ret = quickSubscribeMQTT(sub_topic_cmd);
            //freePointer(&sub_topic_cmd);
            // 订阅失败需要通知MCU
            if (ret < 0) {
            	LOGD("--------- Failed to resubscribe topic\n");
                notifyHeartBeat(CODE_FAILURE);
                return -1;
            }
            LOGD("--------- resubscribe to mqtt done\n");
            return ret;
        } else {
            pub_topic = get_pub_topic_cmd_res();
            int ret = quickPublishMQTTWithPriority(pub_topic, msg, 1);
            //freePointer(&pub_topic);
            return 0;
        }
    } else {
        char mqtt_msg_id[256];
        memset(mqtt_msg_id, '\0', 256);
        strcpy(mqtt_msg_id, mqtt_instruction_pool.getMsgId((char) (mqtt_payload[0]), (char) (mqtt_payload[1])));
        LOGD("out mqtt_msg_id is %s", mqtt_msg_id);
        if ((int) (char) (mqtt_payload[0]) == 0x24) {
            remove_mqtt_instruction_from_pool((char) (mqtt_payload[0]), (char) (mqtt_payload[1]));
            LOGD("in mqtt_msg_id is %s", mqtt_msg_id);
            pub_topic = get_pub_topic_cmd_res();
            LOGD("-------- pub_topic is %s\n", pub_topic);
            if ((int) (char) (mqtt_payload[1]) == 0x09 && (int) (char) (mqtt_payload[2]) == 0x01) {
                // 远程删除用户指令反馈
                // 远程删除用户成功
                if ((int) (char) (mqtt_payload[3]) == 0x00) {
                    printf("---- delete/clear user successfully\n");
                    // sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":\\\"%d\\\"}", mqtt_msg_id, btWifiConfig.wifi_mac, 0);
                    sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":\\\"%d\\\"}",
                            mqtt_msg_id, btWifiConfig.bt_mac, 0);
                } else {
                    // 远程删除用户失败
                    printf("---- delete/clear user fail\n");
                    // sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mqtt_payloadult\\\":\\\"%d\\\"\\}", mqtt_msg_id, 1);
                    // sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":\\\"%d\\\"}", mqtt_msg_id, btWifiConfig.wifi_mac, 500);
                    sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":\\\"%d\\\"}",
                            mqtt_msg_id, btWifiConfig.bt_mac, 1);
                }
                int ret = quickPublishMQTTWithPriority(pub_topic, pub_msg, 1);
                //freePointer(&pub_topic);
                return 0;
            } else {
            	LOGD("mqtt_payload unidentified");
            }
        } else if ((int) (char) (mqtt_payload[0]) == 0x23) {
            remove_mqtt_instruction_from_pool((char) (mqtt_payload[0]), (char) (mqtt_payload[1]));
            LOGD("in mqtt_msg_id is %s", mqtt_msg_id);
            pub_topic = get_pub_topic_cmd_res();
            LOGD("-------- pub_topic is %s\n", pub_topic);
            LOGD("---- from MCU\n");
			if ((int)(char)(mqtt_payload[1]) == 0x82 && (int)(char)(mqtt_payload[2]) == 0x01 &&(int)(char)(mqtt_payload[3]) == 0x01) {
				//freePointer(&pub_topic);
				pub_topic = get_pub_topic_heart_beat();
				// 远程开锁模式心跳上报
				LOGD("---- remote unlock door mode");
				char *msgId = gen_msgId();
				if (versionConfig.sys_ver!= NULL && strlen(versionConfig.sys_ver) > 0) {
					sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"wifi_rssi\\\":%s\\,\\\"battery\\\":%d\\,\\\"index\\\":%d\\,\\\"version\\\":\\\"%s\\\"}", msgId, btWifiConfig.bt_mac, wifi_rssi, battery_level, -1, versionConfig.sys_ver);
				} else {
					sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"wifi_rssi\\\":%s\\,\\\"battery\\\":%d\\,\\\"index\\\":%d\\,\\\"version\\\":\\\"%s\\\"}", msgId, btWifiConfig.bt_mac, wifi_rssi, battery_level, -1, getFirmwareVersion());
				}
				freePointer(&msgId);
				pub_topic = get_pub_topic_heart_beat();
				int ret = quickPublishMQTTWithPriority(pub_topic, pub_msg, 1);
				//freePointer(&pub_topic);
				return 0;
			} else if ((int)(char)(mqtt_payload[1]) == 0x83 && (int)(char)(mqtt_payload[2]) == 0x01) {
                // 远程开锁指令反馈
                // 远程开锁成功
                if ((int) (char) (mqtt_payload[3]) == 0x00) {
                	LOGD("---- unlock door successfully\n");
                    // sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":\\\"%d\\\"}", mqtt_msg_id, btWifiConfig.wifi_mac, 0);
                    sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":\\\"%d\\\"}",
                            mqtt_msg_id, btWifiConfig.bt_mac, 0);
                } else {
                    // 远程开锁失败
                	LOGD("---- unlock door fail\n");
                    // sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":\\\"%d\\\"}", mqtt_msg_id, btWifiConfig.wifi_mac, 500);
                    sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":\\\"%d\\\"}",
                            mqtt_msg_id, btWifiConfig.bt_mac, 1);
                }
                int ret = quickPublishMQTTWithPriority(pub_topic, pub_msg, 1);
                // notifyCommandExecuted(ret);
                g_command_executed = 1;
                g_heart_beat_executed = 1;
                //freePointer(&pub_topic);
                return 0;
            } else if ((int) (char) (mqtt_payload[1]) == 0x15 && (int) (char) (mqtt_payload[2]) == 0x01) {
                // 远程设置合同时间指令反馈
                // 远程设置合同时间成功
                if ((int) (char) (mqtt_payload[3]) == 0x00) {
                	LOGD("---- setup contract successfully\n");
                    // sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":\\\"%d\\\"}", mqtt_msg_id, btWifiConfig.wifi_mac, 0);
                    sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":\\\"%d\\\"}",
                            mqtt_msg_id, btWifiConfig.bt_mac, 0);
                } else {
                    // 远程设置合同时间失败
                	LOGD("---- setup contract fail\n");
                    // sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":\\\"%d\\\"}", mqtt_msg_id, btWifiConfig.wifi_mac, 500);
                    sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"result\\\":\\\"%d\\\"}",
                            mqtt_msg_id, btWifiConfig.bt_mac, 1);
                }
                int ret = quickPublishMQTTWithPriority(pub_topic, pub_msg, 1);
                LOGD("------- before sendStatus ToMCU ret %d\n", ret);
                // 远程设置合同时间成功，并不能立即要求下电，有可能要等待其他指令，比如远程开锁
                // sendStatusToMCU(0x04, ret);
                //freePointer(&pub_topic);
                return 0;
            } else if ((int) (char) (mqtt_payload[1]) == 0x1b) {
                // 注册/开门记录
                int id = (int) (mqtt_payload[3]);
                // TODO:
                LOGD("DBManager::getInstance()->getRecord start id %d", id);
                Record record;
                int ret = DBManager::getInstance()->getRecordByID(id, &record);
                if (ret == 0) {
                	LOGD("register/unlcok record is not NULL start upload record/image");
                    g_uploading_id = record.ID;
                    g_is_uploading_data = 1;
					// 优先上传最新的记录
					while (g_is_auto_uploading == 1) {
						// 如果正在自动上传中，睡眠100ms继续等待
						//usleep(100000);
						vTaskDelay(pdMS_TO_TICKS(100));
					}
                    int ret = uploadRecordImage(&record, true);
                    LOGD("uploadRecordImage end\r\n");
                    // notifyCommandExecuted(ret);
                    g_is_uploading_data = 0;
                    g_command_executed = 1;
                    g_heart_beat_executed = 1;
                    // TODO:
                } else {
                	LOGD("register/unlcok record is NULL");
                }
            } else if ((int) (char) (mqtt_payload[1]) == 0x14 && (int) (char) (mqtt_payload[2]) == 0x0f) {
                // 开门记录上报指令
                if ((int) (char) (mqtt_payload[2]) == 0x0f) {
                	LOGD("unlock record upload to server\r\n");
                    char uid[17];
                    HexToStr(uid, (unsigned char *) (&mqtt_payload[3]), 8);
                    LOGD("unlock record uid %s\r\n", uid);
                    int unlockType = (int) (char) mqtt_payload[11];
                    unsigned int unlockTime = (unsigned int) StrGetUInt32((unsigned char *) (&mqtt_payload[12]));
                    int batteryLevel = (int) mqtt_payload[16];
                    int unlockStatus = (int) (char) mqtt_payload[17];
                    LOGD("unlock record uid %s, unlockType %d, unlockTime %u, batteryLevel %d, unlockStatus %d\r\n",
                             uid, unlockType, unlockTime, batteryLevel, unlockStatus);
                    // sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"userId\\\":\\\"%s\\\"\\, \\\"unlockMethod\\\":%d\\,\\\"unlockTime\\\":%u\\,\\\"batteryPower\\\":%d\\,\\\"unlockState\\\":%d}", gen_msgId(), btWifiConfig.wifi_mac, uid, unlockType, unlockTime, batteryLevel, unlockStatus);
                    sprintf(pub_msg,
                            "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"userId\\\":\\\"%s\\\"\\, \\\"unlockMethod\\\":%d\\,\\\"unlockTime\\\":%u\\,\\\"batteryPower\\\":%d\\,\\\"unlockState\\\":%d}",
                            gen_msgId(), btWifiConfig.bt_mac, uid, unlockType, unlockTime, batteryLevel, unlockStatus);
                    pub_topic = get_pub_topic_record_request();
                    int ret = quickPublishMQTTWithPriority(pub_topic, pub_msg, 1);
                    // notifyCommandExecuted(ret);
                    g_command_executed = 1;
                    g_heart_beat_executed = 1;
                    //freePointer(&pub_topic);
                    return ret;
                } else {
                    LOGD("unlock record length is %d instead of 15\r\n", (int) (char) (mqtt_payload[2]));
                }
            } else if ((int) (char) (mqtt_payload[1]) == CMD_MECHANICAL_LOCK) {
                // 机械开锁记录
                int id = (int) (mqtt_payload[3]);
                // TODO:
                LOGD("DBManager::getInstance()->getRecord start id %d\r\n", id);
                Record record;
                int ret = DBManager::getInstance()->getRecordByID(id, &record);
                if (ret == 0) {
                	LOGD("mechnical record is not NULL start upload record/image\r\n");
                    g_uploading_id = record.ID;
                    g_is_uploading_data = 1;
					// 优先上传最新的记录
					while (g_is_auto_uploading == 1) {
						// 如果正在自动上传中，睡眠100ms继续等待
						//usleep(100000);
						vTaskDelay(pdMS_TO_TICKS(100));
					}
                    int ret = uploadRecordImage(&record, true);
                    LOGD("uploadRecordImage end\r\n");
                    // notifyCommandExecuted(ret);
                    g_is_uploading_data = 0;
                    g_command_executed = 1;
                    g_heart_beat_executed = 1;
                    // TODO:
                } else {
                	LOGD("mechnical record is NULL");
                }
            } else {
            }
        }
        return -1;
    }

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
		if ((dataStr != NULL) && (strlen(dataStr) > 0)) {
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
                    SendMsgToMQTT(pub_msg, strlen(pub_msg));
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
				setTimestamp(currentSec);
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
			unsigned short cal_crc16 = CRC16_X25(reinterpret_cast<unsigned char*>(payload_bin), 3 + len);
			payload_bin[len + 3] = (char)cal_crc16;
			payload_bin[len + 4] = (char)(cal_crc16 >> 8);
			//result = MessageSend(1234, payload_bin, 5 + len);
        	result = SendMsgToMCU((unsigned char *)payload_bin, 5 + len);
		}
	}
	if (mqtt != NULL) {
		cJSON_Delete(mqtt);
		mqtt = NULL;
	}
	return result;
}

static char g_ota_data[UART_RX_BUF_LEN];


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
		mysplit(msg, firstWithLinkId, lastWithTopic, (char *)",\"");

		mysplit(lastWithTopic, topic, lastWithDataLen, (char *)"\",");

		mysplit(lastWithDataLen, dataLen, data, (char *)",");

		if (dataLen == NULL || data == NULL) {
			LOGD("analyzeMQTTMsgInternal dataLen is NULL or data is is NULL");
			return false;
		}
		int data_len = atoi(dataLen);

		if (data != NULL) {
			if (strlen(data) >= data_len) {
				char *ota_topic_cmd = get_ota_topic_cmd();
				if (strncmp(topic, ota_topic_cmd, strlen(ota_topic_cmd)) == 0) {
					if (data[0] == '{') {
						memset(g_ota_data, '\0', sizeof(g_ota_data));
						strcpy(g_ota_data, data);
					} else {
						int len = strlen(g_ota_data);
						strcpy(&g_ota_data[len], data);
					} 
					if (data[data_len - 1] == '}') {
						// log_info("get ota data %d: %s", strlen(g_ota_data), g_ota_data);
						char msgId[MQTT_AT_LEN];
						memset(msgId, '\0', MQTT_AT_LEN);
						int ret = analyzeOTA(g_ota_data, (char*)&msgId);
						LOGD("analyzeOTA ret %d", ret);
					}
					//freePointer(&ota_topic_cmd);
					return true;
				}
				//freePointer(&ota_topic_cmd);
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
				} else if (strncmp((const char*)recv_msg_lines[current_handle_line], ">+MQTTPUB:OK", 12) == 0 || strncmp((const char*)recv_msg_lines[current_handle_line], "+MQTTPUB:OK", 11) == 0) {
					at_cmd_result = AT_CMD_RESULT_OK;
					LOGD("raw data sent OK\r\n");
				} else if (strncmp((const char*)recv_msg_lines[current_handle_line], ">+MQTTPUB:ERROR", 15) == 0 || strncmp((const char*)recv_msg_lines[current_handle_line], "+MQTTPUB:ERROR", 14) == 0) {
                    at_cmd_result = AT_CMD_RESULT_ERROR;
                    LOGD("raw data sent ERROR\r\n");
                }else if (strncmp((const char*)recv_msg_lines[current_handle_line], MQTT_CWJAP, MQTT_CWJAP_SIZE) == 0) {
                    LOGD("\r\n----------------- RUN COMMAND CWJAP ---------- \r\n");
                    char *second = (char *) strstr((const char *) recv_msg_lines[current_handle_line], MQTT_CWJAP);
                    // TODO: 确认是否有OK
                    // GET RSSI

                    rx_len = 0;
                    LOGD("----------------");
                    LOGD("uart rx get wifi RSSI:%s\r\n", second);
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
                            strcpy(wifi_rssi, field42);
                            //at_state = AT_CMD_OK;
							g_is_wifi_connected = 1;
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
                                update_mac(device_mac_from_module);
                            //}
                        }
					}
					// TODO: 确认是否有OK
					// GET MAC
				}else if (strncmp((const char*)recv_msg_lines[current_handle_line], "+MQTTSUBRECV:", 13) == 0) {
					LOGD("####receive subscribe message from mqtt server %s \r\n", (const char*)recv_msg_lines[current_handle_line]);
                    int ret = analyzeMQTTMsgInternal((char*)recv_msg_lines[current_handle_line]);
                }else if (((strncmp((const char*)recv_msg_lines[current_handle_line], MQTT_DISCONNECT, MQTT_DISCONNECT_SIZE) == 0) ||
                        (strncmp((const char*)recv_msg_lines[current_handle_line], MQTT_RAW_DISCONNECT, MQTT_RAW_DISCONNECT_SIZE) == 0)) && (mqtt_init_done == 1)) {
                    LOGD("###receive disconnect message from wifi_module \r\n");
                    g_has_more_upload_data = 0;
                    g_has_more_download_data = 0;
#if 0
                    char pub_msg[50];
                    memset(pub_msg, '\0', 50);
                    sprintf(pub_msg, "%s%s", DEFAULT_HEADER, "reconnect");
                    // NOTE: 此处必须异步操作
                    //MessageSend(1883, pub_msg, strlen(pub_msg));
                    SendMsgToMQTT(pub_msg, strlen(pub_msg));
#endif
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

int uploadRecord(char *msgId, Record *record) {
	char pub_msg[MQTT_AT_LONG_LEN];
	// int result = record->passed ? 0 : 1;
	short power = record->power;
	short power1 = power / 256;
	if(power1 == 255) {
	    power1 = -1;
	}
	short power2 = power % 256;
	if(power2 == 255) {
	    power2 = -1;
	}
	short action_upload = record->action_upload;
	short action = action_upload / 256;
	short upload = action_upload % 256;
	//LOGD("upload record uid %s, type %d, time %d, batteryA %d, batteryB %d, result %d, upload %d\n", record->UUID, record->action, record->time_stamp, record->power, record->power2, record->status, record->upload);
	LOGD("upload record uid %s, type %d, time %d, batteryA %d, batteryB %d, result %d, upload %d\n", record->UUID, action, record->time_stamp, power1, power2, 0/*record->status*/, upload);
	// sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"userId\\\":\\\"%s\\\"\\, \\\"type\\\":%d\\,\\\"time\\\":%u\\,\\\"batteryA\\\":%d\\,\\\"batteryB\\\":%d\\,\\\"result\\\":%d}", msgId, btWifiConfig.bt_mac, record->UID, record->type, record->time, record->power, record->power2, result);
	//sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"userId\\\":\\\"%s\\\"\\, \\\"type\\\":%d\\,\\\"time\\\":%d\\,\\\"batteryA\\\":%d\\,\\\"batteryB\\\":%d\\,\\\"result\\\":%d}", msgId, btWifiConfig.bt_mac, record->UUID, record->action, record->time_stamp, record->power, record->power2, record->status);
	sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"userId\\\":\\\"%s\\\"\\, \\\"type\\\":%d\\,\\\"time\\\":%d\\,\\\"batteryA\\\":%d\\,\\\"batteryB\\\":%d\\,\\\"result\\\":%d}", msgId, btWifiConfig.bt_mac, record->UUID, action, record->time_stamp, power1, power2, 0/*record->status*/);
	char *pub_topic = get_pub_topic_record_request();
	while (g_priority != 0) {
		//usleep(500000);	// 睡眠0.5s
		vTaskDelay(pdMS_TO_TICKS(500));
	}
    //LOGD("%s pub_topic is %s, pub_msg is %s\r\n", __FUNCTION__, pub_topic, pub_msg);
	int ret = quickPublishMQTT(pub_topic, pub_msg);
	if (ret == 0) {
		//record->upload = 1;
		record->action_upload = (record->action_upload & 0xFF00) + 1;
        DBManager::getInstance()->updateRecordByID(record);
	}
	// notifyCommandExecuted(ret);
	//freePointer(&pub_topic);
	return ret;
}

int uploadRecordImage(Record *record, bool online) {
	LOGD("uploadRecordImage online is %d \r\n", online);
	if(online) {
            char *pub_topic = get_pub_topic_pic_report();
            char *msgId = gen_msgId();
            //LOGD("uploadRecordImage msgId is %s\r\n", msgId);
            // 第一步：传记录
            bOasisRecordUpload = false;
            //int ret = pubOasisImage(pub_topic, msgId);
            LOGD("第一步：传记录 \r\n");
            int ret = uploadRecord(msgId, record);
            if (ret == 0) {
                //ret = uploadRecord(msgId, record);
                //record->upload = 1;
            	if((record->action_upload & 0xFF00) != 0xB00) {
            		record->action_upload = (record->action_upload & 0xFF00) + 1;
            	}else {
            		record->action_upload = (record->action_upload & 0xFF00) + 2;
            		bOasisRecordUpload = true;
            	}
            }else{
                LOGD("第一步:传记录失败 \r\n");
            }
            // 第二步：传图片
            if((record->action_upload & 0xFF00) != 0xB00) {
                LOGD("第二步：传图片 %s \r\n", record->image_path );
                ret = pubOasisImage(pub_topic, msgId);
				if (ret == 0) {
					// 第三步：再传记录
                    LOGD("第三步：再传记录 \r\n");
					ret = uploadRecord(msgId, record);
					if (ret == 0) {
						//record->upload = 2;
						record->action_upload = (record->action_upload & 0xFF00) + 2;
					}else{
					    LOGD("第三步:再传记录失败 \r\n");
					}
					bOasisRecordUpload = true;
				}else{
				    LOGD("第二步:传图片失败 \r\n");
				}
            }
            //LOGD("uploadRecordImage record->upload is %d\r\n", record->upload);
            LOGD("uploadRecordImage record->action_upload is 0x%X\r\n", record->action_upload);

            DBManager::getInstance()->updateRecordByID(record);

            freePointer(&pub_topic);
            freePointer(&msgId);
            return ret;
    }else {
        char *filename = (char*)(record->image_path);
        LOGD("uploadRecordImage filename is %s\r\n", filename);
        if (filename != NULL && strlen(filename) > 0 && (fatfs_getsize(filename)) != -1) {
            char *pub_topic = get_pub_topic_pic_report();
            char *msgId = gen_msgId();
            //LOGD("uploadRecordImage msgId is %s\r\n", msgId);
            // 第一步：传记录
            LOGD("第一步：传记录 \r\n");
            int ret = uploadRecord(msgId, record);
            //int ret = pubImage(pub_topic, filename, msgId);
            if (ret == 0) {
                //record->upload = 1;
            	if((record->action_upload & 0xFF00) != 0xB00) {
            		record->action_upload = (record->action_upload & 0xFF00) + 1;
            	}else {
            		record->action_upload = (record->action_upload & 0xFF00) + 2;
            	}
            }else{
                LOGD("第一步:传记录失败 \r\n");
            }
            // 第二步：传图片
            if((record->action_upload & 0xFF00) != 0xB00) {
                LOGD("第二步：传图片 %s \r\n", record->image_path );
				ret = pubImage(pub_topic, filename, msgId);
				if(ret == 0) {
					// 第三步：再传记录
                    LOGD("第三步：再传记录 \r\n");
					ret = uploadRecord(msgId, record);
					if (ret == 0) {
						//record->upload = 2;
						record->action_upload = (record->action_upload & 0xFF00) + 2;
						LOGD("%s delete start \r\n", __FUNCTION__);
						fatfs_delete(filename);
						LOGD("%s delete end \r\n", __FUNCTION__);
					}else{
					    LOGD("第三步:再传记录失败 \r\n");
					}
				}else{
				    LOGD("第二步:传图片失败 \r\n");
				}
            }
            DBManager::getInstance()->updateRecordByID(record);

            freePointer(&pub_topic);
            freePointer(&msgId);
            return ret;
        }
        return -1;
	}
}

int uploadRecords() {
    LOGD("上传历史记录 \r\n");
	g_is_auto_uploading = 1;
	if (g_is_uploading_data == 1) {
		g_is_auto_uploading = 2;
		return -2;
	}

	int fret = 0;
	list<Record*> records = DBManager::getInstance()->getAllUnuploadRecord();
    LOGD("---------------------- register: upload record and image start\r\n");
	// 第一步，只上传未上传的注册记录以及图片，涉及到可能存在的重复上传问题: 注册优先
    list <Record*>::iterator it;
	//for (int i = 0; i < recordNum; i++) {
    for ( it = records.begin( ); it != records.end( ); it++ ) {
        Record* record = (Record*) *it;
		//Record record = records[i];

		LOGD("---------------------- register: upload record id %d g_uploading_id %d\r\n", record->ID, g_uploading_id);
		if(g_is_shutdown == 0) {
		    if ((record->action_upload == 0) && (g_uploading_id != record->ID)) {
		        notifyKeepAlive();
		        vTaskDelay(pdMS_TO_TICKS(20));

		        int ret = uploadRecordImage(record, false);
		        if (g_is_uploading_data == 1) {
		            g_is_auto_uploading = 2;
		            return -2;
		        }
		        if (ret != 0) {
		            LOGD("register: upload record and image id %d fail with ret %d\r\n", record->ID, ret);
		            if (fret == 0) {
		                fret = ret;
		            }
		            //break;
		        } else {
		            LOGD("register: upload record and image id %d success\r\n", record->ID);
		        }
		    }
		}
	}
	records = DBManager::getInstance()->getAllUnuploadRecord();
	LOGD("--------------------- register/opendoor: upload records only start\r\n");
	// 第二步，只上传未上传的注册/开门记录，包括注册记录和开门记录: 记录次之
	//for (int i = 0; i < recordNum; i++) {
	for ( it = records.begin( ); it != records.end( ); it++ ) {
		//Record record = records[i];

		Record* record = (Record*) *it;
		if(g_is_shutdown == 0) {
		    if ((record->action_upload & 0xFF) == 0) {
		        notifyKeepAlive();
		        vTaskDelay(pdMS_TO_TICKS(20));

		        int ret = uploadRecord(gen_msgId(), record);
		        if (g_is_uploading_data == 1) {
		            g_is_auto_uploading = 2;
		            return -2;
		        }
		        if (ret != 0) {
		            LOGD("register/opendoor: upload records id %d fail with ret %d\r\n", record->ID, ret);
		            if (fret == 0) {
		                fret = ret;
		            }
		            //break;
		        } else {
		            LOGD("register/opendoor: upload records id %d success\r\n", record->ID);
		        }
		    }
		}
	}
	records = DBManager::getInstance()->getAllUnuploadRecord();
	LOGD("------------------- opendoor: upload record and image success\r\n");
	// 第三步，上传未上传的开门记录以及图片, 开门图片最后
	//for (int i = 0; i < recordNum; i++) {
	for ( it = records.begin( ); it != records.end( ); it++ ) {
		//Record record = records[i];

		Record* record = (Record*) *it;

		LOGD("---------------------- opendoor: upload record id %d g_uploading_id %d\r\n", record->ID, g_uploading_id);
		if(g_is_shutdown == 0) {
		    if (((record->action_upload & 0xFF) == 0 || (record->action_upload & 0xFF) == 1) && g_uploading_id != record->ID) {
		        notifyKeepAlive();
		        vTaskDelay(pdMS_TO_TICKS(20));

		        int ret = uploadRecordImage(record, false);
		        if (g_is_uploading_data == 1) {
		            g_is_auto_uploading = 2;
		            return -2;
		        }
		        if (ret != 0) {
		            LOGD("opendoor: upload record and image id %d fail with ret %d\r\n", ret, record->ID);
		            if (fret == 0) {
		                fret = ret;
		            }
		            //break;
		        } else {
		            LOGD("opendoor: upload record and image id %d success\r\n", record->ID);
		        }
		    }
		}
	}
    g_is_auto_uploading = 0;
	return fret;
}

int MAX_COUNT = 5;

static void msghandle_task(void *pvParameters)
{
    char const *logTag = "[UART8_WIFI]:msghandle_task-";
    LOGD("%s start...\r\n", logTag);
	int count = 0;

    int is_online_handled = 0;	// g_is_online只处理一次

    memset(wifi_rssi, '\0', sizeof(wifi_rssi));
    wifi_rssi[0] = '0';
    int TIMEOUT_COUNT = 30;
	//mqtt_init_done = 1;
    do {
        vTaskDelay(pdMS_TO_TICKS(1000));

        if (count % 30 == 0) {
			if (g_has_more_upload_data == 0 && g_is_uploading_data == 0) {
				update_rssi();
			}
        }

        // 如果后台已经有指令反馈，我们认为已经连接上后台了，这个时候，30s进行一次心跳
        if (g_is_online == 1 && is_online_handled == 0) {
            // 通知MCU，需要将后台指示灯长亮
            MAX_COUNT = 30;
            notifyHeartBeat(CODE_SUCCESS);
            vTaskDelay(pdMS_TO_TICKS(20));
            is_online_handled = 1;
        }

        // 判断是否需要下电
        if (g_is_online == 1) {
            if (g_has_more_download_data == 0) {
                // 没有更多下载数据
                if (g_has_more_upload_data == 0 && g_is_uploading_data == 0 && (g_shutdown_notified == 0 || (g_shutdown_notified == 1 && count % 5 == 0))) {
                    // 没有更多上传数据
                    if (g_command_executed) {
                        // 如果远程开锁完成或者其他指令执行完成，并且上传也执行完成了
                        if(g_is_shutdown == 0) {
                        	g_is_shutdown = 1;
							CloseLcdBackground();
							LOGD("need to notify command executed 1\r\n");
							notifyCommandExecuted(0);
							vTaskDelay(pdMS_TO_TICKS(100));
							save_files_before_pwd();
                        }
                        // } else if (g_shutdown_notified == 0) {
                    } else { // if (g_shutdown_notified == 0) {
                        // 如果没有收到指令执行完成的状况，可以尝试通知MCU下电，MCU根据是否是远程开锁决定是否下电
							notifyShutdown();
                    }
                    g_shutdown_notified = 1;
                } else if (count % 10 == 0) {
                    // 每隔10s发送一次心跳，确保不会随意下电
                    LOGD("need to keep alive1\r\n");
                    //LOGD("g_has_more_upload_data is %d, g_is_uploading_data is %d\r\n", g_has_more_upload_data, g_is_uploading_data);
                    notifyKeepAlive();
                    vTaskDelay(pdMS_TO_TICKS(20));
                }
			} else if (count % 10 == 0) {
				// 每隔10s发送一次心跳，确保不会随意下电
				LOGD("need to keep alive2\r\n");
				notifyKeepAlive();
                vTaskDelay(pdMS_TO_TICKS(20));
			}
        } else if (g_is_online == 0) {
            // 30秒，如果还没有连接上后台，可以下电
			if (count >= TIMEOUT_COUNT) {
				// TODO: 后续可以使用下电指令来代替
				// notifyShutdown();
				if(g_is_shutdown == 0) {
					g_is_shutdown = 1;
					CloseLcdBackground();
					LOGD("need to notify command executed 2\r\n");
					notifyCommandExecuted(0);
					vTaskDelay(pdMS_TO_TICKS(100));
					save_files_before_pwd();
				}
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

static void send_heartbeat_task(void *pvParameters)
{
    char const *logTag = "[UART8_WIFI]:send_heartbeat_task-";
    LOGD("%s start...\r\n", logTag);
	int count = 0;

    char *pub_topic = NULL;
    char pub_msg[MQTT_AT_LONG_LEN];
    do {
        vTaskDelay(pdMS_TO_TICKS(1000));

        if((count % MAX_COUNT == 0) || (g_heart_beat_executed == 1)) {
            if ((mqtt_init_done == 1) && (bPubOasisImage == false) && (g_is_uploading_data == 0)) {
                char *msgId = gen_msgId();
                memset(pub_msg, '\0', MQTT_AT_LONG_LEN);
                // sprintf(pub_msg, "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"btmac\\\":\\\"%s\\\"\\,\\\"wifi_rssi\\\":%s\\,\\\"battery\\\":%d\\,\\\"version\\\":\\\"%s\\\"}", msgId, btWifiConfig.wifi_mac, btWifiConfig.bt_mac, wifi_rssi, battery_level, getFirmwareVersion());
                sprintf(pub_msg,
                        "{\\\"msgId\\\":\\\"%s\\\"\\,\\\"mac\\\":\\\"%s\\\"\\,\\\"wifi_rssi\\\":%s\\,\\\"battery\\\":%d\\,\\\"index\\\":%d\\,\\\"version\\\":\\\"%s\\\"}",
                        msgId, btWifiConfig.bt_mac, wifi_rssi, battery_level, g_heartbeat_index++,
                        getFirmwareVersion());
                freePointer(&msgId);
                pub_topic = get_pub_topic_heart_beat();
                //LOGD("%s pub_msg is %s\n", __FUNCTION__, pub_msg);
                int ret = quickPublishMQTTWithPriority(pub_topic, pub_msg, 1);
                if(g_heart_beat_executed == 1) {
                	g_heart_beat_executed = 0;
                }
            }
        }

        count++;

    } while (1);
    vTaskDelete(NULL);
    LOGD("\r\n%s end...\r\n", logTag);
}

static void uploaddata_task(void *pvParameters)
{
    char const *logTag = "[UART8_WIFI]:uploaddata_task-";
    LOGD("%s start...\r\n", logTag);
    bool mqtt_upload_records_run = false;

    do {
        vTaskDelay(pdMS_TO_TICKS(1000));

        if((boot_mode == BOOT_MODE_NORMAL) || (boot_mode == BOOT_MODE_REMOTE) || (boot_mode == BOOT_MODE_MECHANICAL_LOCK)) {
            if (g_has_more_download_data == 0 && g_has_more_upload_data == 1) {
                if ((mqtt_init_done == 1) && (g_priority == 0) && (bPubOasisImage == false)) {
                    if (mqtt_upload_records_run == false) {
                        LOGD("----------------- g_priority == 0");
                        int ret = uploadRecords();
                        mqtt_upload_records_run = true;
                    }
                    g_has_more_upload_data = 0;
                }else {
                    //LOGD("mqtt_init_done is %d, g_priority is %d, bPubOasisImage is %d\r\n", mqtt_init_done, g_priority, bPubOasisImage);
                }
            }else {
                //LOGD("g_has_more_download_data is %d, g_has_more_upload_data is %d\r\n", g_has_more_download_data, g_has_more_upload_data);
            }
        }else {
            g_has_more_upload_data = 0;
        }

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
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(mqttinit_task, "mqttinit_task", MQTTTASK_STACKSIZE, NULL, MQTTTASK_PRIORITY,
                                        MqttTaskStack, &s_MqttTaskTCB))
#else
    if (xTaskCreate(mqttinit_task, "mqttinit_task", MQTTTASK_STACKSIZE, NULL, MQTTTASK_PRIORITY, NULL) != pdPASS)
#endif
    {
//        PRINTF("Task mqttinit_task creation failed!.\r\n");
        LOGD("%s failed to create mqttinit_task\r\n", logTag);
        while (1);
    }
#ifndef TIMEOUT_TEST
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(uartrecv_task, "uartrecv_task", UART8RECVTASK_STACKSIZE, NULL, UART8RECVTASK_PRIORITY,
                                            Uart8RecvStack, &s_Uart8RecvTaskTCB))
#else
    if (xTaskCreate(uartrecv_task, "uartrecv_task", UART8RECVTASK_STACKSIZE, NULL, UART8RECVTASK_PRIORITY, NULL) != pdPASS)
#endif
#else
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(uartrecv_timeout_task, "uartrecv_timeout_task", UART8RECVTASK_STACKSIZE, NULL, UART8RECVTASK_PRIORITY,
                                            Uart8RecvStack, &s_Uart8RecvTaskTCB))
#else
    if (xTaskCreate(uartrecv_timeout_task, "uartrecv_timeout_task", UART8RECVTASK_STACKSIZE, NULL, UART8RECVTASK_PRIORITY, NULL) != pdPASS)
#endif
#endif
    {
        LOGD("%s failed to create uartrecv_task\r\n", logTag);
        while (1);
    }
#ifndef TIMEOUT_TEST
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(msghandle_task, "msghandle_task", UART8MSGHANDLETASK_STACKSIZE, NULL, UART8MSGHANDLETASK_PRIORITY,
                                            Uart8MsgHandleTaskStack, &s_Uart8MsgHandleTaskTCB))
#else
    if (xTaskCreate(msghandle_task, "msghandle_task", UART8RECVTASK_STACKSIZE, NULL, UART8MSGHANDLETASK_PRIORITY, NULL) != pdPASS)
#endif
    {
    	LOGD("%s failed to create msghandle_task\r\n", logTag);
    	while (1);
    }
#endif

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(send_heartbeat_task, "send_heartbeat_task", UART8SENDHEARTBEATTASK_STACKSIZE, NULL, UART8SENDHEARTBEATTASK_PRIORITY,
                                            Uart8SendheartbeatTaskStack, &s_Uart8SendheartbeatTaskTCB))
#else
    if (xTaskCreate(send_heartbeat_task, "send_heartbeat_task", UART8SENDHEARTBEATTASK_STACKSIZE, NULL, UART8SENDHEARTBEATTASK_PRIORITY, NULL) != pdPASS)
#endif
    {
    	LOGD("%s failed to create send_heartbeat_task\r\n", logTag);
    	while (1);
    }

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (NULL == xTaskCreateStatic(uploaddata_task, "uploaddata_task", UART8UPLOADDATATASK_STACKSIZE, NULL, UART8UPLOADDATATASK_PRIORITY,
                                            Uart8UploadDataTaskStack, &s_Uart8UploadDataTaskTCB))
#else
    if (xTaskCreate(uploaddata_task, "uploaddata_task", UART8UPLOADDATATASK_STACKSIZE, NULL, UART8UPLOADDATATASK_PRIORITY, NULL) != pdPASS)
#endif
    {
    	LOGD("%s failed to create uploaddata_task\r\n", logTag);
    	while (1);
    }

    LOGD("%s started...\r\n", logTag);
    return 0;
}
