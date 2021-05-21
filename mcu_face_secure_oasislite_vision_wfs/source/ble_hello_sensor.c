/*
 * Copyright 2018, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 *
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 *
 * BLE Vendor Specific Device
 *
 * Features demonstrated
 *  - GATT database and Device configuration initialization
 *  - Registration with LE stack for various events
 *  - Sending data to the client
 *  - Processing write requests from the client
 *
 * To demonstrate the app, work through the following steps.
 * 1. Plug the WICED eval board into your computer
 * 2. Build and download the application (to the WICED board)
 * 3. On application start the device acts as a GATT server and advertises itself as Hello Sensor
 * 4. Connect to GATT server using one of the LE clients (LEExplorer(android)) or (BLE Utility(Apple Store))
 * 5. Once connected the client can read Hello sensor characteristics
 * 6. Write the hello sensor characteristic configuration value from client
 * 7. on the console terminal you can see the value entered in the client
 * 8. To recieve notification the user has to register for notification after LE connection on the client
 * 9. To test notifications user can enter the command "notify" on the console
 * 10. Each time when user enters the notify command a new value of notification can be seen on the LE client.
 *
 * ENABLE_APP_POWERSAVE is defined to enable power-save related changes in the applications
 * For accurate current measurement, turn off LEDs on the board.
 *
 */
/*ble stack intern header*/
#include <string.h>
#include <stdlib.h>
#include "wiced_bt_dev.h"
#include "wiced_bt_ble.h"
#include "wiced_bt_gatt.h"
#include "wiced_bt_cfg.h"
#include "ble_hello_sensor.h"
#include "bt_target.h"
#include "wiced_bt_stack.h"
#include "gattdefs.h"
#include "sdpdefs.h"
#include "wiced_bt_dev.h"
#include "fsl_debug_console.h"
#include "wiced_rtos.h"
#include "platform_imxrt.h"

/*customer define header*/
#include "commondef.h"
#include "cJSON.h"
#include "fsl_log.h"
#include "sln_shell.h"
#include "remote_cmds.h"

//#define SIMULATE_NVRAM_WITH_ITC_RAM

#define MAX_BLE_MTU 244

/* Queue Parameters */
#define RECV_DATA_QUEUE_LENGTH        40
#define RECV_DATA_QUEUE_ITEM_SIZE    MAX_BLE_MTU

/* JSON Items */
#define COMMAND_STRING           "command"
#define DEVICENAME_STRING        "dev"
#define PASSCODE_STRING          "pass_code"
#define LENGTH_STRING            "len"
#define COOKIE_STRING            "cookie"
#define ACCEPT_REQUEST_STRING    "accept_request"

/* Command Cases */
#define CMD_USERINFO        "user_information"
#define CMD_USERDATA        "training_information"
#define CMD_SNAPSHOT        "snapshot_information"
/******************************************************************************
 *                                Constants
 ******************************************************************************/
#define ERR_CMD_OK 0
#define ERR_UNKNOWN 1

#define MAX_ASCII_VALUE 0x7e
#define ROLL_OVER_ASCII_VALUE 0x2f

#ifndef BLE_KEY_FLASH_ADDR
#define BLE_KEY_FLASH_ADDR (0x1B00000)
#endif

extern const wiced_bt_cfg_settings_t wiced_bt_cfg_settings;
extern const wiced_bt_cfg_buf_pool_t wiced_bt_cfg_buf_pools[];

static QueueHandle_t g_recv_data_queue;
static cJSON* parsed_output = NULL;

static TaskHandle_t s_HandleTaskSendReady = NULL;
/******************************************************************************
 *                           Function Prototypes
 ******************************************************************************/
static void hello_sensor_application_init(void);
static wiced_bt_gatt_status_t hello_sensor_gatts_connection_status_handler(wiced_bt_gatt_connection_status_t *p_status);
static wiced_bt_gatt_status_t hello_sensor_gatts_connection_up(wiced_bt_gatt_connection_status_t *p_status);
static wiced_bt_gatt_status_t hello_sensor_gatts_connection_down(wiced_bt_gatt_connection_status_t *p_status);
static wiced_result_t hello_sensor_management_callback(wiced_bt_management_evt_t event,
                                                       wiced_bt_management_evt_data_t *p_event_data);
static wiced_bt_gatt_status_t hello_sensor_gatts_callback(wiced_bt_gatt_evt_t event,
                                                          wiced_bt_gatt_event_data_t *p_data);
static wiced_bt_gatt_status_t hello_sensor_gatt_server_read_request_handler(uint16_t conn_id,
                                                                            wiced_bt_gatt_read_t *p_read_data);
static wiced_bt_gatt_status_t hello_sensor_gatt_server_write_request_handler(uint16_t conn_id,
                                                                             wiced_bt_gatt_write_t *p_data);
static void hello_sensor_set_advertisement_data(void);
static void hello_sensor_set_scan_response_data(void);
static void hello_sensor_send_message(void);
int hello_sensor_send_notification(int argc, char *argv[]);
int hello_sensor_send_indication(int argc, char *argv[]);
static wiced_bt_gatt_status_t hello_sensor_gatt_server_write_and_execute_request_handler(
    uint16_t conn_id, wiced_bt_gatt_exec_flag_t exec_flag);
int hello_sensor_set_adv_tx_power(int argc, char *argv[]);
int hello_sensor_read_adv_tx_power(int argc, char *argv[]);
int hello_sensor_read_rssi(int argc, char *argv[]);
static void hello_sensor_tx_power_callback(wiced_bt_tx_power_result_t *p_tx_power);
static void hello_sensor_read_rssi_callback(wiced_bt_dev_rssi_result_t *p_rssi);

wiced_result_t ble_hello_sensor_load_key_to_addr_resolution_db(void);
void ble_hello_sensor_save_link_key(wiced_bt_device_link_keys_t paired_device_keys);
void ble_hello_sensor_read_link_key(wiced_bt_device_link_keys_t *paired_device_keys);

static int get_current_mtu_size(void);

void wiced_ble_send_ready_task(void *args);
#ifdef SIMULATE_NVRAM_WITH_ITC_RAM
int nvm_load_keys(wiced_bt_device_link_keys_t *keys, int key_len);
int nvm_save_keys(wiced_bt_device_link_keys_t *keys, int key_len);
#endif
/******************************************************************************
 *                                Structures
 ******************************************************************************/

typedef struct
{
    BD_ADDR remote_addr;          /* remote peer device address */
    uint32_t timer_count;         /* timer count */
    uint32_t fine_timer_count;    /* fine timer count */
    uint16_t conn_id;             /* connection ID referenced by the stack */
    uint16_t peer_mtu;            /* peer MTU */
    uint8_t flag_indication_sent; /* indicates waiting for confirmation */
    uint8_t flag_stay_connected;  /* stay connected or disconnect after all
                                   * messages are sent */
    uint8_t battery_level;        /* dummy battery level */
} hello_sensor_state_t;

typedef PACKED struct
{
    BD_ADDR bdaddr;                               /* BD address of the bonded host */
    uint16_t characteristic_client_configuration; /* Current value of the
                                                   * client configuration
                                                   * descriptor */
    uint8_t number_of_blinks;                     /* Sensor config, number of times to blink
                                                   * the LEd when button is pushed. */
} host_info_t;

typedef struct
{
    uint16_t handle;
    uint16_t attr_len;
    void *p_attr;
} attribute_t;

enum command_states_t {
    Command_Userinfo,
    Command_Userdata,
    Command_Snapshot,
    Unknown_Command
};

/******************************************************************************
 *                                Variables Definitions
 ******************************************************************************/

static uint8_t hello_sensor_device_name[]      = "hello-sensor";
static uint8_t hello_sensor_appearance_name[2] = {BIT16_TO_8(APPEARANCE_GENERIC_TAG)};
static char hello_sensor_char_notify_value[]   = {'H', 'E', 'L', 'L', 'O', '0'};
static char hello_sensor_char_mfr_name_value[] = {
    'N',
    'X',
    'P',
    0,
};
static char hello_sensor_char_model_num_value[]    = {'1', '2', '3', '4', 0, 0, 0, 0};
static uint8_t hello_sensor_char_system_id_value[] = {0xbb, 0xb8, 0xa1, 0x80, 0x5f, 0x9f, 0x91, 0x71};
static char hello_sensor_char_indicate_value[]     = {'I', 'N', 'D', 'I', 'C', 'A', 'T', 'E', '0'};
hello_sensor_state_t hello_sensor_state;
host_info_t hello_sensor_hostinfo;
static wiced_bool_t is_connected = FALSE;
static wiced_bool_t is_recieved_ready_from_apk = FALSE;
static uint8_t hello_sensor_write;
char wrt_blink_msg[520];

attribute_t gatt_user_attributes[] = {
    {HANDLE_HSENS_GAP_SERVICE_CHAR_DEV_NAME_VAL, sizeof(hello_sensor_device_name), hello_sensor_device_name},
    {HANDLE_HSENS_GAP_SERVICE_CHAR_DEV_APPEARANCE_VAL, sizeof(hello_sensor_appearance_name), hello_sensor_appearance_name},
    {HANDLE_HSENS_SERVICE_CHAR_NOTIFY_VAL, sizeof(hello_sensor_char_notify_value), hello_sensor_char_notify_value},
    {HANDLE_HSENS_SERVICE_CHAR_CFG_DESC, 2, (void *)&hello_sensor_hostinfo.characteristic_client_configuration},
    {HANDLE_HSENS_SERVICE_CHAR_BLINK_VAL, 1, &hello_sensor_hostinfo.number_of_blinks},
    {HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_MFR_NAME_VAL, sizeof(hello_sensor_char_mfr_name_value), hello_sensor_char_mfr_name_value},
    {HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_MODEL_NUM_VAL, sizeof(hello_sensor_char_model_num_value), hello_sensor_char_model_num_value},
    {HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_SYSTEM_ID_VAL, sizeof(hello_sensor_char_system_id_value), hello_sensor_char_system_id_value},
    {HANDLE_HSENS_BATTERY_SERVICE_CHAR_LEVEL_VAL, 1, &hello_sensor_state.battery_level},
};

static wiced_bt_device_link_keys_t g_ble_dev_link_keys;
/******************************************************************************
 *                                GATT DATABASE
 ******************************************************************************/
/*
 * This is the GATT database for the Hello Sensor application.  It defines
 * services, characteristics and descriptors supported by the sensor.  Each
 * attribute in the database has a handle, (characteristic has two, one for
 * characteristic itself, another for the value).  The handles are used by
 * the peer to access attributes, and can be used locally by application for
 * example to retrieve data written by the peer.  Definition of characteristics
 * and descriptors has GATT Properties (read, write, notify...) but also has
 * permissions which identify if and how peer is allowed to read or write
 * into it.  All handles do not need to be sequential, but need to be in
 * ascending order.
 */
const uint8_t hello_sensor_gatt_database[] = {
    /* Declare mandatory GATT service */
    PRIMARY_SERVICE_UUID16(HANDLE_HSENS_GATT_SERVICE, UUID_SERVCLASS_GATT_SERVER),

    /* Declare mandatory GAP service. Device Name and Appearance are mandatory
     * characteristics of GAP service                                        */
    PRIMARY_SERVICE_UUID16(HANDLE_HSENS_GAP_SERVICE, UUID_SERVCLASS_GAP_SERVER),

    /* Declare mandatory GAP service characteristic: Dev Name */
    CHARACTERISTIC_UUID16(HANDLE_HSENS_GAP_SERVICE_CHAR_DEV_NAME,
                          HANDLE_HSENS_GAP_SERVICE_CHAR_DEV_NAME_VAL,
                          GATT_UUID_GAP_DEVICE_NAME,
                          LEGATTDB_CHAR_PROP_READ,
                          LEGATTDB_PERM_READABLE),

    /* Declare mandatory GAP service characteristic: Appearance */
    CHARACTERISTIC_UUID16(HANDLE_HSENS_GAP_SERVICE_CHAR_DEV_APPEARANCE,
                          HANDLE_HSENS_GAP_SERVICE_CHAR_DEV_APPEARANCE_VAL,
                          GATT_UUID_GAP_ICON,
                          LEGATTDB_CHAR_PROP_READ,
                          LEGATTDB_PERM_READABLE),

    /* Declare proprietary Hello Service with 128 byte UUID */
    PRIMARY_SERVICE_UUID128(HANDLE_HSENS_SERVICE, UUID_SERVICE_WIRELESS_UART),

    /* Declare characteristic Hello Configuration */
    CHARACTERISTIC_UUID128_WRITABLE(HANDLE_HSENS_SERVICE_CHAR_BLINK,
                                    HANDLE_HSENS_SERVICE_CHAR_BLINK_VAL,
                                    UUID_UART_STREAM_TX,
                                    LEGATTDB_CHAR_PROP_WRITE_NO_RESPONSE,
                                    LEGATTDB_PERM_WRITABLE),

    /* Declare characteristic used to notify/indicate change */
    CHARACTERISTIC_UUID128(HANDLE_HSENS_SERVICE_CHAR_NOTIFY,
                           HANDLE_HSENS_SERVICE_CHAR_NOTIFY_VAL,
                           UUID_UART_STREAM_RX,
                           LEGATTDB_CHAR_PROP_NOTIFY,
                           LEGATTDB_PERM_READABLE),

    CHAR_DESCRIPTOR_UUID16(HANDLE_HSENS_SERVICE_CHAR_CFG_DESC,
                           GATT_UUID_CHAR_CLIENT_CONFIG,
                           LEGATTDB_PERM_READABLE),

#if 0
    /* Declare proprietary Hello Service with 128 byte UUID */
    PRIMARY_SERVICE_UUID128(HANDLE_HSENS_SERVICE, UUID_HELLO_SERVICE),

    /* Declare characteristic used to notify/indicate change */
    CHARACTERISTIC_UUID128(HANDLE_HSENS_SERVICE_CHAR_NOTIFY,
                           HANDLE_HSENS_SERVICE_CHAR_NOTIFY_VAL,
                           UUID_HELLO_CHARACTERISTIC_NOTIFY,
                           LEGATTDB_CHAR_PROP_READ | LEGATTDB_CHAR_PROP_NOTIFY | LEGATTDB_CHAR_PROP_INDICATE,
                           LEGATTDB_PERM_READABLE),

    /* Declare client characteristic configuration descriptor
     * Value of the descriptor can be modified by the client
     * Value modified shall be retained during connection and across connection
     * for bonded devices.  Setting value to 1 tells this application to
     * send notification when value of the characteristic changes.
     * Value 2 is to allow indications. */
    CHAR_DESCRIPTOR_UUID16_WRITABLE(HANDLE_HSENS_SERVICE_CHAR_CFG_DESC,
                                    GATT_UUID_CHAR_CLIENT_CONFIG,
                                    LEGATTDB_PERM_READABLE | LEGATTDB_PERM_WRITE_REQ),

    /* Declare characteristic Hello Configuration */
    CHARACTERISTIC_UUID128_WRITABLE(HANDLE_HSENS_SERVICE_CHAR_BLINK,
                                    HANDLE_HSENS_SERVICE_CHAR_BLINK_VAL,
                                    UUID_HELLO_CHARACTERISTIC_CONFIG,
                                    LEGATTDB_CHAR_PROP_READ | LEGATTDB_CHAR_PROP_WRITE,
                                    LEGATTDB_PERM_READABLE | LEGATTDB_PERM_WRITE_CMD | LEGATTDB_PERM_WRITE_REQ),
#endif

    /* Declare Device info service */
    PRIMARY_SERVICE_UUID16(HANDLE_HSENS_DEV_INFO_SERVICE, UUID_SERVCLASS_DEVICE_INFO),

    /* Handle 0x4e: characteristic Manufacturer Name, handle 0x4f
     * characteristic value */
    CHARACTERISTIC_UUID16(HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_MFR_NAME,
                          HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_MFR_NAME_VAL,
                          GATT_UUID_MANU_NAME,
                          LEGATTDB_CHAR_PROP_READ,
                          LEGATTDB_PERM_READABLE),

    /* Handle 0x50: characteristic Model Number, handle 0x51 characteristic
     * value */
    CHARACTERISTIC_UUID16(HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_MODEL_NUM,
                          HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_MODEL_NUM_VAL,
                          GATT_UUID_MODEL_NUMBER_STR,
                          LEGATTDB_CHAR_PROP_READ,
                          LEGATTDB_PERM_READABLE),

    /* Handle 0x52: characteristic System ID,handle 0x53 characteristic value */
    CHARACTERISTIC_UUID16(HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_SYSTEM_ID,
                          HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_SYSTEM_ID_VAL,
                          GATT_UUID_SYSTEM_ID,
                          LEGATTDB_CHAR_PROP_READ,
                          LEGATTDB_PERM_READABLE),

    /* Declare Battery service */
    PRIMARY_SERVICE_UUID16(HANDLE_HSENS_BATTERY_SERVICE, UUID_SERVCLASS_BATTERY),

    /* Handle 0x62: characteristic Battery Level, handle 0x63 characteristic
     * value */
    CHARACTERISTIC_UUID16(HANDLE_HSENS_BATTERY_SERVICE_CHAR_LEVEL,
                          HANDLE_HSENS_BATTERY_SERVICE_CHAR_LEVEL_VAL,
                          GATT_UUID_BATTERY_LEVEL,
                          LEGATTDB_CHAR_PROP_READ,
                          LEGATTDB_PERM_READABLE),
};

static int mtu_size = 23;
/******************************************************************************
 *                          Function Definitions
 ******************************************************************************/
/*
 *  Entry point to the application. Set device configuration and start BT
 *  stack initialization.  The actual application initialization will happen
 *  when stack reports that BT device is ready.
 */
void hello_sensor_start(void)
{
//  wiced_core_init();

    wiced_result_t ret;

    /* Register call back and configuration with stack */
    ret = wiced_bt_stack_init(hello_sensor_management_callback, &wiced_bt_cfg_settings, wiced_bt_cfg_buf_pools);
//    WPRINT_BT_APP_INFO(("Hello Sensor Start\n"));

    UNUSED_VARIABLE(ret);
}

/* To take the notify command from the command line
 * To view the notify messages, notify option should be
 * checked in the LE Explorer and indicate option should
 * be unchecked.
 */
int hello_sensor_send_notification(int argc, char *argv[])
{
    if (is_connected)
    {
        WPRINT_BT_APP_INFO(("Received command: %s\n", argv[0]));
        /* Rolling Over the ASCII value back to '0' */
        if (hello_sensor_char_notify_value[5] >= MAX_ASCII_VALUE)
        {
            hello_sensor_char_notify_value[5] = ROLL_OVER_ASCII_VALUE;
        }
        /* incrementing the value field , to ensure that notification values
         * are changing*/
        hello_sensor_char_notify_value[5]++;
        hello_sensor_send_message();
        return ERR_CMD_OK;
    }
    else
    {
        WPRINT_BT_APP_INFO(("Cannot receive, hello sensor not connected\n"));
        return ERR_CMD_OK;
    }
}

/* To take indication command from the command line.
 * To view the indication messages, Indicate option should
 * be checked in the LE Explorer and notify option should be
 * unchecked.
 */

int hello_sensor_send_indication(int argc, char *argv[])
{
    if (is_connected)
    {
        WPRINT_BT_APP_INFO(("Received command: %s\n", argv[0]));
        hello_sensor_state.flag_indication_sent = FALSE;
        /* Rolling Over the ASCII value back to '0' */
        if (hello_sensor_char_indicate_value[8] >= MAX_ASCII_VALUE)
        {
            hello_sensor_char_indicate_value[8] = ROLL_OVER_ASCII_VALUE;
        }
        hello_sensor_char_indicate_value[8]++;
        hello_sensor_send_message();
        return ERR_CMD_OK;
    }
    else
    {
        WPRINT_BT_APP_INFO(("Cannot receive, hello sensor not connected\n"));
        return ERR_CMD_OK;
    }
}

/* To take adv_power from the command line and set the adv_tx_power */
int hello_sensor_set_adv_tx_power(int argc, char *argv[])
{
    wiced_result_t result;
    int power_val;
    WPRINT_BT_APP_INFO(("hello_sensor_set_adv_tx_power\n"));
    if (!is_connected)
    {
        power_val = atoi(argv[1]);
        WPRINT_BT_APP_INFO(("\nReceived input power_val: %d\n", power_val));
        if (power_val >= -28 && power_val <= 4)
        {
            // stop adv
            result = wiced_bt_start_advertisements(BTM_BLE_ADVERT_OFF, 0, NULL);
            // set tx power
            result = wiced_bt_ble_set_adv_tx_power(power_val);
            // start adv
            result = wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL);
            WPRINT_BT_APP_INFO(("wiced_bt_start_advertisements %d\n", result));
        }
        else
        {
            WPRINT_APP_INFO(("TX power can be set only between range -28db to +4db \n"));
        }
        return ERR_CMD_OK;
    }
    else
    {
        WPRINT_BT_APP_INFO(("Cannot change ADV tx power, hello sensor already connected\n"));
        return ERR_CMD_OK;
    }
}

/* To read the adv_tx_power from the command line */
int hello_sensor_read_adv_tx_power(int argc, char *argv[])
{
    if (!is_connected)
    {
        // read adv tx power
        wiced_bt_ble_read_adv_tx_power((wiced_bt_dev_cmpl_cback_t *)hello_sensor_tx_power_callback);
        return ERR_CMD_OK;
    }
    else
    {
        WPRINT_BT_APP_INFO(("Error in reading ADV TX power\n"));
        return ERR_CMD_OK;
    }
}

int hello_sensor_read_rssi(int argc, char *argv[])
{
    if (is_connected)
    {
        wiced_bt_dev_read_rssi(hello_sensor_state.remote_addr, BT_TRANSPORT_LE,
                               (wiced_bt_dev_cmpl_cback_t *)hello_sensor_read_rssi_callback);
        return ERR_CMD_OK;
    }
    else
    {
        WPRINT_BT_APP_INFO(("Error in reading RSSI value\n"));
        return ERR_UNKNOWN;
    }
}

static void hello_sensor_tx_power_callback(wiced_bt_tx_power_result_t *p_tx_power)
{
    if ((p_tx_power->status == WICED_BT_SUCCESS) && (p_tx_power->hci_status == HCI_SUCCESS))
    {
        WPRINT_BT_APP_INFO(("ADV TX power: %i\n", p_tx_power->tx_power));
    }
    else
    {
        WPRINT_BT_APP_INFO(("Unable to read ADV TX power.(btm_status=0x%x, hci_status=0x%x)\n", p_tx_power->status,
                            p_tx_power->hci_status));
    }
}

static void hello_sensor_read_rssi_callback(wiced_bt_dev_rssi_result_t *p_rssi)
{
    if ((p_rssi->status == WICED_BT_SUCCESS) && (p_rssi->hci_status == HCI_SUCCESS))
    {
        WPRINT_BT_APP_INFO(("RSSI value: %d\n", p_rssi->rssi));
    }
    else
    {
        WPRINT_BT_APP_INFO(
            ("Unable to read RSSI value.(btm_status=0x%x, hci_status=0x%x)\n", p_rssi->status, p_rssi->hci_status));
    }
}

/*
 * This function is executed in the BTM_ENABLED_EVT management callback.
 */
static void hello_sensor_application_init(void)
{
    wiced_bt_gatt_status_t gatt_status;
    wiced_result_t result;

    WPRINT_BT_APP_INFO(("hello_sensor_application_init\n"));

    /* Register with stack to receive GATT callback */
    gatt_status = wiced_bt_gatt_register(hello_sensor_gatts_callback);

    WPRINT_BT_APP_INFO(("\nwiced_bt_gatt_register: %d\n", gatt_status));

    /*  Tell stack to use our GATT database */
    gatt_status = wiced_bt_gatt_db_init(hello_sensor_gatt_database, sizeof(hello_sensor_gatt_database));

    WPRINT_BT_APP_INFO(("wiced_bt_gatt_db_init %d\n", gatt_status));

    /* Set the advertising parameters and make the device discoverable */
    hello_sensor_set_advertisement_data();
    hello_sensor_set_scan_response_data();

    /* Load the address resolution DB with the keys stored in the DCT */
    result = ble_hello_sensor_load_key_to_addr_resolution_db();

    result = wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL);

    WPRINT_BT_APP_INFO(("wiced_bt_start_advertisements %d\n", result));

    if( result == 0 )
    {
        LOGD("[BLE]:%s started.\r\n", wiced_bt_cfg_settings.device_name);
    }
    /*
     * Set flag_stay_connected to remain connected after all messages are sent
     * Reset flag to 0, to disconnect
     */
    hello_sensor_state.flag_stay_connected = 1;

    /* app_powersave_sleep() needs to be called here only in case of Embedded mode.
     * In case of host mode this will be called when BTM_LPM_STATE_LOW_POWER event is received from stack */
#ifdef USE_WICED_HCI
    app_powersave_sleep();
#endif
}

/*
 * Setup advertisement data with 16 byte UUID and device name
 */
static void hello_sensor_set_advertisement_data(void)
{
    wiced_result_t result;
    wiced_bt_ble_advert_elem_t adv_elem[3];
    uint8_t ble_advertisement_flag_value = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;
    uint8_t num_elem                     = 0;

    adv_elem[num_elem].advert_type = BTM_BLE_ADVERT_TYPE_FLAG;
    adv_elem[num_elem].len         = 1;
    adv_elem[num_elem].p_data      = &ble_advertisement_flag_value;
    num_elem++;

    adv_elem[num_elem].advert_type = BTM_BLE_ADVERT_TYPE_NAME_SHORT;
    adv_elem[num_elem].len         = strlen((const char *)wiced_bt_cfg_settings.device_name);
    WPRINT_BT_APP_INFO(("wiced_bt_cfg_settings.device_name:%s\n", wiced_bt_cfg_settings.device_name));
    adv_elem[num_elem].p_data = (uint8_t *)wiced_bt_cfg_settings.device_name;

    num_elem++;

    result = wiced_bt_ble_set_raw_advertisement_data(num_elem, adv_elem);

    WPRINT_BT_APP_INFO(("wiced_bt_ble_set_advertisement_data %d\n", result));
}

/*
 * Setup scan response data with 16 byte UUID and device name
 */
static void hello_sensor_set_scan_response_data(void)
{
    wiced_result_t result;
    wiced_bt_ble_advert_elem_t adv_elem[1];
    uint8_t num_elem                         = 0;
    uint8_t hello_service_uuid[LEN_UUID_128] = {UUID_SERVICE_WIRELESS_UART};

    adv_elem[num_elem].advert_type = BTM_BLE_ADVERT_TYPE_128SRV_COMPLETE;
    adv_elem[num_elem].len         = LEN_UUID_128;
    adv_elem[num_elem].p_data      = hello_service_uuid;
    num_elem++;

    result = wiced_bt_ble_set_raw_scan_response_data(num_elem, adv_elem);

    WPRINT_BT_APP_INFO(("wiced_bt_ble_set_scan_response_data %d\n", result));
}

/*
 * This function is invoked when advertisements stop.  If we are configured to stay connected,
 * disconnection was caused by the peer, start low advertisements, so that peer can connect
 * when it wakes up
 */
void hello_sensor_advertisement_stopped(void)
{
    wiced_result_t result;

    if (hello_sensor_state.flag_stay_connected && !hello_sensor_state.conn_id)
    {
        result = wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_LOW, 0, NULL);
        WPRINT_BT_APP_INFO(("wiced_bt_start_advertisements: %d\n", result));
    }
    else
    {
        WPRINT_BT_APP_INFO(("advertisement stop\n"));
    }
}

/*
 * hello_sensor bt/ble link management callback
 */
static wiced_result_t hello_sensor_management_callback(wiced_bt_management_evt_t event,
                                                       wiced_bt_management_evt_data_t *p_event_data)
{
    wiced_result_t result = WICED_BT_SUCCESS;
    wiced_bt_dev_ble_pairing_info_t *p_info;
    wiced_bt_ble_advert_mode_t *p_mode;
    // wiced_bool_t                   is_warmboot; // = WICED_DEEP_SLEEP_IS_WARMBOOT( );

    // WPRINT_BT_APP_INFO(("hello_sensor_management_callback: %x\n", event ));

    switch (event)
    {
        /* Bluetooth  stack enabled */
        case BTM_ENABLED_EVT:
            hello_sensor_application_init();

#ifdef ENABLE_APP_POWERSAVE
            /* Break early as initializing command console and hence STDIO_UART increases the power numbers */
            break;
#endif
            break;

        case BTM_DISABLED_EVT:
            break;

        case BTM_PAIRING_IO_CAPABILITIES_BLE_REQUEST_EVT:
            p_event_data->pairing_io_capabilities_ble_request.local_io_cap = BTM_IO_CAPABILITIES_NONE;
            p_event_data->pairing_io_capabilities_ble_request.oob_data     = BTM_OOB_NONE;
            p_event_data->pairing_io_capabilities_ble_request.auth_req = BTM_LE_AUTH_REQ_BOND | BTM_LE_AUTH_REQ_MITM;
            p_event_data->pairing_io_capabilities_ble_request.max_key_size = 0x10;
            p_event_data->pairing_io_capabilities_ble_request.init_keys    = BTM_LE_KEY_PENC | BTM_LE_KEY_PID;
            p_event_data->pairing_io_capabilities_ble_request.resp_keys    = BTM_LE_KEY_PENC | BTM_LE_KEY_PID;
            break;

        case BTM_PAIRING_COMPLETE_EVT:
            p_info = &p_event_data->pairing_complete.pairing_complete_info.ble;
            WPRINT_BT_APP_INFO(("Pairing Complete: %d\n", p_info->reason));
            break;

        case BTM_SECURITY_REQUEST_EVT:
            WPRINT_BT_APP_INFO(("BTM Security Event\n"));
            wiced_bt_ble_security_grant(p_event_data->security_request.bd_addr, WICED_BT_SUCCESS);
            break;

        case BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT:
        {
//            WPRINT_APP_INFO(("Key Upd\n"));
//            /******
//            WPRINT_APP_INFO((" Paired Device Link Keys Update Event\n"));
//            WPRINT_APP_INFO(("GATT connection to [%02X:%02X:%02X:%02X:%02X:%02X]\n",
//                        p_event_data->paired_device_link_keys_request.bd_addr[0],
//                        p_event_data->paired_device_link_keys_request.bd_addr[1],
//                        p_event_data->paired_device_link_keys_request.bd_addr[2],
//                        p_event_data->paired_device_link_keys_request.bd_addr[3],
//                        p_event_data->paired_device_link_keys_request.bd_addr[4],
//                        p_event_data->paired_device_link_keys_request.bd_addr[5]));
//            ********/
//            ble_hello_sensor_save_link_key(p_event_data->paired_device_link_keys_request);
            break;
        }

        case BTM_PAIRED_DEVICE_LINK_KEYS_REQUEST_EVT:
        {
            WPRINT_APP_INFO(("Paired Device Link Keys Request Event\n"));
            ble_hello_sensor_read_link_key(&p_event_data->paired_device_link_keys_request);
            WPRINT_APP_INFO(("Key retrieval success\n"));

            break;
        }

        case BTM_BLE_ADVERT_STATE_CHANGED_EVT:
            p_mode = &p_event_data->ble_advert_state_changed;
            WPRINT_BT_APP_INFO(("Advertisement State Change: %d\n", *p_mode));
            if (*p_mode == BTM_BLE_ADVERT_OFF)
            {
                hello_sensor_advertisement_stopped();
            }
            break;

        case BTM_LPM_STATE_LOW_POWER:

#ifdef ENABLE_APP_POWERSAVE
            app_powersave_sleep();
#endif
            break;

        default:
            break;
    }

    return result;
}

/*
 * Check if client has registered for notification/indication
 * and send message if appropriate
 */
static void hello_sensor_send_message(void)
{
    WPRINT_BT_APP_INFO(("%s: Client's Characteristic configuration:%d\n", __func__,
                        hello_sensor_hostinfo.characteristic_client_configuration));

    /* If client has not registered for indication or notification, no action */
    if (hello_sensor_hostinfo.characteristic_client_configuration == 0)
    {
        return;
    }
    else if (hello_sensor_hostinfo.characteristic_client_configuration & GATT_CLIENT_CONFIG_NOTIFICATION)
    {
        uint8_t *p_attr = (uint8_t *)hello_sensor_char_notify_value;
        wiced_bt_gatt_send_notification(hello_sensor_state.conn_id, HANDLE_HSENS_SERVICE_CHAR_NOTIFY_VAL,
                                        sizeof(hello_sensor_char_notify_value), p_attr);
    }
    else
    {
        if (!hello_sensor_state.flag_indication_sent)
        {
            WPRINT_BT_APP_INFO((" Indication message\n"));
            uint8_t *p_attr                         = (uint8_t *)hello_sensor_char_indicate_value;
            hello_sensor_state.flag_indication_sent = TRUE;

            wiced_bt_gatt_send_indication(hello_sensor_state.conn_id, HANDLE_HSENS_SERVICE_CHAR_NOTIFY_VAL,
                                          sizeof(hello_sensor_char_indicate_value), p_attr);
        }
    }
}

static void wiced_bt_send_message(const uint8_t *attr, int len)
{
    wiced_bt_gatt_status_t res_gatt = WICED_BT_GATT_SUCCESS;
    int retry = 5;

    WPRINT_BT_APP_INFO(("%s: Client's Characteristic configuration:%d\n", __func__,
                        hello_sensor_hostinfo.characteristic_client_configuration));

    /* If client has not registered for indication or notification, no action */
    if (hello_sensor_hostinfo.characteristic_client_configuration == 0)
    {
        return;
    }
    else if (hello_sensor_hostinfo.characteristic_client_configuration & GATT_CLIENT_CONFIG_NOTIFICATION)
    {
        uint8_t *p_attr = (uint8_t *)attr;
        do {
            res_gatt = wiced_bt_gatt_send_notification(hello_sensor_state.conn_id, HANDLE_HSENS_SERVICE_CHAR_NOTIFY_VAL, len, p_attr);
            if(res_gatt != WICED_BT_GATT_SUCCESS)
            {
                WPRINT_BT_APP_INFO(("send notification error %d.\r\n", res_gatt));
                vTaskDelay(pdMS_TO_TICKS(150));
            }
            if(res_gatt == WICED_BT_GATT_CONGESTED)
            {
                break;
            }
            else
            {
                vTaskDelay(pdMS_TO_TICKS(250));
            }

            if(--retry <= 0)
            {
                break;
            }

        } while(res_gatt != WICED_BT_GATT_SUCCESS);
    }
    else
    {
        if (!hello_sensor_state.flag_indication_sent)
        {
            WPRINT_BT_APP_INFO((" Indication message\n"));
            uint8_t *p_attr                         = (uint8_t *)attr;
            hello_sensor_state.flag_indication_sent = TRUE;

            wiced_bt_gatt_send_indication(hello_sensor_state.conn_id, HANDLE_HSENS_SERVICE_CHAR_NOTIFY_VAL,
                                        strlen((char *)p_attr), p_attr);
        }
    }
}

/*
 * Find attribute description by handle
 */
static attribute_t *hello_sensor_get_attribute(uint16_t handle)
{
    int i;
    for (i = 0; i < sizeof(gatt_user_attributes) / sizeof(gatt_user_attributes[0]); i++)
    {
        if (gatt_user_attributes[i].handle == handle)
        {
            return (&gatt_user_attributes[i]);
        }
    }
    WPRINT_BT_APP_INFO(("attribute not found:%x\n", handle));
    return NULL;
}

/*
 * Process Read request or command from peer device
 */
static wiced_bt_gatt_status_t hello_sensor_gatt_server_read_request_handler(uint16_t conn_id,
                                                                            wiced_bt_gatt_read_t *p_read_data)
{
    attribute_t *puAttribute;
    int attr_len_to_copy;

    if ((puAttribute = hello_sensor_get_attribute(p_read_data->handle)) == NULL)
    {
        WPRINT_BT_APP_INFO(("read_hndlr attr not found hdl:%x\n", p_read_data->handle));
        return WICED_BT_GATT_INVALID_HANDLE;
    }

    /* Dummy battery value read increment */
    if (p_read_data->handle == HANDLE_HSENS_BATTERY_SERVICE_CHAR_LEVEL_VAL)
    {
        if (hello_sensor_state.battery_level++ > 99)
        {
            hello_sensor_state.battery_level = 0;
        }
    }

    if (p_read_data->handle == HANDLE_HSENS_SERVICE_CHAR_BLINK_VAL)
    {
        puAttribute->p_attr   = &hello_sensor_write;
        puAttribute->attr_len = sizeof(hello_sensor_write);
    }
    attr_len_to_copy = puAttribute->attr_len;

    // WPRINT_BT_APP_INFO(("read_hndlr conn_id:%d hdl:%x offset:%d len:%d\n", conn_id, p_read_data->handle,
    // p_read_data->offset, attr_len_to_copy));

    if (p_read_data->offset >= puAttribute->attr_len)
    {
        attr_len_to_copy = 0;
    }

    if (attr_len_to_copy != 0)
    {
        uint8_t *from;
        int to_copy = attr_len_to_copy - p_read_data->offset;

        if (to_copy > *p_read_data->p_val_len)
        {
            to_copy = *p_read_data->p_val_len;
        }

        from                    = ((uint8_t *)puAttribute->p_attr) + p_read_data->offset;
        *p_read_data->p_val_len = to_copy;

        memcpy(p_read_data->p_val, from, to_copy);
    }

    return WICED_BT_GATT_SUCCESS;
}

/*
 * Process write request or write command from peer device
 */
static wiced_bt_gatt_status_t hello_sensor_gatt_server_write_request_handler(uint16_t conn_id,
                                                                             wiced_bt_gatt_write_t *p_data)
{
    wiced_bt_gatt_status_t result = WICED_BT_GATT_SUCCESS;
    uint8_t *p_attr               = p_data->p_val;

    // WPRINT_BT_APP_INFO(("write_handler: conn_id:%d hdl:0x%x prep:%d offset:%d len:%d\n ", conn_id, p_data->handle,
    // p_data->is_prep, p_data->offset, p_data->val_len));

    switch (p_data->handle)
    {
        /* By writing into Characteristic Client Configuration descriptor
         * peer can enable or disable notification or indication */
        case HANDLE_HSENS_SERVICE_CHAR_CFG_DESC:
            if (p_data->val_len != 2)
            {
                return WICED_BT_GATT_INVALID_ATTR_LEN;
            }
            hello_sensor_hostinfo.characteristic_client_configuration = p_attr[0] | (p_attr[1] << 8);
            break;

        case HANDLE_HSENS_SERVICE_CHAR_BLINK_VAL:
        {
            uint8_t attribute_value = *(uint8_t *)p_data->p_val;
            if (p_data->val_len <= 0)
            {
                return WICED_BT_GATT_INVALID_ATTR_LEN;
            }
//            hello_sensor_hostinfo.number_of_blinks = p_attr[0];
//            if (hello_sensor_hostinfo.number_of_blinks != 0)
//            {
//                WPRINT_BT_APP_INFO(
//                    ("hello_sensor_write_handler:num blinks: %d\n", hello_sensor_hostinfo.number_of_blinks));
//            }
            hello_sensor_write = attribute_value;
//            WPRINT_BT_APP_INFO(("The value written is: %i\n", attribute_value));
            WPRINT_BT_APP_INFO(("packet received length: %d\n", p_data->val_len));
            p_attr[p_data->val_len] = '\0';
            WPRINT_BT_APP_INFO(("%s\r\n", p_attr));

            //TODO: BLE packet callback point.
            if ((xQueueSend(g_recv_data_queue, (char*)p_attr, portMAX_DELAY)) != pdPASS ) {
                LOGE("Cannot send data to queue\r\n");
            }
            break;
        }

        default:
            result = WICED_BT_GATT_INVALID_HANDLE;
            break;
    }
    return result;
}

/*
 * Write Execute Procedure
 */
static wiced_bt_gatt_status_t hello_sensor_gatt_server_write_and_execute_request_handler(
    uint16_t conn_id, wiced_bt_gatt_exec_flag_t exec_flag)
{
    WPRINT_BT_APP_INFO(("write exec: flag:%d\n", exec_flag));
    return WICED_BT_GATT_SUCCESS;
}

/*
 * Process MTU request from the peer
 */
static wiced_bt_gatt_status_t hello_sensor_gatt_server_mtu_request_handler(uint16_t conn_id, uint16_t mtu)
{
    mtu_size = mtu;
    WPRINT_BT_APP_INFO(("req_mtu: %d\n", mtu));
    return WICED_BT_GATT_SUCCESS;
}

static wiced_bt_gatt_status_t hello_sensor_gatt_server_confirmation_handler(uint16_t conn_id, uint16_t handle)
{
    WPRINT_BT_APP_INFO(("hello_sensor_indication_confirmation, conn %d hdl %d\n", conn_id, handle));
    if (!hello_sensor_state.flag_indication_sent)
    {
        WPRINT_BT_APP_INFO(("Hello: Wrong Confirmation!"));
        return WICED_BT_GATT_SUCCESS;
    }
    hello_sensor_state.flag_indication_sent = 0;
    // hello_sensor_send_message();
    return WICED_BT_GATT_SUCCESS;
}

/* This function is invoked when connection is established */
static wiced_bt_gatt_status_t hello_sensor_gatts_connection_up(wiced_bt_gatt_connection_status_t *p_status)
{
    wiced_result_t result;

    WPRINT_BT_APP_INFO(("gatt connect up  id:%d\n:", p_status->conn_id));

    /* Update the connection handler.  Save address of the connected device. */
    hello_sensor_state.conn_id = p_status->conn_id;
    memcpy(hello_sensor_state.remote_addr, p_status->bd_addr, sizeof(BD_ADDR));

    /* Stop advertising */
    result = wiced_bt_start_advertisements(BTM_BLE_ADVERT_OFF, 0, NULL);

    WPRINT_BT_APP_INFO(("stopping advertisements%d\n", result));

#if 1
    // Initiating the security. If call to the Bond returns success, device is bonded, and we just need to setup
    // encryption
    if ((result = wiced_bt_dev_sec_bond(p_status->bd_addr, p_status->addr_type, BT_TRANSPORT_LE, 0, NULL)) ==
        WICED_BT_SUCCESS)
    {
        WPRINT_APP_INFO(("wifi_introducer starting encryption\n"));
        wiced_bt_dev_set_encryption(p_status->bd_addr, BT_TRANSPORT_LE, NULL);
    }
    else
    {
        // WICED_BT_TRACE( "wiced_bt_dev_sec_bond %d \n", result );
        /* Updating the bd address in the  host info in NVRAM  */
        memcpy(hello_sensor_hostinfo.bdaddr, p_status->bd_addr, sizeof(BD_ADDR));
    }
#endif

    memcpy(hello_sensor_hostinfo.bdaddr, p_status->bd_addr, sizeof(BD_ADDR));
    hello_sensor_hostinfo.characteristic_client_configuration = 0;
    hello_sensor_hostinfo.number_of_blinks                    = 0;

    return WICED_BT_GATT_SUCCESS;
}

/*
 * This function is invoked when connection is lost
 */
static wiced_bt_gatt_status_t hello_sensor_gatts_connection_down(wiced_bt_gatt_connection_status_t *p_status)
{
    wiced_result_t result;

    WPRINT_BT_APP_INFO(("gatt connect down conn_id:%d reason:%d\n", p_status->conn_id, p_status->reason));

    /* Resetting the device info */
    memset(hello_sensor_state.remote_addr, 0, 6);
    hello_sensor_state.conn_id          = 0;
    hello_sensor_char_notify_value[5]   = '0';
    hello_sensor_char_indicate_value[8] = '0';

    /* Resetting mtu size */
    mtu_size = 23;

    /*
     * If we are configured to stay connected, disconnection was
     * caused by the peer, start low advertisements, so that peer
     * can connect when it wakes up
     */
    if (hello_sensor_state.flag_stay_connected)
    {
        result = wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_LOW, 0, NULL);
        WPRINT_BT_APP_INFO(("wiced_bt_start_advertisements %d\n", result));
    }

    /* app_powersave_sleep() needs to be called here only in case of Embedded mode.
     * In case of host mode this will be called when BTM_LPM_STATE_LOW_POWER event is received from stack */
#ifdef USE_WICED_HCI
    app_powersave_sleep();
#endif

    return WICED_BT_SUCCESS;
}

/*
 * Connection up/down event
 */
static wiced_bt_gatt_status_t hello_sensor_gatts_connection_status_handler(wiced_bt_gatt_connection_status_t *p_status)
{
    is_connected = p_status->connected;
    if (p_status->connected)
    {
//        vTaskResume(s_HandleTaskSendReady);

        return hello_sensor_gatts_connection_up(p_status);
    }

    is_recieved_ready_from_apk = FALSE;
//    vTaskSuspend(s_HandleTaskSendReady);
    return hello_sensor_gatts_connection_down(p_status);
}

/*
 * Process GATT request from the peer
 */
static wiced_bt_gatt_status_t hello_sensor_gatt_server_request_handler(wiced_bt_gatt_attribute_request_t *p_data)
{
    wiced_bt_gatt_status_t result = WICED_BT_GATT_INVALID_PDU;

    // WPRINT_BT_APP_INFO(( "hello_sensor_gatt_server_request_handler. conn %d, type %d\n", p_data->conn_id,
    // p_data->request_type ));

    switch (p_data->request_type)
    {
        case GATTS_REQ_TYPE_READ:
//            result = hello_sensor_gatt_server_read_request_handler(p_data->conn_id, &(p_data->data.read_req));
            break;

        case GATTS_REQ_TYPE_WRITE:
            result = hello_sensor_gatt_server_write_request_handler(p_data->conn_id, &(p_data->data.write_req));
            break;

        case GATTS_REQ_TYPE_WRITE_EXEC:
            result =
                hello_sensor_gatt_server_write_and_execute_request_handler(p_data->conn_id, p_data->data.exec_write);
            break;

        case GATTS_REQ_TYPE_MTU:
            result = hello_sensor_gatt_server_mtu_request_handler(p_data->conn_id, p_data->data.mtu);
            break;

        case GATTS_REQ_TYPE_CONF:
            result = hello_sensor_gatt_server_confirmation_handler(p_data->conn_id, p_data->data.handle);
            break;

        default:
            break;
    }
    return result;
}

/*
 * Callback for various GATT events.  As this application performs only as a GATT server, some of
 * the events are ommitted.
 */
static wiced_bt_gatt_status_t hello_sensor_gatts_callback(wiced_bt_gatt_evt_t event, wiced_bt_gatt_event_data_t *p_data)
{
    wiced_bt_gatt_status_t result = WICED_BT_GATT_INVALID_PDU;

    switch (event)
    {
        case GATT_CONNECTION_STATUS_EVT:
            result = hello_sensor_gatts_connection_status_handler(&p_data->connection_status);
            break;

        case GATT_ATTRIBUTE_REQUEST_EVT:
            result = hello_sensor_gatt_server_request_handler(&p_data->attribute_request);
            break;

        default:
            break;
    }

    return result;
}

static int get_current_mtu_size(void)
{
    return mtu_size;
}

#if DEBUG_POWERSAVE
static void application_sleep_event_handler(wiced_sleep_event_type_t event, int handler)
{
    switch (event)
    {
        case WICED_SLEEP_EVENT_ENTER:
            WPRINT_BT_APP_INFO(("ble_hello_sensor : App Entering  Low power mode \n"));
            break;

        case WICED_SLEEP_EVENT_CANCEL:
            WPRINT_BT_APP_INFO(("ble_hello_sensor : App Cancelling Low power mode \n"));
            break;

        case WICED_SLEEP_EVENT_LEAVE:
            WPRINT_BT_APP_INFO(("ble_hello_sensor : App Leaving Low power mode \n"));
            break;

        default:
            break;
    }
}

/* Call back , which is called before entering or after leaving sleep state */
WICED_SLEEP_EVENT_HANDLER(application_sleep_event_handler_1)
{
    application_sleep_event_handler(event, 0);
}

#endif

void ble_hello_sensor_save_link_key(wiced_bt_device_link_keys_t paired_device_keys)
{
    // WPRINT_BT_APP_INFO( ("ble_hello_sensor_save_link_key\n"));
    memcpy(&g_ble_dev_link_keys, &paired_device_keys, sizeof(wiced_bt_device_link_keys_t));

    // Save keys to persistant storage
#ifdef SIMULATE_NVRAM_WITH_ITC_RAM
    nvm_save_keys(&g_ble_dev_link_keys, sizeof(wiced_bt_device_link_keys_t));
#else
#if 0
    // Save keys to persistant storage
    platform_nvm_t *nvm = platform_get_nvm();

    nvm->platform_nvm_erase(BLE_KEY_FLASH_ADDR);
    nvm->platform_nvm_save(BLE_KEY_FLASH_ADDR, (uint8_t *)&g_ble_dev_link_keys, sizeof(wiced_bt_device_link_keys_t));
#endif
#endif
}

void ble_hello_sensor_read_link_key(wiced_bt_device_link_keys_t *paired_device_keys)
{
    // WPRINT_APP_INFO( ("ble_hello_sensor_read_link_keys\n"));
    memcpy(&paired_device_keys->key_data, &g_ble_dev_link_keys.key_data, sizeof(paired_device_keys->key_data));
}

wiced_result_t ble_hello_sensor_load_key_to_addr_resolution_db(void)
{
    // TBD: Copy from persistant storage to global keys structure first
#ifdef SIMULATE_NVRAM_WITH_ITC_RAM
    nvm_load_keys(&g_ble_dev_link_keys, sizeof(wiced_bt_device_link_keys_t));
#else
    // Copy from persistant storage to global keys structure first

#if 0
    platform_nvm_t *nvm = platform_get_nvm();

    nvm->platform_nvm_read(BLE_KEY_FLASH_ADDR, (uint8_t *)&g_ble_dev_link_keys, sizeof(wiced_bt_device_link_keys_t));
#endif
#endif

    return wiced_bt_dev_add_device_to_address_resolution_db(&g_ble_dev_link_keys);
}

#ifdef SIMULATE_NVRAM_WITH_ITC_RAM
/*
 *  Non Volatile Memory (NVM) routines for testing.
 *  These test NVM routes uses unsed SRAM_ITC RAM as NVM.
 *  On plain reset (not power reset) the contents of SRAM_ITC are retained.
 *  So after resetting the board also connection with 'paired Android mobile'
 *  should work.
 */
#define SRAM_ITC_BUF ((void *)0x10000)

int nvm_load_keys(wiced_bt_device_link_keys_t *keys, int key_len)
{
    WPRINT_BT_APP_INFO(("#LK\n"));
    memcpy(keys, SRAM_ITC_BUF, key_len);
    return 0;
}

int nvm_save_keys(wiced_bt_device_link_keys_t *keys, int key_len)
{
    WPRINT_BT_APP_INFO(("#SK\n"));
    memcpy(SRAM_ITC_BUF, keys, key_len);
    return 0;
}
#else
int nvm_load_keys(wiced_bt_device_link_keys_t *keys, int key_len)
{
    return 0;
}

int nvm_save_keys(wiced_bt_device_link_keys_t *keys, int key_len)
{
    return 0;
}
#endif

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
            //TODO:
            Remote_FaceAdd(buffer_str + 2, len - 2);
            break;

        case OP_KEY_ADD_START:
            //TODO:
//            Remote_KeyAdd(buffer_str + 2, len - 2);
            break;

        default:
            UsbShell_Printf("[ERROR]:invalid operation\r\n");
            break;
        }
    }

    vPortFree(buffer_str);
}

static remote_frame_cmd_t parse_remote_packet(const char* packet_str, remote_frame_struct_t* frame_packet)
{
    cJSON *frame_packet_json = cJSON_Parse(packet_str);
    if ( frame_packet_json == NULL ) {
        goto end;
    }

    cJSON *frame_header_json = cJSON_GetObjectItemCaseSensitive(frame_packet_json, REMOTE_CTRL_FRAME_HEADER);
    if ( frame_header_json == NULL ) {
        goto end;
    }

    cJSON *frame_header_prefix_json = cJSON_GetObjectItemCaseSensitive(frame_header_json, REMOTE_CTRL_PREFIX);
    if( frame_header_prefix_json == NULL) {
        goto end;
    }
    memcpy(frame_packet->frame_header.prefix, frame_header_prefix_json->valuestring, strlen(frame_header_prefix_json->valuestring));

    cJSON *frame_header_timestamp_json = cJSON_GetObjectItemCaseSensitive(frame_header_json, REMOTE_CTRL_TIME_STAMP);
    if( frame_header_timestamp_json == NULL) {
        goto end;
    }
    frame_packet->frame_header.timestamp = frame_header_timestamp_json->valuedouble;

    cJSON *frame_header_version_json = cJSON_GetObjectItemCaseSensitive(frame_header_json, FH_VERSION);
    if(frame_header_version_json == NULL) {
        goto end;
    }
    frame_packet->frame_header.version = frame_header_version_json->valuedouble;

    cJSON *frame_header_cipher_json = cJSON_GetObjectItemCaseSensitive(frame_header_json, FH_FLAG);
    if(frame_header_cipher_json == NULL) {
        goto end;
    }
    frame_packet->frame_header.cipher = frame_header_cipher_json->valuedouble;

    cJSON *frame_header_length_json = cJSON_GetObjectItemCaseSensitive(frame_header_json, REMOTE_CTRL_LENGTH);
    if(frame_header_length_json == NULL) {
        goto end;
    }
    frame_packet->frame_header.length = frame_header_length_json->valuedouble;

    cJSON *frame_cmd_json = cJSON_GetObjectItemCaseSensitive(frame_packet_json, REMOTE_CTRL_COMMAND);
    if(frame_cmd_json == NULL) {
        goto end;
    }
    frame_packet->command = frame_cmd_json->valuedouble;

    cJSON *frame_payload_json = cJSON_GetObjectItemCaseSensitive(frame_packet_json, REMOTE_CTRL_PAYLOAD);
    if(frame_payload_json == NULL) {
        goto end;
    }
    char *p_payload = pvPortMalloc(strlen(frame_payload_json->valuestring)+1);
    if(p_payload == NULL)
    {
        goto end;
    }
    memcpy(p_payload, frame_payload_json->valuestring, strlen(frame_payload_json->valuestring)+1);
    frame_packet->payload =  p_payload;

    cJSON *frame_crc_json = cJSON_GetObjectItemCaseSensitive(frame_packet_json, REMOTE_CTRL_CRC);
    if(frame_crc_json == NULL) {
        goto end;
    }
    frame_packet->crc = frame_crc_json->valuedouble;

    cJSON_Delete(frame_packet_json);
    return frame_packet->command;

end:
    cJSON_Delete(frame_packet_json);
    return UNKNOW_COMMAND;
}

static char* transform_remote_pakeck(const remote_frame_struct_t frame_packet)
{
    char * string = NULL;

    cJSON *frame_packet_json = cJSON_CreateObject();
    if ( frame_packet_json == NULL ) {
        goto end;
    }

    cJSON *frame_header_json = cJSON_AddObjectToObject(frame_packet_json, REMOTE_CTRL_FRAME_HEADER);
    if(frame_header_json == NULL)
    {
        goto end;
    }

    if(cJSON_AddStringToObject(frame_header_json, REMOTE_CTRL_PREFIX, frame_packet.frame_header.prefix) == NULL)
    {
        goto end;
    }

    if(cJSON_AddNumberToObject(frame_header_json, REMOTE_CTRL_TIME_STAMP, frame_packet.frame_header.timestamp) == NULL)
    {
        goto end;
    }

    if(cJSON_AddNumberToObject(frame_header_json, FH_VERSION, frame_packet.frame_header.version) == NULL)
    {
        goto end;
    }

    if(cJSON_AddNumberToObject(frame_header_json, FH_FLAG, frame_packet.frame_header.cipher) == NULL)
    {
        goto end;
    }

    if(cJSON_AddNumberToObject(frame_header_json, REMOTE_CTRL_LENGTH, frame_packet.frame_header.length) == NULL)
    {
        goto end;
    }

    if(cJSON_AddNumberToObject(frame_packet_json, REMOTE_CTRL_COMMAND, frame_packet.command) == NULL)
    {
        goto end;
    }

    if(cJSON_AddStringToObject(frame_packet_json, REMOTE_CTRL_PAYLOAD, frame_packet.payload) == NULL)
    {
        goto end;
    }

    if(cJSON_AddNumberToObject(frame_packet_json, REMOTE_CTRL_CRC, frame_packet.crc) == NULL)
    {
        goto end;
    }

    string = cJSON_Print(frame_packet_json);

end:
    if(string == NULL)
    {
        LOGE("JSON packet error!.\r\n");
    }
    cJSON_Delete(frame_packet_json);

    return string;
}

int process_receieve_payload_packet(cJSON *head_json)
{
    uint32_t len;
    uint32_t packet_count;
    char *buffer_str, *buffer_ptr;
    remote_frame_cmd_t cmd_state;
    remote_frame_struct_t frame_packet;

    cJSON *length_json = cJSON_GetObjectItemCaseSensitive(head_json, LENGTH_STRING);
    if(length_json == NULL) {
        LOGE("frame header length parse error.\r\n");
        return -1;
    }

    len = length_json->valuedouble;
    packet_count = ( len % RECV_DATA_QUEUE_ITEM_SIZE ) ? ( (len / RECV_DATA_QUEUE_ITEM_SIZE) + 1 ) :  ( len / RECV_DATA_QUEUE_ITEM_SIZE );

    buffer_str = (char*)pvPortMalloc(sizeof(char) * (packet_count * RECV_DATA_QUEUE_ITEM_SIZE));
    if ( buffer_str == NULL ) {
        LOGE("malloc failed\r\n");
        return -1;
    }

    buffer_ptr = buffer_str;

    while ( packet_count-- ) {
        vTaskDelay(pdMS_TO_TICKS(5));

        if ( (xQueueReceive(g_recv_data_queue, buffer_ptr, portMAX_DELAY)) != pdPASS ) {
            LOGE("Cannot receive data from queue\r\n");
            continue;
        }

        buffer_ptr += RECV_DATA_QUEUE_ITEM_SIZE;
    }

    buffer_ptr = buffer_str;

    //JSON parse
    cmd_state = parse_remote_packet(buffer_ptr, &frame_packet);

    vPortFree(buffer_str);

    switch( cmd_state ) {
    //TODO: use to send ready to remote app.
        case READY_INDICATION:
//            vTaskSuspend(s_HandleTaskSendReady);
//            cJSON *payload_json = cJSON_Parse(frame_packet.payload);
//            cJSON *payload_len_json = cJSON_GetObjectItemCaseSensitive(payload_json, "max_payload");
            WicedBLE_Send_Ready();
            is_recieved_ready_from_apk = TRUE;
            LOGD("APK Ready.\r\n");
            break;

    //TODO:
        case AUTHENTICATION_REQUEST:
            break;

        case AUTHENTICATION_RESPONSE:
            break;

        case PASSWORD_REQUEST:
            break;

        case PASSWORD_RESPONSE:
            break;

        case CONFIGURATION_GET_REQUEST:
            //Remote_command_configuration_get_request(frame_packet.payload);
            //TODO: bridge CLI command with remote control
            break;

        case CONFIGURATION_GET_RESPONSE:
            break;

        case CONFIGURATION_UPDATE_REQUEST:
            break;

        case CONFIGURATION_UPDATE_RESPONSE:
            break;

        case FACE_RECORD_GET_REQUEST:
            Remote_command_face_record_get_request(frame_packet.payload);
            //manager user list send to app.
            break;

        case FACE_RECORD_GET_RESPONSE:
            break;

        case FACE_RECORD_UPDATE_REQUEST:
            //manager user update face feature.
            //add, edit, delete
            Remote_command_face_record_update_request(frame_packet.payload);
            break;

        case FACE_RECORD_UPDATE_RESPONSE:
            break;

        case OPEN_DOOR_RESPONSE:
            break;

        case OPEN_DOOR_REQUEST:
            //remote control
            break;

        default:
            LOGE("Unknown Command\r\n");
            break;
    }

    vPortFree(frame_packet.payload);

    return 0;
}

static void wiced_ble_host_task( void* args )
{
    char received_str[256];

    while (1)
    {
        if ( (xQueueReceive(g_recv_data_queue, received_str, portMAX_DELAY)) != pdPASS ) {
            LOGE("[BLE]:Cannot receive data from queue\r\n");
            continue;
        }

        cJSON *frame_hand_head_json = cJSON_Parse(received_str);
        if( frame_hand_head_json == NULL ) {
            LOGE("frame header parse error.\r\n");
            continue;
        }

        process_receieve_payload_packet(frame_hand_head_json);

        cJSON_Delete(frame_hand_head_json);

        vTaskDelay(pdMS_TO_TICKS(100));
    }

}

static void wiced_ble_init(void *arg)
{
    LOGD(("[BLE]:running"));

//    if (xTaskCreate(wiced_ble_send_ready_task, "wiced_send_ready_task", BLETASK_STACKSIZE, NULL, wiced_ble_host_PRIORITY, &s_HandleTaskSendReady) != pdPASS)
//    {
//        LOGE("wiced_ble_heartbeat_task created failed\r\n");
//        while (1);
//    }
//
//    vTaskSuspend(s_HandleTaskSendReady);

    /* Start BLE */
    hello_sensor_start();

#if defined(INCLUDE_uxTaskGetStackHighWaterMark) && INCLUDE_uxTaskGetStackHighWaterMark
    int32_t freeStack = uxTaskGetStackHighWaterMark(NULL);
    WPRINT_BT_APP_INFO("Stack high water mark: %d\r\n", freeStack);
#endif

#if 0//defined(configTOTAL_HEAP_SIZE)
    int32_t freeHeap = xPortGetMinimumEverFreeHeapSize();
    WPRINT_BT_APP_INFO(("Minimum ever free heap size: %d\r\n", freeHeap));
#endif

    vTaskDelete(NULL);
}

static int wiced_send_frame_head(int next_packet_length)
{
    cJSON *head_obj_json = cJSON_CreateObject();
    char *string = NULL;

    if( head_obj_json == NULL ) {
        goto end;
    }

    if( cJSON_AddStringToObject(head_obj_json, DEVICENAME_STRING, "RT-Vision") == NULL ) {
        goto end;
    }

    if( cJSON_AddNumberToObject(head_obj_json, LENGTH_STRING, (double)next_packet_length) == NULL) {
        goto end;
    }

    if (is_connected)
    {
        uint16_t client_config = hello_sensor_hostinfo.characteristic_client_configuration;

        hello_sensor_hostinfo.characteristic_client_configuration |= GATT_CLIENT_CONFIG_NOTIFICATION;

        string = cJSON_Print(head_obj_json);

        wiced_bt_send_message((uint8_t *)string, strlen(string));

        cJSON_free(string);

        hello_sensor_hostinfo.characteristic_client_configuration = client_config;
    }
    else
    {
        WPRINT_BT_APP_INFO(("Cannot receive, hello sensor not connected\n"));
    }

end:
    cJSON_Delete(head_obj_json);

    return 0;
}

static int wiced_send_frame_packet(const uint8_t *string, int len)
{
    if (is_connected)
    {
        uint16_t client_config = hello_sensor_hostinfo.characteristic_client_configuration;

        hello_sensor_hostinfo.characteristic_client_configuration |= GATT_CLIENT_CONFIG_NOTIFICATION;

        wiced_bt_send_message(string, len);

        hello_sensor_hostinfo.characteristic_client_configuration = client_config;
    }
    else
    {
        WPRINT_BT_APP_INFO(("Cannot receive, hello sensor not connected\n"));
        return ERR_CMD_OK;
    }

    return 0;
}

int WicedBLE_Send_Ready(void)
{

    char * str_pre = NULL;

    cJSON *ready_json = cJSON_CreateObject();
    if ( ready_json == NULL ) {
       goto end;
    }

    cJSON_AddNumberToObject(ready_json, "max_payload", MAX_BLE_MTU);
    str_pre = cJSON_Print(ready_json);

    remote_frame_struct_t frame_packet = {
        .frame_header.prefix = "fac",
        .frame_header.timestamp = 0,
        .frame_header.version = 0,
        .frame_header.cipher = 0,
        .frame_header.length = strlen(str_pre)+1,
        .command = READY_INDICATION,
        .payload = str_pre,
        .crc = 0,
    };

    char *str_data =  transform_remote_pakeck(frame_packet);
    cJSON_free(str_pre);

    wiced_send_frame_head(strlen(str_data));

    wiced_send_frame_packet((uint8_t *)str_data, strlen(str_data));
    cJSON_free(str_data);


 end:
    cJSON_Delete(ready_json);
    return 0;
}

void wiced_ble_send_ready_task(void *args)
{

    while(1) {

        WicedBLE_Send_Ready();

        vTaskDelay(pdMS_TO_TICKS(5000));

    }

    vTaskDelete(NULL);
}

int WicedBLE_Send(remote_frame_cmd_t cmd, uint8_t *payload, int len)
{
    remote_frame_struct_t frame_packet = {
        .frame_header.prefix = "fac",
        .frame_header.timestamp = 0,
        .frame_header.version = 0,
        .frame_header.cipher = 0,
        .frame_header.length = len,
        .command = cmd,
        .payload = payload,
        .crc = 0,
    };

    char *str_data =  transform_remote_pakeck(frame_packet);

    wiced_send_frame_head(strlen(str_data));

    uint8_t *p_str_data = (uint8_t *)str_data;
    for(int i = 0; i < strlen(str_data); i += RECV_DATA_QUEUE_ITEM_SIZE) {
        if ( i + RECV_DATA_QUEUE_ITEM_SIZE < strlen(str_data))
            wiced_send_frame_packet(p_str_data+i, RECV_DATA_QUEUE_ITEM_SIZE);
        else
            wiced_send_frame_packet(p_str_data+i, strlen((char *)p_str_data+i));
    }

    LOGD(str_data);

    cJSON_free(str_data);

    return 0;
}

/*!
 * @brief Main function.
 */
int WicedBLE_Start(void)
{
//    cJSON_test();

    g_recv_data_queue = xQueueCreate(RECV_DATA_QUEUE_LENGTH, RECV_DATA_QUEUE_ITEM_SIZE);

    if (xTaskCreate(wiced_ble_init, "wiced_ble_init", WICEDBLE_STACKSIZE, NULL, wicedble_task_PRIORITY, NULL) != pdPASS)
    {
        LOGE("wiced_ble_init create failed!\r\n");
        while (1);
    }

    if (xTaskCreate(wiced_ble_host_task, "wiced_ble_receive_task", 1024*10, NULL, wiced_ble_host_PRIORITY, NULL) != pdPASS)
    {
        LOGE("wiced_ble_host_task created failed\r\n");
        while (1);
    }

    return 0;
}
