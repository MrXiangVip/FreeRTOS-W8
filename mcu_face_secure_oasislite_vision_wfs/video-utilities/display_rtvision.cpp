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

#if RTVISION_BOARD

#include "commondef.h"

#include "semphr.h"
#include "event_groups.h"
#include "display.h"
#include "camera.h"
#include "pxp.h"
#include "fsl_log.h"

#include "fsl_pcal.h"
#if RTFFI_JP_DISP_DRIVER
#include "fsl_jdisp.h"
#endif
#if RTFFI_RV_DISP_DRIVER
#include "fsl_rvdisp.h"
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

void Display_Init_Task(void* param)
{
#if RTFFI_JP_DISP_DRIVER
    memset(s_LcdBuffer, 0, sizeof(s_LcdBuffer));
    JDisp_Init(&s_LcdBuffer[0][0][0]);
#endif
#if RTFFI_RV_DISP_DRIVER
    RVDisp_Init(NULL);
#endif
    xEventGroupSetBits(g_syncVideoEvents, 1 << SYNC_VIDEO_DISPLAY_INIT_BIT);

    vTaskDelete (NULL);
}

int Display_Update(uint32_t backBuffer, uint8_t p_isLiveView )
{

#if 0
    uint16_t* pBuffer = (uint16_t*)backBuffer;

    for (int i = 0; i < JDISP_HEIGHT; i++) 
	{
        for (int j = 0; j < JDISP_WIDTH; j++) 
		{
            //s_lcdBuffer[0][i][j] = RGB565_RED;  // XRGB888    low bytes is Blue
            //s_lcdBuffer[1][i][j] = RGB565_GREEN;
            *pBuffer++ = 0x07e0;
        }
    }

#endif
#if RTFFI_JP_DISP_DRIVER
    JDisp_SendFrame((uint16_t *)backBuffer);
#endif
#if RTFFI_RV_DISP_DRIVER
    RVDisp_SendFrame((uint16_t *)backBuffer);
#endif
#if RTFFI_USB_VIDEO
    ConvertRGB2YUV((uint16_t*)backBuffer, (uint16_t*)s_USBBuffer[0]);
    g_FrameBufferUSB = (uint8_t *)s_USBBuffer[0];
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
