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
#ifndef _SHELL_BLE_H
#define _SHELL_BLE_H
/*!=================================================================================================
\file       shell_ble.h
\brief      This is a header file for the shell application for the BLE stack.
==================================================================================================*/

/*==================================================================================================
Include Files
==================================================================================================*/
#include "cmd_ble.h"

/*==================================================================================================
Public macros
==================================================================================================*/
#define gInvalidDeviceId_c                  0xFF

/*==================================================================================================
Public type definitions
==================================================================================================*/
typedef uint8_t deviceId_t;

typedef enum hrsDemoState_tag
{
    gHrsDemoAddServiceGatt_c,
    gHrsDemoAddServiceGap_c,
    gHrsDemoAddServiceHeartRate_c,
    gHrsDemoAddServiceBattery_c,
    gHrsDemoAddServiceDeviceInfo_c,
    gHrsDemoReadPublicAddress_c,
    gHrsDemoSetAdvData_c,
    gHrsDemoFindBatteryServiceHandle_c,
    gHrsDemoRegisterHandlesForWriteNotif_c,
    gHrsDemoFindHeartRateServiceHandle_c,
    gHrsDemoFindHrMeasurementHandle_c,
    gHrsDemoFindBodySensorLocationHandle_c,
    gHrsDemoFindBatteryLevelHandle_c,
    gHrsDemoWriteHrMeasurement_c,
    gHrsDemoWriteBodyLocation_c,
    gHrsDemoWriteBatteryLevel_c,
    gHrsDemoSetAdvParameters_c,
    gHrsDemoSetupFinishedMessage,
    gHrsDemoWaitingConnection_c,
    gHrsDemoConnectionEstablished_c,
    gHrsDemoStartSendMeasurements,
    gHrsDemoStopSendMeasurements,
    gHrsDemoError_c
} hrsDemoState_t;

typedef enum otapState_tag
{
    gOtapAddServiceGatt_c,
    gOtapAddServiceGap_c,
    gOtapAddServiceOtap_c,
    gOtapAddServiceBattery_c,
    gOtapAddServiceDeviceInfo_c,
    gOtapDbReady,
    gOtapWaitingConnection_c,
    gOtapConnectionEstablished_c
} otapState_t;

typedef enum wuartState_tag
{
    gWUartAddServiceGatt_c,
    gWUartAddServiceGap_c,
    gWUartAddServiceWirelessUART_c,
    gWUartAddServiceBattery_c,
    gWUartAddServiceDeviceInfo_c,
    gWUartReadPublicAddress_c,
    gWUartFindServiceHandle_c,
    gWUartFindUartStreamHandle_c,
    gWUartRegisterHandlesForWriteNotif_c,
    gWUartFindUartStreamReadHandle_c,
    gWUartRegisterHandlesForReadNotif_c,
    gWUartSetAdvData_c,
    gWUartSetPassKey_c,
    gWUartSetAdvParameters_c,
    gWUartWaitingConnection_c,
    gWUartConnectionEstablished_c,
    gWUartDemoError_c
} wuartState_t;

/*==================================================================================================
Public global variables declarations
==================================================================================================*/

/*==================================================================================================
Public function prototypes
==================================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*!*************************************************************************************************
\fn     void SHELL_BLE_Init(void)
\brief  This function is used to initialize the SHELL commands module.
***************************************************************************************************/
void SHELL_BLE_Init(void);

/*!*************************************************************************************************
\fn     void SHELL_BLE_Terminate(void)
\brief  This function is used to mark all BLE global variables as invalid.
***************************************************************************************************/
void SHELL_BLE_Terminate(void);

/*!*************************************************************************************************
\fn     void SHELL_BleEventNotify(void *param)
\brief  Print to host's shell information about BLE events coming from black-box

\param  [in]    param       the received event
***************************************************************************************************/
void SHELL_BleEventNotify(void *param);

void SHELL_PrintDiscoveredPrimaryServices(GATTClientProcedureDiscoverAllPrimaryServicesIndication_t *evt);
void SHELL_PrintDiscoveredChars(GATTClientProcedureDiscoverAllCharacteristicsIndication_t *evt);
void SHELL_PrintDiscoveredDescriptors(GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_t *evt);

void SHELL_GattDb_PrintReadAttr(GATTDBReadAttributeIndication_t *evt);
void SHELL_GattDb_PrintPrimaryService(GATTDBDynamicAddPrimaryServiceDeclarationIndication_t *evt);
void SHELL_GattDb_PrintChar(GATTDBDynamicAddCharacteristicDeclarationAndValueIndication_t *evt);
void SHELL_GattDb_PrintCharDesc(GATTDBDynamicAddCharacteristicDescriptorIndication_t *evt);
void SHELL_GattDb_PrintCCCD(GATTDBDynamicAddCccdIndication_t *evt);

void SHELL_GapPrintPublicAddr(GAPGenericEventPublicAddressReadIndication_t *evt);
void SHELL_GapPrintDeviceScannedIndication(GAPScanningEventDeviceScannedIndication_t *evt);
void SHELL_GapAcceptPairingRequest(GAPConnectionEventPairingRequestIndication_t *evt);

void BleApp_DemoHrs(void);
void BleApp_DemoOtap(void);
void BleApp_DemoWirelessUART(void);
void BleApp_SendLtk(void);
void BleApp_Bond(void);
void BleApp_UpdateConnectionParameters(void);
void BleApp_Encyption(void);
void BleApp_BondReq(void);
void BleApp_LoadCustomerPeerInfo(void);
void BleApp_Sendseqreq(void);
void BleApp_LTKreq(GAPConnectionEventLongTermKeyRequestIndication_t ltkind);
void BleApp_Addbondaddress(GAPGetBondedStaticAddressesIndication_t bondadd);

#ifdef __cplusplus
}
#endif
/*================================================================================================*/
#endif  /* _SHELL_BLE_H */
