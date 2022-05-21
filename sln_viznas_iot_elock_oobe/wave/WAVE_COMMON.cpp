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


