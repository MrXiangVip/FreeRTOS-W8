/**************************************************************************
 * 	FileName:	 	MCU_UART5.h
 *	Description:	This file is including the function interface which is used to processing
 					uart5 communication with mcu.
 *	Copyright(C):	2018-2020 Wave Group Inc.
 *	Version:		V 1.0
 *	Author:			tanqw
 *	Created:		2020-10-26
 *	Updated:		
 *					
**************************************************************************/
//

#ifndef W8_MCU_UART5_2_H
#define W8_MCU_UART5_2_H
#ifdef __cplusplus
extern "C" {
#endif

#include "MCU_UART5_Layer.h"
#define UART5_DRV_RX_RING_BUF_SIZE 256
extern lpuart_rtos_config_t lpuart_config5;
extern QueueHandle_t Uart5MsgQ;

int ProcMessage(
        uint8_t nCommand,
        uint8_t nMessageLen,
        const uint8_t *pszMessage);
int ProcMessageByHead(
        uint8_t head,
        uint8_t nCommand,
        uint8_t nMessageLen,
        const uint8_t *pszMessage);
int ProcMessageFromMQTT(
        uint8_t nCommand,
        uint8_t nMessageLen,
        const uint8_t *pszMessage);
int MCU_UART5_Start();

extern int  Uart5_GetFaceRegResult(uint8_t result, char *pszMessage);
//send qMsg to uart5 task when face recognize over
extern int  Uart5_GetFaceRecResult(uint8_t result, char *pszMessage);

extern int SendMsgToSelf(uint8_t *MsgBuf, uint8_t MsgLen);
extern int Uart5_SendDeinitCameraMsg(void);
extern int Uart5_SendQMsg(void *msg);

#ifdef __cplusplus
}
#endif
#endif //W8_MCU_UART5_2_H
