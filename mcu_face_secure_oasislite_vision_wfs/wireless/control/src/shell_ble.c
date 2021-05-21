/*!
 * Copyright 2018 NXP
 *
 * \file
 *
 * This file is the source file for the GATTDB Shell module
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "network_utils.h"
#include "shell_utils.h"
#include "ble_sig_defines.h"
#include "shell_ble.h"

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
#define UuidToArray(arr, value) \
        arr[1] = value >> 8; \
        arr[0] = value & 0x00FF;

#define mShellGapCmdsCount_c                18
#define mShellGattCmdsCount_c               6
#define mShellGattDbCmdsCount_c             4

#define mMaxServicesCount_d                 6
#define mMaxServiceCharCount_d              8
#define mMaxCharDescriptorsCount_d          2
#define mMaxCharValueLength_d               23
#define mMaxGapAdStructures_d               5
#define mMaxGapScannedDevicesCount_d        10

#define mcEncryptionKeySize_c               16
#define mFastConnMinAdvInterval_c           32      /* 20 ms */
#define mFastConnMaxAdvInterval_c           48      /* 30 ms */
/*! Maximum possible value of the ATT_MTU for this device. This is used during the MTU Exchange. */
#define mAttMaxMtu_c                        247

#define INVALID_HANDLE                      0xFFFF
#define HRS_SHORTENED_LOCAL_NAME            "KHC_HRS"
#define WUART_SHORTENED_LOCAL_NAME          "NXP_FR"

#define HRS_DEMO_TIMEOUT                    100     /* ms */
#define OTAP_TIMEOUT                        100     /* ms */
#define WUART_TIMEOUT                       100     /* ms */

#define mHeartRateReportInterval_c          1       /* heart rate report interval in seconds  */
#define mBatteryLevelReportInterval_c       10      /* battery level report interval in seconds  */

/************************************************************************************
*************************************************************************************
Private type definitions
*************************************************************************************
************************************************************************************/
typedef struct bleShellCmd_tag
{
    char *name;
    int8_t (*cmd)(uint8_t argc, char *argv[]);
} bleShellCmd_t;

/*! LE Security Level */
typedef enum gapSecurityLevel_tag
{
    gSecurityLevel_NoSecurity_c =           0x00,  /*!< No security (combined only with Mode 1). */
    gSecurityLevel_NoMitmProtection_c =     0x01,  /*!< Unauthenticated (no MITM protection). */
    gSecurityLevel_WithMitmProtection_c =   0x02,  /*!< Authenticated (MITM protection by PIN or OOB). */
    gSecurityLevel_LeSecureConnections_c =  0x03   /*!< Authenticated with LE Secure Connections (BLE 4.2 only). */
} gapSecurityLevel_t;

/*! LE Security Mode */
typedef enum gapSecurityMode_tag
{
    gSecurityMode_1_c = 0x10,   /*!< Mode 1 - Encryption required (except for Level 1). */
    gSecurityMode_2_c = 0x20    /*!< Mode 2 - Data Signing required. */
} gapSecurityMode_t;

typedef struct gattUuidNames_tag
{
    uint16_t    uuid;
    char       *name;
} gattUuidNames_t;

typedef enum gattNotifyState_tag
{
    gGetCccd_c,
    gGetNotificationStatus_c
} gattNotifyState_t;

/*! Heart Rate Service - Heart Rate Format */
typedef enum
{
    gHrs_8BitHeartRateFormat_c = 0x00,
    gHrs_16BitHeartRateFormat_c = BIT0
} hrsFlagsHeartRateFormat_tag;

/*! Heart Rate Service - Sensor Contact detected status */
typedef enum
{
    gHrs_SensorContactNotDetected_c    = 0x00,
    gHrs_SensorContactDetected_c       = BIT1
} hrsFlagsSensorContactDetected_tag;

/*! Heart Rate Service - Sensor Contact supported status */
typedef enum
{
    gHrs_SensorContactNotSupported_c    = 0x00,
    gHrs_SensorContactSupported_c       = BIT2
} hrsFlagsSensorContactSupported_tag;

/*! Heart Rate Service - Energy expended status */
typedef enum
{
    gHrs_EnergyExpendedDisabled_c = 0x00,
    gHrs_EnergyExpendedEnabled_c  = BIT3
} hrsFlagsEnergyExpended_tag;

/*! Heart Rate Service - RR Intervals status */
typedef enum
{
    gHrs_RrIntervalsDisabled_c  = 0x00,
    gHrs_RrIntervalsEnabled_c   = BIT4
} hrsFlagsRRIntervals_tag;

typedef uint8_t hrsBodySensorLoc_t;

/*! Heart Rate Service - Body Sensor Location */
typedef enum
{
    gHrs_BodySensorLocOther_c = 0x00,
    gHrs_BodySensorLocChest_c = 0x01,
    gHrs_BodySensorLocWrist_c = 0x02,
    gHrs_BodySensorLocFinger_c = 0x03,
    gHrs_BodySensorLocHand_c = 0x04,
    gHrs_BodySensorLocEarLobe_c = 0x05,
    gHrs_BodySensorLocFoot_c = 0x06
} hrsBodySensorLoc_tag;

typedef enum deviceNamedeviceNameCmdState_tag
{
    gStartState_c,
    gGetServiceHandle_c,
    gServiceHandleReceived_c,
    gGetCharValueHandle_c,
    gCharValueHandleReceived_c,
    gReadAttribute_c,
    gAttrValueReceived_c,
    gWriteAttribute_c,
    gAttributeWritten_c,
    gError_c,
    gFinalState_c
} deviceNamedeviceNameCmdState_t;

typedef uint8_t bleDeviceAddress_t[6];
/************************************************************************************
*************************************************************************************
Public global variables declarations
*************************************************************************************
************************************************************************************/
extern          shell_handle_t ble_shellHandle;
bool_t          gSuppressBleEventPrint = FALSE;
deviceId_t      gPeerDeviceId = gInvalidDeviceId_c;
uint16_t        gLatestHandle = INVALID_HANDLE;
uint16_t        gCccdHandle = INVALID_HANDLE;
bool_t          gIsNotificationActive = FALSE;
hrsDemoState_t  gHrsDemoState = gHrsDemoAddServiceGatt_c;
otapState_t     gOtapState = gOtapAddServiceGatt_c;
wuartState_t    gWUartState = gWUartAddServiceGatt_c;

uint8_t face_PeerAddress_Bond[6];
uint8_t face_PeerAdd_type = 0;
uint16_t ltksend = 0;

bleDeviceAddress_t maBleDeviceAddress;

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static int32_t ShellGap_Command(shell_handle_t context, int32_t argc, char **argv);
static int8_t ShellGap_DeviceAddress(uint8_t argc, char *argv[]);
static int8_t ShellGap_SetSecurityRequirements(void); //FACEREC
static int8_t ShellGap_SetPassKey(uint32_t passkey); //FACEREC
static int8_t ShellGap_GetStaticAdd(void);//FACEREC
static int8_t ShellGap_DeviceName(uint8_t argc, char *argv[]);
static int8_t ShellGap_StartAdvertising(uint8_t argc, char *argv[]);
static int8_t ShellGap_StopAdvertising(uint8_t argc, char *argv[]);
static int8_t ShellGap_SetAdvertisingParameters(uint8_t argc, char *argv[]);
static int8_t ShellGap_ChangeAdvertisingData(uint8_t argc, char *argv[]);
static int8_t ShellGap_StartScanning(uint8_t argc, char *argv[]);
static int8_t ShellGap_StopScanning(uint8_t argc, char *argv[]);
static int8_t ShellGap_SetScanParameters(uint8_t argc, char *argv[]);
static int8_t ShellGap_ChangeScanData(uint8_t argc, char *argv[]);
static int8_t ShellGap_Connect(uint8_t argc, char *argv[]);
static int8_t ShellGap_SetConnectionParameters(uint8_t argc, char *argv[]);
static int8_t ShellGap_Disconnect(uint8_t argc, char *argv[]);
static int8_t ShellGap_UpdateConnection(uint8_t argc, char *argv[]);
static int8_t ShellGap_Pair(uint8_t argc, char *argv[]);
static int8_t ShellGap_PairCfg(uint8_t argc, char *argv[]);
static int8_t ShellGap_EnterPasskey(uint8_t argc, char *argv[]);
static int8_t ShellGap_Bonds(uint8_t argc, char *argv[]);
static void ShellGap_AppendAdvData(GAPSetAdvertisingDataRequest_t *pAdvData,
                                   GAPSetAdvertisingDataRequest_AdvertisingData_AdStructures_Type_t type,
                                   char *pData);

static int32_t ShellGatt_Command(shell_handle_t context, int32_t argc, char **argv);
static int8_t ShellGatt_Discover(uint8_t argc, char *argv[]);
static int8_t ShellGatt_Read(uint8_t argc, char *argv[]);
static int8_t ShellGatt_Write(uint8_t argc, char *argv[], bool_t bNoRsp);
static int8_t ShellGatt_WriteRsp(uint8_t argc, char *argv[]);
static int8_t ShellGatt_WriteCmd(uint8_t argc, char *argv[]);
static int8_t ShellGatt_Notify(uint8_t argc, char *argv[]);
static void ShellGatt_NotifySm(void *param);
static int8_t ShellGatt_Indicate(uint8_t argc, char *argv[]);

static int32_t ShellGattDb_Command(shell_handle_t context, int32_t argc, char **argv);
static int8_t ShellGattDb_Read(uint8_t argc, char *argv[]);
static int8_t ShellGattDb_Write(uint8_t argc, char *argv[]);
static int8_t ShellGattDb_AddService(uint8_t argc, char *argv[]);
static int8_t ShellGattDb_Erase(uint8_t argc, char *argv[]);

static void ShellGattDb_AddPrimaryServiceDecl(uint16_t serviceUuid);
static void ShellGattDb_AddPrimaryServiceDecl128(uint8_t *serviceUuid);
static void ShellGattDb_AddCharDeclValue(
    uint16_t charUuid,
    uint8_t properties,
    uint16_t length,
    uint8_t *value,
    GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_t permissions,
    bool_t varlen
);
static void ShellGattDb_AddCharDeclValue128(
    uint8_t *charUuid,
    uint8_t properties,
    uint16_t length,
    uint8_t *value,
    GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_t permissions,
    bool_t varlen
);
static void ShellGattDb_AddCharDesc(
    uint16_t charUuid,
    uint16_t length,
    uint8_t *value,
    GATTDBDynamicAddCharacteristicDescriptorRequest_DescriptorAccessPermissions_t permissions
);
static void ShellGattDb_AddServiceGatt(void);
static void ShellGattDb_AddServiceGap(void);
static void ShellGattDb_AddServiceBattery(void);
static void ShellGattDb_AddServiceDeviceInfo(void);
static void ShellGattDb_AddServiceHeartRate(void);
static void ShellGattDb_AddServiceOtap(void);
static void ShellGattDb_AddServiceWirelessUART(void);

static int32_t ShellBleDemo_Command(shell_handle_t context, int32_t argc, char **argv);
static void BleApp_DemoHrsSm(void);
static void BleApp_DemoHrsNotify(void *pvParameters);
static void BleApp_DemoHrsNotifyHr(void);
static void BleApp_DemoHrsNotifyBattery(void);
static void BleApp_DemoOtapSm(void);
static void BleApp_DemoWirelessUARTSm(void);
static void BleApp_DemoWirelessUARTWrite(int32_t argc, char **argv);


uint8_t smpLtk[16] =
  {0xD6, 0x93, 0xE8, 0xA4, 0x23, 0x55, 0x48, 0x99,
   0x1D, 0x77, 0x61, 0xE6, 0x63, 0x2B, 0x10, 0x8E};
uint8_t smpIrk[16] =
    {0x0A, 0x2D, 0xF4, 0x65, 0xE3, 0xBD, 0x7B, 0x49,
     0x1E, 0xB4, 0xC0, 0x95, 0x95, 0x13, 0x46, 0x73};

uint8_t smpRand[8] =
    {0x26, 0x1E, 0xF6, 0x09, 0x97, 0x2E, 0xAD, 0x7E};

static uint8_t listofbondeddevice =0;

/************************************************************************************
*************************************************************************************
Private variables declarations
*************************************************************************************
************************************************************************************/
/*                                      Common                                     */
static bool_t mShellCommandsEnabled = FALSE; /* avoid initializing the module multiple times */
static uint16_t mLatestUuid = INVALID_HANDLE;

/*                                       GAP                                       */
/* Scanning and Advertising Data */
static GAPScanningEventDeviceScannedIndication_t mScannedDevices[mMaxGapScannedDevicesCount_d];
static uint8_t mScannedDevicesCount = 0;
static GAPSetAdvertisingDataRequest_t mAppAdvertisingData = { 0 };

static GAPStartScanningRequest_t mAppScanParams =
{
    /* scanning parameters included */  TRUE, \
    {
        \
        /* type */              GAPStartScanningRequest_ScanningParameters_Type_gPassive_c, \
        /* interval */          0x0010, \
        /* window */            0x0010, \
        /* ownAddressType */    GAPStartScanningRequest_ScanningParameters_OwnAddressType_gPublic_c, \
        /* filterPolicy */      GAPStartScanningRequest_ScanningParameters_FilterPolicy_gScanAll_c \
    }
};

static GAPSetAdvertisingParametersRequest_t mAdvParams =
{
    /* minInterval */           1600 /* 1 s */, \
    /* maxInterval */           1600 /* 1 s */, \
    /* advertisingType */       GAPSetAdvertisingParametersRequest_AdvertisingType_gConnectableUndirected_c, \
    /* addressType */           GAPSetAdvertisingParametersRequest_OwnAddressType_gPublic_c, \
    /* directedAddressType */   GAPSetAdvertisingParametersRequest_PeerAddressType_gPublic_c, \
    /* directedAddress */       {0, 0, 0, 0, 0, 0}, \
    /* channelMap */            GAPSetAdvertisingParametersRequest_ChannelMap_gChannel37_c | GAPSetAdvertisingParametersRequest_ChannelMap_gChannel38_c | GAPSetAdvertisingParametersRequest_ChannelMap_gChannel39_c, \
    /* filterPolicy */          GAPSetAdvertisingParametersRequest_FilterPolicy_gProcessAll_c \
};

static GAPPairRequest_t mPairingParameters =
{
    .PairingParameters = {
        .WithBonding = TRUE,
        .SecurityModeAndLevel = GAPPairRequest_PairingParameters_SecurityModeAndLevel_gMode1Level3_c,
        .MaxEncryptionKeySize = mcEncryptionKeySize_c,
        .LocalIoCapabilities = GAPPairRequest_PairingParameters_LocalIoCapabilities_gIoDisplayOnly_c,
        .OobAvailable = FALSE,
        .CentralKeys = GAPPairRequest_PairingParameters_CentralKeys_gLtk_c | GAPPairRequest_PairingParameters_CentralKeys_gIrk_c,
        .PeripheralKeys = GAPPairRequest_PairingParameters_PeripheralKeys_gLtk_c | GAPPairRequest_PairingParameters_PeripheralKeys_gIrk_c,
        .LeSecureConnectionSupported = FALSE,
        .UseKeypressNotifications = FALSE
    }
};

/* Default Connection Request Parameters */
static GAPConnectRequest_t mConnReqParams =
{
    .ScanInterval = 0x0010,
    .ScanWindow = 0x0010,
    .FilterPolicy = GAPConnectRequest_FilterPolicy_gUseDeviceAddress_c,
    .OwnAddressType = GAPConnectRequest_OwnAddressType_gPublic_c,
    .ConnIntervalMin = 0x00A0,
    .ConnIntervalMax = 0x00A0,
    .ConnLatency = 0x0000,
    .SupervisionTimeout = 0x0C80,
    .ConnEventLengthMin = 0x0000,
    .ConnEventLengthMax = 0xFFFF,
};

static char mDeviceName[mMaxCharValueLength_d + 1] = "";
static bool_t mSetDeviceName = FALSE;
static deviceNamedeviceNameCmdState_t deviceNameCmdState = gStartState_c;

/*                                       GATT                                      */
static bool_t mScanningOn = FALSE;
static bool_t mIsMaster = FALSE;
static bool_t mbIndicate = FALSE;
static uint16_t mNotifyHandle = INVALID_HANDLE;
static uint8_t mCurrentServiceInDiscoveryIndex = 0;
static uint8_t mCurrentCharInDiscoveryIndex = 0;
static GATTClientProcedureDiscoverAllPrimaryServicesIndication_t mAllPrimaryServices;
static GATTClientProcedureDiscoverAllCharacteristicsIndication_t mAllChars;

/*                                     GATT-DB                                     */
static uint8_t mGattDbDynamic_GattServiceChangedInitValue[]                 = {0x00, 0x00, 0x00, 0x00};
static uint8_t mGattDbDynamic_GapServiceDeviceNameInitValue[]               = "NXP_FR";
static uint8_t mGattDbDynamic_GapServiceAppearanceInitValue[]               = {UuidArray(0)}; // gUnknown_c
static uint8_t mGattDbDynamic_GapServicePpcpInitValue[]                     = {0x0A, 0x00, 0x10, 0x00, 0x64, 0x00, 0xE2, 0x04};
static uint8_t mGattDbDynamic_HRServiceHrMeasurementInitValue[]             = {0x00, 0xB4};
static uint8_t mGattDbDynamic_HRServiceBodySensorLocationInitValue[]        = {0x01};
static uint8_t mGattDbDynamic_HRServiceHrControlPointInitValue[]            = {0x00};
static uint8_t mGattDbDynamic_BServiceLevelInitValue[]                      = {0x5A};
static uint8_t mGattDbDynamic_BServiceCharPresFormatDescriptorInitValue[]   = {0x04, 0x00, 0xAD, 0x27, 0x01, 0x00, 0x00};
static uint8_t mGattDbDynamic_DIServiceManufNameInitValue[]                 = "NXP";
static uint8_t mGattDbDynamic_DIServiceModelNbInitValue[]                   = "Kinetis BLE";
static uint8_t mGattDbDynamic_DIServiceSerialNoInitValue[]                  = "BLESN01";
static uint8_t mGattDbDynamic_DIServiceHwRevInitValue[]                     = "FRDM-KW41Z";
static uint8_t mGattDbDynamic_DIServiceFwRevInitValue[]                     = "1.1.1";
static uint8_t mGattDbDynamic_DIServiceSwRevInitValue[]                     = "1.1.4";
static uint8_t mGattDbDynamic_DIServiceSysIdInitValue[]                     = {0x00, 0x00, 0x00, 0xFE, 0xFF, 0x9F, 0x04, 0x00};
static uint8_t mGattDbDynamic_DIServiceIeeeRcdlInitValue[]                  = {0x00, 0x00, 0x00, 0x00};
static uint8_t mGattDbDynamic_OtapServicePCPInitValue[16]                   = { 0x00 };
static uint8_t mGattDbDynamic_OtapServiceDataInitValue[mAttMaxMtu_c - 3]    = { 0x00 };
static uint8_t mGattDbDynamic_WUARTServiceStreamInitValue[mAttMaxMtu_c - 2] = { 0x00 };

/*                                      DEMO                                       */
static TaskHandle_t xHrsTaskHandle = NULL;

static uint16_t hServiceBattery;
static uint16_t hServiceHeartRate;
static uint16_t hValueHrMeasurement;
static uint16_t hValueBodyLocation;
static uint16_t hValueBatteryLevel;
uint16_t hUartStream;
uint16_t hPeerUartStream;
uint16_t hReadUartStream;
uint16_t hServiceUartStream;
static uint8_t hrsReportedValue = 0;

static uint8_t uuid_service_otap[16]            = {0xE0, 0x1C, 0x4B, 0x5E, 0x1E, 0xEB, 0xA1, 0x5C, 0xEE, 0xF4, 0x5E, 0xBA, 0x50, 0x55, 0xFF, 0x01};
static uint8_t uuid_char_otap_control_point[16] = {0xE0, 0x1C, 0x4B, 0x5E, 0x1E, 0xEB, 0xA1, 0x5C, 0xEE, 0xF4, 0x5E, 0xBA, 0x51, 0x55, 0xFF, 0x01};
static uint8_t uuid_char_otap_data[16]          = {0xE0, 0x1C, 0x4B, 0x5E, 0x1E, 0xEB, 0xA1, 0x5C, 0xEE, 0xF4, 0x5E, 0xBA, 0x52, 0x55, 0xFF, 0x01};

static uint8_t uuid_service_wireless_uart[16]   = {0xE0, 0x1C, 0x4B, 0x5E, 0x1E, 0xEB, 0xA1, 0x5C, 0xEE, 0xF4, 0x5E, 0xBA, 0x00, 0x01, 0xFF, 0x01};
static uint8_t uuid_uart_stream[16]             = {0xE0, 0x1C, 0x4B, 0x5E, 0x1E, 0xEB, 0xA1, 0x5C, 0xEE, 0xF4, 0x5E, 0xBA, 0x01, 0x01, 0xFF, 0x01};
static uint8_t uuid_uart_stream_read[16]        = {0xE0, 0x1C, 0x4B, 0x5E, 0x1E, 0xEB, 0xA1, 0x5C, 0xEE, 0xF4, 0x5E, 0xBA, 0x02, 0x01, 0xFF, 0x01};


static const gattUuidNames_t mGattServices[] =
{
    {  0x1800  , "Generic Access"},
    {  0x1801  , "Generic Attribute"},
    {  0x1802  , "Immediate Alert"},
    {  0x1803  , "Link Loss"},
    {  0x1804  , "Tx Power"},
    {  0x1805  , "Current Time Service"},
    {  0x1806  , "Reference Time Update Service"},
    {  0x1807  , "Next DST Change Service"},
    {  0x1808  , "Glucose"},
    {  0x1809  , "Health Thermometer"},
    {  0x180A  , "Device Information"},
    {  0x180D  , "Heart Rate"},
    {  0x180E  , "Phone Alert Status Service"},
    {  0x180F  , "Battery Service"},
    {  0x1810  , "Blood Pressure"},
    {  0x1811  , "Alert Notification Service"},
    {  0x1812  , "Human Interface Device"},
    {  0x1813  , "Scan Parameters"},
    {  0x1814  , "Running Speed and Cadence"},
    {  0x1815  , "Automation IO"},
    {  0x1816  , "Cycling Speed and Cadence"},
    {  0x1818  , "Cycling Power"},
    {  0x1819  , "Location and Navigation"},
    {  0x181A  , "Environmental Sensing"},
    {  0x181B  , "Body Composition"},
    {  0x181C  , "User Data"},
    {  0x181D  , "Weight Scale"},
    {  0x181E  , "Bond Management"},
    {  0x181F  , "Continuous Glucose Monitoring"},
    {  0x1820  , "Internet Protocol Support"},
    {  0x1821  , "Indoor Positioning"},
    {  0x1822  , "Pulse Oximeter"},
    {  0x1823  , "HTTP Proxy"},
    {  0x1824  , "Transport Discovery"},
    {  0x1825  , "Object Transfer"}
};

static const gattUuidNames_t mGattChars[] =
{
    {  0x2A00  , "Device Name"},
    {  0x2A01  , "Appearance"},
    {  0x2A02  , "Peripheral Privacy Flag"},
    {  0x2A03  , "Reconnection Address"},
    {  0x2A04  , "Peripheral Preferred Connection Parameters"},
    {  0x2A05  , "Service Changed"},
    {  0x2A06  , "Alert Level"},
    {  0x2A07  , "Tx Power Level"},
    {  0x2A08  , "Date Time"},
    {  0x2A09  , "Day of Week"},
    {  0x2A0A  , "Day Date Time"},
    {  0x2A0C  , "Exact Time 256"},
    {  0x2A0D  , "DST Offset"},
    {  0x2A0E  , "Time Zone"},
    {  0x2A0F  , "Local Time Information"},
    {  0x2A11  , "Time with DST"},
    {  0x2A12  , "Time Accuracy"},
    {  0x2A13  , "Time Source"},
    {  0x2A14  , "Reference Time Information"},
    {  0x2A16  , "Time Update Control Point"},
    {  0x2A17  , "Time Update State"},
    {  0x2A18  , "Glucose Measurement"},
    {  0x2A19  , "Battery Level"},
    {  0x2A1C  , "Temperature Measurement"},
    {  0x2A1D  , "Temperature Type"},
    {  0x2A1E  , "Intermediate Temperature"},
    {  0x2A21  , "Measurement Interval"},
    {  0x2A22  , "Boot Keyboard Input Report"},
    {  0x2A23  , "System ID"},
    {  0x2A24  , "Model Number String"},
    {  0x2A25  , "Serial Number String"},
    {  0x2A26  , "Firmware Revision String"},
    {  0x2A27  , "Hardware Revision String"},
    {  0x2A28  , "Software Revision String"},
    {  0x2A29  , "Manufacturer Name String"},
    {  0x2A2A  , "IEEE 11073-20601 Regulatory Certification Data List"},
    {  0x2A2B  , "Current Time"},
    {  0x2A2C  , "Magnetic Declination"},
    {  0x2A31  , "Scan Refresh"},
    {  0x2A32  , "Boot Keyboard Output Report"},
    {  0x2A33  , "Boot Mouse Input Report"},
    {  0x2A34  , "Glucose Measurement Context"},
    {  0x2A35  , "Blood Pressure Measurement"},
    {  0x2A36  , "Intermediate Cuff Pressure"},
    {  0x2A37  , "Heart Rate Measurement"},
    {  0x2A38  , "Body Sensor Location"},
    {  0x2A39  , "Heart Rate Control Point"},
    {  0x2A3F  , "Alert Status"},
    {  0x2A40  , "Ringer Control Point"},
    {  0x2A41  , "Ringer Setting"},
    {  0x2A42  , "Alert Category ID Bit Mask"},
    {  0x2A43  , "Alert Category ID"},
    {  0x2A44  , "Alert Notification Control Point"},
    {  0x2A45  , "Unread Alert Status"},
    {  0x2A46  , "New Alert"},
    {  0x2A47  , "Supported New Alert Category"},
    {  0x2A48  , "Supported Unread Alert Category"},
    {  0x2A49  , "Blood Pressure Feature"},
    {  0x2A4A  , "HID Information"},
    {  0x2A4B  , "Report Map"},
    {  0x2A4C  , "HID Control Point"},
    {  0x2A4D  , "Report"},
    {  0x2A4E  , "Protocol Mode"},
    {  0x2A4F  , "Scan Interval Window"},
    {  0x2A50  , "PnP ID"},
    {  0x2A51  , "Glucose Feature"},
    {  0x2A52  , "Record Access Control Point"},
    {  0x2A53  , "RSC Measurement"},
    {  0x2A54  , "RSC Feature"},
    {  0x2A55  , "SC Control Point"},
    {  0x2A56  , "Digital"},
    {  0x2A58  , "Analog"},
    {  0x2A5A  , "Aggregate"},
    {  0x2A5B  , "CSC Measurement"},
    {  0x2A5C  , "CSC Feature"},
    {  0x2A5D  , "Sensor Location"},
    {  0x2A5E  , "PLX Spot-Check Measurement"},
    {  0x2A5F  , "PLX Continuous Measurement"},
    {  0x2A60  , "PLX Features"},
    {  0x2A63  , "Cycling Power Measurement"},
    {  0x2A64  , "Cycling Power Vector"},
    {  0x2A65  , "Cycling Power Feature"},
    {  0x2A66  , "Cycling Power Control Point"},
    {  0x2A67  , "Location and Speed"},
    {  0x2A68  , "Navigation"},
    {  0x2A69  , "Position Quality"},
    {  0x2A6A  , "LN Feature"},
    {  0x2A6B  , "LN Control Point"},
    {  0x2A6C  , "Elevation"},
    {  0x2A6D  , "Pressure"},
    {  0x2A6E  , "Temperature"},
    {  0x2A6F  , "Humidity"},
    {  0x2A70  , "True Wind Speed"},
    {  0x2A71  , "True Wind Direction"},
    {  0x2A72  , "Apparent Wind Speed"},
    {  0x2A73  , "Apparent Wind Direction "},
    {  0x2A74  , "Gust Factor"},
    {  0x2A75  , "Pollen Concentration"},
    {  0x2A76  , "UV Index"},
    {  0x2A77  , "Irradiance"},
    {  0x2A78  , "Rainfall"},
    {  0x2A79  , "Wind Chill"},
    {  0x2A7A  , "Heat Index"},
    {  0x2A7B  , "Dew Point"},
    {  0x2A7D  , "Descriptor Value Changed"},
    {  0x2A7E  , "Aerobic Heart Rate Lower Limit"},
    {  0x2A7F  , "Aerobic Threshold"},
    {  0x2A80  , "Age"},
    {  0x2A81  , "Anaerobic Heart Rate Lower Limit"},
    {  0x2A82  , "Anaerobic Heart Rate Upper Limit"},
    {  0x2A83  , "Anaerobic Threshold"},
    {  0x2A84  , "Aerobic Heart Rate Upper Limit"},
    {  0x2A85  , "Date of Birth"},
    {  0x2A86  , "Date of Threshold Assessment"},
    {  0x2A87  , "Email Address"},
    {  0x2A88  , "Fat Burn Heart Rate Lower Limit"},
    {  0x2A89  , "Fat Burn Heart Rate Upper Limit"},
    {  0x2A8A  , "First Name"},
    {  0x2A8B  , "Five Zone Heart Rate Limits"},
    {  0x2A8C  , "Gender"},
    {  0x2A8D  , "Heart Rate Max"},
    {  0x2A8E  , "Height"},
    {  0x2A8F  , "Hip Circumference"},
    {  0x2A90  , "Last Name"},
    {  0x2A91  , "Maximum Recommended Heart Rate"},
    {  0x2A92  , "Resting Heart Rate"},
    {  0x2A93  , "Sport Type for Aerobic and Anaerobic Thresholds"},
    {  0x2A94  , "Three Zone Heart Rate Limits"},
    {  0x2A95  , "Two Zone Heart Rate Limit"},
    {  0x2A96  , "VO2 Max"},
    {  0x2A97  , "Waist Circumference"},
    {  0x2A98  , "Weight"},
    {  0x2A99  , "Database Change Increment"},
    {  0x2A9A  , "User Index"},
    {  0x2A9B  , "Body Composition Feature"},
    {  0x2A9C  , "Body Composition Measurement"},
    {  0x2A9D  , "Weight Measurement"},
    {  0x2A9E  , "Weight Scale Feature"},
    {  0x2A9F  , "User Control Point"},
    {  0x2AA0  , "Magnetic Flux Density - 2D"},
    {  0x2AA1  , "Magnetic Flux Density - 3D"},
    {  0x2AA2  , "Language"},
    {  0x2AA3  , "Barometric Pressure Trend"},
    {  0x2AA4  , "Bond Management Control Point"},
    {  0x2AA5  , "Bond Management Feature"},
    {  0x2AA6  , "Central Address Resolution"},
    {  0x2AA7  , "CGM Measurement"},
    {  0x2AA8  , "CGM Feature"},
    {  0x2AA9  , "CGM Status"},
    {  0x2AAA  , "CGM Session Start Time"},
    {  0x2AAB  , "CGM Session Run Time"},
    {  0x2AAC  , "CGM Specific Ops Control Point"},
    {  0x2AAD  , "Indoor Positioning Configuration"},
    {  0x2AAE  , "Latitude"},
    {  0x2AAF  , "Longitude"},
    {  0x2AB0  , "Local North Coordinate"},
    {  0x2AB1  , "Local East Coordinate"},
    {  0x2AB2  , "Floor Number"},
    {  0x2AB3  , "Altitude"},
    {  0x2AB4  , "Uncertainty"},
    {  0x2AB5  , "Location Name"},
    {  0x2AB6  , "URI"},
    {  0x2AB7  , "HTTP Headers"},
    {  0x2AB8  , "HTTP Status Code"},
    {  0x2AB9  , "HTTP Entity Body"},
    {  0x2ABA  , "HTTP Control Point"},
    {  0x2ABB  , "HTTPS Security"},
    {  0x2ABC  , "TDS Control Point"},
    {  0x2ABD  , "OTS Feature"},
    {  0x2ABE  , "Object Name"},
    {  0x2ABF  , "Object Type"},
    {  0x2AC0  , "Object Size"},
    {  0x2AC1  , "Object First-Created"},
    {  0x2AC2  , "Object Last-Modified"},
    {  0x2AC3  , "Object ID"},
    {  0x2AC4  , "Object Properties"},
    {  0x2AC5  , "Object Action Control Point"},
    {  0x2AC6  , "Object List Control Point"},
    {  0x2AC7  , "Object List Filter"},
    {  0x2AC8  , "Object Changed"}
};

static const gattUuidNames_t mGattDescriptors[] =
{
    {0x2900, "Characteristic Extended Properties"},
    {0x2901, "Characteristic User Description"},
    {0x2902, "Client Characteristic Configuration"},
    {0x2903, "Server Characteristic Configuration"},
    {0x2904, "Characteristic Presentation Format"},
    {0x2905, "Characteristic Aggregate Format"},
    {0x2906, "Valid Range"},
    {0x2907, "External Report Reference"},
    {0x2908, "Report Reference"},
    {0x2909, "Number of Digitals"},
    {0x290A, "Value Trigger Setting"},
    {0x290B, "Environmental Sensing Configuration"},
    {0x290C, "Environmental Sensing Measurement"},
    {0x290D, "Environmental Sensing Trigger Setting"},
    {0x290E, "Time Trigger Setting"}
};

static const bleShellCmd_t mGapShellCmds[mShellGapCmdsCount_c] =
{
    {"address",     ShellGap_DeviceAddress},
    {"devicename",  ShellGap_DeviceName},

    {"advstart",    ShellGap_StartAdvertising},
    {"advstop",     ShellGap_StopAdvertising},
    {"advcfg",      ShellGap_SetAdvertisingParameters},
    {"advdata",     ShellGap_ChangeAdvertisingData},

    {"scanstart",   ShellGap_StartScanning},
    {"scanstop",    ShellGap_StopScanning},
    {"scancfg",     ShellGap_SetScanParameters},
    {"scandata",    ShellGap_ChangeScanData},

    {"connect",     ShellGap_Connect},
    {"connectcfg",  ShellGap_SetConnectionParameters},
    {"disconnect",  ShellGap_Disconnect},
    {"connupdate",  ShellGap_UpdateConnection},

    {"pair",        ShellGap_Pair},
    {"paircfg",     ShellGap_PairCfg},
    {"enterpin",    ShellGap_EnterPasskey},
    {"bonds",       ShellGap_Bonds},
};

static const bleShellCmd_t mGattShellCmds[mShellGattCmdsCount_c] =
{
    {"discover",    ShellGatt_Discover},
    {"read",        ShellGatt_Read},
    {"write",       ShellGatt_WriteRsp},
    {"writecmd",    ShellGatt_WriteCmd},
    {"notify",      ShellGatt_Notify},
    {"indicate",    ShellGatt_Indicate}
};

static const bleShellCmd_t mGattDbShellCmds[mShellGattDbCmdsCount_c] =
{
    {"read",        ShellGattDb_Read},
    {"write",       ShellGattDb_Write},
    {"addservice",  ShellGattDb_AddService},
    {"erase",       ShellGattDb_Erase}
};

static shell_command_t aShellCommands[] =
{
    {
        "gap",
        "\r\n\"gap\": Contains commands for advertising, scanning, connecting, pairing or disconnecting\r\n"
        "   gap address\r\n"
        "   gap devicename [new devicename]\r\n"
        "   gap advcfg [-interval intervalInMs] [-type type]\r\n"
        "   gap advstart\r\n"
        "   gap advstop\r\n"
        "   gap advdata <-erase | [type] [payload]>\r\n"
        "   gap scanstart\r\n"
        "   gap scanstop\r\n"
        "   gap scancfg [-type type] [-interval intervalInMs] [-window windowInMs]\r\n"
        "   gap scandata [-erase] [-add type payload]\r\n"
        "   gap connectcfg [-interval intervalInMs] [-latency latency] [-timeout timeout]\r\n"
        "   gap connect scannedDeviceId\r\n"
        "   gap disconnect\r\n"
        "   gap connupdate mininterval maxinterval latency timeout\r\n"
        "   gap paircfg [-usebonding usebonding] [-seclevel seclevel] [-keyflags flags]\r\n"
        "   gap pair\r\n"
        "   gap enterpin pin\r\n"
        "   gap bonds [-erase] [-remove deviceIndex]\r\n",
        ShellGap_Command, SHELL_IGNORE_PARAMETER_COUNT, {0},
    },
    {
        "gatt",
        "\r\n\"gatt\": Contains commands for service discovery, read, write, notify and indicate\r\n"
        "   gatt discover [-all] [-service serviceUuid16InHex]\r\n"
        "   gatt read handle\r\n"
        "   gatt write handle valueInHex\r\n"
        "   gatt writecmd handle valueInHex\r\n"
        "   gatt notify handle\r\n"
        "   gatt indicate handle valueInHex\r\n"
        "Values in hex must be formatted as 0xXX..X\r\n",
        ShellGatt_Command, SHELL_IGNORE_PARAMETER_COUNT, {0},
    },
    {
        "gattdb",
        "\r\n\"gattdb\": Contains commands for adding services, reading and writing characteristics on the local database\r\n"
        "   gattdb read handle\r\n"
        "   gattdb write handle valueInHex\r\n"
        "   gattdb addservice <gatt | gap | battery | hr | devinfo | serviceUuid16InHex>\r\n"
        "   gattdb erase\r\n"
        "Values in hex must be formatted as 0xXX..X\r\n",
        ShellGattDb_Command, SHELL_IGNORE_PARAMETER_COUNT, {0},
    },
    {
        "bledemo",
        "\r\n\"bledemo\": Commands that implement complete profiles: Heart Rate Sensor (hrs), OTA (otap) and Wireless UART (wuart) profiles are supported\r\n"
        "   bledemo hrs\r\n"
        "   bledemo otap\r\n"
        "   bledemo wuart [text]\r\n\r\n",
        ShellBleDemo_Command, SHELL_IGNORE_PARAMETER_COUNT, {0},
    }
};

/***************************************************************************************************
****************************************************************************************************
Public functions
****************************************************************************************************
***************************************************************************************************/
/*!*************************************************************************************************
\fn     void SHELL_BLE_Init(void)
\brief  This function is used to initialize the SHELL commands module.
***************************************************************************************************/
void SHELL_BLE_Init(void)
{
    if (!mShellCommandsEnabled)
    {
        /* Register functions */
        for (int i = 0; i < NumberOfElements(aShellCommands); i++)
        {
            SHELL_RegisterCommand(ble_shellHandle, &aShellCommands[i]);
        }

        mShellCommandsEnabled = TRUE;
    }
}

/*!*************************************************************************************************
\fn     void SHELL_BLE_Terminate(void)
\brief  This function is used to mark all BLE global variables as invalid.
***************************************************************************************************/
void SHELL_BLE_Terminate(void)
{
    // clear device name
    FLib_MemSet(mDeviceName, 0, sizeof(mDeviceName));

    // clear advertising and scanning data
    for (uint8_t i = 0; i < mAppAdvertisingData.AdvertisingData.NbOfAdStructures; i++)
    {
        MEM_BufferFree(mAppAdvertisingData.AdvertisingData.AdStructures[i].Data);
    }

    for (uint8_t i = 0; i < mAppAdvertisingData.ScanResponseData.NbOfAdStructures; i++)
    {
        MEM_BufferFree(mAppAdvertisingData.ScanResponseData.AdStructures[i].Data);
    }

    MEM_BufferFree(mAppAdvertisingData.AdvertisingData.AdStructures);
    MEM_BufferFree(mAppAdvertisingData.ScanResponseData.AdStructures);
    FLib_MemSet(&mAppAdvertisingData.AdvertisingData, 0, sizeof(mAppAdvertisingData.AdvertisingData));
    FLib_MemSet(&mAppAdvertisingData.ScanResponseData, 0, sizeof(mAppAdvertisingData.ScanResponseData));
    mAppAdvertisingData.AdvertisingDataIncluded = FALSE;
    mAppAdvertisingData.ScanResponseDataIncluded = FALSE;

    gHrsDemoState = gHrsDemoAddServiceGatt_c;
    gOtapState = gOtapAddServiceGatt_c;
    gWUartState = gWUartAddServiceGatt_c;

    gLatestHandle = INVALID_HANDLE;
    gSuppressBleEventPrint = FALSE;

    // stop running HRS
    if (xHrsTaskHandle != NULL)
    {
        vTaskSuspend(xHrsTaskHandle);
    }

    shell_change_prompt(SHELL_PROMPT);
}


void BleApp_SendLtk(void)
{
	uint8_t data[16] = {0x00};

	GAPGetBondedDevicesCountRequest(BLE_FSCI_IF);

	GAPSendSmpKeysRequest_t smp_req;
	smp_req.DeviceId = gPeerDeviceId;
	smp_req.Keys.LtkIncluded = 1;
	smp_req.Keys.LtkInfo.LtkSize = 16;
	smp_req.Keys.LtkInfo.Ltk = smpLtk;
	FLib_MemCpy(smp_req.Keys.Csrk,data,16);
	FLib_MemCpy(smp_req.Keys.AddressInfo.DeviceAddress,maBleDeviceAddress,sizeof(bleDeviceAddress_t));
	smp_req.Keys.AddressInfo.DeviceAddressType = GAPSendSmpKeysRequest_Keys_AddressInfo_DeviceAddressType_gPublic_c;
	smp_req.Keys.CsrkIncluded = 0;
	smp_req.Keys.IrkIncluded = 1;
	FLib_MemCpy(smp_req.Keys.Irk,smpIrk,16);
	smp_req.Keys.AddressIncluded = 1;
	smp_req.Keys.RandEdivInfo.Ediv = 0x1F99;
	smp_req.Keys.RandEdivInfo.RandSize = 8;
	smp_req.Keys.RandEdivInfo.Rand = smpRand;


	GAPSendSmpKeysRequest(&smp_req , BLE_FSCI_IF);

}

void BleApp_Bond(void)
{
	GAPAddDeviceToWhiteListRequest_t bond;

	bond.AddressType = face_PeerAdd_type;
	FLib_MemCpy(bond.Address,face_PeerAddress_Bond,sizeof(bond.Address));

	GAPAddDeviceToWhiteListRequest(&bond,BLE_FSCI_IF);
	//BleApp_BondReq();
	//GAPGetBondedDevicesCountRequest(BLE_FSCI_IF);
}

void BleApp_BondReq(void)
{
	GAPCheckIfBondedRequest_t bondreq;
	bondreq.DeviceId = gPeerDeviceId;
	GAPCheckIfBondedRequest(&bondreq,BLE_FSCI_IF);
}

void BleApp_UpdateConnectionParameters(void)
{
	GAPEnableUpdateConnectionParametersRequest_t update_data;

	update_data.DeviceId = gPeerDeviceId;
	update_data.Enable = TRUE;

	GAPEnableUpdateConnectionParametersRequest(&update_data,BLE_FSCI_IF);

}

void BleApp_LoadCustomerPeerInfo(void)
{
	GAPLoadCustomPeerInformationRequest_t cust_info;
	cust_info.DeviceId = gPeerDeviceId;
	cust_info.InfoSize = 4;
	cust_info.Offset = 0;
	GAPLoadCustomPeerInformationRequest(&cust_info,BLE_FSCI_IF);
}

void BleApp_Encyption(void)
{
	GAPEncryptLinkRequest_t req;
	req.DeviceId = gPeerDeviceId;
	GAPEncryptLinkRequest(&req,BLE_FSCI_IF);
}

void BleApp_Sendseqreq(void)
{
	GAPSendSlaveSecurityRequestRequest_t seqreq;
	seqreq.BondAfterPairing = TRUE;
	seqreq.DeviceId = gPeerDeviceId;
	seqreq.SecurityModeLevel = GAPSendSlaveSecurityRequestRequest_SecurityModeLevel_gMode1Level3_c;
	GAPSendSlaveSecurityRequestRequest(&seqreq,BLE_FSCI_IF);
}

void BleApp_LTKreq(GAPConnectionEventLongTermKeyRequestIndication_t ltkind)
{
	GAPProvideLongTermKeyRequest_t ltkreq;
	if ((ltkind.DeviceId == gPeerDeviceId) && (ltkind.Ediv == 0x1F99)
			&& (ltkind.RandSize == 8)
			&& (TRUE == FLib_MemCmp(ltkind.Rand, smpRand, 8)))
	{
		ltkreq.DeviceId = gPeerDeviceId;
		ltkreq.Ltk = smpLtk;
		ltkreq.LtkSize = 16;
		GAPProvideLongTermKeyRequest(&ltkreq,BLE_FSCI_IF);
	}else
	{
		PRINTF("WRONG LTK REQ\n");
	}
}


void BleApp_Addbondaddress(GAPGetBondedStaticAddressesIndication_t bondadd)
{
	if(bondadd.NbOfDeviceAddresses > 0 )
	{
		PRINTF("Add device to white list number of device present %d\n",bondadd.NbOfDeviceAddresses);
		listofbondeddevice = bondadd.NbOfDeviceAddresses;
		GAPAddDeviceToWhiteListRequest_t bond;
		for(uint8_t i = 0; i < bondadd.NbOfDeviceAddresses; i++)
		{
			FLib_MemSet(bond.Address,'\0',sizeof(bond.Address));
			bond.AddressType = GAPAddDeviceToWhiteListRequest_AddressType_gPublic_c;
			FLib_MemCpy(bond.Address,(void *)&bondadd.DeviceAddresses[i],sizeof(bond.Address));
			GAPAddDeviceToWhiteListRequest(&bond,BLE_FSCI_IF);
		}
	}
	else
	{
		PRINTF("\nNo Bonded device found\n");
	}
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/************************************************************************************
* GAP
************************************************************************************/
static int32_t ShellGap_Command
(
    shell_handle_t context,
    int32_t argc,
    char **argv
)
{
    uint8_t i;

    if (argc < 2)
    {
        return CMD_RET_USAGE;
    }

    for (i = 0; i < mShellGapCmdsCount_c; i++)
    {
        if (!strcmp((char *)argv[1], mGapShellCmds[i].name))
        {
            return mGapShellCmds[i].cmd(argc - 2, (char **)(&argv[2]));
        }
    }

    return CMD_RET_USAGE;
}

static int8_t ShellGap_DeviceAddress(uint8_t argc, char *argv[])
{
    switch (argc)
    {
        case 0:
        {
            GAPReadPublicDeviceAddressRequest(BLE_FSCI_IF);
            return CMD_RET_SUCCESS;
        }

        default:
            return CMD_RET_USAGE;
    }
}

static int8_t ShellGap_SetSecurityRequirements(void)
{
	GAPRegisterDeviceSecurityRequirementsRequest_t sec_req;
	sec_req.SecurityRequirementsIncluded = 1;
	sec_req.SecurityRequirements.NbOfServices = 0;
	sec_req.SecurityRequirements.GapServiceSecurityRequirements = NULL;
	sec_req.SecurityRequirements.MasterSecurityRequirements.Authorization = FALSE;
	sec_req.SecurityRequirements.MasterSecurityRequirements.MinimumEncryptionKeySize = 16;
	sec_req.SecurityRequirements.MasterSecurityRequirements.SecurityModeLevel = GAPSetDefaultPairingParametersRequest_PairingParameters_SecurityModeAndLevel_gMode1Level3_c;

	GAPRegisterDeviceSecurityRequirementsRequest(&sec_req, BLE_FSCI_IF);
	return CMD_RET_SUCCESS;

}

static int8_t ShellGap_SetPassKey(uint32_t passkey)
{
	if(passkey != 0)
	{
		GAPSetLocalPasskeyRequest_t lpasskey;
		lpasskey.Passkey = passkey;
		GAPSetLocalPasskeyRequest(&lpasskey,BLE_FSCI_IF);
		return CMD_RET_SUCCESS;
	}
	else
	{
		return CMD_RET_USAGE;
	}

}

static int8_t ShellGap_GetStaticAdd(void)
{
	GAPGetBondedStaticAddressesRequest_t getbondadd;
	getbondadd.MaxNbOfDevices = 8;
	GAPGetBondedStaticAddressesRequest(&getbondadd,BLE_FSCI_IF);
	return CMD_RET_USAGE;
}

static int8_t ShellGap_ChangeAdvertisingData(uint8_t argc, char *argv[])
{
    switch (argc)
    {
        case 0:
        {
            shell_printf("\r\n-->  Advertising Data Included -> %d", mAppAdvertisingData.AdvertisingDataIncluded);
            shell_printf("\r\n-->  Advertising Data Structures -> %d", mAppAdvertisingData.AdvertisingData.NbOfAdStructures);
            return CMD_RET_SUCCESS;
        }
        break;

        case 1:
        {
            if (!strcmp((char *)argv[0], "-clear"))
            {
                // free allocated memory for adv data
                for (uint8_t i = 0; i < mAppAdvertisingData.AdvertisingData.NbOfAdStructures; i++)
                {
                    MEM_BufferFree(mAppAdvertisingData.AdvertisingData.AdStructures[i].Data);
                }

                MEM_BufferFree(mAppAdvertisingData.AdvertisingData.AdStructures);

                // set to 0
                FLib_MemSet(&mAppAdvertisingData.AdvertisingData, 0, sizeof(mAppAdvertisingData.AdvertisingData));
                mAppAdvertisingData.AdvertisingDataIncluded = FALSE;

                GAPSetAdvertisingDataRequest(&mAppAdvertisingData, BLE_FSCI_IF);
                shell_write("\r\n-->  Advertising Data Erased.");
                return CMD_RET_SUCCESS;
            }
        }
        break;

        default:
        {
            if (argv)
            {
                GAPSetAdvertisingDataRequest_AdvertisingData_AdStructures_Type_t advType =
                    (GAPSetAdvertisingDataRequest_AdvertisingData_AdStructures_Type_t)atoi(argv[0]);
                ShellGap_AppendAdvData(&mAppAdvertisingData, advType, argv[1]);
            }

            GAPSetAdvertisingDataRequest(&mAppAdvertisingData, BLE_FSCI_IF);
            return CMD_RET_SUCCESS;
        }
        break;
    }

    return CMD_RET_USAGE;
}

static void ShellGap_AppendAdvData
(
    GAPSetAdvertisingDataRequest_t *pAdvData,
    GAPSetAdvertisingDataRequest_AdvertisingData_AdStructures_Type_t type,
    char *pData
)
{
    uint8_t i, advIdx = 0;
    uint8_t length;
    uint32_t number;

    if (pAdvData->AdvertisingData.AdStructures == NULL)
    {
        pAdvData->AdvertisingData.AdStructures =
            MEM_BufferAlloc(mMaxGapAdStructures_d * sizeof(pAdvData->AdvertisingData.AdStructures[0]));

        if (pAdvData->AdvertisingData.AdStructures == NULL)
        {
            shell_write("\r\n-->  GATTDB Event: Insufficient memory.");
            return;
        }

        FLib_MemSet(pAdvData->AdvertisingData.AdStructures, 0,
                    mMaxGapAdStructures_d * sizeof(pAdvData->AdvertisingData.AdStructures[0]));
    }

    advIdx = pAdvData->AdvertisingData.NbOfAdStructures;

    // find existing entry
    for (i = 0; i < pAdvData->AdvertisingData.NbOfAdStructures; i++)
    {
        if (pAdvData->AdvertisingData.AdStructures[i].Type == type)
        {
            advIdx = i;
            MEM_BufferFree(pAdvData->AdvertisingData.AdStructures[advIdx].Data);
            break;
        }
    }

    length = strlen(pData);

    switch (type)
    {
        case GAPSetAdvertisingDataRequest_ScanResponseData_AdStructures_Type_gAdShortenedLocalName_c:
        case GAPSetAdvertisingDataRequest_ScanResponseData_AdStructures_Type_gAdCompleteLocalName_c:
        {
            if (!pAdvData->AdvertisingData.AdStructures[advIdx].Data)
            {
                pAdvData->AdvertisingData.AdStructures[advIdx].Data = MEM_BufferAlloc(length);

                if (!pAdvData->AdvertisingData.AdStructures[advIdx].Data)
                {
                    shell_write("\r\n-->  GATTDB Event: Insufficient memory.");
                    return;
                }
            }

            FLib_MemCpy(pAdvData->AdvertisingData.AdStructures[advIdx].Data, pData, length);
        }
        break;

        case GAPSetAdvertisingDataRequest_AdvertisingData_AdStructures_Type_gAdIncomplete128bitServiceList_c:
        case GAPSetAdvertisingDataRequest_AdvertisingData_AdStructures_Type_gAdComplete128bitServiceList_c:
        {
            uint8_t serviceUuid[16];

            if (length != 32)
            {
                shell_write("\r\n-->  GATTDB Event: Bad formatting of 128-bit service value in hex.");
                return;
            }

            /* process 8 hex digits at a time, i.e uint32_t */
            for (i = 0; i < length; i += 8)
            {
                number = NWKU_Revert32(NWKU_AsciiToHex((uint8_t *)pData + i, 8));
                FLib_MemCpy(serviceUuid + i / 2, &number, sizeof(uint32_t));
            }

            if (!pAdvData->AdvertisingData.AdStructures[advIdx].Data)
            {
                pAdvData->AdvertisingData.AdStructures[advIdx].Data = MEM_BufferAlloc(sizeof(serviceUuid));

                if (!pAdvData->AdvertisingData.AdStructures[advIdx].Data)
                {
                    shell_write("\r\n-->  GATTDB Event: Insufficient memory.");
                    return;
                }
            }

            FLib_MemCpy(pAdvData->AdvertisingData.AdStructures[advIdx].Data, serviceUuid, sizeof(serviceUuid));
            length = length / 2;
        }
        break;

        default:
        {
            number = NWKU_AsciiToHex((uint8_t *)pData, length);
            /* halfen length - hex string */
            length = (length % 2) ? (length / 2) + 1 : (length / 2);

            if (!pAdvData->AdvertisingData.AdStructures[advIdx].Data)
            {
                pAdvData->AdvertisingData.AdStructures[advIdx].Data = MEM_BufferAlloc(length);

                if (!pAdvData->AdvertisingData.AdStructures[advIdx].Data)
                {
                    shell_write("\r\n-->  GATTDB Event: Insufficient memory.");
                    return;
                }
            }

            FLib_MemCpy(pAdvData->AdvertisingData.AdStructures[advIdx].Data, &number, length);
        }
        break;
    }

    pAdvData->AdvertisingData.AdStructures[advIdx].Type = type;
    pAdvData->AdvertisingData.AdStructures[advIdx].Length = length;

    if (advIdx == pAdvData->AdvertisingData.NbOfAdStructures)
    {
        pAdvData->AdvertisingData.NbOfAdStructures += 1;
    }

    pAdvData->AdvertisingDataIncluded = TRUE;
}

static void ShellGap_DeviceNameSm(void *param)
{
    switch (deviceNameCmdState)
    {
        case gGetServiceHandle_c:
            if (gLatestHandle != INVALID_HANDLE)
            {
                deviceNameCmdState = gServiceHandleReceived_c;
            }
            else
            {
                deviceNameCmdState = gError_c;
            }

            break;

        case gGetCharValueHandle_c:
            if (gLatestHandle != INVALID_HANDLE)
            {
                deviceNameCmdState = gCharValueHandleReceived_c;
            }
            else
            {
                deviceNameCmdState = gError_c;
            }

            break;

        case gReadAttribute_c:
            deviceNameCmdState = gAttrValueReceived_c;
            break;

        case gWriteAttribute_c:
            deviceNameCmdState = gAttributeWritten_c;
            break;

        default:
            deviceNameCmdState = gError_c;
            break;
    }

    ShellGap_DeviceName(0, NULL);
}

static int8_t ShellGap_DeviceName(uint8_t argc, char *argv[])
{
    if (argc == 1)
    {
        uint8_t length = strlen(argv[0]);

        if (length > mMaxCharValueLength_d)
        {
            shell_write("\r\n-->  Variable length exceeds maximum!");
            return CMD_RET_FAILURE;
        }

        FLib_MemSet(mDeviceName, 0, sizeof(mDeviceName));
        FLib_MemCpy(mDeviceName, argv[0], length);
        mDeviceName[length] = '\0';
        mSetDeviceName = TRUE;
    }

    switch (deviceNameCmdState)
    {
        case gStartState_c:
        {
            gLatestHandle = INVALID_HANDLE;
            deviceNameCmdState = gGetServiceHandle_c;

            GATTDBFindServiceHandleRequest_t req;
            req.StartHandle = 0x0001;  // should be 0x0001 on the first call
            req.UuidType = Uuid16Bits;
            UuidToArray(req.Uuid.Uuid16Bits, gBleSig_GenericAccessProfile_d);

            GATTDBFindServiceHandleRequest(&req, BLE_FSCI_IF);

            OSA_TimeDelay(100);
            ShellGap_DeviceNameSm(argv[0]);

            break;
        }

        case gServiceHandleReceived_c:
        {
            deviceNameCmdState = gGetCharValueHandle_c;

            GATTDBFindCharValueHandleInServiceRequest_t req;
            req.ServiceHandle = gLatestHandle;
            req.UuidType = Uuid16Bits;
            UuidToArray(req.Uuid.Uuid16Bits, gBleSig_GapDeviceName_d);
            GATTDBFindCharValueHandleInServiceRequest(&req, BLE_FSCI_IF);

            gLatestHandle = INVALID_HANDLE;
            OSA_TimeDelay(100);
            ShellGap_DeviceNameSm(NULL);
            break;
        }

        case gCharValueHandleReceived_c:
        {
            if (mSetDeviceName)
            {
                deviceNameCmdState = gWriteAttribute_c;

                GATTDBWriteAttributeRequest_t req;
                req.Handle = gLatestHandle;
                req.ValueLength = strlen(mDeviceName);
                req.Value = (uint8_t *)mDeviceName;

                if (GATTDBWriteAttributeRequest(&req, BLE_FSCI_IF))
                {
                    shell_write("\r\n-->  GATTDB Event: Insufficient memory.");
                    SHELL_NEWLINE();
                }
            }
            else  // get
            {
                deviceNameCmdState = gReadAttribute_c;

                GATTDBReadAttributeRequest_t req;
                req.Handle = gLatestHandle;
                req.MaxBytes = mMaxCharValueLength_d;

                mLatestUuid = gBleSig_GapDeviceName_d;
                GATTDBReadAttributeRequest(&req, BLE_FSCI_IF);
            }

            OSA_TimeDelay(100);
            ShellGap_DeviceNameSm(NULL);
            break;
        }

        case gAttrValueReceived_c:
        {
            deviceNameCmdState = gStartState_c;
            shell_write("\r\n-->  GATTDB Event: Attribute Read ");
            shell_write("\r\n     Value: ");
            shell_write(mDeviceName);
            SHELL_NEWLINE();
            mDeviceName[strlen(mDeviceName)] = '$';
            mDeviceName[strlen(mDeviceName)] = ' ';
            mDeviceName[strlen(mDeviceName) + 1] = '\0';
            shell_change_prompt(mDeviceName);
            break;
        }

        case gAttributeWritten_c:
        {
            deviceNameCmdState = gStartState_c;
            mSetDeviceName = FALSE;

            shell_write("\r\n-->  GATTDB Event: Attribute Written ");
            SHELL_NEWLINE();
            mDeviceName[strlen(mDeviceName)] = '$';
            mDeviceName[strlen(mDeviceName)] = ' ';
            mDeviceName[strlen(mDeviceName) + 1] = '\0';
            shell_change_prompt(mDeviceName);
            break;
        }

        case gError_c:
        {
            deviceNameCmdState = gStartState_c;
            mSetDeviceName = FALSE;
            shell_write("\r\n-->  GATTDB Event: Procedure Error: make sure the GAP service was added! ");
            SHELL_NEWLINE();
            break;
        }

        case gFinalState_c:
        {
            deviceNameCmdState = gStartState_c;
            mSetDeviceName = FALSE;
            break;
        }

        default:
            break;
    }

    return CMD_RET_SUCCESS;
}

static int8_t ShellGap_StartAdvertising(uint8_t argc, char *argv[])
{
    if (argc != 0)
    {
        return CMD_RET_USAGE;
    }

    mIsMaster = FALSE;
    GAPStartAdvertisingRequest(BLE_FSCI_IF);

    return CMD_RET_SUCCESS;
}

static int8_t ShellGap_StopAdvertising(uint8_t argc, char *argv[])
{
    if (argc != 0)
    {
        return CMD_RET_USAGE;
    }

    GAPStopAdvertisingRequest(BLE_FSCI_IF);

    return CMD_RET_SUCCESS;
}

static int8_t ShellGap_SetAdvertisingParameters(uint8_t argc, char *argv[])
{
    uint8_t i;
    bool_t bValidCmd = FALSE;

    if (argc == 0)
    {
        shell_write("\r\n-->  Advertising Parameters:");
        shell_write("\r\n    -->  Advertising Interval: ");
        shell_printf("%d", mAdvParams.MaxInterval * 625 / 1000);
        shell_write(" ms");
        shell_write("\r\n    -->  Advertising Type: ");

        switch (mAdvParams.AdvertisingType)
        {
            case GAPSetAdvertisingParametersRequest_AdvertisingType_gConnectableUndirected_c:
            {
                shell_write("ADV_IND");
            }
            break;

            case GAPSetAdvertisingParametersRequest_AdvertisingType_gDirectedHighDutyCycle_c:
            {
                shell_write("ADV_IND");
            }
            break;

            case GAPSetAdvertisingParametersRequest_AdvertisingType_gNonConnectable_c:
            {
                shell_write("ADV_NONCON_IND");
            }
            break;

            case GAPSetAdvertisingParametersRequest_AdvertisingType_gScannable_c:
            {
                shell_write("ADV_SCAN_IND");
            }
            break;

            case GAPSetAdvertisingParametersRequest_AdvertisingType_gDirectedLowDutyCycle_c:
            {
                shell_write("ADV_DIRECT");
            }
            break;

            default:
                break;
        }

        SHELL_NEWLINE();
        return CMD_RET_SUCCESS;
    }

    for (i = 0; i < argc; i += 2)
    {
        if (!strcmp((char *)argv[i], "-interval") && ((i + 1) < argc))
        {
            uint16_t interval = atoi(argv[i + 1]) * 1000 / 625;
            mAdvParams.MaxInterval = interval;
            mAdvParams.MinInterval = interval;
            bValidCmd = TRUE;
        }

        if (!strcmp((char *)argv[i], "-type") && ((i + 1) < argc))
        {
            uint8_t advType = atoi(argv[i + 1]);
            mAdvParams.AdvertisingType = (GAPSetAdvertisingParametersRequest_AdvertisingType_t)advType;
            bValidCmd = TRUE;
        }
    }

    if (bValidCmd)
    {
        GAPSetAdvertisingParametersRequest(&mAdvParams, BLE_FSCI_IF);
        return CMD_RET_SUCCESS;
    }
    else
    {
        return CMD_RET_USAGE;
    }
}

static void ShellGap_AppendScanData
(
    GAPSetAdvertisingDataRequest_t *pAdvData,
    GAPSetAdvertisingDataRequest_ScanResponseData_AdStructures_Type_t type,
    char *pData
)
{
    uint8_t i, advIdx = 0;
    uint8_t length;
    uint32_t number;

    if (pAdvData->ScanResponseData.AdStructures == NULL)
    {
        pAdvData->ScanResponseData.AdStructures =
            MEM_BufferAlloc(mMaxGapAdStructures_d * sizeof(pAdvData->ScanResponseData.AdStructures[0]));

        if (pAdvData->ScanResponseData.AdStructures == NULL)
        {
            shell_write("\r\n-->  GATTDB Event: Insufficient memory.");
            return;
        }
    }

    advIdx = pAdvData->ScanResponseData.NbOfAdStructures;

    for (i = 0; i < pAdvData->ScanResponseData.NbOfAdStructures; i++)
    {
        if (pAdvData->ScanResponseData.AdStructures[i].Type == type)
        {
            advIdx = i;
            break;
        }
    }

    length = strlen(pData);

    switch (type)
    {
        case GAPSetAdvertisingDataRequest_ScanResponseData_AdStructures_Type_gAdShortenedLocalName_c:
        case GAPSetAdvertisingDataRequest_ScanResponseData_AdStructures_Type_gAdCompleteLocalName_c:
        {
            pAdvData->ScanResponseData.AdStructures[advIdx].Data = MEM_BufferAlloc(length);

            if (!pAdvData->ScanResponseData.AdStructures[advIdx].Data)
            {
                shell_write("\r\n-->  GATTDB Event: Insufficient memory.");
                return;
            }

            FLib_MemCpy(pAdvData->ScanResponseData.AdStructures[advIdx].Data, pData, length);
        }
        break;

        default:
        {
            number = NWKU_AsciiToHex((uint8_t *)pData, length);
            /* halfen length - hex string */
            length = (length % 2) ? (length / 2) + 1 : (length / 2);

            pAdvData->ScanResponseData.AdStructures[advIdx].Data = MEM_BufferAlloc(length);

            if (!pAdvData->ScanResponseData.AdStructures[i].Data)
            {
                shell_write("\r\n-->  GATTDB Event: Insufficient memory.");
                return;
            }

            FLib_MemCpy(pAdvData->ScanResponseData.AdStructures[advIdx].Data, &number, length);
        }
        break;
    }

    pAdvData->ScanResponseData.AdStructures[advIdx].Type = type;
    pAdvData->ScanResponseData.AdStructures[advIdx].Length = length;
    pAdvData->ScanResponseData.NbOfAdStructures += 1;
    pAdvData->ScanResponseDataIncluded = TRUE;
}

static int8_t ShellGap_StartScanning(uint8_t argc, char *argv[])
{
    if (argc != 0)
    {
        return CMD_RET_USAGE;
    }

    for (uint8_t i = 0; i < mScannedDevicesCount; i++)
    {
        MEM_BufferFree(mScannedDevices[i].Data);
    }

    FLib_MemSet(mScannedDevices, 0, sizeof(mScannedDevices));
    mScannedDevicesCount = 0;

    GAPStartScanningRequest(&mAppScanParams, BLE_FSCI_IF);
    mScanningOn = TRUE;
    mIsMaster = TRUE;

    return CMD_RET_SUCCESS;
}

static int8_t ShellGap_StopScanning(uint8_t argc, char *argv[])
{
    if (argc != 0)
    {
        return CMD_RET_USAGE;
    }

    GAPStopScanningRequest(BLE_FSCI_IF);
    mScanningOn = FALSE;

    return CMD_RET_SUCCESS;
}

static int8_t ShellGap_SetScanParameters(uint8_t argc, char *argv[])
{
    uint8_t i;
    bool_t bValidCmd;

    if (argc == 0)
    {
        shell_write("\r\n-->  Scan Parameters:");
        shell_write("\r\n    -->  Scan Interval: ");
        shell_printf("%d", mAppScanParams.ScanningParameters.Interval * 625 / 1000);
        shell_write(" ms");
        shell_write("\r\n    -->  Scan Window: ");
        shell_printf("%d", mAppScanParams.ScanningParameters.Window * 625 / 1000);
        shell_write(" ms");
        shell_write("\r\n    -->  Scan Type: ");
        (mAppScanParams.ScanningParameters.Type) ? shell_write("ACTIVE") : shell_write("PASSIVE");
        SHELL_NEWLINE();
        return CMD_RET_SUCCESS;
    }

    for (i = 0; i < argc; i += 2)
    {
        if (!strcmp((char *)argv[i], "-interval") && ((i + 1) < argc))
        {
            uint16_t interval = atoi(argv[i + 1]) * 1000 / 625;
            mAppScanParams.ScanningParameters.Interval = interval;
            mConnReqParams.ScanInterval = interval;
            bValidCmd = TRUE;
        }

        if (!strcmp((char *)argv[i], "-window") && ((i + 1) < argc))
        {
            uint16_t window = atoi(argv[i + 1]) * 1000 / 625;
            mAppScanParams.ScanningParameters.Window = window;
            mConnReqParams.ScanWindow = window;
            bValidCmd = TRUE;
        }

        if (!strcmp((char *)argv[i], "-type") && ((i + 1) < argc))
        {
            uint8_t scanType = atoi(argv[i + 1]);
            mAppScanParams.ScanningParameters.Type = (GAPStartScanningRequest_ScanningParameters_Type_t)scanType;
            bValidCmd = TRUE;
        }
    }

    if (bValidCmd)
    {
        return CMD_RET_SUCCESS;
    }
    else
    {
        return CMD_RET_USAGE;
    }
}

static int8_t ShellGap_ChangeScanData(uint8_t argc, char *argv[])
{
    switch (argc)
    {
        case 0:
        {
            shell_printf("\r\n-->  Scan Response Data Included -> %d", mAppAdvertisingData.ScanResponseDataIncluded);
            shell_printf("\r\n-->  Scan Response Data Structures -> %d", mAppAdvertisingData.ScanResponseData.NbOfAdStructures);
            return CMD_RET_SUCCESS;
        }
        break;

        case 1:
        {
            if (!strcmp((char *)argv[0], "-clear"))
            {
                // free allocated memory for scan data
                for (uint8_t i = 0; i < mAppAdvertisingData.ScanResponseData.NbOfAdStructures; i++)
                {
                    MEM_BufferFree(mAppAdvertisingData.ScanResponseData.AdStructures[i].Data);
                }

                MEM_BufferFree(mAppAdvertisingData.ScanResponseData.AdStructures);

                // set to 0
                FLib_MemSet(&mAppAdvertisingData.ScanResponseData, 0, sizeof(mAppAdvertisingData.ScanResponseData));
                mAppAdvertisingData.ScanResponseDataIncluded = FALSE;

                GAPSetAdvertisingDataRequest(&mAppAdvertisingData, BLE_FSCI_IF);
                shell_write("\r\n-->  Scan Data Erased.");
                return CMD_RET_SUCCESS;
            }
        }
        break;

        default:
        {
            GAPSetAdvertisingDataRequest_ScanResponseData_AdStructures_Type_t advType =
                (GAPSetAdvertisingDataRequest_ScanResponseData_AdStructures_Type_t)atoi(argv[0]);
            ShellGap_AppendScanData(&mAppAdvertisingData, advType, argv[1]);
            GAPSetAdvertisingDataRequest(&mAppAdvertisingData, BLE_FSCI_IF);
            return CMD_RET_SUCCESS;
        }
        break;
    }

    return CMD_RET_USAGE;
}

static int8_t ShellGap_Connect(uint8_t argc, char *argv[])
{
    uint8_t deviceId;

    if (argc != 1)
    {
        return CMD_RET_USAGE;
    }

    deviceId = (uint8_t)atoi(argv[0]);

    if (deviceId > mScannedDevicesCount)
    {
        shell_write("Device ID does not exist!");
        SHELL_NEWLINE();
        return CMD_RET_USAGE;
    }

    mIsMaster = TRUE;

    mConnReqParams.ScanInterval = mAppScanParams.ScanningParameters.Interval;
    mConnReqParams.ScanWindow = mAppScanParams.ScanningParameters.Window;
    mConnReqParams.PeerAddressType = (GAPConnectRequest_PeerAddressType_t)mScannedDevices[deviceId].AddressType;
    FLib_MemCpy(mConnReqParams.PeerAddress, mScannedDevices[deviceId].Address, sizeof(mConnReqParams.PeerAddress));

    /* Stop scanning if it's on */
    if (mScanningOn)
    {
        GAPStopScanningRequest(BLE_FSCI_IF);
    }

    GAPConnectRequest(&mConnReqParams, BLE_FSCI_IF);

    return CMD_RET_SUCCESS;
}

static int8_t ShellGap_SetConnectionParameters(uint8_t argc, char *argv[])
{
    uint8_t i;
    bool_t bValidCmd = FALSE;

    if (argc == 0)
    {
        bValidCmd = TRUE;
    }

    for (i = 0; i < argc; i += 2)
    {
        if (!strcmp((char *)argv[i], "-interval") && ((i + 1) < argc))
        {
            uint16_t interval = atoi(argv[i + 1]) * 8 / 10;
            mConnReqParams.ConnIntervalMin = interval;
            mConnReqParams.ConnIntervalMax = interval;
            bValidCmd = TRUE;
        }

        if (!strcmp((char *)argv[i], "-latency") && ((i + 1) < argc))
        {
            uint16_t latency = atoi(argv[i + 1]);
            mConnReqParams.ConnLatency = latency;
            bValidCmd = TRUE;
        }

        if (!strcmp((char *)argv[i], "-timeout") && ((i + 1) < argc))
        {
            uint16_t timeout = atoi(argv[i + 1]) / 10;
            mConnReqParams.SupervisionTimeout = timeout;
            bValidCmd = TRUE;
        }
    }

    if (bValidCmd)
    {
        shell_write("\r\n-->  Connection Parameters:");
        shell_write("\r\n    -->  Connection Interval: ");
        shell_printf("%d", mConnReqParams.ConnIntervalMax * 10 / 8);
        shell_write(" ms");

        shell_write("\r\n    -->  Connection Latency: ");
        shell_printf("%d", mConnReqParams.ConnLatency);

        shell_write("\r\n    -->  Supervision Timeout: ");
        shell_printf("%d", mConnReqParams.SupervisionTimeout * 10);
        shell_write(" ms\r\n");

        SHELL_NEWLINE();
        return CMD_RET_SUCCESS;
    }
    else
    {
        return CMD_RET_USAGE;
    }
}

static int8_t ShellGap_Disconnect(uint8_t argc, char *argv[])
{
    if (argc != 0)
    {
        return CMD_RET_USAGE;
    }

    GAPDisconnectRequest_t req;
    req.DeviceId = gPeerDeviceId;  // the connected peer to disconnect from
    GAPDisconnectRequest(&req, BLE_FSCI_IF);

    return CMD_RET_SUCCESS;
}

static int8_t ShellGap_UpdateConnection(uint8_t argc, char *argv[])
{
    GAPUpdateConnectionParametersRequest_t req;

    if (argc != 4)
    {
        return CMD_RET_USAGE;
    }

    req.DeviceId = gPeerDeviceId;
    req.IntervalMin = atoi(argv[0]) * 8 / 10;
    req.IntervalMax = atoi(argv[1]) * 8 / 10;
    req.SlaveLatency = atoi(argv[2]);
    req.TimeoutMultiplier = atoi(argv[3]) / 10;
    req.MinCeLength = mConnReqParams.ConnEventLengthMin;
    req.MaxCeLength = mConnReqParams.ConnEventLengthMax;

    GAPUpdateConnectionParametersRequest(&req, BLE_FSCI_IF);
    return CMD_RET_SUCCESS;
}

static int8_t ShellGap_Pair(uint8_t argc, char *argv[])
{
    if (argc != 0)
    {
        return CMD_RET_USAGE;
    }

    if (gPeerDeviceId == gInvalidDeviceId_c)
    {
        shell_write("\r\n-->  Please connect the node first...");
        return CMD_RET_FAILURE;
    }

    shell_write("\r\n-->  Pairing...\r\n");

    if (mIsMaster)
    {
        mPairingParameters.DeviceId = gPeerDeviceId;
        GAPPairRequest(&mPairingParameters, BLE_FSCI_IF);
    }
    else
    {
        GAPSendSlaveSecurityRequestRequest_t req;
        req.DeviceId = gPeerDeviceId;
        req.BondAfterPairing = mPairingParameters.PairingParameters.WithBonding;
        req.SecurityModeLevel = (GAPSendSlaveSecurityRequestRequest_SecurityModeLevel_t)mPairingParameters.PairingParameters.SecurityModeAndLevel;
        GAPSendSlaveSecurityRequestRequest(&req, BLE_FSCI_IF);
    }

    return CMD_RET_SUCCESS;
}

static int8_t ShellGap_PairCfg(uint8_t argc, char *argv[])
{
    uint8_t i;
    bool_t bValidCmd = FALSE;

    if (argc == 0)
    {
        bValidCmd = TRUE;
    }

    for (i = 0; i < argc; i += 2)
    {
        if (!strcmp((char *)argv[i], "-usebonding") && ((i + 1) < argc))
        {
            uint8_t usebonding = atoi(argv[i + 1]);
            mPairingParameters.PairingParameters.WithBonding = usebonding ? TRUE : FALSE;
            bValidCmd = TRUE;
        }

        if (!strcmp((char *)argv[i], "-seclevel") && ((i + 1) < argc))
        {
            uint8_t level = strtoul(argv[i + 1], NULL, 16);

            if ((level & 0x0F) <= gSecurityLevel_WithMitmProtection_c &&
                (level & 0xF0) <= gSecurityMode_2_c)
            {
                mPairingParameters.PairingParameters.SecurityModeAndLevel =
                    (GAPPairRequest_PairingParameters_SecurityModeAndLevel_t)level;
                bValidCmd = TRUE;
            }
        }

        if (!strcmp((char *)argv[i], "-keyflags") && ((i + 1) < argc))
        {
            uint8_t flags = strtoul(argv[i + 1], NULL, 16);
            mPairingParameters.PairingParameters.CentralKeys =
                (GAPPairRequest_PairingParameters_CentralKeys_t)(flags & 0x07);
            mPairingParameters.PairingParameters.PeripheralKeys =
                (GAPPairRequest_PairingParameters_PeripheralKeys_t)(flags & 0x07);
            bValidCmd = TRUE;
        }
    }

    if (bValidCmd)
    {
        shell_write("\r\n-->  Pairing Configuration:");
        shell_write("\r\n    -->  Use Bonding: ");
        shell_printf("%d", mPairingParameters.PairingParameters.WithBonding);

        shell_write("\r\n    -->  SecurityLevel: ");
        shell_printf("%02x", mPairingParameters.PairingParameters.SecurityModeAndLevel);

        shell_write("\r\n    -->  Flags: ");
        shell_printf("%02x", mPairingParameters.PairingParameters.CentralKeys);

        SHELL_NEWLINE();
        return CMD_RET_SUCCESS;
    }
    else
    {
        return CMD_RET_USAGE;
    }
}

static int8_t ShellGap_EnterPasskey(uint8_t argc, char *argv[])
{
    GAPEnterPasskeyRequest_t req;

    if (argc != 1)
    {
        return CMD_RET_USAGE;
    }

    req.DeviceId = gPeerDeviceId;
    req.Passkey = atoi(argv[0]);

    GAPEnterPasskeyRequest(&req, BLE_FSCI_IF);

    return CMD_RET_SUCCESS;
}

static int8_t ShellGap_Bonds(uint8_t argc, char *argv[])
{
    shell_write("Not implemented");
    GAPRemoveBondRequest_t rmbond;
    for(uint8_t i=0; i< listofbondeddevice;i++)
    {
    	rmbond.NvmIndex = i;
    	GAPRemoveBondRequest(&rmbond,BLE_FSCI_IF);
    }
    return CMD_RET_SUCCESS;
}

/************************************************************************************
* GATT
************************************************************************************/
static int32_t ShellGatt_Command
(
    shell_handle_t context,
    int32_t argc,
    char **argv
)
{
    uint8_t i;

    if (argc < 2)
    {
        return CMD_RET_USAGE;
    }

    for (i = 0; i < mShellGattCmdsCount_c; i++)
    {
        if (!strcmp((char *)argv[1], mGattShellCmds[i].name))
        {
            return mGattShellCmds[i].cmd(argc - 2, (char **)(&argv[2]));
        }
    }

    return CMD_RET_USAGE;
}

static int8_t ShellGatt_Discover(uint8_t argc, char *argv[])
{
    if (gPeerDeviceId == gInvalidDeviceId_c)
    {
        shell_write("\n\r-->  Please connect the node first...");
        return CMD_RET_FAILURE;
    }

    switch (argc)
    {
        case 1:
        {
            if (!strcmp((char *)argv[0], "-all"))
            {
                GATTClientDiscoverAllPrimaryServicesRequest_t req;
                req.DeviceId = gPeerDeviceId;
                req.MaxNbOfServices = mMaxServicesCount_d;
                GATTClientDiscoverAllPrimaryServicesRequest(&req, BLE_FSCI_IF);

                return CMD_RET_SUCCESS;
            }
        }
        break;

        case 2:
        {
            if (!strcmp((char *)argv[0], "-service"))
            {
                GATTClientDiscoverPrimaryServicesByUuidRequest_t req;
                uint16_t uuid16 = strtoul(argv[1], NULL, 16);

                req.DeviceId = gPeerDeviceId;
                req.UuidType = Uuid16Bits;
                UuidToArray(req.Uuid.Uuid16Bits, uuid16);
                req.MaxNbOfServices = mMaxServicesCount_d;

                GATTClientDiscoverPrimaryServicesByUuidRequest(&req, BLE_FSCI_IF);

                return CMD_RET_SUCCESS;
            }
        }
        break;

        default:
            break;
    }

    return CMD_RET_USAGE;
}

static int8_t ShellGatt_Read(uint8_t argc, char *argv[])
{
    GATTClientReadCharacteristicValueRequest_t req = { 0 };

    if (gPeerDeviceId == gInvalidDeviceId_c)
    {
        shell_write("\n\r-->  Please connect the node first...");
        return CMD_RET_FAILURE;
    }

    if (argc != 1)
    {
        return CMD_RET_USAGE;
    }

    req.DeviceId = gPeerDeviceId;
    req.Characteristic.Value.Handle = atoi(argv[0]);
    req.MaxReadBytes = 50;

    GATTClientReadCharacteristicValueRequest(&req, BLE_FSCI_IF);

    return CMD_RET_SUCCESS;
}

static int8_t ShellGatt_WriteRsp(uint8_t argc, char *argv[])
{
    return ShellGatt_Write(argc, argv, FALSE);
}

static int8_t ShellGatt_WriteCmd(uint8_t argc, char *argv[])
{
    return ShellGatt_Write(argc, argv, TRUE);
}

static int8_t ShellGatt_Write(uint8_t argc, char *argv[], bool_t bNoRsp)
{
    uint8_t length = 0;
    GATTClientWriteCharacteristicValueRequest_t req = { 0 };

    if (gPeerDeviceId == gInvalidDeviceId_c)
    {
        shell_write("\n\r-->  Please connect the node first...");
        return CMD_RET_FAILURE;
    }

    if (argc != 2)
    {
        return CMD_RET_USAGE;
    }

    length = NWKU_ParseHexValue(argv[1]);

    if (length > mMaxCharValueLength_d)
    {
        shell_write("\n\r-->  Variable length exceeds maximum!");
        return CMD_RET_FAILURE;
    }

    req.DeviceId = gPeerDeviceId;
    req.Characteristic.Value.Handle = atoi(argv[0]);
    req.Characteristic.Value.UuidType = Uuid16Bits;
    req.ValueLength = length;
    req.Value = (uint8_t *)argv[1];

    req.WithoutResponse = bNoRsp;
    req.SignedWrite = FALSE;
    req.ReliableLongCharWrites = FALSE;

    GATTClientWriteCharacteristicValueRequest(&req, BLE_FSCI_IF);
    GATTClientRegisterNotificationCallbackRequest(BLE_FSCI_IF);

    return CMD_RET_SUCCESS;
}

static int8_t ShellGatt_Notify(uint8_t argc, char *argv[])
{
    GATTDBFindCccdHandleForCharValueHandleRequest_t req;

    if (argc != 1)
    {
        return CMD_RET_USAGE;
    }

    if (gPeerDeviceId == gInvalidDeviceId_c)
    {
        shell_write("\r\n-->  Please connect the node first...\r\n");
        return CMD_RET_FAILURE;
    }

    mNotifyHandle = atoi(argv[0]);

    /* Get handle of CCCD */
    req.CharValueHandle = mNotifyHandle;
    GATTDBFindCccdHandleForCharValueHandleRequest(&req, BLE_FSCI_IF);

    OSA_TimeDelay(100);
    ShellGatt_NotifySm((void *)gGetCccd_c);

    return CMD_RET_SUCCESS;
}

static void ShellGatt_NotifySm(void *param)
{
    if ((gattNotifyState_t)((uint32_t)param) == gGetCccd_c)
    {
        if (gCccdHandle == INVALID_HANDLE)
        {
            shell_write("\r\n-->  No CCCD found!\r\n");
        }
        else
        {
            GAPCheckNotificationStatusRequest_t req;
            req.DeviceId = gPeerDeviceId;
            req.Handle = gCccdHandle;
            GAPCheckNotificationStatusRequest(&req, BLE_FSCI_IF);

            gCccdHandle = INVALID_HANDLE;

            // get to next state
            OSA_TimeDelay(100);
            ShellGatt_NotifySm((void *)gGetNotificationStatus_c);
        }
    }

    else if ((gattNotifyState_t)((uint32_t)param) == gGetNotificationStatus_c)
    {
        if (!gIsNotificationActive)
        {
            shell_write("\r\n-->  CCCD is not set!\r\n");
        }
        else
        {
            if (!mbIndicate)
            {
                GATTServerSendNotificationRequest_t req;
                req.DeviceId = gPeerDeviceId;
                req.Handle = mNotifyHandle;
                GATTServerSendNotificationRequest(&req, BLE_FSCI_IF);
            }
            else
            {
                GATTServerSendIndicationRequest_t req;
                req.DeviceId = gPeerDeviceId;
                req.Handle = mNotifyHandle;
                GATTServerSendIndicationRequest(&req, BLE_FSCI_IF);

                mbIndicate = FALSE;
            }

            gIsNotificationActive = FALSE;
        }
    }
}

static int8_t ShellGatt_Indicate(uint8_t argc, char *argv[])
{
    mbIndicate = TRUE;
    return ShellGatt_Notify(argc, argv);
}

static char *ShellGatt_GetServiceName(uint16_t uuid16)
{
    for (uint8_t i = 0; i < NumberOfElements(mGattServices); i++)
    {
        if (mGattServices[i].uuid == uuid16)
        {
            return mGattServices[i].name;
        }
    }

    return "N/A";
}

static char *ShellGatt_GetCharacteristicName(uint16_t uuid16)
{
    for (uint8_t i = 0; i < NumberOfElements(mGattChars); i++)
    {
        if (mGattChars[i].uuid == uuid16)
        {
            return mGattChars[i].name;
        }
    }

    return "N/A";
}

static char *ShellGatt_GetDescriptorName(uint16_t uuid16)
{
    for (uint8_t i = 0; i < NumberOfElements(mGattDescriptors); i++)
    {
        if (mGattDescriptors[i].uuid == uuid16)
        {
            return mGattDescriptors[i].name;
        }
    }

    return "N/A";
}

/************************************************************************************
* GATT-DB
************************************************************************************/
static int32_t ShellGattDb_Command
(
    shell_handle_t context,
    int32_t argc,
    char **argv
)
{
    uint8_t i;

    if (argc < 2)
    {
        return CMD_RET_USAGE;
    }

    for (i = 0; i < mShellGattDbCmdsCount_c; i++)
    {
        if (!strcmp((char *)argv[1], mGattDbShellCmds[i].name))
        {
            return mGattDbShellCmds[i].cmd(argc - 2, (char **)(&argv[2]));
        }
    }

    return CMD_RET_USAGE;
}

static int8_t ShellGattDb_Read(uint8_t argc, char *argv[])
{
    GATTDBReadAttributeRequest_t req;

    if (argc != 1)
    {
        return CMD_RET_USAGE;
    }

    req.Handle = atoi(argv[0]);
    req.MaxBytes = mMaxCharValueLength_d;

    GATTDBReadAttributeRequest(&req, BLE_FSCI_IF);

    return CMD_RET_SUCCESS;
}

static int8_t ShellGattDb_Write(uint8_t argc, char *argv[])
{
    uint16_t length, handle;
    GATTDBWriteAttributeRequest_t req;

    if (argc != 2)
    {
        return CMD_RET_USAGE;
    }

    handle = atoi(argv[0]);
    length = NWKU_ParseHexValue(argv[1]);

    if (length > mMaxCharValueLength_d)
    {
        shell_write("\n\r-->  Variable length exceeds maximum!");
        return CMD_RET_FAILURE;
    }

    req.Handle = handle;
    req.ValueLength = length;
    req.Value = (uint8_t *)argv[1];

    if (GATTDBWriteAttributeRequest(&req, BLE_FSCI_IF))
    {
        shell_write("\n\r-->  GATTDB Event: Insufficient memory. ");
        SHELL_NEWLINE();
    }

    return CMD_RET_SUCCESS;
}

static int8_t ShellGattDb_AddService(uint8_t argc, char *argv[])
{
    uint16_t serviceUuid = 0, length;

    if (argc != 1)
    {
        return CMD_RET_USAGE;
    }

    if (!strcmp(argv[0], "gatt"))
    {
        ShellGattDb_AddServiceGatt();
    }
    else if (!strcmp(argv[0], "gap"))
    {
        ShellGattDb_AddServiceGap();
    }
    else if (!strcmp(argv[0], "hr"))
    {
        ShellGattDb_AddServiceHeartRate();
    }
    else if (!strcmp(argv[0], "battery"))
    {
        ShellGattDb_AddServiceBattery();
    }
    else if (!strcmp(argv[0], "devinfo"))
    {
        ShellGattDb_AddServiceDeviceInfo();
    }
    else
    {
        length = NWKU_ParseHexValue(argv[0]);

        if (length == 2)
        {
            FLib_MemCpy(&serviceUuid, argv[0], sizeof(uint16_t));
            ShellGattDb_AddPrimaryServiceDecl(serviceUuid);
        }
        else
        {
            return CMD_RET_USAGE;
        }
    }

    return CMD_RET_SUCCESS;
}

static int8_t ShellGattDb_Erase(uint8_t argc, char *argv[])
{
    if (argc != 0)
    {
        return CMD_RET_USAGE;
    }

    GATTDBDynamicReleaseDatabaseRequest(BLE_FSCI_IF);

    return CMD_RET_SUCCESS;
}

static void ShellGattDb_AddPrimaryServiceDecl(uint16_t serviceUuid)
{
    GATTDBDynamicAddPrimaryServiceDeclarationRequest_t req;
    req.UuidType = Uuid16Bits;
    UuidToArray(req.Uuid.Uuid16Bits, serviceUuid);

    if (GATTDBDynamicAddPrimaryServiceDeclarationRequest(&req, BLE_FSCI_IF) != MEM_SUCCESS_c)
    {
        shell_write("\n\r-->  GATTDB Event: Insufficient memory. ");
        SHELL_NEWLINE();
    }
}

static void ShellGattDb_AddPrimaryServiceDecl128(uint8_t *serviceUuid)
{
    GATTDBDynamicAddPrimaryServiceDeclarationRequest_t req;
    req.UuidType = Uuid128Bits;
    FLib_MemCpy(req.Uuid.Uuid128Bits, serviceUuid, 16);

    if (GATTDBDynamicAddPrimaryServiceDeclarationRequest(&req, BLE_FSCI_IF) != MEM_SUCCESS_c)
    {
        shell_write("\n\r-->  GATTDB Event: Insufficient memory. ");
        SHELL_NEWLINE();
    }
}

static void ShellGattDb_AddCharDeclValue
(
    uint16_t charUuid,
    uint8_t properties,
    uint16_t length,
    uint8_t *value,
    GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_t permissions,
    bool_t varlen
)
{
    GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_t req;
    req.UuidType = Uuid16Bits;
    UuidToArray(req.Uuid.Uuid16Bits, charUuid);
    req.CharacteristicProperties = properties;

    if (varlen)
    {
        req.MaxValueLength = length < mMaxCharValueLength_d ? mMaxCharValueLength_d : length;
    }
    else
    {
        req.MaxValueLength = 0;  // for fixed lengths this must be set to 0
    }

    req.InitialValueLength = length;
    req.InitialValue = value;
    req.ValueAccessPermissions = permissions;

    if (GATTDBDynamicAddCharacteristicDeclarationAndValueRequest(&req, BLE_FSCI_IF) != MEM_SUCCESS_c)
    {
        shell_write("\n\r-->  GATTDB Event: Insufficient memory. ");
        SHELL_NEWLINE();
    }
}

static void ShellGattDb_AddCharDeclValue128
(
    uint8_t *charUuid,
    uint8_t properties,
    uint16_t length,
    uint8_t *value,
    GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_t permissions,
    bool_t varlen
)
{
    GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_t req;
    req.UuidType = Uuid128Bits;
    FLib_MemCpy(req.Uuid.Uuid128Bits, charUuid, 16);
    req.CharacteristicProperties = properties;

    if (varlen)
    {
        req.MaxValueLength = length < mMaxCharValueLength_d ? mMaxCharValueLength_d : length;
    }
    else
    {
        req.MaxValueLength = 0;  // for fixed lengths this must be set to 0
    }

    req.InitialValueLength = length;
    req.InitialValue = value;
    req.ValueAccessPermissions = permissions;

    if (GATTDBDynamicAddCharacteristicDeclarationAndValueRequest(&req, BLE_FSCI_IF) != MEM_SUCCESS_c)
    {
        shell_write("\n\r-->  GATTDB Event: Insufficient memory. ");
        SHELL_NEWLINE();
    }
}

static void ShellGattDb_AddCharDesc
(
    uint16_t charUuid,
    uint16_t length,
    uint8_t *value,
    GATTDBDynamicAddCharacteristicDescriptorRequest_DescriptorAccessPermissions_t permissions
)
{
    GATTDBDynamicAddCharacteristicDescriptorRequest_t req;
    req.UuidType = Uuid16Bits;
    UuidToArray(req.Uuid.Uuid16Bits, charUuid);
    req.DescriptorValueLength = length;
    req.Value = value;
    req.DescriptorAccessPermissions = permissions;

    if (GATTDBDynamicAddCharacteristicDescriptorRequest(&req, BLE_FSCI_IF) != MEM_SUCCESS_c)
    {
        shell_write("\n\r-->  GATTDB Event: Insufficient memory. ");
        SHELL_NEWLINE();
    }
}

static void ShellGattDb_AddServiceGatt(void)
{
    ShellGattDb_AddPrimaryServiceDecl(gBleSig_GenericAttributeProfile_d);

    ShellGattDb_AddCharDeclValue(
        gBleSig_GattServiceChanged_d,
        gRead_c | gNotify_c,
        sizeof(mGattDbDynamic_GattServiceChangedInitValue),
        mGattDbDynamic_GattServiceChangedInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionNone_c,
        FALSE
    );

    GATTDBDynamicAddCccdRequest(BLE_FSCI_IF);
}

static void ShellGattDb_AddServiceGap(void)
{
    ShellGattDb_AddPrimaryServiceDecl(gBleSig_GenericAccessProfile_d);

    ShellGattDb_AddCharDeclValue(
        gBleSig_GapDeviceName_d,
        gRead_c,
        sizeof(mGattDbDynamic_GapServiceDeviceNameInitValue),
        mGattDbDynamic_GapServiceDeviceNameInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        TRUE
    );

    ShellGattDb_AddCharDeclValue(
        gBleSig_GapAppearance_d,
        gRead_c,
        sizeof(mGattDbDynamic_GapServiceAppearanceInitValue),
        mGattDbDynamic_GapServiceAppearanceInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        FALSE
    );

    ShellGattDb_AddCharDeclValue(
        gBleSig_GapPpcp_d,
        gRead_c,
        sizeof(mGattDbDynamic_GapServicePpcpInitValue),
        mGattDbDynamic_GapServicePpcpInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        FALSE
    );

    GATTDBDynamicAddCccdRequest(BLE_FSCI_IF);
}

static void ShellGattDb_AddServiceHeartRate(void)
{
    ShellGattDb_AddPrimaryServiceDecl(gBleSig_HeartRateService_d);

    ShellGattDb_AddCharDeclValue(
        gBleSig_HrMeasurement_d,
        gNotify_c,
        sizeof(mGattDbDynamic_HRServiceHrMeasurementInitValue),
        mGattDbDynamic_HRServiceHrMeasurementInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionNone_c,
        TRUE
    );
    GATTDBDynamicAddCccdRequest(BLE_FSCI_IF);

    ShellGattDb_AddCharDeclValue(
        gBleSig_BodySensorLocation_d,
        gRead_c,
        sizeof(mGattDbDynamic_HRServiceBodySensorLocationInitValue),
        mGattDbDynamic_HRServiceBodySensorLocationInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        FALSE
    );

    ShellGattDb_AddCharDeclValue(
        gBleSig_HrControlPoint_d,
        gWrite_c,
        sizeof(mGattDbDynamic_HRServiceHrControlPointInitValue),
        mGattDbDynamic_HRServiceHrControlPointInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagWritable_c,
        FALSE
    );
}

static void ShellGattDb_AddServiceBattery(void)
{
    ShellGattDb_AddPrimaryServiceDecl(gBleSig_BatteryService_d);

    ShellGattDb_AddCharDeclValue(
        gBleSig_BatteryLevel_d,
        gRead_c | gNotify_c,
        sizeof(mGattDbDynamic_BServiceLevelInitValue),
        mGattDbDynamic_BServiceLevelInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        FALSE
    );

    ShellGattDb_AddCharDesc(
        gBleSig_CharPresFormatDescriptor_d,
        sizeof(mGattDbDynamic_BServiceCharPresFormatDescriptorInitValue),
        mGattDbDynamic_BServiceCharPresFormatDescriptorInitValue,
        GATTDBDynamicAddCharacteristicDescriptorRequest_DescriptorAccessPermissions_gPermissionFlagReadable_c
    );

    GATTDBDynamicAddCccdRequest(BLE_FSCI_IF);
}

static void ShellGattDb_AddServiceDeviceInfo(void)
{
    ShellGattDb_AddPrimaryServiceDecl(gBleSig_DeviceInformationService_d);

    ShellGattDb_AddCharDeclValue(
        gBleSig_ManufacturerNameString_d,
        gRead_c,
        sizeof(mGattDbDynamic_DIServiceManufNameInitValue),
        mGattDbDynamic_DIServiceManufNameInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        FALSE
    );
    ShellGattDb_AddCharDeclValue(
        gBleSig_ModelNumberString_d,
        gRead_c,
        sizeof(mGattDbDynamic_DIServiceModelNbInitValue),
        mGattDbDynamic_DIServiceModelNbInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        FALSE
    );
    ShellGattDb_AddCharDeclValue(
        gBleSig_SerialNumberString_d,
        gRead_c,
        sizeof(mGattDbDynamic_DIServiceSerialNoInitValue),
        mGattDbDynamic_DIServiceSerialNoInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        FALSE
    );
    ShellGattDb_AddCharDeclValue(
        gBleSig_HardwareRevisionString_d,
        gRead_c,
        sizeof(mGattDbDynamic_DIServiceHwRevInitValue),
        mGattDbDynamic_DIServiceHwRevInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        FALSE
    );
    ShellGattDb_AddCharDeclValue(
        gBleSig_FirmwareRevisionString_d,
        gRead_c,
        sizeof(mGattDbDynamic_DIServiceFwRevInitValue),
        mGattDbDynamic_DIServiceFwRevInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        FALSE
    );
    ShellGattDb_AddCharDeclValue(
        gBleSig_SoftwareRevisionString_d,
        gRead_c,
        sizeof(mGattDbDynamic_DIServiceSwRevInitValue),
        mGattDbDynamic_DIServiceSwRevInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        FALSE
    );
    ShellGattDb_AddCharDeclValue(
        gBleSig_SystemId_d,
        gRead_c,
        sizeof(mGattDbDynamic_DIServiceSysIdInitValue),
        mGattDbDynamic_DIServiceSysIdInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        FALSE
    );
    ShellGattDb_AddCharDeclValue(
        gBleSig_IeeeRcdl_d,
        gRead_c,
        sizeof(mGattDbDynamic_DIServiceIeeeRcdlInitValue),
        mGattDbDynamic_DIServiceIeeeRcdlInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        FALSE
    );
}

static void ShellGattDb_AddServiceOtap(void)
{
    ShellGattDb_AddPrimaryServiceDecl128(uuid_service_otap);

    ShellGattDb_AddCharDeclValue128(
        uuid_char_otap_control_point,
        gWrite_c | gIndicate_c,
        sizeof(mGattDbDynamic_OtapServicePCPInitValue),
        mGattDbDynamic_OtapServicePCPInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagWritable_c,
        TRUE
    );
    GATTDBDynamicAddCccdRequest(BLE_FSCI_IF);

    ShellGattDb_AddCharDeclValue128(
        uuid_char_otap_data,
        gWriteWithoutRsp_c,
        sizeof(mGattDbDynamic_OtapServiceDataInitValue),
        mGattDbDynamic_OtapServiceDataInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagWritable_c,
        TRUE
    );
}

static void ShellGattDb_AddServiceWirelessUART(void)
{
    ShellGattDb_AddPrimaryServiceDecl128(uuid_service_wireless_uart);

    ShellGattDb_AddCharDeclValue128(
        uuid_uart_stream,
        gWriteWithoutRsp_c,
        sizeof(mGattDbDynamic_WUARTServiceStreamInitValue),
        mGattDbDynamic_WUARTServiceStreamInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagWritable_c,
        TRUE
    );
    ShellGattDb_AddCharDeclValue128(
    	uuid_uart_stream_read,
		gRead_c | gNotify_c,
        sizeof(mGattDbDynamic_WUARTServiceStreamInitValue),
        mGattDbDynamic_WUARTServiceStreamInitValue,
        GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_ValueAccessPermissions_gPermissionFlagReadable_c,
        TRUE
    );
    GATTDBDynamicAddCccdRequest(BLE_FSCI_IF);
}

void SHELL_GattDb_PrintPrimaryService
(
    GATTDBDynamicAddPrimaryServiceDeclarationIndication_t *evt
)
{
    shell_printf("\r\n\tService Handle --> %d\r\n", evt->ServiceHandle);
}

void SHELL_GattDb_PrintReadAttr
(
    GATTDBReadAttributeIndication_t *evt
)
{
    shell_printf("\r\n\tRead Attribute Indication --> 0x");
    shell_print_buffer_reversed(evt->Value, evt->ValueLength);
    SHELL_NEWLINE();

    if (mLatestUuid == gBleSig_GapDeviceName_d)
    {
        FLib_MemSet(mDeviceName, 0, sizeof(mDeviceName));
        FLib_MemCpy(mDeviceName, evt->Value, evt->ValueLength);
        mDeviceName[evt->ValueLength] = '\0';
    }

    MEM_BufferFree(evt->Value);
}

void SHELL_GattDb_PrintChar
(
    GATTDBDynamicAddCharacteristicDeclarationAndValueIndication_t *evt
)
{
    shell_printf("\r\n\tCharacteristic Handle --> %d\r\n", evt->CharacteristicHandle);
}

void SHELL_GattDb_PrintCharDesc
(
    GATTDBDynamicAddCharacteristicDescriptorIndication_t *evt
)
{
    shell_printf("\r\n\tCharacteristic Descriptor Handle --> %d\r\n", evt->DescriptorHandle);
}

void SHELL_GattDb_PrintCCCD
(
    GATTDBDynamicAddCccdIndication_t *evt
)
{
    shell_printf("\r\n\tCCCD Handle --> %d\r\n", evt->CCCDHandle);
}

void SHELL_GapPrintPublicAddr
(
    GAPGenericEventPublicAddressReadIndication_t *evt
)
{
    shell_printf("\r\n\tPublic Address --> 0x");
    shell_print_buffer_reversed(evt->Address, sizeof(evt->Address));
    FLib_MemCpy(maBleDeviceAddress, evt->Address, sizeof(bleDeviceAddress_t));
    SHELL_NEWLINE();
}

void SHELL_GapPrintDeviceScannedIndication
(
    GAPScanningEventDeviceScannedIndication_t *evt
)
{
    shell_printf("\r\nScanned Device ID --> %d; use this ID as argument to gap connect", mScannedDevicesCount);
    shell_printf("\r\nAddress --> 0x");
    shell_print_buffer(evt->Address, sizeof(evt->Address));
    shell_printf("\r\nRSSI --> %d", evt->Rssi);
    shell_printf("\r\nData --> 0x");
    shell_print_buffer(evt->Data, evt->DataLength);
    shell_printf("\r\nDirect RPA Used --> %d", evt->DirectRpaUsed);

    if (evt->DirectRpaUsed)
    {
        shell_printf("\r\nDirect RPA --> 0x");
        shell_print_buffer(evt->DirectRpa, sizeof(evt->DirectRpa));
    }

    shell_printf("\r\nAdvertising Address Resolved --> %d", evt->advertisingAddressResolved);
    SHELL_NEWLINE();

    if (mScannedDevicesCount == mMaxGapScannedDevicesCount_d)
    {
        shell_write("No more memory to store another scanned device!");
    }
    else
    {
        FLib_MemCpy(&mScannedDevices[mScannedDevicesCount++], evt, sizeof(GAPScanningEventDeviceScannedIndication_t));
    }

    MEM_BufferFree(evt->Data);
}

void SHELL_GapAcceptPairingRequest
(
    GAPConnectionEventPairingRequestIndication_t *evt
)
{
    GAPAcceptPairingRequestRequest_t req;

    req.DeviceId = evt->DeviceId;
    FLib_MemCpy(&req.PairingParameters, &mPairingParameters.PairingParameters, sizeof(req.PairingParameters));
    GAPAcceptPairingRequestRequest(&req, BLE_FSCI_IF);
}


static void SHELL_GapDiscoverAllCharsOfService
(
    void *param
)
{
    uint16_t uuid16;

    SHELL_NEWLINE();

    if (mCurrentServiceInDiscoveryIndex < mAllPrimaryServices.NbOfDiscoveredServices)
    {
        uuid16 = mAllPrimaryServices.DiscoveredServices[mCurrentServiceInDiscoveryIndex].Uuid.Uuid16Bits[0] +
                 (mAllPrimaryServices.DiscoveredServices[mCurrentServiceInDiscoveryIndex].Uuid.Uuid16Bits[1] << 8);

        shell_printf("\r\n--> %s Start Handle: %d End Handle: %d\r\n",
                     ShellGatt_GetServiceName(uuid16),
                     mAllPrimaryServices.DiscoveredServices[mCurrentServiceInDiscoveryIndex].StartHandle,
                     mAllPrimaryServices.DiscoveredServices[mCurrentServiceInDiscoveryIndex].EndHandle);

        GATTClientDiscoverAllCharacteristicsOfServiceRequest_t req = { 0 };
        req.DeviceId = gPeerDeviceId;
        req.Service.StartHandle = mAllPrimaryServices.DiscoveredServices[mCurrentServiceInDiscoveryIndex].StartHandle;
        req.Service.EndHandle = mAllPrimaryServices.DiscoveredServices[mCurrentServiceInDiscoveryIndex].EndHandle;
        req.Service.UuidType = Uuid16Bits;
        req.Service.Uuid.Uuid16Bits[0] = mAllPrimaryServices.DiscoveredServices[mCurrentServiceInDiscoveryIndex].Uuid.Uuid16Bits[0];
        req.Service.Uuid.Uuid16Bits[1] = mAllPrimaryServices.DiscoveredServices[mCurrentServiceInDiscoveryIndex].Uuid.Uuid16Bits[1];
        req.MaxNbOfCharacteristics = mMaxServiceCharCount_d;
        GATTClientDiscoverAllCharacteristicsOfServiceRequest(&req, BLE_FSCI_IF);

        mCurrentServiceInDiscoveryIndex++;
    }
    else
    {
        mCurrentServiceInDiscoveryIndex = 0;
        MEM_BufferFree(mAllPrimaryServices.DiscoveredServices);
        shell_refresh();
    }
}

static void SHELL_DiscoverAllDescriptorsOfChar
(
    void *param
)
{
    uint16_t uuid16;

    if (mCurrentCharInDiscoveryIndex < mAllChars.Service.NbOfCharacteristics)
    {
        uuid16 = mAllChars.Service.Characteristics[mCurrentCharInDiscoveryIndex].Value.Uuid.Uuid16Bits[0] +
                 (mAllChars.Service.Characteristics[mCurrentCharInDiscoveryIndex].Value.Uuid.Uuid16Bits[1] << 8);

        shell_printf("\r\n- %s Value Handle: %d\r\n",
                     ShellGatt_GetCharacteristicName(uuid16),
                     mAllChars.Service.Characteristics[mCurrentCharInDiscoveryIndex].Value.Handle);

        if (gWUartState == gWUartConnectionEstablished_c)
        {
            hPeerUartStream = mAllChars.Service.Characteristics[mCurrentCharInDiscoveryIndex].Value.Handle;
        }

        // Value not used here
        MEM_BufferFree(mAllChars.Service.Characteristics[mCurrentCharInDiscoveryIndex].Value.Value);

        if (mAllChars.Service.Characteristics[mCurrentCharInDiscoveryIndex].Value.Handle < mAllChars.Service.EndHandle)
        {
            GATTClientDiscoverAllCharacteristicDescriptorsRequest_t req = { 0 };

            req.DeviceId = gPeerDeviceId;
            req.Characteristic.Properties = mAllChars.Service.Characteristics[mCurrentCharInDiscoveryIndex].Properties;

            req.Characteristic.Value.Handle = mAllChars.Service.Characteristics[mCurrentCharInDiscoveryIndex].Value.Handle;
            req.Characteristic.Value.UuidType = mAllChars.Service.Characteristics[mCurrentCharInDiscoveryIndex].Value.UuidType;
            req.Characteristic.Value.Uuid.Uuid16Bits[0] = mAllChars.Service.Characteristics[mCurrentCharInDiscoveryIndex].Value.Uuid.Uuid16Bits[0];
            req.Characteristic.Value.Uuid.Uuid16Bits[1] = mAllChars.Service.Characteristics[mCurrentCharInDiscoveryIndex].Value.Uuid.Uuid16Bits[1];
            req.Characteristic.Value.MaxValueLength = mAllChars.Service.Characteristics[mCurrentCharInDiscoveryIndex].Value.MaxValueLength;

            req.EndingHandle = mAllChars.Service.EndHandle;
            req.MaxNbOfDescriptors = mMaxCharDescriptorsCount_d;

            GATTClientDiscoverAllCharacteristicDescriptorsRequest(&req, BLE_FSCI_IF);
            mCurrentCharInDiscoveryIndex++;
        }
        else
        {
            mCurrentCharInDiscoveryIndex++;
            OSA_TimeDelay(100);
            SHELL_DiscoverAllDescriptorsOfChar(NULL);
        }
    }
    else
    {
        mCurrentCharInDiscoveryIndex = 0;
        MEM_BufferFree(mAllChars.Service.Characteristics);
        MEM_BufferFree(mAllChars.Service.IncludedServices);
        OSA_TimeDelay(100);
        SHELL_GapDiscoverAllCharsOfService(NULL);
    }
}

void SHELL_PrintDiscoveredDescriptors
(
    GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_t *evt
)
{
    uint16_t uuid16;
    SHELL_NEWLINE();

    for (uint8_t k = 0; k < evt->Characteristic.NbOfDescriptors; k++)
    {
        uuid16 = evt->Characteristic.Descriptors[k].Uuid.Uuid16Bits[0] +
                 (evt->Characteristic.Descriptors[k].Uuid.Uuid16Bits[1] << 8);

        shell_printf("\r\n- %s Descriptor Handle: %d\r\n",
                     ShellGatt_GetDescriptorName(uuid16),
                     evt->Characteristic.Descriptors[k].Handle);

        MEM_BufferFree(evt->Characteristic.Descriptors[k].Value);
    }

    MEM_BufferFree(evt->Characteristic.Descriptors);
    MEM_BufferFree(evt->Characteristic.Value.Value);

    // keep the loop going
    OSA_TimeDelay(100);
    SHELL_DiscoverAllDescriptorsOfChar(NULL);
}

void SHELL_PrintDiscoveredChars
(
    GATTClientProcedureDiscoverAllCharacteristicsIndication_t *evt
)
{
    SHELL_NEWLINE();

    // save for later use
    FLib_MemCpy(&mAllChars, evt, sizeof(GATTClientProcedureDiscoverAllCharacteristicsIndication_t));

    // keep the loop going
    OSA_TimeDelay(100);
    SHELL_DiscoverAllDescriptorsOfChar(NULL);
}

void SHELL_PrintDiscoveredPrimaryServices
(
    GATTClientProcedureDiscoverAllPrimaryServicesIndication_t *evt
)
{
    SHELL_NEWLINE();
    shell_printf("--> Discovered primary services: %d\r\n", evt->NbOfDiscoveredServices);

    // save for later use
    FLib_MemCpy(&mAllPrimaryServices, evt, sizeof(GATTClientProcedureDiscoverAllPrimaryServicesIndication_t));

    // keep the loop going
    OSA_TimeDelay(100);
    SHELL_GapDiscoverAllCharsOfService(NULL);
}

/************************************************************************************
* DEMO
************************************************************************************/
static int32_t ShellBleDemo_Command
(
    shell_handle_t context,
    int32_t argc,
    char **argv
)
{
    if (argc < 2)
    {
        return CMD_RET_USAGE;
    }

    if (!strcmp((char *)argv[1], "hrs"))
    {
        BleApp_DemoHrs();
        return CMD_RET_SUCCESS;
    }
    else if (!strcmp((char *)argv[1], "otap"))
    {
        BleApp_DemoOtap();
        return CMD_RET_SUCCESS;
    }
    else if (!strcmp((char *)argv[1], "wuart"))
    {
        if (argc == 2)
        {
            BleApp_DemoWirelessUART();
        }
        else
        {
            BleApp_DemoWirelessUARTWrite(argc - 2, &argv[2]);
        }

        return CMD_RET_SUCCESS;
    }

    return CMD_RET_USAGE;
}

void BleApp_DemoHrs(void)
{
    BleApp_DemoHrsSm();
}

static void BleApp_DemoHrsSm(void)
{
    switch (gHrsDemoState)
    {
        case gHrsDemoAddServiceGatt_c:
            ShellGattDb_AddServiceGatt();
            gHrsDemoState = gHrsDemoAddServiceGap_c;
            break;

        case gHrsDemoAddServiceGap_c:
            ShellGattDb_AddServiceGap();
            gHrsDemoState = gHrsDemoAddServiceHeartRate_c;
            break;

        case gHrsDemoAddServiceHeartRate_c:
            ShellGattDb_AddServiceHeartRate();
            gHrsDemoState = gHrsDemoAddServiceBattery_c;
            break;

        case gHrsDemoAddServiceBattery_c:
            ShellGattDb_AddServiceBattery();
            gHrsDemoState = gHrsDemoAddServiceDeviceInfo_c;
            break;

        case gHrsDemoAddServiceDeviceInfo_c:
            ShellGattDb_AddServiceDeviceInfo();
            gHrsDemoState = gHrsDemoReadPublicAddress_c;
            break;

        case gHrsDemoReadPublicAddress_c:
            ShellGap_DeviceAddress(0, NULL);
            gHrsDemoState = gHrsDemoSetAdvData_c;
            break;

        case gHrsDemoSetAdvData_c:
        {
            char gBleSig_HeartRateService_d_str[5];
            sprintf(gBleSig_HeartRateService_d_str, "%04X", gBleSig_HeartRateService_d);

            ShellGap_AppendAdvData(&mAppAdvertisingData,
                                   GAPSetAdvertisingDataRequest_AdvertisingData_AdStructures_Type_gAdFlags_c,
                                   "6");    // gLeGeneralDiscoverableMode_c | gBrEdrNotSupported_c
            ShellGap_AppendAdvData(&mAppAdvertisingData,
                                   GAPSetAdvertisingDataRequest_AdvertisingData_AdStructures_Type_gAdIncomplete16bitServiceList_c,
                                   gBleSig_HeartRateService_d_str);
            ShellGap_AppendAdvData(&mAppAdvertisingData,
                                   GAPSetAdvertisingDataRequest_AdvertisingData_AdStructures_Type_gAdShortenedLocalName_c,
                                   HRS_SHORTENED_LOCAL_NAME);
            ShellGap_ChangeAdvertisingData(2, NULL);

            gHrsDemoState = gHrsDemoFindBatteryServiceHandle_c;
            break;
        }

        case gHrsDemoFindBatteryServiceHandle_c:
        {
            GATTDBFindServiceHandleRequest_t req;
            req.StartHandle = 0x0001;  // should be 0x0001 on the first call
            req.UuidType = Uuid16Bits;
            UuidToArray(req.Uuid.Uuid16Bits, gBleSig_BatteryService_d);

            gLatestHandle = INVALID_HANDLE;
            GATTDBFindServiceHandleRequest(&req, BLE_FSCI_IF);
            gHrsDemoState = gHrsDemoRegisterHandlesForWriteNotif_c;
            break;
        }

        case gHrsDemoRegisterHandlesForWriteNotif_c:
        {
            if (gLatestHandle != INVALID_HANDLE)
            {
                uint16_t attrHandles[1] = {gLatestHandle};
                GATTServerRegisterHandlesForWriteNotificationsRequest_t req;
                req.HandleCount = 1;
                req.AttributeHandles = attrHandles;

                GATTServerRegisterHandlesForWriteNotificationsRequest(&req, BLE_FSCI_IF);
                GATTServerRegisterCallbackRequest(BLE_FSCI_IF);

                hServiceBattery = gLatestHandle;
                gHrsDemoState = gHrsDemoFindHeartRateServiceHandle_c;
            }
            else
            {
                shell_write("\r\n-->  GATT DB: Could not find service handle for Battery.");
                gHrsDemoState = gHrsDemoError_c;
            }

            break;
        }

        case gHrsDemoFindHeartRateServiceHandle_c:
        {
            GATTDBFindServiceHandleRequest_t req;
            req.StartHandle = 0x0001;  // should be 0x0001 on the first call
            req.UuidType = Uuid16Bits;
            UuidToArray(req.Uuid.Uuid16Bits, gBleSig_HeartRateService_d);

            gLatestHandle = INVALID_HANDLE;
            GATTDBFindServiceHandleRequest(&req, BLE_FSCI_IF);
            gHrsDemoState = gHrsDemoFindHrMeasurementHandle_c;
            break;
        }

        case gHrsDemoFindHrMeasurementHandle_c:
        {
            if (gLatestHandle != INVALID_HANDLE)
            {
                GATTDBFindCharValueHandleInServiceRequest_t req;
                // save previous handle requested
                hServiceHeartRate = gLatestHandle;

                req.ServiceHandle = hServiceHeartRate;
                req.UuidType = Uuid16Bits;
                UuidToArray(req.Uuid.Uuid16Bits, gBleSig_HrMeasurement_d);

                gLatestHandle = INVALID_HANDLE;
                GATTDBFindCharValueHandleInServiceRequest(&req, BLE_FSCI_IF);

                gHrsDemoState = gHrsDemoFindBodySensorLocationHandle_c;
            }
            else
            {
                shell_write("\r\n-->  GATT DB: Could not find service handle for Heart Rate.");
                gHrsDemoState = gHrsDemoError_c;
            }

            break;
        }

        case gHrsDemoFindBodySensorLocationHandle_c:
        {
            if (gLatestHandle != INVALID_HANDLE)
            {
                GATTDBFindCharValueHandleInServiceRequest_t req;
                // save previous handle requested
                hValueHrMeasurement = gLatestHandle;

                req.ServiceHandle = hServiceHeartRate;
                req.UuidType = Uuid16Bits;
                UuidToArray(req.Uuid.Uuid16Bits, gBleSig_BodySensorLocation_d);

                gLatestHandle = INVALID_HANDLE;
                GATTDBFindCharValueHandleInServiceRequest(&req, BLE_FSCI_IF);

                gHrsDemoState = gHrsDemoFindBatteryLevelHandle_c;
            }
            else
            {
                shell_write("\r\n-->  GATT DB: Could not find char handle for Heart Rate Measurement.");
                gHrsDemoState = gHrsDemoError_c;
            }

            break;
        }

        case gHrsDemoFindBatteryLevelHandle_c:
        {
            if (gLatestHandle != INVALID_HANDLE)
            {
                // save previous handle requested
                hValueBodyLocation = gLatestHandle;

                GATTDBFindCharValueHandleInServiceRequest_t req;
                req.ServiceHandle = hServiceBattery;
                req.UuidType = Uuid16Bits;
                UuidToArray(req.Uuid.Uuid16Bits, gBleSig_BatteryLevel_d);

                gLatestHandle = INVALID_HANDLE;
                GATTDBFindCharValueHandleInServiceRequest(&req, BLE_FSCI_IF);

                gHrsDemoState = gHrsDemoWriteHrMeasurement_c;
            }
            else
            {
                shell_write("\r\n-->  GATT DB: Could not find char handle for Heart Rate Body Sensor Location.");
                gHrsDemoState = gHrsDemoError_c;
            }

            break;
        }

        case gHrsDemoWriteHrMeasurement_c:
        {
            if (gLatestHandle != INVALID_HANDLE)
            {
                GATTDBWriteAttributeRequest_t req;
                uint8_t value = (uint8_t)gHrs_EnergyExpendedEnabled_c | (uint8_t)gHrs_SensorContactSupported_c | (uint8_t)gHrs_SensorContactDetected_c;

                // save previous handle requested
                hValueBatteryLevel = gLatestHandle;

                req.Handle = hValueHrMeasurement;
                req.ValueLength = sizeof(value);
                req.Value = &value;

                GATTDBWriteAttributeRequest(&req, BLE_FSCI_IF);

                gHrsDemoState = gHrsDemoWriteBodyLocation_c;
            }
            else
            {
                shell_write("\r\n-->  GATT DB: Could not find char handle for Battery Level.");
                gHrsDemoState = gHrsDemoError_c;
            }

            break;
        }

        case gHrsDemoWriteBodyLocation_c:
        {
            GATTDBWriteAttributeRequest_t req;
            uint8_t value = gHrs_BodySensorLocChest_c;

            req.Handle = hValueBodyLocation;
            req.ValueLength = sizeof(value);
            req.Value = &value;
            GATTDBWriteAttributeRequest(&req, BLE_FSCI_IF);

            gHrsDemoState = gHrsDemoWriteBatteryLevel_c;
            break;
        }

        case gHrsDemoWriteBatteryLevel_c:
        {
            GATTDBWriteAttributeRequest_t req;
            uint8_t value = 100;  // 100% battery level

            req.Handle = hValueBatteryLevel;
            req.ValueLength = sizeof(value);
            req.Value = &value;
            GATTDBWriteAttributeRequest(&req, BLE_FSCI_IF);

            gHrsDemoState = gHrsDemoSetAdvParameters_c;
            break;
        }

        case gHrsDemoSetAdvParameters_c:
            mAdvParams.MinInterval = mFastConnMinAdvInterval_c;
            mAdvParams.MaxInterval = mFastConnMaxAdvInterval_c;
            GAPSetAdvertisingParametersRequest(&mAdvParams, BLE_FSCI_IF);

            gHrsDemoState = gHrsDemoSetupFinishedMessage;
            break;

        case gHrsDemoSetupFinishedMessage:
            shell_write("\r\n-->  Setup finished, please open IoT Toolbox -> Heart Rate -> KHC_HRS\r\n");
            gHrsDemoState = gHrsDemoWaitingConnection_c;
            break;

        case gHrsDemoWaitingConnection_c:
            shell_write("\r\n-->  Waiting connection from a smartphone..\r\n");
            break;

        case gHrsDemoStartSendMeasurements:
            if (xHrsTaskHandle == NULL)
            {
                /* Create the task, storing the handle. */
                if (xTaskCreate(BleApp_DemoHrsNotify, "HRS_task", configMINIMAL_STACK_SIZE * 2, NULL,
                                host_task_PRIORITY + 1, &xHrsTaskHandle) != pdPASS)
                {
                    shell_printf("HRS task creation failed!.\r\n");

                    while (1)
                        ;
                }
            }
            else
            {
                vTaskResume(xHrsTaskHandle);
            }

            gHrsDemoState = gHrsDemoConnectionEstablished_c;
            return;

        case gHrsDemoStopSendMeasurements:
            vTaskSuspend(xHrsTaskHandle);
            gHrsDemoState = gHrsDemoSetAdvParameters_c;
            break;

        default:
            break;
    }

    if ((gHrsDemoState != gHrsDemoError_c) && (gHrsDemoState != gHrsDemoWaitingConnection_c))
    {
        OSA_TimeDelay(HRS_DEMO_TIMEOUT);
        BleApp_DemoHrsSm();
    }
}

static void BleApp_DemoHrsNotify(void *pvParameters)
{
    uint64_t counter = 0;

    while (1)
    {
        /* sleep 1-sec */
        OSA_TimeDelay(1000);
        counter++;

        /* check to see what we need to notify */
        if (counter % mHeartRateReportInterval_c == 0)
        {
            BleApp_DemoHrsNotifyHr();
        }

        if (counter % mBatteryLevelReportInterval_c == 0)
        {
            BleApp_DemoHrsNotifyBattery();
        }
    }
}

static void BleApp_DemoHrsNotifyHr(void)
{
    if (gHrsDemoState != gHrsDemoConnectionEstablished_c)
    {
        return;
    }

    uint8_t value[2];
    GATTDBWriteAttributeRequest_t req;

    char **argv = MEM_BufferAlloc(sizeof(char *));

    if (!argv)
    {
        shell_write("\r\n-->  HRS Event: Insufficient memory.");
        return;
    }

    argv[0] = MEM_BufferAlloc(3);

    if (!argv[0])
    {
        shell_write("\r\n-->  HRS Event: Insufficient memory.");
        return;
    }

    req.Handle = hValueHrMeasurement;
    req.ValueLength = 2;
    value[0] = gHrs_8BitHeartRateFormat_c;
    value[1] = hrsReportedValue++;

    req.Value = value;
    GATTDBWriteAttributeRequest(&req, BLE_FSCI_IF);

    sprintf(argv[0], "%hu", hValueHrMeasurement);
    ShellGatt_Notify(1, argv);

    MEM_BufferFree(argv[0]);
    MEM_BufferFree(argv);
}

static void BleApp_DemoHrsNotifyBattery(void)
{
    if (gHrsDemoState != gHrsDemoConnectionEstablished_c)
    {
        return;
    }

    GATTDBWriteAttributeRequest_t req;
    req.Handle = hValueBatteryLevel;
    req.ValueLength = 1;
    req.Value = &hrsReportedValue;

    GATTDBWriteAttributeRequest(&req, BLE_FSCI_IF);
}

void BleApp_DemoOtap(void)
{
    BleApp_DemoOtapSm();
}

static void BleApp_DemoOtapSm(void)
{
    switch (gOtapState)
    {
        case gOtapAddServiceGatt_c:
            ShellGattDb_AddServiceGatt();
            gOtapState = gOtapAddServiceGap_c;
            break;

        case gOtapAddServiceGap_c:
            ShellGattDb_AddServiceGap();
            gOtapState = gOtapAddServiceOtap_c;
            break;

        case gOtapAddServiceOtap_c:
            ShellGattDb_AddServiceOtap();
            gOtapState = gOtapAddServiceBattery_c;
            break;

        case gOtapAddServiceBattery_c:
            ShellGattDb_AddServiceBattery();
            gOtapState = gOtapAddServiceDeviceInfo_c;
            break;

        case gOtapAddServiceDeviceInfo_c:
            ShellGattDb_AddServiceDeviceInfo();
            gOtapState = gOtapDbReady;
            break;

        case gOtapDbReady:
            shell_write("\r\nGATTDB configuration completed. Press SW4 to start advertising.\r\n");
            gOtapState = gOtapWaitingConnection_c;
            break;

        default:
            break;
    }

    if (gOtapState != gOtapWaitingConnection_c)
    {
        OSA_TimeDelay(OTAP_TIMEOUT);
        BleApp_DemoOtapSm();
    }
}

void BleApp_DemoWirelessUART(void)
{
    BleApp_DemoWirelessUARTSm();
}

static void BleApp_DemoWirelessUARTSm(void)
{
    switch (gWUartState)
    {
        case gWUartAddServiceGatt_c:
            ShellGattDb_AddServiceGatt();
            gWUartState = gWUartAddServiceGap_c;
            break;

        case gWUartAddServiceGap_c:
            ShellGattDb_AddServiceGap();
            gWUartState = gWUartAddServiceWirelessUART_c;
            break;

        case gWUartAddServiceWirelessUART_c:
        {

            ShellGattDb_AddServiceWirelessUART();
            shell_write("\r\n Device Addservice wuart \r\n");
            gWUartState = gWUartAddServiceBattery_c;
            break;
        }

        case gWUartAddServiceBattery_c:
            ShellGattDb_AddServiceBattery();
            shell_write("\r\n Device Addservice BATT\r\n");
            gWUartState = gWUartAddServiceDeviceInfo_c;
            break;

        case gWUartAddServiceDeviceInfo_c:
            ShellGattDb_AddServiceDeviceInfo();
            shell_write("\r\n Device Addservice \r\n");
            gWUartState = gWUartReadPublicAddress_c;
            break;

        case gWUartReadPublicAddress_c:
            ShellGap_DeviceAddress(0, NULL);
            shell_write("\r\n Device Address \r\n");
            gWUartState = gWUartSetPassKey_c;
            break;

        case gWUartSetPassKey_c:
        {
        	uint32_t passkey = 999999;
        	ShellGap_SetSecurityRequirements();
        	ShellGap_SetPassKey(passkey);
        	ShellGap_GetStaticAdd();
        	gWUartState = gWUartSetAdvData_c;
        	break;
        }

        case gWUartSetAdvData_c:
            ShellGap_AppendAdvData(&mAppAdvertisingData,
                                   GAPSetAdvertisingDataRequest_AdvertisingData_AdStructures_Type_gAdFlags_c,
                                   "6");    // gLeGeneralDiscoverableMode_c | gBrEdrNotSupported_c
            ShellGap_AppendAdvData(&mAppAdvertisingData,
                                   GAPSetAdvertisingDataRequest_AdvertisingData_AdStructures_Type_gAdComplete128bitServiceList_c,
                                   "E01C4B5E1EEBA15CEEF45EBA0001FF01");
            ShellGap_AppendAdvData(&mAppAdvertisingData,
                                   GAPSetAdvertisingDataRequest_AdvertisingData_AdStructures_Type_gAdShortenedLocalName_c,
                                   WUART_SHORTENED_LOCAL_NAME);
            ShellGap_ChangeAdvertisingData(2, NULL);
            shell_write("\r\n Device advdata \r\n");

            gWUartState = gWUartFindServiceHandle_c;
            break;


        case gWUartFindServiceHandle_c:
        {
            GATTDBFindServiceHandleRequest_t req;
            req.StartHandle = 0x0001;  // should be 0x0001 on the first call
            req.UuidType = Uuid128Bits;
            FLib_MemCpy(req.Uuid.Uuid128Bits, uuid_service_wireless_uart, sizeof(uuid_service_wireless_uart));

            gLatestHandle = INVALID_HANDLE;
            GATTDBFindServiceHandleRequest(&req, BLE_FSCI_IF);
            gWUartState = gWUartFindUartStreamHandle_c;
            shell_write("\r\n Device find service handle \r\n");
            break;
        }

        case gWUartFindUartStreamHandle_c:
            if (gLatestHandle != INVALID_HANDLE)
            {
                GATTDBFindCharValueHandleInServiceRequest_t req;

                req.ServiceHandle = gLatestHandle;
                hServiceUartStream =  gLatestHandle;
                req.UuidType = Uuid128Bits;
                FLib_MemCpy(req.Uuid.Uuid128Bits, uuid_uart_stream, sizeof(uuid_uart_stream));

                gLatestHandle = INVALID_HANDLE;
                GATTDBFindCharValueHandleInServiceRequest(&req, BLE_FSCI_IF);

                gWUartState = gWUartRegisterHandlesForWriteNotif_c;
            }
            else
            {
                shell_write("\r\n-->  GATT DB: Could not find service handle for Wireless UART.\r\n");
                gWUartState = gWUartDemoError_c;
            }

            break;

        case gWUartRegisterHandlesForWriteNotif_c:
            if (gLatestHandle != INVALID_HANDLE)
            {
                uint16_t attrHandles[1] = {gLatestHandle};
                GATTServerRegisterHandlesForWriteNotificationsRequest_t req;
                hUartStream = gLatestHandle;

                req.HandleCount = 1;
                req.AttributeHandles = attrHandles;

                GATTServerRegisterHandlesForWriteNotificationsRequest(&req, BLE_FSCI_IF);
                GATTServerRegisterCallbackRequest(BLE_FSCI_IF);

                gWUartState = gWUartFindUartStreamReadHandle_c;
            }
            else
            {
                shell_write("\r\n-->  GATT DB: Could not find value handle for Wireless UART stream.\r\n");
                gWUartState = gWUartDemoError_c;
            }

            break;

        case gWUartFindUartStreamReadHandle_c:
            if (hServiceUartStream != INVALID_HANDLE)
            {
                GATTDBFindCharValueHandleInServiceRequest_t req;

                req.ServiceHandle = hServiceUartStream;
                req.UuidType = Uuid128Bits;
                FLib_MemCpy(req.Uuid.Uuid128Bits, uuid_uart_stream_read, sizeof(uuid_uart_stream_read));

                gLatestHandle = INVALID_HANDLE;
                GATTDBFindCharValueHandleInServiceRequest(&req, BLE_FSCI_IF);

                gWUartState = gWUartRegisterHandlesForReadNotif_c;
            }
            else
            {
                shell_write("\r\n-->  GATT DB: Could not find service handle for Wireless UART.\r\n");
                gWUartState = gWUartDemoError_c;
            }

            break;

        case gWUartRegisterHandlesForReadNotif_c:
            if (gLatestHandle != INVALID_HANDLE)
            {
                uint16_t attrHandles[1] = {gLatestHandle};
//                GATTServerRegisterHandlesForWriteNotificationsRequest_t req;
                GATTServerRegisterHandlesForReadNotificationsRequest_t read_req;
                hReadUartStream = gLatestHandle;

//                req.HandleCount = 1;
//                req.AttributeHandles = attrHandles;

                read_req.HandleCount = 1;
                read_req.AttributeHandles = attrHandles;

                //GATTServerRegisterHandlesForWriteNotificationsRequest(&req, BLE_FSCI_IF);
               // GATTServerRegisterCallbackRequest(BLE_FSCI_IF);
               // GATTServerRegisterHandlesForReadNotificationsRequest(&read_req, BLE_FSCI_IF);

                gWUartState = gWUartSetAdvParameters_c;
            }
            else
            {
                shell_write("\r\n-->  GATT DB: Could not find value handle for Wireless UART stream.\r\n");
                gWUartState = gWUartDemoError_c;
            }

            break;

        case gWUartSetAdvParameters_c:
            mAdvParams.MinInterval = mFastConnMinAdvInterval_c;
            mAdvParams.MaxInterval = mFastConnMinAdvInterval_c;
            GAPSetAdvertisingParametersRequest(&mAdvParams, BLE_FSCI_IF);
            gWUartState = gWUartWaitingConnection_c;
            break;

        case gWUartConnectionEstablished_c:
        {
            /* Find Wireless UART stream handle on peer device */
            GATTClientDiscoverPrimaryServicesByUuidRequest_t req = { 0 };

            req.DeviceId = gPeerDeviceId;
            req.UuidType = Uuid128Bits;
            FLib_MemCpy(req.Uuid.Uuid128Bits, uuid_service_wireless_uart, sizeof(uuid_service_wireless_uart));
            req.MaxNbOfServices = 1;

            hPeerUartStream = INVALID_HANDLE;
            GATTClientDiscoverPrimaryServicesByUuidRequest(&req, BLE_FSCI_IF);
            break;
        }

        default:
            break;
    }

    if ((gWUartState != gWUartDemoError_c) && (gWUartState != gWUartWaitingConnection_c) &&
        (gWUartState != gWUartConnectionEstablished_c))
    {
        OSA_TimeDelay(WUART_TIMEOUT);
        BleApp_DemoWirelessUARTSm();
    }
}

static void BleApp_DemoWirelessUARTWrite
(
    int32_t argc,
    char **argv
)
{
    GATTClientWriteCharacteristicValueRequest_t req = { 0 };
    char *pText;
    uint8_t i, textLength = 0, textIdx = 0;

    if (gWUartState != gWUartConnectionEstablished_c)
    {
        shell_write("\r\n-->  Wireless UART Event: Not connected.");
        return;
    }

    if (hPeerUartStream == INVALID_HANDLE)
    {
        shell_write("\r\n-->  Wireless UART Event: Failed to discover characteristics.");
        return;
    }

    for (i = 0; i < argc; i++)
    {
        textLength += strlen(argv[i]);

        /* Room for spaces between words */
        if (i != argc - 1)
        {
            textLength += 1;
        }
        /* Room for CRLF */
        else
        {
            textLength += 2;
        }
    }

    pText = MEM_BufferAlloc(textLength);

    if (!pText)
    {
        shell_write("\r\n-->  Wireless UART Event: Insufficient memory.");
        return;
    }

    for (i = 0; i < argc; i++)
    {
        FLib_MemCpy(pText + textIdx, argv[i], strlen(argv[i])); textIdx += strlen(argv[i]);

        /* Spaces between words */
        if (i != argc - 1)
        {
            pText[textIdx++] = ' ';
        }
        /* End of Line */
        else
        {
            pText[textIdx++] = '\r';
            pText[textIdx++] = '\n';
        }
    }

    req.DeviceId = gPeerDeviceId;
    req.Characteristic.Value.Handle = hPeerUartStream;
    req.ValueLength = textLength;
    req.Value = (uint8_t *)pText;
    req.WithoutResponse = TRUE;
    req.SignedWrite = FALSE;
    req.ReliableLongCharWrites = FALSE;

    GATTClientWriteCharacteristicValueRequest(&req, BLE_FSCI_IF);
    MEM_BufferFree(pText);
}
