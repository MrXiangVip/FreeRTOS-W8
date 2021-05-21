/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#if 0
#ifndef _EMWIN_SUPPORT_H_
#define _EMWIN_SUPPORT_H_

#define BOARD_LCD_SPI Driver_SPI4
#define BOARD_LCD_SPI_BAUDRATE 30000000U /*! Transfer baudrate */

#define BOARD_LCD_LPSPI_DMA_MUX_BASEADDR DMAMUX
#define BOARD_LCD_LPSPI_DMA_BASEADDR DMA0

#define BOARD_LCD_SPI_IRQ  LPSPI4_IRQn

#define DISP_LPSPI_TX_IRQn DMA6_DMA22_IRQn
#define DISP_LPSPI_RX_IRQn DMA7_DMA23_IRQn

#define BOARD_LCD_PWR_GPIO GPIO2   /*! LCD power port */
#define BOARD_LCD_PWR_GPIO_PIN 16U /*! LCD power pin */

#define BOARD_LCD_RST_GPIO GPIO2     /*! LCD reset port */
#define BOARD_LCD_RST_GPIO_PIN 7U    /*! LCD reset pin */

#define BOARD_LCD_DC_GPIO GPIO2   /*! LCD data/command port */
#define BOARD_LCD_DC_GPIO_PIN 6U /*! LCD data/command pin */

#define BOARD_LCD_READABLE 0

#define LCD_WIDTH 240
#define LCD_HEIGHT 320

#define BOARD_TOUCH_I2C Driver_I2C1
#define BOARD_TOUCH_I2C_IRQ LPI2C1_IRQn

/* Define scale factors */
#define GUI_SCALE_FACTOR 0.8
#define GUI_SCALE_FACTOR_X 1.5
#define GUI_SCALE_FACTOR_Y 1.1

/* Use larger fonts */
#define GUI_NORMAL_FONT (&GUI_Font24_ASCII)
#define GUI_LARGE_FONT (&GUI_Font32B_ASCII)

#define GUI_NUMBYTES 0x25800 /*! Amount of memory assigned to the emWin library */

extern int BOARD_Touch_Poll(void);

#endif
#endif
