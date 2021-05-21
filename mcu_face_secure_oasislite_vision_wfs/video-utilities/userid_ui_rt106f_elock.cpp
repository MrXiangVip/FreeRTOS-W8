/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.d
 *
 * Created by: NXP China Solution Team.
 */
#if RT106F_ELOCK_BOARD

#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"

#include "commondef.h"
#include "database.h"
#include "display.h"
#include "font.h"
#include "oasis.h"
#include "pxp.h"
#include "sln_dev_cfg.h"
#include "sln_system_state.h"
#include "string.h"
#include "util.h"
#include "userid_ui.h"

//lock picture
#include <lock_80_80.h>
#include <unlock_80_80.h>
//#if RTFFI_RV_DISP_DRIVER
#include "greenlock_30x38.h"
// 20201114 wavezgx added for UI development
#include "fsl_log.h"

#include <aw_rec_ing1_v7.h>
#include <aw_rec_ing2_v7.h>
#include <aw_rec_ing3_v7.h>
#include <aw_rec_ing4_v7.h>
#include <aw_bat_disp1_v6.h>
#include <aw_bat_disp2_v6.h>
#include <aw_bat_disp3_v6.h>
#include <aw_bat_disp4_v6.h>
#include <aw_bat_disp5_v6.h>
#include <aw_bat_low_v6.h>
#include <aw_bat_low_img1_v3.h>
#include <aw_bat_low_img2_v3.h>
#include <aw_bat_low_text1_v3.h>
#include <aw_bat_low_text2_v3.h>
#include <aw_reg_anibg_v1.h>
#include <aw_reg_anifg1_v1.h>
#include <aw_reg_anifg2_v1.h>
#include <aw_user_exist_v1.h>
#include <aw_reg_fail_v3.h>
#include <aw_reg_succ_v3.h>
#include <aw_welcome_v4.h>
// 20201114 wavezgx end
#include "MCU_UART5.h"
//#include "time.h"
//#include "timers.h"
#include "aw_wstime.h"

// wszgx added 20201118
#define tickless_task_PRIORITY   ( configMAX_PRIORITIES - 2 )
#define SW_task_PRIORITY   ( configMAX_PRIORITIES - 1 )
#define TIME_DELAY_SLEEP      5000
/* The software timer period. */
#define SW_TIMER_PERIOD_MS (1000 / portTICK_PERIOD_MS)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* The callback function. */
static void SwTimerCallback(TimerHandle_t xTimer);
static bool timer_started = false;
// wszgx end 20201118

//#include "commondef.h"
#include "redlock_30x38.h"
//#endif
#include "welcomehome_240x131.h"
#include "oasis.h"

#define LINE_DIMS  5

#define POS_DB_COUNT_X 2
#define POS_DB_COUNT_Y 5

#define POS_QUALITY_INFO_X 2
#define POS_QUALITY_INFO_Y 170


#define POS_NAME_X 240
#define POS_NAME_Y 20

#define POS_EMOTION_X 240
#define POS_EMOTION_Y 45

#define POS_CONF_X 240
#define POS_CONF_Y 75

#define POS_DT_X 180
#define POS_DT_Y 145
#define POS_RT_X 180
#define POS_RT_Y 170
#define POS_TT_X 180
#define POS_TT_Y 195

#define POS_SW1_X 12
#define POS_TO_X 15
#define POS_LiveView_X 9

#ifdef SHOW_FPS
#define POS_SW1_Y 195
#define POS_TO_Y 210
#define POS_LiveView_Y 225
#else
#define POS_SW1_Y 175
#define POS_TO_Y 195
#define POS_LiveView_Y 215
#endif

#define POS_REGISTRATION_X 70
#define POS_REGISTRATION_Y 10

#define RGB565_RED 0xf800
#define RGB565_GREEN 0x07e0
#define RGB565_BLUE 0x001f
// 20201114 wavezgx added for UI development
#define RGB565_WHITE 0xffff

#define RGB565_NXPGREEN 0xBEA6
#define RGB565_NXPRED 0xFD83
#define RGB565_NXPBLUE 0x6D5B

extern int g_IsAuthenticated;
extern EventGroupHandle_t g_syncVideoEvents;

static int color = 0x0;

const char* emotion_str[] = {
    "Anger",
    "Disgust",
    "Fear",
    "Happy",
    "Sad",
    "Surprised",
    "Normal",
};
// 20201114 wavezgx added for UI development
static int logindex = 0;
static int logfreq = 30;
static int bat_level = 0;
// 20201114 wavezgx end

static void draw_text(char* pText, int x, int y, int text_color, int bg_color, font_vizn type, uint16_t* pCanvasBuffer)
{
    put_string(x, y, pText, text_color, bg_color, type, pCanvasBuffer, APP_AS_WIDTH);
}

static void draw_icon(uint16_t* pIcon, int x, int y, int w, int h, int alpha, uint16_t* pCanvasBuffer)
{
    for (int i = y; i < (y + h); i++)
    {
        for (int j = x; j < (x + w); j++)
        {
            if (*pIcon <= alpha)
                *(pCanvasBuffer + i*APP_AS_WIDTH + j) = *pIcon++;
            else
                pIcon++;
        }
    }
}

static void draw_fillrect(int x, int y, int w, int h, int rect_color, uint16_t* pCanvasBuffer)
{
    for (int i = x; i < (x + w); i++)
        for (int j = y; j < (y + h); j++)
            *(pCanvasBuffer + j*APP_AS_WIDTH + i) = rect_color;
}

static void UIInfo_UpdateLeftInfoBar(uint16_t* pAsBufferAddr, uint16_t *pIcon, QUIInfoMsg infoMsg, uint8_t p_isLiveView)
{
    char tstring[64];

    // Create left bar
    for (int i = 0; i < LCD_HEIGHT; i++)
    {
        for (int j = 0; j < FACEICON_WIDTH; j++)
        {
            if ((i >= (LCD_HEIGHT - FACEICON_HEIGHT) / 2) && (i < (LCD_HEIGHT + FACEICON_HEIGHT) / 2))
            {
                *(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
            }
            else
            {
                *(pAsBufferAddr + i*APP_AS_WIDTH + j) = 0xffffU; // 0xad75; //0xdf7a;//0xffffU;
            }
        }
    }

    // show the db count
    memset(tstring, 0x0, 64);
    sprintf(tstring, "Registered");
    put_string(POS_DB_COUNT_X, POS_DB_COUNT_Y, tstring, RGB565_BLUE, -1, OPENSANS8, pAsBufferAddr, APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    int db_count = 0;
    DB_Count(&db_count);
    sprintf(tstring, "Users:%d", db_count);
    put_string(POS_DB_COUNT_X, POS_DB_COUNT_Y + 15, tstring, RGB565_BLUE, -1, OPENSANS8, pAsBufferAddr,
               APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "Auth:%d", g_IsAuthenticated);
    put_string(POS_DB_COUNT_X, POS_DB_COUNT_Y + 30, tstring, RGB565_BLUE, -1, OPENSANS8, pAsBufferAddr,
               APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "Mode:%s", ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode() ? "Manual" : "Auto");
    put_string(POS_DB_COUNT_X, POS_DB_COUNT_Y + 45, tstring, RGB565_BLUE, -1, OPENSANS8, pAsBufferAddr,
               APP_AS_WIDTH);

#ifdef SHOW_FPS
    memset(tstring, 0x0, 64);
    sprintf(tstring, "FPS:%.2f", infoMsg.fps);
    put_string(POS_DB_COUNT_X, POS_DB_COUNT_Y + 60, tstring, RGB565_BLUE, -1, OPENSANS8, pAsBufferAddr,
               APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "DFPS:%.2f", infoMsg.detect_fps);
    put_string(POS_DB_COUNT_X, POS_SW1_Y - 35, tstring, RGB565_BLUE, -1, OPENSANS8, pAsBufferAddr, APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "RFPS:%.2f", infoMsg.recognize_fps);
    put_string(POS_DB_COUNT_X, POS_SW1_Y - 20, tstring, RGB565_BLUE, -1, OPENSANS8, pAsBufferAddr, APP_AS_WIDTH);
#endif

    if ((ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode())
        && ((Oasis_GetState()== OASIS_STATE_FACE_ADD_START)
            ||(Oasis_GetState()== OASIS_STATE_FACE_ADD_STARTED)))
    {
      memset(tstring, 0x0, 64);
      sprintf(tstring, "Registering");
      put_string(POS_REGISTRATION_X, POS_REGISTRATION_Y, tstring, RGB565_GREEN, -1, OPENSANS8, pAsBufferAddr, APP_AS_WIDTH);
    }

    if ((ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode())
        && ((Oasis_GetState()== OASIS_STATE_FACE_DEL_START)
            ||(Oasis_GetState()== OASIS_STATE_FACE_DEL_STARTED)))
    {
        memset(tstring, 0x0, 64);
        sprintf(tstring, "Unregistering");
        put_string(POS_REGISTRATION_X, POS_REGISTRATION_Y, tstring, RGB565_RED, -1, OPENSANS8, pAsBufferAddr,
                   APP_AS_WIDTH);
    }

    memset(tstring, 0x0, 64);
    sprintf(tstring, "SW1");
    put_string(POS_SW1_X, POS_SW1_Y, tstring, RGB565_BLUE, -1, OPENSANS8, pAsBufferAddr, APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "to");
    put_string(POS_TO_X, POS_TO_Y, tstring, RGB565_BLUE, -1, OPENSANS8, pAsBufferAddr, APP_AS_WIDTH);

    if (p_isLiveView)
    {
        memset(tstring, 0x0, 64);
        sprintf(tstring, "Exit");
        put_string(POS_LiveView_X, POS_LiveView_Y, tstring, RGB565_BLUE, -1, OPENSANS8, pAsBufferAddr,
                   APP_AS_WIDTH);
    }
    else
    {
        memset(tstring, 0x0, 64);
        sprintf(tstring, "LiveView");
        put_string(POS_LiveView_X, POS_LiveView_Y, tstring, RGB565_BLUE, -1, OPENSANS8, pAsBufferAddr,
                   APP_AS_WIDTH);
    }
}

static void UIInfo_UpdateQualityInfo(uint16_t* pAsBufferAddr, QUIInfoMsg infoMsg)
{
    char tstring[64];
    int color;
    font_vizn type = OPENSANS16;
    memset(tstring, 0x0, 64);
    sprintf(tstring, "sim:%.2f",infoMsg.similar/100.0f);

    if((int)infoMsg.similar > 0)
        color = RGB565_GREEN;
    else
        color = RGB565_RED;

    put_string(POS_QUALITY_INFO_X, POS_QUALITY_INFO_Y, tstring, color, -1, type, pAsBufferAddr, APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "blur:%d", infoMsg.blur);
    if(infoMsg.blur == 1)
    {
        color = RGB565_RED;
    }
    else if(infoMsg.blur == 0)
    {
        color = RGB565_GREEN;
    }
    else
    {
        color = RGB565_BLUE;
    }
    put_string(POS_QUALITY_INFO_X, POS_QUALITY_INFO_Y + 20, tstring, color, -1, type, pAsBufferAddr,
           APP_AS_WIDTH);


    memset(tstring, 0x0, 64);
    sprintf(tstring, "irLuma:%d",infoMsg.irBrightness);

    if(infoMsg.irBrightness < 255)
    {
        color = RGB565_GREEN;
    }
    else
    {
        color = RGB565_BLUE;
    }
    put_string(POS_QUALITY_INFO_X+110, POS_QUALITY_INFO_Y, tstring, color, -1, type, pAsBufferAddr,
               APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "irPWM:%d",infoMsg.irPwm);

    if(infoMsg.irPwm <= 100)
    {
        color = RGB565_GREEN;
    }
    else
    {
        color = RGB565_BLUE;
    }

    put_string(POS_QUALITY_INFO_X+110, POS_QUALITY_INFO_Y + 20, tstring, color, -1, type, pAsBufferAddr,
           APP_AS_WIDTH);
    memset(tstring, 0x0, 64);
    sprintf(tstring, "rgbLuma:%d",infoMsg.rgbBrightness);

    if(infoMsg.rgbBrightness < 255)
    {
        color = RGB565_GREEN;
    }
    else
    {
        color = RGB565_BLUE;
    }
    put_string(POS_QUALITY_INFO_X+110, POS_QUALITY_INFO_Y + 20 + 20, tstring, color, -1, type, pAsBufferAddr,
               APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "rgbPWM:%d",infoMsg.rgbPwm);

    if(infoMsg.rgbPwm <= 100)
    {
        color = RGB565_GREEN;
    }
    else
    {
        color = RGB565_BLUE;
    }

    put_string(POS_QUALITY_INFO_X+110, POS_QUALITY_INFO_Y + 20 + 20 +20, tstring, color, -1, type, pAsBufferAddr,
           APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "rgbLive:%d",infoMsg.rgbLive);
    if(infoMsg.rgbLive == 0)
    {
        color = RGB565_RED;
    }
    else if(infoMsg.rgbLive == 1)
    {
        color = RGB565_GREEN;
    }
    else
    {
        color = RGB565_BLUE;
    }
    put_string(POS_QUALITY_INFO_X, POS_QUALITY_INFO_Y + 40, tstring, color, -1, type, pAsBufferAddr,
               APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "irLive:%d",infoMsg.irLive);
    if(infoMsg.irLive == 0)
    {
        color = RGB565_RED;
    }
    else if(infoMsg.irLive == 1)
    {
        color = RGB565_GREEN;
    }
    else
    {
        color = RGB565_BLUE;
    }

    put_string(POS_QUALITY_INFO_X, POS_QUALITY_INFO_Y + 60, tstring, color, -1, type, pAsBufferAddr,
               APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "front:%d",infoMsg.front);
    if(infoMsg.front == 0)
    {
        color = RGB565_RED;
    }
    else if(infoMsg.front == 1)
    {
        color = RGB565_GREEN;
    }
    else
    {
        color = RGB565_BLUE;
    }

    put_string(POS_QUALITY_INFO_X, POS_QUALITY_INFO_Y + 80, tstring, color, -1, type, pAsBufferAddr,
               APP_AS_WIDTH);

}


static void UIInfo_UpdateBottomInfoBar(uint16_t* pAsBufferAddr, QUIInfoMsg infoMsg, uint8_t p_isLiveView)
{
    std::string name = infoMsg.name;
    if (p_isLiveView == 0)
    {
        // show the lock/unlock icon
        uint16_t *pIcon = (uint16_t *)lock;
        if (!name.empty())
        {
            pIcon = (uint16_t *)unlock;
        }
        UIInfo_UpdateLeftInfoBar(pAsBufferAddr, pIcon, infoMsg, p_isLiveView);
        // reset color
        color = 0;
        return;
    }

    char tstring[64];
    int y_shift = POS_DB_COUNT_Y + 189 + 80;
#if AUTO_INFOBAR_HIDE
    int y2 = infoMsg.rect[3];
    if ((y2 + 15) > APP_AS_HEIGHT)
        y_shift = APP_AS_HEIGHT;
    else if ((y2 + 30) > APP_AS_HEIGHT)
        y_shift = POS_DB_COUNT_Y + 219;
    else if ((y2 + 45) > APP_AS_HEIGHT)
        y_shift = POS_DB_COUNT_Y + 204;
    else
        y_shift = POS_DB_COUNT_Y + 189;
#endif

    for (int i = 120; i < APP_AS_WIDTH; i++)
        for (int j = y_shift + 30; j < APP_AS_HEIGHT; j++)
            *(pAsBufferAddr + j*APP_AS_WIDTH + i) = RGB565_NXPBLUE;
    for (int i = 0; i < 120; i++)
        for (int j = y_shift + 30; j < APP_AS_HEIGHT; j++)
            *(pAsBufferAddr + j*APP_AS_WIDTH + i) = RGB565_NXPRED;


    memset(tstring, 0x0, 64);
    int db_count = 0;
    DB_Count(&db_count);
    sprintf(tstring, "Registered Users:%d", db_count);
    if (y_shift <= (POS_DB_COUNT_Y + 189 + 80))
        put_string(POS_DB_COUNT_X + 122, y_shift + 30, tstring, RGB565_BLUE, RGB565_NXPBLUE, OPENSANS8,
                pAsBufferAddr, APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "App:%s", (APP_TYPE_ELOCK_LIGHT == Cfg_AppDataGetApplicationType()
        || APP_TYPE_ELOCK_HEAVY == Cfg_AppDataGetApplicationType())? "elock" : "access");
    if (y_shift <= (POS_DB_COUNT_Y + 189 + 80))
        put_string(POS_DB_COUNT_X + 35+4, y_shift + 30, tstring, RGB565_BLUE, RGB565_NXPRED, OPENSANS8,
                pAsBufferAddr, APP_AS_WIDTH);


    if ((ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode())
        && ((Oasis_GetState()== OASIS_STATE_FACE_ADD_START)
            ||(Oasis_GetState()== OASIS_STATE_FACE_ADD_STARTED)
            ||(Oasis_GetState()== OASIS_STATE_FACE_DEL_START)
            ||(Oasis_GetState()== OASIS_STATE_FACE_DEL_STARTED)))
    {
        for (int i = 0; i < 320; i++)
        {
            for (int j = 0; j < 20; j++)
            {
                *(pAsBufferAddr + i*APP_AS_WIDTH + j) = RGB565_NXPBLUE; // 0xad75; //0xdf7a;//0xffffU;
                *(pAsBufferAddr + i*APP_AS_WIDTH + j + 220) = RGB565_NXPBLUE;
            }
        } 
     }

    if ((ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode())
        && ((Oasis_GetState()== OASIS_STATE_FACE_ADD_START)
            ||(Oasis_GetState()== OASIS_STATE_FACE_ADD_STARTED)))
    {
        memset(tstring, 0x0, 64);
        sprintf(tstring, "Registering");
        put_string(POS_REGISTRATION_X, POS_REGISTRATION_Y + 6, tstring, 0, RGB565_GREEN, OPENSANS16, pAsBufferAddr,
                   APP_AS_WIDTH);
        return;
    }

    if ((ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode())
        && ((Oasis_GetState()== OASIS_STATE_FACE_DEL_START)
            ||(Oasis_GetState()== OASIS_STATE_FACE_DEL_STARTED)))
    {
        memset(tstring, 0x0, 64);
        sprintf(tstring, "Unregistering");
        put_string(POS_REGISTRATION_X-10, POS_REGISTRATION_Y + 6, tstring, 0, RGB565_RED, OPENSANS16, pAsBufferAddr,
                   APP_AS_WIDTH);
        return;
    }

    uint16_t *pIcon = NULL;
    if (!name.empty())
    {
        pIcon = (uint16_t *)greenlock_30x38;
    }
    else
        pIcon = (uint16_t *)redlock_30x38;
    for (int i = 282; i < 320; i++)
    {
        for (int j = 5; j < 35; j++)
        {
            if (*pIcon <= 0xfc00)
                *(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
            else
                pIcon++;
        }
    }
}

static void UIInfo_UpdateBottomInfoBarV2(uint16_t* pAsBufferAddr, QUIInfoMsg infoMsg, uint8_t p_isLiveView)
{
    std::string name = infoMsg.name;
    if (logindex % logfreq == 0) {
    	//LOGD("UIInfo_UpdateBottomInfoBarV2 p_isLiveView %d\r\n", p_isLiveView);
    }
	// 20201114 wavezgx added for UI development
#if 1
	// show date and time in the topbar
	if(bSysTimeSynProc) {
	    struct ws_tm current;
	    memset((void*)&current,0x00,sizeof(current));
	    ws_localtime(ws_systime, &current);
	    char idxstring[64];
	    memset(idxstring, 0x0, 64);
	    sprintf(idxstring, "%04d-%02d-%02d", current.tm_year, current.tm_mon + 1, current.tm_mday);
	    put_string(2, 0, idxstring, RGB565_WHITE, -1, OPENSANS8,
	    		pAsBufferAddr, APP_AS_WIDTH);
	    sprintf(idxstring, "%02d:%02d:%02d", current.tm_hour, current.tm_min, current.tm_sec);
	    put_string(102, 0, idxstring, RGB565_WHITE, -1, OPENSANS8,
	    		pAsBufferAddr, APP_AS_WIDTH);
	}
#endif

#if 1
	// show battery level icon in the topright corner of the screen
	if(bInitSyncInfo) {
		uint16_t *pIcon = NULL;
		// 20201119 wszgx modified for battery level display
		// >5:80-100 4:64-79 3: 48-63 2: 32-47 1: 16-31 0: 0-15
		bat_level = (stInitSyncInfo.PowerVal / 16);
		//LOGD("PowerVal is %d bat_level is %d\r\n", stInitSyncInfo.PowerVal, bat_level);
		if (bat_level == 0) {
			pIcon = (uint16_t *)bat_disp1_v6;
			int img_idx = (logindex) % 12;
			if (img_idx < 6) {
				for (int i = 0; i < 12; i++)
				{
					for (int j = 216; j < 238; j++)
					{
						if (*pIcon <= 0x0090) {
							pIcon++;
						} else {
							*(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
						}
					}
				}
			}
		} else {
			if (bat_level == 1) {
				pIcon = (uint16_t *)bat_disp1_v6;
			} else if (bat_level == 2) {
				pIcon = (uint16_t *)bat_disp2_v6;
			} else if (bat_level == 3) {
				pIcon = (uint16_t *)bat_disp3_v6;
			} else if (bat_level == 4) {
				pIcon = (uint16_t *)bat_disp4_v6;
			} else {
				pIcon = (uint16_t *)bat_disp5_v6;
			}
			for (int i = 0; i < 12; i++)
			{
				for (int j = 216; j < 238; j++)
				{
					if (*pIcon <= 0x0090) {
						pIcon++;
					} else {
						*(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
					}
				}
			}
		}
#endif
#if 1
		// if the battery level is 0, show the low battery warning icon for about 5 seconds 
	    if (bat_level == 0 && logindex <= 30) {
	    	if ((logindex) % 6 < 3) {
	    		uint16_t *pIcon2 = NULL;
	    		pIcon2 = (uint16_t *)bat_low_img1_v3;
	    		for (int i = 120; i < 199; i++)
	    		{
	    			for (int j = 81; j < 158; j++)
	    			{
	    				if (*pIcon2 >= 0x1000) {
	    					*(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon2++;
	    				} else {
	    					pIcon2++;
	    				}
	    			}
	    		}
	    	} else {
	    		uint16_t *pIcon2 = NULL;
	    		pIcon2 = (uint16_t *)bat_low_img2_v3;
	    		for (int i = 120; i < 199; i++)
	    		{
	    			for (int j = 81; j < 158; j++)
	    			{
	    				if (*pIcon2 >= 0x1000) {
	    					*(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon2++;
	    				} else {
	    					pIcon2++;
	    				}
	    			}
	    		}
	    	}
	    	uint16_t *pIcon3 = NULL;
	    	pIcon3 = (uint16_t *)bat_low_text1_v3;
	    	for (int i = 220; i < 257; i++)
	    	{
	    		for (int j = 81; j < 158; j++)
	    		{
	    			if (*pIcon3 >= 0x1000) {
	    				*(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon3++;
	    			} else {
	    				pIcon3++;
	    			}
	    		}
	    	}
	    }
	}
#endif
}
// 20201114 wavezgx end

static void UIInfo_UpdateEmotion(uint16_t* pAsBufferAddr, QUIInfoMsg infoMsg, int x1, int x2, int y1, int y2, int dim, uint8_t p_isLiveView)
{
    char tstring[64];

    // show the emotion info
    if (infoMsg.emotion >= E_ANGER && infoMsg.emotion <= E_SURPRISED) {
        sprintf(tstring, emotion_str[infoMsg.emotion]);
        if (p_isLiveView) {
            put_string(x2 - dim - get_stringwidth(tstring, OPENSANS8), y1 + dim, tstring, 0x0, color, OPENSANS8, pAsBufferAddr, APP_AS_WIDTH);
        } else {
            int eColor = RGB565_GREEN;
            if (infoMsg.name[0] == 0) {
                eColor = RGB565_RED;
            }
            put_string(POS_EMOTION_X - get_stringwidth(tstring, OPENSANS16), POS_EMOTION_Y, tstring, eColor, -1, OPENSANS16, pAsBufferAddr, APP_AS_WIDTH);
        }
    }

}

static void UIInfo_DrawFocusRect(uint16_t* pAsBufferAddr, int x1, int x2, int y1, int y2)
{
    int reg_rect_color;
    if((Oasis_GetState()== OASIS_STATE_FACE_DEL_START)
        || (Oasis_GetState()== OASIS_STATE_FACE_DEL_STARTED)
        || (Oasis_GetState()== OASIS_STATE_FACE_ADD_START)
        || (Oasis_GetState()== OASIS_STATE_FACE_ADD_STARTED))
    {
        reg_rect_color = RGB565_RED;
    }
    else if((Oasis_GetState()== OASIS_STATE_FACE_REC_START)
        || (Oasis_GetState()== OASIS_STATE_FACE_REC_STARTED))
    {
        reg_rect_color = RGB565_BLUE;
    }
    else
    {
        return;
    }

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            *(pAsBufferAddr + (i+y1)*APP_AS_WIDTH + j + x1) = reg_rect_color;
            *(pAsBufferAddr + (i+y1)*APP_AS_WIDTH + j + x2-20) = reg_rect_color;
            *(pAsBufferAddr + (i+y2)*APP_AS_WIDTH + j + x1) = reg_rect_color;
            *(pAsBufferAddr + (i+y2)*APP_AS_WIDTH + j + x2-20) = reg_rect_color;

            *(pAsBufferAddr + (j+y1)*APP_AS_WIDTH + i + x1) = reg_rect_color;
            *(pAsBufferAddr + (j+y2-20)*APP_AS_WIDTH + i + x1-2) = reg_rect_color;
            *(pAsBufferAddr + (j+y1)*APP_AS_WIDTH + i + x2) = reg_rect_color;
            *(pAsBufferAddr + (j+y2-20)*APP_AS_WIDTH + i + x2-2) = reg_rect_color;
        }
    }

}

// 20201114 wavezgx added for UI development
static void UIInfo_DrawFocusRectV2(uint16_t* pAsBufferAddr, int x1, int x2, int y1, int y2)
{
    if((Oasis_GetState()== OASIS_STATE_FACE_DEL_START)
        || (Oasis_GetState()== OASIS_STATE_FACE_DEL_STARTED)
        || (Oasis_GetState()== OASIS_STATE_FACE_ADD_START)
        || (Oasis_GetState()== OASIS_STATE_FACE_ADD_STARTED))
    {
    	uint16_t *pIcon = NULL;
    	pIcon = (uint16_t *)reg_anibg_v1;
    	int x_start = (APP_AS_WIDTH - reg_anibg_v1_W) / 2;
    	int x_end = x_start + reg_anibg_v1_W;
    	int y_start = (APP_AS_HEIGHT - reg_anibg_v1_H) / 2;
    	int y_end = y_start + reg_anibg_v1_H;
    	for (int i = y_start; i < y_end; i++) {
    		for (int j = x_start; j < x_end; j++) {
    			if (*pIcon >= 0x1000) {
    				*(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
    			} else {
    				pIcon++;
    			}
    		}
    	}

    	int step = 23;
    	int img_idx = (logindex) % step;
    	uint16_t *pIcon2 = NULL;
    	pIcon2 = (uint16_t *)reg_anifg1_v1;
    	x_start = (APP_AS_WIDTH - reg_anifg1_v1_W) / 2;
    	x_end = x_start + reg_anifg1_v1_W;
    	int offset = 6 * img_idx;
    	y_start = (APP_AS_HEIGHT - reg_anibg_v1_H) / 2 + offset - 2;
    	y_end = y_start + reg_anifg1_v1_H + offset;
    	for (int x = y_start + 6; x < y_end; x++) {
    		for (int y = x_start; y < x_end; y++) {
    			if (*pIcon2 >= 0x4000) {
    				*(pAsBufferAddr + x*APP_AS_WIDTH + y) = *pIcon2++;
    			} else {
    				pIcon2++;
    			}
    		}
    	}
    } else if((Oasis_GetState()== OASIS_STATE_FACE_REC_START)
        || (Oasis_GetState()== OASIS_STATE_FACE_REC_STARTED)) {
    	uint16_t *pIcon = NULL;
    	int img_idx = (logindex) % 4;
    	if (img_idx == 0) {
    		pIcon = (uint16_t *)rec_ing1_v7;
    	} else if (img_idx == 1) {
    		pIcon = (uint16_t *)rec_ing2_v7;
    	} else if (img_idx == 2) {
    		pIcon = (uint16_t *)rec_ing3_v7;
    	} else if (img_idx == 3) {
    		pIcon = (uint16_t *)rec_ing4_v7;
    	}
    	int x_start = (APP_AS_WIDTH - rec_ing1_v7_W) / 2;
    	int x_end = x_start + rec_ing1_v7_W;
    	int y_start = (APP_AS_HEIGHT - rec_ing1_v7_H) / 2;
    	int y_end = y_start + rec_ing1_v7_H;
    	for (int i = y_start; i < y_end; i++) {
    		for (int j = x_start; j < x_end; j++) {
    			if (*pIcon >= 0x1000) {
    				*(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
    			} else {
    				pIcon++;
    			}
    		}
    	}
    }
}
// 20201114 wavezgx end

static void UIInfo_UpdateNameConf(uint16_t* pAsBufferAddr, QUIInfoMsg infoMsg, int x1, int x2, int y1, int y2, std::string name, int similar, int dim, uint8_t p_isLiveView)
{
    //face name and confidence
    char tstring[64];

    if (p_isLiveView)
    {
        if(!name.empty()&&(Oasis_GetState()==OASIS_STATE_FACE_REC_STOPPED)){
            sprintf(tstring, "%s:lock opened", name.c_str());
            put_string(10, 10, tstring, 0, RGB565_GREEN,
                               OPENSANS16, pAsBufferAddr, APP_AS_WIDTH);

            uint16_t *pIcon = (uint16_t *)welcomehome_240x131;

            for (int i = 60; i < 191; i++)
            {
                for (int j = 0; j < 240; j++)
                {
                    *(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
                }
            }
        }
        if ((Oasis_GetState()==OASIS_STATE_FACE_ADD_STOPPED)&& !name.empty())
        {
            if(infoMsg.newface ) {
                sprintf(tstring, "%s Added", name.c_str());
                put_string(70, 100, tstring, 0x0, RGB565_GREEN, OPENSANS16,
                        pAsBufferAddr, APP_AS_WIDTH);
            }
            else
            {
                sprintf(tstring, "%s had", name.c_str());
                put_string(70, 100, tstring, 0x0, RGB565_GREEN, OPENSANS16,
                        pAsBufferAddr, APP_AS_WIDTH);
                sprintf(tstring, "registered", name.c_str());
                put_string(70, 130, tstring, 0x0, RGB565_GREEN, OPENSANS16,
                        pAsBufferAddr, APP_AS_WIDTH);
            }
        }
        if((Oasis_GetState() == OASIS_STATE_FACE_DEL_STOPPED) && !name.empty())
        {
            sprintf(tstring, "%s removed", name.c_str());
            put_string(50, 100, tstring, 0x0, RGB565_GREEN, OPENSANS16,
                    pAsBufferAddr, APP_AS_WIDTH);
        }

    }
    else
    {
        if (infoMsg.newface)
        {
            sprintf(tstring, "%s ADDED", name.c_str());
            put_string(70, 100, tstring, RGB565_GREEN, -1,
                       OPENSANS16, pAsBufferAddr, APP_AS_WIDTH);
        }
        else
        {

        }
        // show the debug information

        memset(tstring, 0x0, 64);
        sprintf(tstring, "Dt: %d", infoMsg.dt);
        put_string(POS_DT_X, POS_DT_Y, tstring, RGB565_GREEN, -1, OPENSANS16, pAsBufferAddr, APP_AS_WIDTH);

        memset(tstring, 0x0, 64);
        sprintf(tstring, "Rt: %d", infoMsg.rt);
        put_string(POS_RT_X, POS_RT_Y, tstring, RGB565_GREEN, -1, OPENSANS16, pAsBufferAddr, APP_AS_WIDTH);

        memset(tstring, 0x0, 64);
        sprintf(tstring, "Tt: %d", (int)(infoMsg.dt + infoMsg.rt));
        put_string(POS_TT_X, POS_TT_Y, tstring, RGB565_GREEN, -1, OPENSANS16, pAsBufferAddr, APP_AS_WIDTH);
    }
}

// 20201114 wavezgx added for UI development, shutdownCamera is not used now but may be used in the future
static void shutdownCamera(uint16_t* pAsBufferAddr) {
	gpio_pin_config_t pinConfig = {
			kGPIO_DigitalOutput,
			0,
			kGPIO_NoIntmode
	};
	// pinConfig.outputLogic = 1;
	// GPIO_PinInit(BOARD_CAMERA_SWITCH_GPIO, BOARD_CAMERA_SWITCH_GPIO_PIN, &pinConfig);    
	// RGB camera work under normal mode.    
	pinConfig.outputLogic = 1;
	GPIO_PinInit(BOARD_CAMERA_PWD_GPIO, BOARD_CAMERA_PWD_GPIO_PIN, &pinConfig);
	// IR camera work under normal mode.    
	pinConfig.outputLogic = 1;
	GPIO_PinInit(BOARD_CAMERA_IR_PWD_GPIO, BOARD_CAMERA_IR_PWD_GPIO_PIN, &pinConfig);
    memset(pAsBufferAddr, 0, 2 * (APP_AS_HEIGHT * APP_AS_WIDTH - 1));
}

static void UIInfo_UpdateNameConfV2(uint16_t* pAsBufferAddr, QUIInfoMsg infoMsg, int x1, int x2, int y1, int y2, std::string name, int similar, int dim, uint8_t p_isLiveView)
{
    //face name and confidence
    char tstring[64];

    //LOGD("UIInfo_UpdateNameConfV2 state is %d, name is %s\r\n", Oasis_GetState(), name.c_str());
    if (p_isLiveView)
    {
    	// zgx: succeed to recognize and welcome home
        if(!name.empty()&&(Oasis_GetState()==OASIS_STATE_FACE_REC_STOPPED)){
            uint16_t *pIcon = NULL;
            pIcon = (uint16_t *)welcome_v4;
            for (int i = 0; i < 320; i++)
            {
            	for (int j = 0; j < 240; j++)
            	{
            		*(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
            	}
            }
        }
		// failed to register user
        if ((Oasis_GetState()==OASIS_STATE_FACE_ADD_STOPPED)&& name.empty()) {
        	uint16_t *pIcon = (uint16_t *)reg_fail_v3;
        	for (int i = 0; i < 320; i++)
        	{
        		for (int j = 0; j < 240; j++)
        		{
        			//            if (*pIcon <= 0x0090) {
        			//            	pIcon++;
        			//            } else {
        			*(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
        			//            }
        		}
        	}
        	return;
        }
        if ((Oasis_GetState()==OASIS_STATE_FACE_ADD_STOPPED)&& !name.empty())
        {
        	// succeed to register new user
            if(infoMsg.newface ) {
            	uint16_t *pIcon = (uint16_t *)reg_succ_v3;
            	for (int i = 0; i < 320; i++)
            	{
            		for (int j = 0; j < 240; j++)
            		{
            			*(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
            		}
            	}
            } else {
            	// zgx: already registered and not allowed to redo register
            	uint16_t *pIcon = (uint16_t *)user_exist_v1;
            	for (int i = 0; i < 320; i++)
            	{
            		for (int j = 0; j < 240; j++)
            		{
            			*(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
            		}
            	}
            }
        }
        if((Oasis_GetState() == OASIS_STATE_FACE_DEL_STOPPED) && !name.empty())
        {
            sprintf(tstring, "%s removed", name.c_str());
            put_string(50, 100, tstring, 0x0, RGB565_GREEN, OPENSANS16,
                    pAsBufferAddr, APP_AS_WIDTH);
        }

    }
}
// 20201114 zgx end

static void UIInfo_UpdateFaceBox(uint16_t* pAsBufferAddr, int x1, int x2, int y1, int y2, int dim, uint8_t p_isLiveView)
{
    if (p_isLiveView)
    {
        // face rect
        for (int i = x1; i < x2; i++)
        {
            for (int j = y1; j < (y1 + dim); j++)
            {
                *(pAsBufferAddr + j*APP_AS_WIDTH + i) = (uint16_t)color;
            };
        }

        for (int i = y1; i < y2; i++)
        {
            for (int j = x1; j < (x1 + dim); j++)
            {
                *(pAsBufferAddr + i*APP_AS_WIDTH + j) = (uint16_t)color;
            }
        }

        for (int i = x1 + dim; i < x2; i++)
        {
            for (int j = 0; j < dim; j++)
            {
                *(pAsBufferAddr + (y2 - 1 - j)*APP_AS_WIDTH + i) = (uint16_t)color;
            }
        }

        for (int i = y1 + dim; i < y2; i++)
        {
            for (int j = 0; j < dim; j++)
            {
                *(pAsBufferAddr + i*APP_AS_WIDTH + (x2 - 1 - j)) = (uint16_t)color;
            }
        }
    }
    else
    {
        // TODO for future improvements
    }
}

/*!
 * @brief Software timer callback.
 */
static void SwTimerCallback(TimerHandle_t xTimer)
{
    LOGD("T1ick.\r\n");
}


void UIInfo_Update(uint16_t* pAsBufferAddr, QUIInfoMsg infoMsg, bool erase, uint8_t p_isLiveView)
{
    int scale_factor = APP_CAMERA_WIDTH / APP_PS_HEIGHT;
    int shift_x = (APP_CAMERA_HEIGHT - REC_RECT_WIDTH) / 2;
    int shift_y = (APP_CAMERA_WIDTH - REC_RECT_HEIGHT) / 2;

    int x1                           = CAMERA_SURFACE_SHIFT + (infoMsg.rect[0] + shift_x) / scale_factor;
    int y1                           = (infoMsg.rect[1] + shift_y) / scale_factor;
    int x2                           = CAMERA_SURFACE_SHIFT + (infoMsg.rect[2] + shift_x) / scale_factor;
    int y2                           = (infoMsg.rect[3] + shift_y) / scale_factor;
    std::string name                 = infoMsg.name;
    int similar                      = (int)(infoMsg.similar * 10000 / 100);
    static system_states_t sys_state = sysStateDetectedNoUser;



	// 20201114 wavezgx added for UI development
	logindex++;
	
    // clean the screen
    if (p_isLiveView)
    {
        memset(pAsBufferAddr, 0, 2 * (APP_AS_HEIGHT * APP_AS_WIDTH - 1));
    }
    else
    {
        memset(pAsBufferAddr, 0xff, 2 * (APP_AS_HEIGHT * APP_AS_WIDTH - 1));
    }

    if (!erase)
    {
        if (name.empty())
        {
            color     = 0xF800U;
            sys_state = sysStateDetectedUnknownUser;
        }
        else
        {
            color     = 0x07E0; // 0xBEA6; //0x8FE0U;
            sys_state = sysStateDetectedKnownUser;
        }
    }

	// 20201114 wavezgx modified for UI development
//    UIInfo_UpdateBottomInfoBar(pAsBufferAddr, infoMsg, p_isLiveView);
    UIInfo_UpdateBottomInfoBarV2(pAsBufferAddr, infoMsg, p_isLiveView);

    // face detected only if it fits in the PS space from the screen
    if ((x1 >= CAMERA_SURFACE_SHIFT) && (y1 >= 0) && (x2 >= CAMERA_SURFACE_SHIFT) && (y2 > 0) && (x1 != x2) &&
        (y1 != y2) && (x2 < APP_AS_WIDTH) && (y2 < APP_AS_HEIGHT))
    {
        SysState_Set(sys_state);
        //UIInfo_UpdateFaceBox(pAsBufferAddr, x1, x2, y1, y2, LINE_DIMS, p_isLiveView);

        UIInfo_UpdateEmotion(pAsBufferAddr, infoMsg, x1, x2, y1, y2, LINE_DIMS, p_isLiveView);

        if (!name.empty()) {
            //face recognize
			// 20201114 wavezgx modified for UI development
//            UIInfo_UpdateNameConf(pAsBufferAddr, infoMsg, x1, x2, y1, y2, name, similar, LINE_DIMS, p_isLiveView);
            UIInfo_UpdateNameConfV2(pAsBufferAddr, infoMsg, x1, x2, y1, y2, name, similar, LINE_DIMS, p_isLiveView);
        }
        else
        {
			// 20201114 wavezgx modified for UI development,for user register failure
            UIInfo_UpdateNameConfV2(pAsBufferAddr, infoMsg, x1, x2, y1, y2, name, similar, LINE_DIMS, p_isLiveView);
            // face unknown
            if (!p_isLiveView)
            {
                char tstring[64];
                memset(tstring, 0x0, 64);
                sprintf(tstring, "UNKNOWN");
                put_string(POS_CONF_X - get_stringwidth(tstring, OPENSANS16), POS_CONF_Y, tstring, RGB565_RED, -1,
                           OPENSANS16, pAsBufferAddr, APP_AS_WIDTH);
            }
        }
        if(Cfg_AppDataGetVerbosity() == VERBOSE_MODE_L3)
		    UIInfo_UpdateQualityInfo(pAsBufferAddr,infoMsg);
    }
    else
    {
        // face not detected
        SysState_Set(sysStateDetectedNoUser);
        name = "";
    }

    if(name.empty()) {
		// 20201114 wavezgx modified for UI development
//        UIInfo_DrawFocusRect(pAsBufferAddr,60,180,100,220);
         UIInfo_DrawFocusRectV2(pAsBufferAddr,60,180,100,220);
    }
}

#endif
