/* Source file generated from BLE.xml */

/*==================================================================================================
Include Files
==================================================================================================*/
#include "cmd_ble.h"
#include "fsl_log.h"

extern bool_t g_ready_flag;
extern bool_t g_cam_stop;

/*==================================================================================================
Private Prototypes
==================================================================================================*/
#if FSCI_ENABLE
static memStatus_t Load_FSCIErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_FSCIAllowDeviceToSleepConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_FSCIWakeUpIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_FSCIGetWakeupReasonResponse(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* FSCI_ENABLE */

#if L2CAP_ENABLE
static memStatus_t Load_L2CAPConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_L2CAPLePsmConnectionRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_L2CAPLePsmConnectionCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_L2CAPLePsmDisconnectNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_L2CAPNoPeerCreditsIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_L2CAPLocalCreditsNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_L2CAPLeCbDataIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* L2CAP_ENABLE */

#if GATT_ENABLE
static memStatus_t Load_GATTConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTGetMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientProcedureExchangeMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientProcedureDiscoverAllPrimaryServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientProcedureDiscoverPrimaryServicesByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientProcedureFindIncludedServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientProcedureDiscoverAllCharacteristicsIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientProcedureDiscoverCharacteristicByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientProcedureReadCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientProcedureReadUsingCharacteristicUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientProcedureReadMultipleCharacteristicValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientProcedureWriteCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientProcedureReadCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientProcedureWriteCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTClientIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTServerMtuChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTServerHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTServerAttributeWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTServerCharacteristicCccdWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTServerAttributeWrittenWithoutResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTServerErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTServerLongCharacteristicWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTServerAttributeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* GATT_ENABLE */

#if GATTDB_APP_ENABLE
static memStatus_t Load_GATTDBConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTDBReadAttributeIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTDBFindServiceHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTDBFindCharValueHandleInServiceIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTDBFindCccdHandleForCharValueHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTDBFindDescriptorHandleForCharValueHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTDBDynamicAddPrimaryServiceDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTDBDynamicAddSecondaryServiceDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTDBDynamicAddIncludeDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTDBDynamicAddCharacteristicDeclarationAndValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTDBDynamicAddCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTDBDynamicAddCccdIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* GATTDB_APP_ENABLE */

#if GAP_ENABLE
static memStatus_t Load_GAPConfirm(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPCheckNotificationStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPCheckIndicationStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGetBondedStaticAddressesIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPLoadEncryptionInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPLoadCustomPeerInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPCheckIfBondedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGetBondedDevicesCountIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGetBondedDeviceNameIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventInitializationCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventInternalErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventAdvertisingSetupFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventAdvertisingParametersSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventAdvertisingDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventWhiteListSizeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventDeviceAddedToWhiteListIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventDeviceRemovedFromWhiteListIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventWhiteListClearedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventRandomAddressReadyIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventCreateConnectionCanceledIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventPublicAddressReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventAdvTxPowerLevelReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventPrivateResolvableAddressVerifiedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventRandomAddressSetIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPAdvertisingEventStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPAdvertisingEventCommandFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPScanningEventStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPScanningEventCommandFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPScanningEventDeviceScannedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventConnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventPairingRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventSlaveSecurityRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventPairingResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventAuthenticationRejectedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventPasskeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventOobRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventPasskeyDisplayIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventKeyExchangeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventKeysReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventLongTermKeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventEncryptionChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventPairingCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventDisconnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventRssiReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventTxPowerLevelReadIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventPowerReadFailureIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventParameterUpdateRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventParameterUpdateCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventLeDataLengthChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventLeScOobDataRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventLeScDisplayNumericValueIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPConnectionEventLeScKeypressNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventControllerResetCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPLeScPublicKeyRegeneratedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventLeScLocalOobDataIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGenericEventControllerPrivacyStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_GAPGetBondedDevicesIdentityInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload);
#endif  /* GAP_ENABLE */

/*==================================================================================================
Private global variables declarations
==================================================================================================*/
static const bleEvtHandler_t evtHandlerTbl[] =
{
#if FSCI_ENABLE
	{0xA4FE, Load_FSCIErrorIndication},
	{0xA470, Load_FSCIAllowDeviceToSleepConfirm},
	{0xA471, Load_FSCIWakeUpIndication},
	{0xA472, Load_FSCIGetWakeupReasonResponse},
#endif  /* FSCI_ENABLE */

#if L2CAP_ENABLE
	{0x4180, Load_L2CAPConfirm},
	{0x4183, Load_L2CAPLePsmConnectionRequestIndication},
	{0x4184, Load_L2CAPLePsmConnectionCompleteIndication},
	{0x4185, Load_L2CAPLePsmDisconnectNotificationIndication},
	{0x4186, Load_L2CAPNoPeerCreditsIndication},
	{0x4187, Load_L2CAPLocalCreditsNotificationIndication},
	{0x4188, Load_L2CAPLeCbDataIndication},
#endif  /* L2CAP_ENABLE */

#if GATT_ENABLE
	{0x4480, Load_GATTConfirm},
	{0x4481, Load_GATTGetMtuIndication},
	{0x4482, Load_GATTClientProcedureExchangeMtuIndication},
	{0x4483, Load_GATTClientProcedureDiscoverAllPrimaryServicesIndication},
	{0x4484, Load_GATTClientProcedureDiscoverPrimaryServicesByUuidIndication},
	{0x4485, Load_GATTClientProcedureFindIncludedServicesIndication},
	{0x4486, Load_GATTClientProcedureDiscoverAllCharacteristicsIndication},
	{0x4487, Load_GATTClientProcedureDiscoverCharacteristicByUuidIndication},
	{0x4488, Load_GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication},
	{0x4489, Load_GATTClientProcedureReadCharacteristicValueIndication},
	{0x448A, Load_GATTClientProcedureReadUsingCharacteristicUuidIndication},
	{0x448B, Load_GATTClientProcedureReadMultipleCharacteristicValuesIndication},
	{0x448C, Load_GATTClientProcedureWriteCharacteristicValueIndication},
	{0x448D, Load_GATTClientProcedureReadCharacteristicDescriptorIndication},
	{0x448E, Load_GATTClientProcedureWriteCharacteristicDescriptorIndication},
	{0x448F, Load_GATTClientNotificationIndication},
	{0x4490, Load_GATTClientIndicationIndication},
	{0x4491, Load_GATTServerMtuChangedIndication},
	{0x4492, Load_GATTServerHandleValueConfirmationIndication},
	{0x4493, Load_GATTServerAttributeWrittenIndication},
	{0x4494, Load_GATTServerCharacteristicCccdWrittenIndication},
	{0x4495, Load_GATTServerAttributeWrittenWithoutResponseIndication},
	{0x4496, Load_GATTServerErrorIndication},
	{0x4497, Load_GATTServerLongCharacteristicWrittenIndication},
	{0x4498, Load_GATTServerAttributeReadIndication},
#endif  /* GATT_ENABLE */

#if GATTDB_APP_ENABLE
	{0x4580, Load_GATTDBConfirm},
	{0x4581, Load_GATTDBReadAttributeIndication},
	{0x4582, Load_GATTDBFindServiceHandleIndication},
	{0x4583, Load_GATTDBFindCharValueHandleInServiceIndication},
	{0x4584, Load_GATTDBFindCccdHandleForCharValueHandleIndication},
	{0x4585, Load_GATTDBFindDescriptorHandleForCharValueHandleIndication},
	{0x4586, Load_GATTDBDynamicAddPrimaryServiceDeclarationIndication},
	{0x4587, Load_GATTDBDynamicAddSecondaryServiceDeclarationIndication},
	{0x4588, Load_GATTDBDynamicAddIncludeDeclarationIndication},
	{0x4589, Load_GATTDBDynamicAddCharacteristicDeclarationAndValueIndication},
	{0x458A, Load_GATTDBDynamicAddCharacteristicDescriptorIndication},
	{0x458B, Load_GATTDBDynamicAddCccdIndication},
	{0x458C, Load_GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication},
#endif  /* GATTDB_APP_ENABLE */

#if GAP_ENABLE
	{0x4780, Load_GAPConfirm},
	{0x4781, Load_GAPCheckNotificationStatusIndication},
	{0x4782, Load_GAPCheckIndicationStatusIndication},
	{0x4783, Load_GAPGetBondedStaticAddressesIndication},
	{0x4784, Load_GAPLoadEncryptionInformationIndication},
	{0x4785, Load_GAPLoadCustomPeerInformationIndication},
	{0x4786, Load_GAPCheckIfBondedIndication},
	{0x4787, Load_GAPGetBondedDevicesCountIndication},
	{0x4788, Load_GAPGetBondedDeviceNameIndication},
	{0x4789, Load_GAPGenericEventInitializationCompleteIndication},
	{0x478A, Load_GAPGenericEventInternalErrorIndication},
	{0x478B, Load_GAPGenericEventAdvertisingSetupFailedIndication},
	{0x478C, Load_GAPGenericEventAdvertisingParametersSetupCompleteIndication},
	{0x478D, Load_GAPGenericEventAdvertisingDataSetupCompleteIndication},
	{0x478E, Load_GAPGenericEventWhiteListSizeReadIndication},
	{0x478F, Load_GAPGenericEventDeviceAddedToWhiteListIndication},
	{0x4790, Load_GAPGenericEventDeviceRemovedFromWhiteListIndication},
	{0x4791, Load_GAPGenericEventWhiteListClearedIndication},
	{0x4792, Load_GAPGenericEventRandomAddressReadyIndication},
	{0x4793, Load_GAPGenericEventCreateConnectionCanceledIndication},
	{0x4794, Load_GAPGenericEventPublicAddressReadIndication},
	{0x4795, Load_GAPGenericEventAdvTxPowerLevelReadIndication},
	{0x4796, Load_GAPGenericEventPrivateResolvableAddressVerifiedIndication},
	{0x4797, Load_GAPGenericEventRandomAddressSetIndication},
	{0x4798, Load_GAPAdvertisingEventStateChangedIndication},
	{0x4799, Load_GAPAdvertisingEventCommandFailedIndication},
	{0x479A, Load_GAPScanningEventStateChangedIndication},
	{0x479B, Load_GAPScanningEventCommandFailedIndication},
	{0x479C, Load_GAPScanningEventDeviceScannedIndication},
	{0x479D, Load_GAPConnectionEventConnectedIndication},
	{0x479E, Load_GAPConnectionEventPairingRequestIndication},
	{0x479F, Load_GAPConnectionEventSlaveSecurityRequestIndication},
	{0x47A0, Load_GAPConnectionEventPairingResponseIndication},
	{0x47A1, Load_GAPConnectionEventAuthenticationRejectedIndication},
	{0x47A2, Load_GAPConnectionEventPasskeyRequestIndication},
	{0x47A3, Load_GAPConnectionEventOobRequestIndication},
	{0x47A4, Load_GAPConnectionEventPasskeyDisplayIndication},
	{0x47A5, Load_GAPConnectionEventKeyExchangeRequestIndication},
	{0x47A6, Load_GAPConnectionEventKeysReceivedIndication},
	{0x47A7, Load_GAPConnectionEventLongTermKeyRequestIndication},
	{0x47A8, Load_GAPConnectionEventEncryptionChangedIndication},
	{0x47A9, Load_GAPConnectionEventPairingCompleteIndication},
	{0x47AA, Load_GAPConnectionEventDisconnectedIndication},
	{0x47AB, Load_GAPConnectionEventRssiReadIndication},
	{0x47AC, Load_GAPConnectionEventTxPowerLevelReadIndication},
	{0x47AD, Load_GAPConnectionEventPowerReadFailureIndication},
	{0x47AE, Load_GAPConnectionEventParameterUpdateRequestIndication},
	{0x47AF, Load_GAPConnectionEventParameterUpdateCompleteIndication},
	{0x47B0, Load_GAPConnectionEventLeDataLengthChangedIndication},
	{0x47B1, Load_GAPConnectionEventLeScOobDataRequestIndication},
	{0x47B2, Load_GAPConnectionEventLeScDisplayNumericValueIndication},
	{0x47B3, Load_GAPConnectionEventLeScKeypressNotificationIndication},
	{0x47B4, Load_GAPGenericEventControllerResetCompleteIndication},
	{0x47B5, Load_GAPLeScPublicKeyRegeneratedIndication},
	{0x47B6, Load_GAPGenericEventLeScLocalOobDataIndication},
	{0x47B7, Load_GAPGenericEventControllerPrivacyStateChangedIndication},
	{0x4783, Load_GAPGetBondedDevicesIdentityInformationIndication},
#endif  /* GAP_ENABLE */
};

/*==================================================================================================
Public Functions
==================================================================================================*/
#if FSCI_ENABLE
/*!*************************************************************************************************
\fn		static memStatus_t Load_FSCIErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	FSCI is reporting an error condition.
***************************************************************************************************/
static memStatus_t Load_FSCIErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIErrorIndication_t *evt = &(container->Data.FSCIErrorIndication);

	/* Store (OG, OC) in ID */
	container->id = 0xA4FE;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIErrorIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_FSCIAllowDeviceToSleepConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	FSCI-AllowDeviceToSleep.Confirm description
***************************************************************************************************/
static memStatus_t Load_FSCIAllowDeviceToSleepConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIAllowDeviceToSleepConfirm_t *evt = &(container->Data.FSCIAllowDeviceToSleepConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xA470;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIAllowDeviceToSleepConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_FSCIWakeUpIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	FSCI-WakeUp.Indication description
***************************************************************************************************/
static memStatus_t Load_FSCIWakeUpIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIWakeUpIndication_t *evt = &(container->Data.FSCIWakeUpIndication);

	/* Store (OG, OC) in ID */
	container->id = 0xA471;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIWakeUpIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_FSCIGetWakeupReasonResponse(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	FSCI-GetWakeupReason.Response description
***************************************************************************************************/
static memStatus_t Load_FSCIGetWakeupReasonResponse(bleEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIGetWakeupReasonResponse_t *evt = &(container->Data.FSCIGetWakeupReasonResponse);

	/* Store (OG, OC) in ID */
	container->id = 0xA472;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIGetWakeupReasonResponse_t));

	return MEM_SUCCESS_c;
}

#endif  /* FSCI_ENABLE */

#if L2CAP_ENABLE
/*!*************************************************************************************************
\fn		static memStatus_t Load_L2CAPConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the L2CAP request
***************************************************************************************************/
static memStatus_t Load_L2CAPConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPConfirm_t *evt = &(container->Data.L2CAPConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0x4180;

	FLib_MemCpy(evt, pPayload, sizeof(L2CAPConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_L2CAPLePsmConnectionRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Connection request event
***************************************************************************************************/
static memStatus_t Load_L2CAPLePsmConnectionRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPLePsmConnectionRequestIndication_t *evt = &(container->Data.L2CAPLePsmConnectionRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4183;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->LeCbConnectionRequest.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->LeCbConnectionRequest.LePsm), pPayload + idx, sizeof(evt->LeCbConnectionRequest.LePsm)); idx += sizeof(evt->LeCbConnectionRequest.LePsm);
		FLib_MemCpy(&(evt->LeCbConnectionRequest.PeerMtu), pPayload + idx, sizeof(evt->LeCbConnectionRequest.PeerMtu)); idx += sizeof(evt->LeCbConnectionRequest.PeerMtu);
		FLib_MemCpy(&(evt->LeCbConnectionRequest.PeerMps), pPayload + idx, sizeof(evt->LeCbConnectionRequest.PeerMps)); idx += sizeof(evt->LeCbConnectionRequest.PeerMps);
		FLib_MemCpy(&(evt->LeCbConnectionRequest.InitialCredits), pPayload + idx, sizeof(evt->LeCbConnectionRequest.InitialCredits)); idx += sizeof(evt->LeCbConnectionRequest.InitialCredits);
	}


	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_L2CAPLePsmConnectionCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Connection complete event
***************************************************************************************************/
static memStatus_t Load_L2CAPLePsmConnectionCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPLePsmConnectionCompleteIndication_t *evt = &(container->Data.L2CAPLePsmConnectionCompleteIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4184;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->LeCbConnectionComplete.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->LeCbConnectionComplete.ChannelId), pPayload + idx, sizeof(evt->LeCbConnectionComplete.ChannelId)); idx += sizeof(evt->LeCbConnectionComplete.ChannelId);
		FLib_MemCpy(&(evt->LeCbConnectionComplete.PeerMtu), pPayload + idx, sizeof(evt->LeCbConnectionComplete.PeerMtu)); idx += sizeof(evt->LeCbConnectionComplete.PeerMtu);
		FLib_MemCpy(&(evt->LeCbConnectionComplete.PeerMps), pPayload + idx, sizeof(evt->LeCbConnectionComplete.PeerMps)); idx += sizeof(evt->LeCbConnectionComplete.PeerMps);
		FLib_MemCpy(&(evt->LeCbConnectionComplete.InitialCredits), pPayload + idx, sizeof(evt->LeCbConnectionComplete.InitialCredits)); idx += sizeof(evt->LeCbConnectionComplete.InitialCredits);
		FLib_MemCpy(&(evt->LeCbConnectionComplete.Result), pPayload + idx, 2); idx += 2;
	}


	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_L2CAPLePsmDisconnectNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Disconnection notification event
***************************************************************************************************/
static memStatus_t Load_L2CAPLePsmDisconnectNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPLePsmDisconnectNotificationIndication_t *evt = &(container->Data.L2CAPLePsmDisconnectNotificationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4185;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->LeCbDisconnection.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->LeCbDisconnection.ChannelId), pPayload + idx, sizeof(evt->LeCbDisconnection.ChannelId)); idx += sizeof(evt->LeCbDisconnection.ChannelId);
	}


	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_L2CAPNoPeerCreditsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - No peer credits event
***************************************************************************************************/
static memStatus_t Load_L2CAPNoPeerCreditsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPNoPeerCreditsIndication_t *evt = &(container->Data.L2CAPNoPeerCreditsIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4186;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->LeCbNoPeerCredits.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->LeCbNoPeerCredits.ChannelId), pPayload + idx, sizeof(evt->LeCbNoPeerCredits.ChannelId)); idx += sizeof(evt->LeCbNoPeerCredits.ChannelId);
	}


	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_L2CAPLocalCreditsNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Credit Based - Local credits notification event
***************************************************************************************************/
static memStatus_t Load_L2CAPLocalCreditsNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPLocalCreditsNotificationIndication_t *evt = &(container->Data.L2CAPLocalCreditsNotificationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4187;

	evt->InformationIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->InformationIncluded)
	{
		evt->LeCbLocalCreditsNotification.DeviceId = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->LeCbLocalCreditsNotification.ChannelId), pPayload + idx, sizeof(evt->LeCbLocalCreditsNotification.ChannelId)); idx += sizeof(evt->LeCbLocalCreditsNotification.ChannelId);
		FLib_MemCpy(&(evt->LeCbLocalCreditsNotification.LocalCredits), pPayload + idx, sizeof(evt->LeCbLocalCreditsNotification.LocalCredits)); idx += sizeof(evt->LeCbLocalCreditsNotification.LocalCredits);
	}


	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_L2CAPLeCbDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Data packet received through Credit Based Channel
***************************************************************************************************/
static memStatus_t Load_L2CAPLeCbDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	L2CAPLeCbDataIndication_t *evt = &(container->Data.L2CAPLeCbDataIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4188;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->SrcCid), pPayload + idx, sizeof(evt->SrcCid)); idx += sizeof(evt->SrcCid);
	FLib_MemCpy(&(evt->PacketLength), pPayload + idx, sizeof(evt->PacketLength)); idx += sizeof(evt->PacketLength);

	if (evt->PacketLength > 0)
	{
		evt->Packet = MEM_BufferAlloc(evt->PacketLength);

		if (!evt->Packet)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Packet = NULL;
	}

	FLib_MemCpy(evt->Packet, pPayload + idx, evt->PacketLength); idx += evt->PacketLength;

	return MEM_SUCCESS_c;
}

#endif  /* L2CAP_ENABLE */

#if GATT_ENABLE
/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the GATT request
***************************************************************************************************/
static memStatus_t Load_GATTConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTConfirm_t *evt = &(container->Data.GATTConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0x4480;

	FLib_MemCpy(evt, pPayload, sizeof(GATTConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTGetMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies the MTU used with a given connected device
***************************************************************************************************/
static memStatus_t Load_GATTGetMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTGetMtuIndication_t *evt = &(container->Data.GATTGetMtuIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4481;

	FLib_MemCpy(evt, pPayload, sizeof(GATTGetMtuIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientProcedureExchangeMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the MTU exchange procedure ended
***************************************************************************************************/
static memStatus_t Load_GATTClientProcedureExchangeMtuIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureExchangeMtuIndication_t *evt = &(container->Data.GATTClientProcedureExchangeMtuIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4482;

	FLib_MemCpy(evt, pPayload, sizeof(GATTClientProcedureExchangeMtuIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientProcedureDiscoverAllPrimaryServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Primary Service Discovery procedure ended
***************************************************************************************************/
static memStatus_t Load_GATTClientProcedureDiscoverAllPrimaryServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureDiscoverAllPrimaryServicesIndication_t *evt = &(container->Data.GATTClientProcedureDiscoverAllPrimaryServicesIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4483;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureDiscoverAllPrimaryServicesIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfDiscoveredServices = pPayload[idx]; idx++;

	if (evt->NbOfDiscoveredServices > 0)
	{
		evt->DiscoveredServices = MEM_BufferAlloc(evt->NbOfDiscoveredServices * sizeof(evt->DiscoveredServices[0]));

		if (!evt->DiscoveredServices)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->DiscoveredServices = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
	{
		FLib_MemCpy(&(evt->DiscoveredServices[i].StartHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].StartHandle)); idx += sizeof(evt->DiscoveredServices[i].StartHandle);
		FLib_MemCpy(&(evt->DiscoveredServices[i].EndHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].EndHandle)); idx += sizeof(evt->DiscoveredServices[i].EndHandle);
		evt->DiscoveredServices[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->DiscoveredServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		evt->DiscoveredServices[i].NbOfCharacteristics = pPayload[idx]; idx++;

		if (evt->DiscoveredServices[i].NbOfCharacteristics > 0)
		{
			evt->DiscoveredServices[i].Characteristics = MEM_BufferAlloc(evt->DiscoveredServices[i].NbOfCharacteristics * sizeof(evt->DiscoveredServices[i].Characteristics[0]));

			if (!evt->DiscoveredServices[i].Characteristics)
			{
				MEM_BufferFree(evt->DiscoveredServices);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->DiscoveredServices[i].Characteristics = NULL;
		}


		for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
		{
			evt->DiscoveredServices[i].Characteristics[j].Properties = (Properties_t)pPayload[idx]; idx++;
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.Handle), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.Handle)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.Handle);
			evt->DiscoveredServices[i].Characteristics[j].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->DiscoveredServices[i].Characteristics[j].Value.UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength);
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength);

			if (evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength > 0)
			{
				evt->DiscoveredServices[i].Characteristics[j].Value.Value = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength);

				if (!evt->DiscoveredServices[i].Characteristics[j].Value.Value)
				{
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
					}
					MEM_BufferFree(evt->DiscoveredServices);
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->DiscoveredServices[i].Characteristics[j].Value.Value = NULL;
			}

			FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Value, pPayload + idx, evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength); idx += evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength;
			evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors = pPayload[idx]; idx++;

			if (evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors > 0)
			{
				evt->DiscoveredServices[i].Characteristics[j].Descriptors = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors * sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[0]));

				if (!evt->DiscoveredServices[i].Characteristics[j].Descriptors)
				{
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
						}
					}
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
					}
					MEM_BufferFree(evt->DiscoveredServices);
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->DiscoveredServices[i].Characteristics[j].Descriptors = NULL;
			}


			for (uint32_t k = 0; k < evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors; k++)
			{
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle);
				evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].UuidType = (UuidType_t)pPayload[idx]; idx++;

				switch (evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].UuidType)
				{
					case Uuid16Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
						break;

					case Uuid128Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
						break;

					case Uuid32Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
						break;
				}
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength);
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength);

				if (evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength > 0)
				{
					evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength);

					if (!evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value)
					{
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
							{
								MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors);
							}
						}
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
							{
								MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
							}
						}
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
						}
						MEM_BufferFree(evt->DiscoveredServices);
						return MEM_ALLOC_ERROR_c;
					}

				}
				else
				{
					evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value = NULL;
				}

				FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value, pPayload + idx, evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength); idx += evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength;
			}
		}
		evt->DiscoveredServices[i].NbOfIncludedServices = pPayload[idx]; idx++;

		if (evt->DiscoveredServices[i].NbOfIncludedServices > 0)
		{
			evt->DiscoveredServices[i].IncludedServices = MEM_BufferAlloc(evt->DiscoveredServices[i].NbOfIncludedServices * sizeof(evt->DiscoveredServices[i].IncludedServices[0]));

			if (!evt->DiscoveredServices[i].IncludedServices)
			{
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						for (uint32_t k = 0; k < evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors; k++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value);
						}
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors);
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
				}
				MEM_BufferFree(evt->DiscoveredServices);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->DiscoveredServices[i].IncludedServices = NULL;
		}


		for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfIncludedServices; j++)
		{
			FLib_MemCpy(&(evt->DiscoveredServices[i].IncludedServices[j].StartHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].IncludedServices[j].StartHandle)); idx += sizeof(evt->DiscoveredServices[i].IncludedServices[j].StartHandle);
			FLib_MemCpy(&(evt->DiscoveredServices[i].IncludedServices[j].EndHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].IncludedServices[j].EndHandle)); idx += sizeof(evt->DiscoveredServices[i].IncludedServices[j].EndHandle);
			evt->DiscoveredServices[i].IncludedServices[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->DiscoveredServices[i].IncludedServices[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			evt->DiscoveredServices[i].IncludedServices[j].NbOfCharacteristics = pPayload[idx]; idx++;
			evt->DiscoveredServices[i].IncludedServices[j].NbOfIncludedServices = pPayload[idx]; idx++;
		}
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientProcedureDiscoverPrimaryServicesByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Primary Service Discovery By UUID procedure ended
***************************************************************************************************/
static memStatus_t Load_GATTClientProcedureDiscoverPrimaryServicesByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_t *evt = &(container->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4484;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfDiscoveredServices = pPayload[idx]; idx++;

	if (evt->NbOfDiscoveredServices > 0)
	{
		evt->DiscoveredServices = MEM_BufferAlloc(evt->NbOfDiscoveredServices * sizeof(evt->DiscoveredServices[0]));

		if (!evt->DiscoveredServices)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->DiscoveredServices = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
	{
		FLib_MemCpy(&(evt->DiscoveredServices[i].StartHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].StartHandle)); idx += sizeof(evt->DiscoveredServices[i].StartHandle);
		FLib_MemCpy(&(evt->DiscoveredServices[i].EndHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].EndHandle)); idx += sizeof(evt->DiscoveredServices[i].EndHandle);
		evt->DiscoveredServices[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->DiscoveredServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->DiscoveredServices[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		evt->DiscoveredServices[i].NbOfCharacteristics = pPayload[idx]; idx++;

		if (evt->DiscoveredServices[i].NbOfCharacteristics > 0)
		{
			evt->DiscoveredServices[i].Characteristics = MEM_BufferAlloc(evt->DiscoveredServices[i].NbOfCharacteristics * sizeof(evt->DiscoveredServices[i].Characteristics[0]));

			if (!evt->DiscoveredServices[i].Characteristics)
			{
				MEM_BufferFree(evt->DiscoveredServices);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->DiscoveredServices[i].Characteristics = NULL;
		}


		for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
		{
			evt->DiscoveredServices[i].Characteristics[j].Properties = (Properties_t)pPayload[idx]; idx++;
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.Handle), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.Handle)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.Handle);
			evt->DiscoveredServices[i].Characteristics[j].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->DiscoveredServices[i].Characteristics[j].Value.UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength);
			FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Value.MaxValueLength);

			if (evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength > 0)
			{
				evt->DiscoveredServices[i].Characteristics[j].Value.Value = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength);

				if (!evt->DiscoveredServices[i].Characteristics[j].Value.Value)
				{
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
					}
					MEM_BufferFree(evt->DiscoveredServices);
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->DiscoveredServices[i].Characteristics[j].Value.Value = NULL;
			}

			FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Value.Value, pPayload + idx, evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength); idx += evt->DiscoveredServices[i].Characteristics[j].Value.ValueLength;
			evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors = pPayload[idx]; idx++;

			if (evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors > 0)
			{
				evt->DiscoveredServices[i].Characteristics[j].Descriptors = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors * sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[0]));

				if (!evt->DiscoveredServices[i].Characteristics[j].Descriptors)
				{
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
						}
					}
					for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
					}
					MEM_BufferFree(evt->DiscoveredServices);
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->DiscoveredServices[i].Characteristics[j].Descriptors = NULL;
			}


			for (uint32_t k = 0; k < evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors; k++)
			{
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Handle);
				evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].UuidType = (UuidType_t)pPayload[idx]; idx++;

				switch (evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].UuidType)
				{
					case Uuid16Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
						break;

					case Uuid128Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
						break;

					case Uuid32Bits:
						FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
						break;
				}
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength);
				FLib_MemCpy(&(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength), pPayload + idx, sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength)); idx += sizeof(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].MaxValueLength);

				if (evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength > 0)
				{
					evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value = MEM_BufferAlloc(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength);

					if (!evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value)
					{
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
							{
								MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors);
							}
						}
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
							{
								MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
							}
						}
						for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
						}
						MEM_BufferFree(evt->DiscoveredServices);
						return MEM_ALLOC_ERROR_c;
					}

				}
				else
				{
					evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value = NULL;
				}

				FLib_MemCpy(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value, pPayload + idx, evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength); idx += evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].ValueLength;
			}
		}
		evt->DiscoveredServices[i].NbOfIncludedServices = pPayload[idx]; idx++;

		if (evt->DiscoveredServices[i].NbOfIncludedServices > 0)
		{
			evt->DiscoveredServices[i].IncludedServices = MEM_BufferAlloc(evt->DiscoveredServices[i].NbOfIncludedServices * sizeof(evt->DiscoveredServices[i].IncludedServices[0]));

			if (!evt->DiscoveredServices[i].IncludedServices)
			{
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						for (uint32_t k = 0; k < evt->DiscoveredServices[i].Characteristics[j].NbOfDescriptors; k++)
						{
							MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors[k].Value);
						}
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Descriptors);
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfCharacteristics; j++)
					{
						MEM_BufferFree(evt->DiscoveredServices[i].Characteristics[j].Value.Value);
					}
				}
				for (uint32_t i = 0; i < evt->NbOfDiscoveredServices; i++)
				{
					MEM_BufferFree(evt->DiscoveredServices[i].Characteristics);
				}
				MEM_BufferFree(evt->DiscoveredServices);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->DiscoveredServices[i].IncludedServices = NULL;
		}


		for (uint32_t j = 0; j < evt->DiscoveredServices[i].NbOfIncludedServices; j++)
		{
			FLib_MemCpy(&(evt->DiscoveredServices[i].IncludedServices[j].StartHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].IncludedServices[j].StartHandle)); idx += sizeof(evt->DiscoveredServices[i].IncludedServices[j].StartHandle);
			FLib_MemCpy(&(evt->DiscoveredServices[i].IncludedServices[j].EndHandle), pPayload + idx, sizeof(evt->DiscoveredServices[i].IncludedServices[j].EndHandle)); idx += sizeof(evt->DiscoveredServices[i].IncludedServices[j].EndHandle);
			evt->DiscoveredServices[i].IncludedServices[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->DiscoveredServices[i].IncludedServices[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->DiscoveredServices[i].IncludedServices[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			evt->DiscoveredServices[i].IncludedServices[j].NbOfCharacteristics = pPayload[idx]; idx++;
			evt->DiscoveredServices[i].IncludedServices[j].NbOfIncludedServices = pPayload[idx]; idx++;
		}
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientProcedureFindIncludedServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Find Included Services procedure ended
***************************************************************************************************/
static memStatus_t Load_GATTClientProcedureFindIncludedServicesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureFindIncludedServicesIndication_t *evt = &(container->Data.GATTClientProcedureFindIncludedServicesIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4485;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureFindIncludedServicesIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->Service.StartHandle), pPayload + idx, sizeof(evt->Service.StartHandle)); idx += sizeof(evt->Service.StartHandle);
	FLib_MemCpy(&(evt->Service.EndHandle), pPayload + idx, sizeof(evt->Service.EndHandle)); idx += sizeof(evt->Service.EndHandle);
	evt->Service.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Service.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	evt->Service.NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->Service.NbOfCharacteristics > 0)
	{
		evt->Service.Characteristics = MEM_BufferAlloc(evt->Service.NbOfCharacteristics * sizeof(evt->Service.Characteristics[0]));

		if (!evt->Service.Characteristics)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Service.Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
	{
		evt->Service.Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.Handle)); idx += sizeof(evt->Service.Characteristics[i].Value.Handle);
		evt->Service.Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Service.Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Service.Characteristics[i].Value.MaxValueLength);

		if (evt->Service.Characteristics[i].Value.ValueLength > 0)
		{
			evt->Service.Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Service.Characteristics[i].Value.ValueLength);

			if (!evt->Service.Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Service.Characteristics);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->Service.Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Service.Characteristics[i].Value.Value, pPayload + idx, evt->Service.Characteristics[i].Value.ValueLength); idx += evt->Service.Characteristics[i].Value.ValueLength;
		evt->Service.Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Service.Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Service.Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Service.Characteristics[i].NbOfDescriptors * sizeof(evt->Service.Characteristics[i].Descriptors[0]));

			if (!evt->Service.Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Service.Characteristics);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->Service.Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].Handle);
			evt->Service.Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Service.Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Service.Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Service.Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Service.Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Service.Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Service.Characteristics);
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Service.Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Service.Characteristics[i].Descriptors[j].ValueLength); idx += evt->Service.Characteristics[i].Descriptors[j].ValueLength;
		}
	}
	evt->Service.NbOfIncludedServices = pPayload[idx]; idx++;

	if (evt->Service.NbOfIncludedServices > 0)
	{
		evt->Service.IncludedServices = MEM_BufferAlloc(evt->Service.NbOfIncludedServices * sizeof(evt->Service.IncludedServices[0]));

		if (!evt->Service.IncludedServices)
		{
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				for (uint32_t j = 0; j < evt->Service.Characteristics[i].NbOfDescriptors; j++)
				{
					MEM_BufferFree(evt->Service.Characteristics[i].Descriptors[j].Value);
				}
			}
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				MEM_BufferFree(evt->Service.Characteristics[i].Descriptors);
			}
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
			}
			MEM_BufferFree(evt->Service.Characteristics);
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Service.IncludedServices = NULL;
	}


	for (uint32_t i = 0; i < evt->Service.NbOfIncludedServices; i++)
	{
		FLib_MemCpy(&(evt->Service.IncludedServices[i].StartHandle), pPayload + idx, sizeof(evt->Service.IncludedServices[i].StartHandle)); idx += sizeof(evt->Service.IncludedServices[i].StartHandle);
		FLib_MemCpy(&(evt->Service.IncludedServices[i].EndHandle), pPayload + idx, sizeof(evt->Service.IncludedServices[i].EndHandle)); idx += sizeof(evt->Service.IncludedServices[i].EndHandle);
		evt->Service.IncludedServices[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		evt->Service.IncludedServices[i].NbOfCharacteristics = pPayload[idx]; idx++;
		evt->Service.IncludedServices[i].NbOfIncludedServices = pPayload[idx]; idx++;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientProcedureDiscoverAllCharacteristicsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Discovery procedure for a given service ended
***************************************************************************************************/
static memStatus_t Load_GATTClientProcedureDiscoverAllCharacteristicsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureDiscoverAllCharacteristicsIndication_t *evt = &(container->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4486;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureDiscoverAllCharacteristicsIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->Service.StartHandle), pPayload + idx, sizeof(evt->Service.StartHandle)); idx += sizeof(evt->Service.StartHandle);
	FLib_MemCpy(&(evt->Service.EndHandle), pPayload + idx, sizeof(evt->Service.EndHandle)); idx += sizeof(evt->Service.EndHandle);
	evt->Service.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Service.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Service.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	evt->Service.NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->Service.NbOfCharacteristics > 0)
	{
		evt->Service.Characteristics = MEM_BufferAlloc(evt->Service.NbOfCharacteristics * sizeof(evt->Service.Characteristics[0]));

		if (!evt->Service.Characteristics)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Service.Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
	{
		evt->Service.Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.Handle)); idx += sizeof(evt->Service.Characteristics[i].Value.Handle);
		evt->Service.Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Service.Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Service.Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Service.Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Service.Characteristics[i].Value.MaxValueLength);

		if (evt->Service.Characteristics[i].Value.ValueLength > 0)
		{
			evt->Service.Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Service.Characteristics[i].Value.ValueLength);

			if (!evt->Service.Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Service.Characteristics);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->Service.Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Service.Characteristics[i].Value.Value, pPayload + idx, evt->Service.Characteristics[i].Value.ValueLength); idx += evt->Service.Characteristics[i].Value.ValueLength;
		evt->Service.Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Service.Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Service.Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Service.Characteristics[i].NbOfDescriptors * sizeof(evt->Service.Characteristics[i].Descriptors[0]));

			if (!evt->Service.Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Service.Characteristics);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->Service.Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].Handle);
			evt->Service.Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Service.Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Service.Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Service.Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Service.Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Service.Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Service.Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Service.Characteristics);
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Service.Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Service.Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Service.Characteristics[i].Descriptors[j].ValueLength); idx += evt->Service.Characteristics[i].Descriptors[j].ValueLength;
		}
	}
	evt->Service.NbOfIncludedServices = pPayload[idx]; idx++;

	if (evt->Service.NbOfIncludedServices > 0)
	{
		evt->Service.IncludedServices = MEM_BufferAlloc(evt->Service.NbOfIncludedServices * sizeof(evt->Service.IncludedServices[0]));

		if (!evt->Service.IncludedServices)
		{
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				for (uint32_t j = 0; j < evt->Service.Characteristics[i].NbOfDescriptors; j++)
				{
					MEM_BufferFree(evt->Service.Characteristics[i].Descriptors[j].Value);
				}
			}
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				MEM_BufferFree(evt->Service.Characteristics[i].Descriptors);
			}
			for (uint32_t i = 0; i < evt->Service.NbOfCharacteristics; i++)
			{
				MEM_BufferFree(evt->Service.Characteristics[i].Value.Value);
			}
			MEM_BufferFree(evt->Service.Characteristics);
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Service.IncludedServices = NULL;
	}


	for (uint32_t i = 0; i < evt->Service.NbOfIncludedServices; i++)
	{
		FLib_MemCpy(&(evt->Service.IncludedServices[i].StartHandle), pPayload + idx, sizeof(evt->Service.IncludedServices[i].StartHandle)); idx += sizeof(evt->Service.IncludedServices[i].StartHandle);
		FLib_MemCpy(&(evt->Service.IncludedServices[i].EndHandle), pPayload + idx, sizeof(evt->Service.IncludedServices[i].EndHandle)); idx += sizeof(evt->Service.IncludedServices[i].EndHandle);
		evt->Service.IncludedServices[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Service.IncludedServices[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		evt->Service.IncludedServices[i].NbOfCharacteristics = pPayload[idx]; idx++;
		evt->Service.IncludedServices[i].NbOfIncludedServices = pPayload[idx]; idx++;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientProcedureDiscoverCharacteristicByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Discovery procedure for a given service (with a given UUID) ended
***************************************************************************************************/
static memStatus_t Load_GATTClientProcedureDiscoverCharacteristicByUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureDiscoverCharacteristicByUuidIndication_t *evt = &(container->Data.GATTClientProcedureDiscoverCharacteristicByUuidIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4487;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureDiscoverCharacteristicByUuidIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->NbOfCharacteristics > 0)
	{
		evt->Characteristics = MEM_BufferAlloc(evt->NbOfCharacteristics * sizeof(evt->Characteristics[0]));

		if (!evt->Characteristics)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
	{
		evt->Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Characteristics[i].Value.Handle)); idx += sizeof(evt->Characteristics[i].Value.Handle);
		evt->Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Characteristics[i].Value.MaxValueLength);

		if (evt->Characteristics[i].Value.ValueLength > 0)
		{
			evt->Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Characteristics[i].Value.ValueLength);

			if (!evt->Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Characteristics);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Characteristics[i].Value.Value, pPayload + idx, evt->Characteristics[i].Value.ValueLength); idx += evt->Characteristics[i].Value.ValueLength;
		evt->Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Characteristics[i].NbOfDescriptors * sizeof(evt->Characteristics[i].Descriptors[0]));

			if (!evt->Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Characteristics);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Characteristics[i].Descriptors[j].Handle);
			evt->Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Characteristics);
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Characteristics[i].Descriptors[j].ValueLength); idx += evt->Characteristics[i].Descriptors[j].ValueLength;
		}
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Descriptor Discovery procedure for a given characteristic ended
***************************************************************************************************/
static memStatus_t Load_GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_t *evt = &(container->Data.GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4488;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->Characteristic.Properties = (Properties_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Characteristic.Value.Handle), pPayload + idx, sizeof(evt->Characteristic.Value.Handle)); idx += sizeof(evt->Characteristic.Value.Handle);
	evt->Characteristic.Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	FLib_MemCpy(&(evt->Characteristic.Value.ValueLength), pPayload + idx, sizeof(evt->Characteristic.Value.ValueLength)); idx += sizeof(evt->Characteristic.Value.ValueLength);
	FLib_MemCpy(&(evt->Characteristic.Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristic.Value.MaxValueLength)); idx += sizeof(evt->Characteristic.Value.MaxValueLength);

	if (evt->Characteristic.Value.ValueLength > 0)
	{
		evt->Characteristic.Value.Value = MEM_BufferAlloc(evt->Characteristic.Value.ValueLength);

		if (!evt->Characteristic.Value.Value)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Characteristic.Value.Value = NULL;
	}

	FLib_MemCpy(evt->Characteristic.Value.Value, pPayload + idx, evt->Characteristic.Value.ValueLength); idx += evt->Characteristic.Value.ValueLength;
	evt->Characteristic.NbOfDescriptors = pPayload[idx]; idx++;

	if (evt->Characteristic.NbOfDescriptors > 0)
	{
		evt->Characteristic.Descriptors = MEM_BufferAlloc(evt->Characteristic.NbOfDescriptors * sizeof(evt->Characteristic.Descriptors[0]));

		if (!evt->Characteristic.Descriptors)
		{
			MEM_BufferFree(evt->Characteristic.Value.Value);
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Characteristic.Descriptors = NULL;
	}


	for (uint32_t i = 0; i < evt->Characteristic.NbOfDescriptors; i++)
	{
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].Handle), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].Handle)); idx += sizeof(evt->Characteristic.Descriptors[i].Handle);
		evt->Characteristic.Descriptors[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].ValueLength), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].ValueLength)); idx += sizeof(evt->Characteristic.Descriptors[i].ValueLength);
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].MaxValueLength), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].MaxValueLength)); idx += sizeof(evt->Characteristic.Descriptors[i].MaxValueLength);

		if (evt->Characteristic.Descriptors[i].ValueLength > 0)
		{
			evt->Characteristic.Descriptors[i].Value = MEM_BufferAlloc(evt->Characteristic.Descriptors[i].ValueLength);

			if (!evt->Characteristic.Descriptors[i].Value)
			{
				MEM_BufferFree(evt->Characteristic.Descriptors);
				MEM_BufferFree(evt->Characteristic.Value.Value);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->Characteristic.Descriptors[i].Value = NULL;
		}

		FLib_MemCpy(evt->Characteristic.Descriptors[i].Value, pPayload + idx, evt->Characteristic.Descriptors[i].ValueLength); idx += evt->Characteristic.Descriptors[i].ValueLength;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientProcedureReadCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Read procedure for a given characteristic ended
***************************************************************************************************/
static memStatus_t Load_GATTClientProcedureReadCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureReadCharacteristicValueIndication_t *evt = &(container->Data.GATTClientProcedureReadCharacteristicValueIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4489;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureReadCharacteristicValueIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->Characteristic.Properties = (Properties_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Characteristic.Value.Handle), pPayload + idx, sizeof(evt->Characteristic.Value.Handle)); idx += sizeof(evt->Characteristic.Value.Handle);
	evt->Characteristic.Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Characteristic.Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	FLib_MemCpy(&(evt->Characteristic.Value.ValueLength), pPayload + idx, sizeof(evt->Characteristic.Value.ValueLength)); idx += sizeof(evt->Characteristic.Value.ValueLength);
	FLib_MemCpy(&(evt->Characteristic.Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristic.Value.MaxValueLength)); idx += sizeof(evt->Characteristic.Value.MaxValueLength);

	if (evt->Characteristic.Value.ValueLength > 0)
	{
		evt->Characteristic.Value.Value = MEM_BufferAlloc(evt->Characteristic.Value.ValueLength);

		if (!evt->Characteristic.Value.Value)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Characteristic.Value.Value = NULL;
	}

	FLib_MemCpy(evt->Characteristic.Value.Value, pPayload + idx, evt->Characteristic.Value.ValueLength); idx += evt->Characteristic.Value.ValueLength;
	evt->Characteristic.NbOfDescriptors = pPayload[idx]; idx++;

	if (evt->Characteristic.NbOfDescriptors > 0)
	{
		evt->Characteristic.Descriptors = MEM_BufferAlloc(evt->Characteristic.NbOfDescriptors * sizeof(evt->Characteristic.Descriptors[0]));

		if (!evt->Characteristic.Descriptors)
		{
			MEM_BufferFree(evt->Characteristic.Value.Value);
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Characteristic.Descriptors = NULL;
	}


	for (uint32_t i = 0; i < evt->Characteristic.NbOfDescriptors; i++)
	{
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].Handle), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].Handle)); idx += sizeof(evt->Characteristic.Descriptors[i].Handle);
		evt->Characteristic.Descriptors[i].UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristic.Descriptors[i].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].ValueLength), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].ValueLength)); idx += sizeof(evt->Characteristic.Descriptors[i].ValueLength);
		FLib_MemCpy(&(evt->Characteristic.Descriptors[i].MaxValueLength), pPayload + idx, sizeof(evt->Characteristic.Descriptors[i].MaxValueLength)); idx += sizeof(evt->Characteristic.Descriptors[i].MaxValueLength);

		if (evt->Characteristic.Descriptors[i].ValueLength > 0)
		{
			evt->Characteristic.Descriptors[i].Value = MEM_BufferAlloc(evt->Characteristic.Descriptors[i].ValueLength);

			if (!evt->Characteristic.Descriptors[i].Value)
			{
				MEM_BufferFree(evt->Characteristic.Descriptors);
				MEM_BufferFree(evt->Characteristic.Value.Value);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->Characteristic.Descriptors[i].Value = NULL;
		}

		FLib_MemCpy(evt->Characteristic.Descriptors[i].Value, pPayload + idx, evt->Characteristic.Descriptors[i].ValueLength); idx += evt->Characteristic.Descriptors[i].ValueLength;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientProcedureReadUsingCharacteristicUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Read By UUID procedure ended
***************************************************************************************************/
static memStatus_t Load_GATTClientProcedureReadUsingCharacteristicUuidIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureReadUsingCharacteristicUuidIndication_t *evt = &(container->Data.GATTClientProcedureReadUsingCharacteristicUuidIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x448A;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureReadUsingCharacteristicUuidIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->NbOfReadBytes), pPayload + idx, sizeof(evt->NbOfReadBytes)); idx += sizeof(evt->NbOfReadBytes);

	if (evt->NbOfReadBytes > 0)
	{
		evt->ReadBytes = MEM_BufferAlloc(evt->NbOfReadBytes);

		if (!evt->ReadBytes)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->ReadBytes = NULL;
	}

	FLib_MemCpy(evt->ReadBytes, pPayload + idx, evt->NbOfReadBytes); idx += evt->NbOfReadBytes;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientProcedureReadMultipleCharacteristicValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Read Multiple procedure ended
***************************************************************************************************/
static memStatus_t Load_GATTClientProcedureReadMultipleCharacteristicValuesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureReadMultipleCharacteristicValuesIndication_t *evt = &(container->Data.GATTClientProcedureReadMultipleCharacteristicValuesIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x448B;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureReadMultipleCharacteristicValuesIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	evt->NbOfCharacteristics = pPayload[idx]; idx++;

	if (evt->NbOfCharacteristics > 0)
	{
		evt->Characteristics = MEM_BufferAlloc(evt->NbOfCharacteristics * sizeof(evt->Characteristics[0]));

		if (!evt->Characteristics)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Characteristics = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
	{
		evt->Characteristics[i].Properties = (Properties_t)pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->Characteristics[i].Value.Handle), pPayload + idx, sizeof(evt->Characteristics[i].Value.Handle)); idx += sizeof(evt->Characteristics[i].Value.Handle);
		evt->Characteristics[i].Value.UuidType = (UuidType_t)pPayload[idx]; idx++;

		switch (evt->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(evt->Characteristics[i].Value.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
				break;
		}
		FLib_MemCpy(&(evt->Characteristics[i].Value.ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.ValueLength)); idx += sizeof(evt->Characteristics[i].Value.ValueLength);
		FLib_MemCpy(&(evt->Characteristics[i].Value.MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Value.MaxValueLength)); idx += sizeof(evt->Characteristics[i].Value.MaxValueLength);

		if (evt->Characteristics[i].Value.ValueLength > 0)
		{
			evt->Characteristics[i].Value.Value = MEM_BufferAlloc(evt->Characteristics[i].Value.ValueLength);

			if (!evt->Characteristics[i].Value.Value)
			{
				MEM_BufferFree(evt->Characteristics);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->Characteristics[i].Value.Value = NULL;
		}

		FLib_MemCpy(evt->Characteristics[i].Value.Value, pPayload + idx, evt->Characteristics[i].Value.ValueLength); idx += evt->Characteristics[i].Value.ValueLength;
		evt->Characteristics[i].NbOfDescriptors = pPayload[idx]; idx++;

		if (evt->Characteristics[i].NbOfDescriptors > 0)
		{
			evt->Characteristics[i].Descriptors = MEM_BufferAlloc(evt->Characteristics[i].NbOfDescriptors * sizeof(evt->Characteristics[i].Descriptors[0]));

			if (!evt->Characteristics[i].Descriptors)
			{
				for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
				{
					MEM_BufferFree(evt->Characteristics[i].Value.Value);
				}
				MEM_BufferFree(evt->Characteristics);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->Characteristics[i].Descriptors = NULL;
		}


		for (uint32_t j = 0; j < evt->Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].Handle), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].Handle)); idx += sizeof(evt->Characteristics[i].Descriptors[j].Handle);
			evt->Characteristics[i].Descriptors[j].UuidType = (UuidType_t)pPayload[idx]; idx++;

			switch (evt->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
					break;
			}
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].ValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(&(evt->Characteristics[i].Descriptors[j].MaxValueLength), pPayload + idx, sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(evt->Characteristics[i].Descriptors[j].MaxValueLength);

			if (evt->Characteristics[i].Descriptors[j].ValueLength > 0)
			{
				evt->Characteristics[i].Descriptors[j].Value = MEM_BufferAlloc(evt->Characteristics[i].Descriptors[j].ValueLength);

				if (!evt->Characteristics[i].Descriptors[j].Value)
				{
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Descriptors);
					}
					for (uint32_t i = 0; i < evt->NbOfCharacteristics; i++)
					{
						MEM_BufferFree(evt->Characteristics[i].Value.Value);
					}
					MEM_BufferFree(evt->Characteristics);
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Characteristics[i].Descriptors[j].Value = NULL;
			}

			FLib_MemCpy(evt->Characteristics[i].Descriptors[j].Value, pPayload + idx, evt->Characteristics[i].Descriptors[j].ValueLength); idx += evt->Characteristics[i].Descriptors[j].ValueLength;
		}
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientProcedureWriteCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Write procedure for a given characteristic ended
***************************************************************************************************/
static memStatus_t Load_GATTClientProcedureWriteCharacteristicValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureWriteCharacteristicValueIndication_t *evt = &(container->Data.GATTClientProcedureWriteCharacteristicValueIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x448C;

	FLib_MemCpy(evt, pPayload, sizeof(GATTClientProcedureWriteCharacteristicValueIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientProcedureReadCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Descriptor Read procedure for a given characteristic's descriptor ended
***************************************************************************************************/
static memStatus_t Load_GATTClientProcedureReadCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureReadCharacteristicDescriptorIndication_t *evt = &(container->Data.GATTClientProcedureReadCharacteristicDescriptorIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x448D;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->ProcedureResult = (GATTClientProcedureReadCharacteristicDescriptorIndication_ProcedureResult_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Error), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->Descriptor.Handle), pPayload + idx, sizeof(evt->Descriptor.Handle)); idx += sizeof(evt->Descriptor.Handle);
	evt->Descriptor.UuidType = (UuidType_t)pPayload[idx]; idx++;

	switch (evt->Descriptor.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(evt->Descriptor.Uuid.Uuid16Bits, pPayload + idx, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(evt->Descriptor.Uuid.Uuid128Bits, pPayload + idx, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(evt->Descriptor.Uuid.Uuid32Bits, pPayload + idx, 4); idx += 4;
			break;
	}
	FLib_MemCpy(&(evt->Descriptor.ValueLength), pPayload + idx, sizeof(evt->Descriptor.ValueLength)); idx += sizeof(evt->Descriptor.ValueLength);
	FLib_MemCpy(&(evt->Descriptor.MaxValueLength), pPayload + idx, sizeof(evt->Descriptor.MaxValueLength)); idx += sizeof(evt->Descriptor.MaxValueLength);

	if (evt->Descriptor.ValueLength > 0)
	{
		evt->Descriptor.Value = MEM_BufferAlloc(evt->Descriptor.ValueLength);

		if (!evt->Descriptor.Value)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Descriptor.Value = NULL;
	}

	FLib_MemCpy(evt->Descriptor.Value, pPayload + idx, evt->Descriptor.ValueLength); idx += evt->Descriptor.ValueLength;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientProcedureWriteCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Specifies that the Characteristic Descriptor Write procedure for a given characteristic's descriptor ended
***************************************************************************************************/
static memStatus_t Load_GATTClientProcedureWriteCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientProcedureWriteCharacteristicDescriptorIndication_t *evt = &(container->Data.GATTClientProcedureWriteCharacteristicDescriptorIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x448E;

	FLib_MemCpy(evt, pPayload, sizeof(GATTClientProcedureWriteCharacteristicDescriptorIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Client notification
***************************************************************************************************/
static memStatus_t Load_GATTClientNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientNotificationIndication_t *evt = &(container->Data.GATTClientNotificationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x448F;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->CharacteristicValueHandle), pPayload + idx, sizeof(evt->CharacteristicValueHandle)); idx += sizeof(evt->CharacteristicValueHandle);
	FLib_MemCpy(&(evt->ValueLength), pPayload + idx, sizeof(evt->ValueLength)); idx += sizeof(evt->ValueLength);

	if (evt->ValueLength > 0)
	{
		evt->Value = MEM_BufferAlloc(evt->ValueLength);

		if (!evt->Value)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Value = NULL;
	}

	FLib_MemCpy(evt->Value, pPayload + idx, evt->ValueLength); idx += evt->ValueLength;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTClientIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Client indication
***************************************************************************************************/
static memStatus_t Load_GATTClientIndicationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTClientIndicationIndication_t *evt = &(container->Data.GATTClientIndicationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4490;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->CharacteristicValueHandle), pPayload + idx, sizeof(evt->CharacteristicValueHandle)); idx += sizeof(evt->CharacteristicValueHandle);
	FLib_MemCpy(&(evt->ValueLength), pPayload + idx, sizeof(evt->ValueLength)); idx += sizeof(evt->ValueLength);

	if (evt->ValueLength > 0)
	{
		evt->Value = MEM_BufferAlloc(evt->ValueLength);

		if (!evt->Value)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Value = NULL;
	}

	FLib_MemCpy(evt->Value, pPayload + idx, evt->ValueLength); idx += evt->ValueLength;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTServerMtuChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server MTU changed indication
***************************************************************************************************/
static memStatus_t Load_GATTServerMtuChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerMtuChangedIndication_t *evt = &(container->Data.GATTServerMtuChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4491;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerMtuChangedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTServerHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server handle value confirmation
***************************************************************************************************/
static memStatus_t Load_GATTServerHandleValueConfirmationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerHandleValueConfirmationIndication_t *evt = &(container->Data.GATTServerHandleValueConfirmationIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4492;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerHandleValueConfirmationIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTServerAttributeWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server attribute written
***************************************************************************************************/
static memStatus_t Load_GATTServerAttributeWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerAttributeWrittenIndication_t *evt = &(container->Data.GATTServerAttributeWrittenIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4493;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->AttributeWrittenEvent.Handle), pPayload + idx, sizeof(evt->AttributeWrittenEvent.Handle)); idx += sizeof(evt->AttributeWrittenEvent.Handle);
	FLib_MemCpy(&(evt->AttributeWrittenEvent.ValueLength), pPayload + idx, sizeof(evt->AttributeWrittenEvent.ValueLength)); idx += sizeof(evt->AttributeWrittenEvent.ValueLength);

	if (evt->AttributeWrittenEvent.ValueLength > 0)
	{
		evt->AttributeWrittenEvent.Value = MEM_BufferAlloc(evt->AttributeWrittenEvent.ValueLength);

		if (!evt->AttributeWrittenEvent.Value)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->AttributeWrittenEvent.Value = NULL;
	}

	FLib_MemCpy(evt->AttributeWrittenEvent.Value, pPayload + idx, evt->AttributeWrittenEvent.ValueLength); idx += evt->AttributeWrittenEvent.ValueLength;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTServerCharacteristicCccdWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server characteristic cccd written
***************************************************************************************************/
static memStatus_t Load_GATTServerCharacteristicCccdWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerCharacteristicCccdWrittenIndication_t *evt = &(container->Data.GATTServerCharacteristicCccdWrittenIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4494;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerCharacteristicCccdWrittenIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTServerAttributeWrittenWithoutResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server attribute written without response
***************************************************************************************************/
static memStatus_t Load_GATTServerAttributeWrittenWithoutResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerAttributeWrittenWithoutResponseIndication_t *evt = &(container->Data.GATTServerAttributeWrittenWithoutResponseIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4495;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->AttributeWrittenEvent.Handle), pPayload + idx, sizeof(evt->AttributeWrittenEvent.Handle)); idx += sizeof(evt->AttributeWrittenEvent.Handle);
	FLib_MemCpy(&(evt->AttributeWrittenEvent.ValueLength), pPayload + idx, sizeof(evt->AttributeWrittenEvent.ValueLength)); idx += sizeof(evt->AttributeWrittenEvent.ValueLength);

	if (evt->AttributeWrittenEvent.ValueLength > 0)
	{
		evt->AttributeWrittenEvent.Value = MEM_BufferAlloc(evt->AttributeWrittenEvent.ValueLength);

		if (!evt->AttributeWrittenEvent.Value)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->AttributeWrittenEvent.Value = NULL;
	}

	FLib_MemCpy(evt->AttributeWrittenEvent.Value, pPayload + idx, evt->AttributeWrittenEvent.ValueLength); idx += evt->AttributeWrittenEvent.ValueLength;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTServerErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server error
***************************************************************************************************/
static memStatus_t Load_GATTServerErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerErrorIndication_t *evt = &(container->Data.GATTServerErrorIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4496;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerErrorIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTServerLongCharacteristicWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server long characteristic written
***************************************************************************************************/
static memStatus_t Load_GATTServerLongCharacteristicWrittenIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerLongCharacteristicWrittenIndication_t *evt = &(container->Data.GATTServerLongCharacteristicWrittenIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4497;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->LongCharacteristicWrittenEvent.Handle), pPayload + idx, sizeof(evt->LongCharacteristicWrittenEvent.Handle)); idx += sizeof(evt->LongCharacteristicWrittenEvent.Handle);
	FLib_MemCpy(&(evt->LongCharacteristicWrittenEvent.ValueLength), pPayload + idx, sizeof(evt->LongCharacteristicWrittenEvent.ValueLength)); idx += sizeof(evt->LongCharacteristicWrittenEvent.ValueLength);

	if (evt->LongCharacteristicWrittenEvent.ValueLength > 0)
	{
		evt->LongCharacteristicWrittenEvent.Value = MEM_BufferAlloc(evt->LongCharacteristicWrittenEvent.ValueLength);

		if (!evt->LongCharacteristicWrittenEvent.Value)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->LongCharacteristicWrittenEvent.Value = NULL;
	}

	FLib_MemCpy(evt->LongCharacteristicWrittenEvent.Value, pPayload + idx, evt->LongCharacteristicWrittenEvent.ValueLength); idx += evt->LongCharacteristicWrittenEvent.ValueLength;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTServerAttributeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	GATT Server attribute read
***************************************************************************************************/
static memStatus_t Load_GATTServerAttributeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTServerAttributeReadIndication_t *evt = &(container->Data.GATTServerAttributeReadIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4498;

	FLib_MemCpy(evt, pPayload, sizeof(GATTServerAttributeReadIndication_t));

	return MEM_SUCCESS_c;
}

#endif  /* GATT_ENABLE */

#if GATTDB_APP_ENABLE
/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTDBConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the GATT Database (application) request
***************************************************************************************************/
static memStatus_t Load_GATTDBConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBConfirm_t *evt = &(container->Data.GATTDBConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0x4580;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTDBReadAttributeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Reads an attribute value (from application level)
***************************************************************************************************/
static memStatus_t Load_GATTDBReadAttributeIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBReadAttributeIndication_t *evt = &(container->Data.GATTDBReadAttributeIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4581;

	FLib_MemCpy(&(evt->ValueLength), pPayload + idx, sizeof(evt->ValueLength)); idx += sizeof(evt->ValueLength);

	if (evt->ValueLength > 0)
	{
		evt->Value = MEM_BufferAlloc(evt->ValueLength);

		if (!evt->Value)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Value = NULL;
	}

	FLib_MemCpy(evt->Value, pPayload + idx, evt->ValueLength); idx += evt->ValueLength;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTDBFindServiceHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Finds the handle of a Service Declaration with a given UUID inside the database
***************************************************************************************************/
static memStatus_t Load_GATTDBFindServiceHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBFindServiceHandleIndication_t *evt = &(container->Data.GATTDBFindServiceHandleIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4582;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBFindServiceHandleIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTDBFindCharValueHandleInServiceIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Finds the handle of a characteristic value (with a given UUID) inside a service
***************************************************************************************************/
static memStatus_t Load_GATTDBFindCharValueHandleInServiceIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBFindCharValueHandleInServiceIndication_t *evt = &(container->Data.GATTDBFindCharValueHandleInServiceIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4583;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBFindCharValueHandleInServiceIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTDBFindCccdHandleForCharValueHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Finds the handle of a characteristic's CCCD (giving the characteristic's value handle)
***************************************************************************************************/
static memStatus_t Load_GATTDBFindCccdHandleForCharValueHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBFindCccdHandleForCharValueHandleIndication_t *evt = &(container->Data.GATTDBFindCccdHandleForCharValueHandleIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4584;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBFindCccdHandleForCharValueHandleIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTDBFindDescriptorHandleForCharValueHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Finds the handle of a characteristic descriptor (giving the characteristic's value handle and descriptor's UUID)
***************************************************************************************************/
static memStatus_t Load_GATTDBFindDescriptorHandleForCharValueHandleIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBFindDescriptorHandleForCharValueHandleIndication_t *evt = &(container->Data.GATTDBFindDescriptorHandleForCharValueHandleIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4585;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBFindDescriptorHandleForCharValueHandleIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTDBDynamicAddPrimaryServiceDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a Primary Service declaration into the database
***************************************************************************************************/
static memStatus_t Load_GATTDBDynamicAddPrimaryServiceDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddPrimaryServiceDeclarationIndication_t *evt = &(container->Data.GATTDBDynamicAddPrimaryServiceDeclarationIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4586;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddPrimaryServiceDeclarationIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTDBDynamicAddSecondaryServiceDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a Secondary Service declaration into the database
***************************************************************************************************/
static memStatus_t Load_GATTDBDynamicAddSecondaryServiceDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddSecondaryServiceDeclarationIndication_t *evt = &(container->Data.GATTDBDynamicAddSecondaryServiceDeclarationIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4587;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddSecondaryServiceDeclarationIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTDBDynamicAddIncludeDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds an Include declaration into the database
***************************************************************************************************/
static memStatus_t Load_GATTDBDynamicAddIncludeDeclarationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddIncludeDeclarationIndication_t *evt = &(container->Data.GATTDBDynamicAddIncludeDeclarationIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4588;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddIncludeDeclarationIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTDBDynamicAddCharacteristicDeclarationAndValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a Characteristic declaration and its Value into the database
***************************************************************************************************/
static memStatus_t Load_GATTDBDynamicAddCharacteristicDeclarationAndValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddCharacteristicDeclarationAndValueIndication_t *evt = &(container->Data.GATTDBDynamicAddCharacteristicDeclarationAndValueIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4589;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddCharacteristicDeclarationAndValueIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTDBDynamicAddCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a Characteristic descriptor into the database
***************************************************************************************************/
static memStatus_t Load_GATTDBDynamicAddCharacteristicDescriptorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddCharacteristicDescriptorIndication_t *evt = &(container->Data.GATTDBDynamicAddCharacteristicDescriptorIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x458A;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddCharacteristicDescriptorIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTDBDynamicAddCccdIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a CCCD in the database
***************************************************************************************************/
static memStatus_t Load_GATTDBDynamicAddCccdIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddCccdIndication_t *evt = &(container->Data.GATTDBDynamicAddCccdIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x458B;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddCccdIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Adds a Characteristic declaration with a Value contained in an universal value buffer
***************************************************************************************************/
static memStatus_t Load_GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication_t *evt = &(container->Data.GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x458C;

	FLib_MemCpy(evt, pPayload, sizeof(GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication_t));

	return MEM_SUCCESS_c;
}

#endif  /* GATTDB_APP_ENABLE */

#if GAP_ENABLE
/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Status of the GAP request
***************************************************************************************************/
static memStatus_t Load_GAPConfirm(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConfirm_t *evt = &(container->Data.GAPConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0x4780;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPCheckNotificationStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the notification status for a given Client and a given CCCD handle
***************************************************************************************************/
static memStatus_t Load_GAPCheckNotificationStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPCheckNotificationStatusIndication_t *evt = &(container->Data.GAPCheckNotificationStatusIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4781;

	FLib_MemCpy(evt, pPayload, sizeof(GAPCheckNotificationStatusIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPCheckIndicationStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the indication status for a given Client and a given CCCD handle
***************************************************************************************************/
static memStatus_t Load_GAPCheckIndicationStatusIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPCheckIndicationStatusIndication_t *evt = &(container->Data.GAPCheckIndicationStatusIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4782;

	FLib_MemCpy(evt, pPayload, sizeof(GAPCheckIndicationStatusIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGetBondedStaticAddressesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns a list with the static addresses of bonded devices
***************************************************************************************************/
static memStatus_t Load_GAPGetBondedStaticAddressesIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGetBondedStaticAddressesIndication_t *evt = &(container->Data.GAPGetBondedStaticAddressesIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4783;

	evt->NbOfDeviceAddresses = pPayload[idx]; idx++;

	if (evt->NbOfDeviceAddresses > 0)
	{
		evt->DeviceAddresses = MEM_BufferAlloc(evt->NbOfDeviceAddresses * 6);

		if (!evt->DeviceAddresses)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->DeviceAddresses = NULL;
	}

	FLib_MemCpy(evt->DeviceAddresses, pPayload + idx, evt->NbOfDeviceAddresses * 6); idx += evt->NbOfDeviceAddresses * 6;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPLoadEncryptionInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the encryption key for a bonded device
***************************************************************************************************/
static memStatus_t Load_GAPLoadEncryptionInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPLoadEncryptionInformationIndication_t *evt = &(container->Data.GAPLoadEncryptionInformationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4784;

	evt->LtkSize = pPayload[idx]; idx++;

	if (evt->LtkSize > 0)
	{
		evt->Ltk = MEM_BufferAlloc(evt->LtkSize);

		if (!evt->Ltk)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Ltk = NULL;
	}

	FLib_MemCpy(evt->Ltk, pPayload + idx, evt->LtkSize); idx += evt->LtkSize;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPLoadCustomPeerInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the custom peer information in raw data format
***************************************************************************************************/
static memStatus_t Load_GAPLoadCustomPeerInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPLoadCustomPeerInformationIndication_t *evt = &(container->Data.GAPLoadCustomPeerInformationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4785;

	FLib_MemCpy(&(evt->InfoSize), pPayload + idx, sizeof(evt->InfoSize)); idx += sizeof(evt->InfoSize);

	if (evt->InfoSize > 0)
	{
		evt->Info = MEM_BufferAlloc(evt->InfoSize);

		if (!evt->Info)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Info = NULL;
	}

	FLib_MemCpy(evt->Info, pPayload + idx, evt->InfoSize); idx += evt->InfoSize;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPCheckIfBondedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns whether a connected peer device is bonded or not
***************************************************************************************************/
static memStatus_t Load_GAPCheckIfBondedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPCheckIfBondedIndication_t *evt = &(container->Data.GAPCheckIfBondedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4786;

	FLib_MemCpy(evt, pPayload, sizeof(GAPCheckIfBondedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGetBondedDevicesCountIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the number of bonded devices
***************************************************************************************************/
static memStatus_t Load_GAPGetBondedDevicesCountIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGetBondedDevicesCountIndication_t *evt = &(container->Data.GAPGetBondedDevicesCountIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4787;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGetBondedDevicesCountIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGetBondedDeviceNameIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns the name of a bonded device
***************************************************************************************************/
static memStatus_t Load_GAPGetBondedDeviceNameIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGetBondedDeviceNameIndication_t *evt = &(container->Data.GAPGetBondedDeviceNameIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4788;

	evt->NameSize = pPayload[idx]; idx++;

	if (evt->NameSize > 0)
	{
		evt->Name = MEM_BufferAlloc(evt->NameSize);

		if (!evt->Name)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Name = NULL;
	}

	FLib_MemCpy(evt->Name, pPayload + idx, evt->NameSize); idx += evt->NameSize;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventInitializationCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - initialization complete
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventInitializationCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventInitializationCompleteIndication_t *evt = &(container->Data.GAPGenericEventInitializationCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4789;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventInitializationCompleteIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventInternalErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - controller error
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventInternalErrorIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventInternalErrorIndication_t *evt = &(container->Data.GAPGenericEventInternalErrorIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x478A;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventInternalErrorIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventAdvertisingSetupFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - advertising setup failed
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventAdvertisingSetupFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventAdvertisingSetupFailedIndication_t *evt = &(container->Data.GAPGenericEventAdvertisingSetupFailedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x478B;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventAdvertisingSetupFailedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventAdvertisingParametersSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - advertising parameters setup completed
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventAdvertisingParametersSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventAdvertisingParametersSetupCompleteIndication_t *evt = &(container->Data.GAPGenericEventAdvertisingParametersSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x478C;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventAdvertisingParametersSetupCompleteIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventAdvertisingDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - advertising data setup completed
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventAdvertisingDataSetupCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventAdvertisingDataSetupCompleteIndication_t *evt = &(container->Data.GAPGenericEventAdvertisingDataSetupCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x478D;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventAdvertisingDataSetupCompleteIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventWhiteListSizeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - White List size
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventWhiteListSizeReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventWhiteListSizeReadIndication_t *evt = &(container->Data.GAPGenericEventWhiteListSizeReadIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x478E;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventWhiteListSizeReadIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventDeviceAddedToWhiteListIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - device added to White List
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventDeviceAddedToWhiteListIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventDeviceAddedToWhiteListIndication_t *evt = &(container->Data.GAPGenericEventDeviceAddedToWhiteListIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x478F;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventDeviceAddedToWhiteListIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventDeviceRemovedFromWhiteListIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - device removed from White List
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventDeviceRemovedFromWhiteListIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventDeviceRemovedFromWhiteListIndication_t *evt = &(container->Data.GAPGenericEventDeviceRemovedFromWhiteListIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4790;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventDeviceRemovedFromWhiteListIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventWhiteListClearedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - White List cleared
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventWhiteListClearedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventWhiteListClearedIndication_t *evt = &(container->Data.GAPGenericEventWhiteListClearedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4791;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventWhiteListClearedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventRandomAddressReadyIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - random address ready
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventRandomAddressReadyIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventRandomAddressReadyIndication_t *evt = &(container->Data.GAPGenericEventRandomAddressReadyIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4792;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventRandomAddressReadyIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventCreateConnectionCanceledIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - create connection procedure canceled
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventCreateConnectionCanceledIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventCreateConnectionCanceledIndication_t *evt = &(container->Data.GAPGenericEventCreateConnectionCanceledIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4793;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventCreateConnectionCanceledIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventPublicAddressReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - public address read
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventPublicAddressReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventPublicAddressReadIndication_t *evt = &(container->Data.GAPGenericEventPublicAddressReadIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4794;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventPublicAddressReadIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventAdvTxPowerLevelReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - advertising transmission power level
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventAdvTxPowerLevelReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventAdvTxPowerLevelReadIndication_t *evt = &(container->Data.GAPGenericEventAdvTxPowerLevelReadIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4795;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventAdvTxPowerLevelReadIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventPrivateResolvableAddressVerifiedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - Private Resolvable Address verified
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventPrivateResolvableAddressVerifiedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventPrivateResolvableAddressVerifiedIndication_t *evt = &(container->Data.GAPGenericEventPrivateResolvableAddressVerifiedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4796;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventPrivateResolvableAddressVerifiedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventRandomAddressSetIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Controller event - Private Resolvable Address verified
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventRandomAddressSetIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventRandomAddressSetIndication_t *evt = &(container->Data.GAPGenericEventRandomAddressSetIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4797;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventRandomAddressSetIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPAdvertisingEventStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event received when advertising has been successfully enabled or disabled
***************************************************************************************************/
static memStatus_t Load_GAPAdvertisingEventStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPAdvertisingEventStateChangedIndication_t *evt = &(container->Data.GAPAdvertisingEventStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4798;

	FLib_MemCpy(evt, pPayload, sizeof(GAPAdvertisingEventStateChangedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPAdvertisingEventCommandFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event received when advertising could not be enabled or disabled
***************************************************************************************************/
static memStatus_t Load_GAPAdvertisingEventCommandFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPAdvertisingEventCommandFailedIndication_t *evt = &(container->Data.GAPAdvertisingEventCommandFailedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x4799;

	FLib_MemCpy(evt, pPayload, sizeof(GAPAdvertisingEventCommandFailedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPScanningEventStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event received when scanning had been successfully enabled or disabled
***************************************************************************************************/
static memStatus_t Load_GAPScanningEventStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPScanningEventStateChangedIndication_t *evt = &(container->Data.GAPScanningEventStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x479A;

	FLib_MemCpy(evt, pPayload, sizeof(GAPScanningEventStateChangedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPScanningEventCommandFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event received when scanning could not be enabled or disabled
***************************************************************************************************/
static memStatus_t Load_GAPScanningEventCommandFailedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPScanningEventCommandFailedIndication_t *evt = &(container->Data.GAPScanningEventCommandFailedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x479B;

	FLib_MemCpy(evt, pPayload, sizeof(GAPScanningEventCommandFailedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPScanningEventDeviceScannedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event received when an advertising device has been scanned
***************************************************************************************************/
static memStatus_t Load_GAPScanningEventDeviceScannedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPScanningEventDeviceScannedIndication_t *evt = &(container->Data.GAPScanningEventDeviceScannedIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x479C;

	evt->AddressType = (GAPScanningEventDeviceScannedIndication_AddressType_t)pPayload[idx]; idx++;
	FLib_MemCpy(evt->Address, pPayload + idx, 6); idx += 6;
	evt->Rssi = pPayload[idx]; idx++;
	evt->DataLength = pPayload[idx]; idx++;

	if (evt->DataLength > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->DataLength);

		if (!evt->Data)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->DataLength); idx += evt->DataLength;
	evt->AdvEventType = (GAPScanningEventDeviceScannedIndication_AdvEventType_t)pPayload[idx]; idx++;
	evt->DirectRpaUsed = (bool_t)pPayload[idx]; idx++;


	evt->advertisingAddressResolved = (bool_t)pPayload[idx]; idx++;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventConnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A connection has been established
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventConnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventConnectedIndication_t *evt = &(container->Data.GAPConnectionEventConnectedIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x479D;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->ConnectionParameters.ConnInterval), pPayload + idx, sizeof(evt->ConnectionParameters.ConnInterval)); idx += sizeof(evt->ConnectionParameters.ConnInterval);
	FLib_MemCpy(&(evt->ConnectionParameters.ConnLatency), pPayload + idx, sizeof(evt->ConnectionParameters.ConnLatency)); idx += sizeof(evt->ConnectionParameters.ConnLatency);
	FLib_MemCpy(&(evt->ConnectionParameters.SupervisionTimeout), pPayload + idx, sizeof(evt->ConnectionParameters.SupervisionTimeout)); idx += sizeof(evt->ConnectionParameters.SupervisionTimeout);
	evt->ConnectionParameters.MasterClockAccuracy = (GAPConnectionEventConnectedIndication_ConnectionParameters_MasterClockAccuracy_t)pPayload[idx]; idx++;
	evt->PeerAddressType = (GAPConnectionEventConnectedIndication_PeerAddressType_t)pPayload[idx]; idx++;
	FLib_MemCpy(evt->PeerAddress, pPayload + idx, 6); idx += 6;
	evt->peerRpaResolved = (bool_t)pPayload[idx]; idx++;


	evt->localRpaUsed = (bool_t)pPayload[idx]; idx++;



	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventPairingRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A pairing request has been received from the peer Master
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventPairingRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPairingRequestIndication_t *evt = &(container->Data.GAPConnectionEventPairingRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x479E;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPairingRequestIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventSlaveSecurityRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A Slave Security Request has been received from the peer Slave
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventSlaveSecurityRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventSlaveSecurityRequestIndication_t *evt = &(container->Data.GAPConnectionEventSlaveSecurityRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x479F;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventSlaveSecurityRequestIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventPairingResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A pairing response has been received from the peer Slave
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventPairingResponseIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPairingResponseIndication_t *evt = &(container->Data.GAPConnectionEventPairingResponseIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47A0;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPairingResponseIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventAuthenticationRejectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A link encryption or pairing request has been rejected by the peer Slave
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventAuthenticationRejectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventAuthenticationRejectedIndication_t *evt = &(container->Data.GAPConnectionEventAuthenticationRejectedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47A1;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventAuthenticationRejectedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventPasskeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Peer Slave has requested a passkey (maximum 6 digit PIN) for the pairing procedure
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventPasskeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPasskeyRequestIndication_t *evt = &(container->Data.GAPConnectionEventPasskeyRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47A2;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPasskeyRequestIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventOobRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Out-of-Band data must be provided for the pairing procedure
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventOobRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventOobRequestIndication_t *evt = &(container->Data.GAPConnectionEventOobRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47A3;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventOobRequestIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventPasskeyDisplayIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The pairing procedure requires this Slave to display the passkey for the Master's user
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventPasskeyDisplayIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPasskeyDisplayIndication_t *evt = &(container->Data.GAPConnectionEventPasskeyDisplayIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47A4;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPasskeyDisplayIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventKeyExchangeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The pairing procedure requires the SMP keys to be distributed to the peer
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventKeyExchangeRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventKeyExchangeRequestIndication_t *evt = &(container->Data.GAPConnectionEventKeyExchangeRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47A5;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventKeyExchangeRequestIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventKeysReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	SMP keys distributed by the peer during pairing have been received
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventKeysReceivedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventKeysReceivedIndication_t *evt = &(container->Data.GAPConnectionEventKeysReceivedIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x47A6;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->Keys.LtkIncluded = (bool_t)pPayload[idx]; idx++;

	if (evt->Keys.LtkIncluded)
	{
		evt->Keys.LtkInfo.LtkSize = pPayload[idx]; idx++;

		if (evt->Keys.LtkInfo.LtkSize > 0)
		{
			evt->Keys.LtkInfo.Ltk = MEM_BufferAlloc(evt->Keys.LtkInfo.LtkSize);

			if (!evt->Keys.LtkInfo.Ltk)
			{
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->Keys.LtkInfo.Ltk = NULL;
		}

		FLib_MemCpy(evt->Keys.LtkInfo.Ltk, pPayload + idx, evt->Keys.LtkInfo.LtkSize); idx += evt->Keys.LtkInfo.LtkSize;
	}

	evt->Keys.IrkIncluded = (bool_t)pPayload[idx]; idx++;


	evt->Keys.CsrkIncluded = (bool_t)pPayload[idx]; idx++;



	if (evt->Keys.LtkIncluded)
	{
		evt->Keys.RandEdivInfo.RandSize = pPayload[idx]; idx++;

		if (evt->Keys.RandEdivInfo.RandSize > 0)
		{
			evt->Keys.RandEdivInfo.Rand = MEM_BufferAlloc(evt->Keys.RandEdivInfo.RandSize);

			if (!evt->Keys.RandEdivInfo.Rand)
			{
				MEM_BufferFree(evt->Keys.LtkInfo.Ltk);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->Keys.RandEdivInfo.Rand = NULL;
		}

		FLib_MemCpy(evt->Keys.RandEdivInfo.Rand, pPayload + idx, evt->Keys.RandEdivInfo.RandSize); idx += evt->Keys.RandEdivInfo.RandSize;
		FLib_MemCpy(&(evt->Keys.RandEdivInfo.Ediv), pPayload + idx, sizeof(evt->Keys.RandEdivInfo.Ediv)); idx += sizeof(evt->Keys.RandEdivInfo.Ediv);
	}


	if (evt->Keys.IrkIncluded)
	{
		evt->Keys.AddressIncluded = (bool_t)pPayload[idx]; idx++;
	}



	if (evt->Keys.AddressIncluded)
	{
		evt->Keys.AddressInfo.DeviceAddressType = (GAPConnectionEventKeysReceivedIndication_Keys_AddressInfo_DeviceAddressType_t)pPayload[idx]; idx++;
		FLib_MemCpy(evt->Keys.AddressInfo.DeviceAddress, pPayload + idx, 6); idx += 6;
	}


	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventLongTermKeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The bonded peer Master has requested link encryption and the LTK must be provided
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventLongTermKeyRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventLongTermKeyRequestIndication_t *evt = &(container->Data.GAPConnectionEventLongTermKeyRequestIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x47A7;

	evt->DeviceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Ediv), pPayload + idx, sizeof(evt->Ediv)); idx += sizeof(evt->Ediv);
	evt->RandSize = pPayload[idx]; idx++;

	if (evt->RandSize > 0)
	{
		evt->Rand = MEM_BufferAlloc(evt->RandSize);

		if (!evt->Rand)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Rand = NULL;
	}

	FLib_MemCpy(evt->Rand, pPayload + idx, evt->RandSize); idx += evt->RandSize;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventEncryptionChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Link's encryption state has changed, e.g. during pairing or after a reconnection with a bonded peer
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventEncryptionChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventEncryptionChangedIndication_t *evt = &(container->Data.GAPConnectionEventEncryptionChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47A8;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventEncryptionChangedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventPairingCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Pairing procedure is complete, either successfully or with failure
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventPairingCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPairingCompleteIndication_t *evt = &(container->Data.GAPConnectionEventPairingCompleteIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x47A9;

	evt->DeviceId = pPayload[idx]; idx++;
	evt->PairingStatus = (GAPConnectionEventPairingCompleteIndication_PairingStatus_t)pPayload[idx]; idx++;

	switch (evt->PairingStatus)
	{
		case GAPConnectionEventPairingCompleteIndication_PairingStatus_PairingSuccessful:
			evt->PairingData.PairingSuccessful_WithBonding = (bool_t)pPayload[idx]; idx++;
			break;

		case GAPConnectionEventPairingCompleteIndication_PairingStatus_PairingFailed:
			FLib_MemCpy(&(evt->PairingData.PairingFailed_FailReason), pPayload + idx, 2); idx += 2;
			break;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventDisconnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A connection has been terminated
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventDisconnectedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventDisconnectedIndication_t *evt = &(container->Data.GAPConnectionEventDisconnectedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47AA;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventDisconnectedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventRssiReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	RSSI for an active connection has been read
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventRssiReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventRssiReadIndication_t *evt = &(container->Data.GAPConnectionEventRssiReadIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47AB;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventRssiReadIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventTxPowerLevelReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	TX power level for an active connection has been read
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventTxPowerLevelReadIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventTxPowerLevelReadIndication_t *evt = &(container->Data.GAPConnectionEventTxPowerLevelReadIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47AC;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventTxPowerLevelReadIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventPowerReadFailureIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Power reading could not be performed
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventPowerReadFailureIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventPowerReadFailureIndication_t *evt = &(container->Data.GAPConnectionEventPowerReadFailureIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47AD;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventPowerReadFailureIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventParameterUpdateRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	A connection parameter update request has been received
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventParameterUpdateRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventParameterUpdateRequestIndication_t *evt = &(container->Data.GAPConnectionEventParameterUpdateRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47AE;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventParameterUpdateRequestIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventParameterUpdateCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The connection has new parameters
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventParameterUpdateCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventParameterUpdateCompleteIndication_t *evt = &(container->Data.GAPConnectionEventParameterUpdateCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47AF;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventParameterUpdateCompleteIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventLeDataLengthChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The new TX/RX Data Length paramaters
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventLeDataLengthChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventLeDataLengthChangedIndication_t *evt = &(container->Data.GAPConnectionEventLeDataLengthChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47B0;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventLeDataLengthChangedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventLeScOobDataRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event sent to request LE SC OOB Data (r, Cr and Addr) received from a peer
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventLeScOobDataRequestIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventLeScOobDataRequestIndication_t *evt = &(container->Data.GAPConnectionEventLeScOobDataRequestIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47B1;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventLeScOobDataRequestIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventLeScDisplayNumericValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Event sent to display and confirm a Numeric Comparison Value when using the LE SC Numeric Comparison pairing method
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventLeScDisplayNumericValueIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventLeScDisplayNumericValueIndication_t *evt = &(container->Data.GAPConnectionEventLeScDisplayNumericValueIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47B2;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventLeScDisplayNumericValueIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPConnectionEventLeScKeypressNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Remote Keypress Notification recieved during Passkey Entry Pairing Method
***************************************************************************************************/
static memStatus_t Load_GAPConnectionEventLeScKeypressNotificationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPConnectionEventLeScKeypressNotificationIndication_t *evt = &(container->Data.GAPConnectionEventLeScKeypressNotificationIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47B3;

	FLib_MemCpy(evt, pPayload, sizeof(GAPConnectionEventLeScKeypressNotificationIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventControllerResetCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The controller has been reset
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventControllerResetCompleteIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventControllerResetCompleteIndication_t *evt = &(container->Data.GAPGenericEventControllerResetCompleteIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47B4;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventControllerResetCompleteIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPLeScPublicKeyRegeneratedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The private/public key pair used for LE Secure Connections pairing has been regenerated
***************************************************************************************************/
static memStatus_t Load_GAPLeScPublicKeyRegeneratedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPLeScPublicKeyRegeneratedIndication_t *evt = &(container->Data.GAPLeScPublicKeyRegeneratedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47B5;

	FLib_MemCpy(evt, pPayload, sizeof(GAPLeScPublicKeyRegeneratedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventLeScLocalOobDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Local OOB data used for LE Secure Connections pairing
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventLeScLocalOobDataIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventLeScLocalOobDataIndication_t *evt = &(container->Data.GAPGenericEventLeScLocalOobDataIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47B6;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventLeScLocalOobDataIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGenericEventControllerPrivacyStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	The Controller Privacy was enabled or disabled
***************************************************************************************************/
static memStatus_t Load_GAPGenericEventControllerPrivacyStateChangedIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGenericEventControllerPrivacyStateChangedIndication_t *evt = &(container->Data.GAPGenericEventControllerPrivacyStateChangedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0x47B7;

	FLib_MemCpy(evt, pPayload, sizeof(GAPGenericEventControllerPrivacyStateChangedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_GAPGetBondedDevicesIdentityInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
\brief	Returns a list of the identity information of bonded devices
***************************************************************************************************/
static memStatus_t Load_GAPGetBondedDevicesIdentityInformationIndication(bleEvtContainer_t *container, uint8_t *pPayload)
{
	GAPGetBondedDevicesIdentityInformationIndication_t *evt = &(container->Data.GAPGetBondedDevicesIdentityInformationIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x4783;

	evt->NbOfDeviceIdentityAddresses = pPayload[idx]; idx++;

	if (evt->NbOfDeviceIdentityAddresses > 0)
	{
		evt->IdentityAddresses = MEM_BufferAlloc(evt->NbOfDeviceIdentityAddresses * sizeof(evt->IdentityAddresses[0]));

		if (!evt->IdentityAddresses)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->IdentityAddresses = NULL;
	}


	for (uint32_t i = 0; i < evt->NbOfDeviceIdentityAddresses; i++)
	{
		evt->IdentityAddresses[i].IdentityAddressType = (GAPGetBondedDevicesIdentityInformationIndication_IdentityAddresses_IdentityAddressType_t)pPayload[idx]; idx++;
		FLib_MemCpy(evt->IdentityAddresses[i].IdentityAddress, pPayload + idx, 6); idx += 6;
		FLib_MemCpy(evt->IdentityAddresses[i].Irk, pPayload + idx, 16); idx += 16;
	}

	return MEM_SUCCESS_c;
}

#endif  /* GAP_ENABLE */


void KHC_BLE_RX_MsgHandler(void *pData, void *param, uint32_t fsciInterface)
{
	if (!pData || !param)
	{
		return;
	}

	bleEvtContainer_t *container = (bleEvtContainer_t *)param;
	uint8_t og = *((uint8_t *)pData + 1);
	uint8_t oc = *((uint8_t *)pData + 2);
	uint8_t dataSize = *((uint8_t *)pData + 3) + (*((uint8_t *)pData + 4) << 8);
	uint8_t *pPayload = (uint8_t *)pData + 5;
	uint16_t id = (og << 8) + oc, i;

	if (dataSize == 0)
	{
		return;
	}

	for (i = 0; i < sizeof(evtHandlerTbl) / sizeof(evtHandlerTbl[0]); i++)
	{
		if (evtHandlerTbl[i].id == id)
		{
                    //LOGD(">>[blEvent:%x]\r\n", id);
                    evtHandlerTbl[i].handlerFunc(container, pPayload);
                    //LOGD("<<[blEvent:%x]\r\n", id);
			break;
		}
	}
}

/*!*************************************************************************************************
   INCLUDES AND EXTERN VARIABLES FOR GENERATED PRINT EVENT HANDLER
***************************************************************************************************/
#include "shell_utils.h"
#include "shell_ble.h"

extern bool_t          gSuppressBleEventPrint;
extern deviceId_t      gPeerDeviceId;
extern uint16_t        gLatestHandle;
extern uint16_t        gCccdHandle;
extern bool_t          gIsNotificationActive;
extern hrsDemoState_t  gHrsDemoState;
extern otapState_t     gOtapState;
extern wuartState_t    gWUartState;
extern uint8_t face_PeerAddress_Bond[6];
extern uint8_t face_PeerAdd_type;
extern uint16_t ltksend;

/*!*************************************************************************************************
   START OF GENERATED PRINT EVENT HANDLER
***************************************************************************************************/
static char *const gFsciSuccess_c = "gFsciSuccess_c";
static char *const gFsciSAPDisabled_c = "gFsciSAPDisabled_c";
static char *const gFsciAppMsgTooBig_c = "gFsciAppMsgTooBig_c";
static char *const gFsciOutOfMessages_c = "gFsciOutOfMessages_c";
static char *const gFsciUnknownOpcodeGroup_c = "gFsciUnknownOpcodeGroup_c";
static char *const gFsciOpcodeGroupIsDisabled_c = "gFsciOpcodeGroupIsDisabled_c";
static char *const gFsciUnknownOpcode_c = "gFsciUnknownOpcode_c";
static char *const gFsciTooBig_c = "gFsciTooBig_c";
static char *const gFsciError_c = "gFsciError_c";
static char *const gSuccess = "gSuccess";
static char *const gError = "gError";
static char *const gBleSuccess_c = "gBleSuccess_c";
static char *const gBleInvalidParameter_c = "gBleInvalidParameter_c";
static char *const gBleOverflow_c = "gBleOverflow_c";
static char *const gBleUnavailable_c = "gBleUnavailable_c";
static char *const gBleFeatureNotSupported_c = "gBleFeatureNotSupported_c";
static char *const gBleOutOfMemory_c = "gBleOutOfMemory_c";
static char *const gBleAlreadyInitialized_c = "gBleAlreadyInitialized_c";
static char *const gBleOsError_c = "gBleOsError_c";
static char *const gBleUnexpectedError_c = "gBleUnexpectedError_c";
static char *const gBleInvalidState_c = "gBleInvalidState_c";
static char *const gHciUnknownHciCommand_c = "gHciUnknownHciCommand_c";
static char *const gHciUnknownConnectionIdentifier_c = "gHciUnknownConnectionIdentifier_c";
static char *const gHciHardwareFailure_c = "gHciHardwareFailure_c";
static char *const gHciPageTimeout_c = "gHciPageTimeout_c";
static char *const gHciAuthenticationFailure_c = "gHciAuthenticationFailure_c";
static char *const gHciPinOrKeyMissing_c = "gHciPinOrKeyMissing_c";
static char *const gHciMemoryCapacityExceeded_c = "gHciMemoryCapacityExceeded_c";
static char *const gHciConnectionTimeout_c = "gHciConnectionTimeout_c";
static char *const gHciConnectionLimitExceeded_c = "gHciConnectionLimitExceeded_c";
static char *const gHciSynchronousConnectionLimitToADeviceExceeded_c = "gHciSynchronousConnectionLimitToADeviceExceeded_c";
static char *const gHciAclConnectionAlreadyExists_c = "gHciAclConnectionAlreadyExists_c";
static char *const gHciCommandDisallowed_c = "gHciCommandDisallowed_c";
static char *const gHciConnectionRejectedDueToLimitedResources_c = "gHciConnectionRejectedDueToLimitedResources_c";
static char *const gHciConnectionRejectedDueToSecurityReasons_c = "gHciConnectionRejectedDueToSecurityReasons_c";
static char *const gHciConnectionRejectedDueToUnacceptableBdAddr_c = "gHciConnectionRejectedDueToUnacceptableBdAddr_c";
static char *const gHciConnectionAcceptTimeoutExceeded_c = "gHciConnectionAcceptTimeoutExceeded_c";
static char *const gHciUnsupportedFeatureOrParameterValue_c = "gHciUnsupportedFeatureOrParameterValue_c";
static char *const gHciInvalidHciCommandParameters_c = "gHciInvalidHciCommandParameters_c";
static char *const gHciRemoteUserTerminatedConnection_c = "gHciRemoteUserTerminatedConnection_c";
static char *const gHciRemoteDeviceTerminatedConnectionLowResources_c = "gHciRemoteDeviceTerminatedConnectionLowResources_c";
static char *const gHciRemoteDeviceTerminatedConnectionPowerOff_c = "gHciRemoteDeviceTerminatedConnectionPowerOff_c";
static char *const gHciConnectionTerminatedByLocalHost_c = "gHciConnectionTerminatedByLocalHost_c";
static char *const gHciRepeatedAttempts_c = "gHciRepeatedAttempts_c";
static char *const gHciPairingNotAllowed_c = "gHciPairingNotAllowed_c";
static char *const gHciUnknownLpmPdu_c = "gHciUnknownLpmPdu_c";
static char *const gHciUnsupportedRemoteFeature_c = "gHciUnsupportedRemoteFeature_c";
static char *const gHciScoOffsetRejected_c = "gHciScoOffsetRejected_c";
static char *const gHciScoIntervalRejected_c = "gHciScoIntervalRejected_c";
static char *const gHciScoAirModeRejected_c = "gHciScoAirModeRejected_c";
static char *const gHciInvalidLpmParameters_c = "gHciInvalidLpmParameters_c";
static char *const gHciUnspecifiedError_c = "gHciUnspecifiedError_c";
static char *const gHciUnsupportedLpmParameterValue_c = "gHciUnsupportedLpmParameterValue_c";
static char *const gHciRoleChangeNotAllowed_c = "gHciRoleChangeNotAllowed_c";
static char *const gHciLLResponseTimeout_c = "gHciLLResponseTimeout_c";
static char *const gHciLmpErrorTransactionCollision_c = "gHciLmpErrorTransactionCollision_c";
static char *const gHciLmpPduNotAllowed_c = "gHciLmpPduNotAllowed_c";
static char *const gHciEncryptionModeNotAcceptable_c = "gHciEncryptionModeNotAcceptable_c";
static char *const gHciLinkKeyCannotBeChanged_c = "gHciLinkKeyCannotBeChanged_c";
static char *const gHciRequestedQosNotSupported_c = "gHciRequestedQosNotSupported_c";
static char *const gHciInstantPassed_c = "gHciInstantPassed_c";
static char *const gHciPairingWithUnitKeyNotSupported_c = "gHciPairingWithUnitKeyNotSupported_c";
static char *const gHciDifferentTransactionCollision_c = "gHciDifferentTransactionCollision_c";
static char *const gHciReserved_0x2B_c = "gHciReserved_0x2B_c";
static char *const gHciQosNotAcceptableParameter_c = "gHciQosNotAcceptableParameter_c";
static char *const gHciQosRejected_c = "gHciQosRejected_c";
static char *const gHciChannelClassificationNotSupported_c = "gHciChannelClassificationNotSupported_c";
static char *const gHciInsufficientSecurity_c = "gHciInsufficientSecurity_c";
static char *const gHciParameterOutOfMandatoryRange_c = "gHciParameterOutOfMandatoryRange_c";
static char *const gHciReserved_0x31_c = "gHciReserved_0x31_c";
static char *const gHciRoleSwitchPending_c = "gHciRoleSwitchPending_c";
static char *const gHciReserved_0x33_c = "gHciReserved_0x33_c";
static char *const gHciReservedSlotViolation_c = "gHciReservedSlotViolation_c";
static char *const gHciRoleSwitchFailed_c = "gHciRoleSwitchFailed_c";
static char *const gHciExtendedInquiryResponseTooLarge_c = "gHciExtendedInquiryResponseTooLarge_c";
static char *const gHciSecureSimplePairingNotSupportedByHost_c = "gHciSecureSimplePairingNotSupportedByHost_c";
static char *const gHciHostBusyPairing_c = "gHciHostBusyPairing_c";
static char *const gHciConnectionRejectedDueToNoSuitableChannelFound_c = "gHciConnectionRejectedDueToNoSuitableChannelFound_c";
static char *const gHciControllerBusy_c = "gHciControllerBusy_c";
static char *const gHciUnacceptableConnectionParameters_c = "gHciUnacceptableConnectionParameters_c";
static char *const gHciDirectedAdvertisingTimeout_c = "gHciDirectedAdvertisingTimeout_c";
static char *const gHciConnectionTerminatedDueToMicFailure_c = "gHciConnectionTerminatedDueToMicFailure_c";
static char *const gHciConnectionFailedToBeEstablished_c = "gHciConnectionFailedToBeEstablished_c";
static char *const gHciMacConnectionFailed_c = "gHciMacConnectionFailed_c";
static char *const gHciCoarseClockAdjustmentRejected_c = "gHciCoarseClockAdjustmentRejected_c";
static char *const gHciAlreadyInit_c = "gHciAlreadyInit_c";
static char *const gHciInvalidParameter_c = "gHciInvalidParameter_c";
static char *const gHciCallbackNotInstalled_c = "gHciCallbackNotInstalled_c";
static char *const gHciCallbackAlreadyInstalled_c = "gHciCallbackAlreadyInstalled_c";
static char *const gHciCommandNotSupported_c = "gHciCommandNotSupported_c";
static char *const gHciEventNotSupported_c = "gHciEventNotSupported_c";
static char *const gHciTransportError_c = "gHciTransportError_c";
static char *const gL2caAlreadyInit_c = "gL2caAlreadyInit_c";
static char *const gL2caInsufficientResources_c = "gL2caInsufficientResources_c";
static char *const gL2caCallbackNotInstalled_c = "gL2caCallbackNotInstalled_c";
static char *const gL2caCallbackAlreadyInstalled_c = "gL2caCallbackAlreadyInstalled_c";
static char *const gL2caLePsmInvalid_c = "gL2caLePsmInvalid_c";
static char *const gL2caLePsmAlreadyRegistered_c = "gL2caLePsmAlreadyRegistered_c";
static char *const gL2caLePsmNotRegistered_c = "gL2caLePsmNotRegistered_c";
static char *const gL2caLePsmInsufficientResources_c = "gL2caLePsmInsufficientResources_c";
static char *const gL2caChannelInvalid_c = "gL2caChannelInvalid_c";
static char *const gL2caChannelClosed_c = "gL2caChannelClosed_c";
static char *const gL2caChannelAlreadyConnected_c = "gL2caChannelAlreadyConnected_c";
static char *const gL2caConnectionParametersRejected_c = "gL2caConnectionParametersRejected_c";
static char *const gL2caChannelBusy_c = "gL2caChannelBusy_c";
static char *const gL2caInvalidParameter_c = "gL2caInvalidParameter_c";
static char *const gL2caError_c = "gL2caError_c";
static char *const gSmNullCBFunction_c = "gSmNullCBFunction_c";
static char *const gSmCommandNotSupported_c = "gSmCommandNotSupported_c";
static char *const gSmUnexpectedCommand_c = "gSmUnexpectedCommand_c";
static char *const gSmInvalidCommandCode_c = "gSmInvalidCommandCode_c";
static char *const gSmInvalidCommandLength_c = "gSmInvalidCommandLength_c";
static char *const gSmInvalidCommandParameter_c = "gSmInvalidCommandParameter_c";
static char *const gSmInvalidDeviceId_c = "gSmInvalidDeviceId_c";
static char *const gSmInvalidInternalOperation_c = "gSmInvalidInternalOperation_c";
static char *const gSmInvalidConnectionHandle_c = "gSmInvalidConnectionHandle_c";
static char *const gSmInproperKeyDistributionField_c = "gSmInproperKeyDistributionField_c";
static char *const gSmUnexpectedKeyType_c = "gSmUnexpectedKeyType_c";
static char *const gSmUnexpectedPairingTerminationReason_c = "gSmUnexpectedPairingTerminationReason_c";
static char *const gSmUnexpectedKeyset_c = "gSmUnexpectedKeyset_c";
static char *const gSmSmpTimeoutOccurred_c = "gSmSmpTimeoutOccurred_c";
static char *const gSmUnknownSmpPacketType_c = "gSmUnknownSmpPacketType_c";
static char *const gSmInvalidSmpPacketLength_c = "gSmInvalidSmpPacketLength_c";
static char *const gSmInvalidSmpPacketParameter_c = "gSmInvalidSmpPacketParameter_c";
static char *const gSmReceivedUnexpectedSmpPacket_c = "gSmReceivedUnexpectedSmpPacket_c";
static char *const gSmReceivedSmpPacketFromUnknownDevice_c = "gSmReceivedSmpPacketFromUnknownDevice_c";
static char *const gSmReceivedUnexpectedHciEvent_c = "gSmReceivedUnexpectedHciEvent_c";
static char *const gSmReceivedHciEventFromUnknownDevice_c = "gSmReceivedHciEventFromUnknownDevice_c";
static char *const gSmInvalidHciEventParameter_c = "gSmInvalidHciEventParameter_c";
static char *const gSmLlConnectionEncryptionInProgress_c = "gSmLlConnectionEncryptionInProgress_c";
static char *const gSmLlConnectionEncryptionFailure_c = "gSmLlConnectionEncryptionFailure_c";
static char *const gSmInsufficientResources_c = "gSmInsufficientResources_c";
static char *const gSmOobDataAddressMismatch_c = "gSmOobDataAddressMismatch_c";
// static char *const gSmSmpPacketReceivedAfterTimeoutOccurred_c = "gSmSmpPacketReceivedAfterTimeoutOccurred_c";
static char *const gSmPairingErrorPasskeyEntryFailed_c = "gSmPairingErrorPasskeyEntryFailed_c";
static char *const gSmPairingErrorConfirmValueFailed_c = "gSmPairingErrorConfirmValueFailed_c";
static char *const gSmPairingErrorCommandNotSupported_c = "gSmPairingErrorCommandNotSupported_c";
static char *const gSmPairingErrorInvalidParameters_c = "gSmPairingErrorInvalidParameters_c";
static char *const gSmPairingErrorUnknownReason_c = "gSmPairingErrorUnknownReason_c";
static char *const gSmTbResolvableAddressDoesNotMatchIrk_c = "gSmTbResolvableAddressDoesNotMatchIrk_c";
static char *const gSmTbInvalidDataSignature_c = "gSmTbInvalidDataSignature_c";
static char *const gAttInvalidHandle_c = "gAttInvalidHandle_c";
static char *const gAttReadNotPermitted_c = "gAttReadNotPermitted_c";
static char *const gAttWriteNotPermitted_c = "gAttWriteNotPermitted_c";
static char *const gAttInvalidPdu_c = "gAttInvalidPdu_c";
static char *const gAttInsufficientAuthentication_c = "gAttInsufficientAuthentication_c";
static char *const gAttRequestNotSupported_c = "gAttRequestNotSupported_c";
static char *const gAttInvalidOffset_c = "gAttInvalidOffset_c";
static char *const gAttInsufficientAuthorization_c = "gAttInsufficientAuthorization_c";
static char *const gAttPrepareQueueFull_c = "gAttPrepareQueueFull_c";
static char *const gAttAttributeNotFound_c = "gAttAttributeNotFound_c";
static char *const gAttAttributeNotLong_c = "gAttAttributeNotLong_c";
static char *const gAttInsufficientEncryptionKeySize_c = "gAttInsufficientEncryptionKeySize_c";
static char *const gAttInvalidAttributeValueLength_c = "gAttInvalidAttributeValueLength_c";
static char *const gAttUnlikelyor_c = "gAttUnlikelyor_c";
static char *const gAttInsufficientEncryption_c = "gAttInsufficientEncryption_c";
static char *const gAttUnsupportedGroupType_c = "gAttUnsupportedGroupType_c";
static char *const gAttInsufficientResources_c = "gAttInsufficientResources_c";
static char *const gGattAnotherProcedureInProgress_c = "gGattAnotherProcedureInProgress_c";
static char *const gGattLongAttributePacketsCorrupted_c = "gGattLongAttributePacketsCorrupted_c";
static char *const gGattMultipleAttributesOverflow_c = "gGattMultipleAttributesOverflow_c";
static char *const gGattUnexpectedReadMultipleResponseLength_c = "gGattUnexpectedReadMultipleResponseLength_c";
static char *const gGattInvalidValueLength_c = "gGattInvalidValueLength_c";
static char *const gGattServerTimeout_c = "gGattServerTimeout_c";
static char *const gGattIndicationAlreadyInProgress_c = "gGattIndicationAlreadyInProgress_c";
static char *const gGattClientConfirmationTimeout_c = "gGattClientConfirmationTimeout_c";
static char *const gGapAdvDataTooLong_c = "gGapAdvDataTooLong_c";
static char *const gGapScanRspDataTooLong_c = "gGapScanRspDataTooLong_c";
static char *const gGapDeviceNotBonded_c = "gGapDeviceNotBonded_c";
static char *const gDevDbCccdLimitReached_c = "gDevDbCccdLimitReached_c";
static char *const gDevDbCccdNotFound_c = "gDevDbCccdNotFound_c";
static char *const gGattDbInvalidHandle_c = "gGattDbInvalidHandle_c";
static char *const gGattDbCharacteristicNotFound_c = "gGattDbCharacteristicNotFound_c";
static char *const gGattDbCccdNotFound_c = "gGattDbCccdNotFound_c";
static char *const gGattDbServiceNotFound_c = "gGattDbServiceNotFound_c";
static char *const gGattDbDescriptorNotFound_c = "gGattDbDescriptorNotFound_c";
static char *const gGattProcSuccess_c = "gGattProcSuccess_c";
static char *const gProcedureError_c = "gProcedureError_c";
static char *const gBleOsc = "gBleOsc";
static char *const gBleUnexpectedc = "gBleUnexpectedc";
static char *const gHciUnspecifiedc = "gHciUnspecifiedc";
static char *const gHciTransportc = "gHciTransportc";
static char *const gL2cac = "gL2cac";
static char *const gHciCommandStatus_c = "gHciCommandStatus_c";
static char *const gCheckPrivateResolvableAddress_c = "gCheckPrivateResolvableAddress_c";
static char *const gVerifySignature_c = "gVerifySignature_c";
static char *const gAddNewConnection_c = "gAddNewConnection_c";
static char *const gResetController_c = "gResetController_c";
static char *const gSetEventMask_c = "gSetEventMask_c";
static char *const gReadLeBufferSize_c = "gReadLeBufferSize_c";
static char *const gSetLeEventMask_c = "gSetLeEventMask_c";
static char *const gReadDeviceAddress_c = "gReadDeviceAddress_c";
static char *const gReadLocalSupportedFeatures_c = "gReadLocalSupportedFeatures_c";
static char *const gReadWhiteListSize_c = "gReadWhiteListSize_c";
static char *const gClearWhiteList_c = "gClearWhiteList_c";
static char *const gAddDeviceToWhiteList_c = "gAddDeviceToWhiteList_c";
static char *const gRemoveDeviceFromWhiteList_c = "gRemoveDeviceFromWhiteList_c";
static char *const gCancelCreateConnection_c = "gCancelCreateConnection_c";
static char *const gReadRadioPower_c = "gReadRadioPower_c";
static char *const gSetRandomAddress_c = "gSetRandomAddress_c";
static char *const gCreateRandomAddress_c = "gCreateRandomAddress_c";
static char *const gEncryptLink_c = "gEncryptLink_c";
static char *const gProvideLongTermKey_c = "gProvideLongTermKey_c";
static char *const gDenyLongTermKey_c = "gDenyLongTermKey_c";
static char *const gConnect_c = "gConnect_c";
static char *const gDisconnect_c = "gDisconnect_c";
static char *const gTerminatePairing_c = "gTerminatePairing_c";
static char *const gSendSlaveSecurityRequest_c = "gSendSlaveSecurityRequest_c";
static char *const gEnterPasskey_c = "gEnterPasskey_c";
static char *const gProvideOob_c = "gProvideOob_c";
static char *const gSendSmpKeys_c = "gSendSmpKeys_c";
static char *const gWriteSuggestedDefaultDataLength_c = "gWriteSuggestedDefaultDataLength_c";
static char *const gReadSuggestedDefaultDataLength_c = "gReadSuggestedDefaultDataLength_c";
static char *const gUpdateLeDataLength_c = "gUpdateLeDataLength_c";
static char *const gEnableControllerPrivacy_c = "gEnableControllerPrivacy_c";
static char *const gLeScSendKeypressNotification_c = "gLeScSendKeypressNotification_c";
static char *const gLeScSetPeerOobData_c = "gLeScSetPeerOobData_c";
static char *const gLeScGetLocalOobData_c = "gLeScGetLocalOobData_c";
static char *const gLeScValidateNumericValue_c = "gLeScValidateNumericValue_c";
static char *const gLeScRegeneratePublicKey_c = "gLeScRegeneratePublicKey_c";
static char *const gLeSetResolvablePrivateAddressTimeout_c = "gLeSetResolvablePrivateAddressTimeout_c";
static char *const gPublic_c = "gPublic_c";
static char *const gRandom_c = "gRandom_c";
static char *const gBleAdvRepAdvInd_c = "gBleAdvRepAdvInd_c";
static char *const gBleAdvRepAdvDirectInd_c = "gBleAdvRepAdvDirectInd_c";
static char *const gBleAdvRepAdvScanInd_c = "gBleAdvRepAdvScanInd_c";
static char *const gBleAdvRepAdvNonconnInd_c = "gBleAdvRepAdvNonconnInd_c";
static char *const gBleAdvRepScanRsp_c = "gBleAdvRepScanRsp_c";
static char *const gOobNotAvailable_c = "gOobNotAvailable_c";
static char *const gIncompatibleIoCapabilities_c = "gIncompatibleIoCapabilities_c";
static char *const gPairingNotSupported_c = "gPairingNotSupported_c";
static char *const gLowEncryptionKeySize_c = "gLowEncryptionKeySize_c";
static char *const gUnspecifiedReason_c = "gUnspecifiedReason_c";
static char *const gRepeatedAttempts_c = "gRepeatedAttempts_c";
static char *const gLinkEncryptionFailed_c = "gLinkEncryptionFailed_c";
static char *const gNoKeys_c = "gNoKeys_c";
static char *const gLtk_c = "gLtk_c";
static char *const gIrk_c = "gIrk_c";
static char *const gCsrk_c = "gCsrk_c";
static char *const PairingSuccessful = "PairingSuccessful";
static char *const PairingFailed = "PairingFailed";
static char *const gSmUnexpectedPairingTerminationc = "gSmUnexpectedPairingTerminationc";
static char *const gSmPairingErrorUnknownc = "gSmPairingErrorUnknownc";

/*!*************************************************************************************************
\fn     void SHELL_BleEventNotify(void *param)
\brief  Print to host's shell information about BLE events coming from black-box

\param  [in]    param       the received event
***************************************************************************************************/
void SHELL_BleEventNotify(void *param)
{
    bleEvtContainer_t *container = (bleEvtContainer_t *)param;

    if (!gSuppressBleEventPrint)
    {
        SHELL_NEWLINE();
    }

    switch (container->id)
    {

#if FSCI_ENABLE

        case 0xA4FE:
            shell_write("FSCIErrorIndication");
            shell_write(" -> ");

            switch (container->Data.FSCIErrorIndication.Status)
            {
                case FSCIErrorIndication_Status_gFsciSuccess_c:
                    shell_write(gFsciSuccess_c);
                    break;

                case FSCIErrorIndication_Status_gFsciSAPDisabled_c:
                    shell_write(gFsciSAPDisabled_c);
                    break;

                case FSCIErrorIndication_Status_gFsciAppMsgTooBig_c:
                    shell_write(gFsciAppMsgTooBig_c);
                    break;

                case FSCIErrorIndication_Status_gFsciOutOfMessages_c:
                    shell_write(gFsciOutOfMessages_c);
                    break;

                case FSCIErrorIndication_Status_gFsciUnknownOpcodeGroup_c:
                    shell_write(gFsciUnknownOpcodeGroup_c);
                    break;

                case FSCIErrorIndication_Status_gFsciOpcodeGroupIsDisabled_c:
                    shell_write(gFsciOpcodeGroupIsDisabled_c);
                    break;

                case FSCIErrorIndication_Status_gFsciUnknownOpcode_c:
                    shell_write(gFsciUnknownOpcode_c);
                    break;

                case FSCIErrorIndication_Status_gFsciTooBig_c:
                    shell_write(gFsciTooBig_c);
                    break;

                case FSCIErrorIndication_Status_gFsciError_c:
                    shell_write(gFsciError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.FSCIErrorIndication.Status);
                    break;
            }

            break;

        case 0xA470:
            shell_write("FSCIAllowDeviceToSleepConfirm");
            shell_write(" -> ");

            switch (container->Data.FSCIAllowDeviceToSleepConfirm.Status)
            {
                case FSCIAllowDeviceToSleepConfirm_Status_gSuccess:
                    shell_write(gSuccess);
                    break;

                case FSCIAllowDeviceToSleepConfirm_Status_gError:
                    shell_write(gError);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.FSCIAllowDeviceToSleepConfirm.Status);
                    break;
            }

            break;

        case 0xA471:
            shell_write("FSCIWakeUpIndication");
            break;

        case 0xA472:
            shell_write("FSCIGetWakeupReasonResponse");
            break;

#endif  /* FSCI_ENABLE */

        case 0x4180:

            if (gSuppressBleEventPrint)
            {
                break;
            }

#if L2CAP_ENABLE

            shell_write("L2CAPConfirm");
            shell_write(" -> ");

            switch (container->Data.L2CAPConfirm.Status)
            {
                case L2CAPConfirm_Status_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case L2CAPConfirm_Status_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case L2CAPConfirm_Status_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case L2CAPConfirm_Status_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case L2CAPConfirm_Status_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case L2CAPConfirm_Status_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case L2CAPConfirm_Status_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case L2CAPConfirm_Status_gBleOsError_c:
                    shell_write(gBleOsError_c);
                    break;

                case L2CAPConfirm_Status_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedError_c);
                    break;

                case L2CAPConfirm_Status_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case L2CAPConfirm_Status_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case L2CAPConfirm_Status_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case L2CAPConfirm_Status_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case L2CAPConfirm_Status_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case L2CAPConfirm_Status_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case L2CAPConfirm_Status_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case L2CAPConfirm_Status_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case L2CAPConfirm_Status_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case L2CAPConfirm_Status_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case L2CAPConfirm_Status_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case L2CAPConfirm_Status_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case L2CAPConfirm_Status_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case L2CAPConfirm_Status_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case L2CAPConfirm_Status_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case L2CAPConfirm_Status_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case L2CAPConfirm_Status_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case L2CAPConfirm_Status_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case L2CAPConfirm_Status_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case L2CAPConfirm_Status_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case L2CAPConfirm_Status_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case L2CAPConfirm_Status_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case L2CAPConfirm_Status_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case L2CAPConfirm_Status_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case L2CAPConfirm_Status_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case L2CAPConfirm_Status_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case L2CAPConfirm_Status_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case L2CAPConfirm_Status_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case L2CAPConfirm_Status_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case L2CAPConfirm_Status_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case L2CAPConfirm_Status_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case L2CAPConfirm_Status_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedError_c);
                    break;

                case L2CAPConfirm_Status_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case L2CAPConfirm_Status_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case L2CAPConfirm_Status_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case L2CAPConfirm_Status_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case L2CAPConfirm_Status_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case L2CAPConfirm_Status_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case L2CAPConfirm_Status_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case L2CAPConfirm_Status_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case L2CAPConfirm_Status_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case L2CAPConfirm_Status_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case L2CAPConfirm_Status_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case L2CAPConfirm_Status_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case L2CAPConfirm_Status_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case L2CAPConfirm_Status_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case L2CAPConfirm_Status_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case L2CAPConfirm_Status_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case L2CAPConfirm_Status_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case L2CAPConfirm_Status_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case L2CAPConfirm_Status_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case L2CAPConfirm_Status_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case L2CAPConfirm_Status_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case L2CAPConfirm_Status_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case L2CAPConfirm_Status_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case L2CAPConfirm_Status_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case L2CAPConfirm_Status_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case L2CAPConfirm_Status_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case L2CAPConfirm_Status_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case L2CAPConfirm_Status_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case L2CAPConfirm_Status_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case L2CAPConfirm_Status_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case L2CAPConfirm_Status_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case L2CAPConfirm_Status_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case L2CAPConfirm_Status_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case L2CAPConfirm_Status_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case L2CAPConfirm_Status_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case L2CAPConfirm_Status_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case L2CAPConfirm_Status_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case L2CAPConfirm_Status_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case L2CAPConfirm_Status_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case L2CAPConfirm_Status_gHciTransportError_c:
                    shell_write(gHciTransportError_c);
                    break;

                case L2CAPConfirm_Status_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case L2CAPConfirm_Status_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case L2CAPConfirm_Status_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case L2CAPConfirm_Status_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case L2CAPConfirm_Status_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case L2CAPConfirm_Status_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case L2CAPConfirm_Status_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case L2CAPConfirm_Status_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case L2CAPConfirm_Status_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case L2CAPConfirm_Status_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case L2CAPConfirm_Status_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case L2CAPConfirm_Status_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case L2CAPConfirm_Status_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case L2CAPConfirm_Status_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case L2CAPConfirm_Status_gL2caError_c:
                    shell_write(gL2caError_c);
                    break;

                case L2CAPConfirm_Status_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case L2CAPConfirm_Status_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case L2CAPConfirm_Status_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case L2CAPConfirm_Status_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case L2CAPConfirm_Status_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case L2CAPConfirm_Status_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case L2CAPConfirm_Status_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case L2CAPConfirm_Status_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case L2CAPConfirm_Status_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case L2CAPConfirm_Status_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case L2CAPConfirm_Status_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case L2CAPConfirm_Status_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case L2CAPConfirm_Status_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case L2CAPConfirm_Status_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case L2CAPConfirm_Status_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case L2CAPConfirm_Status_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case L2CAPConfirm_Status_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case L2CAPConfirm_Status_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case L2CAPConfirm_Status_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case L2CAPConfirm_Status_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case L2CAPConfirm_Status_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case L2CAPConfirm_Status_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case L2CAPConfirm_Status_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case L2CAPConfirm_Status_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case L2CAPConfirm_Status_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case L2CAPConfirm_Status_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case L2CAPConfirm_Status_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case L2CAPConfirm_Status_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case L2CAPConfirm_Status_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case L2CAPConfirm_Status_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case L2CAPConfirm_Status_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case L2CAPConfirm_Status_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case L2CAPConfirm_Status_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case L2CAPConfirm_Status_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case L2CAPConfirm_Status_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case L2CAPConfirm_Status_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case L2CAPConfirm_Status_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case L2CAPConfirm_Status_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case L2CAPConfirm_Status_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case L2CAPConfirm_Status_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case L2CAPConfirm_Status_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case L2CAPConfirm_Status_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case L2CAPConfirm_Status_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case L2CAPConfirm_Status_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case L2CAPConfirm_Status_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case L2CAPConfirm_Status_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case L2CAPConfirm_Status_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case L2CAPConfirm_Status_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case L2CAPConfirm_Status_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case L2CAPConfirm_Status_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case L2CAPConfirm_Status_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case L2CAPConfirm_Status_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case L2CAPConfirm_Status_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case L2CAPConfirm_Status_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case L2CAPConfirm_Status_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case L2CAPConfirm_Status_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case L2CAPConfirm_Status_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case L2CAPConfirm_Status_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case L2CAPConfirm_Status_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case L2CAPConfirm_Status_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case L2CAPConfirm_Status_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case L2CAPConfirm_Status_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case L2CAPConfirm_Status_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case L2CAPConfirm_Status_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case L2CAPConfirm_Status_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case L2CAPConfirm_Status_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case L2CAPConfirm_Status_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case L2CAPConfirm_Status_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case L2CAPConfirm_Status_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.L2CAPConfirm.Status);
                    break;
            }

            break;

        case 0x4183:
            shell_write("L2CAPLePsmConnectionRequestIndication");
            break;

        case 0x4184:
            shell_write("L2CAPLePsmConnectionCompleteIndication");
            break;

        case 0x4185:
            shell_write("L2CAPLePsmDisconnectNotificationIndication");
            break;

        case 0x4186:
            shell_write("L2CAPNoPeerCreditsIndication");
            break;

        case 0x4187:
            shell_write("L2CAPLocalCreditsNotificationIndication");
            break;

        case 0x4188:
            shell_write("L2CAPLeCbDataIndication");
            break;

#endif  /* L2CAP_ENABLE */

#if GATT_ENABLE

        case 0x4480:

            if (gSuppressBleEventPrint)
            {
                break;
            }

            shell_write("GATTConfirm");
            shell_write(" -> ");

            switch (container->Data.GATTConfirm.Status)
            {
                case GATTConfirm_Status_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GATTConfirm_Status_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTConfirm_Status_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTConfirm_Status_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTConfirm_Status_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTConfirm_Status_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTConfirm_Status_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTConfirm_Status_gBleOsError_c:
                    shell_write(gBleOsError_c);
                    break;

                case GATTConfirm_Status_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedError_c);
                    break;

                case GATTConfirm_Status_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTConfirm_Status_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTConfirm_Status_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTConfirm_Status_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTConfirm_Status_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTConfirm_Status_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTConfirm_Status_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTConfirm_Status_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTConfirm_Status_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTConfirm_Status_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTConfirm_Status_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTConfirm_Status_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTConfirm_Status_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTConfirm_Status_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTConfirm_Status_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTConfirm_Status_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTConfirm_Status_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTConfirm_Status_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTConfirm_Status_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTConfirm_Status_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTConfirm_Status_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTConfirm_Status_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTConfirm_Status_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTConfirm_Status_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTConfirm_Status_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTConfirm_Status_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTConfirm_Status_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTConfirm_Status_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTConfirm_Status_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTConfirm_Status_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTConfirm_Status_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTConfirm_Status_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedError_c);
                    break;

                case GATTConfirm_Status_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTConfirm_Status_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTConfirm_Status_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTConfirm_Status_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTConfirm_Status_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTConfirm_Status_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTConfirm_Status_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTConfirm_Status_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTConfirm_Status_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTConfirm_Status_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTConfirm_Status_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTConfirm_Status_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTConfirm_Status_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTConfirm_Status_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTConfirm_Status_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTConfirm_Status_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTConfirm_Status_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTConfirm_Status_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTConfirm_Status_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTConfirm_Status_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTConfirm_Status_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTConfirm_Status_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTConfirm_Status_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTConfirm_Status_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTConfirm_Status_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTConfirm_Status_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTConfirm_Status_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTConfirm_Status_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTConfirm_Status_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTConfirm_Status_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTConfirm_Status_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTConfirm_Status_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTConfirm_Status_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTConfirm_Status_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTConfirm_Status_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTConfirm_Status_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTConfirm_Status_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTConfirm_Status_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTConfirm_Status_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTConfirm_Status_gHciTransportError_c:
                    shell_write(gHciTransportError_c);
                    break;

                case GATTConfirm_Status_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTConfirm_Status_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTConfirm_Status_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTConfirm_Status_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTConfirm_Status_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTConfirm_Status_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTConfirm_Status_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTConfirm_Status_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTConfirm_Status_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTConfirm_Status_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTConfirm_Status_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTConfirm_Status_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTConfirm_Status_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTConfirm_Status_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTConfirm_Status_gL2caError_c:
                    shell_write(gL2caError_c);
                    break;

                case GATTConfirm_Status_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTConfirm_Status_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTConfirm_Status_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTConfirm_Status_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTConfirm_Status_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTConfirm_Status_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTConfirm_Status_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTConfirm_Status_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTConfirm_Status_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTConfirm_Status_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTConfirm_Status_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTConfirm_Status_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTConfirm_Status_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTConfirm_Status_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTConfirm_Status_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTConfirm_Status_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTConfirm_Status_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTConfirm_Status_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTConfirm_Status_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTConfirm_Status_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTConfirm_Status_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTConfirm_Status_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTConfirm_Status_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTConfirm_Status_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTConfirm_Status_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTConfirm_Status_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTConfirm_Status_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTConfirm_Status_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTConfirm_Status_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTConfirm_Status_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTConfirm_Status_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTConfirm_Status_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTConfirm_Status_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTConfirm_Status_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTConfirm_Status_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTConfirm_Status_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTConfirm_Status_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTConfirm_Status_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTConfirm_Status_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTConfirm_Status_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTConfirm_Status_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTConfirm_Status_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTConfirm_Status_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTConfirm_Status_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTConfirm_Status_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTConfirm_Status_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTConfirm_Status_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTConfirm_Status_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTConfirm_Status_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTConfirm_Status_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTConfirm_Status_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTConfirm_Status_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTConfirm_Status_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTConfirm_Status_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTConfirm_Status_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTConfirm_Status_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTConfirm_Status_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTConfirm_Status_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTConfirm_Status_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTConfirm_Status_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTConfirm_Status_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTConfirm_Status_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTConfirm_Status_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTConfirm_Status_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTConfirm_Status_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTConfirm_Status_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTConfirm_Status_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTConfirm_Status_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTConfirm_Status_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTConfirm.Status);
                    break;
            }

            break;

        case 0x4481:
            shell_write("GATTGetMtuIndication");
            break;

        case 0x4482:
            shell_write("GATTClientProcedureExchangeMtuIndication");
            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureExchangeMtuIndication.ProcedureResult)
            {
                case GATTClientProcedureExchangeMtuIndication_ProcedureResult_gGattProcSuccess_c:
                    shell_write(gGattProcSuccess_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_ProcedureResult_gProcedureError_c:
                    shell_write(gProcedureError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GATTClientProcedureExchangeMtuIndication.ProcedureResult);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureExchangeMtuIndication.Error)
            {
                case GATTClientProcedureExchangeMtuIndication_Error_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gBleOsError_c:
                    shell_write(gBleOsc);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedc);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedc);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gHciTransportError_c:
                    shell_write(gHciTransportc);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gL2caError_c:
                    shell_write(gL2cac);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTClientProcedureExchangeMtuIndication_Error_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTClientProcedureExchangeMtuIndication_Error_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTClientProcedureExchangeMtuIndication.Error);
                    break;
            }

            break;

        case 0x4483:
            shell_write("GATTClientProcedureDiscoverAllPrimaryServicesIndication");
            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureDiscoverAllPrimaryServicesIndication.ProcedureResult)
            {
                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_ProcedureResult_gGattProcSuccess_c:
                    shell_write(gGattProcSuccess_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_ProcedureResult_gProcedureError_c:
                    shell_write(gProcedureError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GATTClientProcedureDiscoverAllPrimaryServicesIndication.ProcedureResult);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureDiscoverAllPrimaryServicesIndication.Error)
            {
                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    SHELL_PrintDiscoveredPrimaryServices(&(container->Data.GATTClientProcedureDiscoverAllPrimaryServicesIndication));
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gBleOsError_c:
                    shell_write(gBleOsc);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedc);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedc);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gHciTransportError_c:
                    shell_write(gHciTransportc);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gL2caError_c:
                    shell_write(gL2cac);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllPrimaryServicesIndication_Error_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTClientProcedureDiscoverAllPrimaryServicesIndication.Error);
                    break;
            }

            break;

        case 0x4484:
            shell_write("GATTClientProcedureDiscoverPrimaryServicesByUuidIndication");
            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.ProcedureResult)
            {
                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_ProcedureResult_gGattProcSuccess_c:
                    shell_write(gGattProcSuccess_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_ProcedureResult_gProcedureError_c:
                    shell_write(gProcedureError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.ProcedureResult);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.Error)
            {
                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    SHELL_PrintDiscoveredPrimaryServices((GATTClientProcedureDiscoverAllPrimaryServicesIndication_t *) & (container->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication));
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gBleOsError_c:
                    shell_write(gBleOsc);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedc);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedc);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gHciTransportError_c:
                    shell_write(gHciTransportc);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gL2caError_c:
                    shell_write(gL2cac);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTClientProcedureDiscoverPrimaryServicesByUuidIndication_Error_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTClientProcedureDiscoverPrimaryServicesByUuidIndication.Error);
                    break;
            }

            break;

        case 0x4485:
            shell_write("GATTClientProcedureFindIncludedServicesIndication");
            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureFindIncludedServicesIndication.ProcedureResult)
            {
                case GATTClientProcedureFindIncludedServicesIndication_ProcedureResult_gGattProcSuccess_c:
                    shell_write(gGattProcSuccess_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_ProcedureResult_gProcedureError_c:
                    shell_write(gProcedureError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GATTClientProcedureFindIncludedServicesIndication.ProcedureResult);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureFindIncludedServicesIndication.Error)
            {
                case GATTClientProcedureFindIncludedServicesIndication_Error_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gBleOsError_c:
                    shell_write(gBleOsc);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedc);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedc);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gHciTransportError_c:
                    shell_write(gHciTransportc);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gL2caError_c:
                    shell_write(gL2cac);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTClientProcedureFindIncludedServicesIndication_Error_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTClientProcedureFindIncludedServicesIndication.Error);
                    break;
            }

            break;

        case 0x4486:
            shell_write("GATTClientProcedureDiscoverAllCharacteristicsIndication");
            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication.ProcedureResult)
            {
                case GATTClientProcedureDiscoverAllCharacteristicsIndication_ProcedureResult_gGattProcSuccess_c:
                    shell_write(gGattProcSuccess_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_ProcedureResult_gProcedureError_c:
                    shell_write(gProcedureError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication.ProcedureResult);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication.Error)
            {
                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    SHELL_PrintDiscoveredChars(&(container->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication));
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gBleOsError_c:
                    shell_write(gBleOsc);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedc);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedc);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gHciTransportError_c:
                    shell_write(gHciTransportc);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gL2caError_c:
                    shell_write(gL2cac);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicsIndication_Error_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTClientProcedureDiscoverAllCharacteristicsIndication.Error);
                    break;
            }

            break;

        case 0x4487:
            shell_write("GATTClientProcedureDiscoverCharacteristicByUuidIndication");
            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureDiscoverCharacteristicByUuidIndication.ProcedureResult)
            {
                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_ProcedureResult_gGattProcSuccess_c:
                    shell_write(gGattProcSuccess_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_ProcedureResult_gProcedureError_c:
                    shell_write(gProcedureError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GATTClientProcedureDiscoverCharacteristicByUuidIndication.ProcedureResult);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureDiscoverCharacteristicByUuidIndication.Error)
            {
                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gBleOsError_c:
                    shell_write(gBleOsc);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedc);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedc);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gHciTransportError_c:
                    shell_write(gHciTransportc);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gL2caError_c:
                    shell_write(gL2cac);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTClientProcedureDiscoverCharacteristicByUuidIndication_Error_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTClientProcedureDiscoverCharacteristicByUuidIndication.Error);
                    break;
            }

            break;

        case 0x4488:
            shell_write("GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication");
            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication.ProcedureResult)
            {
                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_ProcedureResult_gGattProcSuccess_c:
                    shell_write(gGattProcSuccess_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_ProcedureResult_gProcedureError_c:
                    shell_write(gProcedureError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication.ProcedureResult);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication.Error)
            {
                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    SHELL_PrintDiscoveredDescriptors(&(container->Data.GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication));
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gBleOsError_c:
                    shell_write(gBleOsc);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedc);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedc);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gHciTransportError_c:
                    shell_write(gHciTransportc);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gL2caError_c:
                    shell_write(gL2cac);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication_Error_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTClientProcedureDiscoverAllCharacteristicDescriptorsIndication.Error);
                    break;
            }

            break;

        case 0x4489:
            shell_write("GATTClientProcedureReadCharacteristicValueIndication");
            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureReadCharacteristicValueIndication.ProcedureResult)
            {
                case GATTClientProcedureReadCharacteristicValueIndication_ProcedureResult_gGattProcSuccess_c:
                    shell_write(gGattProcSuccess_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_ProcedureResult_gProcedureError_c:
                    shell_write(gProcedureError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GATTClientProcedureReadCharacteristicValueIndication.ProcedureResult);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureReadCharacteristicValueIndication.Error)
            {
                case GATTClientProcedureReadCharacteristicValueIndication_Error_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    shell_write(" -> ");
                    shell_print_buffer(
                        container->Data.GATTClientProcedureReadCharacteristicValueIndication.Characteristic.Value.Value,
                        container->Data.GATTClientProcedureReadCharacteristicValueIndication.Characteristic.Value.ValueLength
                    );
                    MEM_BufferFree(container->Data.GATTClientProcedureReadCharacteristicValueIndication.Characteristic.Value.Value);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gBleOsError_c:
                    shell_write(gBleOsc);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedc);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedc);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gHciTransportError_c:
                    shell_write(gHciTransportc);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gL2caError_c:
                    shell_write(gL2cac);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTClientProcedureReadCharacteristicValueIndication_Error_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTClientProcedureReadCharacteristicValueIndication.Error);
                    break;
            }

            break;

        case 0x448A:
            shell_write("GATTClientProcedureReadUsingCharacteristicUuidIndication");
            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureReadUsingCharacteristicUuidIndication.ProcedureResult)
            {
                case GATTClientProcedureReadUsingCharacteristicUuidIndication_ProcedureResult_gGattProcSuccess_c:
                    shell_write(gGattProcSuccess_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_ProcedureResult_gProcedureError_c:
                    shell_write(gProcedureError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GATTClientProcedureReadUsingCharacteristicUuidIndication.ProcedureResult);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureReadUsingCharacteristicUuidIndication.Error)
            {
                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gBleOsError_c:
                    shell_write(gBleOsc);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedc);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedc);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gHciTransportError_c:
                    shell_write(gHciTransportc);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gL2caError_c:
                    shell_write(gL2cac);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTClientProcedureReadUsingCharacteristicUuidIndication_Error_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTClientProcedureReadUsingCharacteristicUuidIndication.Error);
                    break;
            }

            break;

        case 0x448B:
            shell_write("GATTClientProcedureReadMultipleCharacteristicValuesIndication");
            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureReadMultipleCharacteristicValuesIndication.ProcedureResult)
            {
                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_ProcedureResult_gGattProcSuccess_c:
                    shell_write(gGattProcSuccess_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_ProcedureResult_gProcedureError_c:
                    shell_write(gProcedureError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GATTClientProcedureReadMultipleCharacteristicValuesIndication.ProcedureResult);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureReadMultipleCharacteristicValuesIndication.Error)
            {
                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gBleOsError_c:
                    shell_write(gBleOsc);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedc);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedc);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gHciTransportError_c:
                    shell_write(gHciTransportc);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gL2caError_c:
                    shell_write(gL2cac);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTClientProcedureReadMultipleCharacteristicValuesIndication_Error_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTClientProcedureReadMultipleCharacteristicValuesIndication.Error);
                    break;
            }

            break;

        case 0x448C:
            shell_write("GATTClientProcedureWriteCharacteristicValueIndication");
            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureWriteCharacteristicValueIndication.ProcedureResult)
            {
                case GATTClientProcedureWriteCharacteristicValueIndication_ProcedureResult_gGattProcSuccess_c:
                    shell_write(gGattProcSuccess_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_ProcedureResult_gProcedureError_c:
                    shell_write(gProcedureError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GATTClientProcedureWriteCharacteristicValueIndication.ProcedureResult);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureWriteCharacteristicValueIndication.Error)
            {
                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gBleOsError_c:
                    shell_write(gBleOsc);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedc);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedc);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gHciTransportError_c:
                    shell_write(gHciTransportc);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gL2caError_c:
                    shell_write(gL2cac);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTClientProcedureWriteCharacteristicValueIndication_Error_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTClientProcedureWriteCharacteristicValueIndication.Error);
                    break;
            }

            break;

        case 0x448D:
            shell_write("GATTClientProcedureReadCharacteristicDescriptorIndication");
            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureReadCharacteristicDescriptorIndication.ProcedureResult)
            {
                case GATTClientProcedureReadCharacteristicDescriptorIndication_ProcedureResult_gGattProcSuccess_c:
                    shell_write(gGattProcSuccess_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_ProcedureResult_gProcedureError_c:
                    shell_write(gProcedureError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GATTClientProcedureReadCharacteristicDescriptorIndication.ProcedureResult);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureReadCharacteristicDescriptorIndication.Error)
            {
                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gBleOsError_c:
                    shell_write(gBleOsc);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedc);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedc);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gHciTransportError_c:
                    shell_write(gHciTransportc);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gL2caError_c:
                    shell_write(gL2cac);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTClientProcedureReadCharacteristicDescriptorIndication_Error_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTClientProcedureReadCharacteristicDescriptorIndication.Error);
                    break;
            }

            break;

        case 0x448E:
            shell_write("GATTClientProcedureWriteCharacteristicDescriptorIndication");
            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureWriteCharacteristicDescriptorIndication.ProcedureResult)
            {
                case GATTClientProcedureWriteCharacteristicDescriptorIndication_ProcedureResult_gGattProcSuccess_c:
                    shell_write(gGattProcSuccess_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_ProcedureResult_gProcedureError_c:
                    shell_write(gProcedureError_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GATTClientProcedureWriteCharacteristicDescriptorIndication.ProcedureResult);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GATTClientProcedureWriteCharacteristicDescriptorIndication.Error)
            {
                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gBleOsError_c:
                    shell_write(gBleOsc);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedc);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedc);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gHciTransportError_c:
                    shell_write(gHciTransportc);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gL2caError_c:
                    shell_write(gL2cac);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTClientProcedureWriteCharacteristicDescriptorIndication_Error_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTClientProcedureWriteCharacteristicDescriptorIndication.Error);
                    break;
            }

            break;

        case 0x448F:
            shell_write("GATTClientNotificationIndication");
            SHELL_NEWLINE();
            shell_printf("\tCharacteristic Value Handle -> %d", container->Data.GATTClientNotificationIndication.CharacteristicValueHandle);
            SHELL_NEWLINE();
            shell_write("\tValue -> ");
            shell_print_buffer(container->Data.GATTClientNotificationIndication.Value, container->Data.GATTClientNotificationIndication.ValueLength);
            MEM_BufferFree(container->Data.GATTClientNotificationIndication.Value);
            break;

        case 0x4490:
            shell_write("GATTClientIndicationIndication");
            break;

        case 0x4491:
            shell_write("GATTServerMtuChangedIndication");
            break;

        case 0x4492:
            if (!gSuppressBleEventPrint)
            {
                shell_write("GATTServerHandleValueConfirmationIndication");
            }

            break;

        case 0x4493:
            shell_write("GATTServerAttributeWrittenIndication");
            MEM_BufferFree(container->Data.GATTServerAttributeWrittenIndication.AttributeWrittenEvent.Value);
            break;

        case 0x4494:
            if (!gSuppressBleEventPrint)
            {
                shell_write("GATTServerCharacteristicCccdWrittenIndication");
            }

            break;

        case 0x4495:
            if (gOtapState != gOtapConnectionEstablished_c &&
                gWUartState != gWUartConnectionEstablished_c)
            {
                shell_write("GATTServerAttributeWrittenWithoutResponseIndication");
            }

            if (gWUartState == gWUartConnectionEstablished_c)
            {
//                shell_printf("\r\n[Wireless UART RX] %s\r\n",
//                             container->Data.GATTServerAttributeWrittenWithoutResponseIndication.AttributeWrittenEvent.Value);
                g_ready_flag = TRUE;
            }

            MEM_BufferFree(container->Data.GATTServerAttributeWrittenWithoutResponseIndication.AttributeWrittenEvent.Value);
            break;

        case 0x4496:
            shell_write("GATTServerErrorIndication");
            break;

        case 0x4497:
            shell_write("GATTServerLongCharacteristicWrittenIndication");
            break;

        case 0x4498:
            shell_write("GATTServerAttributeReadIndication");
            break;

#endif  /* GATT_ENABLE */

#if GATTDB_APP_ENABLE

        case 0x4580:

            if (gSuppressBleEventPrint)
            {
                break;
            }

            shell_write("GATTDBConfirm");
            shell_write(" -> ");

            switch (container->Data.GATTDBConfirm.Status)
            {
                case GATTDBConfirm_Status_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GATTDBConfirm_Status_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GATTDBConfirm_Status_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GATTDBConfirm_Status_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GATTDBConfirm_Status_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GATTDBConfirm_Status_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GATTDBConfirm_Status_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GATTDBConfirm_Status_gBleOsError_c:
                    shell_write(gBleOsError_c);
                    break;

                case GATTDBConfirm_Status_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedError_c);
                    break;

                case GATTDBConfirm_Status_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GATTDBConfirm_Status_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GATTDBConfirm_Status_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GATTDBConfirm_Status_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GATTDBConfirm_Status_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GATTDBConfirm_Status_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GATTDBConfirm_Status_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GATTDBConfirm_Status_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GATTDBConfirm_Status_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GATTDBConfirm_Status_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GATTDBConfirm_Status_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GATTDBConfirm_Status_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GATTDBConfirm_Status_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GATTDBConfirm_Status_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GATTDBConfirm_Status_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GATTDBConfirm_Status_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GATTDBConfirm_Status_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GATTDBConfirm_Status_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GATTDBConfirm_Status_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GATTDBConfirm_Status_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GATTDBConfirm_Status_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GATTDBConfirm_Status_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GATTDBConfirm_Status_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GATTDBConfirm_Status_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GATTDBConfirm_Status_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GATTDBConfirm_Status_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GATTDBConfirm_Status_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GATTDBConfirm_Status_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GATTDBConfirm_Status_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GATTDBConfirm_Status_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GATTDBConfirm_Status_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GATTDBConfirm_Status_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedError_c);
                    break;

                case GATTDBConfirm_Status_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GATTDBConfirm_Status_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GATTDBConfirm_Status_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GATTDBConfirm_Status_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GATTDBConfirm_Status_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GATTDBConfirm_Status_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GATTDBConfirm_Status_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GATTDBConfirm_Status_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GATTDBConfirm_Status_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GATTDBConfirm_Status_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GATTDBConfirm_Status_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GATTDBConfirm_Status_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GATTDBConfirm_Status_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GATTDBConfirm_Status_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GATTDBConfirm_Status_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GATTDBConfirm_Status_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GATTDBConfirm_Status_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GATTDBConfirm_Status_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GATTDBConfirm_Status_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GATTDBConfirm_Status_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GATTDBConfirm_Status_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GATTDBConfirm_Status_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GATTDBConfirm_Status_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GATTDBConfirm_Status_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GATTDBConfirm_Status_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GATTDBConfirm_Status_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GATTDBConfirm_Status_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GATTDBConfirm_Status_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GATTDBConfirm_Status_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GATTDBConfirm_Status_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GATTDBConfirm_Status_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GATTDBConfirm_Status_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GATTDBConfirm_Status_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GATTDBConfirm_Status_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GATTDBConfirm_Status_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GATTDBConfirm_Status_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GATTDBConfirm_Status_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GATTDBConfirm_Status_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GATTDBConfirm_Status_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GATTDBConfirm_Status_gHciTransportError_c:
                    shell_write(gHciTransportError_c);
                    break;

                case GATTDBConfirm_Status_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GATTDBConfirm_Status_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GATTDBConfirm_Status_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GATTDBConfirm_Status_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GATTDBConfirm_Status_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GATTDBConfirm_Status_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GATTDBConfirm_Status_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GATTDBConfirm_Status_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GATTDBConfirm_Status_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GATTDBConfirm_Status_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GATTDBConfirm_Status_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GATTDBConfirm_Status_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GATTDBConfirm_Status_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GATTDBConfirm_Status_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GATTDBConfirm_Status_gL2caError_c:
                    shell_write(gL2caError_c);
                    break;

                case GATTDBConfirm_Status_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GATTDBConfirm_Status_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GATTDBConfirm_Status_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GATTDBConfirm_Status_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GATTDBConfirm_Status_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GATTDBConfirm_Status_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GATTDBConfirm_Status_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GATTDBConfirm_Status_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GATTDBConfirm_Status_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GATTDBConfirm_Status_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GATTDBConfirm_Status_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GATTDBConfirm_Status_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GATTDBConfirm_Status_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GATTDBConfirm_Status_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GATTDBConfirm_Status_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GATTDBConfirm_Status_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GATTDBConfirm_Status_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GATTDBConfirm_Status_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GATTDBConfirm_Status_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GATTDBConfirm_Status_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GATTDBConfirm_Status_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GATTDBConfirm_Status_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GATTDBConfirm_Status_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GATTDBConfirm_Status_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GATTDBConfirm_Status_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GATTDBConfirm_Status_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GATTDBConfirm_Status_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GATTDBConfirm_Status_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GATTDBConfirm_Status_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GATTDBConfirm_Status_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GATTDBConfirm_Status_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GATTDBConfirm_Status_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GATTDBConfirm_Status_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GATTDBConfirm_Status_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GATTDBConfirm_Status_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GATTDBConfirm_Status_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GATTDBConfirm_Status_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GATTDBConfirm_Status_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GATTDBConfirm_Status_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GATTDBConfirm_Status_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GATTDBConfirm_Status_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GATTDBConfirm_Status_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GATTDBConfirm_Status_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GATTDBConfirm_Status_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GATTDBConfirm_Status_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GATTDBConfirm_Status_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GATTDBConfirm_Status_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GATTDBConfirm_Status_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GATTDBConfirm_Status_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GATTDBConfirm_Status_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GATTDBConfirm_Status_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GATTDBConfirm_Status_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GATTDBConfirm_Status_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GATTDBConfirm_Status_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GATTDBConfirm_Status_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GATTDBConfirm_Status_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GATTDBConfirm_Status_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GATTDBConfirm_Status_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GATTDBConfirm_Status_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GATTDBConfirm_Status_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GATTDBConfirm_Status_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GATTDBConfirm_Status_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GATTDBConfirm_Status_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GATTDBConfirm_Status_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GATTDBConfirm_Status_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GATTDBConfirm_Status_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GATTDBConfirm_Status_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GATTDBConfirm_Status_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GATTDBConfirm_Status_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GATTDBConfirm.Status);
                    break;
            }

            break;

        case 0x4581:
            shell_write("GATTDBReadAttributeIndication");
            SHELL_GattDb_PrintReadAttr(&(container->Data.GATTDBReadAttributeIndication));
            break;

        case 0x4582:
            shell_write("GATTDBFindServiceHandleIndication");
            shell_printf("\r\n\tService Handle --> %d", container->Data.GATTDBFindServiceHandleIndication.ServiceHandle);
            gLatestHandle = container->Data.GATTDBFindServiceHandleIndication.ServiceHandle;
            break;

        case 0x4583:
            if (!gSuppressBleEventPrint)
            {
                shell_write("GATTDBFindCharValueHandleInServiceIndication");
                shell_printf("\r\n\tCharacteristic Value Handle --> %d", container->Data.GATTDBFindCharValueHandleInServiceIndication.CharValueHandle);
            }

            gLatestHandle = container->Data.GATTDBFindCharValueHandleInServiceIndication.CharValueHandle;
            break;

        case 0x4584:

            if (!gSuppressBleEventPrint)
            {
                shell_write("GATTDBFindCccdHandleForCharValueHandleIndication");
            }

            gCccdHandle = container->Data.GATTDBFindCccdHandleForCharValueHandleIndication.CccdHandle;
            break;

        case 0x4585:
            shell_write("GATTDBFindDescriptorHandleForCharValueHandleIndication");
            break;

        case 0x4586:
            shell_write("GATTDBDynamicAddPrimaryServiceDeclarationIndication");
            SHELL_GattDb_PrintPrimaryService(&(container->Data.GATTDBDynamicAddPrimaryServiceDeclarationIndication));
            break;

        case 0x4587:
            shell_write("GATTDBDynamicAddSecondaryServiceDeclarationIndication");
            break;

        case 0x4588:
            shell_write("GATTDBDynamicAddIncludeDeclarationIndication");
            break;

        case 0x4589:
            shell_write("GATTDBDynamicAddCharacteristicDeclarationAndValueIndication");
            SHELL_GattDb_PrintChar(&(container->Data.GATTDBDynamicAddCharacteristicDeclarationAndValueIndication));
            break;

        case 0x458A:
            shell_write("GATTDBDynamicAddCharacteristicDescriptorIndication");
            SHELL_GattDb_PrintCharDesc(&(container->Data.GATTDBDynamicAddCharacteristicDescriptorIndication));
            break;

        case 0x458B:
            shell_write("GATTDBDynamicAddCccdIndication");
            SHELL_GattDb_PrintCCCD(&(container->Data.GATTDBDynamicAddCccdIndication));
            break;

        case 0x458C:
            shell_write("GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueIndication");
            break;

#endif  /* GATTDB_APP_ENABLE */

#if GAP_ENABLE

        case 0x4780:

            if (gSuppressBleEventPrint)
            {
                break;
            }

            shell_write("GAPConfirm");
            shell_write(" -> ");

            switch (container->Data.GAPConfirm.Status)
            {
                case GAPConfirm_Status_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GAPConfirm_Status_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GAPConfirm_Status_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GAPConfirm_Status_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GAPConfirm_Status_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GAPConfirm_Status_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GAPConfirm_Status_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GAPConfirm_Status_gBleOsError_c:
                    shell_write(gBleOsError_c);
                    break;

                case GAPConfirm_Status_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedError_c);
                    break;

                case GAPConfirm_Status_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GAPConfirm_Status_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GAPConfirm_Status_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GAPConfirm_Status_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GAPConfirm_Status_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GAPConfirm_Status_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GAPConfirm_Status_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GAPConfirm_Status_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GAPConfirm_Status_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GAPConfirm_Status_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GAPConfirm_Status_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GAPConfirm_Status_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GAPConfirm_Status_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GAPConfirm_Status_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GAPConfirm_Status_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GAPConfirm_Status_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GAPConfirm_Status_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GAPConfirm_Status_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GAPConfirm_Status_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GAPConfirm_Status_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GAPConfirm_Status_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GAPConfirm_Status_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GAPConfirm_Status_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GAPConfirm_Status_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GAPConfirm_Status_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GAPConfirm_Status_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GAPConfirm_Status_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GAPConfirm_Status_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GAPConfirm_Status_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GAPConfirm_Status_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GAPConfirm_Status_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GAPConfirm_Status_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedError_c);
                    break;

                case GAPConfirm_Status_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GAPConfirm_Status_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GAPConfirm_Status_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GAPConfirm_Status_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GAPConfirm_Status_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GAPConfirm_Status_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GAPConfirm_Status_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GAPConfirm_Status_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GAPConfirm_Status_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GAPConfirm_Status_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GAPConfirm_Status_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GAPConfirm_Status_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GAPConfirm_Status_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GAPConfirm_Status_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GAPConfirm_Status_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GAPConfirm_Status_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GAPConfirm_Status_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GAPConfirm_Status_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GAPConfirm_Status_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GAPConfirm_Status_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GAPConfirm_Status_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GAPConfirm_Status_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GAPConfirm_Status_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GAPConfirm_Status_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GAPConfirm_Status_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GAPConfirm_Status_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GAPConfirm_Status_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GAPConfirm_Status_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GAPConfirm_Status_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GAPConfirm_Status_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GAPConfirm_Status_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GAPConfirm_Status_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GAPConfirm_Status_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GAPConfirm_Status_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GAPConfirm_Status_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GAPConfirm_Status_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GAPConfirm_Status_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GAPConfirm_Status_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GAPConfirm_Status_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GAPConfirm_Status_gHciTransportError_c:
                    shell_write(gHciTransportError_c);
                    break;

                case GAPConfirm_Status_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GAPConfirm_Status_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GAPConfirm_Status_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GAPConfirm_Status_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GAPConfirm_Status_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GAPConfirm_Status_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GAPConfirm_Status_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GAPConfirm_Status_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GAPConfirm_Status_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GAPConfirm_Status_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GAPConfirm_Status_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GAPConfirm_Status_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GAPConfirm_Status_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GAPConfirm_Status_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GAPConfirm_Status_gL2caError_c:
                    shell_write(gL2caError_c);
                    break;

                case GAPConfirm_Status_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GAPConfirm_Status_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GAPConfirm_Status_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GAPConfirm_Status_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GAPConfirm_Status_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GAPConfirm_Status_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GAPConfirm_Status_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GAPConfirm_Status_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GAPConfirm_Status_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GAPConfirm_Status_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GAPConfirm_Status_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GAPConfirm_Status_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GAPConfirm_Status_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GAPConfirm_Status_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GAPConfirm_Status_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GAPConfirm_Status_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GAPConfirm_Status_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GAPConfirm_Status_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GAPConfirm_Status_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GAPConfirm_Status_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GAPConfirm_Status_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GAPConfirm_Status_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GAPConfirm_Status_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GAPConfirm_Status_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GAPConfirm_Status_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GAPConfirm_Status_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GAPConfirm_Status_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GAPConfirm_Status_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GAPConfirm_Status_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GAPConfirm_Status_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GAPConfirm_Status_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GAPConfirm_Status_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GAPConfirm_Status_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GAPConfirm_Status_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GAPConfirm_Status_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GAPConfirm_Status_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GAPConfirm_Status_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GAPConfirm_Status_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GAPConfirm_Status_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GAPConfirm_Status_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GAPConfirm_Status_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GAPConfirm_Status_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GAPConfirm_Status_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GAPConfirm_Status_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GAPConfirm_Status_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GAPConfirm_Status_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GAPConfirm_Status_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GAPConfirm_Status_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GAPConfirm_Status_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GAPConfirm_Status_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GAPConfirm_Status_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GAPConfirm_Status_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GAPConfirm_Status_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GAPConfirm_Status_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GAPConfirm_Status_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GAPConfirm_Status_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GAPConfirm_Status_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GAPConfirm_Status_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GAPConfirm_Status_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GAPConfirm_Status_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GAPConfirm_Status_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GAPConfirm_Status_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GAPConfirm_Status_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GAPConfirm_Status_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GAPConfirm_Status_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GAPConfirm_Status_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GAPConfirm_Status_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GAPConfirm_Status_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GAPConfirm_Status_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GAPConfirm.Status);
                    break;
            }

            break;

        case 0x4781:
            if (!gSuppressBleEventPrint)
            {
                shell_write("GAPCheckNotificationStatusIndication");
            }

            gIsNotificationActive = container->Data.GAPCheckNotificationStatusIndication.IsActive;
            break;

        case 0x4782:
            if (!gSuppressBleEventPrint)
            {
                shell_write("GAPCheckIndicationStatusIndication");
            }

            break;

        case 0x4783:
            shell_write("GAPGetBondedStaticAddressesIndication");
            BleApp_Addbondaddress(container->Data.GAPGetBondedStaticAddressesIndication);
            break;

        case 0x4784:
            shell_write("GAPLoadEncryptionInformationIndication");
            break;

        case 0x4785:
            shell_write("GAPLoadCustomPeerInformationIndication");
            break;

        case 0x4786:
            shell_write("GAPCheckIfBondedIndication");
            if(container->Data.GAPCheckIfBondedIndication.IsBonded)
            {
            	BleApp_LoadCustomerPeerInfo();
            }
            else
            {
            	BleApp_Sendseqreq();
            }
            break;

        case 0x4787:
            shell_write("GAPGetBondedDevicesCountIndication");
            break;

        case 0x4788:
            shell_write("GAPGetBondedDeviceNameIndication");
            break;

        case 0x4789:
            shell_write("GAPGenericEventInitializationCompleteIndication");
            break;

        case 0x478A:
            shell_write("GAPGenericEventInternalErrorIndication");
            shell_write(" -> ");

            switch (container->Data.GAPGenericEventInternalErrorIndication.ErrorCode)
            {
                case GAPGenericEventInternalErrorIndication_ErrorCode_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gBleOsError_c:
                    shell_write(gBleOsError_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedError_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedError_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gHciTransportError_c:
                    shell_write(gHciTransportError_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gL2caError_c:
                    shell_write(gL2caError_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorCode_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GAPGenericEventInternalErrorIndication.ErrorCode);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GAPGenericEventInternalErrorIndication.ErrorSource)
            {
                case GAPGenericEventInternalErrorIndication_ErrorSource_gHciCommandStatus_c:
                    shell_write(gHciCommandStatus_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gCheckPrivateResolvableAddress_c:
                    shell_write(gCheckPrivateResolvableAddress_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gVerifySignature_c:
                    shell_write(gVerifySignature_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gAddNewConnection_c:
                    shell_write(gAddNewConnection_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gResetController_c:
                    shell_write(gResetController_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gSetEventMask_c:
                    shell_write(gSetEventMask_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gReadLeBufferSize_c:
                    shell_write(gReadLeBufferSize_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gSetLeEventMask_c:
                    shell_write(gSetLeEventMask_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gReadDeviceAddress_c:
                    shell_write(gReadDeviceAddress_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gReadLocalSupportedFeatures_c:
                    shell_write(gReadLocalSupportedFeatures_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gReadWhiteListSize_c:
                    shell_write(gReadWhiteListSize_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gClearWhiteList_c:
                    shell_write(gClearWhiteList_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gAddDeviceToWhiteList_c:
                    shell_write(gAddDeviceToWhiteList_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gRemoveDeviceFromWhiteList_c:
                    shell_write(gRemoveDeviceFromWhiteList_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gCancelCreateConnection_c:
                    shell_write(gCancelCreateConnection_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gReadRadioPower_c:
                    shell_write(gReadRadioPower_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gSetRandomAddress_c:
                    shell_write(gSetRandomAddress_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gCreateRandomAddress_c:
                    shell_write(gCreateRandomAddress_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gEncryptLink_c:
                    shell_write(gEncryptLink_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gProvideLongTermKey_c:
                    shell_write(gProvideLongTermKey_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gDenyLongTermKey_c:
                    shell_write(gDenyLongTermKey_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gConnect_c:
                    shell_write(gConnect_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gDisconnect_c:
                    shell_write(gDisconnect_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gTerminatePairing_c:
                    shell_write(gTerminatePairing_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gSendSlaveSecurityRequest_c:
                    shell_write(gSendSlaveSecurityRequest_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gEnterPasskey_c:
                    shell_write(gEnterPasskey_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gProvideOob_c:
                    shell_write(gProvideOob_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gSendSmpKeys_c:
                    shell_write(gSendSmpKeys_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gWriteSuggestedDefaultDataLength_c:
                    shell_write(gWriteSuggestedDefaultDataLength_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gReadSuggestedDefaultDataLength_c:
                    shell_write(gReadSuggestedDefaultDataLength_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gUpdateLeDataLength_c:
                    shell_write(gUpdateLeDataLength_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gEnableControllerPrivacy_c:
                    shell_write(gEnableControllerPrivacy_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gLeScSendKeypressNotification_c:
                    shell_write(gLeScSendKeypressNotification_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gLeScSetPeerOobData_c:
                    shell_write(gLeScSetPeerOobData_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gLeScGetLocalOobData_c:
                    shell_write(gLeScGetLocalOobData_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gLeScValidateNumericValue_c:
                    shell_write(gLeScValidateNumericValue_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gLeScRegeneratePublicKey_c:
                    shell_write(gLeScRegeneratePublicKey_c);
                    break;

                case GAPGenericEventInternalErrorIndication_ErrorSource_gLeSetResolvablePrivateAddressTimeout_c:
                    shell_write(gLeSetResolvablePrivateAddressTimeout_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GAPGenericEventInternalErrorIndication.ErrorSource);
                    break;
            }

            break;

        case 0x478B:
            shell_write("GAPGenericEventAdvertisingSetupFailedIndication");
            shell_write(" -> ");

            switch (container->Data.GAPGenericEventAdvertisingSetupFailedIndication.SetupFailReason)
            {
                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gBleOsError_c:
                    shell_write(gBleOsError_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedError_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedError_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gHciTransportError_c:
                    shell_write(gHciTransportError_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gL2caError_c:
                    shell_write(gL2caError_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GAPGenericEventAdvertisingSetupFailedIndication_SetupFailReason_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GAPGenericEventAdvertisingSetupFailedIndication.SetupFailReason);
                    break;
            }

            break;

        case 0x478C:
            shell_write("GAPGenericEventAdvertisingParametersSetupCompleteIndication");
            break;

        case 0x478D:
            shell_write("GAPGenericEventAdvertisingDataSetupCompleteIndication");
            break;

        case 0x478E:
            shell_write("GAPGenericEventWhiteListSizeReadIndication");
            break;

        case 0x478F:
            shell_write("GAPGenericEventDeviceAddedToWhiteListIndication");
            break;

        case 0x4790:
            shell_write("GAPGenericEventDeviceRemovedFromWhiteListIndication");
            break;

        case 0x4791:
            shell_write("GAPGenericEventWhiteListClearedIndication");
            break;

        case 0x4792:
            shell_write("GAPGenericEventRandomAddressReadyIndication");
            break;

        case 0x4793:
            shell_write("GAPGenericEventCreateConnectionCanceledIndication");
            break;

        case 0x4794:
            shell_write("GAPGenericEventPublicAddressReadIndication");
            SHELL_GapPrintPublicAddr(&(container->Data.GAPGenericEventPublicAddressReadIndication));
            break;

        case 0x4795:
            shell_write("GAPGenericEventAdvTxPowerLevelReadIndication");
            break;

        case 0x4796:
            shell_write("GAPGenericEventPrivateResolvableAddressVerifiedIndication");
            break;

        case 0x4797:
            shell_write("GAPGenericEventRandomAddressSetIndication");
            break;

        case 0x4798:
            shell_write("GAPAdvertisingEventStateChangedIndication");
            break;

        case 0x4799:
            shell_write("GAPAdvertisingEventCommandFailedIndication");
            shell_write(" -> ");

            switch (container->Data.GAPAdvertisingEventCommandFailedIndication.FailReason)
            {
                case GAPAdvertisingEventCommandFailedIndication_FailReason_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gBleOsError_c:
                    shell_write(gBleOsError_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedError_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedError_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gHciTransportError_c:
                    shell_write(gHciTransportError_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gL2caError_c:
                    shell_write(gL2caError_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GAPAdvertisingEventCommandFailedIndication_FailReason_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GAPAdvertisingEventCommandFailedIndication.FailReason);
                    break;
            }

            break;

        case 0x479A:
            shell_write("GAPScanningEventStateChangedIndication");
            break;

        case 0x479B:
            shell_write("GAPScanningEventCommandFailedIndication");
            shell_write(" -> ");

            switch (container->Data.GAPScanningEventCommandFailedIndication.FailReason)
            {
                case GAPScanningEventCommandFailedIndication_FailReason_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gBleOsError_c:
                    shell_write(gBleOsError_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedError_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedError_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gHciTransportError_c:
                    shell_write(gHciTransportError_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gL2caError_c:
                    shell_write(gL2caError_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GAPScanningEventCommandFailedIndication_FailReason_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GAPScanningEventCommandFailedIndication_FailReason_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GAPScanningEventCommandFailedIndication.FailReason);
                    break;
            }

            break;

        case 0x479C:
            shell_write("GAPScanningEventDeviceScannedIndication");
            shell_write(" -> ");

            switch (container->Data.GAPScanningEventDeviceScannedIndication.AddressType)
            {
                case GAPScanningEventDeviceScannedIndication_AddressType_gPublic_c:
                    shell_write(gPublic_c);
                    break;

                case GAPScanningEventDeviceScannedIndication_AddressType_gRandom_c:
                    shell_write(gRandom_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GAPScanningEventDeviceScannedIndication.AddressType);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.GAPScanningEventDeviceScannedIndication.AdvEventType)
            {
                case GAPScanningEventDeviceScannedIndication_AdvEventType_gBleAdvRepAdvInd_c:
                    shell_write(gBleAdvRepAdvInd_c);
                    break;

                case GAPScanningEventDeviceScannedIndication_AdvEventType_gBleAdvRepAdvDirectInd_c:
                    shell_write(gBleAdvRepAdvDirectInd_c);
                    break;

                case GAPScanningEventDeviceScannedIndication_AdvEventType_gBleAdvRepAdvScanInd_c:
                    shell_write(gBleAdvRepAdvScanInd_c);
                    break;

                case GAPScanningEventDeviceScannedIndication_AdvEventType_gBleAdvRepAdvNonconnInd_c:
                    shell_write(gBleAdvRepAdvNonconnInd_c);
                    break;

                case GAPScanningEventDeviceScannedIndication_AdvEventType_gBleAdvRepScanRsp_c:
                    shell_write(gBleAdvRepScanRsp_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GAPScanningEventDeviceScannedIndication.AdvEventType);
                    break;
            }

            SHELL_GapPrintDeviceScannedIndication(&(container->Data.GAPScanningEventDeviceScannedIndication));

            break;

        case 0x479D:
            shell_write("GAPConnectionEventConnectedIndication");
            shell_write(" -> ");

            switch (container->Data.GAPConnectionEventConnectedIndication.PeerAddressType)
            {
                case GAPConnectionEventConnectedIndication_PeerAddressType_gPublic_c:
                    shell_write(gPublic_c);
                    FLib_MemSet(face_PeerAddress_Bond,'\0',6);
                    FLib_MemCpy(face_PeerAddress_Bond,container->Data.GAPConnectionEventConnectedIndication.PeerAddress,6);
                    face_PeerAdd_type = container->Data.GAPConnectionEventConnectedIndication.PeerAddressType;
                    break;

                case GAPConnectionEventConnectedIndication_PeerAddressType_gRandom_c:
                    shell_write(gRandom_c);
                    FLib_MemSet(face_PeerAddress_Bond,'\0',6);
                    FLib_MemCpy(face_PeerAddress_Bond,container->Data.GAPConnectionEventConnectedIndication.PeerAddress,6);
                    face_PeerAdd_type = container->Data.GAPConnectionEventConnectedIndication.PeerAddressType;
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GAPConnectionEventConnectedIndication.PeerAddressType);
                    break;
            }

            gPeerDeviceId = container->Data.GAPConnectionEventConnectedIndication.DeviceId;
            BleApp_BondReq();

            if (gHrsDemoState == gHrsDemoWaitingConnection_c)
            {
                gHrsDemoState = gHrsDemoStartSendMeasurements;
                BleApp_DemoHrs();
            }

            if (gOtapState == gOtapWaitingConnection_c)
            {
                gOtapState = gOtapConnectionEstablished_c;
                gSuppressBleEventPrint = TRUE;
            }
            //shell_printf("\n wait gWUart status %d\n\n",gWUartState);
            gSuppressBleEventPrint = TRUE;

            if (gWUartState == gWUartWaitingConnection_c)
            {
                gWUartState = gWUartConnectionEstablished_c;
                gSuppressBleEventPrint = TRUE;
                BleApp_DemoWirelessUART();
                BleApp_UpdateConnectionParameters();
            }

            break;

        case 0x479E:
            shell_write("GAPConnectionEventPairingRequestIndication");
            SHELL_GapAcceptPairingRequest(&(container->Data.GAPConnectionEventPairingRequestIndication));
            break;

        case 0x479F:
            shell_write("GAPConnectionEventSlaveSecurityRequestIndication");
            break;

        case 0x47A0:
            shell_write("GAPConnectionEventPairingResponseIndication");
            break;

        case 0x47A1:
            shell_write("GAPConnectionEventAuthenticationRejectedIndication");
            shell_write(" -> ");

            switch (container->Data.GAPConnectionEventAuthenticationRejectedIndication.RejectReason)
            {
                case GAPConnectionEventAuthenticationRejectedIndication_RejectReason_gOobNotAvailable_c:
                    shell_write(gOobNotAvailable_c);
                    break;

                case GAPConnectionEventAuthenticationRejectedIndication_RejectReason_gIncompatibleIoCapabilities_c:
                    shell_write(gIncompatibleIoCapabilities_c);
                    break;

                case GAPConnectionEventAuthenticationRejectedIndication_RejectReason_gPairingNotSupported_c:
                    shell_write(gPairingNotSupported_c);
                    break;

                case GAPConnectionEventAuthenticationRejectedIndication_RejectReason_gLowEncryptionKeySize_c:
                    shell_write(gLowEncryptionKeySize_c);
                    break;

                case GAPConnectionEventAuthenticationRejectedIndication_RejectReason_gUnspecifiedReason_c:
                    shell_write(gUnspecifiedReason_c);
                    break;

                case GAPConnectionEventAuthenticationRejectedIndication_RejectReason_gRepeatedAttempts_c:
                    shell_write(gRepeatedAttempts_c);
                    break;

                case GAPConnectionEventAuthenticationRejectedIndication_RejectReason_gLinkEncryptionFailed_c:
                    shell_write(gLinkEncryptionFailed_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GAPConnectionEventAuthenticationRejectedIndication.RejectReason);
                    break;
            }

            break;

        case 0x47A2:
            shell_write("GAPConnectionEventPasskeyRequestIndication");
            break;

        case 0x47A3:
            shell_write("GAPConnectionEventOobRequestIndication");
            break;

        case 0x47A4:
            shell_write("GAPConnectionEventPasskeyDisplayIndication");
            break;

        case 0x47A5:
            shell_write("GAPConnectionEventKeyExchangeRequestIndication");
            shell_write(" -> ");

            switch (container->Data.GAPConnectionEventKeyExchangeRequestIndication.RequestedKeys)
            {
                case GAPConnectionEventKeyExchangeRequestIndication_RequestedKeys_gNoKeys_c:
                    shell_write(gNoKeys_c);
                    break;

                case GAPConnectionEventKeyExchangeRequestIndication_RequestedKeys_gLtk_c:
                case GAPConnectionEventKeyExchangeRequestIndication_RequestedKeys_gLtk_Irk_c:
                    shell_write(gLtk_c);
                    BleApp_SendLtk();
                    break;

                case GAPConnectionEventKeyExchangeRequestIndication_RequestedKeys_gIrk_c:
                    shell_write(gIrk_c);
                    break;

                case GAPConnectionEventKeyExchangeRequestIndication_RequestedKeys_gCsrk_c:
                    shell_write(gCsrk_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GAPConnectionEventKeyExchangeRequestIndication.RequestedKeys);
                    break;
            }

            break;

        case 0x47A6:
            shell_write("GAPConnectionEventKeysReceivedIndication");
            FLib_MemSet(face_PeerAddress_Bond,'\0',6);
            FLib_MemCpy(face_PeerAddress_Bond,container->Data.GAPConnectionEventKeysReceivedIndication.Keys.AddressInfo.DeviceAddress,6);
            face_PeerAdd_type = container->Data.GAPConnectionEventKeysReceivedIndication.Keys.AddressInfo.DeviceAddressType;
            break;

        case 0x47A7:
            shell_write("GAPConnectionEventLongTermKeyRequestIndication");
            BleApp_LTKreq(container->Data.GAPConnectionEventLongTermKeyRequestIndication);
            break;

        case 0x47A8:
            shell_write("GAPConnectionEventEncryptionChangedIndication");
            break;

        case 0x47A9:
            shell_write("GAPConnectionEventPairingCompleteIndication");
            shell_write(" -> ");

            switch (container->Data.GAPConnectionEventPairingCompleteIndication.PairingStatus)
            {
                case GAPConnectionEventPairingCompleteIndication_PairingStatus_PairingSuccessful:
                	if(container->Data.GAPConnectionEventPairingCompleteIndication.PairingData.PairingSuccessful_WithBonding)
                	{
                		shell_write("Bonding successful");
                	}
                    shell_write(PairingSuccessful);
                    BleApp_Bond();
                    break;

                case GAPConnectionEventPairingCompleteIndication_PairingStatus_PairingFailed:
                    shell_write(PairingFailed);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GAPConnectionEventPairingCompleteIndication.PairingStatus);
                    break;
            }

            break;

        case 0x47AA:
            shell_write("GAPConnectionEventDisconnectedIndication");
            shell_write(" -> ");

            switch (container->Data.GAPConnectionEventDisconnectedIndication.Reason)
            {
                case GAPConnectionEventDisconnectedIndication_Reason_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gBleOsError_c:
                    shell_write(gBleOsError_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedError_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedError_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gHciTransportError_c:
                    shell_write(gHciTransportError_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gL2caError_c:
                    shell_write(gL2caError_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationc);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GAPConnectionEventDisconnectedIndication_Reason_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownc);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GAPConnectionEventDisconnectedIndication_Reason_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GAPConnectionEventDisconnectedIndication.Reason);
                    break;
            }

            gPeerDeviceId = gInvalidDeviceId_c;

            if (gHrsDemoState == gHrsDemoConnectionEstablished_c)
            {
                gSuppressBleEventPrint = FALSE;
                gHrsDemoState = gHrsDemoStopSendMeasurements;
                BleApp_DemoHrs();
            }

            if (gOtapState == gOtapConnectionEstablished_c)
            {
                gOtapState = gOtapWaitingConnection_c;
                gSuppressBleEventPrint = FALSE;
                BleApp_DemoOtap();
            }

            if (gWUartState == gWUartConnectionEstablished_c)
            {
                gWUartState = gWUartWaitingConnection_c;
                gSuppressBleEventPrint = FALSE;
                GAPStartAdvertisingRequest(BLE_FSCI_IF);
                g_cam_stop = TRUE;
            }

            break;

        case 0x47AB:
            shell_write("GAPConnectionEventRssiReadIndication");
            break;

        case 0x47AC:
            shell_write("GAPConnectionEventTxPowerLevelReadIndication");
            break;

        case 0x47AD:
            shell_write("GAPConnectionEventPowerReadFailureIndication");
            shell_write(" -> ");

            switch (container->Data.GAPConnectionEventPowerReadFailureIndication.FailReason)
            {
                case GAPConnectionEventPowerReadFailureIndication_FailReason_gBleSuccess_c:
                    shell_write(gBleSuccess_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gBleInvalidParameter_c:
                    shell_write(gBleInvalidParameter_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gBleOverflow_c:
                    shell_write(gBleOverflow_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gBleUnavailable_c:
                    shell_write(gBleUnavailable_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gBleFeatureNotSupported_c:
                    shell_write(gBleFeatureNotSupported_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gBleOutOfMemory_c:
                    shell_write(gBleOutOfMemory_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gBleAlreadyInitialized_c:
                    shell_write(gBleAlreadyInitialized_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gBleOsError_c:
                    shell_write(gBleOsError_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gBleUnexpectedError_c:
                    shell_write(gBleUnexpectedError_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gBleInvalidState_c:
                    shell_write(gBleInvalidState_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciUnknownHciCommand_c:
                    shell_write(gHciUnknownHciCommand_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciUnknownConnectionIdentifier_c:
                    shell_write(gHciUnknownConnectionIdentifier_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciHardwareFailure_c:
                    shell_write(gHciHardwareFailure_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciPageTimeout_c:
                    shell_write(gHciPageTimeout_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciAuthenticationFailure_c:
                    shell_write(gHciAuthenticationFailure_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciPinOrKeyMissing_c:
                    shell_write(gHciPinOrKeyMissing_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciMemoryCapacityExceeded_c:
                    shell_write(gHciMemoryCapacityExceeded_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciConnectionTimeout_c:
                    shell_write(gHciConnectionTimeout_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciConnectionLimitExceeded_c:
                    shell_write(gHciConnectionLimitExceeded_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciSynchronousConnectionLimitToADeviceExceeded_c:
                    shell_write(gHciSynchronousConnectionLimitToADeviceExceeded_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciAclConnectionAlreadyExists_c:
                    shell_write(gHciAclConnectionAlreadyExists_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciCommandDisallowed_c:
                    shell_write(gHciCommandDisallowed_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciConnectionRejectedDueToLimitedResources_c:
                    shell_write(gHciConnectionRejectedDueToLimitedResources_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciConnectionRejectedDueToSecurityReasons_c:
                    shell_write(gHciConnectionRejectedDueToSecurityReasons_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciConnectionRejectedDueToUnacceptableBdAddr_c:
                    shell_write(gHciConnectionRejectedDueToUnacceptableBdAddr_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciConnectionAcceptTimeoutExceeded_c:
                    shell_write(gHciConnectionAcceptTimeoutExceeded_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciUnsupportedFeatureOrParameterValue_c:
                    shell_write(gHciUnsupportedFeatureOrParameterValue_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciInvalidHciCommandParameters_c:
                    shell_write(gHciInvalidHciCommandParameters_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciRemoteUserTerminatedConnection_c:
                    shell_write(gHciRemoteUserTerminatedConnection_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciRemoteDeviceTerminatedConnectionLowResources_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionLowResources_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciRemoteDeviceTerminatedConnectionPowerOff_c:
                    shell_write(gHciRemoteDeviceTerminatedConnectionPowerOff_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciConnectionTerminatedByLocalHost_c:
                    shell_write(gHciConnectionTerminatedByLocalHost_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciRepeatedAttempts_c:
                    shell_write(gHciRepeatedAttempts_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciPairingNotAllowed_c:
                    shell_write(gHciPairingNotAllowed_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciUnknownLpmPdu_c:
                    shell_write(gHciUnknownLpmPdu_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciUnsupportedRemoteFeature_c:
                    shell_write(gHciUnsupportedRemoteFeature_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciScoOffsetRejected_c:
                    shell_write(gHciScoOffsetRejected_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciScoIntervalRejected_c:
                    shell_write(gHciScoIntervalRejected_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciScoAirModeRejected_c:
                    shell_write(gHciScoAirModeRejected_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciInvalidLpmParameters_c:
                    shell_write(gHciInvalidLpmParameters_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciUnspecifiedError_c:
                    shell_write(gHciUnspecifiedError_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciUnsupportedLpmParameterValue_c:
                    shell_write(gHciUnsupportedLpmParameterValue_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciRoleChangeNotAllowed_c:
                    shell_write(gHciRoleChangeNotAllowed_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciLLResponseTimeout_c:
                    shell_write(gHciLLResponseTimeout_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciLmpErrorTransactionCollision_c:
                    shell_write(gHciLmpErrorTransactionCollision_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciLmpPduNotAllowed_c:
                    shell_write(gHciLmpPduNotAllowed_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciEncryptionModeNotAcceptable_c:
                    shell_write(gHciEncryptionModeNotAcceptable_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciLinkKeyCannotBeChanged_c:
                    shell_write(gHciLinkKeyCannotBeChanged_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciRequestedQosNotSupported_c:
                    shell_write(gHciRequestedQosNotSupported_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciInstantPassed_c:
                    shell_write(gHciInstantPassed_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciPairingWithUnitKeyNotSupported_c:
                    shell_write(gHciPairingWithUnitKeyNotSupported_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciDifferentTransactionCollision_c:
                    shell_write(gHciDifferentTransactionCollision_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciReserved_0x2B_c:
                    shell_write(gHciReserved_0x2B_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciQosNotAcceptableParameter_c:
                    shell_write(gHciQosNotAcceptableParameter_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciQosRejected_c:
                    shell_write(gHciQosRejected_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciChannelClassificationNotSupported_c:
                    shell_write(gHciChannelClassificationNotSupported_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciInsufficientSecurity_c:
                    shell_write(gHciInsufficientSecurity_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciParameterOutOfMandatoryRange_c:
                    shell_write(gHciParameterOutOfMandatoryRange_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciReserved_0x31_c:
                    shell_write(gHciReserved_0x31_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciRoleSwitchPending_c:
                    shell_write(gHciRoleSwitchPending_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciReserved_0x33_c:
                    shell_write(gHciReserved_0x33_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciReservedSlotViolation_c:
                    shell_write(gHciReservedSlotViolation_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciRoleSwitchFailed_c:
                    shell_write(gHciRoleSwitchFailed_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciExtendedInquiryResponseTooLarge_c:
                    shell_write(gHciExtendedInquiryResponseTooLarge_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciSecureSimplePairingNotSupportedByHost_c:
                    shell_write(gHciSecureSimplePairingNotSupportedByHost_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciHostBusyPairing_c:
                    shell_write(gHciHostBusyPairing_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciConnectionRejectedDueToNoSuitableChannelFound_c:
                    shell_write(gHciConnectionRejectedDueToNoSuitableChannelFound_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciControllerBusy_c:
                    shell_write(gHciControllerBusy_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciUnacceptableConnectionParameters_c:
                    shell_write(gHciUnacceptableConnectionParameters_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciDirectedAdvertisingTimeout_c:
                    shell_write(gHciDirectedAdvertisingTimeout_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciConnectionTerminatedDueToMicFailure_c:
                    shell_write(gHciConnectionTerminatedDueToMicFailure_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciConnectionFailedToBeEstablished_c:
                    shell_write(gHciConnectionFailedToBeEstablished_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciMacConnectionFailed_c:
                    shell_write(gHciMacConnectionFailed_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciCoarseClockAdjustmentRejected_c:
                    shell_write(gHciCoarseClockAdjustmentRejected_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciAlreadyInit_c:
                    shell_write(gHciAlreadyInit_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciInvalidParameter_c:
                    shell_write(gHciInvalidParameter_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciCallbackNotInstalled_c:
                    shell_write(gHciCallbackNotInstalled_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciCallbackAlreadyInstalled_c:
                    shell_write(gHciCallbackAlreadyInstalled_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciCommandNotSupported_c:
                    shell_write(gHciCommandNotSupported_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciEventNotSupported_c:
                    shell_write(gHciEventNotSupported_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gHciTransportError_c:
                    shell_write(gHciTransportError_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caAlreadyInit_c:
                    shell_write(gL2caAlreadyInit_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caInsufficientResources_c:
                    shell_write(gL2caInsufficientResources_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caCallbackNotInstalled_c:
                    shell_write(gL2caCallbackNotInstalled_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caCallbackAlreadyInstalled_c:
                    shell_write(gL2caCallbackAlreadyInstalled_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caLePsmInvalid_c:
                    shell_write(gL2caLePsmInvalid_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caLePsmAlreadyRegistered_c:
                    shell_write(gL2caLePsmAlreadyRegistered_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caLePsmNotRegistered_c:
                    shell_write(gL2caLePsmNotRegistered_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caLePsmInsufficientResources_c:
                    shell_write(gL2caLePsmInsufficientResources_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caChannelInvalid_c:
                    shell_write(gL2caChannelInvalid_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caChannelClosed_c:
                    shell_write(gL2caChannelClosed_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caChannelAlreadyConnected_c:
                    shell_write(gL2caChannelAlreadyConnected_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caConnectionParametersRejected_c:
                    shell_write(gL2caConnectionParametersRejected_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caChannelBusy_c:
                    shell_write(gL2caChannelBusy_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caInvalidParameter_c:
                    shell_write(gL2caInvalidParameter_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gL2caError_c:
                    shell_write(gL2caError_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmNullCBFunction_c:
                    shell_write(gSmNullCBFunction_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmCommandNotSupported_c:
                    shell_write(gSmCommandNotSupported_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmUnexpectedCommand_c:
                    shell_write(gSmUnexpectedCommand_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmInvalidCommandCode_c:
                    shell_write(gSmInvalidCommandCode_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmInvalidCommandLength_c:
                    shell_write(gSmInvalidCommandLength_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmInvalidCommandParameter_c:
                    shell_write(gSmInvalidCommandParameter_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmInvalidDeviceId_c:
                    shell_write(gSmInvalidDeviceId_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmInvalidInternalOperation_c:
                    shell_write(gSmInvalidInternalOperation_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmInvalidConnectionHandle_c:
                    shell_write(gSmInvalidConnectionHandle_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmInproperKeyDistributionField_c:
                    shell_write(gSmInproperKeyDistributionField_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmUnexpectedKeyType_c:
                    shell_write(gSmUnexpectedKeyType_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmUnexpectedPairingTerminationReason_c:
                    shell_write(gSmUnexpectedPairingTerminationReason_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmUnexpectedKeyset_c:
                    shell_write(gSmUnexpectedKeyset_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmSmpTimeoutOccurred_c:
                    shell_write(gSmSmpTimeoutOccurred_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmUnknownSmpPacketType_c:
                    shell_write(gSmUnknownSmpPacketType_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmInvalidSmpPacketLength_c:
                    shell_write(gSmInvalidSmpPacketLength_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmInvalidSmpPacketParameter_c:
                    shell_write(gSmInvalidSmpPacketParameter_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmReceivedUnexpectedSmpPacket_c:
                    shell_write(gSmReceivedUnexpectedSmpPacket_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmReceivedSmpPacketFromUnknownDevice_c:
                    shell_write(gSmReceivedSmpPacketFromUnknownDevice_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmReceivedUnexpectedHciEvent_c:
                    shell_write(gSmReceivedUnexpectedHciEvent_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmReceivedHciEventFromUnknownDevice_c:
                    shell_write(gSmReceivedHciEventFromUnknownDevice_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmInvalidHciEventParameter_c:
                    shell_write(gSmInvalidHciEventParameter_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmLlConnectionEncryptionInProgress_c:
                    shell_write(gSmLlConnectionEncryptionInProgress_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmLlConnectionEncryptionFailure_c:
                    shell_write(gSmLlConnectionEncryptionFailure_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmInsufficientResources_c:
                    shell_write(gSmInsufficientResources_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmOobDataAddressMismatch_c:
                    shell_write(gSmOobDataAddressMismatch_c);
                    break;

//                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmSmpPacketReceivedAfterTimeoutOccurred_c:
//                    shell_write(gSmSmpPacketReceivedAfterTimeoutOccurred_c);
//                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmPairingErrorPasskeyEntryFailed_c:
                    shell_write(gSmPairingErrorPasskeyEntryFailed_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmPairingErrorConfirmValueFailed_c:
                    shell_write(gSmPairingErrorConfirmValueFailed_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmPairingErrorCommandNotSupported_c:
                    shell_write(gSmPairingErrorCommandNotSupported_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmPairingErrorInvalidParameters_c:
                    shell_write(gSmPairingErrorInvalidParameters_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmPairingErrorUnknownReason_c:
                    shell_write(gSmPairingErrorUnknownReason_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmTbResolvableAddressDoesNotMatchIrk_c:
                    shell_write(gSmTbResolvableAddressDoesNotMatchIrk_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gSmTbInvalidDataSignature_c:
                    shell_write(gSmTbInvalidDataSignature_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttInvalidHandle_c:
                    shell_write(gAttInvalidHandle_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttReadNotPermitted_c:
                    shell_write(gAttReadNotPermitted_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttWriteNotPermitted_c:
                    shell_write(gAttWriteNotPermitted_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttInvalidPdu_c:
                    shell_write(gAttInvalidPdu_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttInsufficientAuthentication_c:
                    shell_write(gAttInsufficientAuthentication_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttRequestNotSupported_c:
                    shell_write(gAttRequestNotSupported_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttInvalidOffset_c:
                    shell_write(gAttInvalidOffset_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttInsufficientAuthorization_c:
                    shell_write(gAttInsufficientAuthorization_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttPrepareQueueFull_c:
                    shell_write(gAttPrepareQueueFull_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttAttributeNotFound_c:
                    shell_write(gAttAttributeNotFound_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttAttributeNotLong_c:
                    shell_write(gAttAttributeNotLong_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttInsufficientEncryptionKeySize_c:
                    shell_write(gAttInsufficientEncryptionKeySize_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttInvalidAttributeValueLength_c:
                    shell_write(gAttInvalidAttributeValueLength_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttUnlikelyor_c:
                    shell_write(gAttUnlikelyor_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttInsufficientEncryption_c:
                    shell_write(gAttInsufficientEncryption_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttUnsupportedGroupType_c:
                    shell_write(gAttUnsupportedGroupType_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gAttInsufficientResources_c:
                    shell_write(gAttInsufficientResources_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGattAnotherProcedureInProgress_c:
                    shell_write(gGattAnotherProcedureInProgress_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGattLongAttributePacketsCorrupted_c:
                    shell_write(gGattLongAttributePacketsCorrupted_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGattMultipleAttributesOverflow_c:
                    shell_write(gGattMultipleAttributesOverflow_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGattUnexpectedReadMultipleResponseLength_c:
                    shell_write(gGattUnexpectedReadMultipleResponseLength_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGattInvalidValueLength_c:
                    shell_write(gGattInvalidValueLength_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGattServerTimeout_c:
                    shell_write(gGattServerTimeout_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGattIndicationAlreadyInProgress_c:
                    shell_write(gGattIndicationAlreadyInProgress_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGattClientConfirmationTimeout_c:
                    shell_write(gGattClientConfirmationTimeout_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGapAdvDataTooLong_c:
                    shell_write(gGapAdvDataTooLong_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGapScanRspDataTooLong_c:
                    shell_write(gGapScanRspDataTooLong_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGapDeviceNotBonded_c:
                    shell_write(gGapDeviceNotBonded_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gDevDbCccdLimitReached_c:
                    shell_write(gDevDbCccdLimitReached_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gDevDbCccdNotFound_c:
                    shell_write(gDevDbCccdNotFound_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGattDbInvalidHandle_c:
                    shell_write(gGattDbInvalidHandle_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGattDbCharacteristicNotFound_c:
                    shell_write(gGattDbCharacteristicNotFound_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGattDbCccdNotFound_c:
                    shell_write(gGattDbCccdNotFound_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGattDbServiceNotFound_c:
                    shell_write(gGattDbServiceNotFound_c);
                    break;

                case GAPConnectionEventPowerReadFailureIndication_FailReason_gGattDbDescriptorNotFound_c:
                    shell_write(gGattDbDescriptorNotFound_c);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.GAPConnectionEventPowerReadFailureIndication.FailReason);
                    break;
            }

            break;

        case 0x47AE:
            shell_write("GAPConnectionEventParameterUpdateRequestIndication");
            break;

        case 0x47AF:
            shell_write("GAPConnectionEventParameterUpdateCompleteIndication");
            gSuppressBleEventPrint = TRUE;
            break;

        case 0x47B0:
            shell_write("GAPConnectionEventLeDataLengthChangedIndication");
            break;

        case 0x47B1:
            shell_write("GAPConnectionEventLeScOobDataRequestIndication");
            break;

        case 0x47B2:
            shell_write("GAPConnectionEventLeScDisplayNumericValueIndication");
            break;

        case 0x47B3:
            shell_write("GAPConnectionEventLeScKeypressNotificationIndication");
            shell_write(" -> ");

            switch (container->Data.GAPConnectionEventLeScKeypressNotificationIndication.GapLeScKeypressNotificationParams_keypressNotifType)
            {
                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.GAPConnectionEventLeScKeypressNotificationIndication.GapLeScKeypressNotificationParams_keypressNotifType);
                    break;
            }

            break;

        case 0x47B4:
            shell_write("GAPGenericEventControllerResetCompleteIndication");
            break;

        case 0x47B5:
            shell_write("GAPLeScPublicKeyRegeneratedIndication");
            break;

        case 0x47B6:
            shell_write("GAPGenericEventLeScLocalOobDataIndication");
            break;

        case 0x47B7:
            shell_write("GAPGenericEventControllerPrivacyStateChangedIndication");
            break;

//        case 0x4783:
//            shell_write("GAPGetBondedDevicesIdentityInformationIndication");
//            break;

#endif  /* GAP_ENABLE */

    }

    if (!gSuppressBleEventPrint)
    {
        shell_refresh();
    }
}
