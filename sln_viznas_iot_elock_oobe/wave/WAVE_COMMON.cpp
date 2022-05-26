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

#include "cJSON.h"
#include "util.h"
#include "fsl_pjdisp.h"
#include "fatfs_op.h"
#include "camera_rt106f_elock.h"
#include "WAVE_COMMON.h"

bool lcd_back_ground = true;
static const char *logtag ="[WAVE-COMMON]-";
QueueHandle_t  Uart5FromFakeUartMsgQueue;

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
            LOGD("%s send [ DATA=%s] \n",logtag,   message.Data );

        }else{
            LOGD("%s could not send to the queue \n",logtag);
        }
        taskYIELD();
    }else{
        LOGD("create  message queue first \n");
    }
}

//
void vConvertRegistClass2UserExtend(RegisteClass *regist, UserExtend  *userExtend){
    LOGD("%s 转换注册类为用户扩展结构 \r\n",logtag );

    char 	*cjson_str;
    cJSON * cObj = cJSON_CreateObject();
    cJSON_AddStringToObject(cObj, UERID,  regist->UUID);
    cJSON_AddNumberToObject(cObj, TIMES, regist->uStartTime);
    cJSON_AddNumberToObject(cObj, TIMEE, regist->uEndTime);
    cJSON_AddStringToObject(cObj, ADEV, regist->cDeviceId);
    cjson_str = cJSON_PrintUnformatted(cObj);

    LOGD("cjson %s \r\n", cjson_str);
    memcpy( userExtend->UUID, regist->UUID, sizeof(regist->UUID));
    memcpy( userExtend->jsonData, cjson_str, strlen(cjson_str));
    LOGD("UserExtend UUID:%s \r\n", userExtend->UUID);
    LOGD("UserExtend jsonData:%s \r\n", userExtend->jsonData);
};