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

#include "MCU_UART5_Layer.h"

#ifdef __cplusplus
extern "C" {
#endif
#define UART5_DRV_RX_RING_BUF_SIZE 256
extern lpuart_rtos_config_t lpuart_config5;

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


extern int SendMsgToSelf(uint8_t *MsgBuf, uint8_t MsgLen);
extern int Uart5_SendDeinitCameraMsg(void);
extern int Uart5_SendQMsg(void *msg);

#ifdef __cplusplus
}
#endif
extern QueueHandle_t Uart5MsgQ;
extern int Uart5_GetFaceInfo( void  *faceInfo );
#endif //W8_MCU_UART5_2_H
