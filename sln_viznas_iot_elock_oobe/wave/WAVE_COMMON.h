//
// Created by xshx on 2022/5/20.
//

#ifndef W8_WAVE_COMMON_H
#define W8_WAVE_COMMON_H

#include "commondef.h"
#include "../db/UserExtendManager.h"


typedef  struct
{
    int MessageID;
    char Data[ 128 ];
} UartMessage, *pUartMessage;





extern bool lcd_back_ground;
extern void OpenLcdBackground();
extern void CloseLcdBackground();


extern  void SendMessageToUart5FromFakeUart(  char *data);

extern  QueueHandle_t  Uart5FromFakeUartMsgQueue;


#if (configSUPPORT_STATIC_ALLOCATION == 1)
DTC_BSS static StackType_t UartFakeTaskStack[FakeUartTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_UartFakeTaskTCB;
#endif

#endif //W8_WAVE_COMMON_H
