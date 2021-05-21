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

#include "commondef.h"

#include "semphr.h"
#include "event_groups.h"
#include "display.h"
#include "camera.h"
#include "pxp.h"
#include "fsl_log.h"
#include "sln_shell.h"
#include "fsl_pcal.h"
#include "MCU_UART5.h"
// 20201114 wavezgx modified for UI development
// #include "nxp_vertical_logo.h"
#include "aw_wave_logo_v3.h"
#if !BOARD_SUPPORT_PARALLEL_LCD
#include "fsl_stdisp.h"
#else
#include "fsl_elcdif.h"
#include "fsl_pjdisp.h"
#define APP_POL_FLAGS (kELCDIF_DataEnableActiveHigh | kELCDIF_VsyncActiveLow | kELCDIF_HsyncActiveLow | kELCDIF_DriveDataOnRisingClkEdge)
#define APP_LCDIF_DATA_BUS kELCDIF_DataBus16Bit
#endif

static QueueHandle_t DisplayMsgQ = NULL;
#define DISPLAY_MSG_Q_COUNT 8
static QMsg gDisplayReqMsg;

#if RTFFI_USB_VIDEO
SDK_ALIGN(static uint16_t s_USBBuffer[1][LCD_HEIGHT][LCD_WIDTH], FRAME_BUFFER_ALIGN);
extern uint8_t *g_FrameBufferUSB;
#endif

SDK_ALIGN(static uint16_t s_BufferLcd[2][LCD_WIDTH * LCD_HEIGHT], FRAME_BUFFER_ALIGN);

extern EventGroupHandle_t g_syncVideoEvents;

static uint8_t gActiveFrameIndex = 0;

int Display_SendQMsg(void* msg)
{
    BaseType_t ret;
    ret = xQueueSend(DisplayMsgQ, msg, (TickType_t)0);

    if (ret != pdPASS) {
        LOGE("[ERROR]:Display_SendQMsg failed\r\n");
        return -1;
    }

    return 0;
}

static void Display_Task(void* param)
{
    BaseType_t ret;
    QMsg* pQMsg;
#ifdef SHOW_FPS
    bool first = true;
    uint32_t timeStart, timeStop, count = 0;
    float fps = 0.0f;
#endif
    xEventGroupWaitBits(g_syncVideoEvents, 1 << SYNC_VIDEO_DISPLAY_INIT_BIT | 1 << SYNC_VIDEO_CAMERA_INIT_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    LOGD("[Display]:running\r\n");
    pQMsg = &gDisplayReqMsg;
    Camera_SendQMsg((void*)&pQMsg);
    while (1) {
        // pick up message
        ret = xQueueReceive(DisplayMsgQ, (void*)&pQMsg, portMAX_DELAY);

        if (ret == pdTRUE) {
            switch (pQMsg->id) {
            case QMSG_DISPLAY_FRAME_RES: {
#if SHOW_FPS
                count ++;
                if (first)
                {
                    timeStart = Time_Now();
                    first = false;
                }
                else
                {
                    timeStop = Time_Now();
                    uint32_t diff = timeStart - timeStop;

                    if (diff > 1000000 / PIT_TIMER_UNIT)
                    {
                        fps = count * 1000.0f / diff;
                        UsbShell_Printf("[Display]:fps = %d\r\n",(int)fps);
                        first = true;
                        count = 0;
                    }
                }
#endif
                Display_Update((uint32_t )s_BufferLcd[gActiveFrameIndex], 1);
                gActiveFrameIndex ^= 1U;

                pQMsg = &gDisplayReqMsg;
                gDisplayReqMsg.msg.raw.data = (void*)s_BufferLcd[gActiveFrameIndex];
                Camera_SendQMsg((void*)&pQMsg);
            }
            break;

            case QMSG_DISPLAY_INTERFACE: {
                //s_DisplayInterfaceMode = *(uint8_t*)pQMsg->msg.raw.data;
            }
            break;

            default:
                break;
            }
        }
    }
}

int Display_Start()
{
    LOGD("[Display]:start\r\n");

    gDisplayReqMsg.id = QMSG_DISPLAY_FRAME_REQ;
    gDisplayReqMsg.msg.raw.data = (void*)s_BufferLcd[gActiveFrameIndex];

    DisplayMsgQ = xQueueCreate(DISPLAY_MSG_Q_COUNT, sizeof(QMsg*));

    if (DisplayMsgQ == NULL) {
        LOGE("[ERROR]:xQueueCreate display queue\r\n");
        return -1;
    }

    if (xTaskCreate(Display_Init_Task, "Display_Init", DISPLAYINITTASK_STACKSIZE,
                    NULL, DISPLAYINITTASK_PRIORITY, NULL) != pdPASS)
    {
        LOGE("[ERROR]Display Init created failed\r\n");

        while (1);
    }

    if (xTaskCreate(Display_Task, "Display Task", DISPLAYTASK_STACKSIZE,
                    NULL, DISPLAYTASK_PRIORITY, NULL) != pdPASS) {
        LOGE("[ERROR]DisplayTask created failed\r\n");

        while (1);
    }

    LOGD("[Camra]:started\r\n");
    return 0;
}

int Display_Sync()
{
    return 0;
}

#if BOARD_SUPPORT_PARALLEL_LCD
static void ELCDIF_Init(void)
{
    const elcdif_rgb_mode_config_t config = {
        .panelWidth    = 240,
        .panelHeight   = 320,
        /*.hsw           = 10,
        .hfp           = 10,
        .hbp           = 20,
        .vsw           = 2,
        .vfp           = 4,
        .vbp           = 2,*/
        .hsw           = 4,
        .hfp           = 20,   
        .hbp           = 16,
        .vsw           = 4,
        .vfp           = 16,
        .vbp           =12, 
        .polarityFlags = APP_POL_FLAGS,
        .bufferAddr    = (uint32_t)s_BufferLcd,
        .pixelFormat   = kELCDIF_PixelFormatRGB565,
        .dataBus       = APP_LCDIF_DATA_BUS,
    };
    ELCDIF_RgbModeInit(LCDIF, &config);
}

static void LcdifPixelClock_Init(void)
{
    /*
     * (240 + 10 + 20 + 10) * (320 + 2 + 2 +4) *60 = 5.51M
     * The desired output frame rate is 60Hz. So the pixel clock frequency is:
     * (480 + 41 + 4 + 18) * (272 + 10 + 4 + 2) * 60 = 9.2M.
     * Here set the LCDIF pixel clock to 9.3M.
     */

    /*
     * Initialize the Video PLL.
     * Video PLL output clock is OSC24M * (loopDivider + (denominator / numerator)) / postDivider = 93MHz.
     */
    clock_video_pll_config_t config = {
        .loopDivider = 20,
        .postDivider = 8,
        .numerator   = 0,
        .denominator = 0,
        .src = 0
    };

    CLOCK_InitVideoPll(&config);

    /*
     * 000 derive clock from PLL2
     * 001 derive clock from PLL3 PFD3
     * 010 derive clock from PLL5
     * 011 derive clock from PLL2 PFD0
     * 100 derive clock from PLL2 PFD1
     * 101 derive clock from PLL3 PFD1
     */
    CLOCK_SetMux(kCLOCK_LcdifPreMux, 2);

    CLOCK_SetDiv(kCLOCK_LcdifPreDiv, 4);

    CLOCK_SetDiv(kCLOCK_LcdifDiv, 1);
}

void BOARD_LCD_Init(void)
{
	CLOCK_EnableClock(kCLOCK_Gpio1);
	CLOCK_EnableClock(kCLOCK_Gpio2);
	CLOCK_EnableClock(kCLOCK_Gpio3);
	PJDisp_Init();
	LcdifPixelClock_Init();
	ELCDIF_Init();
}
#endif

void Display_Init_Task(void* param)
{
#if !RTFFI_USB_VIDEO
#if BOARD_SUPPORT_PARALLEL_LCD
    BOARD_LCD_Init();
    ELCDIF_RgbModeStart(LCDIF);
#else
    STDisp_Init(NULL);
#endif
#endif
	// 20201114 wavezgx modified
    // Display_Update((uint32_t)nxp_vertical_logo, 1);
    Display_Update((uint32_t)wave_logo_v3, 1);

#if !RTFFI_USB_VIDEO && BOARD_SUPPORT_PARALLEL_LCD
    PJDisp_TurnOnBacklight();
#endif
    vTaskDelay(500);//logo display time

    xEventGroupSetBits(g_syncVideoEvents, 1 << SYNC_VIDEO_DISPLAY_INIT_BIT);
    memset((uint8_t*)s_BufferLcd[0], 0x00, LCD_WIDTH * LCD_HEIGHT*2);
    Display_Update((uint32_t)s_BufferLcd[0], 1);

    vTaskDelete (NULL);
}

int Display_Update(uint32_t backBuffer, uint8_t p_isLiveView )
{
#if RTFFI_USB_VIDEO
    ConvertRGB2YUV((uint16_t*)backBuffer, (uint16_t*)s_USBBuffer[0]);
    g_FrameBufferUSB = (uint8_t *)s_USBBuffer[0];
#else
#if BOARD_SUPPORT_PARALLEL_LCD
    PJDisp_SendFrame(backBuffer);
#else
    STDisp_SendFrame((uint16_t *)backBuffer);
#endif
#endif

    return 0;
}

void BOARD_TurnOnLcd()
{
}

void BOARD_TurnOffLcd()
{
}

#endif
