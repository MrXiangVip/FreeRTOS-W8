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

#ifndef SPI_PERIPHERALS_TEST_H_
#define SPI_PERIPHERALS_TEST_H_

#if RTVISION_BOARD

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_common.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if 0
/* Master related */
#define EXAMPLE_LPSPI_MASTER_BASEADDR (LPSPI4)
#define EXAMPLE_LPSPI_MASTER_IRQN LPSPI4_IRQn
#define EXAMPLE_LPSPI_MASTER_IRQHandler LPSPI4_IRQHandler

/* Select USB1 PLL PFD0 (720 MHz) as lpspi clock source */
#define EXAMPLE_LPSPI_CLOCK_SOURCE_SELECT (1U)
/* Clock divider for master lpspi clock source */
#define EXAMPLE_LPSPI_CLOCK_SOURCE_DIVIDER (7U)

#define LPSPI_MASTER_CLK_FREQ (CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk) / (EXAMPLE_LPSPI_CLOCK_SOURCE_DIVIDER + 1U))
#define TRANSFER_BAUDRATE 500000U /*! Transfer baudrate - 500k */

#define EXAMPLE_LPSPI_MASTER_PCS_FOR_INIT (kLPSPI_Pcs0)
#define EXAMPLE_LPSPI_MASTER_PCS_FOR_TRANSFER (kLPSPI_MasterPcs0)
#endif

#define CS_PIN_GPIO    GPIO2
#define CS_PIN        0U


/* Hardware related display defines */

/* The QTMR instance/channel used for display com inversion */
#define DISP_QTMR_BASEADDR TMR2
#define DISP_QTMR_PWM_CHANNEL kQTMR_Channel_2
/* Get source clock for the display QTMR driver */
#define DISP_QTMR_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_IpgClk)

/*GPIO for DISP_DISP signal */
#define DISP_DISP_PIN_GPIO    GPIO1
#define DISP_DISP_PIN        1U

/*GPIO for DISP_EXTMODE signal */
#define DISP_EXTMODE_PIN_GPIO    GPIO1
#define DISP_EXTMODE_PIN        8U

/* Display colors for 4 bit mode, the LSB bit is dummy, bits are ordered as RGBD */

#define DISP_BLACK     0x00
#define DISP_WHITE     0x0E
/* Primary colors */
#define DISP_RED       0x08
#define DISP_GREEN     0x04
#define DISP_BLUE      0x02

/* Other combinations */
#define DISP_CYAN      0x06
#define DISP_MAGENTA   0x0A
#define DISP_YELLOW    0x0C

#define DISP_CMD_ALL_CLEAR         0x20 /* Set all display memory to black */
#define DISP_CMD_UPDATE_4BIT_MODE  0x90 /* Send one or more lines to the display */
#define DISP_CMD_NO_UPDATE         0x00 /* Used for SPI VCOM update when not using external vcom toggling and no recent command was sent */
#define DISP_CMD_VCOM              0x40 /* used as a flag (M1) to invert vcom */

#if defined(__cplusplus)
extern "C" {
#endif


/*******************************************************************************
 * ENUMS
 ******************************************************************************/
enum spistatus {
    SPI_FAIL = -1,
    SPI_PASS = 0,
};

/*******************************************************************************
 * Prototype
 ******************************************************************************/

/* Software related display defines */
#ifdef RapidIOT_LCD
#define JDISP_WIDTH 176
#define JDISP_HEIGHT 176
#else
#define JDISP_WIDTH 400
#define JDISP_HEIGHT 240
#endif

void displayOn(int turnOn);
int JDisp_Init( uint8_t* pDispBuffer );
void JDisp_CopyFrameToDisplayBuffer(uint16_t* pFrameAddr, uint8_t* pDispBuffer, uint8_t pDitherEnable);
void JDisp_SendFrame(uint8_t* pFrame);

#if defined(__cplusplus)
}
#endif

#endif

#endif /* SPI_PERIPHERALS_TEST_H_ */
