#if RTVISION_BOARD

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
#if RTFFI_RV_DISP_DRIVER
#include "greenlock_30x38.h"
#include "redlock_30x38.h"
#endif
#include "welcomehome_320x122.h"
#include "oasis.h"

#define LINE_DIMS  5

#define POS_DB_COUNT_X 2
#define POS_DB_COUNT_Y 5

#define POS_QUALITY_INFO_X 2
#define POS_QUALITY_INFO_Y 90


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

#define POS_REGISTRATION_X 100
#define POS_REGISTRATION_Y 10

#define RGB565_RED 0xf800
#define RGB565_GREEN 0x07e0
#define RGB565_BLUE 0x001f

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

    if ((ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode()) && (Oasis_GetState()==OASIS_STATE_FACE_ADD))
    {
      memset(tstring, 0x0, 64);
      sprintf(tstring, "Registering");
      put_string(POS_REGISTRATION_X, POS_REGISTRATION_Y, tstring, RGB565_GREEN, -1, OPENSANS8, pAsBufferAddr, APP_AS_WIDTH);
    }

    if ((ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode()) && (Oasis_GetState()==OASIS_STATE_FACE_DEL))
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
    sprintf(tstring, "sim:%d",(int)infoMsg.similar);

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
    put_string(POS_QUALITY_INFO_X, POS_QUALITY_INFO_Y + 15, tstring, color, -1, type, pAsBufferAddr,
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
    put_string(POS_QUALITY_INFO_X, POS_QUALITY_INFO_Y + 30, tstring, color, -1, type, pAsBufferAddr,
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

    put_string(POS_QUALITY_INFO_X, POS_QUALITY_INFO_Y + 45, tstring, color, -1, type, pAsBufferAddr,
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

    put_string(POS_QUALITY_INFO_X, POS_QUALITY_INFO_Y + 60, tstring, color, -1, type, pAsBufferAddr,
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
    int y_shift = POS_DB_COUNT_Y + 189;
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

    for (int i = 130; i < APP_AS_WIDTH; i++)
        for (int j = y_shift + 30; j < APP_AS_HEIGHT; j++)
            *(pAsBufferAddr + j*APP_AS_WIDTH + i) = RGB565_NXPBLUE;
    for (int i = 0; i < 160; i++)
        for (int j = y_shift + 30; j < APP_AS_HEIGHT; j++)
            *(pAsBufferAddr + j*APP_AS_WIDTH + i) = RGB565_NXPRED;


    memset(tstring, 0x0, 64);
    int db_count = 0;
    DB_Count(&db_count);
    sprintf(tstring, "Registered Users:%d", db_count);
    if (y_shift <= (POS_DB_COUNT_Y + 189))
        put_string(POS_DB_COUNT_X + 162, y_shift + 30, tstring, RGB565_BLUE, RGB565_NXPBLUE, OPENSANS8,
                pAsBufferAddr, APP_AS_WIDTH);

    memset(tstring, 0x0, 64);
    sprintf(tstring, "Mode:%s", ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode() ? "Manual" : "Auto");
    if (y_shift <= (POS_DB_COUNT_Y + 189))
        put_string(POS_DB_COUNT_X + 60, y_shift + 30, tstring, RGB565_BLUE, RGB565_NXPRED, OPENSANS8,
                pAsBufferAddr, APP_AS_WIDTH);


    if ((ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode())
        && ((Oasis_GetState()==OASIS_STATE_FACE_ADD)
        || (Oasis_GetState()==OASIS_STATE_FACE_DEL)))
    {
        for (int i = 0; i < 240; i++)
        {
            for (int j = 0; j < 60; j++)
            {
                *(pAsBufferAddr + i*APP_AS_WIDTH + j) = RGB565_NXPBLUE; // 0xad75; //0xdf7a;//0xffffU;
                *(pAsBufferAddr + i*APP_AS_WIDTH + j + 260) = RGB565_NXPBLUE;
            }
        }
     }

    if ((ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode()) && (Oasis_GetState()==OASIS_STATE_FACE_ADD))
    {
        memset(tstring, 0x0, 64);
        sprintf(tstring, "Registering");
        put_string(POS_REGISTRATION_X, POS_REGISTRATION_Y + 6, tstring, 0, RGB565_GREEN, OPENSANS16, pAsBufferAddr,
                   APP_AS_WIDTH);
        return;
    }

    if ((ENROLMENT_MODE_MANUAL == Cfg_AppDataGetEnrolmentMode()) && (Oasis_GetState()== OASIS_STATE_FACE_DEL))
    {
        memset(tstring, 0x0, 64);
        sprintf(tstring, "Unregistering");
        put_string(POS_REGISTRATION_X, POS_REGISTRATION_Y + 6, tstring, 0, RGB565_RED, OPENSANS16, pAsBufferAddr,
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
    for (int i = 202; i < 240; i++)
    {
        for (int j = 16; j < 46; j++)
        {
            if (*pIcon <= 0xfc00)
                *(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
            else
                pIcon++;
        }
    }
}

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
    int reg_rect_color = RGB565_RED;
    if(Oasis_GetState()== OASIS_STATE_FACE_REC)
        reg_rect_color = RGB565_BLUE;

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

static void UIInfo_UpdateNameConf(uint16_t* pAsBufferAddr, QUIInfoMsg infoMsg, int x1, int x2, int y1, int y2, std::string name, int similar, int dim, uint8_t p_isLiveView)
{
    //face name and confidence
    char tstring[64];

    if (p_isLiveView)
    {
        if(!name.empty()&&(Oasis_GetState()==OASIS_STATE_FACE_REC)){
            sprintf(tstring, "%s:lock opened", name.c_str());
            put_string(50, 10, tstring, 0, RGB565_GREEN,
                               OPENSANS16, pAsBufferAddr, APP_AS_WIDTH);

            uint16_t *pIcon = (uint16_t *)welcomehome_320x122;

            for (int i = 60; i < 182; i++)
            {
                for (int j = 0; j < 320; j++)
                {
                    *(pAsBufferAddr + i*APP_AS_WIDTH + j) = *pIcon++;
                }
            }
        }
        if ((Oasis_GetState()==OASIS_STATE_FACE_ADD)&& !name.empty())
        {
            if(infoMsg.newface ) {
                sprintf(tstring, "%s Added", name.c_str());
                put_string(100, 100, tstring, 0x0, RGB565_GREEN, OPENSANS16,
                        pAsBufferAddr, APP_AS_WIDTH);
            }
            else
            {
                sprintf(tstring, "%s had", name.c_str());
                put_string(100, 100, tstring, 0x0, RGB565_GREEN, OPENSANS16,
                        pAsBufferAddr, APP_AS_WIDTH);
                sprintf(tstring, "registered", name.c_str());
                put_string(100, 130, tstring, 0x0, RGB565_GREEN, OPENSANS16,
                        pAsBufferAddr, APP_AS_WIDTH);
            }
        }
        if((Oasis_GetState()==OASIS_STATE_FACE_DEL) && !name.empty())
        {
            sprintf(tstring, "%s removed", name.c_str());
            put_string(70, 100, tstring, 0x0, RGB565_GREEN, OPENSANS16,
                    pAsBufferAddr, APP_AS_WIDTH);
        }

    }
    else
    {


        if (infoMsg.newface)
        {
            sprintf(tstring, "%s ADDED", name.c_str());
            put_string(100, 100, tstring, RGB565_GREEN, -1,
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

void UIInfo_Update(uint16_t* pAsBufferAddr, QUIInfoMsg infoMsg, bool erase, uint8_t p_isLiveView)
{
    int scale_factor = APP_CAMERA_WIDTH / APP_PS_WIDTH;
    int shift_x = (APP_CAMERA_WIDTH - REC_RECT_WIDTH) / 2;
    int shift_y = (APP_CAMERA_HEIGHT - REC_RECT_HEIGHT) / 2;

    int x1                           = CAMERA_SURFACE_SHIFT + (infoMsg.rect[0] + shift_x) / scale_factor;
    int y1                           = (infoMsg.rect[1] + shift_y) / scale_factor;
    int x2                           = CAMERA_SURFACE_SHIFT + (infoMsg.rect[2] + shift_x) / scale_factor;
    int y2                           = (infoMsg.rect[3] + shift_y) / scale_factor;
    std::string name                 = infoMsg.name;
    int similar                      = (int)(infoMsg.similar * 10000 / 100);
    static system_states_t sys_state = sysStateDetectedNoUser;

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

#if  1//!RTFFI_USB_VIDEO
    UIInfo_UpdateBottomInfoBar(pAsBufferAddr, infoMsg, p_isLiveView);
#else
    // show the lock/unlock icon
    uint16_t *pIcon = (uint16_t *)lock;

    if (!name.empty())
    {
        pIcon = (uint16_t *)unlock;
    }

    UIInfo_UpdateLeftInfoBar(pAsBufferAddr, pIcon, infoMsg, p_isLiveView);
#endif
    // face detected only if it fits in the PS space from the screen
    if ((x1 >= CAMERA_SURFACE_SHIFT) && (y1 >= 0) && (x2 >= CAMERA_SURFACE_SHIFT) && (y2 > 0) && (x1 != x2) &&
        (y1 != y2) && (x2 < APP_AS_WIDTH) && (y2 < APP_AS_HEIGHT))
    {
        SysState_Set(sys_state);
        //UIInfo_UpdateFaceBox(pAsBufferAddr, x1, x2, y1, y2, LINE_DIMS, p_isLiveView);

        UIInfo_UpdateEmotion(pAsBufferAddr, infoMsg, x1, x2, y1, y2, LINE_DIMS, p_isLiveView);

        if(Cfg_AppDataGetVerbosity() == VERBOSE_MODE_L3)
		    UIInfo_UpdateQualityInfo(pAsBufferAddr,infoMsg);

        if (!name.empty()) {
            //face recognize
            UIInfo_UpdateNameConf(pAsBufferAddr, infoMsg, x1, x2, y1, y2, name, similar, LINE_DIMS, p_isLiveView);
        }
        else
        {
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
    }
    else
    {
        // face not detected
        SysState_Set(sysStateDetectedNoUser);
        name = "";
    }

    if(name.empty())
        UIInfo_DrawFocusRect(pAsBufferAddr,110, 210, 70,170);
}

#endif
