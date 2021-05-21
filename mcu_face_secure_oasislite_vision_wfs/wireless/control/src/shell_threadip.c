/*
 * Copyright (c) 2014 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
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
 * o Neither the name of the copyright holder nor the names of its
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
\file       shell_threadip.c
\brief      This is a public source file for the ThreadIP shell application. It
            contains the implementation of the shell commands used in the application.
*************************************************************************************
************************************************************************************/
#include "network_utils.h"
#include "shell_utils.h"
#include "shell_threadip.h"

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
#define THR_CHILD_BITS_SIZE     9
/*! The maximum child ID */
#define THR_MAX_CHILD_IDS       (1<<THR_CHILD_BITS_SIZE) / 32
#define THR_ML_PREFIX_LEN_BITS  (64)
#define THR_MAX_NEIGHBORS       (60)

/* Thread border router prefix flags */
#define THR_BR_PREFIX_FLAGS_P_PREFERENCE_MASK           0xC0
#define THR_BR_PREFIX_FLAGS_P_PREFERRED_MASK            0x20
#define THR_BR_PREFIX_FLAGS_P_SLAAC_MASK                0x10
#define THR_BR_PREFIX_FLAGS_P_DHCP_MASK                 0x08
#define THR_BR_PREFIX_FLAGS_P_CONFIGURE_MASK            0x04
#define THR_BR_PREFIX_FLAGS_P_DEFAULT_MASK              0x02
#define THR_BR_PREFIX_FLAGS_P_ON_MESH_MASK              0x01
#define THR_BR_PREFIX_FLAGS_P_ND_DNS_MASK               0x80  // MSB of Reserved

#define THR_BR_PREFIX_FLAGS_P_PREFERENCE_OFFSET         0x06
#define THR_BR_PREFIX_FLAGS_P_PREFERRED_OFFSET          0x05
#define THR_BR_PREFIX_FLAGS_P_SLAAC_OFFSET              0x04
#define THR_BR_PREFIX_FLAGS_P_DHCP_OFFSET               0x03
#define THR_BR_PREFIX_FLAGS_P_CONFIGURE_OFFSET          0x02
#define THR_BR_PREFIX_FLAGS_P_DEFAULT_OFFSET            0x01
#define THR_BR_PREFIX_FLAGS_P_ON_MESH_OFFSET            0x00
#define THR_BR_PREFIX_FLAGS_P_ND_DNS_OFFSET             0x07  // MSB of Reserved

/* thread border router external route flags */
#define THR_BR_EXT_ROUTE_FLAGS_R_PREF_MASK              0x03
#define THR_BR_EXT_ROUTE_FLAGS_R_PREF_OFFSET            0x00

/* thread border router prefix set/get */
#define THR_BR_FLAGS_SET(flag, value, mask, offset)     ((flag)=(((flag)&(~mask))|((value)<<offset)))
#define THR_BR_FLAGS_GET(value, mask, offset)           (((value)&mask)>>offset)

#define APP_TEMP_URI_PATH           "/temp"
#define APP_LED_URI_PATH            "/led"

#define PING_PAYLOAD_DEFAULT_SIZE   (32U)
#define PING_DEFAULT_TIMEOUT        (2000U)
#define PING_MIN_TIMEOUT            (2000U)
#define PING_DELAY                  (500U)

#define COAP_DEFAULT_PORT           5683

/************************************************************************************
*************************************************************************************
Public global variables declarations
*************************************************************************************
************************************************************************************/
uint32_t                gThreadInstanceId = 0;
bool_t                  gContinuousPing = FALSE;
uint8_t                 gCoAPInstanceId = THR_ALL_FFs8;
bool_t                  gAutostartInProgress = FALSE;
shellAutostartStates_t  gAutostartState = gAutostartFirstState_c;
uint8_t                 gAutostartChannel = THR_ALL_FFs8;

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
static void SHELL_EvMonitorInit(void);
static int32_t SHELL_ThrCommands(shell_handle_t context, int32_t argc, char **argv);
static int32_t SHELL_Ifconfig(shell_handle_t context, int32_t argc, char **argv);
static int32_t SHELL_MulticastGroups(shell_handle_t context, int32_t argc, char **argv);
static int32_t SHELL_Ping(shell_handle_t context, int32_t argc, char **argv);
static int32_t SHELL_CoapSend(shell_handle_t context, int32_t argc, char **argv);
static int32_t SHELL_Reboot(shell_handle_t context, int32_t argc, char **argv);
static int32_t SHELL_FactoryResetReq(shell_handle_t context, int32_t argc, char **argv);
static int32_t SHELL_Autostart(shell_handle_t context, int32_t argc, char **argv);

static void SHELL_PrintParentInfo(void *param);
static void SHELL_PrintDevRole(void *param);
static void SHELL_PrintDevType(void *param);
static void SHELL_PrintActiveTimestamp(void *param);

static void PING_RetransmitHandle(void *param);

/************************************************************************************
*************************************************************************************
Private type definitions
*************************************************************************************
************************************************************************************/
typedef void (*pfHandleAttr_t)(void *param);

typedef enum shellValueType_tag
{
    gString_c,
    gDecimal_c,
    gHex_c,
    gHexReversed_c,
    gArray_c,
    gTable_c,
} shellValueType_t;

typedef struct aShellThrSetAttr_tag
{
    char                                *pAttrName;
    THR_GetAttrConfirm_AttributeId_t    attrId;
    shellValueType_t                    valueType;
    uint32_t                            maxSize;
    pfHandleAttr_t                      pfHandleAttr;
    bool_t                              writable;
} aShellThrSetAttr_t;

typedef struct attrRef_tag
{
    void *pValue;
    bool_t requireFree;
} attrRef_t;

/*! Specific octet string type, 16 bytes */
typedef struct thrOctet16_tag
{
    uint8_t length;
    uint8_t aStr[16];
} thrOctet16_t;

/*! Specific octet string type, 32 bytes */
typedef struct thrOctet32_tag
{
    uint8_t length;
    uint8_t aStr[32];
} thrOctet32_t;

/*! Specific octet string type, 64 bytes */
typedef struct thrOctet64_tag
{
    uint8_t length;
    uint8_t aStr[64];
} thrOctet64_t;

typedef struct thrPrefixAttr_tag
{
    ipAddr_t prefix;
    uint8_t prefixLenBits;
} thrPrefixAttr_t;

typedef struct thrActiveDataSetAttr_tag
{
    uint8_t         channel;                    /*!< The current channel. */
    uint32_t        scanChannelMask;            /*!< The channels mask used when a network scanning is performed (energy scan, active scan or both);
                                                   0x07FFF800 means all 16 channels are used (from 11 to 26). */
    uint16_t        panId;                      /*!< The PAN identifier (ID).
                                                   - On network creation (calling THR_NwkCreate()), if it is set 0xffff then the device will generate a random pan ID. */
    uint8_t         xPanId[8];                  /*!< The extended PAN ID.
                                                   - On network creation (calling THR_NwkCreate()), if all bytes are 0xff's then the device will generate a random extended pan ID.
                                                   - On joining using out-of-band configuration (calling THR_NwkJoin() when devIsCommissioned = TRUE), if xPanId != all ff's
                                                   the device is using this extended pan id to find the pan ID and channel; otherwise no filter is apply*/
    thrPrefixAttr_t MLprefix;
    uint8_t         nwkMasterKey[16];           /*!< The network master key */
    thrOctet16_t    nwkName;                    /*!< Network Name.
                                                 -  On joining calling THR_NwkJoin() with devIsCommissioned == TRUE, if
                                                    nwkName.length != 0, the device will filter after network name to find the network parameters. */
    uint8_t         pskC[16];                   /*!< The Pre-Shared Key (PSKc) derived from  Commissioning Credential (network password)*/
    uint8_t         securityPolicy;             /*!< O and N bits without rotation time */
    uint32_t        nwkKeyRotationInterval;     /*!< The current key rotation interval in minutes */
    uint8_t         timestamp[8];               /*!< First 6 bytes: seconds, last 2 bytes: ticks */
} thrActiveAttr_t;

/*! Device types */
typedef enum thrDeviceType_tag
{
    gThrDevType_EndNode_c,       /*!< The node can be sleepy or non-sleepy end device (no routing capability )  */
    gThrDevType_ComboNode_c      /*!< The node can have any device role above */
} thrDeviceType_t;

/*! Device roles */
typedef enum thrInternalDeviceRole_tag
{
    gThrDevRole_Disconnected,   /*!< Device is disconnected */
    gThrDevRole_SED_c,          /*!< Sleepy End Device, no routing capability */
    gThrDevRole_MED_c,          /*!< Minimal End Device, no routing capability */
    gThrDevRole_FED_c,          /*!< Full End Device, address discovery and no routing capability */
    gThrDevRole_REED_c,         /*!< Router eligible end device (REED) */
    gThrDevRole_Router_c,       /*!< Router device */
    gThrDevRole_Leader_c        /*!< Leader device */
} thrInternalDeviceRole_t;

/*!
    Thread network information base (Thread Nibs) structure:
    - 802.15.4 attributes
    - thread specific attributes
*/
typedef struct thrAttr_tag
{
    /* 802.15.4 attributes */
    /* ------------------- */
    uint64_t ieeeAddr;                          /*!< The MAC extended address, also called IEEE address,
                                                   long address or 64-bit MAC address */
    uint64_t shaHashAddr;                       /*!< MAC address used for commissioning generated
                                                   from SHA256 hash on the ieeeAddr */

    uint64_t  randExtAddr;                      /*!< The random MAC extended address, used for
                                                   communication inside the network after commissioning */
    /* ------------------- */
    uint8_t   scanDuration;                     /*!< The scan duration time. This is an exponential scale, as seen in the 802.15.4 specification.
                                                   Range: 0 - 14. Values greater than 14 will be set to 14, as described in Thread Specification chapter 8.10.2.10.
                                                   The user can take into account that the total scanning time also depends on the number of channels scanned.
                                                   Ex.: for a scanmask of 16 channels, maximum value of scanDuration is 8, meaning 3.75 seconds/channel. */
    /* ------------------- */
    uint16_t  shortAddr;                        /*!< The short address.*/
    bool_t    permitJoin;                       /*!< Permit Join(Router devices only).
                                                   True = Device is allowing the child to join the network,
                                                   False = Device is not allowing any child to join the network */
    bool_t    rxOnWhenIdle;                     /*!< The receiver is ON when the device is in idle state.
                                                   Set RxOnWhenIdle TRUE for mains-powered (non-sleeping) end-devices. Set this
                                                   FALSE for sleeping end-devices. When FALSE, end-devices will poll their
                                                   parent for messages. See sedPollInterval for the polling timeout.*/
    uint32_t  sedPollInterval;                  /*!< The poll interval in milliseconds. This attribute is used only for sleepy end devices */
    /* ------------------- */
    /* Thread specific attributes: */
    /* ------------------- */
    uint32_t  sedFastPollInterval;              /*!< The fast poll interval in milliseconds. This attribute is used only for sleepy end devices
                                                   during the joining procedure*/
    bool_t    isFastPollEnabled;                /*!< Specify if the fast polling is enabled */
    bool_t    uniqueExtAddr;                    /*!< If is set to TRUE, the device is automatically generated a random extended address */
    bool_t    devIsConnected;                   /*!< Specifies if the device is connected or not */
    bool_t    devIsCommissioned;                /*!< If TRUE than the device is commissioned.
                                                   - On joining calling THR_NwkJoin() with devIsCommissioned == TRUE, the device will perform the attaching
                                                   procedure using the commissioning settings. Note that a network active scan is performed before attaching.
                                                   - On joining calling THR_NwkJoin() with devIsCommissioned == FALSE, the device will perform the
                                                   joining with Commissioner procedure (mesh-cop joining).*/
    /* ------------------- */
    uint32_t  sedTimeoutPeriod;                 /*!< The Timeout period used by the parent to consider a sleepy end device (SED) disconnected. */
    uint32_t  edTimeoutPeriod;                  /*!< The Timeout period used by the parent to consider an end device (ED) disconnected. */
    bool_t    childEDReqFullNwkData;            /*!< If it is set TRUE, the end device is requesting the full network data(stable and temporary nwk data).
                                                   If it is set FALSE, the end device is requesting only the unstable (temporary) network data. */
    thrDeviceType_t   deviceType;               /*!< The device type: 0x00 -EndNode, 0x01 - Combo Node  */
    thrInternalDeviceRole_t   devRole;          /*!< The device role: 0x01 - SED, 0x02 - MED, 0x03 - FED,0x04 - REED, 0x05 - Router, 0x06 - Leader */
    uint8_t   thrNwkCapabilitiesBitMap;         /*!< A bitmap that specify network capabilities of this device */
    /* ------------------- */
    uint32_t  nwkKeySeq;                        /*!< The current network key sequence number */
    /* ------------------- */
    uint32_t  childAddrMask[THR_MAX_CHILD_IDS]; /*!< The child address mask */
    /* ------------------- */
    uint32_t  partitionId;                      /*!< The current partition identifier */
    uint8_t   weighting;                        /*!< Leader weight */
    bool_t    doNotGeneratePartitionId;         /*!< Avoid random generation of partition ID */
    uint8_t   joinLqiThreshold;                 /*!< The joining LQI threshold used to select a potential parent. */
    uint8_t   selBestChannelEDThreshold;        /*!< The energy channel threshold to select the best channel when
                                                   more channels are scan (energy detect scan) to form the network.
                                                   Note that this is used only if the scanChannelMask attribute includes more than one channel.   */
    uint16_t   joinerUdpPort;                   /*!< Joiner UDP port */
    uint16_t   commissionerUdpPort;             /*!< Commissioner UDP port */
    uint32_t   keySwitchGuardTime;              /*!< The thread Key switch guard time to prevent inadvertent key switching */
    uint16_t   parentHoldTime;                  /*!< Thread hold time in seconds used by the parent to
                                                   hold the packets for SED devices */
    uint8_t    slaacPolicy;                     /*!< Used SLAAC policy (see thrSlaacPolicy_t)*/

    /* ------------------- */
    bool_t     nvmRestoreAutoStart;             /*!< Stack starts automatically with NVM restore after reset*/
    bool_t     nvmRestore;                      /*!< Restore from NVM */
    thrPrefixAttr_t brGlobalOnMeshPrefix;       /*!< Global /64 on-Mesh Prefix on Border Router */
    bool_t     brDefaultRoute;                  /*!< Default Route of the /64 on-mesh prefix */
    thrPrefixAttr_t brExternalIfPrefix;         /*!< Global /64 external interface prefix  */
    uint8_t    discoveryReqMacTxOptions;        /*!< The default discovery request Mac Tx options (see thrDiscReqTxOptions_t) */
    /* ------------------- */

    uint32_t   minDelayTimer;                    /*!< The minimum accepted time before a Pending Dataset can be installed[s] */
} thrAttr_t;

/*! Thread status */
typedef enum thrStatus_tag
{
    gThrStatus_Success_c = 0x00,
    gThrStatus_Failed_c = 0x01,
    gThrStatus_InvalidInstance_c = 0x02,
    gThrStatus_InvalidParam_c = 0x03,
    gThrStatus_NotPermitted_c = 0x04,
    gThrStatus_NotStarted_c = 0x05,
    gThrStatus_NoMem_c = 0x06,
    gThrStatus_UnsupportedAttr_c = 0x07,
    gThrStatus_EmptyEntry_c = 0x08,
    gThrStatus_InvalidValue_c = 0x09,
    gThrStatus_AlreadyConnected_c = 0x0A,
    gThrStatus_AlreadyCreated_c   = 0x0B,
    gThrStatus_NoTimers_c         = 0x0C,

    gThrStatus_EntryNotFound_c = THR_ALL_FFs8
} thrStatus_t;

/************************************************************************************
*************************************************************************************
Private variables declarations
*************************************************************************************
************************************************************************************/
shell_handle_t                  threadip_shellHandle;
static bool_t                   mShellCommandsEnabled = FALSE;    /*!< Avoid initializing the module multiple times */
static NWKU_PingRequest_t       mPingReq;
static uint32_t                 mPingTimeoutMs;
static ipAddr_t                 mDstIpAddr;
static char                     mAddrStr[INET6_ADDRSTRLEN];
static uint16_t                 mPingSize = 0;
static uint16_t                 mPingCounter = 0;
static uint64_t                 mPingTimestamp = 0;
static ipAddr_t                 *pSrcIpAddr = NULL; /* Used for keeping ping source address */

static const aShellThrSetAttr_t aShellThrAttr[] =
{
    {"activetimestamp",         THR_GetAttrConfirm_AttributeId_ActiveTimestamp,                 gDecimal_c,      GetSizeOfMember(thrActiveAttr_t, timestamp), SHELL_PrintActiveTimestamp, TRUE},
    {"autostart",               THR_GetAttrConfirm_AttributeId_NVM_RestoreAutoStart,            gDecimal_c,      sizeof(uint8_t), NULL, TRUE},
    {"bestchannelthreshold",    THR_GetAttrConfirm_AttributeId_SelectBestChannelEDThreshold,    gDecimal_c,      sizeof(uint8_t), NULL, TRUE},
    {"channel",                 THR_GetAttrConfirm_AttributeId_Channel,                         gDecimal_c,      sizeof(uint8_t), NULL, TRUE},
    {"childaddrmask",           THR_GetAttrConfirm_AttributeId_ChildAddrMask,                   gHex_c,          GetSizeOfMember(thrAttr_t, childAddrMask), NULL, FALSE}, // ????
    {"childreqfullnd",          THR_GetAttrConfirm_AttributeId_EndDevice_ChildEDReqFullNwkData, gDecimal_c,      sizeof(bool_t),  NULL, TRUE},
    {"childtimeout",            THR_GetAttrConfirm_AttributeId_ChildEDTimeout,                  gDecimal_c,      sizeof(uint32_t), NULL, TRUE},
    {"devicerole",              THR_GetAttrConfirm_AttributeId_DeviceRole,                      gString_c,       sizeof(uint8_t), SHELL_PrintDevRole, TRUE},
    {"devicetype",              THR_GetAttrConfirm_AttributeId_DeviceType,                      gDecimal_c,      sizeof(uint8_t), SHELL_PrintDevType, FALSE},
    {"eui",                     THR_GetAttrConfirm_AttributeId_IeeeExtendedAddr,                gHexReversed_c,  sizeof(uint64_t), NULL, FALSE},
    {"hashaddr",                THR_GetAttrConfirm_AttributeId_HashIeeeAddr,                    gHexReversed_c,  sizeof(uint64_t), NULL, FALSE},
    {"iscommissioned",          THR_GetAttrConfirm_AttributeId_IsDevCommissioned,               gDecimal_c,      sizeof(uint8_t), NULL, TRUE},
    {"isconnected",             THR_GetAttrConfirm_AttributeId_IsDevConnected,                  gDecimal_c,      sizeof(uint8_t), NULL, FALSE},
    {"isfastpollenabled",       THR_GetAttrConfirm_AttributeId_EndDevice_IsFastPollEnabled,     gDecimal_c,      sizeof(bool_t), NULL, FALSE},
    {"joinlqithreshold",        THR_GetAttrConfirm_AttributeId_JoinLqiThreshold,                gDecimal_c,      sizeof(uint8_t), NULL, TRUE},
    {"keyrotationinterval",     THR_GetAttrConfirm_AttributeId_Security_KeyRotationInterval,    gDecimal_c,      sizeof(uint32_t), NULL, TRUE},
    {"keyswitchguard",          THR_GetAttrConfirm_AttributeId_Security_KeySwitchGuardTime,     gDecimal_c,      sizeof(uint32_t), NULL, TRUE},
    {"leaderweight",            THR_GetAttrConfirm_AttributeId_LeaderWeight,                    gDecimal_c,      sizeof(uint8_t), NULL, TRUE},
    {"masterkey",               THR_GetAttrConfirm_AttributeId_Security_NwkMasterKey,           gArray_c,        GetSizeOfMember(thrActiveAttr_t, nwkMasterKey), NULL, TRUE},
    {"mlprefix",                THR_GetAttrConfirm_AttributeId_MLPrefix,                        gArray_c, (THR_ML_PREFIX_LEN_BITS >> 3), NULL, TRUE},
    {"modelname",               THR_GetAttrConfirm_AttributeId_ModelName,                       gString_c,       sizeof(thrOctet16_t), NULL, FALSE},
    {"neighbor", (THR_GetAttrConfirm_AttributeId_t)0,                            gTable_c,        0, SHELL_PrintNeighborInfo, FALSE},
    {"neighbors", (THR_GetAttrConfirm_AttributeId_t)0,                            gTable_c,        0, SHELL_PrintNeighborTbl, FALSE},
    {"nvm",                     THR_GetAttrConfirm_AttributeId_NVM_Restore,                     gDecimal_c,      sizeof(uint8_t), NULL, TRUE},
    {"nwkcapabilities",         THR_GetAttrConfirm_AttributeId_NwkCapabilities,                 gHex_c,          sizeof(uint8_t), NULL, TRUE},
    {"nwkname",                 THR_GetAttrConfirm_AttributeId_NwkName,                         gString_c,       sizeof(thrOctet16_t), NULL, TRUE},
    {"panid",                   THR_GetAttrConfirm_AttributeId_ShortPanId,                      gHex_c,          sizeof(uint16_t), NULL, TRUE},
    {"parent", (THR_GetAttrConfirm_AttributeId_t)0,                        gTable_c,        0, SHELL_PrintParentInfo, FALSE},
    {"parentholdtime",          THR_GetAttrConfirm_AttributeId_ParentHoldTime,                  gDecimal_c,      sizeof(uint16_t), NULL, TRUE},
    {"partitionid",             THR_GetAttrConfirm_AttributeId_PartitionId,                     gHex_c,          sizeof(uint32_t), NULL, TRUE},
    {"permitjoin",              THR_GetAttrConfirm_AttributeId_PermitJoin,                      gDecimal_c,      sizeof(uint8_t), NULL, FALSE},
    {"provisioningurl",         THR_GetAttrConfirm_AttributeId_ProvisioningURL,                 gString_c,       sizeof(thrOctet64_t), NULL, TRUE},
    {"pskcpassword",            THR_GetAttrConfirm_AttributeId_Security_PSKc,                   gString_c,       sizeof(thrOctet32_t), NULL, FALSE},
    {"pskd",                    THR_GetAttrConfirm_AttributeId_Security_PSKd,                   gString_c,       sizeof(thrOctet32_t), NULL, TRUE},
    {"randomaddr",              THR_GetAttrConfirm_AttributeId_RandomExtendedAddr,              gHexReversed_c,  sizeof(uint64_t), NULL, TRUE},
    {"routes", (THR_GetAttrConfirm_AttributeId_t)0,                            gTable_c,        0, SHELL_PrintRoutingTbl, FALSE},
    {"rxonidle",                THR_GetAttrConfirm_AttributeId_RxOnIdle,                        gDecimal_c,      sizeof(uint8_t), NULL, TRUE},
    {"scanduration",            THR_GetAttrConfirm_AttributeId_ScanDuration,                    gDecimal_c,      sizeof(uint8_t), NULL, TRUE},
    {"scanmask",                THR_GetAttrConfirm_AttributeId_ScanChannelMask,                 gHex_c,          sizeof(uint32_t), NULL, TRUE},
    {"sedfastpollinterval",     THR_GetAttrConfirm_AttributeId_SleepyEndDevice_FastPollInterval, gDecimal_c,      sizeof(uint32_t), NULL, TRUE},
    {"sedpollinterval",         THR_GetAttrConfirm_AttributeId_SedPollInterval,                 gDecimal_c,      sizeof(uint32_t), NULL, TRUE},
    {"shortaddr",               THR_GetAttrConfirm_AttributeId_ShortAddress,                    gHex_c,          sizeof(uint16_t), NULL, FALSE},
    {"slaacpolicy",             THR_GetAttrConfirm_AttributeId_SlaacPolicy,                     gDecimal_c,      sizeof(uint8_t), NULL, TRUE},
    {"stackversion",            THR_GetAttrConfirm_AttributeId_StackVersion,                    gString_c,       0, NULL, FALSE},
    {"swversion",               THR_GetAttrConfirm_AttributeId_SwVersion,                       gString_c,       sizeof(thrOctet16_t), NULL, FALSE},
    {"uniqueaddr",              THR_GetAttrConfirm_AttributeId_UniqueExtendedAddress,           gDecimal_c,      sizeof(uint8_t), NULL, TRUE},
    {"vendordata",              THR_GetAttrConfirm_AttributeId_VendorData,                      gString_c,       sizeof(thrOctet64_t), NULL, FALSE},
    {"vendorname",              THR_GetAttrConfirm_AttributeId_VendorName,                      gString_c,       sizeof(thrOctet32_t), NULL, FALSE},
    {"xpan",                    THR_GetAttrConfirm_AttributeId_ExtendedPanId,                   gArray_c,        GetSizeOfMember(thrActiveAttr_t, xPanId), NULL, TRUE}
};

static shell_command_t aShellCommands[] =
{
    {
        "thr",
        "\r\n\"thr\": List of commands for the Thread network\r\n"
        "   thr create\r\n"
        "   thr join\r\n"
        "   thr scan [active|energy|both]\r\n"
        "   thr detach\r\n"
        "   thr commissioner [start|stop]\r\n"
        "   thr joiner add <psk> <eui>\r\n"
        "   thr joiner remove <eui>\r\n"
        "   thr joiner removeall\r\n"
        "   thr sync steering {all}\r\n"
        "   thr sync nwkdata\r\n"
        "   thr get attributes - displays a list of all Thread attributes available for getting/setting\r\n"
        "   thr get <ATTRNAME/TABLENAME> - displays the value for the specified attribute\r\n"
        "   thr set <ATTRNAME> <value> - changes the value of the attribute with the specified value\r\n"
        "   thr nwkdata add slaac -p <Prefix> - len <prefixLength> -t <lifetime in seconds>\r\n"
        "   thr nwkdata add dhcpserver -p <Prefix> - len <prefixLength> -t <lifetime in seconds>\r\n"
        "   thr nwkdata add extroute -p <Prefix> - len <prefixLength> -t <lifetime in seconds>\r\n"
        "   thr nwkdata remove -p <Prefix> - len <prefixLength>\r\n"
        "   thr nwkdata removeall\r\n",
        SHELL_ThrCommands, SHELL_IGNORE_PARAMETER_COUNT, {0}
    },
    {
        "ifconfig",
        "\r\n\"ifconfig\": Displays all interfaces and addresses configured on the device\r\n",
        SHELL_Ifconfig, 0
    },
    {
        "mcastgroup",
        "\r\n\"mcastgroup\": Multicast groups management\r\n"
        "   mcastgroup show - displays all joined multicast groups\r\n"
        "   mcastgroup add <IP group address> <interface id> - joins to a new multicast group\r\n"
        "   mcastgroup leave <IP group address> <interface id> - leaves a multicast group\r\n"
        "Warning! Do not remove addresses that may affect stack's behavior!\r\n",
        SHELL_MulticastGroups, SHELL_IGNORE_PARAMETER_COUNT, {0}
    },
    {
        "ping",
        "\r\n\"ping\": IP Stack ping tool\r\n"
        "   ping <ip address> -i <timeout> -c <count> -s <size> -t <continuous ping> -S <source IP address>\r\n",
        SHELL_Ping, SHELL_IGNORE_PARAMETER_COUNT, {0}
    },
    {
        "coap",
        "\r\n\"coap\": Send a CoAP message\r\n"
        "   coap <reqtype: CON/NON> <reqcode (GET/POST/PUT/DELETE)> <IP addr dest> <URI path> <payload ASCII>\r\n"
        "   Example: coap CON POST 2001::1 /led on \r\n"
        "         coap CON POST 2001::1 /led off \r\n"
        "         coap CON POST 2001::1 /led toggle \r\n"
        "         coap CON POST 2001::1 /led flash\r\n"
        "         coap CON POST 2001::1 /led rgb r255 g255 b255\r\n"
        "         coap CON GET 2001::1 /temp\r\n",
        SHELL_CoapSend, SHELL_IGNORE_PARAMETER_COUNT, {0}
    },
    {
        "reboot",
        "\r\n\"reboot\": MCU reset\r\n",
        SHELL_Reboot, 0
    },
    {
        "factoryreset",
        "\r\n\"factoryreset\": Reset MCU to factory defaults\r\n",
        SHELL_FactoryResetReq, 0
    },
    {
        "autostart",
        "\r\n\"autostart\": Start the device as a Leader and allow all joiners with PSKd `THREAD`\r\n"
        "   autostart -c <802.15.4. channel>\r\n",
        SHELL_Autostart, 2
    },
    {
        SHELL_ETX,
        "\r\n\"Ctrl-C + ENTER\": Stop commands that run indefinitely, such as ping\r\n\r\n",
        SHELL_Resume, 0
    }
};

/***************************************************************************************************
****************************************************************************************************
Public functions
****************************************************************************************************
***************************************************************************************************/
/*!*************************************************************************************************
\fn     void SHELL_ThreadIP_Init(void)
\brief  This function is used to initialize the SHELL commands module.
***************************************************************************************************/
void SHELL_ThreadIP_Init(void)
{
    if (!mShellCommandsEnabled)
    {
        TMR_TimeStampInit();

        SHELL_EvMonitorInit();

        /* Register functions */
        for (int i = 0; i < NumberOfElements(aShellCommands); i++)
        {
            SHELL_RegisterCommand(threadip_shellHandle, &aShellCommands[i]);
        }

        mShellCommandsEnabled = TRUE;
    }
}

/*!*************************************************************************************************
\fn     void SHELL_ThrNwkCreate(void *param)
\brief  This function is used to create a Thread network.

\param  [in]    param    Not used
***************************************************************************************************/
void SHELL_ThrNwkCreate
(
    void *param
)
{
    THR_CreateNwkRequest_t req;
    req.InstanceID = gThreadInstanceId;

    if (THR_CreateNwkRequest(&req, THR_FSCI_IF) != MEM_SUCCESS_c)
    {
        shell_write("No memory!\n\r");
    }
}

/*!*************************************************************************************************
\fn     void SHELL_ThrStartCommissioner(void *param)
\brief  This function is used to start a commissioner.

\param  [in]    param    Not used
***************************************************************************************************/
void SHELL_ThrStartCommissioner
(
    void *param
)
{
    MESHCOP_StartCommissionerRequest_t req;
    req.InstanceId = gThreadInstanceId;

    if (MESHCOP_StartCommissionerRequest(&req, THR_FSCI_IF) != MEM_SUCCESS_c)
    {
        shell_write("No memory!\n\r");
    }
}

/*!*************************************************************************************************
\fn     void SHELL_ThrSetChannel(void *param)
\brief  This function is used to set the 802.15.4 channel.

\param  [in]    param    Not used
***************************************************************************************************/
void SHELL_ThrSetChannel(void *param)
{
    THR_SetAttrRequest_t req;
    req.InstanceId = gThreadInstanceId;
    req.AttributeId = THR_SetAttrRequest_AttributeId_Channel;
    req.Index = 0;
    req.AttributeValue.Channel.AttrSize = 1;
    req.AttributeValue.Channel.Value = gAutostartChannel;
    shell_printf("\r\nSet channel to %d\r\n", gAutostartChannel);
    THR_SetAttrRequest(&req, THR_FSCI_IF);
}

/*!*************************************************************************************************
\fn     void SHELL_NwkScanPrint(THR_EventNwkScanConfirm_t *pScanResults)
\brief  This function is used to network scan information.

\param  [in]    pScanResults   The scan results
***************************************************************************************************/
void SHELL_NwkScanPrint
(
    THR_EventNwkScanConfirm_t *pScanResults
)
{
    SHELL_NEWLINE();

    /* Handle the network scan result here */
    if (pScanResults)
    {
        shell_printf("Scan duration: %d, Scan mask: %04X\n\r", pScanResults->ScanDuration, pScanResults->ScanChannelMask);

        if (pScanResults->ScanType == THR_EventNwkScanConfirm_ScanType_EnergyDetect)
        {
            shell_write("\n\rEnergy on channel 11 to 26: 0x");
            shell_print_buffer(pScanResults->Data.EnergyDetect.EnergyDetectList,
                               pScanResults->Data.EnergyDetect.EnergyDetectEntries);
            shell_printf("\n\r");

            MEM_BufferFree(pScanResults->Data.EnergyDetect.EnergyDetectList);
        }

        if (pScanResults->ScanType == THR_EventNwkScanConfirm_ScanType_ActiveScan)
        {
            for (uint8_t i = 0; i < pScanResults->Data.ActiveScan.NwkDiscoveryEntries; i++)
            {
                shell_printf("\rThread Network: %d\n\r", i);
                shell_printf("\r\tPAN ID: 0x%x \n\r",           pScanResults->Data.ActiveScan.NwkDiscoveryList[i].PanId);
                shell_printf("\r\tChannel: %d \n\r",            pScanResults->Data.ActiveScan.NwkDiscoveryList[i].Channel);
                shell_printf("\r\tLQI: %d\n\r",                 pScanResults->Data.ActiveScan.NwkDiscoveryList[i].Reserved);
                shell_printf("\r\tReceived beacons: %d\n\r",    pScanResults->Data.ActiveScan.NwkDiscoveryList[i].NumOfRcvdBeacons);
            }

            MEM_BufferFree(pScanResults->Data.ActiveScan.NwkDiscoveryList);
        }

        if (pScanResults->ScanType == THR_EventNwkScanConfirm_ScanType_EnergyDetectAndActiveScan)
        {
            shell_write("\n\rEnergy on channel 11 to 26: 0x");
            shell_print_buffer(pScanResults->Data.EnergyDetectAndActiveScan.EnergyDetectList,
                               pScanResults->Data.EnergyDetectAndActiveScan.EnergyDetectEntries);
            shell_printf("\n\r");
            MEM_BufferFree(pScanResults->Data.EnergyDetectAndActiveScan.EnergyDetectList);

            for (uint8_t i = 0; i < pScanResults->Data.EnergyDetectAndActiveScan.NwkDiscoveryEntries; i++)
            {
                shell_printf("\rThread Network: %d\n\r", i);
                shell_printf("\r\tPAN ID: 0x%x \n\r",           pScanResults->Data.EnergyDetectAndActiveScan.NwkDiscoveryList[i].PanId);
                shell_printf("\r\tChannel: %d \n\r",            pScanResults->Data.EnergyDetectAndActiveScan.NwkDiscoveryList[i].Channel);
                shell_printf("\r\tLQI: %d\n\r",                 pScanResults->Data.EnergyDetectAndActiveScan.NwkDiscoveryList[i].Reserved);
                shell_printf("\r\tReceived beacons: %d\n\r",    pScanResults->Data.EnergyDetectAndActiveScan.NwkDiscoveryList[i].NumOfRcvdBeacons);
            }

            MEM_BufferFree(pScanResults->Data.EnergyDetectAndActiveScan.NwkDiscoveryList);
        }

        shell_refresh();
    }
}

/*!*************************************************************************************************
\fn     void SHELL_PrintNeighborInfo(void *param)
\brief  This function is used to print information about a specific neighbor.

\param  [in]    param    THR_GetNeighborInfoConfirm_t
***************************************************************************************************/
void SHELL_PrintNeighborInfo
(
    void *param
)
{
    THR_GetNeighborInfoConfirm_t *neighbor = (THR_GetNeighborInfoConfirm_t *)param;

    uint64_t reversedExtAddr = neighbor->NeighborInfo.Success.ExtendedAddress;

    NWKU_SwapArrayBytes((uint8_t *)&reversedExtAddr, 8);
    shell_printf("Extended Address: 0x");
    shell_print_buffer((uint8_t *)&reversedExtAddr, 8);
    shell_printf("\n\r");

    shell_printf("Short Address:    0x%04X\n\r", neighbor->NeighborInfo.Success.ShortAddress);
    shell_printf("Last communication: %d seconds ago\n\r",
                 ((uint32_t)TmrMicrosecondsToSeconds(TMR_GetTimestamp()) - neighbor->NeighborInfo.Success.LastCommTime));
    shell_printf("InLinkMargin: %d\n\r", neighbor->NeighborInfo.Success.InRSSI);
    shell_printf("Device Timeout value: %d seconds\n\r", neighbor->NeighborInfo.Success.Timeoutsec);
}

/*!*************************************************************************************************
\fn     void SHELL_PrintNeighborTbl(void *param)
\brief  This function is used to print neighbor table.

\param  [in]    param    THR_GetNeighborTableConfirm
***************************************************************************************************/
void SHELL_PrintNeighborTbl
(
    void *param
)
{
    THR_GetNeighborTableConfirm_t *tbl = (THR_GetNeighborTableConfirm_t *)param;

    if (tbl->NoOfElements == 0)
    {
        shell_printf("No neighbors connected!\n\r");
    }

    shell_printf("Index Extended Address     ShortAddr  LastTime LastRSSI\n\r");

    for (uint8_t i = 0; i < tbl->NoOfElements; i++)
    {
        uint64_t reversedExtAddr = tbl->NeighborEntries[i].ExtendedAddress;

        NWKU_SwapArrayBytes((uint8_t *)&reversedExtAddr, 8);
        shell_printf("\r%d     ", i);
        shell_printf("0x");
        shell_print_buffer((uint8_t *)&reversedExtAddr, 8);

        shell_printf("   ");
        shell_printf("0x%04X", tbl->NeighborEntries[i].ShortAddress);

        shell_printf("     ");
        shell_printf("%d", ((uint32_t)TmrMicrosecondsToSeconds(TMR_GetTimestamp()) - tbl->NeighborEntries[i].LastCommTime));

        shell_printf("\t%d  ", tbl->NeighborEntries[i].LastRSSI);
        shell_printf("\n\r");
    }

    MEM_BufferFree(tbl->NeighborEntries);
}

/*!*************************************************************************************************
\fn     void SHELL_PrintRoutingTbl(void *param)
\brief  This function is used to print routing table.

\param  [in]    param    THR_GetRoutingTableConfirm
***************************************************************************************************/
void SHELL_PrintRoutingTbl
(
    void *param
)
{
    THR_GetRoutingTableConfirm_t *tbl = (THR_GetRoutingTableConfirm_t *)param;

    if (tbl->NoOfElements == 0)
    {
        shell_printf("Router ID set is empty!\n\r");
        return;
    }

    shell_printf("ID Sequence: %d\n\r", tbl->IdSequenceNb);
    shell_printf("Router ID Mask: ");
    shell_print_buffer((uint8_t *)&tbl->RouterIDMask, 8);
    shell_printf("\n\r");

    shell_printf("RouterID    Short Address    Next Hop    Cost    NOut    NIn \n\r");

    /* count the number of allocated router IDs */
    for (uint32_t i = 0; i < tbl->NoOfElements; i++)
    {
        shell_printf("%d", tbl->RoutingEntries[i].RouterID);

        shell_printf("           ");
        shell_printf("0x%04X", tbl->RoutingEntries[i].ShortAddress);

        shell_printf("           ");
        shell_printf("0x%04X", tbl->RoutingEntries[i].NextHop);

        shell_printf("      ");
        shell_printf("%d", tbl->RoutingEntries[i].Cost);

        shell_printf("       ");
        shell_printf("%d", tbl->RoutingEntries[i].nOut);

        shell_printf("       ");
        shell_printf("%d", tbl->RoutingEntries[i].nIn);

        shell_printf("\n\r");
    }

    MEM_BufferFree(tbl->RoutingEntries);
}

/*!*************************************************************************************************
\fn     static attrRef_t SHELL_GetAttrRefFromId(THR_GetAttrConfirm_t *evt)
\brief  Helper function for SHELL_PrintGetAttrRsp.

\param  [in]    param    THR_GetAttrConfirm_t
***************************************************************************************************/
static attrRef_t SHELL_GetAttrRefFromId
(
    THR_GetAttrConfirm_t *evt
)
{
    attrRef_t res;
    res.pValue = NULL;
    res.requireFree = FALSE;

    switch (evt->AttributeId)
    {
        case THR_GetAttrConfirm_AttributeId_RandomExtendedAddr:
            res.pValue = &(evt->AttributeValue.RandomExtendedAddr);
            break;

        case THR_GetAttrConfirm_AttributeId_ShortAddress:
            res.pValue = &(evt->AttributeValue.ShortAddress);
            break;

        case THR_GetAttrConfirm_AttributeId_ScanChannelMask:
            res.pValue = &(evt->AttributeValue.ScanChannelMask);
            break;

        case THR_GetAttrConfirm_AttributeId_ScanDuration:
            res.pValue = &(evt->AttributeValue.ScanDuration);
            break;

        case THR_GetAttrConfirm_AttributeId_Channel:
            res.pValue = &(evt->AttributeValue.Channel);
            break;

        case THR_GetAttrConfirm_AttributeId_ShortPanId:
            res.pValue = &(evt->AttributeValue.ShortPanId);
            break;

        case THR_GetAttrConfirm_AttributeId_ExtendedPanId:
            res.pValue = evt->AttributeValue.ExtendedPanId;
            res.requireFree = TRUE;
            break;

        case THR_GetAttrConfirm_AttributeId_PermitJoin:
            res.pValue = &(evt->AttributeValue.PermitJoin);
            break;

        case THR_GetAttrConfirm_AttributeId_RxOnIdle:
            res.pValue = &(evt->AttributeValue.RxOnIdle);
            break;

        case THR_GetAttrConfirm_AttributeId_SedPollInterval:
            res.pValue = &(evt->AttributeValue.SedPollInterval);
            break;

        case THR_GetAttrConfirm_AttributeId_UniqueExtendedAddress:
            res.pValue = &(evt->AttributeValue.UniqueExtendedAddress);
            break;

        case THR_GetAttrConfirm_AttributeId_VendorName:
            res.pValue = evt->AttributeValue.VendorName;
            res.requireFree = TRUE;
            break;

        case THR_GetAttrConfirm_AttributeId_ModelName:
            res.pValue = evt->AttributeValue.ModelName;
            res.requireFree = TRUE;
            break;

        case THR_GetAttrConfirm_AttributeId_SwVersion:
            res.pValue = evt->AttributeValue.SwVersion;
            res.requireFree = TRUE;
            break;

        case THR_GetAttrConfirm_AttributeId_StackVersion:
            res.pValue = &(evt->AttributeValue.StackVersionStruct);
            break;

        case THR_GetAttrConfirm_AttributeId_NwkCapabilities:
            res.pValue = &(evt->AttributeValue.NwkCapabilities);
            break;

        case THR_GetAttrConfirm_AttributeId_NwkName:
            res.pValue = evt->AttributeValue.NwkName;
            res.requireFree = TRUE;
            break;

        case THR_GetAttrConfirm_AttributeId_DeviceType:
            res.pValue = &(evt->AttributeValue.DeviceType);
            break;

        case THR_GetAttrConfirm_AttributeId_IsDevConnected:
            res.pValue = &(evt->AttributeValue.IsDevConnected);
            break;

        case THR_GetAttrConfirm_AttributeId_IsDevCommissioned:
            res.pValue = &(evt->AttributeValue.IsDevCommissioned);
            break;

        case THR_GetAttrConfirm_AttributeId_PartitionId:
            res.pValue = &(evt->AttributeValue.PartitionId);
            break;

        case THR_GetAttrConfirm_AttributeId_DeviceRole:
            res.pValue = &(evt->AttributeValue.DeviceRole);
            break;

        case THR_GetAttrConfirm_AttributeId_Security_NwkMasterKey:
            res.pValue = evt->AttributeValue.Security_NwkMasterKey;
            res.requireFree = TRUE;
            break;

        case THR_GetAttrConfirm_AttributeId_Security_NwkKeySeq:
            res.pValue = &(evt->AttributeValue.Security_NwkKeySeq);
            break;

        case THR_GetAttrConfirm_AttributeId_Security_PSKc:
            res.pValue = evt->AttributeValue.Security_PSKc;
            res.requireFree = TRUE;
            break;

        case THR_GetAttrConfirm_AttributeId_Security_PSKd:
            res.pValue = evt->AttributeValue.Security_PSKd;
            res.requireFree = TRUE;
            break;

        case THR_GetAttrConfirm_AttributeId_VendorData:
            res.pValue = evt->AttributeValue.VendorData;
            res.requireFree = TRUE;
            break;

        case THR_GetAttrConfirm_AttributeId_MLPrefix:
            res.pValue = &(evt->AttributeValue.MLPrefix);
            break;

        case THR_GetAttrConfirm_AttributeId_MacFilteringEntry:
            res.pValue = &(evt->AttributeValue.MacFilteringEntry);
            break;

        case THR_GetAttrConfirm_AttributeId_Security_KeyRotationInterval:
            res.pValue = &(evt->AttributeValue.Security_KeyRotationInterval);
            break;

        case THR_GetAttrConfirm_AttributeId_ChildAddrMask:
            res.pValue = evt->AttributeValue.ChildAddrMask;
            res.requireFree = TRUE;
            break;

        case THR_GetAttrConfirm_AttributeId_ChildSEDTimeout:
            res.pValue = &(evt->AttributeValue.ChildSEDTimeout);
            break;

        case THR_GetAttrConfirm_AttributeId_ChildEDTimeout:
            res.pValue = &(evt->AttributeValue.ChildEDTimeout);
            break;

        case THR_GetAttrConfirm_AttributeId_EndDevice_ChildEDReqFullNwkData:
            res.pValue = &(evt->AttributeValue.EndDevice_ChildEDReqFullNwkData);
            break;

        case THR_GetAttrConfirm_AttributeId_EndDevice_IsFastPollEnabled:
            res.pValue = &(evt->AttributeValue.EndDevice_IsFastPollEnabled);
            break;

        case THR_GetAttrConfirm_AttributeId_SleepyEndDevice_FastPollInterval:
            res.pValue = &(evt->AttributeValue.SleepyEndDevice_FastPollInterval);
            break;

        case THR_GetAttrConfirm_AttributeId_JoinLqiThreshold:
            res.pValue = &(evt->AttributeValue.JoinLqiThreshold);
            break;

        case THR_GetAttrConfirm_AttributeId_ProvisioningURL:
            res.pValue = evt->AttributeValue.ProvisioningURL;
            res.requireFree = TRUE;
            break;

        case THR_GetAttrConfirm_AttributeId_SelectBestChannelEDThreshold:
            res.pValue = &(evt->AttributeValue.SelectBestChannelEDThreshold);
            break;

        case THR_GetAttrConfirm_AttributeId_CommissionerMode:
            res.pValue = &(evt->AttributeValue.CommissionerMode);
            break;

        case THR_GetAttrConfirm_AttributeId_BorderRouter_BrPrefixEntry:
            res.pValue = &(evt->AttributeValue.BorderRouter_BrPrefixEntry);
            break;

        case THR_GetAttrConfirm_AttributeId_SteeringData:
            res.pValue = evt->AttributeValue.SteeringData;
            res.requireFree = TRUE;
            break;

        case THR_GetAttrConfirm_AttributeId_Security_KeySwitchGuardTime:
            res.pValue = &(evt->AttributeValue.Security_KeySwitchGuardTime);
            break;

        case THR_GetAttrConfirm_AttributeId_ParentHoldTime:
            res.pValue = &(evt->AttributeValue.ParentHoldTime);
            break;

        case THR_GetAttrConfirm_AttributeId_Security_Policy:
            res.pValue = &(evt->AttributeValue.Security_Policy);
            break;

        case THR_GetAttrConfirm_AttributeId_NVM_RestoreAutoStart:
            res.pValue = &(evt->AttributeValue.NVM_RestoreAutoStart);
            break;

        case THR_GetAttrConfirm_AttributeId_NVM_Restore:
            res.pValue = &(evt->AttributeValue.NVM_Restore);
            break;

        case THR_GetAttrConfirm_AttributeId_SlaacPolicy:
            res.pValue = &(evt->AttributeValue.SlaacPolicy);
            break;

        case THR_GetAttrConfirm_AttributeId_IeeeExtendedAddr:
            res.pValue = &(evt->AttributeValue.IeeeExtendedAddr);
            break;

        case THR_GetAttrConfirm_AttributeId_LeaderWeight:
            res.pValue = &(evt->AttributeValue.LeaderWeight);
            break;

        case THR_GetAttrConfirm_AttributeId_HashIeeeAddr:
            res.pValue = &(evt->AttributeValue.HashIeeeAddr);
            break;

        case THR_GetAttrConfirm_AttributeId_BorderRouter_BrGlobalOnMeshPrefix:
            res.pValue = &(evt->AttributeValue.BorderRouter_BrGlobalOnMeshPrefix);
            break;

        case THR_GetAttrConfirm_AttributeId_BorderRouter_BrDefaultRouteOnMeshPrefix:
            res.pValue = &(evt->AttributeValue.BorderRouter_BrDefaultRouteOnMeshPrefix);
            break;

        case THR_GetAttrConfirm_AttributeId_BorderRouter_BrExternalIfPrefix:
            res.pValue = &(evt->AttributeValue.BorderRouter_BrExternalIfPrefix);
            break;

        case THR_GetAttrConfirm_AttributeId_ActiveTimestamp:
            res.pValue = &(evt->AttributeValue.ActiveTimestamp);
            break;

        case THR_GetAttrConfirm_AttributeId_CommissionerId:
            res.pValue = evt->AttributeValue.CommissionerID;
            res.requireFree = TRUE;
            break;

        case THR_GetAttrConfirm_AttributeId_JoinerPort:
            res.pValue = &(evt->AttributeValue.JoinerPort);
            break;

        case THR_GetAttrConfirm_AttributeId_CommissionerUdpPort:
            res.pValue = &(evt->AttributeValue.CommissionerUdpPort);
            break;

        default:
            break;
    }

    return res;
}

/*!*************************************************************************************************
\fn     void SHELL_PrintGetAttrRsp(THR_GetAttrConfirm_t *evt)
\brief  This function is used to print the value of an attribute.

\param  [in]    param    THR_GetAttrConfirm_t
***************************************************************************************************/
void SHELL_PrintGetAttrRsp
(
    THR_GetAttrConfirm_t *evt
)
{
    uint64_t value = 0;
    char strValue[64] = { 0 };

    attrRef_t attrRef = SHELL_GetAttrRefFromId(evt);

    if (attrRef.pValue == NULL)
    {
        return;
    }

    for (uint8_t i = 0; i < NumberOfElements(aShellThrAttr); i++)
    {
        if (aShellThrAttr[i].attrId == evt->AttributeId)
        {
            /* check if a function handles this */
            if (NULL != aShellThrAttr[i].pfHandleAttr)
            {
                aShellThrAttr[i].pfHandleAttr(attrRef.pValue);
            }

            else
            {
                switch (aShellThrAttr[i].valueType)
                {
                    case gDecimal_c:
                        FLib_MemCpy(&value, attrRef.pValue, evt->AttrSize);
                        shell_printf("%s: %d\n\r", aShellThrAttr[i].pAttrName, value);
                        break;

                    case gHex_c:
                        FLib_MemCpy(&value, attrRef.pValue, evt->AttrSize);

                        if (evt->AttrSize <= 4)
                        {
                            shell_printf("%s: 0x%04x\n\r", aShellThrAttr[i].pAttrName, value);
                        }
                        else
                        {
                            shell_printf("%s: 0x%08x\n\r", aShellThrAttr[i].pAttrName, value);
                        }

                        break;

                    case gArray_c:
                        shell_printf("%s: 0x", aShellThrAttr[i].pAttrName);
                        shell_print_buffer((uint8_t *)attrRef.pValue, evt->AttrSize);
                        shell_printf("\n\r");
                        break;

                    case gString_c:

                        if (aShellThrAttr[i].attrId == THR_GetAttrConfirm_AttributeId_Security_PSKc)
                        {
                            shell_printf("%s: 0x", "Binary PSKc");
                            shell_print_buffer((uint8_t *)attrRef.pValue, evt->AttrSize);
                            shell_printf("\n\r");
                        }
                        else
                        {
                            FLib_MemCpy(strValue, attrRef.pValue, evt->AttrSize);
                            shell_printf("%s: %s\n\r", aShellThrAttr[i].pAttrName, strValue);
                        }

                        break;

                    case gHexReversed_c:
                    {
                        FLib_MemCpy(&value, attrRef.pValue, evt->AttrSize);
                        NWKU_SwapArrayBytes((uint8_t *)&value, 8);
                        shell_printf("%s: 0x", aShellThrAttr[i].pAttrName);
                        shell_print_buffer((uint8_t *)&value, 8);
                        shell_printf("\n\r");
                    }
                    break;

                    default:
                        break;
                }
            }

            break;
        }
    }

    if (attrRef.requireFree)
    {
        MEM_BufferFree(attrRef.pValue);
    }
}

/*!*************************************************************************************************
\fn     void SHELL_PrintIfconfigAllRsp(NWKU_IfconfigAllResponse_t *evt)
\brief  This function is used to print the IPv6 addresses of the black-box.

\param  [in]    param    NWKU_IfconfigAllResponse_t
***************************************************************************************************/
void SHELL_PrintIfconfigAllRsp
(
    NWKU_IfconfigAllResponse_t *evt
)
{
    char addrStr[INET6_ADDRSTRLEN];
    uint8_t temp[INET_ADDRSTRLEN];  // container for the reversed bytes

    for (uint8_t i = 0; i < evt->CountInterfaces; i++)
    {
        // protection against badly formatted response
        if (evt->InterfaceIDs[i].InterfaceID != i)
        {
            SHELL_NEWLINE();
            // shell_write("No address available!");
            break;
        }

        shell_printf("\n\rInterface #%d", i);

        for (uint8_t j = 0; j < evt->InterfaceIDs[i].CountIpAddresses; j++)
        {
            FLib_MemCpyReverseOrder(temp, &evt->InterfaceIDs[i].Addresses[j * INET_ADDRSTRLEN], INET_ADDRSTRLEN);
            ntop(AF_INET6, (ipAddr_t *)temp, addrStr, INET6_ADDRSTRLEN);
            shell_printf("\n\r\tAddress #%d: %s", j + 1, addrStr);
        }

        MEM_BufferFree(evt->InterfaceIDs[i].Addresses);
    }

    MEM_BufferFree(evt->InterfaceIDs);
}

/*!*************************************************************************************************
\fn     int32_t SHELL_Resume(shell_handle_t context, int32_t argc, char **argv)
\brief  This function is used to stop commands that run indefinitely and resume SHELL execution.
***************************************************************************************************/
int32_t SHELL_Resume
(
    shell_handle_t context,
    int32_t argc,
    char **argv
)
{
    gContinuousPing = FALSE;

    if (pSrcIpAddr)
    {
        MEM_BufferFree(pSrcIpAddr);
        pSrcIpAddr = NULL;
    }

    return CMD_RET_SUCCESS;
}

/*!*************************************************************************************************
\fn     void PING_EchoReplyReceive(void *pParam)
\brief  Interface function for the user application. It handles a received Ping Echo Reply message.

\param  [in]    pParam    not used
***************************************************************************************************/
void PING_EchoReplyReceive
(
    void *pParam
)
{
    uint64_t tempTimestamp;

    if (IP_IsAddrIPv6(&mDstIpAddr))
    {
        ntop(AF_INET6, &mDstIpAddr, mAddrStr, INET6_ADDRSTRLEN);
    }
    else
    {
        ntop(AF_INET, &mDstIpAddr, mAddrStr, INET_ADDRSTRLEN);
    }

    shell_write("Reply from ");
    shell_write(mAddrStr);
    shell_printf(": bytes=%d", mPingSize);
    shell_write(" time=");
    tempTimestamp = TMR_GetTimestamp();
    tempTimestamp -= mPingTimestamp;
    tempTimestamp /= 1000;
    shell_printf("%dms", (uint32_t)tempTimestamp);

    /* Continuous ping: restart */
    if (gContinuousPing)
    {
        /* send next ping after PING_DELAY */
        OSA_TimeDelay(PING_DELAY);
        PING_RetransmitHandle(NULL);
    }
    else
    {
        SHELL_Resume(NULL, 0, NULL);
    }
}

/*!*************************************************************************************************
\fn     void PING_HandleTimeout(void *param)
\brief  This function is used to handle the ping timeout.

\param  [in]    param    Not used
***************************************************************************************************/
void PING_HandleTimeout
(
    void *param
)
{
    /* Ping reply was not received */
    shell_write("Request timed out.");

    if (gContinuousPing)
    {
        if (mPingCounter > 0 && mPingCounter != THR_ALL_FFs16)
        {
            mPingCounter--;
        }

        /* Counter has reached 0: stop pinging */
        if (mPingCounter == 0)
        {
            gContinuousPing = FALSE;
            FLib_MemSet(&mPingReq, 0, sizeof(NWKU_PingRequest_t));
        }

        PING_RetransmitHandle(NULL);
    }
    else
    {
        SHELL_Resume(NULL, 0, NULL);
    }
}

/*!************************************************************************************************
\fn     void SHELL_PrintCoapMsg(NWKU_CoapMsgReceivedIndication_t *evt)
\brief  This function is used to print info about a received CoAP message.

\param  [in]    param    NWKU_CoapMsgReceivedIndication_t
***************************************************************************************************/
void SHELL_PrintCoapMsg
(
    NWKU_CoapMsgReceivedIndication_t *evt
)
{
    SHELL_NEWLINE();
    shell_write("CoAP Message Received -> ");

    if (!strncmp(evt->URIpath, APP_TEMP_URI_PATH, strlen(APP_TEMP_URI_PATH)))
    {
        shell_writeN((char *)evt->Payload, evt->PayloadLength);
    }

    else if (!strncmp(evt->URIpath, APP_LED_URI_PATH, strlen(APP_LED_URI_PATH)))
    {
        shell_writeN((char *)evt->Payload, evt->PayloadLength);
        // APP_ProcessLedCmd(evt->Payload, evt->PayloadLength); // TODO
    }

    else if (evt->PayloadLength > 0)
    {
        shell_write("payload -> ");
        shell_writeN((char *)evt->Payload, evt->PayloadLength);
        shell_write(", hex -> ");
        shell_print_buffer(evt->Payload, evt->PayloadLength);
    }
}

/*!************************************************************************************************
\fn     void SHELL_PrintMcastGroups(NWKU_McastGroupShowResponse_t *evt)
\brief  This function is used to print info about the registered multicast groups.

\param  [in]    param    NWKU_McastGroupShowResponse_t
***************************************************************************************************/
void SHELL_PrintMcastGroups
(
    NWKU_McastGroupShowResponse_t *evt
)
{
    char groupStr[INET6_ADDRSTRLEN] = { 0 };
    ipAddr_t temp;

    if (evt->CountIpAddresses == 0)
    {
        SHELL_NEWLINE();
        shell_write("No multicast groups registered!\n\r");
    }

    for (uint8_t i = 0; i < evt->CountIpAddresses; i++)
    {
        FLib_MemCpyReverseOrder(temp.addr8, &evt->Addresses[sizeof(ipAddr_t) * i], sizeof(ipAddr_t));
        ntop(AF_INET6, &temp, groupStr, INET6_ADDRSTRLEN);
        shell_printf("\n\r%s", groupStr);
        FLib_MemSet(groupStr, 0, INET6_ADDRSTRLEN);
    }

    MEM_BufferFree(evt->Addresses);
}

/***************************************************************************************************
****************************************************************************************************
* Private functions
****************************************************************************************************
***************************************************************************************************/
/*!*************************************************************************************************
\fn     void SHELL_EvMonitorInit(instanceId_t instanceId)
\brief  This function is used as a greeting shell message.
***************************************************************************************************/
static void SHELL_EvMonitorInit(void)
{
    /* Init demo application  */
    shell_write("\n\rSHELL starting...\n");
    shell_write("\rEnter \"thr join\" to join a network, or \"thr create\" to start new network\n");
    shell_write("\rEnter \"help\" for other commands\n");
    shell_refresh();
}

/*!*************************************************************************************************
\private
\fn     static void SHELL_ThrNwkJoin(void *param)
\brief  This function is used to join to a Thread network.

\param  [in]    param    Not used
***************************************************************************************************/
static void SHELL_ThrNwkJoin
(
    void *param
)
{
    THR_JoinRequest_t req;
    req.InstanceID = gThreadInstanceId;
    req.discoveryMethod = THR_JoinRequest_discoveryMethod_gUseThreadDiscovery_c;

    if (THR_JoinRequest(&req, THR_FSCI_IF) != MEM_SUCCESS_c)
    {
        shell_write("No memory!\n\r");
    }
}

/*!************************************************************************************************
*
\private
\fn     static void SHELL_ThrStopCommissioner(void *param)
\brief  This function is used to stop a commissioner..

\param  [in]    param    Not used
***************************************************************************************************/
static void SHELL_ThrStopCommissioner
(
    void *param
)
{
    MESHCOP_StopCommissionerRequest_t req;
    req.InstanceId = gThreadInstanceId;

    if (MESHCOP_StopCommissionerRequest(&req, THR_FSCI_IF) != MEM_SUCCESS_c)
    {
        shell_write("No memory!\n\r");
    }
}

/*!*************************************************************************************************
\fn     static void SHELL_PrintStatus(thrStatus_t statusCode)
\brief  This function is used for printing in shell terminal the status of the input operation.

\param  [in]    statusCode    The code of the status to be printed
***************************************************************************************************/
static void SHELL_PrintStatus
(
    thrStatus_t statusCode
)
{
    if (gThrStatus_Success_c != statusCode)
    {
        shell_write(" An error has occurred!");
    }
}

/*!************************************************************************************************
\private
\fn     static int32_t SHELL_ThrCommands(shell_handle_t context, int32_t argc, char **argv)
\brief  This function is used to send commands for Thread network.

\param  [in]    context     Shell environment
\param  [in]    argc        Number of arguments the command was called with
\param  [in]    argv        Pointer to a list of pointers to the arguments

\return         int32_t     Status of the command
***************************************************************************************************/
static int32_t SHELL_ThrCommands
(
    shell_handle_t context,
    int32_t argc,
    char **argv
)
{
    thrStatus_t status = gThrStatus_EntryNotFound_c;

    if (!strcmp(argv[1], "create"))
    {
        shell_write("Creating network...");
        SHELL_ThrNwkCreate(NULL);
    }

    else if (!strcmp(argv[1], "join"))
    {
        shell_write("Joining network...");
        SHELL_ThrNwkJoin(NULL);
    }

    else if (!strcmp(argv[1], "detach"))
    {
        shell_write("Detaching from network...");

        THR_DisconnectRequest_t req;
        req.InstanceID = gThreadInstanceId;

        if (THR_DisconnectRequest(&req, THR_FSCI_IF) != MEM_SUCCESS_c)
        {
            status = gThrStatus_NoMem_c;
        }
        else
        {
            status = gThrStatus_Success_c;
        }
    }

    else if (!strcmp(argv[1], "commissioner"))
    {
        if (!strcmp(argv[2], "start"))
        {
            SHELL_ThrStartCommissioner(NULL);
        }
        else if (!strcmp(argv[2], "stop"))
        {
            SHELL_ThrStopCommissioner(NULL);
        }
    }

    else if (!strcmp(argv[1], "joiner"))
    {
        if (!strcmp(argv[2], "add"))
        {
            uint8_t eui[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            int32_t pskLen = strlen(argv[3]);
            uint8_t psk[32];

            FLib_MemSet(&psk, 0, 32);
            FLib_MemCpy(&psk, argv[3], pskLen);

            if (NULL != argv[4])
            {
                FLib_MemSet(&eui, 0, 8);
                shell_read_buffer(argv[4] + 2, eui, 8);
            }

            MESHCOP_AddExpectedJoinerRequest_t req;
            req.InstanceId = gThreadInstanceId;
            req.Selected = TRUE;
            req.EuiType = MESHCOP_AddExpectedJoinerRequest_EuiType_LongEUI;
            req.EUI.LongEUI = NWKU_TransformArrayToUint64(eui);
            req.PSKdSize = pskLen;
            req.PSKd = (char *)psk;

            if (MESHCOP_AddExpectedJoinerRequest(&req, THR_FSCI_IF) != MEM_SUCCESS_c)
            {
                status = gThrStatus_NoMem_c;
            }
            else
            {
                status = gThrStatus_Success_c;
            }
        }

        else if (!strcmp(argv[2], "remove"))
        {
            uint8_t eui[8];

            FLib_MemSet(&eui, 0, 8);

            if (NULL != argv[3])
            {
                shell_read_buffer(argv[3] + 2, eui, 8);
            }

            MESHCOP_RemoveExpectedJoinerRequest_t req;
            req.InstanceId = gThreadInstanceId;
            req.EuiType = MESHCOP_RemoveExpectedJoinerRequest_EuiType_LongEUI;
            req.EUI.LongEUI = NWKU_TransformArrayToUint64(eui);

            if (MESHCOP_RemoveExpectedJoinerRequest(&req, THR_FSCI_IF) != MEM_SUCCESS_c)
            {
                status = gThrStatus_NoMem_c;
            }
            else
            {
                status = gThrStatus_Success_c;
            }
        }

        else if (!strcmp(argv[2], "removeall"))
        {
            MESHCOP_RemoveAllExpectedJoinersRequest_t req;
            req.InstanceId = gThreadInstanceId;

            if (MESHCOP_RemoveAllExpectedJoinersRequest(&req, THR_FSCI_IF) != MEM_SUCCESS_c)
            {
                status = gThrStatus_NoMem_c;
            }
            else
            {
                status = gThrStatus_Success_c;
            }
        }
    }

    else if (!strcmp(argv[1], "sync"))
    {
        if (!strcmp(argv[2], "steering"))
        {
            MESHCOP_SyncSteeringDataRequest_t req;
            req.InstanceId = gThreadInstanceId;

            if (argv[3] && !strcmp(argv[3], "all"))
            {
                req.EuiMask = MESHCOP_SyncSteeringDataRequest_EuiMask_AllFFs;
            }
            else
            {
                req.EuiMask = MESHCOP_SyncSteeringDataRequest_EuiMask_ExpectedJoiners;
            }

            if (MESHCOP_SyncSteeringDataRequest(&req, THR_FSCI_IF) != MEM_SUCCESS_c)
            {
                status = gThrStatus_NoMem_c;
            }
            else
            {
                status = gThrStatus_Success_c;
            }
        }

        else if (!strcmp(argv[2], "nwkdata"))
        {
            THR_BrPrefixSyncRequest_t req;
            req.InstanceId = gThreadInstanceId;

            if (THR_BrPrefixSyncRequest(&req, THR_FSCI_IF) != MEM_SUCCESS_c)
            {
                status = gThrStatus_NoMem_c;
            }
            else
            {
                status = gThrStatus_Success_c;
            }
        }
    }

    else if (!strcmp(argv[1], "scan"))
    {
        THR_NwkScanRequest_t req;
        req.InstanceID = gThreadInstanceId;
        req.ScanChannelMask = 0x07FFF800;   // all channels
        req.ScanDuration = 2;               // (Range:1 - 14)
        req.maxThrNwkToDiscover = 10;       // maximum thread network to be discovered

        if (!strcmp(argv[2], "energy"))
        {
            req.ScanType = THR_NwkScanRequest_ScanType_EnergyDetect;
        }
        else if (!strcmp(argv[2], "active"))
        {
            req.ScanType = THR_NwkScanRequest_ScanType_ActiveScan;
        }
        else if (!strcmp(argv[2], "both"))
        {
            req.ScanType = THR_NwkScanRequest_ScanType_EnergyDetectAndActiveScan;
        }
        else
        {
            req.ScanType = THR_NwkScanRequest_ScanType_ActiveScan;
        }

        THR_NwkScanRequest(&req, THR_FSCI_IF);
    }

    else if (!strcmp(argv[1], "nwkdata"))
    {
        char *pValue;
        ipAddr_t prefix;
        uint8_t prefixLength = 64;
        uint32_t lifetime = 0;  //gThrNwkDataMinStableLifetime;

        /* Get prefix */
        pValue = shell_get_opt(argc, argv, "-p");

        if (pValue)
        {
            pton(AF_INET6, pValue, &prefix);
        }

        /* Get prefix length */
        pValue = shell_get_opt(argc, argv, "-len");

        if (pValue)
        {
            prefixLength = (uint8_t)NWKU_atoi(pValue);
        }

        /* Get prefix lifetime */
        pValue = shell_get_opt(argc, argv, "-t");

        if (pValue)
        {
            lifetime = NWKU_atoi(pValue);
        }

        if (!strcmp(argv[2], "add"))
        {
            THR_BrPrefixAddEntryRequest_t req = { 0 };
            req.InstanceId = gThreadInstanceId;
            req.prefixLength = prefixLength;
            FLib_MemCpy(req.PrefixValue, prefix.addr8, sizeof(ipAddr_t));
            req.prefixLifetime = lifetime;

            pValue = shell_get_opt(argc, argv, "dhcpserver");

            if (pValue)
            {
                THR_BR_FLAGS_SET(req.PrefixFlags, 1, THR_BR_PREFIX_FLAGS_P_DHCP_MASK, THR_BR_PREFIX_FLAGS_P_DHCP_OFFSET);
                THR_BR_FLAGS_SET(req.PrefixFlags, 1, THR_BR_PREFIX_FLAGS_P_ON_MESH_MASK, THR_BR_PREFIX_FLAGS_P_ON_MESH_OFFSET);
                req.prefixLifetime = lifetime;
                req.prefixAdvertised = TRUE;
            }

            pValue = shell_get_opt(argc, argv, "slaac");

            if (pValue)
            {
                THR_BR_FLAGS_SET(req.PrefixFlags, 1, THR_BR_PREFIX_FLAGS_P_SLAAC_MASK, THR_BR_PREFIX_FLAGS_P_SLAAC_OFFSET);
                THR_BR_FLAGS_SET(req.PrefixFlags, 1, THR_BR_PREFIX_FLAGS_P_ON_MESH_MASK, THR_BR_PREFIX_FLAGS_P_ON_MESH_OFFSET);
                req.prefixLifetime = lifetime;
                req.prefixAdvertised = TRUE;
            }

            pValue = shell_get_opt(argc, argv, "extroute");

            if (pValue)
            {
                THR_BR_FLAGS_SET(req.ExternalRouteFlags, 1, THR_BR_EXT_ROUTE_FLAGS_R_PREF_MASK, THR_BR_EXT_ROUTE_FLAGS_R_PREF_OFFSET);
                req.ExternalRouteAdvertised = TRUE;
                req.ExternalRouteLifetime = lifetime;
            }

            THR_BrPrefixAddEntryRequest(&req, THR_FSCI_IF);
        }
        else if (!strcmp(argv[2], "remove"))
        {

            if (!strcmp(argv[3], "dnsserver"))
            {
                // TODO
            }
            else
            {
                THR_BrPrefixRemoveEntryRequest_t req;
                req.InstanceId = gThreadInstanceId;
                req.prefixLength = prefixLength;
                FLib_MemCpy(req.PrefixValue, prefix.addr8, sizeof(ipAddr_t));

                THR_BrPrefixRemoveEntryRequest(&req, THR_FSCI_IF);
            }
        }
        else if (!strcmp(argv[2], "removeall"))
        {
            THR_BrPrefixRemoveAllRequest_t req;
            req.InstanceId = gThreadInstanceId;

            THR_BrPrefixRemoveAllRequest(&req, THR_FSCI_IF);
        }
    }

    else if (!strcmp(argv[1], "get"))
    {
        uint32_t i;

        if (!strcmp(argv[2], "attributes"))
        {
            shell_printf("Thread network attributes:\n\r");

            for (i = 0; i < NumberOfElements(aShellThrAttr); i++)
            {
                shell_printf("\t%s - %s\n\r", aShellThrAttr[i].pAttrName, aShellThrAttr[i].writable ? "RW" : "RO");
            }
        }

        else if (!strcmp(argv[2], "neighbor"))
        {
            if (argv[3])
            {
                THR_GetNeighborInfoRequest_t req;
                req.ShortAddress = NWKU_AsciiToHex((uint8_t *)(argv[3] + 2), strlen(argv[3] + 2));
                req.InstanceId = gThreadInstanceId;
                THR_GetNeighborInfoRequest(&req, THR_FSCI_IF);
            }
            else
            {
                shell_write("\n\rPlease provide the short address of the neighbor!");
            }

        }

        else if (!strcmp(argv[2], "neighbors"))
        {
            THR_GetNeighborTableRequest_t req;
            req.InstanceId = gThreadInstanceId;
            req.StartIndex = 0;
            req.NoOfElements = THR_MAX_NEIGHBORS;
            (void)THR_GetNeighborTableRequest(&req, THR_FSCI_IF);
        }

        else if (!strcmp(argv[2], "routes"))
        {
            THR_GetRoutingTableRequest_t req;
            req.InstanceId = gThreadInstanceId;
            req.StartIndex = 0;
            req.NoOfElements = 10;
            (void)THR_GetRoutingTableRequest(&req, THR_FSCI_IF);
        }

        else if (!strcmp(argv[2], "parent"))
        {
            THR_GetParentRequest_t req;
            req.InstanceId = gThreadInstanceId;
            (void)THR_GetParentRequest(&req, THR_FSCI_IF);
        }

        else
        {
            for (i = 0; i < NumberOfElements(aShellThrAttr); i++)
            {
                if (!strcmp(argv[2], aShellThrAttr[i].pAttrName))
                {
                    THR_GetAttrRequest_t req;
                    req.InstanceId = gThreadInstanceId;
                    req.AttributeId = (THR_GetAttrRequest_AttributeId_t)(aShellThrAttr[i].attrId);
                    req.Index = 0;

                    (void)THR_GetAttrRequest(&req, THR_FSCI_IF);

                    break;
                }
            }

            if (i == NumberOfElements(aShellThrAttr))
            {
                shell_write("Unknown parameter!");
            }
        }
    }

    else if (!strcmp(argv[1], "set"))
    {
        uint32_t i;
        THR_SetAttrRequest_t req;
        req.InstanceId = gThreadInstanceId;
        req.Index = 0;

        for (i = 0; i < NumberOfElements(aShellThrAttr); i++)
        {
            if (!strcmp(argv[2], aShellThrAttr[i].pAttrName))
            {
                if (!aShellThrAttr[i].writable)
                {
                    shell_write("Entry is read only!");
                    break;
                }

                req.AttributeId = (THR_SetAttrRequest_AttributeId_t)aShellThrAttr[i].attrId;

                switch (aShellThrAttr[i].valueType)
                {
                    case gDecimal_c:
                    {
                        int64_t pValue = NWKU_atol(argv[3]);

                        if (THR_GetAttrConfirm_AttributeId_ActiveTimestamp == aShellThrAttr[i].attrId)
                        {
                            /* Ticks are not included. Add 0 for ticks */
                            pValue = pValue << 16;
                        }

                        FLib_MemCpy((uint8_t *)&req.AttributeValue, (void *)&aShellThrAttr[i].maxSize, 1);
                        FLib_MemCpy((uint8_t *)&req.AttributeValue + 1, (void *)&pValue, aShellThrAttr[i].maxSize);
                    }

                    break;

                    case gHex_c:
                    {
                        uint32_t value = NWKU_AsciiToHex((uint8_t *)(argv[3] + 2), strlen(argv[3] + 2));

                        FLib_MemCpy((uint8_t *)&req.AttributeValue, (void *)&aShellThrAttr[i].maxSize, 1);
                        FLib_MemCpy((uint8_t *)&req.AttributeValue + 1, (void *)&value, aShellThrAttr[i].maxSize);
                    }
                    break;

                    case gString_c:
                    {
                        uint8_t pValue[32];
                        int32_t len = strlen(argv[3]);
                        FLib_MemCpy(&pValue, argv[3], len);

                        if (THR_GetAttrConfirm_AttributeId_Security_PSKc == aShellThrAttr[i].attrId)
                        {
                            shell_write("The PSKc is derived from the PSKd on the black-box.");
                        }

                        else if (THR_GetAttrConfirm_AttributeId_DeviceRole == aShellThrAttr[i].attrId)
                        {
                            THR_SetAttrRequest_AttributeValue_DeviceRole_Value_t devRole = THR_SetAttrRequest_AttributeValue_DeviceRole_Value_RouterEligibleEndDevice;

                            if (FLib_MemCmp(pValue, "SED", 3))
                            {
                                devRole = THR_SetAttrRequest_AttributeValue_DeviceRole_Value_SleepyEndDevice;
                            }
                            else if (FLib_MemCmp(pValue, "FED", 3))
                            {
                                devRole = THR_SetAttrRequest_AttributeValue_DeviceRole_Value_FullEndDevice;
                            }
                            else if (FLib_MemCmp(pValue, "MED", 3))
                            {
                                devRole = THR_SetAttrRequest_AttributeValue_DeviceRole_Value_MinimalEndDevice;
                            }

                            req.AttributeValue.DeviceRole.AttrSize = 1;
                            req.AttributeValue.DeviceRole.Value = devRole;
                        }

                        else
                        {
                            /* writable string attributes */
                            if (THR_GetAttrConfirm_AttributeId_NwkName == aShellThrAttr[i].attrId)
                            {
                                req.AttributeValue.NwkName.AttrSize = len;
                                req.AttributeValue.NwkName.Value = MEM_BufferAlloc(len);
                                FLib_MemCpy(req.AttributeValue.NwkName.Value, pValue, len);
                            }
                            else if (THR_GetAttrConfirm_AttributeId_ProvisioningURL == aShellThrAttr[i].attrId)
                            {
                                req.AttributeValue.ProvisioningURL.AttrSize = len;
                                req.AttributeValue.ProvisioningURL.Value = MEM_BufferAlloc(len);
                                FLib_MemCpy(req.AttributeValue.ProvisioningURL.Value, pValue, len);
                            }
                            else if (THR_GetAttrConfirm_AttributeId_Security_PSKd == aShellThrAttr[i].attrId)
                            {
                                req.AttributeValue.Security_PSKd.AttrSize = len;
                                req.AttributeValue.Security_PSKd.Value = MEM_BufferAlloc(len);
                                FLib_MemCpy(req.AttributeValue.Security_PSKd.Value, pValue, len);
                            }
                        }
                    }
                    break;

                    case gArray_c:
                    {
                        uint8_t pValue[16];
                        shell_read_buffer(argv[3] + 2, pValue, aShellThrAttr[i].maxSize);

                        /* writable array attributes */
                        if (THR_GetAttrConfirm_AttributeId_Security_NwkMasterKey == aShellThrAttr[i].attrId)
                        {
                            req.AttributeValue.Security_NwkMasterKey.AttrSize = aShellThrAttr[i].maxSize;
                            req.AttributeValue.Security_NwkMasterKey.Value = MEM_BufferAlloc(aShellThrAttr[i].maxSize);
                            FLib_MemCpy(req.AttributeValue.Security_NwkMasterKey.Value, pValue, aShellThrAttr[i].maxSize);
                        }
                        else if (THR_GetAttrConfirm_AttributeId_MLPrefix == aShellThrAttr[i].attrId)
                        {
                            req.AttributeValue.MLPrefix.AttrSize = aShellThrAttr[i].maxSize;
                            req.AttributeValue.MLPrefix.Value.PrefixLength = aShellThrAttr[i].maxSize;
                            FLib_MemCpy(req.AttributeValue.MLPrefix.Value.PrefixData, pValue, aShellThrAttr[i].maxSize);
                        }
                        else if (THR_GetAttrConfirm_AttributeId_ExtendedPanId == aShellThrAttr[i].attrId)
                        {
                            req.AttributeValue.ExtendedPanId.AttrSize = aShellThrAttr[i].maxSize;
                            req.AttributeValue.ExtendedPanId.Value = MEM_BufferAlloc(aShellThrAttr[i].maxSize);
                            FLib_MemCpy(req.AttributeValue.ExtendedPanId.Value, pValue, aShellThrAttr[i].maxSize);
                        }
                    }
                    break;

                    case gHexReversed_c:
                    {
                        uint8_t pValue[8];
                        shell_read_buffer(argv[3] + 2, pValue, 8);
                        NWKU_SwapArrayBytes(pValue, 8);

                        FLib_MemCpy((uint8_t *)&req.AttributeValue, (void *)&aShellThrAttr[i].maxSize, 1);
                        FLib_MemCpy((uint8_t *)&req.AttributeValue + 1, pValue, aShellThrAttr[i].maxSize);
                    }
                    break;

                    default:
                        break;
                }

                THR_SetAttrRequest(&req, THR_FSCI_IF);
                break;
            }

        }

        if (i == NumberOfElements(aShellThrAttr))
        {
            shell_write("Unknown parameter!");
        }
    }

    else if (!strcmp(argv[1], "remove"))
    {
        if (!strcmp(argv[2], "router"))
        {
            uint16_t shortAddress = NWKU_AsciiToHex((uint8_t *)(argv[3] + 2), strlen(argv[3] + 2));

            THR_LeaderRemoveRouterIdRequest_t req;
            req.InstanceId = gThreadInstanceId;
            req.RouterShortAddr = shortAddress;

            (void)THR_LeaderRemoveRouterIdRequest(&req, THR_FSCI_IF);
        }
    }

    else
    {
        shell_write("Unknown command!");
    }

    if (status != gThrStatus_EntryNotFound_c)
    {
        SHELL_PrintStatus(status);
    }

    SHELL_NEWLINE();

    return CMD_RET_SUCCESS;
}

/*!*************************************************************************************************
\private
\fn     static int32_t SHELL_Ifconfig(shell_handle_t context, int32_t argc, char **argv)
\brief  This function is used for ifconfig.

\param  [in]    argc      Number of arguments the command was called with
\param  [in]    argv      Pointer to a list of pointers to the arguments

\return         int32_t   Status of the command
***************************************************************************************************/
static int32_t SHELL_Ifconfig
(
    shell_handle_t context,
    int32_t argc,
    char **argv
)
{
    (void)NWKU_IfconfigAllRequest(THR_FSCI_IF);
    return CMD_RET_SUCCESS;
}

/*!*************************************************************************************************
\fn     static int32_t SHELL_MulticastGroups(shell_handle_t context, int32_t argc, char **argv)
\brief  This function is used for checking the joined multicast groups or to join a new one.

\param  [in]    argc      Number of arguments the command was called with
\param  [in]    argv      Pointer to a list of pointers to the arguments

\return         int32_t   Status of the command
***************************************************************************************************/
static int32_t SHELL_MulticastGroups
(
    shell_handle_t context,
    int32_t argc,
    char **argv
)
{
    ipAddr_t groupAddr = {0};
    command_ret_t ret = CMD_RET_SUCCESS;

    if (!strcmp(argv[1], "show"))
    {
        NWKU_McastGroupShowRequest_t req;
        req.InterfaceId = 0;  // only 6LoWPAN for now

        NWKU_McastGroupShowRequest(&req, THR_FSCI_IF);
    }

    else if ((!strcmp(argv[1], "add")) || (!strcmp(argv[1], "leave")))
    {
        if (argv[2] != NULL)
        {
            if (1 != pton(AF_INET6, argv[2], &groupAddr))
            {
                shell_write("Invalid multicast group address");
            }
            else
            {
                NWKU_McastGroupManageRequest_t req;
                req.InterfaceId = 0;  // only 6LoWPAN for now
                FLib_MemCpyReverseOrder(req.McastAddress, groupAddr.addr8, sizeof(ipAddr_t));

                if (!strcmp(argv[1], "add"))
                {
                    req.Action = NWKU_McastGroupManageRequest_Action_JoinGroup;
                }
                else if (!strcmp(argv[1], "leave"))
                {
                    req.Action = NWKU_McastGroupManageRequest_Action_LeaveGroup;
                }
                else
                {
                    shell_write("Invalid multicast group action. Please choose between 'add' and 'leave'.");
                    return ret;
                }

                NWKU_McastGroupManageRequest(&req, THR_FSCI_IF);
            }
        }
        else
        {
            shell_write("Type a multicast group address");
        }
    }

    return ret;
}

/*!*************************************************************************************************
\private
\fn     static int32_t SHELL_Ping(shell_handle_t context, int32_t argc, char **argv)
\brief  This function is used for "ping" shell command.

\param  [in]    argc      Number of arguments the command was called with
\param  [in]    argv      Pointer to a list of pointers to the arguments

\return         int8_t    Status of the command
***************************************************************************************************/
static int32_t SHELL_Ping
(
    shell_handle_t context,
    int32_t argc,
    char **argv
)
{
    command_ret_t ret = CMD_RET_SUCCESS;
    uint8_t i, ap = AF_UNSPEC;
    uint16_t count;
    char *pValue;
    bool_t validDstIpAddr = FALSE;

    /* Stop infinite ping */
    if (argc == 0)
    {
        if (gContinuousPing)
        {
            SHELL_Resume(NULL, 0, NULL);
        }
    }
    /* Check number of arguments according to the shellComm table */
    else
    {
        /* Reset the size of the ping */
        mPingSize = PING_PAYLOAD_DEFAULT_SIZE;
        mPingTimeoutMs = PING_DEFAULT_TIMEOUT;
        count = 0; /* infinite ping */

        /* Get option value for -s */
        pValue = shell_get_opt(argc, argv, "-s");

        if (pValue)
        {
            mPingSize = (uint16_t)NWKU_atoi(pValue);
        }

        /* Get option value for -i */
        pValue = shell_get_opt(argc, argv, "-i");

        if (pValue)
        {
            mPingTimeoutMs = (uint16_t)NWKU_atoi(pValue);

            if (mPingTimeoutMs < PING_MIN_TIMEOUT)
            {
                mPingTimeoutMs = PING_MIN_TIMEOUT;
            }
        }

        /* Get option value for -c */
        pValue = shell_get_opt(argc, argv, "-c");

        if (pValue)
        {
            count = (uint16_t)NWKU_atoi(pValue);
        }

        /* Get option value for -src */
        pValue = shell_get_opt(argc, argv, "-S");

        if (pValue)
        {
            /* Use MEM_BufferAllocForever() in case the ping takes more than 2 minutes */
            pSrcIpAddr = MEM_BufferAlloc(sizeof(ipAddr_t));
            pton(AF_INET6, pValue, pSrcIpAddr);
        }

        /* Find "-t" option */
        for (i = 1; i < argc; i++)
        {
            if (FLib_MemCmp(argv[i], "-t", 2))
            {
                count = THR_ALL_FFs16;
                break;
            }
        }

        /* Check if the destination IPv4/IPv6 address is valid */
        for (i = 1; i < argc; i++)
        {
            /* Verify IP address (v4 or v6) */
            uint8_t *pText = (uint8_t *)argv[i];

            while (*pText != '\0')
            {
                if (*pText == '.')
                {
                    ap = AF_INET;
                    break;
                }

                if (*pText == ':')
                {
                    ap = AF_INET6;
                    break;
                }

                pText++;
            }

            if ((ap != AF_UNSPEC) && (1 == pton(ap, argv[i], &mDstIpAddr)))
            {
                validDstIpAddr = TRUE;
                break;
            }
        }

        if (!validDstIpAddr)
        {
            shell_write("Invalid destination IP address\r\n");
            return CMD_RET_FAILURE;
        }

        /* Pause SHELL command parsing */
        ret = CMD_RET_ASYNC;
        /* Check number of counts */
        gContinuousPing = TRUE;
        mPingCounter = THR_ALL_FFs16;

        if (count == 0)
        {
            mPingCounter = 3;
        }
        else if (count == 1)
        {
            gContinuousPing = FALSE;

        }
        else if (count != THR_ALL_FFs16)
        {
            mPingCounter = count - 1;
        }

        /* Create Ping packet */
        FLib_MemCpyReverseOrder(mPingReq.DestinationIpAddress, mDstIpAddr.addr8, sizeof(ipAddr_t));

        if (pSrcIpAddr)
        {
            FLib_MemCpyReverseOrder(mPingReq.SourceIpAddress, pSrcIpAddr, sizeof(ipAddr_t));
        }

        mPingReq.Payloadlength = mPingSize;
        mPingReq.Timeout = mPingTimeoutMs;
        mPingReq.Secured = TRUE;

        shell_printf("Pinging %s with %u bytes of data:\r\n", argv[i], mPingSize);
        mPingTimestamp = TMR_GetTimestamp();
        (void)NWKU_PingRequest(&mPingReq, THR_FSCI_IF);

    } /* Correct number of arguments */

    return ret;
}

/*!*************************************************************************************************
\private
\fn     void SHELL_CoapSendMsg(void *param)
\brief  This function is used to send a CoAP command to the black-box. May be called on a timer.

\param  [in]    param      NWKU_CoapSendRequest_t populated with the user request
***************************************************************************************************/
static void SHELL_CoapSendMsg
(
    void *param
)
{
    NWKU_CoapSendRequest_t *coapSendReq = (NWKU_CoapSendRequest_t *)param;

    if (gCoAPInstanceId != THR_ALL_FFs8)
    {
        NWKU_CoapSendRequest(coapSendReq, THR_FSCI_IF);
    }
    else
    {
        shell_write("CoAP instance could not be created! Please try again..\r\n");
    }

    MEM_BufferFree(coapSendReq->Payload);
    MEM_BufferFree(coapSendReq);
}

/*!*************************************************************************************************
\private
\fn     static int32_t SHELL_CoapSend(shell_handle_t context, int32_t argc, char **argv)
\brief  This function is used for "coap" command. Wrapper over SHELL_CoapSendMsg that also creates
        the CoAP instance if not already created.

\param  [in]    argc      Number of arguments the command was called with
\param  [in]    argv      Pointer to a list of pointers to the arguments

\return         int32_t   Status of the command
***************************************************************************************************/
static int32_t SHELL_CoapSend
(
    shell_handle_t context,
    int32_t argc,
    char **argv
)
{
    /*coap <reqtype: CON/NON> <reqcode (GET/POST/PUT/DELETE)> <IP addr dest> <URI path> <payload ASCII>*/
    char *pValue;
    command_ret_t ret = CMD_RET_SUCCESS;
    NWKU_CoapSendRequest_RequestType_t requestType;
    NWKU_CoapSendRequest_MessageType_t requestCode;
    uint8_t *pCoapPayload = NULL;
    uint32_t coapPayloadSize = 0;
    ipAddr_t destAddr;

    NWKU_CoapSendRequest_t *pCoapSendReq = MEM_BufferAlloc(sizeof(NWKU_CoapSendRequest_t));

    if (!pCoapSendReq)
    {
        shell_write("Insufficient memory to create the FSCI request!\n\r");
        return CMD_RET_SUCCESS;
    }

    FLib_MemSet(pCoapSendReq, 0, sizeof(NWKU_CoapSendRequest_t));
    pCoapSendReq->InstanceID = gThreadInstanceId;
    pCoapSendReq->UDPPort = COAP_DEFAULT_PORT;

    if (argc >= 4)
    {
        if (!strcmp(argv[1], "CON"))
        {
            requestType = NWKU_CoapSendRequest_RequestType_CON;
        }
        else if (!strcmp(argv[1], "NON"))
        {
            requestType = NWKU_CoapSendRequest_RequestType_NON;
            ret = CMD_RET_SUCCESS;
        }
        else
        {
            SHELL_PrintStatus(gThrStatus_Failed_c);
            return CMD_RET_SUCCESS;
        }

        if (!strcmp(argv[2], "GET"))
        {
            requestCode = NWKU_CoapSendRequest_MessageType_GET;
        }
        else if (!strcmp(argv[2], "POST"))
        {
            requestCode = NWKU_CoapSendRequest_MessageType_POST;
        }
        else if (!strcmp(argv[2], "PUT"))
        {
            requestCode = NWKU_CoapSendRequest_MessageType_PUT;
        }
        else if (!strcmp(argv[2], "DELETE"))
        {
            requestCode = NWKU_CoapSendRequest_MessageType_DELETE;
        }
        else
        {
            SHELL_PrintStatus(gThrStatus_Failed_c);
            return CMD_RET_SUCCESS;
        }

        pCoapSendReq->RequestType = requestType;
        pCoapSendReq->MessageType = requestCode;

        /* Get destination address */
        pton(AF_INET6, argv[3], &destAddr);
        FLib_MemCpyReverseOrder(pCoapSendReq->DestinationIpAddress, destAddr.addr8, sizeof(ipAddr_t));

        /* Get URI path */
        if (argc >= 5)
        {
            pValue = argv[4];

            if (pValue)
            {
                FLib_MemCpy(pCoapSendReq->URIpath, pValue, strlen(pValue));
            }
            else
            {
                SHELL_PrintStatus(gThrStatus_Failed_c);
                return CMD_RET_SUCCESS;
            }
        }

        /* Get payload */
        if (argc >= 6)
        {
            pValue = argv[5];

            if (pValue)
            {
                coapPayloadSize = strlen(pValue);

                if (!strcmp(argv[5], "rgb"))
                {
                    coapPayloadSize += strlen(argv[6]) + strlen(argv[7]) + strlen(argv[8]) + 4;
                }

                pCoapPayload = MEM_BufferAlloc(coapPayloadSize);

                if (pCoapPayload)
                {
                    FLib_MemSet(pCoapPayload, 0, coapPayloadSize);
                    FLib_MemCpy(pCoapPayload, pValue, coapPayloadSize);
                }
            }
        }

        pCoapSendReq->PayloadLength = coapPayloadSize;
        pCoapSendReq->Payload = (char *)pCoapPayload;

        /* create the CoAP instance if not already created */
        if (THR_ALL_FFs8 == gCoAPInstanceId)
        {
            NWKU_CoapCreateInstanceRequest_t req;
            req.UDPPort = COAP_DEFAULT_PORT;
            req.SocketDomain = NWKU_CoapCreateInstanceRequest_SocketDomain_AF_INET6;

            NWKU_CoapCreateInstanceRequest(&req, THR_FSCI_IF);
            OSA_TimeDelay(100);
        }

        /* send the CoAP message */
        SHELL_CoapSendMsg((void *)pCoapSendReq);
    }
    else
    {
        shell_write("Invalid number of parameters!\r\n");
        ret = CMD_RET_SUCCESS;
    }

    return ret;
}

/*!*************************************************************************************************
\private
\fn     static int32_t SHELL_Reboot(shell_handle_t context, int32_t argc, char **argv);
\brief  This function is used to reset the device.

\param  [in]    argc      Number of arguments the command was called with
\param  [in]    argv      Pointer to a list of pointers to the arguments

\return         int32_t   Status of the command
***************************************************************************************************/
static int32_t SHELL_Reboot
(
    shell_handle_t context,
    int32_t argc,
    char **argv
)
{
    THR_CpuResetRequest_t req;
    req.TimeoutMs = 0;

    if (THR_CpuResetRequest(&req, THR_FSCI_IF) != MEM_SUCCESS_c)
    {
        shell_write("No memory!\n\r");
    }

    return CMD_RET_SUCCESS;
}

/*!*************************************************************************************************
\private
\fn     static int32_t SHELL_FactoryResetReq(shell_handle_t context, int32_t argc, char **argv);
\brief  This function is used to reset the device and restore the factory defaults.

\param  [in]    argc      Number of arguments the command was called with
\param  [in]    argv      Pointer to a list of pointers to the arguments

\return         int32_t   Status of the command
***************************************************************************************************/
static int32_t SHELL_FactoryResetReq
(
    shell_handle_t context,
    int32_t argc,
    char **argv
)
{
    shell_write("Reset Thread stack to factory defaults!\r\n");
    (void)THR_FactoryResetRequest(THR_FSCI_IF);
    return CMD_RET_SUCCESS;
}

/*!*************************************************************************************************
\private
\fn     static int32_t SHELL_Autostart(shell_handle_t context, int32_t argc, char **argv)
\brief  This function is used to reset the device and restore the factory defaults.

\param  [in]    argc      Number of arguments the command was called with
\param  [in]    argv      Pointer to a list of pointers to the arguments

\return         int32_t   Status of the command
***************************************************************************************************/
static int32_t SHELL_Autostart
(
    shell_handle_t context,
    int32_t argc,
    char **argv
)
{
    char *pValue = shell_get_opt(argc, argv, "-c");

    if (!pValue)
    {
        shell_write("Please provide the 802.15.4 to start on!\r\n");
        return CMD_RET_SUCCESS;
    }
    else
    {
        gAutostartChannel = (uint8_t)NWKU_atoi(pValue);

        if (gAutostartChannel < 11 || gAutostartChannel > 26)
        {
            shell_write("Invalid 802.15.4 channel, please select one in interval [11-26]!\r\n");
            return CMD_RET_SUCCESS;
        }
    }

    gAutostartInProgress = TRUE;
    gAutostartState = gAutostartFirstState_c;

    SHELL_FactoryResetReq(context, argc, argv);
    return CMD_RET_SUCCESS;
}

/*!*************************************************************************************************
\private
\fn     static void SHELL_PrintDevRole(void *param)
\brief  This function is used to print the device's role.

\param  [in]    param    Not used
***************************************************************************************************/
static void SHELL_PrintDevRole
(
    void *param
)
{
    THR_GetAttrConfirm_AttributeValue_DeviceRole_t devRole = *((THR_GetAttrConfirm_AttributeValue_DeviceRole_t *)param);

    shell_printf("Device Role: ");

    switch (devRole)
    {
        case THR_GetAttrConfirm_AttributeValue_DeviceRole_Disconnected:
            shell_printf("Disconnected");
            break;

        case THR_GetAttrConfirm_AttributeValue_DeviceRole_SleepyEndDevice:
            shell_printf("Sleepy End Device");
            break;

        case THR_GetAttrConfirm_AttributeValue_DeviceRole_MinimalEndDevice:
            shell_printf("Minimal End Device");
            break;

        case THR_GetAttrConfirm_AttributeValue_DeviceRole_FullEndDevice:
            shell_printf("Full End Device");
            break;

        case THR_GetAttrConfirm_AttributeValue_DeviceRole_RouterEligibleEndDevice:
            shell_printf("Router Eligible End Device");
            break;

        case THR_GetAttrConfirm_AttributeValue_DeviceRole_Router:
            shell_printf("Router");
            break;

        case THR_GetAttrConfirm_AttributeValue_DeviceRole_Leader:
            shell_printf("Leader");
            break;

        default:
            shell_printf("Unrecognized device role");
            break;
    }
}

/*!*************************************************************************************************
\private
\fn     static void SHELL_PrintDevType(void *param)
\brief  This function is used to print the device's type.

\param  [in]    param    Not used
***************************************************************************************************/
static void SHELL_PrintDevType
(
    void *param
)
{
    THR_GetAttrConfirm_AttributeValue_DeviceType_t devType = *((THR_GetAttrConfirm_AttributeValue_DeviceType_t *)param);

    shell_printf("Device Type: ");

    switch (devType)
    {
        case THR_GetAttrConfirm_AttributeValue_DeviceType_EndNode:
            shell_printf("End Node");
            break;

        case THR_GetAttrConfirm_AttributeValue_DeviceType_Combo:
            shell_printf("Combo Node");
            break;

        default:
            shell_printf("Unrecognized device type");
            break;
    }
}

/*!*************************************************************************************************
\private
\fn     static void SHELL_PrintActiveTimestamp(void *param)
\brief  This function is used to print the active timestamp.

\param  [in]    param    Not used
***************************************************************************************************/
static void SHELL_PrintActiveTimestamp
(
    void *param
)
{
    shell_printf("0x");
    shell_print_buffer((uint8_t *)(param), 6);
    shell_printf(" seconds\n\r");
}

/*!*************************************************************************************************
\private
\fn     void PING_RetransmitHandle(void *param)
\brief  This function handles the 500ms timeout. This timer is used to send another Ping.Request
        after a Ping.Reply packet was received.

\param  [in]    param    Not used
***************************************************************************************************/
static void PING_RetransmitHandle
(
    void *param
)
{
    mPingTimestamp = TMR_GetTimestamp();
    NWKU_PingRequest(&mPingReq, THR_FSCI_IF);

    if (mPingCounter > 0 && mPingCounter != THR_ALL_FFs16)
    {
        mPingCounter--;
    }

    /* Counter have reached 0: stop pinging */
    if (mPingCounter == 0)
    {
        gContinuousPing = FALSE;
        MEM_BufferFree(pSrcIpAddr);
        pSrcIpAddr = NULL;
        FLib_MemSet(&mPingReq, 0, sizeof(NWKU_PingRequest_t));
    }
}

/*!*************************************************************************************************
\private
\fn     static void SHELL_PrintParentInfo(void *param)
\brief  This function is used to print information about the parent of the node.

\param  [in]    param    THR_GetParentConfirm_t
***************************************************************************************************/
static void SHELL_PrintParentInfo
(
    void *param
)
{
    THR_GetParentConfirm_t *parent = (THR_GetParentConfirm_t *)param;
    uint64_t reversedExtAddr = parent->ExtendedAddress;

    NWKU_SwapArrayBytes((uint8_t *)&reversedExtAddr, 8);
    shell_printf("Parent short address: 0x%04X\n\r", parent->ShortAddress);
    shell_printf("Parent extended address: 0x");
    shell_print_buffer((uint8_t *)&reversedExtAddr, 8);
    shell_printf("\n\r");
}
