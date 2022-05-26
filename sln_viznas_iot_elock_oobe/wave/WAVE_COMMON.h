//
// Created by xshx on 2022/5/20.
//

#ifndef W8_WAVE_COMMON_H
#define W8_WAVE_COMMON_H

#include "commondef.h"
#include "../db/UserExtendManager.h"

#define UERID  "UERID"
#define TIMES   "TIMES"
#define TIMEE   "TIMEE"
#define ADEV    "ADEV"
#define EXUSR   "EXUSR"

typedef  struct
{
    int MessageID;
    char Data[ 128 ];
} UartMessage, *pUartMessage;


//注册时的结构体    uuid , 起始时间, 结束时间, 设备ID
typedef struct{
    char UUID[17];//
    int  uStartTime;//
    int  uEndTime;
    char cDeviceId[48];//
}RegisteClass, *PRegisteClass;



extern bool lcd_back_ground;
extern void OpenLcdBackground();
extern void CloseLcdBackground();
//xshx add
extern void vConvertRegistClass2UserExtend(RegisteClass *regist, UserExtend  *userExtend);

extern  void SendMessageToUart5FromFakeUart(  char *data);

extern  QueueHandle_t  Uart5FromFakeUartMsgQueue;


#if (configSUPPORT_STATIC_ALLOCATION == 1)
DTC_BSS static StackType_t UartFakeTaskStack[FakeUartTASK_STACKSIZE];
DTC_BSS static StaticTask_t s_UartFakeTaskTCB;
#endif

#endif //W8_WAVE_COMMON_H
