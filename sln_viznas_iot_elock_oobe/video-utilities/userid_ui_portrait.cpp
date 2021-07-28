/*
 * Copyright 2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.d
 *
 * Created by: NXP China Solution Team.
 */

#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"

#include "commondef.h"
#include "sln_api_internal.h"
#include "database.h"
#include "display.h"
#include "font.h"
#include "oasis.h"
#include "pxp.h"
#include "sln_dev_cfg.h"
#include "sln_connection.h"
#include "string.h"
#include "util.h"
#include "userid_ui.h"
#include "sln_audio.h"

// lock picture
#include "lock_80_80.h"
#include "unlock_80_80.h"
#include "greenlock_30x38.h"
#include "redlock_30x38.h"
#include "wifi_16x16.h"
#include "ble_16x16.h"

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
#include "aw_wstime.h"

#if SCREEN_PORTRAIT_MODE
#include "welcomehome_240x131.h"
#include "oasis.h"

/*******************************************************************************
 * Definitions
 *******************************************************************************/
#define LINE_DIMS 5

#define LOCK_SPACING 2
#define LOCK_WIDTH   greenlock_W

#define POS_QUALITY_INFO_X 2
#define POS_QUALITY_INFO_Y 170

#define POS_NAME_X 240
#define POS_NAME_Y 20

#define POS_EMOTION_X 240
#define POS_EMOTION_Y 45

#define POS_REGISTRATION_X 70
#define POS_REGISTRATION_Y 10

#define RGB565_RED   0xf800
#define RGB565_GREEN 0x07e0
#define RGB565_BLUE  0x001f
#define RGB565_WHITE 0xffff

#define RGB565_NXPGREEN 0xBEA6
#define RGB565_NXPRED   0xFD83
#define RGB565_NXPBLUE  0x6D5B

#define RECT_HEIGHT         20
#define RECT_WIDTH          (APP_AS_WIDTH - LOCK_WIDTH - LOCK_SPACING) / 3
#define RED_RECT_WIDTH      (RECT_WIDTH + LOCK_WIDTH + LOCK_SPACING)
#define GREEN_RECT_WIDTH    RED_RECT_WIDTH
#define BLUE_RECT_WIDTH     (APP_AS_WIDTH - RED_RECT_WIDTH - GREEN_RECT_WIDTH)
#define POS_RECT_Y          (APP_AS_HEIGHT - RECT_HEIGHT)
#define POS_NXPRED_RECT_X   0
#define POS_NXPBLUE_RECT_X  (POS_NXPRED_RECT_X + RED_RECT_WIDTH)
#define POS_NXPGREEN_RECT_X (POS_NXPBLUE_RECT_X + BLUE_RECT_WIDTH)

#define WIFI_ICON_RELATIVE_X    2
#define BLE_ICON_RELATIVE_X     WIFI_ICON_RELATIVE_X + WIFI_W + LOCK_SPACING / 2
#define REGISTRATION_RELATIVE_X 2
#define MODE_RELATIVE_X         (LOCK_SPACING + LOCK_WIDTH)
#define REGISTRATION_RELATIVE_Y 3
#define MODE_RELATIVE_Y         3
#define APP_RELATIVE_X          (LOCK_SPACING + LOCK_WIDTH)
#define APP_RELATIVE_Y          3
/*******************************************************************************
 * Variables
 *******************************************************************************/
uint32_t s_OasisEvents;

extern volatile uint8_t g_AddNewFace;
extern volatile uint8_t g_RemoveExistingFace;
extern volatile uint8_t g_FaceSystemLocked;
static int s_Color        = 0x0;
const char *emotion_str[] = {
    "Anger", "Disgust", "Fear", "Happy", "Sad", "Surprised", "Normal",
};

// 20201114 wavezgx added for UI development
static int logindex = 0;
static int bat_level = 0;
// 20201114 wavezgx end
extern int boot_mode;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void draw_text(char *pText, int x, int y, int text_color, int bg_color, font_vizn type, uint16_t *pCanvasBuffer);
static void draw_icon(uint16_t *pIcon, int x, int y, int w, int h, int alpha, uint16_t *pCanvasBuffer);
static void draw_fillrect(int x, int y, int w, int h, int rect_color, uint16_t *pCanvasBuffer);
static void UIInfo_UpdateBottomInfoBar(uint16_t *pBufferAddr, QUIInfoMsg* infoMsg, uint8_t appType);
static void UIInfo_FaceGuideLines(uint16_t *pBufferAddr);
static void UIInfo_UpdateFaceInfo(uint16_t *pBufferAddr, QUIInfoMsg* infoMsg);
static void UIInfo_UpdateFaceBoxUYVY(uint16_t *pBufferAddr, int x1, int x2, int y1, int y2, int dim);
static void UIInfo_UpdateFaceBoxRGB565(uint16_t *pBufferAddr, int x1, int x2, int y1, int y2, int dim);
static void UIInfo_UpdateNameConf(uint16_t *pBufferAddr, std::string name, int x1, int x2, int y1, int y2, int dim);
static void UIInfo_Userid(uint16_t *pBufferAddr, QUIInfoMsg* infoMsg, uint8_t p_DisplayInterfaceMode);
static void UIInfo_Elock(uint16_t *pBufferAddr, QUIInfoMsg* infoMsg, uint8_t p_DisplayInterfaceMode);

/*******************************************************************************
 * Code
 *******************************************************************************/

static void draw_text(char *pText, int x, int y, int text_color, int bg_color, font_vizn type, uint16_t *pCanvasBuffer)
{
    put_string(x, y, pText, text_color, bg_color, type, pCanvasBuffer, APP_AS_WIDTH);
}

static void draw_icon(uint16_t *pIcon, int x, int y, int w, int h, int alpha, uint16_t *pCanvasBuffer)
{
    for (int i = y; i < (y + h); i++)
    {
        for (int j = x; j < (x + w); j++)
        {
            if (*pIcon <= alpha)
                *(pCanvasBuffer + i * APP_AS_WIDTH + j) = *pIcon++;
            else
                pIcon++;
        }
    }
}

static void draw_fillrect(int x, int y, int w, int h, int rect_color, uint16_t *pCanvasBuffer)
{
    for (int i = x; i < (x + w); i++)
        for (int j = y; j < (y + h); j++)
            *(pCanvasBuffer + j * APP_AS_WIDTH + i) = rect_color;
}

static void UIInfo_UpdateQualityInfo(uint16_t *pAsBufferAddr, QUIInfoMsg* infoMsg)
{
    char tstring[64];
    int color;
    font_vizn type = OPENSANS16;

    if (Cfg_AppDataGetVerbosity() != VERBOSE_MODE_L3)
        return;

    memset(tstring, 0x0, 64);
    sprintf(tstring, "sim:%.2f",infoMsg->similar/100.0f);

    if ((int)infoMsg->similar > 0)
    {
        color = RGB565_BLUE;
    }
    else
    {
        color = RGB565_RED;
    }
    draw_text(tstring, POS_QUALITY_INFO_X + CAMERA_SURFACE_SHIFT, POS_QUALITY_INFO_Y, color, -1, type, pAsBufferAddr);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "blur:%d", infoMsg->blur);
    if (infoMsg->blur == 1)
    {
        color = RGB565_RED;
    }
    else if (infoMsg->blur == 0)
    {
        color = RGB565_GREEN;
    }
    else
    {
        color = RGB565_BLUE;
    }
    draw_text(tstring, POS_QUALITY_INFO_X + CAMERA_SURFACE_SHIFT, POS_QUALITY_INFO_Y + 20, color, -1, type,
              pAsBufferAddr);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "rgbLive:%d", infoMsg->rgbLive);
    if (infoMsg->rgbLive == 0)
    {
        color = RGB565_RED;
    }
    else if (infoMsg->rgbLive == 1)
    {
        color = RGB565_GREEN;
    }
    else
    {
        color = RGB565_BLUE;
    }
    draw_text(tstring, POS_QUALITY_INFO_X + CAMERA_SURFACE_SHIFT, POS_QUALITY_INFO_Y + 40, color, -1, type,
              pAsBufferAddr);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "irLive:%d", infoMsg->irLive);
    if (infoMsg->irLive == 0)
    {
        color = RGB565_RED;
    }
    else if (infoMsg->irLive == 1)
    {
        color = RGB565_GREEN;
    }
    else
    {
        color = RGB565_BLUE;
    }

    draw_text(tstring, POS_QUALITY_INFO_X + CAMERA_SURFACE_SHIFT, POS_QUALITY_INFO_Y + 60, color, -1, type,
              pAsBufferAddr);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "front:%d", infoMsg->front);
    if (infoMsg->front == 0)
    {
        color = RGB565_RED;
    }
    else if (infoMsg->front == 1)
    {
        color = RGB565_GREEN;
    }
    else
    {
        color = RGB565_BLUE;
    }

    draw_text(tstring, POS_QUALITY_INFO_X + CAMERA_SURFACE_SHIFT, POS_QUALITY_INFO_Y + 80, color, -1, type,
              pAsBufferAddr);
}

static void UIInfo_UpdateBottomInfoBar(uint16_t *pBufferAddr, QUIInfoMsg* infoMsg, uint8_t appType)
{
    char tstring[64];
    int db_count     = 0;
    std::string name = infoMsg->name;
    uint16_t *pIcon  = NULL;

    draw_fillrect(POS_NXPGREEN_RECT_X, POS_RECT_Y, GREEN_RECT_WIDTH, RECT_HEIGHT, RGB565_NXPGREEN, pBufferAddr);
    draw_fillrect(POS_NXPBLUE_RECT_X, POS_RECT_Y, BLUE_RECT_WIDTH, RECT_HEIGHT, RGB565_NXPBLUE, pBufferAddr);
    draw_fillrect(POS_NXPRED_RECT_X, POS_RECT_Y, RED_RECT_WIDTH, RECT_HEIGHT, RGB565_NXPRED, pBufferAddr);

    DB_Count(&db_count);
    memset(tstring, 0x0, 64);
    sprintf(tstring, "Registered Users:%d", db_count);
    draw_text(tstring, POS_NXPGREEN_RECT_X + REGISTRATION_RELATIVE_X, POS_RECT_Y + REGISTRATION_RELATIVE_Y, RGB565_BLUE,
              RGB565_NXPGREEN, OPENSANS8, pBufferAddr);

    memset(tstring, 0x0, 64);
    if (APP_TYPE_USERID == appType)
    {
        sprintf(tstring, "APP: %s", "userid");
    }
    else if (APP_TYPE_ELOCK_LIGHT == appType || APP_TYPE_ELOCK_HEAVY == appType)
    {
        sprintf(tstring, "APP: %s", "elock");
    }
    else
    {
        sprintf(tstring, "APP: %s", "access");
    }
    draw_text(tstring, POS_NXPRED_RECT_X + APP_RELATIVE_X, POS_RECT_Y + APP_RELATIVE_Y, RGB565_BLUE, RGB565_NXPRED,
              OPENSANS8, pBufferAddr);

    // memset(tstring, 0x0, 64);
    // sprintf(tstring, "Mode:%s", ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode() ? "Manual" : "Auto");
    // draw_text(tstring, POS_NXPRED_RECT_X + MODE_RELATIVE_X, POS_RECT_Y + REGISTRATION_RELATIVE_Y, RGB565_BLUE,
    //           RGB565_NXPRED, OPENSANS8, pBufferAddr);

    if (SLN_Connection_WIFI_isConnected())
    {
        pIcon = (uint16_t *)wifi16x16_data;
    }
    else
    {
        pIcon = (uint16_t *)no_wifi16x16_data;
    }
    draw_icon(pIcon, POS_NXPBLUE_RECT_X + WIFI_ICON_RELATIVE_X, 223+80, WIFI_W, WIFI_H, 0xE000, pBufferAddr);
    if (SLN_Connection_BLE_isOn())
    {
        pIcon = (uint16_t *)bluetooth16x16_data;
    }
    else
    {
        pIcon = (uint16_t *)no_bluetooth16x16_data;
    }
    draw_icon(pIcon, POS_NXPBLUE_RECT_X + BLE_ICON_RELATIVE_X, 223+80, BLE_W, BLE_H, 0xE000, pBufferAddr);
    // draw lock icon

    if (!name.empty())
    {
        pIcon = (uint16_t *)greenlock_30x38;
    }
    else
    {
        pIcon = (uint16_t *)redlock_30x38;
    }
    draw_icon(pIcon, LOCK_SPACING / 2 + POS_NXPRED_RECT_X, 202+80, 30, 38, 0xfc00, pBufferAddr);
}

static void UIInfo_UpdateBottomInfoBarV2(uint16_t *pBufferAddr, QUIInfoMsg* infoMsg, uint8_t appType)
{
    // show date and time in the topbar
    if(bSysTimeSynProc) {
        struct ws_tm current;
        memset((void*)&current,0x00,sizeof(current));
        ws_localtime(ws_systime + 8*60*60, &current);
        char idxstring[32];
        memset(idxstring, 0x0, 64);
        sprintf(idxstring, "%04d-%02d-%02d", current.tm_year, current.tm_mon + 1, current.tm_mday);
        put_string(2, 0, idxstring, RGB565_WHITE, -1, OPENSANS8,
        		pBufferAddr, APP_AS_WIDTH);
        sprintf(idxstring, "%02d:%02d:%02d", current.tm_hour, current.tm_min, current.tm_sec);
        put_string(102, 0, idxstring, RGB565_WHITE, -1, OPENSANS8,
        		pBufferAddr, APP_AS_WIDTH);
    }

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
                            *(pBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
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
                        *(pBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
                    }
                }
            }
        }

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
                            *(pBufferAddr + i*APP_AS_WIDTH + j) = *pIcon2++;
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
                            *(pBufferAddr + i*APP_AS_WIDTH + j) = *pIcon2++;
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
                        *(pBufferAddr + i*APP_AS_WIDTH + j) = *pIcon3++;
                    } else {
                        pIcon3++;
                    }
                }
            }
        }
    }
}

static void UIInfo_UpdateOasisState(uint16_t *pBufferAddr)
{
    char tstring[64];
    if (g_AddNewFace)
    {
        memset(tstring, 0x0, 64);
        sprintf(tstring, "Registering");
        draw_text(tstring, CAMERA_SURFACE_SHIFT + POS_REGISTRATION_X, POS_REGISTRATION_Y + 6, 0, RGB565_GREEN,
                  OPENSANS16, pBufferAddr);
    }
    else if (g_RemoveExistingFace)
    {
        memset(tstring, 0x0, 64);
        sprintf(tstring, "Deregistering");
        draw_text(tstring, CAMERA_SURFACE_SHIFT + POS_REGISTRATION_X, POS_REGISTRATION_Y + 6, 0, RGB565_RED, OPENSANS16,
                  pBufferAddr);
    }
}

static void UIInfo_FaceGuideLines(uint16_t *pBufferAddr)
{
    char tstring[64];
    int reg_rect_color;
    memset(tstring, 0x0, 64);

    // registration and unregistration page
    if (!g_FaceSystemLocked)
    {
        //  need mask both side during face adding and removing
        if ((g_AddNewFace || g_RemoveExistingFace))
        {
            draw_fillrect(0, 0, 30, 320, RGB565_NXPBLUE, pBufferAddr);
            draw_fillrect(210, 0, 30, 320, RGB565_NXPBLUE, pBufferAddr);
        }

        if (g_AddNewFace)
            reg_rect_color = RGB565_GREEN;
        else if (g_RemoveExistingFace)
            reg_rect_color = RGB565_RED;
        else
            reg_rect_color = RGB565_NXPBLUE;

        draw_fillrect(30, 10+20, 2, 60, reg_rect_color, pBufferAddr);
        draw_fillrect(30, 150+20, 2, 60, reg_rect_color, pBufferAddr);
        draw_fillrect(210, 10+20, 2, 60, reg_rect_color, pBufferAddr);
        draw_fillrect(210, 150+20, 2, 60, reg_rect_color, pBufferAddr);

        draw_fillrect(30, 10+20, 60, 2, reg_rect_color, pBufferAddr);
        draw_fillrect(30, 208+20, 60, 2, reg_rect_color, pBufferAddr);
        draw_fillrect(150, 10+20, 60, 2, reg_rect_color, pBufferAddr);
        draw_fillrect(150, 208+20, 60, 2, reg_rect_color, pBufferAddr);
    }
}

static void UIInfo_UpdateFaceInfo(uint16_t *pBufferAddr, QUIInfoMsg* infoMsg)
{
    char tstring[64];
    std::string name = infoMsg->name;

    switch (s_OasisEvents)
    {
        case kEvents_API_Layer_NoEvent:
            break;
        case 1 << kEvents_API_Layer_FaceExist:
        {
            sprintf(tstring, "Already Registered");
            draw_text(tstring, CAMERA_SURFACE_SHIFT +10, 10, 0x0, RGB565_RED, OPENSANS16, pBufferAddr);
        }
        break;
        case 1 << kEvents_API_Layer_DeregFailed:
        {
            sprintf(tstring, "Remove Failed");
            draw_text(tstring, CAMERA_SURFACE_SHIFT + 10, 10, 0x0, RGB565_RED, OPENSANS16, pBufferAddr);
        }
        break;
        case 1 << kEvents_API_Layer_DeregSuccess:
        {
            sprintf(tstring, "%s removed", name.c_str());
            draw_text(tstring, CAMERA_SURFACE_SHIFT + 30, 10, 0x0, RGB565_GREEN, OPENSANS16, pBufferAddr);
        }
        break;
        case 1 << kEvents_API_Layer_RegSuccess:
        {
            sprintf(tstring, "%s Added", name.c_str());
            draw_text(tstring, CAMERA_SURFACE_SHIFT + 30, 10, 0x0, RGB565_GREEN, OPENSANS16, pBufferAddr);
        }
        break;
        case 1 << kEvents_API_Layer_RegFailed:
        {
            sprintf(tstring, "Registration Failed");
            draw_text(tstring, CAMERA_SURFACE_SHIFT + 10, 10, 0x0, RGB565_RED, OPENSANS16, pBufferAddr);
        }
        break;
        case 1 << kEvents_API_Layer_RecSuccess:
        {
            uint16_t *pIcon;
            sprintf(tstring, "%s:Unlocked", name.c_str());
            draw_text(tstring, CAMERA_SURFACE_SHIFT+10, 10, 0, RGB565_GREEN, OPENSANS16, pBufferAddr);

            pIcon = (uint16_t *)welcomehome_240x131;
            draw_icon(pIcon, CAMERA_SURFACE_SHIFT, 60, welcomehome_W, welcomehome_H, 0xffff, pBufferAddr);
        }
        break;

        case 1 << kEvents_API_Layer_RecFailed:
        {
            sprintf(tstring, "Recognition Timeout");
            draw_text(tstring, CAMERA_SURFACE_SHIFT + 10, 10, 0x0, RGB565_RED, OPENSANS16, pBufferAddr);
        }
        break;
        default:
            break;
    }
}

static void UIInfo_UpdateFaceInfo2(uint16_t *pBufferAddr, QUIInfoMsg* infoMsg)
{
    char tstring[64];
    std::string name = infoMsg->name;

    switch (s_OasisEvents)
    {
        case kEvents_API_Layer_NoEvent:
            break;
        case 1 << kEvents_API_Layer_FaceExist:
        {
            uint16_t *pIcon = (uint16_t *)user_exist_v1;
            for (int i = 0; i < 320; i++)
            {
                for (int j = 0; j < 240; j++)
                {
                    *(pBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
                }
            }
        }
        break;
        case 1 << kEvents_API_Layer_DeregFailed:
        {
            sprintf(tstring, "Remove Failed");
            draw_text(tstring, CAMERA_SURFACE_SHIFT + 10, 10, 0x0, RGB565_RED, OPENSANS16, pBufferAddr);
        }
        break;
        case 1 << kEvents_API_Layer_DeregSuccess:
        {
            sprintf(tstring, "%s removed", name.c_str());
            draw_text(tstring, CAMERA_SURFACE_SHIFT + 30, 10, 0x0, RGB565_GREEN, OPENSANS16, pBufferAddr);
        }
        break;
        case 1 << kEvents_API_Layer_RegSuccess:
        {
            uint16_t *pIcon = (uint16_t *)reg_succ_v3;
			for (int i = 0; i < 320; i++)
			{
				for (int j = 0; j < 240; j++)
				{
					*(pBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
				}
			}
			CloseLcdBackground();

        }
        break;
        case 1 << kEvents_API_Layer_RegFailed:
        {
            uint16_t *pIcon = (uint16_t *)reg_fail_v3;
        	for (int i = 0; i < 320; i++)
        	{
        		for (int j = 0; j < 240; j++)
        		{
        			*(pBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
        		}
        	}
        	CloseLcdBackground();
        }
        break;
        case 1 << kEvents_API_Layer_RecSuccess:
        {
#if	0
            uint16_t *pIcon = NULL;
            pIcon = (uint16_t *)welcome_v4;
            for (int i = 0; i < 320; i++)
            {
            	for (int j = 0; j < 240; j++)
            	{
            		*(pBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
            	}
            }
#endif
        }
        break;

        case 1 << kEvents_API_Layer_RecFailed:
        {
            sprintf(tstring, "Recognition Timeout");
            draw_text(tstring, CAMERA_SURFACE_SHIFT + 10, 10, 0x0, RGB565_RED, OPENSANS16, pBufferAddr);
        }
        break;
        default:
            break;
    }
}


static void UIInfo_DrawFocusRectV2(uint16_t* pAsBufferAddr, QUIInfoMsg* infoMsg)
{
    if((Oasis_GetState()== OASIS_STATE_FACE_DEREG_START)
        || (Oasis_GetState()== OASIS_STATE_FACE_REG_START))
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
    } else if((Oasis_GetState()== OASIS_STATE_FACE_REC_START) || (Oasis_GetState()== OASIS_STATE_FACE_REC_STOP)) {
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

static void UIInfo_UpdateFaceBoxUYVY(uint16_t *pBufferAddr, int x1, int x2, int y1, int y2, int dim)
{
    uint16_t UY, VY, camera_width;
    if (pBufferAddr == NULL)
        return;
    // the conversion si 2 pixel in 2 bytes

    camera_width = APP_CAMERA_WIDTH;
    RGB5652YUV((uint16_t)s_Color, (uint16_t)s_Color, &UY, &VY);

    x1  = x1 - x1 % 2;
    x2  = x2 - x2 % 2;
    dim = dim - dim % 2;

    // x1 and x2 and dim are even are even

    for (int i = x1; i < x2; i = i + 2)
    {
        for (int j = y1; j < (y1 + dim); j++)
        {
            *(pBufferAddr + j * camera_width + i)     = UY;
            *(pBufferAddr + j * camera_width + i + 1) = VY;
        }
    }
    for (int i = y1; i < y2; i++)
    {
        for (int j = x1; j < (x1 + dim - 1); j = j + 2)
        {
            *(pBufferAddr + i * camera_width + j)     = UY;
            *(pBufferAddr + i * camera_width + j + 1) = VY;
        }
    }

    for (int i = x1 + dim; i < x2 - 1; i = i + 2)
    {
        for (int j = 0; j < dim; j++)
        {
            *(pBufferAddr + (y2 - 1 - j) * camera_width + i)     = UY;
            *(pBufferAddr + (y2 - 1 - j) * camera_width + i + 1) = VY;
        }
    }

    for (int i = y1 + dim; i < y2; i++)
    {
        for (int j = 0; j < dim - 1; j = j + 2)
        {
            *(pBufferAddr + i * camera_width + (x2 - j - 2)) = UY;
            *(pBufferAddr + i * camera_width + (x2 - j - 1)) = VY;
        }
    }
}

static void UIInfo_UpdateFaceBoxRGB565(uint16_t *pBufferAddr, int x1, int x2, int y1, int y2, int dim)
{
    if (pBufferAddr == NULL)
        return;
    // face rect
    draw_fillrect(x1, y1, x2 - x1 + 1, dim, s_Color, pBufferAddr);
    draw_fillrect(x1, y1, dim, y2 - y1 + 1, s_Color, pBufferAddr);
    draw_fillrect(x2 - dim, y1, dim, y2 - y1 + 1, s_Color, pBufferAddr);
    draw_fillrect(x1, y2 - dim, x2 - x1 + 1, dim, s_Color, pBufferAddr);
}

static void UIInfo_UpdateNameConf(uint16_t *pBufferAddr, std::string name, int x1, int x2, int y1, int y2, int dim)
{
    // face name and confidence
    if (name.empty())
    {
        return;
    }

    if (pBufferAddr == NULL)
    {
        return;
    }
    put_string(x1 + dim, y1 + dim, (char *)name.c_str(), RGB565_BLACK, s_Color, OPENSANS8, pBufferAddr, APP_AS_WIDTH);
}


static void UIInfo_Userid(uint16_t *pBufferAddr, QUIInfoMsg* infoMsg, uint8_t p_DisplayInterfaceMode)
{
    uint16_t screenWidth, screenHeight;
    int scale_factor = APP_CAMERA_WIDTH / APP_PS_WIDTH;
    int shift_x      = (APP_CAMERA_WIDTH - REC_RECT_WIDTH) / 2;
    int shift_y      = (APP_CAMERA_HEIGHT - REC_RECT_HEIGHT) / 2;
    if (p_DisplayInterfaceMode == DISPLAY_INTERFACE_LOOPBACK)
    {
        // The box is added before the scaling
        scale_factor = 1;
    }

    int x1 = CAMERA_SURFACE_SHIFT + (infoMsg->rect[0] + shift_x) / scale_factor;
    int y1 = (infoMsg->rect[1] + shift_y) / scale_factor;
    int x2 = CAMERA_SURFACE_SHIFT + (infoMsg->rect[2] + shift_x) / scale_factor;
    int y2 = (infoMsg->rect[3] + shift_y) / scale_factor;

    screenWidth  = APP_CAMERA_WIDTH / scale_factor;
    screenHeight = APP_CAMERA_HEIGHT / scale_factor;

    if ((x1 >= CAMERA_SURFACE_SHIFT) && (y1 >= 0) && (x2 >= CAMERA_SURFACE_SHIFT) && (y2 > 0) && (x1 != x2) &&
        (y1 != y2) && x2 < screenWidth && y2 < screenHeight)
    {
        std::string name = infoMsg->name;

        if (name.empty())
        {
            s_Color = RGB565_RED;
        }
        else
        {
            s_Color = RGB565_GREEN;
        }

        if (p_DisplayInterfaceMode == DISPLAY_INTERFACE_INFOBAR)
        {
            UIInfo_UpdateFaceBoxRGB565(pBufferAddr, x1, x2, y1, y2, LINE_DIMS);
            UIInfo_UpdateNameConf(pBufferAddr, name, x1, x2, y1, y2, LINE_DIMS);
        }
        else
        {
            UIInfo_UpdateFaceBoxUYVY(pBufferAddr, x1, x2, y1, y2, LINE_DIMS);
        }
    }
}

static void UIInfo_Elock(uint16_t *pBufferAddr, QUIInfoMsg* infoMsg, uint8_t p_DisplayInterfaceMode)
{
    if (p_DisplayInterfaceMode == DISPLAY_INTERFACE_INFOBAR)
    {
        logindex ++;

        memset(pBufferAddr, 0, 2 * (APP_AS_HEIGHT * APP_AS_WIDTH - 1));
        //UIInfo_FaceGuideLines(pBufferAddr);
        VIZN_GetEvents(NULL, &s_OasisEvents);
        UIInfo_UpdateFaceInfo2(pBufferAddr, infoMsg);
        UIInfo_UpdateQualityInfo(pBufferAddr, infoMsg);
        if((boot_mode == BOOT_MODE_NORMAL) || (boot_mode == BOOT_MODE_REG)) {
        	UIInfo_DrawFocusRectV2(pBufferAddr, infoMsg);
        }
    }
}

void UIInfo_Update(uint16_t *pBufferAddr, QUIInfoMsg* infoMsg, uint8_t p_DisplayInterfaceMode)
{
    uint8_t appType = Cfg_AppDataGetApplicationType();
    if (p_DisplayInterfaceMode == DISPLAY_INTERFACE_INFOBAR)
    {
        memset(pBufferAddr, 0, 2 * (APP_AS_HEIGHT * APP_AS_WIDTH - 1));
    }
    switch (appType)
    {
        case APP_TYPE_USERID:
            UIInfo_Userid(pBufferAddr, infoMsg, p_DisplayInterfaceMode);
            break;
        case APP_TYPE_ELOCK_LIGHT:
        case APP_TYPE_ELOCK_HEAVY:
        case APP_TYPE_DOOR_ACCESS_LIGHT:
        case APP_TYPE_DOOR_ACCESS_HEAVY:
            UIInfo_Elock(pBufferAddr, infoMsg, p_DisplayInterfaceMode);
            break;

        default:
            while (1)
            {
            }
    }
    if (p_DisplayInterfaceMode == DISPLAY_INTERFACE_INFOBAR)
    {
        UIInfo_UpdateOasisState(pBufferAddr);
        //UIInfo_UpdateBottomInfoBar(pBufferAddr, infoMsg, appType);
        UIInfo_UpdateBottomInfoBarV2(pBufferAddr, infoMsg, appType);
    }
}

#endif //SCREEN_PORTRAIT_MODE
