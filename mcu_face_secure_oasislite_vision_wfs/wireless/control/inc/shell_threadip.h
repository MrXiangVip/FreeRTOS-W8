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
#ifndef _SHELL_THREADIP_H
#define _SHELL_THREADIP_H
/*!=================================================================================================
\file       shell_threadip.h
\brief      This is a header file for the shell application for the Thread IP stack.
==================================================================================================*/

/*==================================================================================================
Include Files
==================================================================================================*/
#include "cmd_threadip.h"

/*==================================================================================================
Public macros
==================================================================================================*/

/*==================================================================================================
Public type definitions
==================================================================================================*/
typedef enum shellAutoStartStates_tag
{
    gAutostartFirstState_c,
    gFactoryResetState_c,
    gNetworkCreated_c,
    gCommissionerStarted_c,
    gAddedExpectedJoiners_c
} shellAutostartStates_t;

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
\fn     void SHELL_ThreadIP_Init(void)
\brief  This function is used to initialize the SHELL commands module.
***************************************************************************************************/
void SHELL_ThreadIP_Init(void);

/*!*************************************************************************************************
\fn     void SHELL_ThrEventNotify(void *container)
\brief  Print to host's shell information about Thread events coming from black-box

\param  [in]    container       the received event
***************************************************************************************************/
void SHELL_ThrEventNotify(void *container);


int32_t SHELL_Resume(shell_handle_t context, int32_t argc, char **argv);
void SHELL_ThrNwkCreate(void *param);
void SHELL_ThrSetChannel(void *param);
void SHELL_ThrStartCommissioner(void *param);
void SHELL_NwkScanPrint(THR_EventNwkScanConfirm_t *pScanResults);
void SHELL_PrintGetAttrRsp(THR_GetAttrConfirm_t *evt);
void SHELL_PrintNeighborInfo(void *param);
void SHELL_PrintNeighborTbl(void *param);
void SHELL_PrintRoutingTbl(void *param);
void SHELL_PrintIfconfigAllRsp(NWKU_IfconfigAllResponse_t *evt);
void SHELL_PrintCoapMsg(NWKU_CoapMsgReceivedIndication_t *evt);
void SHELL_PrintMcastGroups(NWKU_McastGroupShowResponse_t *evt);

void PING_EchoReplyReceive(void *pParam);
void PING_HandleTimeout(void *param);

#ifdef __cplusplus
}
#endif
/*================================================================================================*/
#endif  /* _SHELL_THREADIP_H */
