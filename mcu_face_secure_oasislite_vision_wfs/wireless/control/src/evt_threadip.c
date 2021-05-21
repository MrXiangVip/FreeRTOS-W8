/* Source file generated from ThreadIP.xml */

/*==================================================================================================
Include Files
==================================================================================================*/
#include "cmd_threadip.h"
#include "fsl_log.h"

/*==================================================================================================
Private Prototypes
==================================================================================================*/
#if SOCKET_ENABLE
static memStatus_t Load_SocketCreateConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_SocketShutdownConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_SocketBindConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_SocketSendConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_SocketSendToConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_SocketReceiveConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_SocketReceiveFromConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_SocketConnectConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_SocketListenConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_SocketAcceptConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_SocketSetOptionConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_SocketGetOptionConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
#endif  /* SOCKET_ENABLE */

#if MAC_ENABLE
static memStatus_t Load_MAC_MacFilteringAddEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MAC_MacFilteringRemoveEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MAC_MacFilteringEnableConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MAC_MacFilteringGetTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
#endif  /* MAC_ENABLE */

#if THREADMGMT_ENABLE
static memStatus_t Load_THR_SetDeviceConfigConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_NwkScanConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_CreateNwkConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_JoinConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_FactoryResetConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_CpuResetConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_CpuResetIndication(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_DisconnectConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_AttachConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_PromoteAsRouterConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_EventNwkScanConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_EventNwkCreateConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_EventNwkJoinConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_EventNwkJoinSelectParentsConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_EventGeneralConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_EventNwkCommissioningIndication(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_CommissioningDiagnosticIndication(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_MgmtDiagnosticGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_DiagTestGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_MgmtDiagnosticResetConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_MgmtReadMemoryConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_MgmtWriteMemoryConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_SetManualSlaacIIDConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_SendProactiveAddrNotifConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_NwkDiscoveryConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_NwkDiscoveryStopConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_SearchNwkWithAnounceConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_MgmtDiagnosticGetRspIndication(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_DiagTestGetRspIndication(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_MgmtDiagnosticResetRspIndication(thrEvtContainer_t *container, uint8_t *pPayload);
#endif  /* THREADMGMT_ENABLE */

#if THREADUTILS_ENABLE
static memStatus_t Load_THR_SetNwkIdTimeoutConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_SetThresholdConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_GetNeighborInfoConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_GetChildrenTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_GetNeighborTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_GetRoutingTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_GetAttrConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_SetAttrConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_GetThreadIpAddrConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_GetParentConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_ChildUpdateToParentConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_LeaderRemoveRouterIdConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_GenerateAllKeysConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_SwitchKeyKeyConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_BrPrefixAddEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_BrPrefixGetTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_BrPrefixRemoveEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_BrServiceRemoveEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_BrServiceAddEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_BrPrefixSyncConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_BrPrefixRemoveAllConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_THR_IdentifyConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
#endif  /* THREADUTILS_ENABLE */

#if IPUTILS_ENABLE
static memStatus_t Load_NWKU_IfconfigBindConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_NWKU_IfconfigAllResponse(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_NWKU_PingConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_NWKU_EchoUDPConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_NWKU_CoapMsgReceivedIndication(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_NWKU_DnsResponseReceivedIndication(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_NWKU_CoapRegisterConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_NWKU_CoapCreateInstanceConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_NWKU_CoapSendConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_NWKU_DnsSendRequestConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_NWKU_McastGroupManageConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_NWKU_McastGroupShowResponse(thrEvtContainer_t *container, uint8_t *pPayload);
#endif  /* IPUTILS_ENABLE */

#if MESHCOP_ENABLE
static memStatus_t Load_MESHCOP_StartCommissionerConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_StartNativeCommissionerConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_StopCommissionerConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_AddExpectedJoinerConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_GetExpectedJoinerConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_RemoveExpectedJoinerConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_RemoveAllExpectedJoinersConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_SyncSteeringDataConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmtSetConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmtGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_SetCommissionerCredentialConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmNwkFormConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmtCommissionerGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmtActiveGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmtPendingGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmtCommissionerSetConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmtActiveSetConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmtPendingSetConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmtSendPanIdQueryConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmtPanIdConflictConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmtSendEdScanConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmtEdReportConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MESHCOP_MgmtSendAnnounceBeginConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
#endif  /* MESHCOP_ENABLE */

#if DTLS_ENABLE
static memStatus_t Load_DTLSOpenConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_DTLSCloseConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_DTLSClosePeerConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_DTLSConnectConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_DTLSClientConnectedConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_DTLSSendConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_DTLSReceiveConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
#endif  /* DTLS_ENABLE */

#if OTHERUTILS_ENABLE
static memStatus_t Load_FSCIGetUniqueIdConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_FSCIGetMcuIdConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_FSCIGetSwVersionsConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_Sniffer_MacSetPIBAttributeConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_MAC_PromiscuousRxIndication(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_SerialTun_IPPacketReceivedConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_AspSetPowerLevelConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_AspGetPowerLevelConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
static memStatus_t Load_DBGConfirm(thrEvtContainer_t *container, uint8_t *pPayload);
#endif  /* OTHERUTILS_ENABLE */

/*==================================================================================================
Private global variables declarations
==================================================================================================*/
static const thrEvtHandler_t evtHandlerTbl[] =
{
#if SOCKET_ENABLE
	{0xCF00, Load_SocketCreateConfirm},
	{0xCF01, Load_SocketShutdownConfirm},
	{0xCF02, Load_SocketBindConfirm},
	{0xCF03, Load_SocketSendConfirm},
	{0xCF04, Load_SocketSendToConfirm},
	{0xCF05, Load_SocketReceiveConfirm},
	{0xCF06, Load_SocketReceiveFromConfirm},
	{0xCF07, Load_SocketConnectConfirm},
	{0xCF08, Load_SocketListenConfirm},
	{0xCF09, Load_SocketAcceptConfirm},
	{0xCF0A, Load_SocketSetOptionConfirm},
	{0xCF0B, Load_SocketGetOptionConfirm},
#endif  /* SOCKET_ENABLE */

#if MAC_ENABLE
	{0xCF12, Load_MAC_MacFilteringAddEntryConfirm},
	{0xCF13, Load_MAC_MacFilteringRemoveEntryConfirm},
	{0xCF14, Load_MAC_MacFilteringEnableConfirm},
	{0xCF15, Load_MAC_MacFilteringGetTableConfirm},
#endif  /* MAC_ENABLE */

#if THREADMGMT_ENABLE
	{0xCF16, Load_THR_SetDeviceConfigConfirm},
	{0xCF1A, Load_THR_NwkScanConfirm},
	{0xCF1B, Load_THR_CreateNwkConfirm},
	{0xCF1C, Load_THR_JoinConfirm},
	{0xCF1F, Load_THR_FactoryResetConfirm},
	{0xCF21, Load_THR_CpuResetConfirm},
	{0xCF22, Load_THR_CpuResetIndication},
	{0xCF1D, Load_THR_DisconnectConfirm},
	{0xCF80, Load_THR_AttachConfirm},
	{0xCF94, Load_THR_PromoteAsRouterConfirm},
	{0xCF50, Load_THR_EventNwkScanConfirm},
	{0xCF51, Load_THR_EventNwkCreateConfirm},
	{0xCF52, Load_THR_EventNwkJoinConfirm},
	{0xCF53, Load_THR_EventNwkJoinSelectParentsConfirm},
	{0xCF54, Load_THR_EventGeneralConfirm},
	{0xCF55, Load_THR_EventNwkCommissioningIndication},
	{0xCF4E, Load_THR_CommissioningDiagnosticIndication},
	{0xCF61, Load_THR_MgmtDiagnosticGetConfirm},
	{0xCF67, Load_THR_DiagTestGetConfirm},
	{0xCF62, Load_THR_MgmtDiagnosticResetConfirm},
	{0xCF65, Load_THR_MgmtReadMemoryConfirm},
	{0xCF66, Load_THR_MgmtWriteMemoryConfirm},
	{0xCF75, Load_THR_SetManualSlaacIIDConfirm},
	{0xCF76, Load_THR_SendProactiveAddrNotifConfirm},
	{0xCFC0, Load_THR_NwkDiscoveryConfirm},
	{0xCFC1, Load_THR_NwkDiscoveryStopConfirm},
	{0xCFC2, Load_THR_SearchNwkWithAnounceConfirm},
	{0xCF63, Load_THR_MgmtDiagnosticGetRspIndication},
	{0xCF68, Load_THR_DiagTestGetRspIndication},
	{0xCF64, Load_THR_MgmtDiagnosticResetRspIndication},
#endif  /* THREADMGMT_ENABLE */

#if THREADUTILS_ENABLE
	{0xCF2E, Load_THR_SetNwkIdTimeoutConfirm},
	{0xCF20, Load_THR_SetThresholdConfirm},
	{0xCF10, Load_THR_GetNeighborInfoConfirm},
	{0xCF23, Load_THR_GetChildrenTableConfirm},
	{0xCF24, Load_THR_GetNeighborTableConfirm},
	{0xCF25, Load_THR_GetRoutingTableConfirm},
	{0xCF17, Load_THR_GetAttrConfirm},
	{0xCF18, Load_THR_SetAttrConfirm},
	{0xCF19, Load_THR_GetThreadIpAddrConfirm},
	{0xCF1E, Load_THR_GetParentConfirm},
	{0xCF2F, Load_THR_ChildUpdateToParentConfirm},
	{0xCF26, Load_THR_LeaderRemoveRouterIdConfirm},
	{0xCF28, Load_THR_GenerateAllKeysConfirm},
	{0xCF27, Load_THR_SwitchKeyKeyConfirm},
	{0xCF29, Load_THR_BrPrefixAddEntryConfirm},
	{0xCF2A, Load_THR_BrPrefixGetTableConfirm},
	{0xCF2B, Load_THR_BrPrefixRemoveEntryConfirm},
	{0xCF86, Load_THR_BrServiceRemoveEntryConfirm},
	{0xCF85, Load_THR_BrServiceAddEntryConfirm},
	{0xCF2C, Load_THR_BrPrefixSyncConfirm},
	{0xCF2D, Load_THR_BrPrefixRemoveAllConfirm},
	{0xCF69, Load_THR_IdentifyConfirm},
#endif  /* THREADUTILS_ENABLE */

#if IPUTILS_ENABLE
	{0xCF0C, Load_NWKU_IfconfigBindConfirm},
	{0xCF0D, Load_NWKU_IfconfigAllResponse},
	{0xCF0E, Load_NWKU_PingConfirm},
	{0xCF70, Load_NWKU_EchoUDPConfirm},
	{0xCF92, Load_NWKU_CoapMsgReceivedIndication},
	{0xCF96, Load_NWKU_DnsResponseReceivedIndication},
	{0xCF91, Load_NWKU_CoapRegisterConfirm},
	{0xCF93, Load_NWKU_CoapCreateInstanceConfirm},
	{0xCF90, Load_NWKU_CoapSendConfirm},
	{0xCF95, Load_NWKU_DnsSendRequestConfirm},
	{0xCF71, Load_NWKU_McastGroupManageConfirm},
	{0xCF72, Load_NWKU_McastGroupShowResponse},
#endif  /* IPUTILS_ENABLE */

#if MESHCOP_ENABLE
	{0xCF40, Load_MESHCOP_StartCommissionerConfirm},
	{0xCF4F, Load_MESHCOP_StartNativeCommissionerConfirm},
	{0xCF41, Load_MESHCOP_StopCommissionerConfirm},
	{0xCF42, Load_MESHCOP_AddExpectedJoinerConfirm},
	{0xCF43, Load_MESHCOP_GetExpectedJoinerConfirm},
	{0xCF44, Load_MESHCOP_RemoveExpectedJoinerConfirm},
	{0xCF45, Load_MESHCOP_RemoveAllExpectedJoinersConfirm},
	{0xCF46, Load_MESHCOP_SyncSteeringDataConfirm},
	{0xCF4B, Load_MESHCOP_MgmtSetConfirm},
	{0xCF4C, Load_MESHCOP_MgmtGetConfirm},
	{0xCF4D, Load_MESHCOP_SetCommissionerCredentialConfirm},
	{0xCF47, Load_MESHCOP_MgmNwkFormConfirm},
	{0xCFA0, Load_MESHCOP_MgmtCommissionerGetConfirm},
	{0xCFA2, Load_MESHCOP_MgmtActiveGetConfirm},
	{0xCFA4, Load_MESHCOP_MgmtPendingGetConfirm},
	{0xCFA1, Load_MESHCOP_MgmtCommissionerSetConfirm},
	{0xCFA3, Load_MESHCOP_MgmtActiveSetConfirm},
	{0xCFA5, Load_MESHCOP_MgmtPendingSetConfirm},
	{0xCFA8, Load_MESHCOP_MgmtSendPanIdQueryConfirm},
	{0xCFA9, Load_MESHCOP_MgmtPanIdConflictConfirm},
	{0xCFAA, Load_MESHCOP_MgmtSendEdScanConfirm},
	{0xCFAB, Load_MESHCOP_MgmtEdReportConfirm},
	{0xCFA7, Load_MESHCOP_MgmtSendAnnounceBeginConfirm},
#endif  /* MESHCOP_ENABLE */

#if DTLS_ENABLE
	{0xCF30, Load_DTLSOpenConfirm},
	{0xCF31, Load_DTLSCloseConfirm},
	{0xCF32, Load_DTLSClosePeerConfirm},
	{0xCF33, Load_DTLSConnectConfirm},
	{0xCF34, Load_DTLSClientConnectedConfirm},
	{0xCF35, Load_DTLSSendConfirm},
	{0xCF36, Load_DTLSReceiveConfirm},
#endif  /* DTLS_ENABLE */

#if OTHERUTILS_ENABLE
	{0xA4B0, Load_FSCIGetUniqueIdConfirm},
	{0xA4B1, Load_FSCIGetMcuIdConfirm},
	{0xA4B2, Load_FSCIGetSwVersionsConfirm},
	{0x840D, Load_Sniffer_MacSetPIBAttributeConfirm},
	{0x8603, Load_MAC_PromiscuousRxIndication},
	{0xCFF3, Load_SerialTun_IPPacketReceivedConfirm},
	{0x940F, Load_AspSetPowerLevelConfirm},
	{0x941F, Load_AspGetPowerLevelConfirm},
	{0xCF89, Load_DBGConfirm},
#endif  /* OTHERUTILS_ENABLE */
};

/*==================================================================================================
Public Functions
==================================================================================================*/
#if SOCKET_ENABLE
/*!*************************************************************************************************
\fn		static memStatus_t Load_SocketCreateConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the socket create request.
***************************************************************************************************/
static memStatus_t Load_SocketCreateConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	SocketCreateConfirm_t *evt = &(container->Data.SocketCreateConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF00;

	FLib_MemCpy(evt, pPayload, sizeof(SocketCreateConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_SocketShutdownConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the socket shutdown request.
***************************************************************************************************/
static memStatus_t Load_SocketShutdownConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	SocketShutdownConfirm_t *evt = &(container->Data.SocketShutdownConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF01;

	FLib_MemCpy(evt, pPayload, sizeof(SocketShutdownConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_SocketBindConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the socket bind request.
***************************************************************************************************/
static memStatus_t Load_SocketBindConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	SocketBindConfirm_t *evt = &(container->Data.SocketBindConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF02;

	FLib_MemCpy(evt, pPayload, sizeof(SocketBindConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_SocketSendConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the socket send request.
***************************************************************************************************/
static memStatus_t Load_SocketSendConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	SocketSendConfirm_t *evt = &(container->Data.SocketSendConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF03;

	FLib_MemCpy(evt, pPayload, sizeof(SocketSendConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_SocketSendToConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the socket send request.
***************************************************************************************************/
static memStatus_t Load_SocketSendToConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	SocketSendToConfirm_t *evt = &(container->Data.SocketSendToConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF04;

	FLib_MemCpy(evt, pPayload, sizeof(SocketSendToConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_SocketReceiveConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the socket receive request.
***************************************************************************************************/
static memStatus_t Load_SocketReceiveConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	SocketReceiveConfirm_t *evt = &(container->Data.SocketReceiveConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF05;

	FLib_MemCpy(&(evt->Size), pPayload + idx, sizeof(evt->Size)); idx += sizeof(evt->Size);

	if (evt->Size > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->Size);

		if (!evt->Data)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->Size); idx += evt->Size;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_SocketReceiveFromConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the socket receive request.
***************************************************************************************************/
static memStatus_t Load_SocketReceiveFromConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	SocketReceiveFromConfirm_t *evt = &(container->Data.SocketReceiveFromConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF06;

	evt->Status = (SocketReceiveFromConfirm_Status_t)pPayload[idx]; idx++;
	FLib_MemCpy(evt->RemoteIpAddress, pPayload + idx, sizeof(evt->RemoteIpAddress)); idx += sizeof(evt->RemoteIpAddress);
	FLib_MemCpy(&(evt->RemotePort), pPayload + idx, sizeof(evt->RemotePort)); idx += sizeof(evt->RemotePort);
	FLib_MemCpy(&(evt->Size), pPayload + idx, sizeof(evt->Size)); idx += sizeof(evt->Size);

	if (evt->Size > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->Size);

		if (!evt->Data)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->Size); idx += evt->Size;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_SocketConnectConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the socket connect request.
***************************************************************************************************/
static memStatus_t Load_SocketConnectConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	SocketConnectConfirm_t *evt = &(container->Data.SocketConnectConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF07;

	FLib_MemCpy(evt, pPayload, sizeof(SocketConnectConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_SocketListenConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the socket listen request.
***************************************************************************************************/
static memStatus_t Load_SocketListenConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	SocketListenConfirm_t *evt = &(container->Data.SocketListenConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF08;

	FLib_MemCpy(evt, pPayload, sizeof(SocketListenConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_SocketAcceptConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the socket accept request.
***************************************************************************************************/
static memStatus_t Load_SocketAcceptConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	SocketAcceptConfirm_t *evt = &(container->Data.SocketAcceptConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF09;

	FLib_MemCpy(evt, pPayload, sizeof(SocketAcceptConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_SocketSetOptionConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the socket set option request.
***************************************************************************************************/
static memStatus_t Load_SocketSetOptionConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	SocketSetOptionConfirm_t *evt = &(container->Data.SocketSetOptionConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF0A;

	FLib_MemCpy(evt, pPayload, sizeof(SocketSetOptionConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_SocketGetOptionConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Get socket option.
***************************************************************************************************/
static memStatus_t Load_SocketGetOptionConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	SocketGetOptionConfirm_t *evt = &(container->Data.SocketGetOptionConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF0B;

	FLib_MemCpy(evt, pPayload, sizeof(SocketGetOptionConfirm_t));

	return MEM_SUCCESS_c;
}

#endif  /* SOCKET_ENABLE */

#if MAC_ENABLE
/*!*************************************************************************************************
\fn		static memStatus_t Load_MAC_MacFilteringAddEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the addition of a new entry in the Mac filtering List table.
***************************************************************************************************/
static memStatus_t Load_MAC_MacFilteringAddEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MAC_MacFilteringAddEntryConfirm_t *evt = &(container->Data.MAC_MacFilteringAddEntryConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF12;

	FLib_MemCpy(evt, pPayload, sizeof(MAC_MacFilteringAddEntryConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MAC_MacFilteringRemoveEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Remove Mac filtering list entry confirmation.
***************************************************************************************************/
static memStatus_t Load_MAC_MacFilteringRemoveEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MAC_MacFilteringRemoveEntryConfirm_t *evt = &(container->Data.MAC_MacFilteringRemoveEntryConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF13;

	FLib_MemCpy(evt, pPayload, sizeof(MAC_MacFilteringRemoveEntryConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MAC_MacFilteringEnableConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Enable Mac filtering list table confirmation.
***************************************************************************************************/
static memStatus_t Load_MAC_MacFilteringEnableConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MAC_MacFilteringEnableConfirm_t *evt = &(container->Data.MAC_MacFilteringEnableConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF14;

	FLib_MemCpy(evt, pPayload, sizeof(MAC_MacFilteringEnableConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MAC_MacFilteringGetTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Get Mac filtering list table response.
***************************************************************************************************/
static memStatus_t Load_MAC_MacFilteringGetTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MAC_MacFilteringGetTableConfirm_t *evt = &(container->Data.MAC_MacFilteringGetTableConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF15;

	evt->InstanceId = pPayload[idx]; idx++;
	evt->NoOfElements = pPayload[idx]; idx++;

	if (evt->NoOfElements > 0)
	{
		evt->MacFilteringEntries = MEM_BufferAlloc(evt->NoOfElements * sizeof(evt->MacFilteringEntries[0]));

		if (!evt->MacFilteringEntries)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->MacFilteringEntries = NULL;
	}


	for (uint32_t i = 0; i < evt->NoOfElements; i++)
	{
		FLib_MemCpy(&(evt->MacFilteringEntries[i].ExtendedAddress), pPayload + idx, sizeof(evt->MacFilteringEntries[i].ExtendedAddress)); idx += sizeof(evt->MacFilteringEntries[i].ExtendedAddress);
		FLib_MemCpy(&(evt->MacFilteringEntries[i].ShortAddress), pPayload + idx, sizeof(evt->MacFilteringEntries[i].ShortAddress)); idx += sizeof(evt->MacFilteringEntries[i].ShortAddress);
		evt->MacFilteringEntries[i].LinkIndicator = pPayload[idx]; idx++;
		evt->MacFilteringEntries[i].BlockedNeighbor = (bool_t)pPayload[idx]; idx++;
	}

	return MEM_SUCCESS_c;
}

#endif  /* MAC_ENABLE */

#if THREADMGMT_ENABLE
/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_SetDeviceConfigConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Set device configuration confirm
***************************************************************************************************/
static memStatus_t Load_THR_SetDeviceConfigConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_SetDeviceConfigConfirm_t *evt = &(container->Data.THR_SetDeviceConfigConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF16;

	FLib_MemCpy(evt, pPayload, sizeof(THR_SetDeviceConfigConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_NwkScanConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the scan network request.
***************************************************************************************************/
static memStatus_t Load_THR_NwkScanConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_NwkScanConfirm_t *evt = &(container->Data.THR_NwkScanConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF1A;

	FLib_MemCpy(evt, pPayload, sizeof(THR_NwkScanConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_CreateNwkConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the create network request.
***************************************************************************************************/
static memStatus_t Load_THR_CreateNwkConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_CreateNwkConfirm_t *evt = &(container->Data.THR_CreateNwkConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF1B;

	FLib_MemCpy(evt, pPayload, sizeof(THR_CreateNwkConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_JoinConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the join network request.
***************************************************************************************************/
static memStatus_t Load_THR_JoinConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_JoinConfirm_t *evt = &(container->Data.THR_JoinConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF1C;

	FLib_MemCpy(evt, pPayload, sizeof(THR_JoinConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_FactoryResetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Reset the device to factory default
***************************************************************************************************/
static memStatus_t Load_THR_FactoryResetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_FactoryResetConfirm_t *evt = &(container->Data.THR_FactoryResetConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF1F;

	FLib_MemCpy(evt, pPayload, sizeof(THR_FactoryResetConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_CpuResetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the device reset.
***************************************************************************************************/
static memStatus_t Load_THR_CpuResetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_CpuResetConfirm_t *evt = &(container->Data.THR_CpuResetConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF21;

	FLib_MemCpy(evt, pPayload, sizeof(THR_CpuResetConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_CpuResetIndication(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	THR_CPUReset indication
***************************************************************************************************/
static memStatus_t Load_THR_CpuResetIndication(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_CpuResetIndication_t *evt = &(container->Data.THR_CpuResetIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF22;

	evt->Status = (THR_CpuResetIndication_Status_t)pPayload[idx]; idx++;

	switch (evt->Status)
	{
		case THR_CpuResetIndication_Status_ResetCpuSuccess:
			evt->ResetCpuPayload.ResetCpuSuccess.BoardNameLen = pPayload[idx]; idx++;

			if (evt->ResetCpuPayload.ResetCpuSuccess.BoardNameLen > 0)
			{
				evt->ResetCpuPayload.ResetCpuSuccess.BoardName = MEM_BufferAlloc(evt->ResetCpuPayload.ResetCpuSuccess.BoardNameLen);

				if (!evt->ResetCpuPayload.ResetCpuSuccess.BoardName)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->ResetCpuPayload.ResetCpuSuccess.BoardName = NULL;
			}

			FLib_MemCpy(evt->ResetCpuPayload.ResetCpuSuccess.BoardName, pPayload + idx, evt->ResetCpuPayload.ResetCpuSuccess.BoardNameLen); idx += evt->ResetCpuPayload.ResetCpuSuccess.BoardNameLen;
			FLib_MemCpy(evt->ResetCpuPayload.ResetCpuSuccess.UniqueMcuId, pPayload + idx, sizeof(evt->ResetCpuPayload.ResetCpuSuccess.UniqueMcuId)); idx += sizeof(evt->ResetCpuPayload.ResetCpuSuccess.UniqueMcuId);
			FLib_MemCpy(&(evt->ResetCpuPayload.ResetCpuSuccess.StackVersionStruct.StackVendorOUI), pPayload + idx, 3); idx += 3;
			FLib_MemCpy(&(evt->ResetCpuPayload.ResetCpuSuccess.StackVersionStruct.StackVersion), pPayload + idx, 3); idx += 3;
			evt->ResetCpuPayload.ResetCpuSuccess.SwVersionLen = pPayload[idx]; idx++;

			if (evt->ResetCpuPayload.ResetCpuSuccess.SwVersionLen > 0)
			{
				evt->ResetCpuPayload.ResetCpuSuccess.SwVersion = MEM_BufferAlloc(evt->ResetCpuPayload.ResetCpuSuccess.SwVersionLen);

				if (!evt->ResetCpuPayload.ResetCpuSuccess.SwVersion)
				{
					MEM_BufferFree(evt->ResetCpuPayload.ResetCpuSuccess.BoardName);
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->ResetCpuPayload.ResetCpuSuccess.SwVersion = NULL;
			}

			FLib_MemCpy(evt->ResetCpuPayload.ResetCpuSuccess.SwVersion, pPayload + idx, evt->ResetCpuPayload.ResetCpuSuccess.SwVersionLen); idx += evt->ResetCpuPayload.ResetCpuSuccess.SwVersionLen;
			break;

		case THR_CpuResetIndication_Status_ResetCpuPending:
			FLib_MemCpy(&(evt->ResetCpuPayload.ResetCpuPending_TimeoutMs), pPayload + idx, sizeof(evt->ResetCpuPayload.ResetCpuPending_TimeoutMs)); idx += sizeof(evt->ResetCpuPayload.ResetCpuPending_TimeoutMs);
			break;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_DisconnectConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the THR_Disconnect.Request.
***************************************************************************************************/
static memStatus_t Load_THR_DisconnectConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_DisconnectConfirm_t *evt = &(container->Data.THR_DisconnectConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF1D;

	FLib_MemCpy(evt, pPayload, sizeof(THR_DisconnectConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_AttachConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the THR_Attach.Request.
***************************************************************************************************/
static memStatus_t Load_THR_AttachConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_AttachConfirm_t *evt = &(container->Data.THR_AttachConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF80;

	FLib_MemCpy(evt, pPayload, sizeof(THR_AttachConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_PromoteAsRouterConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the THR_PromoteAsRouter.Request
***************************************************************************************************/
static memStatus_t Load_THR_PromoteAsRouterConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_PromoteAsRouterConfirm_t *evt = &(container->Data.THR_PromoteAsRouterConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF94;

	FLib_MemCpy(evt, pPayload, sizeof(THR_PromoteAsRouterConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_EventNwkScanConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the network scan event
***************************************************************************************************/
static memStatus_t Load_THR_EventNwkScanConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_EventNwkScanConfirm_t *evt = &(container->Data.THR_EventNwkScanConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF50;

	evt->InstanceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->EventStatus), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->DataSize), pPayload + idx, sizeof(evt->DataSize)); idx += sizeof(evt->DataSize);
	FLib_MemCpy(&(evt->ScanChannelMask), pPayload + idx, sizeof(evt->ScanChannelMask)); idx += sizeof(evt->ScanChannelMask);
	evt->ScanType = (THR_EventNwkScanConfirm_ScanType_t)pPayload[idx]; idx++;
	evt->ScanDuration = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->maxThrNwkToDiscover), pPayload + idx, sizeof(evt->maxThrNwkToDiscover)); idx += sizeof(evt->maxThrNwkToDiscover);

	switch (evt->ScanType)
	{
		case THR_EventNwkScanConfirm_ScanType_EnergyDetect:
			evt->Data.EnergyDetect.EnergyDetectEntries = pPayload[idx]; idx++;

			if (evt->Data.EnergyDetect.EnergyDetectEntries > 0)
			{
				evt->Data.EnergyDetect.EnergyDetectList = MEM_BufferAlloc(evt->Data.EnergyDetect.EnergyDetectEntries);

				if (!evt->Data.EnergyDetect.EnergyDetectList)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Data.EnergyDetect.EnergyDetectList = NULL;
			}

			FLib_MemCpy(evt->Data.EnergyDetect.EnergyDetectList, pPayload + idx, evt->Data.EnergyDetect.EnergyDetectEntries); idx += evt->Data.EnergyDetect.EnergyDetectEntries;
			break;

		case THR_EventNwkScanConfirm_ScanType_ActiveScan:
			evt->Data.ActiveScan.NwkDiscoveryEntries = pPayload[idx]; idx++;

			if (evt->Data.ActiveScan.NwkDiscoveryEntries > 0)
			{
				evt->Data.ActiveScan.NwkDiscoveryList = MEM_BufferAlloc(evt->Data.ActiveScan.NwkDiscoveryEntries * sizeof(evt->Data.ActiveScan.NwkDiscoveryList[0]));

				if (!evt->Data.ActiveScan.NwkDiscoveryList)
				{
					MEM_BufferFree(evt->Data.EnergyDetect.EnergyDetectList);
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Data.ActiveScan.NwkDiscoveryList = NULL;
			}


			for (uint32_t i = 0; i < evt->Data.ActiveScan.NwkDiscoveryEntries; i++)
			{
				FLib_MemCpy(&(evt->Data.ActiveScan.NwkDiscoveryList[i].NumOfRcvdBeacons), pPayload + idx, sizeof(evt->Data.ActiveScan.NwkDiscoveryList[i].NumOfRcvdBeacons)); idx += sizeof(evt->Data.ActiveScan.NwkDiscoveryList[i].NumOfRcvdBeacons);
				FLib_MemCpy(&(evt->Data.ActiveScan.NwkDiscoveryList[i].PanId), pPayload + idx, sizeof(evt->Data.ActiveScan.NwkDiscoveryList[i].PanId)); idx += sizeof(evt->Data.ActiveScan.NwkDiscoveryList[i].PanId);
				evt->Data.ActiveScan.NwkDiscoveryList[i].Channel = pPayload[idx]; idx++;
				evt->Data.ActiveScan.NwkDiscoveryList[i].Reserved = pPayload[idx]; idx++;
			}
			break;

		case THR_EventNwkScanConfirm_ScanType_EnergyDetectAndActiveScan:
			evt->Data.EnergyDetectAndActiveScan.EnergyDetectEntries = pPayload[idx]; idx++;

			if (evt->Data.EnergyDetectAndActiveScan.EnergyDetectEntries > 0)
			{
				evt->Data.EnergyDetectAndActiveScan.EnergyDetectList = MEM_BufferAlloc(evt->Data.EnergyDetectAndActiveScan.EnergyDetectEntries);

				if (!evt->Data.EnergyDetectAndActiveScan.EnergyDetectList)
				{
					MEM_BufferFree(evt->Data.ActiveScan.NwkDiscoveryList);
					MEM_BufferFree(evt->Data.EnergyDetect.EnergyDetectList);
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Data.EnergyDetectAndActiveScan.EnergyDetectList = NULL;
			}

			FLib_MemCpy(evt->Data.EnergyDetectAndActiveScan.EnergyDetectList, pPayload + idx, evt->Data.EnergyDetectAndActiveScan.EnergyDetectEntries); idx += evt->Data.EnergyDetectAndActiveScan.EnergyDetectEntries;
			evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryEntries = pPayload[idx]; idx++;

			if (evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryEntries > 0)
			{
				evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryList = MEM_BufferAlloc(evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryEntries * sizeof(evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryList[0]));

				if (!evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryList)
				{
					MEM_BufferFree(evt->Data.EnergyDetectAndActiveScan.EnergyDetectList);
					MEM_BufferFree(evt->Data.ActiveScan.NwkDiscoveryList);
					MEM_BufferFree(evt->Data.EnergyDetect.EnergyDetectList);
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryList = NULL;
			}


			for (uint32_t i = 0; i < evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryEntries; i++)
			{
				FLib_MemCpy(&(evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryList[i].NumOfRcvdBeacons), pPayload + idx, sizeof(evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryList[i].NumOfRcvdBeacons)); idx += sizeof(evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryList[i].NumOfRcvdBeacons);
				FLib_MemCpy(&(evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryList[i].PanId), pPayload + idx, sizeof(evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryList[i].PanId)); idx += sizeof(evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryList[i].PanId);
				evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryList[i].Channel = pPayload[idx]; idx++;
				evt->Data.EnergyDetectAndActiveScan.NwkDiscoveryList[i].Reserved = pPayload[idx]; idx++;
			}
			break;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_EventNwkCreateConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the network create event
***************************************************************************************************/
static memStatus_t Load_THR_EventNwkCreateConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_EventNwkCreateConfirm_t *evt = &(container->Data.THR_EventNwkCreateConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF51;

	evt->InstanceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->EventStatus), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->DataSize), pPayload + idx, sizeof(evt->DataSize)); idx += sizeof(evt->DataSize);

	if (evt->DataSize > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->DataSize);

		if (!evt->Data)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->DataSize); idx += evt->DataSize;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_EventNwkJoinConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the network join event
***************************************************************************************************/
static memStatus_t Load_THR_EventNwkJoinConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_EventNwkJoinConfirm_t *evt = &(container->Data.THR_EventNwkJoinConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF52;

	evt->InstanceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->EventStatus), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->DataSize), pPayload + idx, sizeof(evt->DataSize)); idx += sizeof(evt->DataSize);

	if (evt->DataSize > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->DataSize);

		if (!evt->Data)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->DataSize); idx += evt->DataSize;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_EventNwkJoinSelectParentsConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the network join event
***************************************************************************************************/
static memStatus_t Load_THR_EventNwkJoinSelectParentsConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_EventNwkJoinSelectParentsConfirm_t *evt = &(container->Data.THR_EventNwkJoinSelectParentsConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF53;

	evt->InstanceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->EventStatus), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->DataSize), pPayload + idx, sizeof(evt->DataSize)); idx += sizeof(evt->DataSize);

	if (evt->DataSize > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->DataSize);

		if (!evt->Data)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->DataSize); idx += evt->DataSize;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_EventGeneralConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the network join event
***************************************************************************************************/
static memStatus_t Load_THR_EventGeneralConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_EventGeneralConfirm_t *evt = &(container->Data.THR_EventGeneralConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF54;

	evt->InstanceId = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->EventStatus), pPayload + idx, 2); idx += 2;
	FLib_MemCpy(&(evt->DataSize), pPayload + idx, sizeof(evt->DataSize)); idx += sizeof(evt->DataSize);

	if (evt->DataSize > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->DataSize);

		if (!evt->Data)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->DataSize); idx += evt->DataSize;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_EventNwkCommissioningIndication(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Commissioning Events
***************************************************************************************************/
static memStatus_t Load_THR_EventNwkCommissioningIndication(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_EventNwkCommissioningIndication_t *evt = &(container->Data.THR_EventNwkCommissioningIndication);

	/* Store (OG, OC) in ID */
	container->id = 0xCF55;

	FLib_MemCpy(evt, pPayload, sizeof(THR_EventNwkCommissioningIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_CommissioningDiagnosticIndication(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Commissioning diagnostic messages
***************************************************************************************************/
static memStatus_t Load_THR_CommissioningDiagnosticIndication(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_CommissioningDiagnosticIndication_t *evt = &(container->Data.THR_CommissioningDiagnosticIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF4E;

	evt->Direction = (THR_CommissioningDiagnosticIndication_Direction_t)pPayload[idx]; idx++;
	evt->Type = (THR_CommissioningDiagnosticIndication_Type_t)pPayload[idx]; idx++;
	FLib_MemCpy(evt->EUI, pPayload + idx, 8); idx += 8;
	evt->TlvsLength = pPayload[idx]; idx++;

	if (evt->TlvsLength > 0)
	{
		evt->TlvsBytes = MEM_BufferAlloc(evt->TlvsLength);

		if (!evt->TlvsBytes)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->TlvsBytes = NULL;
	}

	FLib_MemCpy(evt->TlvsBytes, pPayload + idx, evt->TlvsLength); idx += evt->TlvsLength;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_MgmtDiagnosticGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The confirmation to network diagnostic get request.
***************************************************************************************************/
static memStatus_t Load_THR_MgmtDiagnosticGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_MgmtDiagnosticGetConfirm_t *evt = &(container->Data.THR_MgmtDiagnosticGetConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF61;

	FLib_MemCpy(evt, pPayload, sizeof(THR_MgmtDiagnosticGetConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_DiagTestGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The confirmation to network diagnostic get request.
***************************************************************************************************/
static memStatus_t Load_THR_DiagTestGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_DiagTestGetConfirm_t *evt = &(container->Data.THR_DiagTestGetConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF67;

	FLib_MemCpy(evt, pPayload, sizeof(THR_DiagTestGetConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_MgmtDiagnosticResetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The confirmation to network diagnostic reset request.
***************************************************************************************************/
static memStatus_t Load_THR_MgmtDiagnosticResetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_MgmtDiagnosticResetConfirm_t *evt = &(container->Data.THR_MgmtDiagnosticResetConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF62;

	FLib_MemCpy(evt, pPayload, sizeof(THR_MgmtDiagnosticResetConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_MgmtReadMemoryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Memory content corresponding to the THR_MgmtReadMemory.Request.
***************************************************************************************************/
static memStatus_t Load_THR_MgmtReadMemoryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_MgmtReadMemoryConfirm_t *evt = &(container->Data.THR_MgmtReadMemoryConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF65;

	FLib_MemCpy(evt, pPayload, sizeof(THR_MgmtReadMemoryConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_MgmtWriteMemoryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirm of the THR_MgmtWriteMemory.Request command. Is always success.
***************************************************************************************************/
static memStatus_t Load_THR_MgmtWriteMemoryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_MgmtWriteMemoryConfirm_t *evt = &(container->Data.THR_MgmtWriteMemoryConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF66;

	FLib_MemCpy(evt, pPayload, sizeof(THR_MgmtWriteMemoryConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_SetManualSlaacIIDConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the THR_SetManualSlaacIID.Request.
***************************************************************************************************/
static memStatus_t Load_THR_SetManualSlaacIIDConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_SetManualSlaacIIDConfirm_t *evt = &(container->Data.THR_SetManualSlaacIIDConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF75;

	FLib_MemCpy(evt, pPayload, sizeof(THR_SetManualSlaacIIDConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_SendProactiveAddrNotifConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	THR_SendProactiveAddrNotif confirmation
***************************************************************************************************/
static memStatus_t Load_THR_SendProactiveAddrNotifConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_SendProactiveAddrNotifConfirm_t *evt = &(container->Data.THR_SendProactiveAddrNotifConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF76;

	FLib_MemCpy(evt, pPayload, sizeof(THR_SendProactiveAddrNotifConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_NwkDiscoveryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the nwk discovery request.
***************************************************************************************************/
static memStatus_t Load_THR_NwkDiscoveryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_NwkDiscoveryConfirm_t *evt = &(container->Data.THR_NwkDiscoveryConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCFC0;

	FLib_MemCpy(evt, pPayload, sizeof(THR_NwkDiscoveryConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_NwkDiscoveryStopConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the nwk discovery stop request.
***************************************************************************************************/
static memStatus_t Load_THR_NwkDiscoveryStopConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_NwkDiscoveryStopConfirm_t *evt = &(container->Data.THR_NwkDiscoveryStopConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCFC1;

	FLib_MemCpy(evt, pPayload, sizeof(THR_NwkDiscoveryStopConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_SearchNwkWithAnounceConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the search for nwk with announce.
***************************************************************************************************/
static memStatus_t Load_THR_SearchNwkWithAnounceConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_SearchNwkWithAnounceConfirm_t *evt = &(container->Data.THR_SearchNwkWithAnounceConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCFC2;

	FLib_MemCpy(evt, pPayload, sizeof(THR_SearchNwkWithAnounceConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_MgmtDiagnosticGetRspIndication(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The response to nwk diagnostic get request.
***************************************************************************************************/
static memStatus_t Load_THR_MgmtDiagnosticGetRspIndication(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_MgmtDiagnosticGetRspIndication_t *evt = &(container->Data.THR_MgmtDiagnosticGetRspIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF63;

	evt->Status = (THR_MgmtDiagnosticGetRspIndication_Status_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->CoapMsgId), pPayload + idx, sizeof(evt->CoapMsgId)); idx += sizeof(evt->CoapMsgId);
	FLib_MemCpy(&(evt->DataLen), pPayload + idx, sizeof(evt->DataLen)); idx += sizeof(evt->DataLen);

	if (evt->DataLen > 0)
	{
		evt->Payload = MEM_BufferAlloc(evt->DataLen * sizeof(evt->Payload[0]));

		if (!evt->Payload)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Payload = NULL;
	}


	for (uint32_t i = 0; i < evt->DataLen; i++)
	{
		evt->Payload[i].TlvId = (THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_t)pPayload[idx]; idx++;
		evt->Payload[i].TlvLength = pPayload[idx]; idx++;

		switch (evt->Payload[i].TlvId)
		{
			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_SourceAddr:
				FLib_MemCpy(&(evt->Payload[i].TlvData.SourceAddr), pPayload + idx, sizeof(evt->Payload[i].TlvData.SourceAddr)); idx += sizeof(evt->Payload[i].TlvData.SourceAddr);
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_ShortAddr:
				FLib_MemCpy(&(evt->Payload[i].TlvData.ShortAddr), pPayload + idx, sizeof(evt->Payload[i].TlvData.ShortAddr)); idx += sizeof(evt->Payload[i].TlvData.ShortAddr);
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_Mode:
				evt->Payload[i].TlvData.Mode = pPayload[idx]; idx++;
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_Timeout:
				FLib_MemCpy(&(evt->Payload[i].TlvData.Timeout), pPayload + idx, sizeof(evt->Payload[i].TlvData.Timeout)); idx += sizeof(evt->Payload[i].TlvData.Timeout);
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_LinkQuality:
				evt->Payload[i].TlvData.LinkQuality.MaxChildCount = pPayload[idx]; idx++;
				evt->Payload[i].TlvData.LinkQuality.ChildCount = pPayload[idx]; idx++;
				evt->Payload[i].TlvData.LinkQuality.cLinkQuality3 = pPayload[idx]; idx++;
				evt->Payload[i].TlvData.LinkQuality.cLinkQuality2 = pPayload[idx]; idx++;
				evt->Payload[i].TlvData.LinkQuality.cLinkQuality1 = pPayload[idx]; idx++;
				evt->Payload[i].TlvData.LinkQuality.LeaderCost = pPayload[idx]; idx++;
				evt->Payload[i].TlvData.LinkQuality.IdSequence = pPayload[idx]; idx++;
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_RoutingTable:

				if (evt->Payload[i].TlvLength > 0)
				{
					evt->Payload[i].TlvData.RoutingTable = MEM_BufferAlloc(evt->Payload[i].TlvLength);

					if (!evt->Payload[i].TlvData.RoutingTable)
					{
						MEM_BufferFree(evt->Payload);
						return MEM_ALLOC_ERROR_c;
					}

				}
				else
				{
					evt->Payload[i].TlvData.RoutingTable = NULL;
				}

				FLib_MemCpy(evt->Payload[i].TlvData.RoutingTable, pPayload + idx, evt->Payload[i].TlvLength); idx += evt->Payload[i].TlvLength;
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_LeaderData:
				FLib_MemCpy(&(evt->Payload[i].TlvData.LeaderData.PartitionId), pPayload + idx, sizeof(evt->Payload[i].TlvData.LeaderData.PartitionId)); idx += sizeof(evt->Payload[i].TlvData.LeaderData.PartitionId);
				evt->Payload[i].TlvData.LeaderData.Weighting = pPayload[idx]; idx++;
				evt->Payload[i].TlvData.LeaderData.DataVersion = pPayload[idx]; idx++;
				evt->Payload[i].TlvData.LeaderData.StableVersion = pPayload[idx]; idx++;
				evt->Payload[i].TlvData.LeaderData.LeaderId = pPayload[idx]; idx++;
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_NwkData:

				if (evt->Payload[i].TlvLength > 0)
				{
					evt->Payload[i].TlvData.NwkData = MEM_BufferAlloc(evt->Payload[i].TlvLength);

					if (!evt->Payload[i].TlvData.NwkData)
					{
						MEM_BufferFree(evt->Payload);
						return MEM_ALLOC_ERROR_c;
					}

				}
				else
				{
					evt->Payload[i].TlvData.NwkData = NULL;
				}

				FLib_MemCpy(evt->Payload[i].TlvData.NwkData, pPayload + idx, evt->Payload[i].TlvLength); idx += evt->Payload[i].TlvLength;
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_Ip6AddrList:

				if (evt->Payload[i].TlvLength > 0)
				{
					evt->Payload[i].TlvData.Ip6AddrList = MEM_BufferAlloc(evt->Payload[i].TlvLength);

					if (!evt->Payload[i].TlvData.Ip6AddrList)
					{
						MEM_BufferFree(evt->Payload);
						return MEM_ALLOC_ERROR_c;
					}

				}
				else
				{
					evt->Payload[i].TlvData.Ip6AddrList = NULL;
				}

				FLib_MemCpy(evt->Payload[i].TlvData.Ip6AddrList, pPayload + idx, evt->Payload[i].TlvLength); idx += evt->Payload[i].TlvLength;
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_MacCounters:
				FLib_MemCpy(&(evt->Payload[i].TlvData.MacCounters.ifInUnknownProtos), pPayload + idx, sizeof(evt->Payload[i].TlvData.MacCounters.ifInUnknownProtos)); idx += sizeof(evt->Payload[i].TlvData.MacCounters.ifInUnknownProtos);
				FLib_MemCpy(&(evt->Payload[i].TlvData.MacCounters.ifInErrors), pPayload + idx, sizeof(evt->Payload[i].TlvData.MacCounters.ifInErrors)); idx += sizeof(evt->Payload[i].TlvData.MacCounters.ifInErrors);
				FLib_MemCpy(&(evt->Payload[i].TlvData.MacCounters.ifOutErrors), pPayload + idx, sizeof(evt->Payload[i].TlvData.MacCounters.ifOutErrors)); idx += sizeof(evt->Payload[i].TlvData.MacCounters.ifOutErrors);
				FLib_MemCpy(&(evt->Payload[i].TlvData.MacCounters.ifInUcastPkts), pPayload + idx, sizeof(evt->Payload[i].TlvData.MacCounters.ifInUcastPkts)); idx += sizeof(evt->Payload[i].TlvData.MacCounters.ifInUcastPkts);
				FLib_MemCpy(&(evt->Payload[i].TlvData.MacCounters.ifInBroadcastPkts), pPayload + idx, sizeof(evt->Payload[i].TlvData.MacCounters.ifInBroadcastPkts)); idx += sizeof(evt->Payload[i].TlvData.MacCounters.ifInBroadcastPkts);
				FLib_MemCpy(&(evt->Payload[i].TlvData.MacCounters.ifInDiscards), pPayload + idx, sizeof(evt->Payload[i].TlvData.MacCounters.ifInDiscards)); idx += sizeof(evt->Payload[i].TlvData.MacCounters.ifInDiscards);
				FLib_MemCpy(&(evt->Payload[i].TlvData.MacCounters.ifOutUcastPkts), pPayload + idx, sizeof(evt->Payload[i].TlvData.MacCounters.ifOutUcastPkts)); idx += sizeof(evt->Payload[i].TlvData.MacCounters.ifOutUcastPkts);
				FLib_MemCpy(&(evt->Payload[i].TlvData.MacCounters.ifOutBroadcastPkts), pPayload + idx, sizeof(evt->Payload[i].TlvData.MacCounters.ifOutBroadcastPkts)); idx += sizeof(evt->Payload[i].TlvData.MacCounters.ifOutBroadcastPkts);
				FLib_MemCpy(&(evt->Payload[i].TlvData.MacCounters.ifOutDiscards), pPayload + idx, sizeof(evt->Payload[i].TlvData.MacCounters.ifOutDiscards)); idx += sizeof(evt->Payload[i].TlvData.MacCounters.ifOutDiscards);
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_BatteryLevel:
				evt->Payload[i].TlvData.BatteryLevel = pPayload[idx]; idx++;
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_SupplyVoltage:
				FLib_MemCpy(&(evt->Payload[i].TlvData.SupplyVoltage), pPayload + idx, sizeof(evt->Payload[i].TlvData.SupplyVoltage)); idx += sizeof(evt->Payload[i].TlvData.SupplyVoltage);
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_ChildTable:

				if (evt->Payload[i].TlvLength > 0)
				{
					evt->Payload[i].TlvData.ChildTable = MEM_BufferAlloc(evt->Payload[i].TlvLength);

					if (!evt->Payload[i].TlvData.ChildTable)
					{
						MEM_BufferFree(evt->Payload);
						return MEM_ALLOC_ERROR_c;
					}

				}
				else
				{
					evt->Payload[i].TlvData.ChildTable = NULL;
				}

				FLib_MemCpy(evt->Payload[i].TlvData.ChildTable, pPayload + idx, evt->Payload[i].TlvLength); idx += evt->Payload[i].TlvLength;
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_ChannelPages:
				evt->Payload[i].TlvData.ChannelPages = pPayload[idx]; idx++;
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_Fsl_Mac6lowPanNvmDataCount:
				FLib_MemCpy(&(evt->Payload[i].TlvData.Fsl_Mac6lowPanNvmDataCount.NvmDataSetId), pPayload + idx, 2); idx += 2;
				FLib_MemCpy(&(evt->Payload[i].TlvData.Fsl_Mac6lowPanNvmDataCount.DataSetCount), pPayload + idx, sizeof(evt->Payload[i].TlvData.Fsl_Mac6lowPanNvmDataCount.DataSetCount)); idx += sizeof(evt->Payload[i].TlvData.Fsl_Mac6lowPanNvmDataCount.DataSetCount);
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_Fsl_NetworkNvmDataCount_c:

				if (evt->Payload[i].TlvLength > 0)
				{
					evt->Payload[i].TlvData.Fsl_NetworkNvmDataCount_c = MEM_BufferAlloc(evt->Payload[i].TlvLength);

					if (!evt->Payload[i].TlvData.Fsl_NetworkNvmDataCount_c)
					{
						MEM_BufferFree(evt->Payload);
						return MEM_ALLOC_ERROR_c;
					}

				}
				else
				{
					evt->Payload[i].TlvData.Fsl_NetworkNvmDataCount_c = NULL;
				}

				FLib_MemCpy(evt->Payload[i].TlvData.Fsl_NetworkNvmDataCount_c, pPayload + idx, evt->Payload[i].TlvLength); idx += evt->Payload[i].TlvLength;
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_Fsl_SecurityNvmDataCount_c:
				FLib_MemCpy(&(evt->Payload[i].TlvData.Fsl_SecurityNvmDataCount_c.NvmDataSetId), pPayload + idx, 2); idx += 2;
				FLib_MemCpy(&(evt->Payload[i].TlvData.Fsl_SecurityNvmDataCount_c.DataSetCount), pPayload + idx, sizeof(evt->Payload[i].TlvData.Fsl_SecurityNvmDataCount_c.DataSetCount)); idx += sizeof(evt->Payload[i].TlvData.Fsl_SecurityNvmDataCount_c.DataSetCount);
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_Fsl_FunctionalNvmDataCount_c:
				FLib_MemCpy(&(evt->Payload[i].TlvData.Fsl_FunctionalNvmDataCount_c.NvmDataSetId), pPayload + idx, 2); idx += 2;
				FLib_MemCpy(&(evt->Payload[i].TlvData.Fsl_FunctionalNvmDataCount_c.DataSetCount), pPayload + idx, sizeof(evt->Payload[i].TlvData.Fsl_FunctionalNvmDataCount_c.DataSetCount)); idx += sizeof(evt->Payload[i].TlvData.Fsl_FunctionalNvmDataCount_c.DataSetCount);
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_Fsl_BoardName_c:

				if (evt->Payload[i].TlvLength > 0)
				{
					evt->Payload[i].TlvData.Fsl_BoardName_c = MEM_BufferAlloc(evt->Payload[i].TlvLength);

					if (!evt->Payload[i].TlvData.Fsl_BoardName_c)
					{
						MEM_BufferFree(evt->Payload);
						return MEM_ALLOC_ERROR_c;
					}

				}
				else
				{
					evt->Payload[i].TlvData.Fsl_BoardName_c = NULL;
				}

				FLib_MemCpy(evt->Payload[i].TlvData.Fsl_BoardName_c, pPayload + idx, evt->Payload[i].TlvLength); idx += evt->Payload[i].TlvLength;
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_Fsl_UniqueMcuId_c:
				FLib_MemCpy(evt->Payload[i].TlvData.Fsl_UniqueMcuId_c, pPayload + idx, sizeof(evt->Payload[i].TlvData.Fsl_UniqueMcuId_c)); idx += sizeof(evt->Payload[i].TlvData.Fsl_UniqueMcuId_c);
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_Fsl_StackVersion_c:
				FLib_MemCpy(&(evt->Payload[i].TlvData.Fsl_StackVersion_c.StackVendorOUI), pPayload + idx, 3); idx += 3;
				FLib_MemCpy(&(evt->Payload[i].TlvData.Fsl_StackVersion_c.StackVersion), pPayload + idx, 3); idx += 3;
				break;

			case THR_MgmtDiagnosticGetRspIndication_Payload_TlvId_Fsl_SoftwareVersion_c:

				if (evt->Payload[i].TlvLength > 0)
				{
					evt->Payload[i].TlvData.Fsl_SoftwareVersion_c = MEM_BufferAlloc(evt->Payload[i].TlvLength);

					if (!evt->Payload[i].TlvData.Fsl_SoftwareVersion_c)
					{
						MEM_BufferFree(evt->Payload);
						return MEM_ALLOC_ERROR_c;
					}

				}
				else
				{
					evt->Payload[i].TlvData.Fsl_SoftwareVersion_c = NULL;
				}

				FLib_MemCpy(evt->Payload[i].TlvData.Fsl_SoftwareVersion_c, pPayload + idx, evt->Payload[i].TlvLength); idx += evt->Payload[i].TlvLength;
				break;
		}
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_DiagTestGetRspIndication(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The response to large network diagnostic get request.
***************************************************************************************************/
static memStatus_t Load_THR_DiagTestGetRspIndication(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_DiagTestGetRspIndication_t *evt = &(container->Data.THR_DiagTestGetRspIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF68;

	evt->Status = (THR_DiagTestGetRspIndication_Status_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->CoapMsgId), pPayload + idx, sizeof(evt->CoapMsgId)); idx += sizeof(evt->CoapMsgId);
	FLib_MemCpy(&(evt->DataLen), pPayload + idx, sizeof(evt->DataLen)); idx += sizeof(evt->DataLen);
	evt->Payload.TlvId = (THR_DiagTestGetRspIndication_Payload_TlvId_t)pPayload[idx]; idx++;

	switch (evt->Payload.TlvId)
	{
		case THR_DiagTestGetRspIndication_Payload_TlvId_ColdFactoryReset:
			FLib_MemCpy(&(evt->Payload.TLVPayload.ColdFactoryReset), pPayload + idx, sizeof(evt->Payload.TLVPayload.ColdFactoryReset)); idx += sizeof(evt->Payload.TLVPayload.ColdFactoryReset);
			break;

		case THR_DiagTestGetRspIndication_Payload_TlvId_WarmCPUReset:
			FLib_MemCpy(&(evt->Payload.TLVPayload.WarmCPUReset), pPayload + idx, sizeof(evt->Payload.TLVPayload.WarmCPUReset)); idx += sizeof(evt->Payload.TLVPayload.WarmCPUReset);
			break;

		case THR_DiagTestGetRspIndication_Payload_TlvId_Data:
			evt->Payload.TLVPayload.Data.Flags = pPayload[idx]; idx++;
			FLib_MemCpy(&(evt->Payload.TLVPayload.Data.RspLatency), pPayload + idx, sizeof(evt->Payload.TLVPayload.Data.RspLatency)); idx += sizeof(evt->Payload.TLVPayload.Data.RspLatency);
			FLib_MemCpy(&(evt->Payload.TLVPayload.Data.Offset), pPayload + idx, sizeof(evt->Payload.TLVPayload.Data.Offset)); idx += sizeof(evt->Payload.TLVPayload.Data.Offset);
			evt->Payload.TLVPayload.Data.SequenceNumber = pPayload[idx]; idx++;
			evt->Payload.TLVPayload.Data.PayloadSize = pPayload[idx]; idx++;

			if (evt->Payload.TLVPayload.Data.PayloadSize > 0)
			{
				evt->Payload.TLVPayload.Data.Payload = MEM_BufferAlloc(evt->Payload.TLVPayload.Data.PayloadSize);

				if (!evt->Payload.TLVPayload.Data.Payload)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Payload.TLVPayload.Data.Payload = NULL;
			}

			FLib_MemCpy(evt->Payload.TLVPayload.Data.Payload, pPayload + idx, evt->Payload.TLVPayload.Data.PayloadSize); idx += evt->Payload.TLVPayload.Data.PayloadSize;
			break;

		case THR_DiagTestGetRspIndication_Payload_TlvId_Results:
			FLib_MemCpy(&(evt->Payload.TLVPayload.Results.ReqLatency), pPayload + idx, sizeof(evt->Payload.TLVPayload.Results.ReqLatency)); idx += sizeof(evt->Payload.TLVPayload.Results.ReqLatency);
			FLib_MemCpy(&(evt->Payload.TLVPayload.Results.RspLatency), pPayload + idx, sizeof(evt->Payload.TLVPayload.Results.RspLatency)); idx += sizeof(evt->Payload.TLVPayload.Results.RspLatency);
			FLib_MemCpy(&(evt->Payload.TLVPayload.Results.Offset), pPayload + idx, sizeof(evt->Payload.TLVPayload.Results.Offset)); idx += sizeof(evt->Payload.TLVPayload.Results.Offset);
			evt->Payload.TLVPayload.Results.SequenceNumber = pPayload[idx]; idx++;
			break;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_MgmtDiagnosticResetRspIndication(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The response to Network diagnostic reset request.
***************************************************************************************************/
static memStatus_t Load_THR_MgmtDiagnosticResetRspIndication(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_MgmtDiagnosticResetRspIndication_t *evt = &(container->Data.THR_MgmtDiagnosticResetRspIndication);

	/* Store (OG, OC) in ID */
	container->id = 0xCF64;

	FLib_MemCpy(evt, pPayload, sizeof(THR_MgmtDiagnosticResetRspIndication_t));

	return MEM_SUCCESS_c;
}

#endif  /* THREADMGMT_ENABLE */

#if THREADUTILS_ENABLE
/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_SetNwkIdTimeoutConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Set network ID timeout confirm
***************************************************************************************************/
static memStatus_t Load_THR_SetNwkIdTimeoutConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_SetNwkIdTimeoutConfirm_t *evt = &(container->Data.THR_SetNwkIdTimeoutConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF2E;

	FLib_MemCpy(evt, pPayload, sizeof(THR_SetNwkIdTimeoutConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_SetThresholdConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Thread set threshold confirm
***************************************************************************************************/
static memStatus_t Load_THR_SetThresholdConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_SetThresholdConfirm_t *evt = &(container->Data.THR_SetThresholdConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF20;

	FLib_MemCpy(evt, pPayload, sizeof(THR_SetThresholdConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_GetNeighborInfoConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Get detailed info about neighbor response.
***************************************************************************************************/
static memStatus_t Load_THR_GetNeighborInfoConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_GetNeighborInfoConfirm_t *evt = &(container->Data.THR_GetNeighborInfoConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF10;

	evt->Status = (THR_GetNeighborInfoConfirm_Status_t)pPayload[idx]; idx++;

	switch (evt->Status)
	{
		case THR_GetNeighborInfoConfirm_Status_Success:
			FLib_MemCpy(&(evt->NeighborInfo.Success.ExtendedAddress), pPayload + idx, sizeof(evt->NeighborInfo.Success.ExtendedAddress)); idx += sizeof(evt->NeighborInfo.Success.ExtendedAddress);
			FLib_MemCpy(&(evt->NeighborInfo.Success.ShortAddress), pPayload + idx, sizeof(evt->NeighborInfo.Success.ShortAddress)); idx += sizeof(evt->NeighborInfo.Success.ShortAddress);
			FLib_MemCpy(&(evt->NeighborInfo.Success.LastCommTime), pPayload + idx, sizeof(evt->NeighborInfo.Success.LastCommTime)); idx += sizeof(evt->NeighborInfo.Success.LastCommTime);
			evt->NeighborInfo.Success.InRSSI = pPayload[idx]; idx++;
			FLib_MemCpy(&(evt->NeighborInfo.Success.Timeoutsec), pPayload + idx, sizeof(evt->NeighborInfo.Success.Timeoutsec)); idx += sizeof(evt->NeighborInfo.Success.Timeoutsec);
			break;

		case THR_GetNeighborInfoConfirm_Status_NeighborNotFound:
			break;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_GetChildrenTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Get neighbor table response.
***************************************************************************************************/
static memStatus_t Load_THR_GetChildrenTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_GetChildrenTableConfirm_t *evt = &(container->Data.THR_GetChildrenTableConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF23;

	evt->InstanceId = pPayload[idx]; idx++;
	evt->NoOfElements = pPayload[idx]; idx++;

	if (evt->NoOfElements > 0)
	{
		evt->NeighborEntries = MEM_BufferAlloc(evt->NoOfElements * sizeof(evt->NeighborEntries[0]));

		if (!evt->NeighborEntries)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->NeighborEntries = NULL;
	}


	for (uint32_t i = 0; i < evt->NoOfElements; i++)
	{
		FLib_MemCpy(&(evt->NeighborEntries[i].ExtendedAddress), pPayload + idx, sizeof(evt->NeighborEntries[i].ExtendedAddress)); idx += sizeof(evt->NeighborEntries[i].ExtendedAddress);
		FLib_MemCpy(&(evt->NeighborEntries[i].ShortAddress), pPayload + idx, sizeof(evt->NeighborEntries[i].ShortAddress)); idx += sizeof(evt->NeighborEntries[i].ShortAddress);
		FLib_MemCpy(&(evt->NeighborEntries[i].LastCommTime), pPayload + idx, sizeof(evt->NeighborEntries[i].LastCommTime)); idx += sizeof(evt->NeighborEntries[i].LastCommTime);
		evt->NeighborEntries[i].LastRSSI = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->NeighborEntries[i].Timeout), pPayload + idx, sizeof(evt->NeighborEntries[i].Timeout)); idx += sizeof(evt->NeighborEntries[i].Timeout);
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_GetNeighborTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Get neighbor table response.
***************************************************************************************************/
static memStatus_t Load_THR_GetNeighborTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_GetNeighborTableConfirm_t *evt = &(container->Data.THR_GetNeighborTableConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF24;

	evt->InstanceId = pPayload[idx]; idx++;
	evt->NoOfElements = pPayload[idx]; idx++;

	if (evt->NoOfElements > 0)
	{
		evt->NeighborEntries = MEM_BufferAlloc(evt->NoOfElements * sizeof(evt->NeighborEntries[0]));

		if (!evt->NeighborEntries)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->NeighborEntries = NULL;
	}


	for (uint32_t i = 0; i < evt->NoOfElements; i++)
	{
		FLib_MemCpy(&(evt->NeighborEntries[i].ExtendedAddress), pPayload + idx, sizeof(evt->NeighborEntries[i].ExtendedAddress)); idx += sizeof(evt->NeighborEntries[i].ExtendedAddress);
		FLib_MemCpy(&(evt->NeighborEntries[i].ShortAddress), pPayload + idx, sizeof(evt->NeighborEntries[i].ShortAddress)); idx += sizeof(evt->NeighborEntries[i].ShortAddress);
		FLib_MemCpy(&(evt->NeighborEntries[i].LastCommTime), pPayload + idx, sizeof(evt->NeighborEntries[i].LastCommTime)); idx += sizeof(evt->NeighborEntries[i].LastCommTime);
		evt->NeighborEntries[i].LastRSSI = pPayload[idx]; idx++;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_GetRoutingTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Get routing table response.
***************************************************************************************************/
static memStatus_t Load_THR_GetRoutingTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_GetRoutingTableConfirm_t *evt = &(container->Data.THR_GetRoutingTableConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF25;

	evt->NoOfElements = pPayload[idx]; idx++;
	evt->IdSequenceNb = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->RouterIDMask), pPayload + idx, sizeof(evt->RouterIDMask)); idx += sizeof(evt->RouterIDMask);

	if (evt->NoOfElements > 0)
	{
		evt->RoutingEntries = MEM_BufferAlloc(evt->NoOfElements * sizeof(evt->RoutingEntries[0]));

		if (!evt->RoutingEntries)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->RoutingEntries = NULL;
	}


	for (uint32_t i = 0; i < evt->NoOfElements; i++)
	{
		evt->RoutingEntries[i].RouterID = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->RoutingEntries[i].ShortAddress), pPayload + idx, sizeof(evt->RoutingEntries[i].ShortAddress)); idx += sizeof(evt->RoutingEntries[i].ShortAddress);
		FLib_MemCpy(&(evt->RoutingEntries[i].NextHop), pPayload + idx, sizeof(evt->RoutingEntries[i].NextHop)); idx += sizeof(evt->RoutingEntries[i].NextHop);
		evt->RoutingEntries[i].Cost = pPayload[idx]; idx++;
		evt->RoutingEntries[i].nIn = pPayload[idx]; idx++;
		evt->RoutingEntries[i].nOut = pPayload[idx]; idx++;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_GetAttrConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Get attribute response
***************************************************************************************************/
static memStatus_t Load_THR_GetAttrConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_GetAttrConfirm_t *evt = &(container->Data.THR_GetAttrConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF17;

	evt->InstanceId = pPayload[idx]; idx++;
	evt->AttributeId = (THR_GetAttrConfirm_AttributeId_t)pPayload[idx]; idx++;
	evt->Index = pPayload[idx]; idx++;
	evt->Status = (THR_GetAttrConfirm_Status_t)pPayload[idx]; idx++;
	evt->AttrSize = pPayload[idx]; idx++;

	switch (evt->AttributeId)
	{
		case THR_GetAttrConfirm_AttributeId_RandomExtendedAddr:
			FLib_MemCpy(&(evt->AttributeValue.RandomExtendedAddr), pPayload + idx, sizeof(evt->AttributeValue.RandomExtendedAddr)); idx += sizeof(evt->AttributeValue.RandomExtendedAddr);
			break;

		case THR_GetAttrConfirm_AttributeId_ShortAddress:
			FLib_MemCpy(&(evt->AttributeValue.ShortAddress), pPayload + idx, sizeof(evt->AttributeValue.ShortAddress)); idx += sizeof(evt->AttributeValue.ShortAddress);
			break;

		case THR_GetAttrConfirm_AttributeId_ScanChannelMask:
			FLib_MemCpy(&(evt->AttributeValue.ScanChannelMask), pPayload + idx, sizeof(evt->AttributeValue.ScanChannelMask)); idx += sizeof(evt->AttributeValue.ScanChannelMask);
			break;

		case THR_GetAttrConfirm_AttributeId_ScanDuration:
			FLib_MemCpy(&(evt->AttributeValue.ScanDuration), pPayload + idx, sizeof(evt->AttributeValue.ScanDuration)); idx += sizeof(evt->AttributeValue.ScanDuration);
			break;

		case THR_GetAttrConfirm_AttributeId_Channel:
			evt->AttributeValue.Channel = pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_ShortPanId:
			FLib_MemCpy(&(evt->AttributeValue.ShortPanId), pPayload + idx, sizeof(evt->AttributeValue.ShortPanId)); idx += sizeof(evt->AttributeValue.ShortPanId);
			break;

		case THR_GetAttrConfirm_AttributeId_ExtendedPanId:

			if (evt->AttrSize > 0)
			{
				evt->AttributeValue.ExtendedPanId = MEM_BufferAlloc(evt->AttrSize);

				if (!evt->AttributeValue.ExtendedPanId)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->AttributeValue.ExtendedPanId = NULL;
			}

			FLib_MemCpy(evt->AttributeValue.ExtendedPanId, pPayload + idx, evt->AttrSize); idx += evt->AttrSize;
			break;

		case THR_GetAttrConfirm_AttributeId_PermitJoin:
			evt->AttributeValue.PermitJoin = (bool_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_RxOnIdle:
			evt->AttributeValue.RxOnIdle = (bool_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_SedPollInterval:
			FLib_MemCpy(&(evt->AttributeValue.SedPollInterval), pPayload + idx, sizeof(evt->AttributeValue.SedPollInterval)); idx += sizeof(evt->AttributeValue.SedPollInterval);
			break;

		case THR_GetAttrConfirm_AttributeId_UniqueExtendedAddress:
			evt->AttributeValue.UniqueExtendedAddress = (bool_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_VendorName:

			if (evt->AttrSize > 0)
			{
				evt->AttributeValue.VendorName = MEM_BufferAlloc(evt->AttrSize);

				if (!evt->AttributeValue.VendorName)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->AttributeValue.VendorName = NULL;
			}

			FLib_MemCpy(evt->AttributeValue.VendorName, pPayload + idx, evt->AttrSize); idx += evt->AttrSize;
			break;

		case THR_GetAttrConfirm_AttributeId_ModelName:

			if (evt->AttrSize > 0)
			{
				evt->AttributeValue.ModelName = MEM_BufferAlloc(evt->AttrSize);

				if (!evt->AttributeValue.ModelName)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->AttributeValue.ModelName = NULL;
			}

			FLib_MemCpy(evt->AttributeValue.ModelName, pPayload + idx, evt->AttrSize); idx += evt->AttrSize;
			break;

		case THR_GetAttrConfirm_AttributeId_SwVersion:

			if (evt->AttrSize > 0)
			{
				evt->AttributeValue.SwVersion = MEM_BufferAlloc(evt->AttrSize);

				if (!evt->AttributeValue.SwVersion)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->AttributeValue.SwVersion = NULL;
			}

			FLib_MemCpy(evt->AttributeValue.SwVersion, pPayload + idx, evt->AttrSize); idx += evt->AttrSize;
			break;

		case THR_GetAttrConfirm_AttributeId_StackVersion:
			break;

		case THR_GetAttrConfirm_AttributeId_NwkCapabilities:
			evt->AttributeValue.NwkCapabilities = pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_NwkName:

			if (evt->AttrSize > 0)
			{
				evt->AttributeValue.NwkName = MEM_BufferAlloc(evt->AttrSize);

				if (!evt->AttributeValue.NwkName)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->AttributeValue.NwkName = NULL;
			}

			FLib_MemCpy(evt->AttributeValue.NwkName, pPayload + idx, evt->AttrSize); idx += evt->AttrSize;
			break;

		case THR_GetAttrConfirm_AttributeId_DeviceType:
			evt->AttributeValue.DeviceType = (THR_GetAttrConfirm_AttributeValue_DeviceType_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_IsDevConnected:
			evt->AttributeValue.IsDevConnected = (bool_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_IsDevCommissioned:
			evt->AttributeValue.IsDevCommissioned = (bool_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_PartitionId:
			FLib_MemCpy(&(evt->AttributeValue.PartitionId), pPayload + idx, sizeof(evt->AttributeValue.PartitionId)); idx += sizeof(evt->AttributeValue.PartitionId);
			break;

		case THR_GetAttrConfirm_AttributeId_DeviceRole:
			evt->AttributeValue.DeviceRole = (THR_GetAttrConfirm_AttributeValue_DeviceRole_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_Security_NwkMasterKey:

			if (evt->AttrSize > 0)
			{
				evt->AttributeValue.Security_NwkMasterKey = MEM_BufferAlloc(evt->AttrSize);

				if (!evt->AttributeValue.Security_NwkMasterKey)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->AttributeValue.Security_NwkMasterKey = NULL;
			}

			FLib_MemCpy(evt->AttributeValue.Security_NwkMasterKey, pPayload + idx, evt->AttrSize); idx += evt->AttrSize;
			break;

		case THR_GetAttrConfirm_AttributeId_Security_NwkKeySeq:
			FLib_MemCpy(&(evt->AttributeValue.Security_NwkKeySeq), pPayload + idx, sizeof(evt->AttributeValue.Security_NwkKeySeq)); idx += sizeof(evt->AttributeValue.Security_NwkKeySeq);
			break;

		case THR_GetAttrConfirm_AttributeId_Security_PSKc:

			if (evt->AttrSize > 0)
			{
				evt->AttributeValue.Security_PSKc = MEM_BufferAlloc(evt->AttrSize);

				if (!evt->AttributeValue.Security_PSKc)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->AttributeValue.Security_PSKc = NULL;
			}

			FLib_MemCpy(evt->AttributeValue.Security_PSKc, pPayload + idx, evt->AttrSize); idx += evt->AttrSize;
			break;

		case THR_GetAttrConfirm_AttributeId_Security_PSKd:

			if (evt->AttrSize > 0)
			{
				evt->AttributeValue.Security_PSKd = MEM_BufferAlloc(evt->AttrSize);

				if (!evt->AttributeValue.Security_PSKd)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->AttributeValue.Security_PSKd = NULL;
			}

			FLib_MemCpy(evt->AttributeValue.Security_PSKd, pPayload + idx, evt->AttrSize); idx += evt->AttrSize;
			break;

		case THR_GetAttrConfirm_AttributeId_VendorData:

			if (evt->AttrSize > 0)
			{
				evt->AttributeValue.VendorData = MEM_BufferAlloc(evt->AttrSize);

				if (!evt->AttributeValue.VendorData)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->AttributeValue.VendorData = NULL;
			}

			FLib_MemCpy(evt->AttributeValue.VendorData, pPayload + idx, evt->AttrSize); idx += evt->AttrSize;
			break;

		case THR_GetAttrConfirm_AttributeId_MLPrefix:
			FLib_MemCpy(evt->AttributeValue.MLPrefix.PrefixData, pPayload + idx, 16); idx += 16;
			evt->AttributeValue.MLPrefix.PrefixLength = pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_MacFilteringEntry:
			FLib_MemCpy(&(evt->AttributeValue.MacFilteringEntry.ExtendedAddress), pPayload + idx, sizeof(evt->AttributeValue.MacFilteringEntry.ExtendedAddress)); idx += sizeof(evt->AttributeValue.MacFilteringEntry.ExtendedAddress);
			FLib_MemCpy(&(evt->AttributeValue.MacFilteringEntry.ShortAddress), pPayload + idx, sizeof(evt->AttributeValue.MacFilteringEntry.ShortAddress)); idx += sizeof(evt->AttributeValue.MacFilteringEntry.ShortAddress);
			evt->AttributeValue.MacFilteringEntry.LinkIndicator = pPayload[idx]; idx++;
			evt->AttributeValue.MacFilteringEntry.BlockNeighbor = (bool_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_Security_KeyRotationInterval:
			FLib_MemCpy(&(evt->AttributeValue.Security_KeyRotationInterval), pPayload + idx, sizeof(evt->AttributeValue.Security_KeyRotationInterval)); idx += sizeof(evt->AttributeValue.Security_KeyRotationInterval);
			break;

		case THR_GetAttrConfirm_AttributeId_ChildAddrMask:

			if (evt->AttrSize > 0)
			{
				evt->AttributeValue.ChildAddrMask = MEM_BufferAlloc(evt->AttrSize);

				if (!evt->AttributeValue.ChildAddrMask)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->AttributeValue.ChildAddrMask = NULL;
			}

			FLib_MemCpy(evt->AttributeValue.ChildAddrMask, pPayload + idx, evt->AttrSize); idx += evt->AttrSize;
			break;

		case THR_GetAttrConfirm_AttributeId_ChildSEDTimeout:
			FLib_MemCpy(&(evt->AttributeValue.ChildSEDTimeout), pPayload + idx, sizeof(evt->AttributeValue.ChildSEDTimeout)); idx += sizeof(evt->AttributeValue.ChildSEDTimeout);
			break;

		case THR_GetAttrConfirm_AttributeId_ChildEDTimeout:
			FLib_MemCpy(&(evt->AttributeValue.ChildEDTimeout), pPayload + idx, sizeof(evt->AttributeValue.ChildEDTimeout)); idx += sizeof(evt->AttributeValue.ChildEDTimeout);
			break;

		case THR_GetAttrConfirm_AttributeId_EndDevice_ChildEDReqFullNwkData:
			evt->AttributeValue.EndDevice_ChildEDReqFullNwkData = (bool_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_EndDevice_IsFastPollEnabled:
			evt->AttributeValue.EndDevice_IsFastPollEnabled = (bool_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_SleepyEndDevice_FastPollInterval:
			FLib_MemCpy(&(evt->AttributeValue.SleepyEndDevice_FastPollInterval), pPayload + idx, sizeof(evt->AttributeValue.SleepyEndDevice_FastPollInterval)); idx += sizeof(evt->AttributeValue.SleepyEndDevice_FastPollInterval);
			break;

		case THR_GetAttrConfirm_AttributeId_JoinLqiThreshold:
			evt->AttributeValue.JoinLqiThreshold = pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_ProvisioningURL:

			if (evt->AttrSize > 0)
			{
				evt->AttributeValue.ProvisioningURL = MEM_BufferAlloc(evt->AttrSize);

				if (!evt->AttributeValue.ProvisioningURL)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->AttributeValue.ProvisioningURL = NULL;
			}

			FLib_MemCpy(evt->AttributeValue.ProvisioningURL, pPayload + idx, evt->AttrSize); idx += evt->AttrSize;
			break;

		case THR_GetAttrConfirm_AttributeId_SelectBestChannelEDThreshold:
			evt->AttributeValue.SelectBestChannelEDThreshold = pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_CommissionerMode:
			evt->AttributeValue.CommissionerMode = (THR_GetAttrConfirm_AttributeValue_CommissionerMode_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_BorderRouter_BrPrefixEntry:
			evt->AttributeValue.BorderRouter_BrPrefixEntry.prefixLength = pPayload[idx]; idx++;
			FLib_MemCpy(evt->AttributeValue.BorderRouter_BrPrefixEntry.PrefixValue, pPayload + idx, 16); idx += 16;
			evt->AttributeValue.BorderRouter_BrPrefixEntry.PrefixFlagsReserved = pPayload[idx]; idx++;
			evt->AttributeValue.BorderRouter_BrPrefixEntry.PrefixFlags = pPayload[idx]; idx++;
			FLib_MemCpy(&(evt->AttributeValue.BorderRouter_BrPrefixEntry.prefixLifetime), pPayload + idx, sizeof(evt->AttributeValue.BorderRouter_BrPrefixEntry.prefixLifetime)); idx += sizeof(evt->AttributeValue.BorderRouter_BrPrefixEntry.prefixLifetime);
			evt->AttributeValue.BorderRouter_BrPrefixEntry.prefixAdvertised = (bool_t)pPayload[idx]; idx++;
			evt->AttributeValue.BorderRouter_BrPrefixEntry.ExternalRouteFlags = pPayload[idx]; idx++;
			FLib_MemCpy(&(evt->AttributeValue.BorderRouter_BrPrefixEntry.ExternalRouteLifetime), pPayload + idx, sizeof(evt->AttributeValue.BorderRouter_BrPrefixEntry.ExternalRouteLifetime)); idx += sizeof(evt->AttributeValue.BorderRouter_BrPrefixEntry.ExternalRouteLifetime);
			evt->AttributeValue.BorderRouter_BrPrefixEntry.ExternalRouteAdvertised = (bool_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_SteeringData:

			if (evt->AttrSize > 0)
			{
				evt->AttributeValue.SteeringData = MEM_BufferAlloc(evt->AttrSize);

				if (!evt->AttributeValue.SteeringData)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->AttributeValue.SteeringData = NULL;
			}

			FLib_MemCpy(evt->AttributeValue.SteeringData, pPayload + idx, evt->AttrSize); idx += evt->AttrSize;
			break;

		case THR_GetAttrConfirm_AttributeId_Security_KeySwitchGuardTime:
			FLib_MemCpy(&(evt->AttributeValue.Security_KeySwitchGuardTime), pPayload + idx, sizeof(evt->AttributeValue.Security_KeySwitchGuardTime)); idx += sizeof(evt->AttributeValue.Security_KeySwitchGuardTime);
			break;

		case THR_GetAttrConfirm_AttributeId_ParentHoldTime:
			FLib_MemCpy(&(evt->AttributeValue.ParentHoldTime), pPayload + idx, sizeof(evt->AttributeValue.ParentHoldTime)); idx += sizeof(evt->AttributeValue.ParentHoldTime);
			break;

		case THR_GetAttrConfirm_AttributeId_Security_Policy:
			evt->AttributeValue.Security_Policy = pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_NVM_RestoreAutoStart:
			evt->AttributeValue.NVM_RestoreAutoStart = (bool_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_NVM_Restore:
			evt->AttributeValue.NVM_Restore = (bool_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_SlaacPolicy:
			evt->AttributeValue.SlaacPolicy = (THR_GetAttrConfirm_AttributeValue_SlaacPolicy_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_IeeeExtendedAddr:
			FLib_MemCpy(&(evt->AttributeValue.IeeeExtendedAddr), pPayload + idx, sizeof(evt->AttributeValue.IeeeExtendedAddr)); idx += sizeof(evt->AttributeValue.IeeeExtendedAddr);
			break;

		case THR_GetAttrConfirm_AttributeId_LeaderWeight:
			evt->AttributeValue.LeaderWeight = pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_HashIeeeAddr:
			FLib_MemCpy(&(evt->AttributeValue.HashIeeeAddr), pPayload + idx, sizeof(evt->AttributeValue.HashIeeeAddr)); idx += sizeof(evt->AttributeValue.HashIeeeAddr);
			break;

		case THR_GetAttrConfirm_AttributeId_BorderRouter_BrGlobalOnMeshPrefix:
			FLib_MemCpy(evt->AttributeValue.BorderRouter_BrGlobalOnMeshPrefix.PrefixData, pPayload + idx, 16); idx += 16;
			evt->AttributeValue.BorderRouter_BrGlobalOnMeshPrefix.PrefixLength = pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_BorderRouter_BrDefaultRouteOnMeshPrefix:
			evt->AttributeValue.BorderRouter_BrDefaultRouteOnMeshPrefix = (bool_t)pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_BorderRouter_BrExternalIfPrefix:
			FLib_MemCpy(evt->AttributeValue.BorderRouter_BrExternalIfPrefix.PrefixData, pPayload + idx, 16); idx += 16;
			evt->AttributeValue.BorderRouter_BrExternalIfPrefix.PrefixLength = pPayload[idx]; idx++;
			break;

		case THR_GetAttrConfirm_AttributeId_ActiveTimestamp:
			FLib_MemCpy(evt->AttributeValue.ActiveTimestamp.ActiveSeconds, pPayload + idx, 6); idx += 6;
			FLib_MemCpy(evt->AttributeValue.ActiveTimestamp.ActiveTicks, pPayload + idx, 2); idx += 2;
			break;

		case THR_GetAttrConfirm_AttributeId_PendingChannel:
			break;

		case THR_GetAttrConfirm_AttributeId_PendingChannelMask:
			break;

		case THR_GetAttrConfirm_AttributeId_PendingXpanId:
			break;

		case THR_GetAttrConfirm_AttributeId_PendingMLprefix:
			break;

		case THR_GetAttrConfirm_AttributeId_PendingNwkMasterKey:
			break;

		case THR_GetAttrConfirm_AttributeId_PendingNwkName:
			break;

		case THR_GetAttrConfirm_AttributeId_PendingPanId:
			break;

		case THR_GetAttrConfirm_AttributeId_PendingPSK:
			break;

		case THR_GetAttrConfirm_AttributeId_PendingSecurityPolicy:
			break;

		case THR_GetAttrConfirm_AttributeId_PendingNwkKeyRotationInterval:
			break;

		case THR_GetAttrConfirm_AttributeId_PendingDelayTimer:
			break;

		case THR_GetAttrConfirm_AttributeId_PendingActiveTimestamp:
			break;

		case THR_GetAttrConfirm_AttributeId_PendingTimestamp:
			break;

		case THR_GetAttrConfirm_AttributeId_CommissionerId:

			if (evt->AttrSize > 0)
			{
				evt->AttributeValue.CommissionerID = MEM_BufferAlloc(evt->AttrSize);

				if (!evt->AttributeValue.CommissionerID)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->AttributeValue.CommissionerID = NULL;
			}

			FLib_MemCpy(evt->AttributeValue.CommissionerID, pPayload + idx, evt->AttrSize); idx += evt->AttrSize;
			break;

		case THR_GetAttrConfirm_AttributeId_JoinerPort:
			FLib_MemCpy(&(evt->AttributeValue.JoinerPort), pPayload + idx, sizeof(evt->AttributeValue.JoinerPort)); idx += sizeof(evt->AttributeValue.JoinerPort);
			break;

		case THR_GetAttrConfirm_AttributeId_CommissionerUdpPort:
			FLib_MemCpy(&(evt->AttributeValue.CommissionerUdpPort), pPayload + idx, sizeof(evt->AttributeValue.CommissionerUdpPort)); idx += sizeof(evt->AttributeValue.CommissionerUdpPort);
			break;

		case THR_GetAttrConfirm_AttributeId_DiscoveryReqMacTxOptions:
			break;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_SetAttrConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Set attribute confirm
***************************************************************************************************/
static memStatus_t Load_THR_SetAttrConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_SetAttrConfirm_t *evt = &(container->Data.THR_SetAttrConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF18;

	FLib_MemCpy(evt, pPayload, sizeof(THR_SetAttrConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_GetThreadIpAddrConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Get thread IP addr confirm
***************************************************************************************************/
static memStatus_t Load_THR_GetThreadIpAddrConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_GetThreadIpAddrConfirm_t *evt = &(container->Data.THR_GetThreadIpAddrConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF19;

	evt->InstanceId = pPayload[idx]; idx++;
	evt->Status = (THR_GetThreadIpAddrConfirm_Status_t)pPayload[idx]; idx++;
	evt->AddressType = pPayload[idx]; idx++;
	evt->NoOfIpAddr = pPayload[idx]; idx++;

	if (evt->NoOfIpAddr > 0)
	{
		evt->AddressList = MEM_BufferAlloc(evt->NoOfIpAddr * 16);

		if (!evt->AddressList)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->AddressList = NULL;
	}

	FLib_MemCpy(evt->AddressList, pPayload + idx, evt->NoOfIpAddr * 16); idx += evt->NoOfIpAddr * 16;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_GetParentConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Get parent confirm
***************************************************************************************************/
static memStatus_t Load_THR_GetParentConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_GetParentConfirm_t *evt = &(container->Data.THR_GetParentConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF1E;

	FLib_MemCpy(evt, pPayload, sizeof(THR_GetParentConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_ChildUpdateToParentConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the THR_ChildUpdateToParent.Request.
***************************************************************************************************/
static memStatus_t Load_THR_ChildUpdateToParentConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_ChildUpdateToParentConfirm_t *evt = &(container->Data.THR_ChildUpdateToParentConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF2F;

	FLib_MemCpy(evt, pPayload, sizeof(THR_ChildUpdateToParentConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_LeaderRemoveRouterIdConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of remove router id request.
***************************************************************************************************/
static memStatus_t Load_THR_LeaderRemoveRouterIdConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_LeaderRemoveRouterIdConfirm_t *evt = &(container->Data.THR_LeaderRemoveRouterIdConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF26;

	FLib_MemCpy(evt, pPayload, sizeof(THR_LeaderRemoveRouterIdConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_GenerateAllKeysConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of THR_GenerateAllKeys.Request.
***************************************************************************************************/
static memStatus_t Load_THR_GenerateAllKeysConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_GenerateAllKeysConfirm_t *evt = &(container->Data.THR_GenerateAllKeysConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF28;

	FLib_MemCpy(evt, pPayload, sizeof(THR_GenerateAllKeysConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_SwitchKeyKeyConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of ThrSwitchKey request.
***************************************************************************************************/
static memStatus_t Load_THR_SwitchKeyKeyConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_SwitchKeyKeyConfirm_t *evt = &(container->Data.THR_SwitchKeyKeyConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF27;

	FLib_MemCpy(evt, pPayload, sizeof(THR_SwitchKeyKeyConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_BrPrefixAddEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of THR_BrPrefixAddEntry request.
***************************************************************************************************/
static memStatus_t Load_THR_BrPrefixAddEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_BrPrefixAddEntryConfirm_t *evt = &(container->Data.THR_BrPrefixAddEntryConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF29;

	FLib_MemCpy(evt, pPayload, sizeof(THR_BrPrefixAddEntryConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_BrPrefixGetTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	THR_BrPrefixGetTable response.
***************************************************************************************************/
static memStatus_t Load_THR_BrPrefixGetTableConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_BrPrefixGetTableConfirm_t *evt = &(container->Data.THR_BrPrefixGetTableConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF2A;

	evt->InstanceId = pPayload[idx]; idx++;
	evt->NoOfElements = pPayload[idx]; idx++;

	if (evt->NoOfElements > 0)
	{
		evt->BrPrefixEntries = MEM_BufferAlloc(evt->NoOfElements * sizeof(evt->BrPrefixEntries[0]));

		if (!evt->BrPrefixEntries)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->BrPrefixEntries = NULL;
	}


	for (uint32_t i = 0; i < evt->NoOfElements; i++)
	{
		evt->BrPrefixEntries[i].prefixLength = pPayload[idx]; idx++;
		FLib_MemCpy(evt->BrPrefixEntries[i].PrefixValue, pPayload + idx, 16); idx += 16;
		evt->BrPrefixEntries[i].PrefixFlagsReserved = pPayload[idx]; idx++;
		evt->BrPrefixEntries[i].PrefixFlags = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->BrPrefixEntries[i].prefixLifetime), pPayload + idx, sizeof(evt->BrPrefixEntries[i].prefixLifetime)); idx += sizeof(evt->BrPrefixEntries[i].prefixLifetime);
		evt->BrPrefixEntries[i].prefixAdvertised = (bool_t)pPayload[idx]; idx++;
		evt->BrPrefixEntries[i].ExternalRouteFlags = pPayload[idx]; idx++;
		FLib_MemCpy(&(evt->BrPrefixEntries[i].ExternalRouteLifetime), pPayload + idx, sizeof(evt->BrPrefixEntries[i].ExternalRouteLifetime)); idx += sizeof(evt->BrPrefixEntries[i].ExternalRouteLifetime);
		evt->BrPrefixEntries[i].ExternalRouteAdvertised = (bool_t)pPayload[idx]; idx++;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_BrPrefixRemoveEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of THR_BrPrefixRemoveEntry request.
***************************************************************************************************/
static memStatus_t Load_THR_BrPrefixRemoveEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_BrPrefixRemoveEntryConfirm_t *evt = &(container->Data.THR_BrPrefixRemoveEntryConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF2B;

	FLib_MemCpy(evt, pPayload, sizeof(THR_BrPrefixRemoveEntryConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_BrServiceRemoveEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of THR_BrServiceRemoveEntry request.
***************************************************************************************************/
static memStatus_t Load_THR_BrServiceRemoveEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_BrServiceRemoveEntryConfirm_t *evt = &(container->Data.THR_BrServiceRemoveEntryConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF86;

	FLib_MemCpy(evt, pPayload, sizeof(THR_BrServiceRemoveEntryConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_BrServiceAddEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of THR_BrServiceAddEntry request.
***************************************************************************************************/
static memStatus_t Load_THR_BrServiceAddEntryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_BrServiceAddEntryConfirm_t *evt = &(container->Data.THR_BrServiceAddEntryConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF85;

	FLib_MemCpy(evt, pPayload, sizeof(THR_BrServiceAddEntryConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_BrPrefixSyncConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of THR_BrPrefixSync request.
***************************************************************************************************/
static memStatus_t Load_THR_BrPrefixSyncConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_BrPrefixSyncConfirm_t *evt = &(container->Data.THR_BrPrefixSyncConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF2C;

	FLib_MemCpy(evt, pPayload, sizeof(THR_BrPrefixSyncConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_BrPrefixRemoveAllConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of THR_BrPrefixRemoveAll request.
***************************************************************************************************/
static memStatus_t Load_THR_BrPrefixRemoveAllConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_BrPrefixRemoveAllConfirm_t *evt = &(container->Data.THR_BrPrefixRemoveAllConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF2D;

	FLib_MemCpy(evt, pPayload, sizeof(THR_BrPrefixRemoveAllConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_THR_IdentifyConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the board identification command.
***************************************************************************************************/
static memStatus_t Load_THR_IdentifyConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	THR_IdentifyConfirm_t *evt = &(container->Data.THR_IdentifyConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF69;

	FLib_MemCpy(evt, pPayload, sizeof(THR_IdentifyConfirm_t));

	return MEM_SUCCESS_c;
}

#endif  /* THREADUTILS_ENABLE */

#if IPUTILS_ENABLE
/*!*************************************************************************************************
\fn		static memStatus_t Load_NWKU_IfconfigBindConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation of the bind to interface request.
***************************************************************************************************/
static memStatus_t Load_NWKU_IfconfigBindConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	NWKU_IfconfigBindConfirm_t *evt = &(container->Data.NWKU_IfconfigBindConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF0C;

	FLib_MemCpy(evt, pPayload, sizeof(NWKU_IfconfigBindConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_NWKU_IfconfigAllResponse(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Response to IfconfigAll request.
***************************************************************************************************/
static memStatus_t Load_NWKU_IfconfigAllResponse(thrEvtContainer_t *container, uint8_t *pPayload)
{
	NWKU_IfconfigAllResponse_t *evt = &(container->Data.NWKU_IfconfigAllResponse);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF0D;

	evt->CountInterfaces = pPayload[idx]; idx++;

	if (evt->CountInterfaces > 0)
	{
		evt->InterfaceIDs = MEM_BufferAlloc(evt->CountInterfaces * sizeof(evt->InterfaceIDs[0]));

		if (!evt->InterfaceIDs)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->InterfaceIDs = NULL;
	}


	for (uint32_t i = 0; i < evt->CountInterfaces; i++)
	{
		evt->InterfaceIDs[i].InterfaceID = pPayload[idx]; idx++;
		evt->InterfaceIDs[i].CountIpAddresses = pPayload[idx]; idx++;

		if (evt->InterfaceIDs[i].CountIpAddresses > 0)
		{
			evt->InterfaceIDs[i].Addresses = MEM_BufferAlloc(evt->InterfaceIDs[i].CountIpAddresses * 16);

			if (!evt->InterfaceIDs[i].Addresses)
			{
				MEM_BufferFree(evt->InterfaceIDs);
				return MEM_ALLOC_ERROR_c;
			}

		}
		else
		{
			evt->InterfaceIDs[i].Addresses = NULL;
		}

		FLib_MemCpy(evt->InterfaceIDs[i].Addresses, pPayload + idx, evt->InterfaceIDs[i].CountIpAddresses * 16); idx += evt->InterfaceIDs[i].CountIpAddresses * 16;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_NWKU_PingConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The response to ping request.
***************************************************************************************************/
static memStatus_t Load_NWKU_PingConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	NWKU_PingConfirm_t *evt = &(container->Data.NWKU_PingConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF0E;

	FLib_MemCpy(evt, pPayload, sizeof(NWKU_PingConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_NWKU_EchoUDPConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The response to echo UDP request.
***************************************************************************************************/
static memStatus_t Load_NWKU_EchoUDPConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	NWKU_EchoUDPConfirm_t *evt = &(container->Data.NWKU_EchoUDPConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF70;

	FLib_MemCpy(evt, pPayload, sizeof(NWKU_EchoUDPConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_NWKU_CoapMsgReceivedIndication(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The notification for receiving a CoAP message
***************************************************************************************************/
static memStatus_t Load_NWKU_CoapMsgReceivedIndication(thrEvtContainer_t *container, uint8_t *pPayload)
{
	NWKU_CoapMsgReceivedIndication_t *evt = &(container->Data.NWKU_CoapMsgReceivedIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF92;

	evt->Status = (NWKU_CoapMsgReceivedIndication_Status_t)pPayload[idx]; idx++;
	FLib_MemCpy(evt->RemoteIpAddress, pPayload + idx, sizeof(evt->RemoteIpAddress)); idx += sizeof(evt->RemoteIpAddress);
	FLib_MemCpy(&(evt->UDPPort), pPayload + idx, sizeof(evt->UDPPort)); idx += sizeof(evt->UDPPort);
	evt->RequestType = (NWKU_CoapMsgReceivedIndication_RequestType_t)pPayload[idx]; idx++;
	evt->MessageType = (NWKU_CoapMsgReceivedIndication_MessageType_t)pPayload[idx]; idx++;
	FLib_MemCpy(evt->URIpath, pPayload + idx, 30); idx += 30;
	evt->PayloadLength = pPayload[idx]; idx++;

	if (evt->PayloadLength > 0)
	{
		evt->Payload = MEM_BufferAlloc(evt->PayloadLength);

		if (!evt->Payload)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Payload = NULL;
	}

	FLib_MemCpy(evt->Payload, pPayload + idx, evt->PayloadLength); idx += evt->PayloadLength;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_NWKU_DnsResponseReceivedIndication(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The notification for receiving a CoAP message
***************************************************************************************************/
static memStatus_t Load_NWKU_DnsResponseReceivedIndication(thrEvtContainer_t *container, uint8_t *pPayload)
{
	NWKU_DnsResponseReceivedIndication_t *evt = &(container->Data.NWKU_DnsResponseReceivedIndication);

	/* Store (OG, OC) in ID */
	container->id = 0xCF96;

	FLib_MemCpy(evt, pPayload, sizeof(NWKU_DnsResponseReceivedIndication_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_NWKU_CoapRegisterConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The confirmation for registering CoAP URI-path.
***************************************************************************************************/
static memStatus_t Load_NWKU_CoapRegisterConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	NWKU_CoapRegisterConfirm_t *evt = &(container->Data.NWKU_CoapRegisterConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF91;

	FLib_MemCpy(evt, pPayload, sizeof(NWKU_CoapRegisterConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_NWKU_CoapCreateInstanceConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The confirmation for createing a CoAP instance.
***************************************************************************************************/
static memStatus_t Load_NWKU_CoapCreateInstanceConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	NWKU_CoapCreateInstanceConfirm_t *evt = &(container->Data.NWKU_CoapCreateInstanceConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF93;

	FLib_MemCpy(evt, pPayload, sizeof(NWKU_CoapCreateInstanceConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_NWKU_CoapSendConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The confirmation for sending CoAP message.
***************************************************************************************************/
static memStatus_t Load_NWKU_CoapSendConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	NWKU_CoapSendConfirm_t *evt = &(container->Data.NWKU_CoapSendConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF90;

	FLib_MemCpy(evt, pPayload, sizeof(NWKU_CoapSendConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_NWKU_DnsSendRequestConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The confirmation for sending a DNS request message.
***************************************************************************************************/
static memStatus_t Load_NWKU_DnsSendRequestConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	NWKU_DnsSendRequestConfirm_t *evt = &(container->Data.NWKU_DnsSendRequestConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF95;

	FLib_MemCpy(evt, pPayload, sizeof(NWKU_DnsSendRequestConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_NWKU_McastGroupManageConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	The confirmation for sending a McastGroupManage request message.
***************************************************************************************************/
static memStatus_t Load_NWKU_McastGroupManageConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	NWKU_McastGroupManageConfirm_t *evt = &(container->Data.NWKU_McastGroupManageConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF71;

	FLib_MemCpy(evt, pPayload, sizeof(NWKU_McastGroupManageConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_NWKU_McastGroupShowResponse(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Response to McastGroupShow request.
***************************************************************************************************/
static memStatus_t Load_NWKU_McastGroupShowResponse(thrEvtContainer_t *container, uint8_t *pPayload)
{
	NWKU_McastGroupShowResponse_t *evt = &(container->Data.NWKU_McastGroupShowResponse);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF72;

	evt->CountIpAddresses = pPayload[idx]; idx++;

	if (evt->CountIpAddresses > 0)
	{
		evt->Addresses = MEM_BufferAlloc(evt->CountIpAddresses * 16);

		if (!evt->Addresses)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Addresses = NULL;
	}

	FLib_MemCpy(evt->Addresses, pPayload + idx, evt->CountIpAddresses * 16); idx += evt->CountIpAddresses * 16;

	return MEM_SUCCESS_c;
}

#endif  /* IPUTILS_ENABLE */

#if MESHCOP_ENABLE
/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_StartCommissionerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for starting a Commissioner
***************************************************************************************************/
static memStatus_t Load_MESHCOP_StartCommissionerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_StartCommissionerConfirm_t *evt = &(container->Data.MESHCOP_StartCommissionerConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF40;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_StartCommissionerConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_StartNativeCommissionerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for starting a Native Commissioner
***************************************************************************************************/
static memStatus_t Load_MESHCOP_StartNativeCommissionerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_StartNativeCommissionerConfirm_t *evt = &(container->Data.MESHCOP_StartNativeCommissionerConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF4F;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_StartNativeCommissionerConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_StopCommissionerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for stopping a Commissioner
***************************************************************************************************/
static memStatus_t Load_MESHCOP_StopCommissionerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_StopCommissionerConfirm_t *evt = &(container->Data.MESHCOP_StopCommissionerConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF41;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_StopCommissionerConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_AddExpectedJoinerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for adding a Joiner in the Commissioner list
***************************************************************************************************/
static memStatus_t Load_MESHCOP_AddExpectedJoinerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_AddExpectedJoinerConfirm_t *evt = &(container->Data.MESHCOP_AddExpectedJoinerConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF42;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_AddExpectedJoinerConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_GetExpectedJoinerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for getting information about Joiner from the Commissioner list
***************************************************************************************************/
static memStatus_t Load_MESHCOP_GetExpectedJoinerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_GetExpectedJoinerConfirm_t *evt = &(container->Data.MESHCOP_GetExpectedJoinerConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF43;

	evt->Status = (MESHCOP_GetExpectedJoinerConfirm_Status_t)pPayload[idx]; idx++;
	evt->Selected = (MESHCOP_GetExpectedJoinerConfirm_Selected_t)pPayload[idx]; idx++;
	evt->PSKdSize = pPayload[idx]; idx++;

	if (evt->PSKdSize > 0)
	{
		evt->PSKd = MEM_BufferAlloc(evt->PSKdSize);

		if (!evt->PSKd)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->PSKd = NULL;
	}

	FLib_MemCpy(evt->PSKd, pPayload + idx, evt->PSKdSize); idx += evt->PSKdSize;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_RemoveExpectedJoinerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for removing a Joiner from the Commissioner list
***************************************************************************************************/
static memStatus_t Load_MESHCOP_RemoveExpectedJoinerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_RemoveExpectedJoinerConfirm_t *evt = &(container->Data.MESHCOP_RemoveExpectedJoinerConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF44;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_RemoveExpectedJoinerConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_RemoveAllExpectedJoinersConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for removing all Joiners from the Commissioner's list
***************************************************************************************************/
static memStatus_t Load_MESHCOP_RemoveAllExpectedJoinersConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_RemoveAllExpectedJoinersConfirm_t *evt = &(container->Data.MESHCOP_RemoveAllExpectedJoinersConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF45;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_RemoveAllExpectedJoinersConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_SyncSteeringDataConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for syncing the steering data
***************************************************************************************************/
static memStatus_t Load_MESHCOP_SyncSteeringDataConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_SyncSteeringDataConfirm_t *evt = &(container->Data.MESHCOP_SyncSteeringDataConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF46;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_SyncSteeringDataConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmtSetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for the Management Set request
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmtSetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmtSetConfirm_t *evt = &(container->Data.MESHCOP_MgmtSetConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF4B;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_MgmtSetConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmtGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for the Management Get request
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmtGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmtGetConfirm_t *evt = &(container->Data.MESHCOP_MgmtGetConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF4C;

	evt->Status = (MESHCOP_MgmtGetConfirm_Status_t)pPayload[idx]; idx++;
	evt->Type = (MESHCOP_MgmtGetConfirm_Type_t)pPayload[idx]; idx++;
	evt->Length = pPayload[idx]; idx++;

	switch (evt->Type)
	{
		case MESHCOP_MgmtGetConfirm_Type_Channel:
			evt->Value.Channel = pPayload[idx]; idx++;
			break;

		case MESHCOP_MgmtGetConfirm_Type_ChannelMask:

			if (evt->Length > 0)
			{
				evt->Value.ChannelMask = MEM_BufferAlloc(evt->Length);

				if (!evt->Value.ChannelMask)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Value.ChannelMask = NULL;
			}

			FLib_MemCpy(evt->Value.ChannelMask, pPayload + idx, evt->Length); idx += evt->Length;
			break;

		case MESHCOP_MgmtGetConfirm_Type_PanId:

			if (evt->Length > 0)
			{
				evt->Value.PanId = MEM_BufferAlloc(evt->Length);

				if (!evt->Value.PanId)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Value.PanId = NULL;
			}

			FLib_MemCpy(evt->Value.PanId, pPayload + idx, evt->Length); idx += evt->Length;
			break;

		case MESHCOP_MgmtGetConfirm_Type_XpanId:

			if (evt->Length > 0)
			{
				evt->Value.XpanId = MEM_BufferAlloc(evt->Length);

				if (!evt->Value.XpanId)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Value.XpanId = NULL;
			}

			FLib_MemCpy(evt->Value.XpanId, pPayload + idx, evt->Length); idx += evt->Length;
			break;

		case MESHCOP_MgmtGetConfirm_Type_NetworkName:

			if (evt->Length > 0)
			{
				evt->Value.NetworkName = MEM_BufferAlloc(evt->Length);

				if (!evt->Value.NetworkName)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Value.NetworkName = NULL;
			}

			FLib_MemCpy(evt->Value.NetworkName, pPayload + idx, evt->Length); idx += evt->Length;
			break;

		case MESHCOP_MgmtGetConfirm_Type_PSKc:

			if (evt->Length > 0)
			{
				evt->Value.PSKc = MEM_BufferAlloc(evt->Length);

				if (!evt->Value.PSKc)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Value.PSKc = NULL;
			}

			FLib_MemCpy(evt->Value.PSKc, pPayload + idx, evt->Length); idx += evt->Length;
			break;

		case MESHCOP_MgmtGetConfirm_Type_MasterKey:

			if (evt->Length > 0)
			{
				evt->Value.MasterKey = MEM_BufferAlloc(evt->Length);

				if (!evt->Value.MasterKey)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Value.MasterKey = NULL;
			}

			FLib_MemCpy(evt->Value.MasterKey, pPayload + idx, evt->Length); idx += evt->Length;
			break;

		case MESHCOP_MgmtGetConfirm_Type_KeySequence:

			if (evt->Length > 0)
			{
				evt->Value.KeySequence = MEM_BufferAlloc(evt->Length);

				if (!evt->Value.KeySequence)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Value.KeySequence = NULL;
			}

			FLib_MemCpy(evt->Value.KeySequence, pPayload + idx, evt->Length); idx += evt->Length;
			break;

		case MESHCOP_MgmtGetConfirm_Type_MeshLocalUla:

			if (evt->Length > 0)
			{
				evt->Value.MeshLocalUla = MEM_BufferAlloc(evt->Length);

				if (!evt->Value.MeshLocalUla)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Value.MeshLocalUla = NULL;
			}

			FLib_MemCpy(evt->Value.MeshLocalUla, pPayload + idx, evt->Length); idx += evt->Length;
			break;

		case MESHCOP_MgmtGetConfirm_Type_SteeringData:

			if (evt->Length > 0)
			{
				evt->Value.SteeringData = MEM_BufferAlloc(evt->Length);

				if (!evt->Value.SteeringData)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Value.SteeringData = NULL;
			}

			FLib_MemCpy(evt->Value.SteeringData, pPayload + idx, evt->Length); idx += evt->Length;
			break;

		case MESHCOP_MgmtGetConfirm_Type_BorderRouterLocator:

			if (evt->Length > 0)
			{
				evt->Value.BorderRouterLocator = MEM_BufferAlloc(evt->Length);

				if (!evt->Value.BorderRouterLocator)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Value.BorderRouterLocator = NULL;
			}

			FLib_MemCpy(evt->Value.BorderRouterLocator, pPayload + idx, evt->Length); idx += evt->Length;
			break;

		case MESHCOP_MgmtGetConfirm_Type_CommissionerID:

			if (evt->Length > 0)
			{
				evt->Value.CommissionerID = MEM_BufferAlloc(evt->Length);

				if (!evt->Value.CommissionerID)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Value.CommissionerID = NULL;
			}

			FLib_MemCpy(evt->Value.CommissionerID, pPayload + idx, evt->Length); idx += evt->Length;
			break;

		case MESHCOP_MgmtGetConfirm_Type_CommissionerSessionID:

			if (evt->Length > 0)
			{
				evt->Value.CommissionerSessionID = MEM_BufferAlloc(evt->Length);

				if (!evt->Value.CommissionerSessionID)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Value.CommissionerSessionID = NULL;
			}

			FLib_MemCpy(evt->Value.CommissionerSessionID, pPayload + idx, evt->Length); idx += evt->Length;
			break;

		case MESHCOP_MgmtGetConfirm_Type_SecurityPolicy:

			if (evt->Length > 0)
			{
				evt->Value.SecurityPolicy = MEM_BufferAlloc(evt->Length);

				if (!evt->Value.SecurityPolicy)
				{
					return MEM_ALLOC_ERROR_c;
				}

			}
			else
			{
				evt->Value.SecurityPolicy = NULL;
			}

			FLib_MemCpy(evt->Value.SecurityPolicy, pPayload + idx, evt->Length); idx += evt->Length;
			break;

		case MESHCOP_MgmtGetConfirm_Type_DelayTimer:
			break;

		case MESHCOP_MgmtGetConfirm_Type_ActiveTimestamp:
			break;

		case MESHCOP_MgmtGetConfirm_Type_PendingTimestamp:
			break;
	}

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_SetCommissionerCredentialConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for setting up a Native Commissioner credential
***************************************************************************************************/
static memStatus_t Load_MESHCOP_SetCommissionerCredentialConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_SetCommissionerCredentialConfirm_t *evt = &(container->Data.MESHCOP_SetCommissionerCredentialConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF4D;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_SetCommissionerCredentialConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmNwkFormConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Network form confirmation message
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmNwkFormConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmNwkFormConfirm_t *evt = &(container->Data.MESHCOP_MgmNwkFormConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF47;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_MgmNwkFormConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmtCommissionerGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for the Management Commissioner Get request
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmtCommissionerGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmtCommissionerGetConfirm_t *evt = &(container->Data.MESHCOP_MgmtCommissionerGetConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCFA0;

	evt->Status = (MESHCOP_MgmtCommissionerGetConfirm_Status_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Length), pPayload + idx, sizeof(evt->Length)); idx += sizeof(evt->Length);

	if (evt->Length > 0)
	{
		evt->TLVs = MEM_BufferAlloc(evt->Length);

		if (!evt->TLVs)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->TLVs = NULL;
	}

	FLib_MemCpy(evt->TLVs, pPayload + idx, evt->Length); idx += evt->Length;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmtActiveGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for the Management Active Get request
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmtActiveGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmtActiveGetConfirm_t *evt = &(container->Data.MESHCOP_MgmtActiveGetConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCFA2;

	evt->Status = (MESHCOP_MgmtActiveGetConfirm_Status_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Length), pPayload + idx, sizeof(evt->Length)); idx += sizeof(evt->Length);

	if (evt->Length > 0)
	{
		evt->TLVs = MEM_BufferAlloc(evt->Length);

		if (!evt->TLVs)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->TLVs = NULL;
	}

	FLib_MemCpy(evt->TLVs, pPayload + idx, evt->Length); idx += evt->Length;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmtPendingGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for the Management Pending Get request
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmtPendingGetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmtPendingGetConfirm_t *evt = &(container->Data.MESHCOP_MgmtPendingGetConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCFA4;

	evt->Status = (MESHCOP_MgmtPendingGetConfirm_Status_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Length), pPayload + idx, sizeof(evt->Length)); idx += sizeof(evt->Length);

	if (evt->Length > 0)
	{
		evt->TLVs = MEM_BufferAlloc(evt->Length);

		if (!evt->TLVs)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->TLVs = NULL;
	}

	FLib_MemCpy(evt->TLVs, pPayload + idx, evt->Length); idx += evt->Length;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmtCommissionerSetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for the MESHCOP_MgmtCommissionerSet.Request
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmtCommissionerSetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmtCommissionerSetConfirm_t *evt = &(container->Data.MESHCOP_MgmtCommissionerSetConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCFA1;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_MgmtCommissionerSetConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmtActiveSetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for the MESHCOP_MgmtActiveSet.Request
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmtActiveSetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmtActiveSetConfirm_t *evt = &(container->Data.MESHCOP_MgmtActiveSetConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCFA3;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_MgmtActiveSetConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmtPendingSetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for the MESHCOP_MgmtPendingSet.Request
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmtPendingSetConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmtPendingSetConfirm_t *evt = &(container->Data.MESHCOP_MgmtPendingSetConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCFA5;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_MgmtPendingSetConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmtSendPanIdQueryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for the Pan ID query
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmtSendPanIdQueryConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmtSendPanIdQueryConfirm_t *evt = &(container->Data.MESHCOP_MgmtSendPanIdQueryConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCFA8;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_MgmtSendPanIdQueryConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmtPanIdConflictConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Pan ID conflict detected
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmtPanIdConflictConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmtPanIdConflictConfirm_t *evt = &(container->Data.MESHCOP_MgmtPanIdConflictConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCFA9;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_MgmtPanIdConflictConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmtSendEdScanConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for the ED scan
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmtSendEdScanConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmtSendEdScanConfirm_t *evt = &(container->Data.MESHCOP_MgmtSendEdScanConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCFAA;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_MgmtSendEdScanConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmtEdReportConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Energy detect report
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmtEdReportConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmtEdReportConfirm_t *evt = &(container->Data.MESHCOP_MgmtEdReportConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCFAB;

	evt->Status = (MESHCOP_MgmtEdReportConfirm_Status_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->ScanChannelMask), pPayload + idx, sizeof(evt->ScanChannelMask)); idx += sizeof(evt->ScanChannelMask);
	evt->Length = pPayload[idx]; idx++;

	if (evt->Length > 0)
	{
		evt->EnergyList = MEM_BufferAlloc(evt->Length);

		if (!evt->EnergyList)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->EnergyList = NULL;
	}

	FLib_MemCpy(evt->EnergyList, pPayload + idx, evt->Length); idx += evt->Length;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MESHCOP_MgmtSendAnnounceBeginConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Confirmation for the MESHCOP_MgmtSendAnnounceBegin.Request
***************************************************************************************************/
static memStatus_t Load_MESHCOP_MgmtSendAnnounceBeginConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MESHCOP_MgmtSendAnnounceBeginConfirm_t *evt = &(container->Data.MESHCOP_MgmtSendAnnounceBeginConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCFA7;

	FLib_MemCpy(evt, pPayload, sizeof(MESHCOP_MgmtSendAnnounceBeginConfirm_t));

	return MEM_SUCCESS_c;
}

#endif  /* MESHCOP_ENABLE */

#if DTLS_ENABLE
/*!*************************************************************************************************
\fn		static memStatus_t Load_DTLSOpenConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	DTLS-Open.Confirm
***************************************************************************************************/
static memStatus_t Load_DTLSOpenConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	DTLSOpenConfirm_t *evt = &(container->Data.DTLSOpenConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF30;

	FLib_MemCpy(evt, pPayload, sizeof(DTLSOpenConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_DTLSCloseConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	DTLS-Close.Confirm
***************************************************************************************************/
static memStatus_t Load_DTLSCloseConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	DTLSCloseConfirm_t *evt = &(container->Data.DTLSCloseConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF31;

	FLib_MemCpy(evt, pPayload, sizeof(DTLSCloseConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_DTLSClosePeerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	DTLS-ClosePeer.Confirm
***************************************************************************************************/
static memStatus_t Load_DTLSClosePeerConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	DTLSClosePeerConfirm_t *evt = &(container->Data.DTLSClosePeerConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF32;

	FLib_MemCpy(evt, pPayload, sizeof(DTLSClosePeerConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_DTLSConnectConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	DTLS-Connect.Confirm
***************************************************************************************************/
static memStatus_t Load_DTLSConnectConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	DTLSConnectConfirm_t *evt = &(container->Data.DTLSConnectConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF33;

	FLib_MemCpy(evt, pPayload, sizeof(DTLSConnectConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_DTLSClientConnectedConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	DTLS-ClientConnected.Confirm
***************************************************************************************************/
static memStatus_t Load_DTLSClientConnectedConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	DTLSClientConnectedConfirm_t *evt = &(container->Data.DTLSClientConnectedConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF34;

	FLib_MemCpy(evt, pPayload, sizeof(DTLSClientConnectedConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_DTLSSendConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	DTLS-Send.Confirm
***************************************************************************************************/
static memStatus_t Load_DTLSSendConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	DTLSSendConfirm_t *evt = &(container->Data.DTLSSendConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF35;

	FLib_MemCpy(evt, pPayload, sizeof(DTLSSendConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_DTLSReceiveConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	DTLS-Receive.Confirm
***************************************************************************************************/
static memStatus_t Load_DTLSReceiveConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	DTLSReceiveConfirm_t *evt = &(container->Data.DTLSReceiveConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCF36;

	evt->PeerIndex = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->Size), pPayload + idx, sizeof(evt->Size)); idx += sizeof(evt->Size);

	if (evt->Size > 0)
	{
		evt->Data = MEM_BufferAlloc(evt->Size);

		if (!evt->Data)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->Data = NULL;
	}

	FLib_MemCpy(evt->Data, pPayload + idx, evt->Size); idx += evt->Size;

	return MEM_SUCCESS_c;
}

#endif  /* DTLS_ENABLE */

#if OTHERUTILS_ENABLE
/*!*************************************************************************************************
\fn		static memStatus_t Load_FSCIGetUniqueIdConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Response for Unique ID request
***************************************************************************************************/
static memStatus_t Load_FSCIGetUniqueIdConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIGetUniqueIdConfirm_t *evt = &(container->Data.FSCIGetUniqueIdConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xA4B0;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIGetUniqueIdConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_FSCIGetMcuIdConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Response for MCU ID request
***************************************************************************************************/
static memStatus_t Load_FSCIGetMcuIdConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIGetMcuIdConfirm_t *evt = &(container->Data.FSCIGetMcuIdConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xA4B1;

	FLib_MemCpy(evt, pPayload, sizeof(FSCIGetMcuIdConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_FSCIGetSwVersionsConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	FSCI-GetSwVersions.Confirm description
***************************************************************************************************/
static memStatus_t Load_FSCIGetSwVersionsConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	FSCIGetSwVersionsConfirm_t *evt = &(container->Data.FSCIGetSwVersionsConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xA4B2;

	evt->listSize = pPayload[idx]; idx++;

	if (evt->listSize > 0)
	{
		evt->SwVersions = MEM_BufferAlloc(evt->listSize * 6);

		if (!evt->SwVersions)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->SwVersions = NULL;
	}

	FLib_MemCpy(evt->SwVersions, pPayload + idx, evt->listSize * 6); idx += evt->listSize * 6;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_Sniffer_MacSetPIBAttributeConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Sniffer_MacSetPIBAttribute.Confirm description
***************************************************************************************************/
static memStatus_t Load_Sniffer_MacSetPIBAttributeConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	Sniffer_MacSetPIBAttributeConfirm_t *evt = &(container->Data.Sniffer_MacSetPIBAttributeConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x840D;

	evt->Status = (Sniffer_MacSetPIBAttributeConfirm_Status_t)pPayload[idx]; idx++;
	evt->PIBAttribute = (Sniffer_MacSetPIBAttributeConfirm_PIBAttribute_t)pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->DataLength), pPayload + idx, sizeof(evt->DataLength)); idx += sizeof(evt->DataLength);

	if (evt->DataLength > 0)
	{
		evt->PIBAttributeValue = MEM_BufferAlloc(evt->DataLength);

		if (!evt->PIBAttributeValue)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->PIBAttributeValue = NULL;
	}

	FLib_MemCpy(evt->PIBAttributeValue, pPayload + idx, evt->DataLength); idx += evt->DataLength;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_MAC_PromiscuousRxIndication(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	MAC_PromiscuousRx.Indication Rx description
***************************************************************************************************/
static memStatus_t Load_MAC_PromiscuousRxIndication(thrEvtContainer_t *container, uint8_t *pPayload)
{
	MAC_PromiscuousRxIndication_t *evt = &(container->Data.MAC_PromiscuousRxIndication);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0x8603;

	evt->LinkQuality = pPayload[idx]; idx++;
	FLib_MemCpy(&(evt->TimeStamp), pPayload + idx, sizeof(evt->TimeStamp)); idx += sizeof(evt->TimeStamp);
	evt->msduLength = pPayload[idx]; idx++;

	if (evt->msduLength > 0)
	{
		evt->msdu = MEM_BufferAlloc(evt->msduLength);

		if (!evt->msdu)
		{
			return MEM_ALLOC_ERROR_c;
		}

	}
	else
	{
		evt->msdu = NULL;
	}

	FLib_MemCpy(evt->msdu, pPayload + idx, evt->msduLength); idx += evt->msduLength;

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_SerialTun_IPPacketReceivedConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	Received an IP packet from the VTUN interface
***************************************************************************************************/
static memStatus_t Load_SerialTun_IPPacketReceivedConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	SerialTun_IPPacketReceivedConfirm_t *evt = &(container->Data.SerialTun_IPPacketReceivedConfirm);

	uint32_t idx = 0;

	/* Store (OG, OC) in ID */
	container->id = 0xCFF3;

	evt->Size = pPayload[0] + (pPayload[1] << 8); idx += 2;
	evt->IPpayload = MEM_BufferAlloc(evt->Size);

	if (!evt->IPpayload)
	{
		return MEM_ALLOC_ERROR_c;
	}

	FLib_MemCpy(evt->IPpayload, pPayload + idx, evt->Size);

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_AspSetPowerLevelConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	AspSetPowerLevel.Confirm description
***************************************************************************************************/
static memStatus_t Load_AspSetPowerLevelConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	AspSetPowerLevelConfirm_t *evt = &(container->Data.AspSetPowerLevelConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0x940F;

	FLib_MemCpy(evt, pPayload, sizeof(AspSetPowerLevelConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_AspGetPowerLevelConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	AspGetPowerLevel.Confirm description
***************************************************************************************************/
static memStatus_t Load_AspGetPowerLevelConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	AspGetPowerLevelConfirm_t *evt = &(container->Data.AspGetPowerLevelConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0x941F;

	FLib_MemCpy(evt, pPayload, sizeof(AspGetPowerLevelConfirm_t));

	return MEM_SUCCESS_c;
}

/*!*************************************************************************************************
\fn		static memStatus_t Load_DBGConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
\brief	DBG messages
***************************************************************************************************/
static memStatus_t Load_DBGConfirm(thrEvtContainer_t *container, uint8_t *pPayload)
{
	DBGConfirm_t *evt = &(container->Data.DBGConfirm);

	/* Store (OG, OC) in ID */
	container->id = 0xCF89;

	FLib_MemCpy(evt, pPayload, sizeof(DBGConfirm_t));

	return MEM_SUCCESS_c;
}

#endif  /* OTHERUTILS_ENABLE */


void KHC_ThreadIP_RX_MsgHandler(void *pData, void *param, uint32_t fsciInterface)
{
	if (!pData || !param)
	{
		return;
	}

	thrEvtContainer_t *container = (thrEvtContainer_t *)param;
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
			if (id == 0xCFF3)
			{
				/* SerialTUN packet received indication does not include a size, so go back two
				   bytes to load it properly. */
				pPayload -= 2;
			}

                    LOGD(">>[thEvent:%x]\r\n", id);
                    evtHandlerTbl[i].handlerFunc(container, pPayload);
                    LOGD("<<[thEvent:%x]\r\n", id);
			break;
		}
	}
}

/*!*************************************************************************************************
   INCLUDES AND EXTERN VARIABLES FOR GENERATED PRINT EVENT HANDLER
***************************************************************************************************/
#include "network_utils.h"
#include "shell_utils.h"
#include "shell_threadip.h"
#if gHybridApp_d
    #include "shell_ble.h"
#endif

extern uint32_t                gThreadInstanceId;
extern bool_t                  gContinuousPing;
extern uint8_t                 gCoAPInstanceId;
extern bool_t                  gAutostartInProgress;
extern shellAutostartStates_t  gAutostartState;
extern uint8_t                 gAutostartChannel;


/*!*************************************************************************************************
   START OF GENERATED PRINT EVENT HANDLER
***************************************************************************************************/
static char *const OK = "OK";
static char *const ERROR = "ERROR";
static char *const Success = "Success";
static char *const Notpermitted = "Notpermitted";
static char *const Nomemory = "Nomemory";
static char *const Invalidinstance = "Invalidinstance";
static char *const Invalidparameter = "Invalidparameter";
static char *const UnsupportedAttribute = "UnsupportedAttribute";
static char *const EmptyEntry = "EmptyEntry";
static char *const InvalidValue = "InvalidValue";
static char *const InvalidParam = "InvalidParam";
static char *const Theselectedconfigurationisnotvalid = "Theselectedconfigurationisnotvalid";
static char *const AlreadyConnected = "AlreadyConnected";
static char *const AlreadyCreated = "AlreadyCreated";
static char *const ResetCpuSuccess = "ResetCpuSuccess";
static char *const ResetCpuPending = "ResetCpuPending";
static char *const ScanResult = "ScanResult";
static char *const EnergyDetect = "EnergyDetect";
static char *const ActiveScan = "ActiveScan";
static char *const EnergyDetectAndActiveScan = "EnergyDetectAndActiveScan";
static char *const Failed = "Failed";
static char *const SelectBestChannel = "SelectBestChannel";
static char *const GeneratePSKc = "GeneratePSKc";
static char *const Attaching = "Attaching";
static char *const JoinSuccess = "JoinSuccess";
static char *const JoinFailed = "JoinFailed";
static char *const ScanStarted = "ScanStarted";
static char *const ReceivedBeacon = "ReceivedBeacon";
static char *const ScanEnded = "ScanEnded";
static char *const Disconnected = "Disconnected";
static char *const Connected = "Connected";
static char *const Resettofactorydefault = "Resettofactorydefault";
static char *const Instancerestorestarted = "Instancerestorestarted";
static char *const RouterSynced = "RouterSynced";
static char *const EndDeviceSynced = "EndDeviceSynced";
static char *const Connectingstarted = "Connectingstarted";
static char *const Connectingfailed = "Connectingfailed";
static char *const Connectingdeffered = "Connectingdeffered";
static char *const DeviceisLeader = "DeviceisLeader";
static char *const DeviceisRouter = "DeviceisRouter";
static char *const DeviceisREED = "DeviceisREED";
static char *const DeviceisEndDevice = "DeviceisEndDevice";
static char *const DeviceisSleepyEndDevice = "DeviceisSleepyEndDevice";
static char *const RequestingGlobalAddress = "RequestingGlobalAddress";
static char *const GlobalAddressassigned = "GlobalAddressassigned";
static char *const RequestingRouterId = "RequestingRouterId";
static char *const RouterIdassigned = "RouterIdassigned";
static char *const RouterIdassignedfailed = "RouterIdassignedfailed";
static char *const Allowdevicetosleep = "Allowdevicetosleep";
static char *const Disallowdevicetosleep = "Disallowdevicetosleep";
static char *const ChildIdassigned = "ChildIdassigned";
static char *const JoinerDiscoveryStarted = "JoinerDiscoveryStarted";
static char *const JoinerDiscoveryFailedNoBeacon = "JoinerDiscoveryFailedNoBeacon";
static char *const JoinerDiscoveryFailedFiltered = "JoinerDiscoveryFailedFiltered";
static char *const JoinerDiscoverySuccess = "JoinerDiscoverySuccess";
static char *const JoinerDtlsSessionStarted = "JoinerDtlsSessionStarted";
static char *const JoinerDtlsError = "JoinerDtlsError";
static char *const JoinerError = "JoinerError";
static char *const JoinerAccepted = "JoinerAccepted";
static char *const CommissionerPetitionStarted = "CommissionerPetitionStarted";
static char *const CommissionerPetitionAccepted = "CommissionerPetitionAccepted";
static char *const CommissionerPetitionRejected = "CommissionerPetitionRejected";
static char *const CommissionerPetitionError = "CommissionerPetitionError";
static char *const CommissionerKeepAliveSent = "CommissionerKeepAliveSent";
static char *const CommissionerError = "CommissionerError";
static char *const CommissionerJoinerDtlsSessionStarted = "CommissionerJoinerDtlsSessionStarted";
static char *const CommissionerJoinerDtlsError = "CommissionerJoinerDtlsError";
static char *const CommissionerJoinerAccepted = "CommissionerJoinerAccepted";
static char *const CommissionerNwkDataSynced = "CommissionerNwkDataSynced";
static char *const CommissionerBrDtlsSessionStarted = "CommissionerBrDtlsSessionStarted";
static char *const CommissionerBrDtlsError = "CommissionerBrDtlsError";
static char *const CommissionerBrError = "CommissionerBrError";
static char *const CommissionerBrAccepted = "CommissionerBrAccepted";
static char *const BrCommissionerDtlsSessionStarted = "BrCommissionerDtlsSessionStarted";
static char *const BrCommissionerDtlsError = "BrCommissionerDtlsError";
static char *const BrCommissionerAccepted = "BrCommissionerAccepted";
static char *const BrCommissionerDataRelayedInbound = "BrCommissionerDataRelayedInbound";
static char *const BrCommissionerDataRelayedOutbound = "BrCommissionerDataRelayedOutbound";
static char *const JoinerrouterJoinerDataRelayedInbound = "JoinerrouterJoinerDataRelayedInbound";
static char *const JoinerrouterJoinerDataRelayedOutbound = "JoinerrouterJoinerDataRelayedOutbound";
static char *const JoinerrouterJoinerAccepted = "JoinerrouterJoinerAccepted";
static char *const StartVendorProvisioning = "StartVendorProvisioning";
static char *const OUT = "OUT";
static char *const IN = "IN";
static char *const JOIN_FIN_REQ = "JOIN_FIN_REQ";
static char *const JOIN_FIN_RSP = "JOIN_FIN_RSP";
static char *const JOIN_ENT_REQ = "JOIN_ENT_REQ";
static char *const JOIN_ENT_RSP = "JOIN_ENT_RSP";
static char *const DTLS_CLOSE_NOTIFY = "DTLS_CLOSE_NOTIFY";
static char *const InvalidParameter = "InvalidParameter";
static char *const NotPermitted = "NotPermitted";
static char *const NoMemory = "NoMemory";
static char *const Error = "Error";
static char *const InvalidInstance = "InvalidInstance";
static char *const NotStarted = "NotStarted";
static char *const FailedNotsupported = "FailedNotsupported";
static char *const FailedNotallowed = "FailedNotallowed";
static char *const NeighborNotFound = "NeighborNotFound";
static char *const Addressesmaximumlimitreached = "Addressesmaximumlimitreached";
static char *const RequestTimeout = "RequestTimeout";
static char *const Wrongdestinationaddress = "Wrongdestinationaddress";
static char *const Duplicate = "Duplicate";
static char *const FALSE_str = "FALSE";
static char *const TRUE_str = "TRUE";

/*!*************************************************************************************************
\fn     void SHELL_ThrEventNotify(void *param)
\brief  Print to host's shell information about Thread events coming from black-box

\param  [in]    param       the received event
***************************************************************************************************/
void SHELL_ThrEventNotify(void *param)
{
    thrEvtContainer_t *container = (thrEvtContainer_t *)param;

    SHELL_NEWLINE();

    switch (container->id)
    {

#if SOCKET_ENABLE

        case 0xCF00:
            shell_write("SocketCreateConfirm");
            break;

        case 0xCF01:
            shell_write("SocketShutdownConfirm");
            shell_write(" -> ");

            switch (container->Data.SocketShutdownConfirm.Status)
            {
                case SocketShutdownConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case SocketShutdownConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.SocketShutdownConfirm.Status);
                    break;
            }

            break;

        case 0xCF02:
            shell_write("SocketBindConfirm");
            shell_write(" -> ");

            switch (container->Data.SocketBindConfirm.Status)
            {
                case SocketBindConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case SocketBindConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.SocketBindConfirm.Status);
                    break;
            }

            break;

        case 0xCF03:
            shell_write("SocketSendConfirm");
            shell_write(" -> ");

            switch (container->Data.SocketSendConfirm.Status)
            {
                case SocketSendConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case SocketSendConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.SocketSendConfirm.Status);
                    break;
            }

            break;

        case 0xCF04:
            shell_write("SocketSendToConfirm");
            shell_write(" -> ");

            switch (container->Data.SocketSendToConfirm.Status)
            {
                case SocketSendToConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case SocketSendToConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.SocketSendToConfirm.Status);
                    break;
            }

            break;

        case 0xCF05:
            shell_write("SocketReceiveConfirm");
            break;

        case 0xCF06:
            shell_write("SocketReceiveFromConfirm");
            shell_write(" -> ");

            switch (container->Data.SocketReceiveFromConfirm.Status)
            {
                case SocketReceiveFromConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case SocketReceiveFromConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.SocketReceiveFromConfirm.Status);
                    break;
            }

            break;

        case 0xCF07:
            shell_write("SocketConnectConfirm");
            shell_write(" -> ");

            switch (container->Data.SocketConnectConfirm.Status)
            {
                case SocketConnectConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case SocketConnectConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.SocketConnectConfirm.Status);
                    break;
            }

            break;

        case 0xCF08:
            shell_write("SocketListenConfirm");
            shell_write(" -> ");

            switch (container->Data.SocketListenConfirm.Status)
            {
                case SocketListenConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case SocketListenConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.SocketListenConfirm.Status);
                    break;
            }

            break;

        case 0xCF09:
            shell_write("SocketAcceptConfirm");
            shell_write(" -> ");

            switch (container->Data.SocketAcceptConfirm.Status)
            {
                case SocketAcceptConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case SocketAcceptConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.SocketAcceptConfirm.Status);
                    break;
            }

            break;

        case 0xCF0A:
            shell_write("SocketSetOptionConfirm");
            shell_write(" -> ");

            switch (container->Data.SocketSetOptionConfirm.Status)
            {
                case SocketSetOptionConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case SocketSetOptionConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.SocketSetOptionConfirm.Status);
                    break;
            }

            break;

        case 0xCF0B:
            shell_write("SocketGetOptionConfirm");
            shell_write(" -> ");

            switch (container->Data.SocketGetOptionConfirm.Status)
            {
                case SocketGetOptionConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case SocketGetOptionConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.SocketGetOptionConfirm.Status);
                    break;
            }

            break;

#endif  /* SOCKET_ENABLE */

#if MAC_ENABLE

        case 0xCF12:
            shell_write("MAC_MacFilteringAddEntryConfirm");
            shell_write(" -> ");

            switch (container->Data.MAC_MacFilteringAddEntryConfirm.Status)
            {
                case MAC_MacFilteringAddEntryConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MAC_MacFilteringAddEntryConfirm_Status_Notpermitted:
                    shell_write(Notpermitted);
                    break;

                case MAC_MacFilteringAddEntryConfirm_Status_Nomemory:
                    shell_write(Nomemory);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MAC_MacFilteringAddEntryConfirm.Status);
                    break;
            }

            break;

        case 0xCF13:
            shell_write("MAC_MacFilteringRemoveEntryConfirm");
            shell_write(" -> ");

            switch (container->Data.MAC_MacFilteringRemoveEntryConfirm.Status)
            {
                case MAC_MacFilteringRemoveEntryConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MAC_MacFilteringRemoveEntryConfirm_Status_Notpermitted:
                    shell_write(Notpermitted);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MAC_MacFilteringRemoveEntryConfirm.Status);
                    break;
            }

            break;

        case 0xCF14:
            shell_write("MAC_MacFilteringEnableConfirm");
            shell_write(" -> ");

            switch (container->Data.MAC_MacFilteringEnableConfirm.Status)
            {
                case MAC_MacFilteringEnableConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MAC_MacFilteringEnableConfirm_Status_Notpermitted:
                    shell_write(Notpermitted);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MAC_MacFilteringEnableConfirm.Status);
                    break;
            }

            break;

        case 0xCF15:
            shell_write("MAC_MacFilteringGetTableConfirm");
            break;

#endif  /* MAC_ENABLE */

#if THREADMGMT_ENABLE

        case 0xCF16:
            shell_write("THR_SetDeviceConfigConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_SetDeviceConfigConfirm.Status)
            {
                case THR_SetDeviceConfigConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_SetDeviceConfigConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_SetDeviceConfigConfirm_Status_Invalidparameter:
                    shell_write(Invalidparameter);
                    break;

                case THR_SetDeviceConfigConfirm_Status_Notpermitted:
                    shell_write(Notpermitted);
                    break;

                case THR_SetDeviceConfigConfirm_Status_UnsupportedAttribute:
                    shell_write(UnsupportedAttribute);
                    break;

                case THR_SetDeviceConfigConfirm_Status_EmptyEntry:
                    shell_write(EmptyEntry);
                    break;

                case THR_SetDeviceConfigConfirm_Status_InvalidValue:
                    shell_write(InvalidValue);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_SetDeviceConfigConfirm.Status);
                    break;
            }

            break;

        case 0xCF1A:
            shell_write("THR_NwkScanConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_NwkScanConfirm.Status)
            {
                case THR_NwkScanConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_NwkScanConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_NwkScanConfirm_Status_InvalidParam:
                    shell_write(InvalidParam);
                    break;

                case THR_NwkScanConfirm_Status_Nomemory:
                    shell_write(Nomemory);
                    break;

                case THR_NwkScanConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_NwkScanConfirm.Status);
                    break;
            }

            break;

        case 0xCF1B:
            shell_write("THR_CreateNwkConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_CreateNwkConfirm.Status)
            {
                case THR_CreateNwkConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case THR_CreateNwkConfirm_Status_AlreadyConnected:
                    shell_write(AlreadyConnected);
                    break;

                case THR_CreateNwkConfirm_Status_AlreadyCreated:
                    shell_write(AlreadyCreated);
                    break;

                case THR_CreateNwkConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_CreateNwkConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_CreateNwkConfirm.Status);
                    break;
            }

            break;

        case 0xCF1C:
            shell_write("THR_JoinConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_JoinConfirm.Status)
            {
                case THR_JoinConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case THR_JoinConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_JoinConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_JoinConfirm.Status);
                    break;
            }

            break;

        case 0xCF1F:
            shell_write("THR_FactoryResetConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_FactoryResetConfirm.Status)
            {
                case THR_FactoryResetConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_FactoryResetConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_FactoryResetConfirm.Status);
                    break;
            }

            break;

        case 0xCF21:
            shell_write("THR_CpuResetConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_CpuResetConfirm.Status)
            {
                case THR_CpuResetConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_CpuResetConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_CpuResetConfirm.Status);
                    break;
            }

            break;

        case 0xCF22:
            shell_write("THR_CpuResetIndication");
            shell_write(" -> ");

            switch (container->Data.THR_CpuResetIndication.Status)
            {
                case THR_CpuResetIndication_Status_ResetCpuSuccess:
                    shell_write(ResetCpuSuccess);
                    MEM_BufferFree(container->Data.THR_CpuResetIndication.ResetCpuPayload.ResetCpuSuccess.BoardName);
                    MEM_BufferFree(container->Data.THR_CpuResetIndication.ResetCpuPayload.ResetCpuSuccess.SwVersion);

                    /* invalidate the CoAP instance ID */
                    gCoAPInstanceId = THR_ALL_FFs8;
#if gHybridApp_d
                    SHELL_BLE_Terminate();
#endif

                    break;

                case THR_CpuResetIndication_Status_ResetCpuPending:
                    shell_write(ResetCpuPending);
                    shell_printf(" in %u milliseconds", container->Data.THR_CpuResetIndication.ResetCpuPayload.ResetCpuPending_TimeoutMs);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_CpuResetIndication.Status);
                    break;
            }

            break;

        case 0xCF1D:
            shell_write("THR_DisconnectConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_DisconnectConfirm.Status)
            {
                case THR_DisconnectConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case THR_DisconnectConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_DisconnectConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_DisconnectConfirm.Status);
                    break;
            }

            break;

        case 0xCF80:
            shell_write("THR_AttachConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_AttachConfirm.Status)
            {
                case THR_AttachConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case THR_AttachConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_AttachConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_AttachConfirm.Status);
                    break;
            }

            break;

        case 0xCF94:
            shell_write("THR_PromoteAsRouterConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_PromoteAsRouterConfirm.Status)
            {
                case THR_PromoteAsRouterConfirm_Status_OK:
                    shell_write(OK);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_PromoteAsRouterConfirm.Status);
                    break;
            }

            break;

        case 0xCF50:
            shell_write("THR_EventNwkScanConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_EventNwkScanConfirm.EventStatus)
            {
                case THR_EventNwkScanConfirm_EventStatus_ScanResult:
                    shell_write(ScanResult);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.THR_EventNwkScanConfirm.EventStatus);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.THR_EventNwkScanConfirm.ScanType)
            {
                case THR_EventNwkScanConfirm_ScanType_EnergyDetect:
                    shell_write(EnergyDetect);
                    break;

                case THR_EventNwkScanConfirm_ScanType_ActiveScan:
                    shell_write(ActiveScan);
                    break;

                case THR_EventNwkScanConfirm_ScanType_EnergyDetectAndActiveScan:
                    shell_write(EnergyDetectAndActiveScan);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_EventNwkScanConfirm.ScanType);
                    break;
            }

            SHELL_NwkScanPrint(&container->Data.THR_EventNwkScanConfirm);
            break;

        case 0xCF51:
            shell_write("THR_EventNwkCreateConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_EventNwkCreateConfirm.EventStatus)
            {
                case THR_EventNwkCreateConfirm_EventStatus_Success:
                    shell_write(Success);
                    break;

                case THR_EventNwkCreateConfirm_EventStatus_Failed:
                    shell_write(Failed);
                    break;

                case THR_EventNwkCreateConfirm_EventStatus_SelectBestChannel:
                    shell_write(SelectBestChannel);
                    break;

                case THR_EventNwkCreateConfirm_EventStatus_GeneratePSKc:
                    shell_write(GeneratePSKc);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.THR_EventNwkCreateConfirm.EventStatus);
                    break;
            }

            MEM_BufferFree(container->Data.THR_EventNwkCreateConfirm.Data);
            break;

        case 0xCF52:
            shell_write("THR_EventNwkJoinConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_EventNwkJoinConfirm.EventStatus)
            {
                case THR_EventNwkJoinConfirm_EventStatus_Attaching:
                    shell_write(Attaching);
                    break;

                case THR_EventNwkJoinConfirm_EventStatus_JoinSuccess:
                    shell_write(JoinSuccess);
                    break;

                case THR_EventNwkJoinConfirm_EventStatus_JoinFailed:
                    shell_write(JoinFailed);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.THR_EventNwkJoinConfirm.EventStatus);
                    break;
            }

            break;

        case 0xCF53:
            shell_write("THR_EventNwkJoinSelectParentsConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_EventNwkJoinSelectParentsConfirm.EventStatus)
            {
                case THR_EventNwkJoinSelectParentsConfirm_EventStatus_ScanStarted:
                    shell_write(ScanStarted);
                    break;

                case THR_EventNwkJoinSelectParentsConfirm_EventStatus_ReceivedBeacon:
                    shell_write(ReceivedBeacon);
                    break;

                case THR_EventNwkJoinSelectParentsConfirm_EventStatus_ScanEnded:
                    shell_write(ScanEnded);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.THR_EventNwkJoinSelectParentsConfirm.EventStatus);
                    break;
            }

            break;

        case 0xCF54:
            shell_write("THR_EventGeneralConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_EventGeneralConfirm.EventStatus)
            {
                case THR_EventGeneralConfirm_EventStatus_Disconnected:
                    shell_write(Disconnected);
                    break;

                case THR_EventGeneralConfirm_EventStatus_Connected:
                    shell_write(Connected);
                    break;

                case THR_EventGeneralConfirm_EventStatus_Resettofactorydefault:
                    shell_write(Resettofactorydefault);

                    /* on autostart, after factory reset set the channel */
                    if (gAutostartInProgress && (gAutostartState == gAutostartFirstState_c))
                    {
                        gAutostartState = gFactoryResetState_c;

                        /* send next command after a small delay - leave time for DHCPv6-PD */
                        OSA_TimeDelay(3000);
                        SHELL_ThrSetChannel(NULL);
                    }

#if gHybridApp_d
                    SHELL_BLE_Terminate();
#endif

                    break;

                case THR_EventGeneralConfirm_EventStatus_Instancerestorestarted:
                    shell_write(Instancerestorestarted);
                    break;

                case THR_EventGeneralConfirm_EventStatus_RouterSynced:
                    shell_write(RouterSynced);
                    break;

                case THR_EventGeneralConfirm_EventStatus_EndDeviceSynced:
                    shell_write(EndDeviceSynced);
                    break;

                case THR_EventGeneralConfirm_EventStatus_Connectingstarted:
                    shell_write(Connectingstarted);
                    break;

                case THR_EventGeneralConfirm_EventStatus_Connectingfailed:
                    shell_write(Connectingfailed);
                    break;

                case THR_EventGeneralConfirm_EventStatus_Connectingdeffered:
                    shell_write(Connectingdeffered);
                    break;

                case THR_EventGeneralConfirm_EventStatus_DeviceisLeader:
                    shell_write(DeviceisLeader);
                    break;

                case THR_EventGeneralConfirm_EventStatus_DeviceisRouter:
                    shell_write(DeviceisRouter);
                    break;

                case THR_EventGeneralConfirm_EventStatus_DeviceisREED:
                    shell_write(DeviceisREED);
                    break;

                case THR_EventGeneralConfirm_EventStatus_DeviceisEndDevice:
                    shell_write(DeviceisEndDevice);
                    break;

                case THR_EventGeneralConfirm_EventStatus_DeviceisSleepyEndDevice:
                    shell_write(DeviceisSleepyEndDevice);
                    break;

                case THR_EventGeneralConfirm_EventStatus_RequestingGlobalAddress:
                    shell_write(RequestingGlobalAddress);
                    break;

                case THR_EventGeneralConfirm_EventStatus_GlobalAddressassigned:
                    shell_write(GlobalAddressassigned);
                    break;

                case THR_EventGeneralConfirm_EventStatus_RequestingRouterId:
                    shell_write(RequestingRouterId);
                    break;

                case THR_EventGeneralConfirm_EventStatus_RouterIdassigned:
                    shell_write(RouterIdassigned);
                    break;

                case THR_EventGeneralConfirm_EventStatus_RouterIdassignedfailed:
                    shell_write(RouterIdassignedfailed);
                    break;

                case THR_EventGeneralConfirm_EventStatus_Allowdevicetosleep:
                    shell_write(Allowdevicetosleep);
                    break;

                case THR_EventGeneralConfirm_EventStatus_Disallowdevicetosleep:
                    shell_write(Disallowdevicetosleep);
                    break;

                case THR_EventGeneralConfirm_EventStatus_ChildIdassigned:
                    shell_write(ChildIdassigned);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.THR_EventGeneralConfirm.EventStatus);
                    break;
            }

            break;

        case 0xCF55:
            shell_write("THR_EventNwkCommissioningIndication");
            shell_write(" -> ");

            switch (container->Data.THR_EventNwkCommissioningIndication.EventStatus)
            {
                case THR_EventNwkCommissioningIndication_EventStatus_JoinerDiscoveryStarted:
                    shell_write(JoinerDiscoveryStarted);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_JoinerDiscoveryFailedNoBeacon:
                    shell_write(JoinerDiscoveryFailedNoBeacon);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_JoinerDiscoveryFailedFiltered:
                    shell_write(JoinerDiscoveryFailedFiltered);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_JoinerDiscoverySuccess:
                    shell_write(JoinerDiscoverySuccess);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_JoinerDtlsSessionStarted:
                    shell_write(JoinerDtlsSessionStarted);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_JoinerDtlsError:
                    shell_write(JoinerDtlsError);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_JoinerError:
                    shell_write(JoinerError);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_JoinerAccepted:
                    shell_write(JoinerAccepted);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerPetitionStarted:
                    shell_write(CommissionerPetitionStarted);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerPetitionAccepted:
                    shell_write(CommissionerPetitionAccepted);

                    if (gAutostartInProgress && (gAutostartState == gFactoryResetState_c))
                    {
                        gAutostartState = gNetworkCreated_c;
                        shell_write("\r\nStarting Commissioner..");

                        /* send next command after a small delay */
                        OSA_TimeDelay(500);
                        SHELL_ThrStartCommissioner(NULL);
                    }
                    else if (gAutostartInProgress && (gAutostartState == gNetworkCreated_c))
                    {
                        gAutostartState = gCommissionerStarted_c;
                        shell_write("\r\nAllowing all joiners with PSKd `THREAD`..");

                        MESHCOP_AddExpectedJoinerRequest_t req;
                        req.InstanceId = gThreadInstanceId;
                        req.Selected = TRUE;
                        req.EuiType = MESHCOP_AddExpectedJoinerRequest_EuiType_LongEUI;
                        req.EUI.LongEUI = THR_ALL_FFs64;
                        req.PSKdSize = strlen("THREAD");
                        req.PSKd = "THREAD";

                        MESHCOP_AddExpectedJoinerRequest(&req, THR_FSCI_IF);
                    }

                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerPetitionRejected:
                    shell_write(CommissionerPetitionRejected);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerPetitionError:
                    shell_write(CommissionerPetitionError);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerKeepAliveSent:
                    shell_write(CommissionerKeepAliveSent);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerError:
                    shell_write(CommissionerError);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerJoinerDtlsSessionStarted:
                    shell_write(CommissionerJoinerDtlsSessionStarted);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerJoinerDtlsError:
                    shell_write(CommissionerJoinerDtlsError);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerJoinerAccepted:
                    shell_write(CommissionerJoinerAccepted);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerNwkDataSynced:
                    shell_write(CommissionerNwkDataSynced);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerBrDtlsSessionStarted:
                    shell_write(CommissionerBrDtlsSessionStarted);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerBrDtlsError:
                    shell_write(CommissionerBrDtlsError);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerBrError:
                    shell_write(CommissionerBrError);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_CommissionerBrAccepted:
                    shell_write(CommissionerBrAccepted);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_BrCommissionerDtlsSessionStarted:
                    shell_write(BrCommissionerDtlsSessionStarted);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_BrCommissionerDtlsError:
                    shell_write(BrCommissionerDtlsError);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_BrCommissionerAccepted:
                    shell_write(BrCommissionerAccepted);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_BrCommissionerDataRelayedInbound:
                    shell_write(BrCommissionerDataRelayedInbound);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_BrCommissionerDataRelayedOutbound:
                    shell_write(BrCommissionerDataRelayedOutbound);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_JoinerrouterJoinerDataRelayedInbound:
                    shell_write(JoinerrouterJoinerDataRelayedInbound);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_JoinerrouterJoinerDataRelayedOutbound:
                    shell_write(JoinerrouterJoinerDataRelayedOutbound);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_JoinerrouterJoinerAccepted:
                    shell_write(JoinerrouterJoinerAccepted);
                    break;

                case THR_EventNwkCommissioningIndication_EventStatus_StartVendorProvisioning:
                    shell_write(StartVendorProvisioning);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%04X", container->Data.THR_EventNwkCommissioningIndication.EventStatus);
                    break;
            }

            break;

        case 0xCF4E:
            shell_write("THR_CommissioningDiagnosticIndication");
            shell_write(" -> ");

            switch (container->Data.THR_CommissioningDiagnosticIndication.Direction)
            {
                case THR_CommissioningDiagnosticIndication_Direction_OUT:
                    shell_write(OUT);
                    break;

                case THR_CommissioningDiagnosticIndication_Direction_IN:
                    shell_write(IN);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_CommissioningDiagnosticIndication.Direction);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.THR_CommissioningDiagnosticIndication.Type)
            {
                case THR_CommissioningDiagnosticIndication_Type_JOIN_FIN_REQ:
                    shell_write(JOIN_FIN_REQ);
                    break;

                case THR_CommissioningDiagnosticIndication_Type_JOIN_FIN_RSP:
                    shell_write(JOIN_FIN_RSP);
                    break;

                case THR_CommissioningDiagnosticIndication_Type_JOIN_ENT_REQ:
                    shell_write(JOIN_ENT_REQ);
                    break;

                case THR_CommissioningDiagnosticIndication_Type_JOIN_ENT_RSP:
                    shell_write(JOIN_ENT_RSP);
                    break;

                case THR_CommissioningDiagnosticIndication_Type_DTLS_CLOSE_NOTIFY:
                    shell_write(DTLS_CLOSE_NOTIFY);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_CommissioningDiagnosticIndication.Type);
                    break;
            }

            MEM_BufferFree(container->Data.THR_CommissioningDiagnosticIndication.TlvsBytes);

            break;

        case 0xCF61:
            shell_write("THR_MgmtDiagnosticGetConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_MgmtDiagnosticGetConfirm.Status)
            {
                case THR_MgmtDiagnosticGetConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_MgmtDiagnosticGetConfirm_Status_InvalidParameter:
                    shell_write(InvalidParameter);
                    break;

                case THR_MgmtDiagnosticGetConfirm_Status_NotPermitted:
                    shell_write(NotPermitted);
                    break;

                case THR_MgmtDiagnosticGetConfirm_Status_NoMemory:
                    shell_write(NoMemory);
                    break;

                case THR_MgmtDiagnosticGetConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_MgmtDiagnosticGetConfirm.Status);
                    break;
            }

            break;

        case 0xCF67:
            shell_write("THR_DiagTestGetConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_DiagTestGetConfirm.Status)
            {
                case THR_DiagTestGetConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_DiagTestGetConfirm_Status_InvalidParameter:
                    shell_write(InvalidParameter);
                    break;

                case THR_DiagTestGetConfirm_Status_NotPermitted:
                    shell_write(NotPermitted);
                    break;

                case THR_DiagTestGetConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_DiagTestGetConfirm.Status);
                    break;
            }

            break;

        case 0xCF62:
            shell_write("THR_MgmtDiagnosticResetConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_MgmtDiagnosticResetConfirm.Status)
            {
                case THR_MgmtDiagnosticResetConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_MgmtDiagnosticResetConfirm_Status_InvalidParameter:
                    shell_write(InvalidParameter);
                    break;

                case THR_MgmtDiagnosticResetConfirm_Status_NotPermitted:
                    shell_write(NotPermitted);
                    break;

                case THR_MgmtDiagnosticResetConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_MgmtDiagnosticResetConfirm.Status);
                    break;
            }

            break;

        case 0xCF65:
            shell_write("THR_MgmtReadMemoryConfirm");
            break;

        case 0xCF66:
            shell_write("THR_MgmtWriteMemoryConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_MgmtWriteMemoryConfirm.Status)
            {
                case THR_MgmtWriteMemoryConfirm_Status_Success:
                    shell_write(Success);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_MgmtWriteMemoryConfirm.Status);
                    break;
            }

            break;

        case 0xCF75:
            shell_write("THR_SetManualSlaacIIDConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_SetManualSlaacIIDConfirm.Status)
            {
                case THR_SetManualSlaacIIDConfirm_Status_OK:
                    shell_write(OK);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_SetManualSlaacIIDConfirm.Status);
                    break;
            }

            break;

        case 0xCF76:
            shell_write("THR_SendProactiveAddrNotifConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_SendProactiveAddrNotifConfirm.Status)
            {
                case THR_SendProactiveAddrNotifConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_SendProactiveAddrNotifConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case THR_SendProactiveAddrNotifConfirm_Status_InvalidInstance:
                    shell_write(InvalidInstance);
                    break;

                case THR_SendProactiveAddrNotifConfirm_Status_NotStarted:
                    shell_write(NotStarted);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_SendProactiveAddrNotifConfirm.Status);
                    break;
            }

            break;

        case 0xCFC0:
            shell_write("THR_NwkDiscoveryConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_NwkDiscoveryConfirm.Status)
            {
                case THR_NwkDiscoveryConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case THR_NwkDiscoveryConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_NwkDiscoveryConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_NwkDiscoveryConfirm.Status);
                    break;
            }

            break;

        case 0xCFC1:
            shell_write("THR_NwkDiscoveryStopConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_NwkDiscoveryStopConfirm.Status)
            {
                case THR_NwkDiscoveryStopConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case THR_NwkDiscoveryStopConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_NwkDiscoveryStopConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_NwkDiscoveryStopConfirm.Status);
                    break;
            }

            break;

        case 0xCFC2:
            shell_write("THR_SearchNwkWithAnounceConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_SearchNwkWithAnounceConfirm.Status)
            {
                case THR_SearchNwkWithAnounceConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case THR_SearchNwkWithAnounceConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_SearchNwkWithAnounceConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_SearchNwkWithAnounceConfirm.Status);
                    break;
            }

            break;

        case 0xCF63:
            shell_write("THR_MgmtDiagnosticGetRspIndication");
            shell_write(" -> ");

            switch (container->Data.THR_MgmtDiagnosticGetRspIndication.Status)
            {
                case THR_MgmtDiagnosticGetRspIndication_Status_Success:
                    shell_write(Success);
                    break;

                case THR_MgmtDiagnosticGetRspIndication_Status_FailedNotsupported:
                    shell_write(FailedNotsupported);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_MgmtDiagnosticGetRspIndication.Status);
                    break;
            }

            break;

        case 0xCF68:
            shell_write("THR_DiagTestGetRspIndication");
            shell_write(" -> ");

            switch (container->Data.THR_DiagTestGetRspIndication.Status)
            {
                case THR_DiagTestGetRspIndication_Status_Success:
                    shell_write(Success);
                    break;

                case THR_DiagTestGetRspIndication_Status_FailedNotsupported:
                    shell_write(FailedNotsupported);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_DiagTestGetRspIndication.Status);
                    break;
            }

            break;

        case 0xCF64:
            shell_write("THR_MgmtDiagnosticResetRspIndication");
            shell_write(" -> ");

            switch (container->Data.THR_MgmtDiagnosticResetRspIndication.Status)
            {
                case THR_MgmtDiagnosticResetRspIndication_Status_Success:
                    shell_write(Success);
                    break;

                case THR_MgmtDiagnosticResetRspIndication_Status_FailedNotallowed:
                    shell_write(FailedNotallowed);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_MgmtDiagnosticResetRspIndication.Status);
                    break;
            }

            break;

#endif  /* THREADMGMT_ENABLE */

#if THREADUTILS_ENABLE

        case 0xCF2E:
            shell_write("THR_SetNwkIdTimeoutConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_SetNwkIdTimeoutConfirm.Status)
            {
                case THR_SetNwkIdTimeoutConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_SetNwkIdTimeoutConfirm_Status_Notpermitted:
                    shell_write(Notpermitted);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_SetNwkIdTimeoutConfirm.Status);
                    break;
            }

            break;

        case 0xCF20:
            shell_write("THR_SetThresholdConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_SetThresholdConfirm.Status)
            {
                case THR_SetThresholdConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_SetThresholdConfirm_Status_InvalidParameter:
                    shell_write(InvalidParameter);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_SetThresholdConfirm.Status);
                    break;
            }

            break;

        case 0xCF10:
            shell_write("THR_GetNeighborInfoConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_GetNeighborInfoConfirm.Status)
            {
                case THR_GetNeighborInfoConfirm_Status_Success:
                    SHELL_PrintNeighborInfo(&(container->Data.THR_GetNeighborInfoConfirm));
                    break;

                case THR_GetNeighborInfoConfirm_Status_NeighborNotFound:
                    shell_write(NeighborNotFound);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_GetNeighborInfoConfirm.Status);
                    break;
            }

            break;

        case 0xCF23:
            shell_write("THR_GetChildrenTableConfirm");
            break;

        case 0xCF24:
            SHELL_PrintNeighborTbl(&(container->Data.THR_GetNeighborTableConfirm));
            break;

        case 0xCF25:
            SHELL_PrintRoutingTbl(&(container->Data.THR_GetRoutingTableConfirm));
            break;

        case 0xCF17:
            shell_write("THR_GetAttrConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_GetAttrConfirm.Status)
            {
                case THR_GetAttrConfirm_Status_Success:
                    SHELL_PrintGetAttrRsp(&container->Data.THR_GetAttrConfirm);
                    break;

                case THR_GetAttrConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_GetAttrConfirm_Status_Invalidparameter:
                    shell_write(Invalidparameter);
                    break;

                case THR_GetAttrConfirm_Status_Notpermitted:
                    shell_write(Notpermitted);
                    break;

                case THR_GetAttrConfirm_Status_UnsupportedAttribute:
                    shell_write(UnsupportedAttribute);
                    break;

                case THR_GetAttrConfirm_Status_EmptyEntry:
                    shell_write(EmptyEntry);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_GetAttrConfirm.Status);
                    break;
            }

            break;

        case 0xCF18:
            shell_write("THR_SetAttrConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_SetAttrConfirm.Status)
            {
                case THR_SetAttrConfirm_Status_Success:
                    shell_write(Success);

                    if (gAutostartInProgress)
                    {
                        shell_write("\r\nCreating the network..");
                        SHELL_ThrNwkCreate(NULL);
                    }

                    break;

                case THR_SetAttrConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_SetAttrConfirm_Status_Invalidparameter:
                    shell_write(Invalidparameter);
                    break;

                case THR_SetAttrConfirm_Status_Notpermitted:
                    shell_write(Notpermitted);
                    break;

                case THR_SetAttrConfirm_Status_UnsupportedAttribute:
                    shell_write(UnsupportedAttribute);
                    break;

                case THR_SetAttrConfirm_Status_EmptyEntry:
                    shell_write(EmptyEntry);
                    break;

                case THR_SetAttrConfirm_Status_InvalidValue:
                    shell_write(InvalidValue);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_SetAttrConfirm.Status);
                    break;
            }

            break;

        case 0xCF19:
            shell_write("THR_GetThreadIpAddrConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_GetThreadIpAddrConfirm.Status)
            {
                case THR_GetThreadIpAddrConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_GetThreadIpAddrConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case THR_GetThreadIpAddrConfirm_Status_InvalidInstance:
                    shell_write(InvalidInstance);
                    break;

                case THR_GetThreadIpAddrConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_GetThreadIpAddrConfirm.Status);
                    break;
            }

            break;

        case 0xCF1E:
            shell_write("THR_GetParentConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_GetParentConfirm.Status)
            {
                case THR_GetParentConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_GetParentConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case THR_GetParentConfirm_Status_InvalidInstance:
                    shell_write(InvalidInstance);
                    break;

                case THR_GetParentConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_GetParentConfirm.Status);
                    break;
            }

            break;

        case 0xCF2F:
            shell_write("THR_ChildUpdateToParentConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_ChildUpdateToParentConfirm.Status)
            {
                case THR_ChildUpdateToParentConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case THR_ChildUpdateToParentConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_ChildUpdateToParentConfirm_Status_NotPermitted:
                    shell_write(NotPermitted);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_ChildUpdateToParentConfirm.Status);
                    break;
            }

            break;

        case 0xCF26:
            shell_write("THR_LeaderRemoveRouterIdConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_LeaderRemoveRouterIdConfirm.Status)
            {
                case THR_LeaderRemoveRouterIdConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case THR_LeaderRemoveRouterIdConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_LeaderRemoveRouterIdConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_LeaderRemoveRouterIdConfirm.Status);
                    break;
            }

            break;

        case 0xCF28:
            shell_write("THR_GenerateAllKeysConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_GenerateAllKeysConfirm.Status)
            {
                case THR_GenerateAllKeysConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case THR_GenerateAllKeysConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_GenerateAllKeysConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_GenerateAllKeysConfirm.Status);
                    break;
            }

            break;

        case 0xCF27:
            shell_write("THR_SwitchKeyKeyConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_SwitchKeyKeyConfirm.Status)
            {
                case THR_SwitchKeyKeyConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case THR_SwitchKeyKeyConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_SwitchKeyKeyConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_SwitchKeyKeyConfirm.Status);
                    break;
            }

            break;

        case 0xCF29:
            shell_write("THR_BrPrefixAddEntryConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_BrPrefixAddEntryConfirm.Status)
            {
                case THR_BrPrefixAddEntryConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_BrPrefixAddEntryConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_BrPrefixAddEntryConfirm_Status_Notpermitted:
                    shell_write(Notpermitted);
                    break;

                case THR_BrPrefixAddEntryConfirm_Status_Nomemory:
                    shell_write(Nomemory);
                    break;

                case THR_BrPrefixAddEntryConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_BrPrefixAddEntryConfirm.Status);
                    break;
            }

            break;

        case 0xCF2A:
            shell_write("THR_BrPrefixGetTableConfirm");
            break;

        case 0xCF2B:
            shell_write("THR_BrPrefixRemoveEntryConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_BrPrefixRemoveEntryConfirm.Status)
            {
                case THR_BrPrefixRemoveEntryConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_BrPrefixRemoveEntryConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_BrPrefixRemoveEntryConfirm_Status_InvalidParameter:
                    shell_write(InvalidParameter);
                    break;

                case THR_BrPrefixRemoveEntryConfirm_Status_Nomemory:
                    shell_write(Nomemory);
                    break;

                case THR_BrPrefixRemoveEntryConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_BrPrefixRemoveEntryConfirm.Status);
                    break;
            }

            break;

        case 0xCF86:
            shell_write("THR_BrServiceRemoveEntryConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_BrServiceRemoveEntryConfirm.Status)
            {
                case THR_BrServiceRemoveEntryConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_BrServiceRemoveEntryConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_BrServiceRemoveEntryConfirm_Status_InvalidParameter:
                    shell_write(InvalidParameter);
                    break;

                case THR_BrServiceRemoveEntryConfirm_Status_Nomemory:
                    shell_write(Nomemory);
                    break;

                case THR_BrServiceRemoveEntryConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_BrServiceRemoveEntryConfirm.Status);
                    break;
            }

            break;

        case 0xCF85:
            shell_write("THR_BrServiceAddEntryConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_BrServiceAddEntryConfirm.Status)
            {
                case THR_BrServiceAddEntryConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_BrServiceAddEntryConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_BrServiceAddEntryConfirm_Status_InvalidParameter:
                    shell_write(InvalidParameter);
                    break;

                case THR_BrServiceAddEntryConfirm_Status_NotPermitted:
                    shell_write(NotPermitted);
                    break;

                case THR_BrServiceAddEntryConfirm_Status_Nomemory:
                    shell_write(Nomemory);
                    break;

                case THR_BrServiceAddEntryConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_BrServiceAddEntryConfirm.Status);
                    break;
            }

            break;

        case 0xCF2C:
            shell_write("THR_BrPrefixSyncConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_BrPrefixSyncConfirm.Status)
            {
                case THR_BrPrefixSyncConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_BrPrefixSyncConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_BrPrefixSyncConfirm_Status_InvalidParameter:
                    shell_write(InvalidParameter);
                    break;

                case THR_BrPrefixSyncConfirm_Status_Notpermitted:
                    shell_write(Notpermitted);
                    break;

                case THR_BrPrefixSyncConfirm_Status_Nomemory:
                    shell_write(Nomemory);
                    break;

                case THR_BrPrefixSyncConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_BrPrefixSyncConfirm.Status);
                    break;
            }

            break;

        case 0xCF2D:
            shell_write("THR_BrPrefixRemoveAllConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_BrPrefixRemoveAllConfirm.Status)
            {
                case THR_BrPrefixRemoveAllConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case THR_BrPrefixRemoveAllConfirm_Status_Invalidinstance:
                    shell_write(Invalidinstance);
                    break;

                case THR_BrPrefixRemoveAllConfirm_Status_InvalidParameter:
                    shell_write(InvalidParameter);
                    break;

                case THR_BrPrefixRemoveAllConfirm_Status_Notpermitted:
                    shell_write(Notpermitted);
                    break;

                case THR_BrPrefixRemoveAllConfirm_Status_Nomemory:
                    shell_write(Nomemory);
                    break;

                case THR_BrPrefixRemoveAllConfirm_Status_Theselectedconfigurationisnotvalid:
                    shell_write(Theselectedconfigurationisnotvalid);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_BrPrefixRemoveAllConfirm.Status);
                    break;
            }

            break;

        case 0xCF69:
            shell_write("THR_IdentifyConfirm");
            shell_write(" -> ");

            switch (container->Data.THR_IdentifyConfirm.Status)
            {
                case THR_IdentifyConfirm_Status_Success:
                    shell_write(Success);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.THR_IdentifyConfirm.Status);
                    break;
            }

            break;

#endif  /* THREADUTILS_ENABLE */

#if IPUTILS_ENABLE

        case 0xCF0C:
            shell_write("NWKU_IfconfigBindConfirm");
            shell_write(" -> ");

            switch (container->Data.NWKU_IfconfigBindConfirm.Status)
            {
                case NWKU_IfconfigBindConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case NWKU_IfconfigBindConfirm_Status_Addressesmaximumlimitreached:
                    shell_write(Addressesmaximumlimitreached);
                    break;

                case NWKU_IfconfigBindConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.NWKU_IfconfigBindConfirm.Status);
                    break;
            }

            break;

        case 0xCF0D:
            SHELL_PrintIfconfigAllRsp(&container->Data.NWKU_IfconfigAllResponse);
            break;

        case 0xCF0E:
            switch (container->Data.NWKU_PingConfirm.Status)
            {
                case NWKU_PingConfirm_Status_OK:
                    PING_EchoReplyReceive(NULL);
                    break;

                case NWKU_PingConfirm_Status_RequestTimeout:
                    PING_HandleTimeout(NULL);
                    break;

                case NWKU_PingConfirm_Status_Wrongdestinationaddress:
                    shell_write(Wrongdestinationaddress);
                    break;

                case NWKU_PingConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.NWKU_PingConfirm.Status);
                    break;
            }

            break;

        case 0xCF70:
            shell_write("NWKU_EchoUDPConfirm");
            shell_write(" -> ");

            switch (container->Data.NWKU_EchoUDPConfirm.Status)
            {
                case NWKU_EchoUDPConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case NWKU_EchoUDPConfirm_Status_RequestTimeout:
                    shell_write(RequestTimeout);
                    break;

                case NWKU_EchoUDPConfirm_Status_Wrongdestinationaddress:
                    shell_write(Wrongdestinationaddress);
                    break;

                case NWKU_EchoUDPConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.NWKU_EchoUDPConfirm.Status);
                    break;
            }

            break;

        case 0xCF92:
            switch (container->Data.NWKU_CoapMsgReceivedIndication.Status)
            {
                case NWKU_CoapMsgReceivedIndication_Status_Success:
                    SHELL_PrintCoapMsg(&(container->Data.NWKU_CoapMsgReceivedIndication));
                    break;

                case NWKU_CoapMsgReceivedIndication_Status_Failed:
                    shell_write(Failed);
                    break;

                case NWKU_CoapMsgReceivedIndication_Status_Duplicate:
                    shell_write(Duplicate);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.NWKU_CoapMsgReceivedIndication.Status);
                    break;
            }

            MEM_BufferFree(container->Data.NWKU_CoapMsgReceivedIndication.Payload);

            break;

        case 0xCF96:
            shell_write("NWKU_DnsResponseReceivedIndication");
            break;

        case 0xCF91:
            shell_write("NWKU_CoapRegisterConfirm");
            shell_write(" -> ");

            switch (container->Data.NWKU_CoapRegisterConfirm.Status)
            {
                case NWKU_CoapRegisterConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case NWKU_CoapRegisterConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.NWKU_CoapRegisterConfirm.Status);
                    break;
            }

            break;

        case 0xCF93:
            shell_write("NWKU_CoapCreateInstanceConfirm");
            shell_write(" -> ");

            switch (container->Data.NWKU_CoapCreateInstanceConfirm.Status)
            {
                case NWKU_CoapCreateInstanceConfirm_Status_Success:
                    shell_write(Success);
                    gCoAPInstanceId = 0;
                    break;

                case NWKU_CoapCreateInstanceConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.NWKU_CoapCreateInstanceConfirm.Status);
                    break;
            }

            break;

        case 0xCF90:
            shell_write("NWKU_CoapSendConfirm");
            shell_write(" -> ");

            switch (container->Data.NWKU_CoapSendConfirm.Status)
            {
                case NWKU_CoapSendConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case NWKU_CoapSendConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.NWKU_CoapSendConfirm.Status);
                    break;
            }

            break;

        case 0xCF95:
            shell_write("NWKU_DnsSendRequestConfirm");
            shell_write(" -> ");

            switch (container->Data.NWKU_DnsSendRequestConfirm.Status)
            {
                case NWKU_DnsSendRequestConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case NWKU_DnsSendRequestConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.NWKU_DnsSendRequestConfirm.Status);
                    break;
            }

            break;

        case 0xCF71:
            shell_write("NWKU_McastGroupManageConfirm");
            shell_write(" -> ");

            switch (container->Data.NWKU_McastGroupManageConfirm.Status)
            {
                case NWKU_McastGroupManageConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case NWKU_McastGroupManageConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.NWKU_McastGroupManageConfirm.Status);
                    break;
            }

            break;

        case 0xCF72:
            SHELL_PrintMcastGroups(&(container->Data.NWKU_McastGroupShowResponse));
            break;

#endif  /* IPUTILS_ENABLE */

#if MESHCOP_ENABLE

        case 0xCF40:
            shell_write("MESHCOP_StartCommissionerConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_StartCommissionerConfirm.Status)
            {
                case MESHCOP_StartCommissionerConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_StartCommissionerConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_StartCommissionerConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_StartCommissionerConfirm.Status);
                    break;
            }

            break;

        case 0xCF4F:
            shell_write("MESHCOP_StartNativeCommissionerConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_StartNativeCommissionerConfirm.Status)
            {
                case MESHCOP_StartNativeCommissionerConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_StartNativeCommissionerConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_StartNativeCommissionerConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_StartNativeCommissionerConfirm.Status);
                    break;
            }

            break;

        case 0xCF41:
            shell_write("MESHCOP_StopCommissionerConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_StopCommissionerConfirm.Status)
            {
                case MESHCOP_StopCommissionerConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_StopCommissionerConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_StopCommissionerConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_StopCommissionerConfirm.Status);
                    break;
            }

            break;

        case 0xCF42:
            shell_write("MESHCOP_AddExpectedJoinerConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_AddExpectedJoinerConfirm.Status)
            {
                case MESHCOP_AddExpectedJoinerConfirm_Status_Success:
                    shell_write(Success);

                    if (gAutostartInProgress)
                    {
                        shell_write("\r\nSynchronizing the steering data..");
                        MESHCOP_SyncSteeringDataRequest_t req;
                        req.InstanceId = gThreadInstanceId;
                        req.EuiMask = MESHCOP_SyncSteeringDataRequest_EuiMask_AllFFs;

                        MESHCOP_SyncSteeringDataRequest(&req, THR_FSCI_IF);
                    }

                    break;

                case MESHCOP_AddExpectedJoinerConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_AddExpectedJoinerConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_AddExpectedJoinerConfirm.Status);
                    break;
            }

            break;

        case 0xCF43:
            shell_write("MESHCOP_GetExpectedJoinerConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_GetExpectedJoinerConfirm.Status)
            {
                case MESHCOP_GetExpectedJoinerConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_GetExpectedJoinerConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_GetExpectedJoinerConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_GetExpectedJoinerConfirm.Status);
                    break;
            }

            shell_write(" -> ");

            switch (container->Data.MESHCOP_GetExpectedJoinerConfirm.Selected)
            {
                case MESHCOP_GetExpectedJoinerConfirm_Selected_FALSE:
                    shell_write(FALSE_str);
                    break;

                case MESHCOP_GetExpectedJoinerConfirm_Selected_TRUE:
                    shell_write(TRUE_str);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_GetExpectedJoinerConfirm.Selected);
                    break;
            }

            break;

        case 0xCF44:
            shell_write("MESHCOP_RemoveExpectedJoinerConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_RemoveExpectedJoinerConfirm.Status)
            {
                case MESHCOP_RemoveExpectedJoinerConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_RemoveExpectedJoinerConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_RemoveExpectedJoinerConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_RemoveExpectedJoinerConfirm.Status);
                    break;
            }

            break;

        case 0xCF45:
            shell_write("MESHCOP_RemoveAllExpectedJoinersConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_RemoveAllExpectedJoinersConfirm.Status)
            {
                case MESHCOP_RemoveAllExpectedJoinersConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_RemoveAllExpectedJoinersConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_RemoveAllExpectedJoinersConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_RemoveAllExpectedJoinersConfirm.Status);
                    break;
            }

            break;

        case 0xCF46:
            shell_write("MESHCOP_SyncSteeringDataConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_SyncSteeringDataConfirm.Status)
            {
                case MESHCOP_SyncSteeringDataConfirm_Status_Success:
                    shell_write(Success);

                    /* this is the last command in the autostart demo */
                    if (gAutostartInProgress)
                    {
                        gAutostartInProgress = FALSE;
                        gAutostartState = gAutostartFirstState_c;
                        shell_write("\r\nAutostart completed successfully!");
                        shell_printf("\r\n\tJoiners may be added on channel %d, any EUI, PSKd `THREAD`", gAutostartChannel);
                        SHELL_Resume(NULL, 0, NULL);
                    }

                    break;

                case MESHCOP_SyncSteeringDataConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_SyncSteeringDataConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_SyncSteeringDataConfirm.Status);
                    break;
            }

            break;

        case 0xCF4B:
            shell_write("MESHCOP_MgmtSetConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmtSetConfirm.Status)
            {
                case MESHCOP_MgmtSetConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmtSetConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_MgmtSetConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmtSetConfirm.Status);
                    break;
            }

            break;

        case 0xCF4C:
            shell_write("MESHCOP_MgmtGetConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmtGetConfirm.Status)
            {
                case MESHCOP_MgmtGetConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmtGetConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_MgmtGetConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmtGetConfirm.Status);
                    break;
            }

            break;

        case 0xCF4D:
            shell_write("MESHCOP_SetCommissionerCredentialConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_SetCommissionerCredentialConfirm.Status)
            {
                case MESHCOP_SetCommissionerCredentialConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_SetCommissionerCredentialConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_SetCommissionerCredentialConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_SetCommissionerCredentialConfirm.Status);
                    break;
            }

            break;

        case 0xCF47:
            shell_write("MESHCOP_MgmNwkFormConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmNwkFormConfirm.Status)
            {
                case MESHCOP_MgmNwkFormConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmNwkFormConfirm_Status_Notpermitted:
                    shell_write(Notpermitted);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmNwkFormConfirm.Status);
                    break;
            }

            break;

        case 0xCFA0:
            shell_write("MESHCOP_MgmtCommissionerGetConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmtCommissionerGetConfirm.Status)
            {
                case MESHCOP_MgmtCommissionerGetConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmtCommissionerGetConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_MgmtCommissionerGetConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmtCommissionerGetConfirm.Status);
                    break;
            }

            break;

        case 0xCFA2:
            shell_write("MESHCOP_MgmtActiveGetConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmtActiveGetConfirm.Status)
            {
                case MESHCOP_MgmtActiveGetConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmtActiveGetConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_MgmtActiveGetConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmtActiveGetConfirm.Status);
                    break;
            }

            break;

        case 0xCFA4:
            shell_write("MESHCOP_MgmtPendingGetConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmtPendingGetConfirm.Status)
            {
                case MESHCOP_MgmtPendingGetConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmtPendingGetConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_MgmtPendingGetConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmtPendingGetConfirm.Status);
                    break;
            }

            break;

        case 0xCFA1:
            shell_write("MESHCOP_MgmtCommissionerSetConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmtCommissionerSetConfirm.Status)
            {
                case MESHCOP_MgmtCommissionerSetConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmtCommissionerSetConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_MgmtCommissionerSetConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmtCommissionerSetConfirm.Status);
                    break;
            }

            break;

        case 0xCFA3:
            shell_write("MESHCOP_MgmtActiveSetConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmtActiveSetConfirm.Status)
            {
                case MESHCOP_MgmtActiveSetConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmtActiveSetConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_MgmtActiveSetConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmtActiveSetConfirm.Status);
                    break;
            }

            break;

        case 0xCFA5:
            shell_write("MESHCOP_MgmtPendingSetConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmtPendingSetConfirm.Status)
            {
                case MESHCOP_MgmtPendingSetConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmtPendingSetConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_MgmtPendingSetConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmtPendingSetConfirm.Status);
                    break;
            }

            break;

        case 0xCFA8:
            shell_write("MESHCOP_MgmtSendPanIdQueryConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmtSendPanIdQueryConfirm.Status)
            {
                case MESHCOP_MgmtSendPanIdQueryConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmtSendPanIdQueryConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_MgmtSendPanIdQueryConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmtSendPanIdQueryConfirm.Status);
                    break;
            }

            break;

        case 0xCFA9:
            shell_write("MESHCOP_MgmtPanIdConflictConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmtPanIdConflictConfirm.Status)
            {
                case MESHCOP_MgmtPanIdConflictConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmtPanIdConflictConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_MgmtPanIdConflictConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmtPanIdConflictConfirm.Status);
                    break;
            }

            break;

        case 0xCFAA:
            shell_write("MESHCOP_MgmtSendEdScanConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmtSendEdScanConfirm.Status)
            {
                case MESHCOP_MgmtSendEdScanConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmtSendEdScanConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_MgmtSendEdScanConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmtSendEdScanConfirm.Status);
                    break;
            }

            break;

        case 0xCFAB:
            shell_write("MESHCOP_MgmtEdReportConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmtEdReportConfirm.Status)
            {
                case MESHCOP_MgmtEdReportConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmtEdReportConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_MgmtEdReportConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmtEdReportConfirm.Status);
                    break;
            }

            break;

        case 0xCFA7:
            shell_write("MESHCOP_MgmtSendAnnounceBeginConfirm");
            shell_write(" -> ");

            switch (container->Data.MESHCOP_MgmtSendAnnounceBeginConfirm.Status)
            {
                case MESHCOP_MgmtSendAnnounceBeginConfirm_Status_Success:
                    shell_write(Success);
                    break;

                case MESHCOP_MgmtSendAnnounceBeginConfirm_Status_Failed:
                    shell_write(Failed);
                    break;

                case MESHCOP_MgmtSendAnnounceBeginConfirm_Status_Error:
                    shell_write(Error);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.MESHCOP_MgmtSendAnnounceBeginConfirm.Status);
                    break;
            }

            break;

#endif  /* MESHCOP_ENABLE */

#if DTLS_ENABLE

        case 0xCF30:
            shell_write("DTLSOpenConfirm");
            break;

        case 0xCF31:
            shell_write("DTLSCloseConfirm");
            shell_write(" -> ");

            switch (container->Data.DTLSCloseConfirm.Status)
            {
                case DTLSCloseConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case DTLSCloseConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.DTLSCloseConfirm.Status);
                    break;
            }

            break;

        case 0xCF32:
            shell_write("DTLSClosePeerConfirm");
            shell_write(" -> ");

            switch (container->Data.DTLSClosePeerConfirm.Status)
            {
                case DTLSClosePeerConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case DTLSClosePeerConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.DTLSClosePeerConfirm.Status);
                    break;
            }

            break;

        case 0xCF33:
            shell_write("DTLSConnectConfirm");
            shell_write(" -> ");

            switch (container->Data.DTLSConnectConfirm.Status)
            {
                case DTLSConnectConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case DTLSConnectConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.DTLSConnectConfirm.Status);
                    break;
            }

            break;

        case 0xCF34:
            shell_write("DTLSClientConnectedConfirm");
            shell_write(" -> ");

            switch (container->Data.DTLSClientConnectedConfirm.Status)
            {
                case DTLSClientConnectedConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case DTLSClientConnectedConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.DTLSClientConnectedConfirm.Status);
                    break;
            }

            break;

        case 0xCF35:
            shell_write("DTLSSendConfirm");
            shell_write(" -> ");

            switch (container->Data.DTLSSendConfirm.Status)
            {
                case DTLSSendConfirm_Status_OK:
                    shell_write(OK);
                    break;

                case DTLSSendConfirm_Status_ERROR:
                    shell_write(ERROR);
                    break;

                default:
                    shell_printf("Unrecognized status 0x%02X", container->Data.DTLSSendConfirm.Status);
                    break;
            }

            break;

        case 0xCF36:
            shell_write("DTLSReceiveConfirm");
            break;

#endif  /* DTLS_ENABLE */

#if OTHERUTILS_ENABLE

        case 0xCFF3:
            shell_write("SerialTun_IPPacketReceivedConfirm");
            break;

        case 0xCF89:
            shell_write("DBGConfirm");
            break;

#endif  /* OTHERUTILS_ENABLE */

    }

    if (!gContinuousPing)
    {
        shell_refresh();
    }
}
