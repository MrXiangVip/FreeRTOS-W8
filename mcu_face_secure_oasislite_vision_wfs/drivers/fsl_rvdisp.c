/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#if RTVISION_BOARD

#include "platform_disp.h"
#include "App_Common.h"
#include "fsl_log.h"
#include "fsl_rvdisp.h"

#if RTFFI_RV_DISP_DRIVER
/* Global used for buffer optimization */
Gpu_Hal_Context_t host, *phost;

static void DisplayBitmap();

static float lerp(float t, float a, float b)
{
    return (float)((1 - t) * a + t * b);
}

static float smoothlerp(float t, float a, float b)
{
    float lt = 3 * t * t - 2 * t * t * t;
    return lerp(lt, a, b);
}

extern uint16_t *g_backBuffer;

static void DisplayBitmap()
{
    int16_t xball = (DispWidth / 2), yball = 120, rball = (DispWidth / 8);
    int16_t numpoints = 6, numlines = 8, i, asize, aradius, gridsize = 20;
    int32_t asmooth, dispr = (DispWidth - 10), displ = 10, dispa = 10, dispb = (DispHeight - 10), xflag = 1, yflag = 1;

    dispr -= ((dispr - displ) % gridsize);
    dispb -= ((dispb - dispa) % gridsize);

    /* endless loop */
    while (1)
    {
        App_WrDl_Buffer(phost, CLEAR_COLOR_RGB(128, 128, 0));
        App_WrDl_Buffer(phost, CLEAR(1, 1, 1));
        App_WrDl_Buffer(phost, STENCIL_OP(INCR, INCR));
        App_WrDl_Buffer(phost, COLOR_RGB(0, 0, 0));
#if 0
      /* draw grid */
      App_WrDl_Buffer(phost, LINE_WIDTH(16));
      App_WrDl_Buffer(phost, BEGIN(LINES));

      for(i=0;i<=((dispr - displ)/gridsize);i++)
        {
          App_WrDl_Buffer(phost, VERTEX2F((displ + i*gridsize)*16,dispa*16));
          App_WrDl_Buffer(phost, VERTEX2F((displ + i*gridsize)*16,dispb*16));
        }

      for(i=0;i<=((dispb - dispa)/gridsize);i++)
        {
          App_WrDl_Buffer(phost, VERTEX2F(displ*16,(dispa + i*gridsize)*16));
          App_WrDl_Buffer(phost, VERTEX2F(dispr*16,(dispa + i*gridsize)*16));
        }

      App_WrDl_Buffer(phost, END());
#endif
        App_WrDl_Buffer(phost, COLOR_RGB(255, 255, 255));
#if 0
      App_WrDl_Buffer(phost, BEGIN(BITMAPS)); // start drawing bitmaps
      App_WrDl_Buffer(phost, VERTEX2II(100, 110, 31, 'N'));
      App_WrDl_Buffer(phost, VERTEX2II(126, 110, 31, 'X'));
      App_WrDl_Buffer(phost, VERTEX2II(150, 110, 31, 'P'));
      App_WrDl_Buffer(phost, END());
#endif
        //      App_WrDl_Buffer(phost, BITMAP_SOURCE(128 * 16 + 32));

        if (g_backBuffer)
            Gpu_Hal_WrMem(phost, RAM_G, (uint8_t *)g_backBuffer, 400 * 240 * 2);
        App_WrDl_Buffer(phost, BITMAP_SOURCE(0));
        App_WrDl_Buffer(phost, BITMAP_LAYOUT(RGB565, 800, 240));
        App_WrDl_Buffer(phost, BITMAP_SIZE(NEAREST, BORDER, BORDER, 240, 240));
#if 0
      App_WrDl_Buffer(phost, BITMAP_TRANSFORM_A(0));
      App_WrDl_Buffer(phost, BITMAP_TRANSFORM_B(255*256));
      App_WrDl_Buffer(phost, BITMAP_TRANSFORM_D(256));
      App_WrDl_Buffer(phost, BITMAP_TRANSFORM_E(0));
#else
        App_WrDl_Buffer(phost, BITMAP_TRANSFORM_A(256));
        App_WrDl_Buffer(phost, BITMAP_TRANSFORM_B(0));
        App_WrDl_Buffer(phost, BITMAP_TRANSFORM_D(0));
        App_WrDl_Buffer(phost, BITMAP_TRANSFORM_E(256));

#endif
        App_WrDl_Buffer(phost, BEGIN(BITMAPS));
        App_WrDl_Buffer(phost, VERTEX2II(0, 0, 0, 0));
        App_WrDl_Buffer(phost, END());

        // Gpu_CoCmd_Text(phost,DispWidth/2,DispHeight/2,31,OPT_CENTERX|OPT_CENTERY,"NXP");

        App_WrDl_Buffer(phost, COLOR_MASK(1, 1, 1, 1)); // enable all the colors
        App_WrDl_Buffer(phost, STENCIL_FUNC(ALWAYS, 1, 255));
        App_WrDl_Buffer(phost, STENCIL_OP(KEEP, KEEP));
        App_WrDl_Buffer(phost, COLOR_RGB(255, 255, 255));
        App_WrDl_Buffer(phost, DISPLAY());

        /* Download the DL into DL RAM */
        App_Flush_DL_Buffer(phost);

        /* Do a swap */
        Gpu_Hal_DLSwap(phost, DLSWAP_FRAME);
    } /* while */
}

#define SW_ROTATE

void RVDisp_CopyFrameToDisplayBuffer(uint16_t *pFrameAddr, uint16_t *pDispBuffer)
{
    int i, j;
#ifdef SW_ROTATE
    for (i = 0; i < RVDISP_WIDTH; i++)
        for (j = RVDISP_HEIGHT - 1; j >= 0; j--)
            pDispBuffer[j * RVDISP_WIDTH + i] = pFrameAddr[i * 320 + RVDISP_HEIGHT - 1 - j];
#else
    memcpy(pDispBuffer, pFrameAddr, 400 * 240 * 2);
#endif
    // uint32_t t1 = Time_Now();
    // uint32_t t2 = Time_Now();
    // LOGD("[dither][%d]\r\n", t1 - t2);
}

#if 0
static const struct element {
  uint32_t src;
  uint16_t w, h, x0, y0;
} el[] = {
  { 0,      74, 145,  54,  48 },
  { 5365,  110, 218,  18,  11 },
  { 17355,  28,  29,   8,  66 },
  { 17761,  28,  29,   8, 145 },
  { 18167,  30,  28,  29,  45 },
  { 18587,  30,  29,  29, 167 },
  { 19022,  30,  28,  51,  23 },
  { 19442,  30,  28,  51, 189 },
  { 19862,  28,  28,  73,   1 },
  { 20254,  28,  28,  73, 211 },
  { 20646, 400, 149, 190,  47 },
  { 50446, 194, 120, 597,  47 },
  { 62086, 602,  29, 190, 183 }
};

static int32_t scaling;
static int16_t YOFFSET;

#define FX16(x) ((int32_t)((x)*65536.0f))

static int32_t tween(int32_t t, int32_t a, int32_t b)
{
  uint16_t tu, tu2, tu3;
  if (t < 0)
    return a;
  if (t > 65535)
    return b;
  tu = t;
  tu = ~tu;
  tu2 = ((uint32_t)tu * tu) >> 16;
  tu3 = ((uint32_t)tu2 * tu) >> 16;
  tu3 = ~tu3;
  return a + (((int32_t)tu3 * (b - a)) >> 16);
}

int32_t dc(int16_t c)
{
  return (scaling * c) >> 12;
}

static void setup_element(Gpu_Hal_Context_t *phost, const struct element *e)
{
  App_WrCoCmd_Buffer(phost, BITMAP_SOURCE(e->src));
  App_WrCoCmd_Buffer(phost, BITMAP_LAYOUT(L4, e->w / 2, e->h));
  App_WrCoCmd_Buffer(phost, BITMAP_SIZE(BILINEAR,BORDER,BORDER,(dc(e->w) >> 4) + 1,(dc(e->h) >> 4) + 1));
#ifdef FT81X_ENABLE
  App_WrCoCmd_Buffer(phost, BITMAP_LAYOUT_H((e->w / 2)>>10, 0));
  App_WrCoCmd_Buffer(phost, BITMAP_SIZE_H(((dc(e->w) >> 4) + 1)>>9, ((dc(e->h) >> 4) + 1)>>9));
#endif
}

#define FX16(x) ((int32_t)((x)*65536.0f))

static void draw(Gpu_Hal_Context_t *phost, const struct element *e, int32_t x)
{
  setup_element(phost, e);
  App_WrCoCmd_Buffer(phost, VERTEX2F(dc(e->x0) + x, YOFFSET + dc(e->y0)));
}

#endif

void RVDisp_SendFrame(uint16_t *pFrame)
{
    int line;
    static uint8_t execCoInit = 1;

    // uint32_t t1 = Time_Now();

    // Upload the framebuffer as a bitmap/texture
#ifdef SW_ROTATE
    Gpu_Hal_WrMem(phost, RAM_G, (uint8_t *)pFrame, 240 * 320 * 2);
#else
    Gpu_Hal_WrMem(phost, RAM_G, (uint8_t *)pFrame, 400 * 240 * 2);
#endif

    if (execCoInit)
    {
        execCoInit = 0;
#if 0
	// Generate a small display list to use the bitmap
	App_WrDl_Buffer(phost, CLEAR_COLOR_RGB(128, 128, 0) );
    App_WrDl_Buffer(phost, CLEAR(1, 1, 1));

    App_WrDl_Buffer(phost, BITMAP_SOURCE(0));
    App_WrDl_Buffer(phost, BITMAP_LAYOUT(RGB565, 240*2, 320));
    App_WrDl_Buffer(phost, BITMAP_SIZE(NEAREST, BORDER, BORDER, 240, 320));

#if 0
    App_WrDl_Buffer(phost, BITMAP_TRANSFORM_A(0));
    App_WrDl_Buffer(phost, BITMAP_TRANSFORM_B(255*256));
    App_WrDl_Buffer(phost, BITMAP_TRANSFORM_D(256));
    App_WrDl_Buffer(phost, BITMAP_TRANSFORM_E(0));
#else
//     App_WrDl_Buffer(phost, BITMAP_TRANSFORM_A(256));
//     App_WrDl_Buffer(phost, BITMAP_TRANSFORM_B(0));
//     App_WrDl_Buffer(phost, BITMAP_TRANSFORM_D(0));
//     App_WrDl_Buffer(phost, BITMAP_TRANSFORM_E(256));
#endif
    App_WrDl_Buffer(phost, BEGIN(BITMAPS));
    App_WrDl_Buffer(phost, VERTEX2II(0, 0, 0, 0) );
    App_WrDl_Buffer(phost, END());

    App_WrDl_Buffer(phost,DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(phost);

     /* Do a swap */
    Gpu_Hal_DLSwap(phost,DLSWAP_FRAME);

    //uint32_t t2 = Time_Now();
    //LOGD("[send][%d]\r\n", t1 - t2);
#else
        Gpu_CoCmd_Dlstart(phost);
        App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
        App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
        App_WrCoCmd_Buffer(phost, BITMAP_HANDLE(0));
        App_WrCoCmd_Buffer(phost, BITMAP_SOURCE(0));
#ifdef SW_ROTATE
        App_WrCoCmd_Buffer(phost, BITMAP_LAYOUT(RGB565, 240 * 2, 320));
        App_WrCoCmd_Buffer(phost, BITMAP_SIZE(NEAREST, BORDER, BORDER, 240, 320));
        App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
        App_WrCoCmd_Buffer(phost, VERTEX2II(0, 0, 0, 0));
        App_WrCoCmd_Buffer(phost, END());
#else
        App_WrCoCmd_Buffer(phost, BITMAP_LAYOUT(RGB565, 400 * 2, 240));
        App_WrCoCmd_Buffer(phost, BITMAP_SIZE(NEAREST, BORDER, BORDER, 0, 0));
        App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
        Gpu_CoCmd_LoadIdentity(phost);
        Gpu_CoCmd_Translate(phost, 65536 * 200, 65536 * 120);
        Gpu_CoCmd_Rotate(phost, 270 * 65536 / 360);
        Gpu_CoCmd_Translate(phost, 65536 * -200, 65536 * -200);
        Gpu_CoCmd_SetMatrix(phost);
        App_WrCoCmd_Buffer(phost, VERTEX2II(0, 0, 0, 0));
        App_WrCoCmd_Buffer(phost, END());
#endif

        ////////////////////////////////////////////////////////////////////////////////////////
        App_WrCoCmd_Buffer(phost, DISPLAY());

        Gpu_CoCmd_Swap(phost);
        App_Flush_Co_Buffer(phost);
        Gpu_Hal_WaitCmdfifo_empty(phost);

#endif
    }
}

int RVDisp_Init(uint16_t *pDispBuffer)
{
    phost = &host;

    /* Init HW Hal */
    App_Common_Init(&host);

    return 1;
}
#endif /*RTFFI_RV_DISP_DRIVER*/
#endif
