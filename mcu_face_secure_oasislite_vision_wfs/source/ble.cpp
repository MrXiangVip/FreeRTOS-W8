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

#include "ble.h"

#include "database.h"
#include "fsl_shell.h"
#include "fsl_lpuart_freertos.h"
#include "FsciCommunication.h"
#include "cmd_ble.h"
#include "shell_ble.h"
#include "cmd_threadip.h"
#include "shell_threadip.h"

#include "cJSON.h"
#include "database.h"
#include "remote_cmds.h"
#include "switch.h"
#include "sln_shell.h"

#define MAX_BLE_MTU 244

/* BIT 0 is set to start the wireless UART app and start advertising */
#define BIT_0        (1 << 0)
/* BIT 1 is set to start the camera after user validation */
#define BIT_1        (1 << 1)
/*Bit 2 is set to start receiving the JSON data chunks */
#define BIT_2        (1 << 2)

/* Queue Parameters */
#define RECV_DATA_QUEUE_LENGTH        40
#define RECV_DATA_QUEUE_ITEM_SIZE    MAX_BLE_MTU

/* JSON Items */
#define COMMAND_STRING            "command"
#define DEVICENAME_STRING        "deviceName"
#define PASSCODE_STRING            "pass_code"
#define LENGTH_STRING            "length"
#define COOKIE_STRING            "cookie"
#define ACCEPT_REQUEST_STRING    "accept_request"

/* Command Cases */
#define CMD_USERINFO        "user_information"
#define CMD_USERDATA        "training_information"
#define CMD_SNAPSHOT        "snapshot_information"

/*******************************************************************************
 * GLobal  Declarations
 ******************************************************************************/

enum command_states_t {
    Command_Userinfo,
    Command_Userdata,
    Command_Snapshot,
    Unknown_Command
};

static thrEvtContainer_t thrEvtcontainer;
static bleEvtContainer_t bleEvtContainer;

cJSON* parsed_output = NULL;

bool_t g_ready_flag = FALSE;
bool_t g_data_avail = FALSE;
bool_t g_cam_stop = FALSE;
bool_t g_list_clear = FALSE;


/*******************************************************************************
 * FSCI Declarations
 ******************************************************************************/
static const char* send_ring_overrun = "\r\nRing buffer overrun!\r\n";
static const char* send_hardware_overrun = "\r\nHardware buffer overrun!\r\n";

lpuart_rtos_handle_t handle;
static uint8_t background_buffer[4096]; /*!< LPUART RX software ring buffer */
static struct _lpuart_handle t_handle;

static lpuart_rtos_config_t lpuart_config;

EventGroupHandle_t g_event_group;
QueueHandle_t g_recv_data_queue;

extern wuartState_t gWUartState;

static QMsg bleResetMsg;

static void SHELL_SendDataCallback(uint8_t* buf, uint32_t len)
{
    while (len--) {
        PUTCHAR(*(buf++));
    }
}

static void SHELL_ReceiveDataCallback(uint8_t* buf, uint32_t len)
{
    while (len--) {
        *(buf++) = GETCHAR();
    }
}

static int32_t compareString(const char *str1, const char *str2, int32_t count)
{
    while (count--)
    {
        if (*str1++ != *str2++)
        {
            return *(unsigned char *)(str1 - 1) - *(unsigned char *)(str2 - 1);
        }
    }
    return 0;
}

uint8_t getCmdState(char* cmd)
{
    if ( compareString(cmd, CMD_USERINFO, sizeof(CMD_USERINFO)) == 0 ) {
        return Command_Userinfo;
    } else if ( compareString(cmd, CMD_USERDATA, sizeof(CMD_USERDATA)) == 0 ) {
        return Command_Userdata;
    } else if ( compareString(cmd, CMD_SNAPSHOT, sizeof(CMD_SNAPSHOT)) == 0 ) {
        return Command_Snapshot;
    } else {
        return Unknown_Command;
    }
}

cJSON* getItemObject(cJSON* data, char* item_str)
{
    return cJSON_GetObjectItemCaseSensitive(data, item_str);
}

void processUserdata(void)
{
    uint32_t len;
    cJSON* length;
    uint32_t packet_count;
    char* buffer_str, *buffer_ptr;

    length = getItemObject(parsed_output, LENGTH_STRING);

    if ( length == NULL ) {
        UsbShell_Printf("Could not get item length\r\n");
        return;
    }

    len = (uint32_t) length->valuedouble;

    packet_count = ( len % RECV_DATA_QUEUE_ITEM_SIZE ) ? ( (len / RECV_DATA_QUEUE_ITEM_SIZE) + 1 ) :  ( len / RECV_DATA_QUEUE_ITEM_SIZE );
    buffer_str = (char*)pvPortMalloc(sizeof(char) * (packet_count * RECV_DATA_QUEUE_ITEM_SIZE));

    if ( buffer_str == NULL ) {
        UsbShell_Printf("Malloc Failed\r\n");
        return;
    }

    buffer_ptr = buffer_str;

    while ( packet_count-- ) {
        vTaskDelay(pdMS_TO_TICKS(5));

        if ( (xQueueReceive(g_recv_data_queue, buffer_ptr, portMAX_DELAY)) != pdPASS ) {
            UsbShell_Printf("Cannot receive data from queue\r\n");
            continue;
        }

        buffer_ptr += RECV_DATA_QUEUE_ITEM_SIZE;
    }

    buffer_ptr = buffer_str;
    UsbShell_Printf("[UserData]:%d\r\n", len);

    if (buffer_str[0] != FEATUREDATA_MAGIC_VALID) {
        UsbShell_Printf("[ERROR]:invalid user data\r\n");
    } else {
        switch (buffer_str[1]) {
        case OP_ADD_FACE:
            Remote_FaceAdd(buffer_str + 2, len - 2);
            break;

        case OP_KEY_ADD_START:
            Remote_KeyAdd(buffer_str + 2, len - 2);
            break;

        default:
            UsbShell_Printf("[ERROR]:invalid operation\r\n");
            break;
        }
    }

    vPortFree(buffer_str);
}

static void KHC_ReceiveCb(clientPacket_t* pFsciPacket, uint8_t vintf)
{
    switch (vintf) {
    case THR_FSCI_IF:
        //PRINTF("THR_FSCI_IF");
        KHC_ThreadIP_RX_MsgHandler(pFsciPacket, &thrEvtcontainer, vintf);
        //PRINTF("ID : %x \r\n",thrEvtcontainer.id);

#if FEATURE_BORDER_ROUTER

        if (thrEvtcontainer.id == 0xCFF3) {
            // IPv6 packet received
            SerialTun_IPPacketReceivedConfirm_t* ip6pkt = &(thrEvtcontainer.Data.SerialTun_IPPacketReceivedConfirm);
            struct pbuf* p = pbuf_alloc(PBUF_RAW_TX, ip6pkt->Size, PBUF_REF);

            if (p) {
                // point to payload
                p->payload = ip6pkt->IPpayload;

                // send it
                fsl_netif0.linkoutput(&fsl_netif0, p);

                // free packet
                MEM_BufferFree(p->payload);
                pbuf_free(p);
            }
        } else
#endif

        {
            SHELL_ThrEventNotify(&thrEvtcontainer);

            if ( thrEvtcontainer.Data.THR_EventGeneralConfirm.EventStatus == THR_EventGeneralConfirm_EventStatus_Resettofactorydefault ) {
                xEventGroupSetBits(g_event_group, BIT_0);
            }
        }

        break;

#if gHybridApp_d

    case BLE_FSCI_IF:
        KHC_BLE_RX_MsgHandler(pFsciPacket, &bleEvtContainer, vintf);
        SHELL_BleEventNotify(&bleEvtContainer);

        if ( gWUartState == gWUartConnectionEstablished_c && g_ready_flag == TRUE ) {
            if ( (xQueueSend(g_recv_data_queue, (char*)bleEvtContainer.Data.GATTServerAttributeWrittenWithoutResponseIndication.AttributeWrittenEvent.Value,
                             portMAX_DELAY)) != pdPASS ) {
                PRINTF("Cannot send data to queue\r\n");
            }

            g_ready_flag = FALSE;
        }

        break;
#endif

    default:
        while (1);
    }
}

static void blackbox_task(void* pvParameters)
{
    int error;

    g_recv_data_queue = xQueueCreate(RECV_DATA_QUEUE_LENGTH, RECV_DATA_QUEUE_ITEM_SIZE);

    if ( g_recv_data_queue == NULL ) {
        PRINTF("Queue not created\r\n");

        while (1);
    }

    FSCI_RegisterCb(KHC_ReceiveCb);

#if 0
    QMsg* pQMsg = &bleResetMsg;
    bleResetMsg.id = QMSG_BLE_RESET;
    Switch_SendQMsg((void*)&pQMsg);
#endif
#if RTVISION_BOARD
    KW41Reset();
#endif
    LOGD("[BLACK]:running\r\n");

    /* Receive user input and send it back to terminal. */
    do {
        error = FSCI_receivePacket();

        if (error == kStatus_LPUART_RxHardwareOverrun) {
            /* Notify about hardware buffer overrun */
            PRINTF(send_hardware_overrun);
        }

        if (error == kStatus_LPUART_RxRingBufferOverrun) {
            /* Notify about ring buffer overrun */
            PRINTF(send_ring_overrun);
        }
    } while (kStatus_Success == error);

    /* Should not get here */
    while (1);
}

static void bleFsciTask( void* args )
{
    uint8_t cmd_state;
    char received_str[256];
    cJSON* cmd;
    lpuart_config.srcclk = BOARD_DebugConsoleSrcFreq();
    lpuart_config.base = BLE_LPUART;

    NVIC_SetPriority(BLE_LPUART_IRQn, 5);

    if (0 > LPUART_RTOS_Init(&handle, &t_handle, &lpuart_config)) {
        vTaskSuspend(NULL);
    }

    //blackbox_task(NULL);

    BaseType_t xret = xTaskCreate(blackbox_task, "Bbox_task", BLACKTASK_STACKSIZE, NULL, blackbox_task_PRIORITY, NULL);

    if ( xret != pdPASS) {
        PRINTF("Bbox_task: Task creation failed!. %d \r\n", xret);

        while (1)
            ;
    }

    //SHELL_BLE_Init();

    xEventGroupWaitBits(g_event_group, BIT_0, pdTRUE, pdFALSE, portMAX_DELAY);
    BleApp_DemoWirelessUART();
    GAPStartAdvertisingRequest(BLE_FSCI_IF);

    LOGD("[FSCI]:running\r\n");

    while (1) {
        if ( gWUartState == gWUartConnectionEstablished_c ) {

            if ( (xQueueReceive(g_recv_data_queue, received_str, portMAX_DELAY)) != pdPASS ) {
                PRINTF("Cannot receive data from queue\r\n");
                continue;
            }

            parsed_output = cJSON_Parse(received_str);

            if ( parsed_output == NULL ) {
                PRINTF("Parsing failed\r\n");
                continue;
            }

            cmd = getItemObject(parsed_output, COMMAND_STRING);

            if ( cmd == NULL ) {
                PRINTF("Could not get item 'command'\r\n");
                continue;
            }

            LOGD("[cmd]:[%s]\r\n", cmd->valuestring);
            cmd_state = getCmdState(cmd->valuestring);

            switch (cmd_state) {

            case Command_Userinfo:
                //processUserinfo();
                break;

            case Command_Userdata:
                processUserdata();
                break;

            case Command_Snapshot:
                //takeSnapshot();
                break;

            default:
                PRINTF("Unknown Command\r\n");
                break;

            }

            cJSON_Delete(parsed_output);
        }

        vTaskDelay(pdMS_TO_TICKS(100));

    }

}

int BLE_Start(void)
{
    g_event_group = xEventGroupCreate();

    lpuart_config.baudrate = 230400;
    lpuart_config.parity = kLPUART_ParityDisabled;
    lpuart_config.stopbits = kLPUART_OneStopBit;
    lpuart_config.buffer = background_buffer;
    lpuart_config.buffer_size = sizeof(background_buffer);

    LOGD("[BLE]:start\r\n");

    if (xTaskCreate(bleFsciTask, "BLE Task", BLETASK_STACKSIZE,
                    NULL, host_task_PRIORITY, NULL) != pdPASS) {
        LOGE("[ERROR]Camera Task created failed\r\n");

        while (1);
    }

    LOGD("[BLE]:started\r\n");

    return 0;
}
