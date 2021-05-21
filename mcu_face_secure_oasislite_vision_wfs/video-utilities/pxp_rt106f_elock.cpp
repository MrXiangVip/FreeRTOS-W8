/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the license terms that accompany it. By expressly accepting
 * such terms or by downloading, installing, activating and/or otherwise using
 * the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 *
 * Created by: NXP China Solution Team.
 */
 
#if RT106F_ELOCK_BOARD && !CAMERA_SUPPORT_3D

#include "FreeRTOS.h"
#include "event_groups.h"
#include "fsl_pxp.h"
#include "fsl_log.h"
#include "commondef.h"
#include "display.h"
#include "oasis.h"
#include "pxp.h"
#include "util.h"
#include "userid_ui.h"
#include "camera.h"
#include "display.h"


#define CLIP(X) ((X) > 255 ? 255 : (X) < 0 ? 0 : X)

// RGB -> YUV
#define RGB2Y(R, G, B) CLIP(((66 * (R) + 129 * (G) + 25 * (B) + 128) >> 8) + 16)
#define RGB2U(R, G, B) CLIP(((-38 * (R)-74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(((112 * (R)-94 * (G)-18 * (B) + 128) >> 8) + 128)

static pxp_output_buffer_config_t outputBufferConfig;
static pxp_ps_buffer_config_t psBufferConfig;
static pxp_as_buffer_config_t asBufferConfig;
static pxp_as_blend_config_t asBlendConfig;
static uint16_t s_asBufferPxp[APP_AS_HEIGHT*APP_AS_WIDTH];
extern uint16_t* rotateBuff;

extern EventGroupHandle_t g_syncVideoEvents;

extern "C"{
    void PXP_IRQHandler(void)
    {
        if(g_syncVideoEvents != NULL)
        {
            BaseType_t result;
            BaseType_t HigherPriorityTaskWoken = pdFALSE;
            PXP_ClearStatusFlags(APP_PXP, kPXP_CompleteFlag);
            result = xEventGroupSetBitsFromISR(g_syncVideoEvents, 1 << SYNC_VIDEO_PXP_COMPLET_BIT, &HigherPriorityTaskWoken);
            if( result != pdFAIL )
            {
                portYIELD_FROM_ISR(HigherPriorityTaskWoken);
            }
        }
    }
}

void RGB5652YUV(uint16_t source1, uint16_t source2, uint16_t *dest1, uint16_t *dest2)
{
    uint8_t R, G, B, Y0, Y1, U0, V0, U1, V1;
    R      = ((source1 & RGB565_RED) >> 11) << 3;
    G      = ((source1 & RGB565_GREEN) >> 5) << 2;
    B      = ((source1 & RGB565_BLUE)) << 3;
    Y0     = RGB2Y(R, G, B);
    U0     = RGB2U(R, G, B);
    V0     = RGB2V(R, G, B);
    R      = ((source2 & RGB565_RED) >> 11) << 3;
    G      = ((source2 & RGB565_GREEN) >> 5) << 2;
    B      = ((source2 & RGB565_BLUE)) << 3;
    Y1     = RGB2Y(R, G, B);
    U1     = RGB2U(R, G, B);
    V1     = RGB2V(R, G, B);
    U0     = (U0 + U1) / 2;
    V0     = (V0 + V1) / 2;
    *dest2 = (V0 << 8) | Y1;
    *dest1 = (U0 << 8) | Y0;
}

void ConvertRGB2YUV(uint16_t *sourceLcdBuffer, uint16_t *destLcdBuffer)
{
    int line, col;

    for (line = 0; line < LCD_HEIGHT; line++)
        for (col = 0; col < LCD_WIDTH - 1; col = col + 2)
        {
            RGB5652YUV(*(sourceLcdBuffer + line * LCD_WIDTH + col), *(sourceLcdBuffer + line * LCD_WIDTH + col + 1),
                       (destLcdBuffer + line * LCD_WIDTH + col), (destLcdBuffer + line * LCD_WIDTH + col + 1));
        }
}

void APP_PXPInit()
{
    PXP_Init(APP_PXP);
    PXP_EnableInterrupts(APP_PXP, kPXP_CompleteInterruptEnable);
    EnableIRQ(PXP_IRQn);
    NVIC_SetPriority(PXP_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);

    /* PS configure. */
    psBufferConfig.pixelFormat = kPXP_PsPixelFormatUYVY1P422;//kPXP_PsPixelFormatRGB565;//kPXP_PsPixelFormatRGB888;//kPXP_PsPixelFormatRGB565;
    psBufferConfig.swapByte = false;
    //psBufferConfig.bufferAddr = (uint32_t)s_psBufferPxp;
    psBufferConfig.bufferAddrU = 0U;
    psBufferConfig.bufferAddrV = 0U;
    psBufferConfig.pitchBytes  = APP_CAMERA_WIDTH*2;//camera data as PS input data

    PXP_SetProcessSurfaceBackGroundColor(APP_PXP, 0x1U);
    PXP_SetProcessSurfaceBufferConfig(APP_PXP, &psBufferConfig);

    /* AS config. */
    asBufferConfig.pixelFormat = kPXP_AsPixelFormatRGB565;
    asBufferConfig.bufferAddr = (uint32_t)s_asBufferPxp;
    asBufferConfig.pitchBytes = APP_AS_WIDTH * 2;
    asBlendConfig.alpha       = 0xb0U; /* Don't care. */
    asBlendConfig.invertAlpha = false; /* Don't care. */
    asBlendConfig.alphaMode   = kPXP_AlphaOverride;
    asBlendConfig.ropMode     = kPXP_RopMaskAs;

    PXP_SetAlphaSurfaceBufferConfig(APP_PXP, &asBufferConfig);
    PXP_SetAlphaSurfaceBlendConfig(APP_PXP, &asBlendConfig);

    /* Output config. */
    outputBufferConfig.pixelFormat =
        kPXP_OutputPixelFormatRGB565; // kPXP_OutputPixelFormatRGB888;//kPXP_OutputPixelFormatRGB565;
    outputBufferConfig.interlacedMode = kPXP_OutputProgressive;
    outputBufferConfig.buffer1Addr    = 0U;
    outputBufferConfig.pitchBytes     = LCD_WIDTH * 2;
    outputBufferConfig.width          = LCD_WIDTH;
    outputBufferConfig.height         = LCD_HEIGHT;

    PXP_SetOutputBufferConfig(APP_PXP, &outputBufferConfig);

    /* Disable CSC1, it is enabled by default. */
    PXP_SetCsc1Mode(APP_PXP, kPXP_Csc1YCbCr2RGB);
    PXP_EnableCsc1(APP_PXP, true);
}


static float getFPS()
{
    static float fps = 0.0f;
#ifdef SHOW_FPS
    static int frameCount = 0;
    static int firstFrame = 1;
    static int tStart, tEnd;
    if (firstFrame)
    {
        firstFrame = 0;
        frameCount = 0;
        tStart = tEnd = Time_Current();
    }
    else
    {
        frameCount++;
        tEnd = Time_Current();
        if (tEnd - tStart >= 1000)
        {
            fps        = (frameCount * 1000.0f) / (float)(tEnd - tStart);
            frameCount = 0;
            tStart     = tEnd;
        }
        // LOGD("[fps]:%.3f\r\n", fps);
    }
#endif
    return fps;
}

static void APP_PXPAlpha(uint32_t enable, int l, int t, int r, int b, uint8_t alpha, uint32_t enableColorKey)
{
    if (enable)
    {
        PXP_SetAlphaSurfacePosition(APP_PXP, l, t, r, b);
        asBlendConfig.alpha       = alpha;
        PXP_SetAlphaSurfaceBlendConfig(APP_PXP, &asBlendConfig);
        if(enableColorKey)
        {
            PXP_SetAlphaSurfaceOverlayColorKey(APP_PXP, 0x0, 0x0);
            PXP_EnableAlphaSurfaceOverlayColorKey(APP_PXP, true);
        }
        else
        {
            PXP_EnableAlphaSurfaceOverlayColorKey(APP_PXP, false);
        }
    }
    else
    {
        PXP_SetAlphaSurfacePosition(APP_PXP, 0xFFFFU, 0xFFFFU, 0U, 0U);
    }
    PXP_SetAlphaSurfaceBufferConfig(APP_PXP, &asBufferConfig);
}

static void APP_PXPOut(int w, int h, int l, int t, int r, int b, int iw, int ih, int ow, int oh, pxp_output_pixel_format_t format, uint16_t pitchBytes )
{
    outputBufferConfig.pitchBytes = pitchBytes;
    outputBufferConfig.pixelFormat = format;
    outputBufferConfig.width = w;
    outputBufferConfig.height = h;
    PXP_SetOutputBufferConfig(APP_PXP, &outputBufferConfig);
    PXP_SetProcessSurfacePosition(APP_PXP, l, t, r, b);
    PXP_SetProcessSurfaceScaler(APP_PXP, iw, ih, ow, oh);
}

static void APP_PXPStart(uint32_t psBuffer, uint32_t outBuffer, uint32_t rotate)
{
    /* Convert the camera input picture to RGB format. */
    if (psBuffer)
    {
        psBufferConfig.bufferAddr = psBuffer;
        PXP_SetProcessSurfaceBufferConfig(APP_PXP, &psBufferConfig);
    }

    if (outBuffer)
    {
        outputBufferConfig.buffer0Addr = outBuffer;
        if(rotate == 90)
        {
            PXP_SetRotateConfig(APP_PXP, kPXP_RotateOutputBuffer, kPXP_Rotate90, kPXP_FlipBoth);
        }
        else
        {
            PXP_SetRotateConfig(APP_PXP, kPXP_RotateOutputBuffer, kPXP_Rotate180, kPXP_FlipBoth);
        }
        PXP_SetOutputBufferConfig(APP_PXP, &outputBufferConfig);
    }

    PXP_Start(APP_PXP);

    /* Wait for PXP process complete. */
    xEventGroupWaitBits(g_syncVideoEvents, 1 << SYNC_VIDEO_PXP_COMPLET_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
}

void APP_SetPxpRotate(uint32_t psBuffer, uint32_t rotateBuf)
{
    //PXP_Init(APP_PXP);
    /* PS configure. */
    //PXP_Init(APP_PXP);
    //PXP_EnableInterrupts(APP_PXP, kPXP_CompleteInterruptEnable);

    psBufferConfig.pixelFormat = kPXP_PsPixelFormatUYVY1P422;
    psBufferConfig.swapByte = false;
    psBufferConfig.bufferAddr  = psBuffer,
    psBufferConfig.bufferAddrU = 0U;
    psBufferConfig.bufferAddrV = 0U;
    psBufferConfig.pitchBytes  = APP_CAMERA_WIDTH*2,

    PXP_SetProcessSurfaceBackGroundColor(APP_PXP, 0U);

    PXP_SetProcessSurfaceBufferConfig(APP_PXP, &psBufferConfig);

    PXP_SetProcessSurfaceScaler(APP_PXP, APP_CAMERA_WIDTH, APP_CAMERA_HEIGHT, APP_CAMERA_WIDTH, APP_CAMERA_HEIGHT);
    PXP_SetProcessSurfacePosition(APP_PXP, 0, 0, APP_CAMERA_HEIGHT - 1, APP_CAMERA_WIDTH - 1);

    /* Disable AS. */
    PXP_SetAlphaSurfacePosition(APP_PXP, 0xFFFFU, 0xFFFFU, 0U, 0U);

    /* Output config. */  //    640*480   ------->  480*640
    outputBufferConfig.pixelFormat    = kPXP_OutputPixelFormatUYVY1P422;
    outputBufferConfig.interlacedMode = kPXP_OutputProgressive;
    outputBufferConfig.buffer0Addr    =  rotateBuf;
    outputBufferConfig.buffer1Addr    = 0U;
    outputBufferConfig.pitchBytes     = APP_CAMERA_HEIGHT*2;
    outputBufferConfig.width          = APP_CAMERA_HEIGHT;
    outputBufferConfig.height         = APP_CAMERA_WIDTH;
    PXP_SetOutputBufferConfig(APP_PXP, &outputBufferConfig);



    /* Disable CSC1, it is enabled by default. */
    PXP_EnableCsc1(APP_PXP, false);
    APP_PXP->CSC1_COEF0 |= (1<<31);

    PXP_SetRotateConfig(APP_PXP, kPXP_RotateProcessSurface, kPXP_Rotate90, kPXP_FlipDisable);
    PXP_Start(APP_PXP);
    /* Wait for PXP process complete. */
    xEventGroupWaitBits(g_syncVideoEvents, 1 << SYNC_VIDEO_PXP_COMPLET_BIT, pdTRUE, pdTRUE, portMAX_DELAY);

}

void APP_SetPxpInCommomMode(uint32_t psBuffer, uint32_t outputBuf)
{
    /* PS configure. */
    psBufferConfig.pixelFormat = kPXP_PsPixelFormatUYVY1P422;
    psBufferConfig.swapByte = false;
    psBufferConfig.bufferAddr = psBuffer;
    psBufferConfig.bufferAddrU = 0U;
    psBufferConfig.bufferAddrV = 0U;
    psBufferConfig.pitchBytes  = APP_CAMERA_HEIGHT*2;//camera rotated data as PS input data

    PXP_SetProcessSurfaceBackGroundColor(APP_PXP, 0x1U);
    PXP_SetProcessSurfaceBufferConfig(APP_PXP, &psBufferConfig);

    /* AS config. */
    asBufferConfig.pixelFormat = kPXP_AsPixelFormatRGB565;
    asBufferConfig.bufferAddr = (uint32_t)s_asBufferPxp;
    asBufferConfig.pitchBytes = APP_AS_WIDTH * 2;
    asBlendConfig.alpha       = 0xb0U; /* Don't care. */
    asBlendConfig.invertAlpha = false; /* Don't care. */
    asBlendConfig.alphaMode   = kPXP_AlphaOverride;
    asBlendConfig.ropMode     = kPXP_RopMaskAs;

    PXP_SetAlphaSurfaceBufferConfig(APP_PXP, &asBufferConfig);
    PXP_SetAlphaSurfaceBlendConfig(APP_PXP, &asBlendConfig);

    /* Output config. */
    outputBufferConfig.pixelFormat =  kPXP_OutputPixelFormatRGB565; // kPXP_OutputPixelFormatRGB888;//kPXP_OutputPixelFormatRGB565;
    outputBufferConfig.interlacedMode = kPXP_OutputProgressive;
    outputBufferConfig.buffer0Addr    =  outputBuf;
    outputBufferConfig.buffer1Addr    = 0U;
    outputBufferConfig.pitchBytes     = LCD_WIDTH * 2;
    outputBufferConfig.width          = LCD_WIDTH;
    outputBufferConfig.height         = LCD_HEIGHT;

    PXP_SetOutputBufferConfig(APP_PXP, &outputBufferConfig);
    PXP_SetCsc1Mode(APP_PXP, kPXP_Csc1YCbCr2RGB);
    PXP_EnableCsc1(APP_PXP, true);
}

void APP_PXPStartCamera2Display(uint32_t cameraBuffer, QUIInfoMsg infoMsg, uint8_t p_isLiveView, uint32_t outBuffer)
{
    infoMsg.fps = getFPS();
#if CAMERA_ROTATE_FLAG
    APP_SetPxpInCommomMode(cameraBuffer, outBuffer);
#endif

    UIInfo_Update(s_asBufferPxp, infoMsg, false, p_isLiveView);
    //updateASSurface(infoMsg, LINE_DIMS, false, p_isLiveView);
#if RTFFI_USB_VIDEO
    APP_PXPAlpha(1,0, 0, LCD_WIDTH-1, LCD_HEIGHT-1, 0xf0U, 1);
#else
	// 20201114 wavezgx modified: fix alpha 0xb0U to 0xffU for no transparency
    if(p_isLiveView)
        APP_PXPAlpha(1,0, 0, LCD_WIDTH-1, LCD_HEIGHT-1, 0xffU, 1);
    else
        APP_PXPAlpha(1,0, 0, LCD_WIDTH-1, LCD_HEIGHT-1, 0xffU, 0);
#endif
    APP_PXPOut(LCD_WIDTH, LCD_HEIGHT,
                0, 0,
                LCD_WIDTH - 1, LCD_HEIGHT - 1,
   #if CAMERA_ROTATE_FLAG
                APP_CAMERA_HEIGHT, APP_CAMERA_WIDTH, APP_PS_WIDTH, APP_PS_HEIGHT,
   #else
                APP_CAMERA_WIDTH, APP_CAMERA_HEIGHT, APP_PS_WIDTH, APP_PS_HEIGHT,
   #endif
                kPXP_OutputPixelFormatRGB565, LCD_WIDTH * 2);
    APP_PXPStart(cameraBuffer, outBuffer, 0);
}

void APP_PXPStartCamera2DetBuf(uint32_t cameraBuffer, uint32_t detBuffer)
{
#if CAMERA_ROTATE_FLAG
    APP_SetPxpInCommomMode(cameraBuffer, detBuffer);
#endif

    APP_PXPAlpha(0, 0, 0, 0, 0, 0, 0);
    APP_PXPOut(REC_RECT_WIDTH, REC_RECT_HEIGHT,
               0, 0, REC_RECT_WIDTH - 1, REC_RECT_HEIGHT - 1,
#if CAMERA_ROTATE_FLAG
                APP_CAMERA_HEIGHT, APP_CAMERA_WIDTH, REC_RECT_WIDTH, REC_RECT_HEIGHT,
#else
                APP_CAMERA_WIDTH, APP_CAMERA_HEIGHT, REC_RECT_WIDTH, REC_RECT_HEIGHT,
#endif
                kPXP_OutputPixelFormatRGB888P, REC_RECT_WIDTH * 3);
    APP_PXPStart(cameraBuffer, detBuffer, 0);
}


static QueueHandle_t PXPMsgQ = NULL;
/*!< Message sent to Oasis Task by Camera task to signal that a frame is available */
static QMsg FResMsg;
/*!< Message sent to Display Task by Camera task to signal that a frame is available */
static QMsg DResMsg;
//static uint8_t s_DisplayInterfaceMode = DISPLAY_LAST_INTERFACE;

static void PXP_Task(void* param)
{
    BaseType_t ret;
    QMsg* pQMsg;

    while (1)
    {

        // pick up message
        ret = xQueueReceive(PXPMsgQ, (void*)&pQMsg, portMAX_DELAY);

        if (ret == pdTRUE)
        {
            switch (pQMsg->id)
            {
            case QMSG_PXP_FACEREC:{
            #if CAMERA_ROTATE_FLAG
                 APP_SetPxpRotate(pQMsg->msg.pxp.in_buffer, (uint32_t)rotateBuff);
                 APP_PXPStartCamera2DetBuf((uint32_t)rotateBuff, pQMsg->msg.pxp.out_buffer);
            #else
                 APP_PXPStartCamera2DetBuf(pQMsg->msg.pxp.in_buffer, pQMsg->msg.pxp.out_buffer);
            #endif
                pQMsg = &FResMsg;
                Camera_SendQMsg((void*)&pQMsg);
            }
            break;
            case QMSG_PXP_DISPLAY:{
            #if CAMERA_ROTATE_FLAG
                APP_SetPxpRotate(pQMsg->msg.pxp.in_buffer, (uint32_t)rotateBuff);
                APP_PXPStartCamera2Display((uint32_t)rotateBuff, *pQMsg->msg.pxp.info, 1, pQMsg->msg.pxp.out_buffer);
            #else
                APP_PXPStartCamera2Display(pQMsg->msg.pxp.in_buffer, *pQMsg->msg.pxp.info, 1, pQMsg->msg.pxp.out_buffer);
            #endif
                pQMsg = &DResMsg;
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

void APP_PXP_Start(void)
{
    LOGD("[PXP]:start\r\n");
    APP_PXPInit();
    FResMsg.id   = QMSG_PXP_FACEREC;
    DResMsg.id  = QMSG_PXP_DISPLAY;

    PXPMsgQ = xQueueCreate(6, sizeof(QMsg*));

    if (PXPMsgQ == NULL)
    {
        LOGE("[ERROR]:xQueueCreate pxp queue\r\n");
        while (1);
    }

    if (xTaskCreate(PXP_Task, "PXP Task", PXPTASK_STACKSIZE,
                    NULL, PXPTASK_PRIORITY, NULL) != pdPASS)
    {
        LOGE("[ERROR]PXP_Task created failed\r\n");

        while (1);
    }
}

int PXP_SendQMsg(void* msg)
{
    BaseType_t ret;

    ret = xQueueSend(PXPMsgQ, msg, (TickType_t)0);

    if (ret != pdPASS)
    {
        //LOGE("[ERROR]:PXP_SendQMsg failed %d\r\n", ret);
        return -1;
    }

    return 0;
}

#endif
