/*
 * The Clear BSD License
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#if RTVISION_BOARD

//#define USE_RTOS_SPI

#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "fsl_qtmr.h"
#include "fsl_lpspi.h"
#ifdef USE_RTOS_SPI
#include "fsl_lpspi_freertos.h"
#endif
#include "fsl_pcal.h"
#include "fsl_pxp.h"
#include "fsl_jdisp.h"
//#include "nxp-logo.h"
#include "fsl_video_common.h"
#include "fsl_time.h"
#include "fsl_log.h"
#include "cr_section_macros.h"

#define FUNC_AT_ITC __attribute__((section(".ramfunc.$SRAM_ITC")))

/*******************************************************************************
 * Prototype
 ******************************************************************************/
/*!
* @brief LPSPI4 initialization
*/

/* Master related */
#define DISP_LPSPI_MASTER_BASEADDR (LPSPI4)
#define DISP_LPSPI_MASTER_IRQN LPSPI4_IRQn
#define DISP_LPSPI_MASTER_IRQHandler LPSPI4_IRQHandler

/* Select USB1 PLL PFD0 (720 MHz) as lpspi clock source */
#define DISP_LPSPI_CLOCK_SOURCE_SELECT (1U)
/* Clock divider for master lpspi clock source */
#define DISP_LPSPI_CLOCK_SOURCE_DIVIDER (7U)

#define DISP_LPSPI_MASTER_CLK_FREQ (CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk) / (DISP_LPSPI_CLOCK_SOURCE_DIVIDER + 1U))
#define DISP_TRANSFER_BAUDRATE 6000000U /*! Transfer baudrate - 1M */

#define DISP_LPSPI_MASTER_PCS_FOR_INIT (kLPSPI_Pcs0)
#define DISP_LPSPI_MASTER_PCS_FOR_TRANSFER (kLPSPI_MasterPcs0)

#ifdef USE_RTOS_SPI
uint32_t LPSPI_GetInstance(LPSPI_Type *base);
lpspi_rtos_handle_t g_disp_spi_handle;
static const IRQn_Type s_lpspiIRQ[] = LPSPI_IRQS;
#endif

static void initDisplayLPSPI4()
{
	uint32_t srcClock_Hz;
	lpspi_master_config_t masterConfig;

	/*Set clock source for LPSPI*/
	CLOCK_SetMux(kCLOCK_LpspiMux, DISP_LPSPI_CLOCK_SOURCE_SELECT);
	CLOCK_SetDiv(kCLOCK_LpspiDiv, DISP_LPSPI_CLOCK_SOURCE_DIVIDER);

	/* Initialize SPI master */
	/*Master config*/
	masterConfig.baudRate = DISP_TRANSFER_BAUDRATE;
	masterConfig.bitsPerFrame = 8;
	masterConfig.cpol = kLPSPI_ClockPolarityActiveHigh;
	masterConfig.cpha = kLPSPI_ClockPhaseFirstEdge;
	masterConfig.direction = kLPSPI_MsbFirst;

	masterConfig.pcsToSckDelayInNanoSec = 1000000000 / masterConfig.baudRate;
	masterConfig.lastSckToPcsDelayInNanoSec = 1000000000 / masterConfig.baudRate;
	masterConfig.betweenTransferDelayInNanoSec = 1000000000 / masterConfig.baudRate;

	masterConfig.whichPcs = DISP_LPSPI_MASTER_PCS_FOR_INIT;
	masterConfig.pcsActiveHighOrLow = kLPSPI_PcsActiveHigh;

	masterConfig.pinCfg = kLPSPI_SdiInSdoOut;
	masterConfig.dataOutConfig = kLpspiDataOutRetained;

	srcClock_Hz = DISP_LPSPI_MASTER_CLK_FREQ;
	PRINTF("[SPI Clock] %d\r\n", srcClock_Hz);

#ifndef USE_RTOS_SPI
    LPSPI_MasterInit(DISP_LPSPI_MASTER_BASEADDR, &masterConfig, srcClock_Hz);
#else
    LPSPI_RTOS_Init(&g_disp_spi_handle, DISP_LPSPI_MASTER_BASEADDR, &masterConfig, srcClock_Hz);
    NVIC_SetPriority(s_lpspiIRQ[LPSPI_GetInstance(DISP_LPSPI_MASTER_BASEADDR)],2);
#endif

}


/*!
* @brief Display SPI test API
*/

uint8_t barcolors[4]=
{
		DISP_RED,
		DISP_GREEN,
		DISP_BLUE,
		DISP_WHITE
};


void displayOn(int turnOn)
{
	if ( turnOn )
	{
		set_iox_port_pin(OUTPUT_PORT1, 4, 1);
	}
	else
	{
		set_iox_port_pin(OUTPUT_PORT1, 4, 0);
	}
}

static void displayExtVcom(int useExtVcom)
{
	if ( useExtVcom )
	{
		set_iox_port_pin(OUTPUT_PORT1, 5, 1);
	}
	else
	{
		set_iox_port_pin(OUTPUT_PORT1, 5, 0);
	}
}

static void displayComTimerOn(int turnOn)
{
    qtmr_config_t qtmrConfig;
    if (turnOn) {

        QTMR_GetDefaultConfig(&qtmrConfig);

        qtmrConfig.primarySource = kQTMR_ClockDivide_128;
        QTMR_Init(DISP_QTMR_BASEADDR, DISP_QTMR_PWM_CHANNEL,  &qtmrConfig);

        /* Generate a 50Khz PWM signal with 50% dutycycle */
        QTMR_SetupPwm(DISP_QTMR_BASEADDR, DISP_QTMR_PWM_CHANNEL, 60, 50, false, DISP_QTMR_SOURCE_CLOCK / 128);

        /* Start the counter */
        QTMR_StartTimer(DISP_QTMR_BASEADDR, DISP_QTMR_PWM_CHANNEL, kQTMR_PriSrcRiseEdge);

        displayExtVcom(1);
    } else {
        displayExtVcom(0);
        QTMR_StopTimer(DISP_QTMR_BASEADDR, DISP_QTMR_PWM_CHANNEL);
        QTMR_Deinit(DISP_QTMR_BASEADDR, DISP_QTMR_PWM_CHANNEL);
    }
}

static void displaySetPixel(uint8_t * pFrame, int line, int col, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t *pCrtPixel;
    uint8_t color = (red<<3) | (green<<2) | (blue<<1);

    pCrtPixel = &pFrame[(line*JDISP_WIDTH + col) /2];

    if (col%2) {
        *pCrtPixel &= 0xf0;
        *pCrtPixel |= color;
    } else {
        *pCrtPixel &= 0x0f;
        *pCrtPixel |= color<<4;
    }
}

static int displaySendLine(uint8_t *pFrame, int line)
{
    lpspi_transfer_t masterXfer;
    uint8_t masterTxData[4 + JDISP_WIDTH/2 ]; /* CMD(1)+LineAddr(1)+data(disp_line_size)+dummy(2);*/

    masterTxData[0] = DISP_CMD_UPDATE_4BIT_MODE;
    masterTxData[1] = line + 1;
    memcpy(masterTxData+2, &pFrame[line*JDISP_WIDTH/2], JDISP_WIDTH/2);

    masterXfer.dataSize = JDISP_WIDTH/2+4;
    masterXfer.txData = &masterTxData[0];
    masterXfer.rxData = NULL;
    masterXfer.configFlags = DISP_LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterPcsContinuous;

#ifndef USE_RTOS_SPI
    LPSPI_MasterTransferBlocking(DISP_LPSPI_MASTER_BASEADDR, &masterXfer);
#else
    LPSPI_RTOS_Transfer(&g_disp_spi_handle, &masterXfer);
#endif
    return 1;
}

static int displayClear()
{
    lpspi_transfer_t masterXfer;
    uint8_t masterTxData[2];

    /* Send all clear */
    masterTxData[0] = DISP_CMD_ALL_CLEAR;
    masterTxData[1] = 0x0; /*dummy data */
    masterXfer.txData = &masterTxData[0];
    masterXfer.rxData = NULL;
    masterXfer.dataSize = 2;
    masterXfer.configFlags = DISP_LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterPcsContinuous;
#ifndef USE_RTOS_SPI
    LPSPI_MasterTransferBlocking(DISP_LPSPI_MASTER_BASEADDR, &masterXfer);
#else
    LPSPI_RTOS_Transfer(&g_disp_spi_handle, &masterXfer);
#endif
    /* Wait at least 1ms before sending another command */
    VIDEO_DelayMs(10);
    return 1;
}

const int indexBayern4x4[16] = {0,  8,  2,  10,
                                12, 4,  14, 6,
                                3,  11, 1,  9,
                                15, 7,  13, 5};

/* return bayern index for a 4x4 bayern dither pattern not divided by 16*/
int indexValue(int yCoord, int xCoord) {
    int x,y;
    x = xCoord % 4;
    y = yCoord % 4;
    return indexBayern4x4[(x + y * 4)] /* do not divide by 16 here /16.0 */ ;
}

int dither(int color, int yCoord, int xCoord) {
    int closestColor = (color < 16) ? 0 : 1;
    int secondClosestColor = 1 - closestColor;
    int d = indexValue(yCoord, xCoord)*2; /* Scale value to 32, value is already scaled by 16 so just multiply by 2 */
    int distance = abs((closestColor<<5) - color);
    return (distance <= d) ? closestColor : secondClosestColor;
}

////////////////////////////

#define COLORS 8
//#define WIDTH PROCESS_WIDTH
#define WIDTH 400
#define HEIGHT 240

#define SRC_STRIDE_PIXELS 480

__BSS(SRAM_DTC)
unsigned char line[3][WIDTH];

/*__BSS(SRAM_DTC)*/ unsigned char colormap[COLORS][3] = {
    {  0,   0,   0},     /* Black       This color map should be replaced   */
	{  0,   0, 255},     /* Blue        by one available on your hardware.  */
    {  0, 255,   0},     /* Green       It may contain any number of colors */
	{  0, 255, 255},     /* Cyan        as long as the constant COLORS is   */
	{255,   0,   0},	 /* Red  		set correctly.                      */
	{255,   0, 255},     /* Magenta							                */
	{255, 255,   0},     /* Yellow                                         */
    {255, 255, 255} };   /* White                                           */

FUNC_AT_ITC
int putdot(uint8_t * pDispBuffer, int x, int y, int c)
{
	displaySetPixel(pDispBuffer, y/*+75*/,x, colormap[c][0]>>7, colormap[c][1]>>7, colormap[c][2]>>7);
	return 1;
}

FUNC_AT_ITC
int getimageline(uint16_t * FrameAddr, int crtline)
{
	int i;
	uint16_t *psSrc = FrameAddr + (HEIGHT - crtline )  * WIDTH - 1;
	if (crtline >= HEIGHT)
		return 0;

	for (i=0; i<WIDTH; i++)
	{
		line[0][i] = ((*psSrc >> 11) & 0x1f)<<3;
		line[1][i] = ((*psSrc >> 5) & 0x3f) <<2;
		line[2][i] = ((*psSrc) & 0x1f)<<3;
		psSrc--;
	}
	return 1;
}

/* Floyd/Steinberg error diffusion dithering algorithm in color.  The array
** line[][] contains the RGB values for the current line being processed;
** line[0][x] = red, line[1][x] = green, line[2][x] = blue.  It uses the
** external functions getline() and putdot(), whose pupose should be easy
** to see from the code.
*/

//int getimageline(int line);  /* Function to read line[] from image file;  */
                             /* must return EOF when done.                */
//putdot(int x, int y, int c); /* Plot dot of color c at location x, y.     */
FUNC_AT_ITC
int dither_fs(uint16_t * FrameAddr, uint8_t * pDispBuffer)
{
    static int ed[3][WIDTH] = {0};      /* Errors distributed down, i.e., */
                                        /* to the next line.              */
    int x, y, h, c, nc, v,              /* Working variables              */
        e[4],                           /* Error parts (7/8,1/8,5/8,3/8). */
        ef[3];                          /* Error distributed forward.     */
    long dist, sdist;                   /* Used for least-squares match.  */
    int crtline;
    for (x=0; x<WIDTH; ++x) {
        ed[0][x] = ed[1][x] = ed[2][x] = 0;
    }
    y = 0;                              /* Get one line at a time from    */
    for (crtline = 0; crtline < HEIGHT; crtline++){
    	getimageline(FrameAddr, crtline);	/* input image.                   */
        ef[0] = ef[1] = ef[2] = 0;      /* No forward error for first dot */

        for (x=0; x<WIDTH; ++x) {
            for (c=0; c<3; ++c) {
                v = line[c][x] + ef[c] + ed[c][x];  /* Add errors from    */
                if (v < 0) v = 0;                   /* previous pixels    */
                if (v > 255) v = 255;               /* and clip.          */
                line[c][x] = v;
            }

            sdist = 255L * 255L * 255L + 1L;        /* Compute the color  */
            for (c=0; c<COLORS; ++c) {              /* in colormap[] that */
                                                    /* is nearest to the  */
#define D(z) (line[z][x]-colormap[c][z])            /* corrected color.   */

                dist = D(0)*D(0) + D(1)*D(1) + D(2)*D(2);
                if (dist < sdist) {
                    nc = c;
                    sdist = dist;
                }
            }
            putdot(pDispBuffer, x, y, nc);           /* Nearest color found; plot it.  */

            for (c=0; c<3; ++c) {
                v = line[c][x] - colormap[nc][c];   /* V = new error; h = */
                h = v >> 1;                         /* half of v, e[1..4] */
                e[1] = (7 * h) >> 3;                /* will be filled     */
                e[2] = h - e[1];                    /* with the Floyd and */
                h = v - h;                          /* Steinberg weights. */
                e[3] = (5 * h) >> 3;
                e[4] = h = e[3];

                ef[c] = e[1];                       /* Distribute errors. */
                if (x < WIDTH-1) ed[c][x+1] = e[2];
                if (x == 0) ed[c][x] = e[3]; else ed[c][x] += e[3];
                if (x > 0) ed[c][x-1] += e[4];
            }
        } /* next x */

        ++y;
    } /* next y */

    return 0;
}

FUNC_AT_ITC
int nodither_fs(uint16_t * FrameAddr, uint8_t * pDispBuffer){

    int x, y, nc;              /* Working variables              */
    int crtline;
    y = 0;
    for (crtline = 0; crtline < HEIGHT; crtline++){
    	getimageline(FrameAddr, crtline);	/* input image.               */
        for (x=0; x<WIDTH; ++x) {

            nc = /*red*/((line[0][x]>>7)<<2) + /*green*/((line[1][x]>>7)<<1) + /*blue*/(line[2][x]>>7);
            putdot(pDispBuffer, x, y, nc);           /* Nearest color found; plot it.  */
        } /* next x */

        ++y;
    } /* next y */

    return 0;
}

void JDisp_CopyFrameToDisplayBuffer(uint16_t* pFrameAddr, uint8_t * pDispBuffer, uint8_t pDitherEnable)
{
    //uint32_t t1 = Time_Now();
	if(pDitherEnable){
		dither_fs(pFrameAddr, pDispBuffer);
	}
	else {
		nodither_fs(pFrameAddr, pDispBuffer);
	}
    //uint32_t t2 = Time_Now();
    //LOGD("[dither][%d]\r\n", t1 - t2);
}

void JDisp_SendFrame(uint8_t *pFrame)
{
    int line;
    //uint32_t t1 = Time_Now();
    for (line = 0; line < HEIGHT; line++) {
        displaySendLine(pFrame, line);
    }
    //uint32_t t2 = Time_Now();
    //LOGD("[send][%d]\r\n", t1 - t2);
}

int JDisp_Init( uint8_t *pDispBuffer )
{
    //uint8_t *pCameraFrame;
    initDisplayLPSPI4();
    //displayInitBuffer(pDispBuffer);

    /* Enable com timer 60hz */
    displayComTimerOn(1);

    displayClear();

    displayOn(1);

    displayClear();

    //JDisp_SendFrame(pDispBuffer);
    return 1;
}
#endif
