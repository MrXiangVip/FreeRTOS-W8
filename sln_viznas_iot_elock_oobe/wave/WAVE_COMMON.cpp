//
// Created by xshx on 2022/5/20.
//
#include <vector>
#include <string>

#include "board.h"
#include "pin_mux.h"
#include "fsl_lpuart_freertos.h"
#include "fsl_lpuart.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "sln_api.h"
#include "MCU_UART5.h"
#include "util_crc16.h"
#include "commondef.h"
#include "user_info.h"

#include "fsl_log.h"

#if defined(FSL_RTOS_FREE_RTOS)
#include "FreeRTOS.h"
#endif

#include "util.h"
#include "fsl_pjdisp.h"
#include "fatfs_op.h"
#include "camera_rt106f_elock.h"
#include "WAVE_COMMON.h"

bool lcd_back_ground = true;
static const char *logtag ="[WAVE-COMMON]-";
QueueHandle_t  Uart5FromFakeUartMsgQueue;
ws_time_t ws_systime = 0;

void OpenLcdBackground() {
    if (!lcd_back_ground) {
        LOGD("[%s]:\r\n", __FUNCTION__);
        lcd_back_ground = true;
        //if(stInitSyncInfo.LightVal == 2) {
        //	OpenCameraPWM();
        //}
        PJDisp_TurnOnBacklight();
    }
}

void CloseLcdBackground() {
#if  SUPPORT_POWEROFF
    if (lcd_back_ground) {
        LOGD("[%s]:\r\n", __FUNCTION__);
        lcd_back_ground = false;
        //if(stInitSyncInfo.LightVal == 2) {
        //CloseCameraPWM();
        //}
        PJDisp_TurnOffBacklight();
    }
#endif
}

void SendMessageToUart5FromFakeUart(  char *data){

    if( Uart5FromFakeUartMsgQueue != NULL ){
        UartMessage message;
        memset(&message, 0, sizeof(UartMessage));
        strcpy(message.Data, data);
        if(xQueueSend( Uart5FromFakeUartMsgQueue, (void *)&message, 0 ) == pdPASS ){
            LOGD("%s send [ DATA=%s] \r\n",logtag,   message.Data );

        }else{
            LOGD("%s could not send to the queue \r\n",logtag);
        }
        taskYIELD();
    }else{
        LOGD("create  message queue first \r\n");
    }
}

