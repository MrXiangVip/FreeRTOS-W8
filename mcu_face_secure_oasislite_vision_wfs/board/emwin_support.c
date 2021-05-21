/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if 0
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "GUI.h"
#include "WM.h"
//#include "GUIDRV_Template.h"
#include "GUIDRV_FlexColor.h"
#include "emwin_support.h"

#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "fsl_lpspi.h"
#include "fsl_st7789.h"
//#include "fsl_ft6x06.h"
//#include "fsl_lpi2c.h"
#include "fsl_lpspi_cmsis.h"
//#include "fsl_lpi2c_cmsis.h"
#include "fsl_cache.h"
#include <event_groups.h>

#ifndef GUI_MEMORY_ADDR
static uint32_t s_gui_memory[(GUI_NUMBYTES + 3) / 4]; /* needs to be word aligned */
#define GUI_MEMORY_ADDR ((uint32_t)s_gui_memory)
#endif
static SemaphoreHandle_t xQueueMutex;
static SemaphoreHandle_t xSemaTxDone;
static TimerHandle_t timer = NULL;

extern SemaphoreHandle_t xSemaphoreCsiBuffer;


/* Select SYS PLL2 PFD2 (328 MHz) as lpspi clock source */
#define LPSPI_CLOCK_SOURCE_SELECT (3U)
/* Clock divider for master lpspi clock source */
#define LPSPI_CLOCK_SOURCE_DIVIDER (2U)
/*******************************************************************************
 * Implementation of PortAPI for emWin LCD driver
 ******************************************************************************/
static volatile uint32_t spi_event;
static volatile bool spi_event_received;

uint32_t LPSPI4_GetFreq(void)
{
    return (CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk) / (LPSPI_CLOCK_SOURCE_DIVIDER + 1U));
}

static void SPI_MasterSignalEvent(uint32_t event)
{
    spi_event          = event;
    spi_event_received = true;
}

static uint32_t SPI_WaitEvent(void)
{
    uint32_t event;

    while (!spi_event_received)
        ;
    event              = spi_event;
    spi_event_received = false;

    return event;
}

static void APP_pfWrite8_A0(U8 Data)
{
    L1CACHE_CleanDCacheByRange((uint32_t)&Data, 1);
    GPIO_PortClear(BOARD_LCD_DC_GPIO, 1u << BOARD_LCD_DC_GPIO_PIN);
    BOARD_LCD_SPI.Send(&Data, 1);
    SPI_WaitEvent();
}

static void APP_pfWrite8_A1(U8 Data)
{
    L1CACHE_CleanDCacheByRange((uint32_t)&Data, 1);
    GPIO_PortSet(BOARD_LCD_DC_GPIO, 1u << BOARD_LCD_DC_GPIO_PIN);
    BOARD_LCD_SPI.Send(&Data, 1);
    SPI_WaitEvent();
}

static void APP_pfWriteM8_A1(U8 *pData, int NumItems)
{
    L1CACHE_CleanDCacheByRange((uint32_t)pData, NumItems);
    GPIO_PortSet(BOARD_LCD_DC_GPIO, 1u << BOARD_LCD_DC_GPIO_PIN);
    BOARD_LCD_SPI.Send(pData, NumItems);
    SPI_WaitEvent();
}

static U8 APP_pfRead8_A1(void)
{
    uint8_t Data;
#if defined(BOARD_LCD_READABLE) && (BOARD_LCD_READABLE == 0)
    PRINTF("Warning: LCD does not support read operation, the image may get distorted.\r\n");
    assert(0);
#endif
    GPIO_PortSet(BOARD_LCD_DC_GPIO, 1u << BOARD_LCD_DC_GPIO_PIN);
    BOARD_LCD_SPI.Receive(&Data, 1);
    SPI_WaitEvent();
    return Data;
}

static void APP_pfReadM8_A1(U8 *pData, int NumItems)
{
#if defined(BOARD_LCD_READABLE) && (BOARD_LCD_READABLE == 0)
    PRINTF("Warning: LCD does not support read operation, the image may get distorted.\r\n");
    assert(0);
#endif
    GPIO_PortSet(BOARD_LCD_DC_GPIO, 1u << BOARD_LCD_DC_GPIO_PIN);
    BOARD_LCD_SPI.Receive(pData, NumItems);
    SPI_WaitEvent();
}

static void APP_pfWrite16_A0(U16 Data)
{
    L1CACHE_CleanDCacheByRange((uint32_t)&Data, 1);
    GPIO_PortClear(BOARD_LCD_DC_GPIO, 1u << BOARD_LCD_DC_GPIO_PIN);
    BOARD_LCD_SPI.Send(&Data, 1);
    SPI_WaitEvent();
}

static void APP_pfWrite16_A1(U16 Data)
{
    BOARD_LCD_SPI.Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | ARM_SPI_DATA_BITS(16), BOARD_LCD_SPI_BAUDRATE);
    L1CACHE_CleanDCacheByRange((uint32_t)&Data, 2);
    GPIO_PortSet(BOARD_LCD_DC_GPIO, 1u << BOARD_LCD_DC_GPIO_PIN);
    BOARD_LCD_SPI.Send(&Data, 2);
    SPI_WaitEvent();
    BOARD_LCD_SPI.Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | ARM_SPI_DATA_BITS(8), BOARD_LCD_SPI_BAUDRATE);
}

static void APP_pfWriteM16_A1(U16 *pData, int NumItems)
{
    BOARD_LCD_SPI.Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | ARM_SPI_DATA_BITS(16), BOARD_LCD_SPI_BAUDRATE);
    L1CACHE_CleanDCacheByRange((uint32_t)pData, 2*NumItems);
    GPIO_PortSet(BOARD_LCD_DC_GPIO, 1u << BOARD_LCD_DC_GPIO_PIN);
    BOARD_LCD_SPI.Send(pData, 2*NumItems);
    SPI_WaitEvent();
    BOARD_LCD_SPI.Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | ARM_SPI_DATA_BITS(8), BOARD_LCD_SPI_BAUDRATE);
}

static U16 APP_pfRead16_A1(void)
{
    uint16_t Data;
#if defined(BOARD_LCD_READABLE) && (BOARD_LCD_READABLE == 0)
    PRINTF("Warning: LCD does not support read operation, the image may get distorted.\r\n");
    assert(0);
#endif
    GPIO_PortSet(BOARD_LCD_DC_GPIO, 1u << BOARD_LCD_DC_GPIO_PIN);
    BOARD_LCD_SPI.Receive(&Data, 2);
    SPI_WaitEvent();
    return Data;
}

static void APP_pfReadM16_A1(U16 *pData, int NumItems)
{
#if defined(BOARD_LCD_READABLE) && (BOARD_LCD_READABLE == 0)
    PRINTF("Warning: LCD does not support read operation, the image may get distorted.\r\n");
    assert(0);
#endif
    GPIO_PortSet(BOARD_LCD_DC_GPIO, 1u << BOARD_LCD_DC_GPIO_PIN);
    BOARD_LCD_SPI.Receive(pData, 2*NumItems);
    SPI_WaitEvent();
}

static void BOARD_LCD_InterfaceInit(void)
{
    /* Define the init structure for the data/command output pin */
    gpio_pin_config_t pin_config = {
        kGPIO_DigitalOutput,
        1,
    };

    /* Init power enable GPIO output . */
    GPIO_PinInit(BOARD_LCD_PWR_GPIO, BOARD_LCD_PWR_GPIO_PIN, &pin_config);
    GPIO_PortClear(BOARD_LCD_PWR_GPIO, 1 << BOARD_LCD_PWR_GPIO_PIN);

    /* Init data/command GPIO output . */
    GPIO_PinInit(BOARD_LCD_DC_GPIO, BOARD_LCD_DC_GPIO_PIN, &pin_config);

    /* Init reset GPIO output . */
    GPIO_PinInit(BOARD_LCD_RST_GPIO, BOARD_LCD_RST_GPIO_PIN, &pin_config);
    GUI_X_Delay(200); /* settle down delay after reset */

    GPIO_PortClear(BOARD_LCD_RST_GPIO, 1 << BOARD_LCD_RST_GPIO_PIN);
    GUI_X_Delay(200); /* settle down delay after reset */

    GPIO_PortSet(BOARD_LCD_RST_GPIO, 1 << BOARD_LCD_RST_GPIO_PIN);
    GUI_X_Delay(800); /* settle down delay after reset */

    /*Set clock source for LPSPI*/
    CLOCK_SetMux(kCLOCK_LpspiMux, LPSPI_CLOCK_SOURCE_SELECT);
    CLOCK_SetDiv(kCLOCK_LpspiDiv, LPSPI_CLOCK_SOURCE_DIVIDER);

/* DMA Mux init and EDMA init */
#ifdef BOARD_LCD_LPSPI_DMA_BASEADDR
    /* Create Event Group for transfer completed */
    //g_lpspiDispTransferCompleted = xEventGroupCreate();
    edma_config_t edmaConfig = {0};
    EDMA_GetDefaultConfig(&edmaConfig);
    EDMA_Init(BOARD_LCD_LPSPI_DMA_BASEADDR, &edmaConfig);
#endif
#ifdef BOARD_LCD_LPSPI_DMA_MUX_BASEADDR
    DMAMUX_Init(BOARD_LCD_LPSPI_DMA_MUX_BASEADDR);
#endif

    /* SPI master init */
    BOARD_LCD_SPI.Initialize(SPI_MasterSignalEvent);
    BOARD_LCD_SPI.PowerControl(ARM_POWER_FULL);
    BOARD_LCD_SPI.Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | ARM_SPI_DATA_BITS(8), BOARD_LCD_SPI_BAUDRATE);

#ifdef BOARD_LCD_SPI_IRQ
    NVIC_SetPriority(BOARD_LCD_SPI_IRQ, 1U);
    NVIC_SetPriority(DISP_LPSPI_TX_IRQn, 2);
    NVIC_SetPriority(DISP_LPSPI_RX_IRQn, 2);
#endif
}

void BOARD_LCD_InterfaceDeinit(void)
{
    BOARD_LCD_SPI.PowerControl(ARM_POWER_OFF);
    BOARD_LCD_SPI.Uninitialize();
}

/*******************************************************************************
 * Application implemented functions required by emWin library
 ******************************************************************************/
void LCD_X_Config(void)
{
    GUI_DEVICE *pDevice;
    GUI_PORT_API PortAPI;

#if 0
    CONFIG_FLEXCOLOR Config = {0, 0, GUI_ROTATION_0, 0, 1};
    pDevice = GUI_DEVICE_CreateAndLink(GUIDRV_FLEXCOLOR, GUICC_M565, 0, 0);
    GUIDRV_FlexColor_Config(pDevice, &Config);

    LCD_SetSizeEx(0, LCD_WIDTH, LCD_HEIGHT);
    LCD_SetVSizeEx(0, LCD_WIDTH, LCD_HEIGHT);

    PortAPI.pfWrite8_A0  = APP_pfWrite8_A0;
    PortAPI.pfWrite8_A1  = APP_pfWrite8_A1;
    PortAPI.pfWriteM8_A1 = APP_pfWriteM8_A1;
    PortAPI.pfRead8_A1   = APP_pfRead8_A1;
    PortAPI.pfReadM8_A1  = APP_pfReadM8_A1;

    PortAPI.pfWrite16_A0  = APP_pfWrite16_A0;
    PortAPI.pfWrite16_A1  = APP_pfWrite16_A1;
    PortAPI.pfWriteM16_A1 = APP_pfWriteM16_A1;
    PortAPI.pfRead16_A1   = APP_pfRead16_A1;
    PortAPI.pfReadM16_A1  = APP_pfReadM16_A1;

    GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66709, GUIDRV_FLEXCOLOR_M16C0B8);
#endif

#if 1
    pDevice = GUI_DEVICE_CreateAndLink(&GUIDRV_Template_API, GUICC_M565, 0, 0);

    LCD_SetSizeEx(0, LCD_WIDTH, LCD_HEIGHT);
    LCD_SetVSizeEx(0, LCD_WIDTH, LCD_HEIGHT);

    PortAPI.pfWrite8_A0  = APP_pfWrite8_A0;
    PortAPI.pfWrite8_A1  = APP_pfWrite8_A1;
    PortAPI.pfWriteM8_A1 = APP_pfWriteM8_A1;
    PortAPI.pfRead8_A1   = APP_pfRead8_A1;
    PortAPI.pfReadM8_A1  = APP_pfReadM8_A1;

    PortAPI.pfWrite16_A0  = APP_pfWrite16_A0;
    PortAPI.pfWrite16_A1  = APP_pfWrite16_A1;
    PortAPI.pfWriteM16_A1 = APP_pfWriteM16_A1;
    PortAPI.pfRead16_A1   = APP_pfRead16_A1;
    PortAPI.pfReadM16_A1  = APP_pfReadM16_A1;

    GUIDRV_Template_SetFunc(pDevice, &PortAPI);
#endif

    //BOARD_Touch_InterfaceInit();
}

static U16 pdata[320*240];
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void *pData)
{
    int result = 0;

    switch (Cmd)
    {
        case LCD_X_INITCONTROLLER:
            BOARD_LCD_InterfaceInit();
            GUI_X_Delay(50); /* settle down delay after reset */
            ST7789_Init(APP_pfWrite8_A1, APP_pfWrite8_A0);

#if 0
            unsigned char xbuf[4] = {0, 0, 0, 150};
            unsigned char ybuf[4] = {0, 0, 0, 150};
            for(int i = 0; i < 320*240; i++)
                pdata[i] = 0xf800;

            APP_pfWrite8_A0(0x2a);
            APP_pfWrite8_A1(xbuf[0]);
            APP_pfWrite8_A1(xbuf[1]);
            APP_pfWrite8_A1(xbuf[2]);
            APP_pfWrite8_A1(xbuf[3]);

            APP_pfWrite8_A0(0x2b);
            APP_pfWrite8_A1(ybuf[0]);
            APP_pfWrite8_A1(ybuf[1]);
            APP_pfWrite8_A1(ybuf[2]);
            APP_pfWrite8_A1(ybuf[3]);

            APP_pfWrite8_A0(0x2c);
//            for(int i = 0; i < 320*240; i++)
//            {
//                APP_pfWrite8_A1(0x00);
//                APP_pfWrite8_A1(0x1f);
//            }
            int major_loop = 150*150/30/1024;
            for(int i = 0; i < major_loop; i++) {
                APP_pfWriteM16_A1(pdata, 30*1024);
            }
            int minor_loop = (150*150) % (30 *1024);

            APP_pfWriteM16_A1(pdata, minor_loop);
#endif
            break;
        default:
            result = -1;
            break;
    }

    return result;
}

void GUI_X_Config(void)
{
    /* Assign work memory area to emWin */
    GUI_ALLOC_AssignMemory((void *)GUI_MEMORY_ADDR, GUI_NUMBYTES);

    /* Select default font */
    GUI_SetDefaultFont(GUI_FONT_6X8);

}
void GUI_X_Init(void)
{
#if 1
	GUI_X_InitOS();
	GUI_SetWaitEventFunc(GUI_X_WaitEvent);
	GUI_SetSignalEventFunc(GUI_X_SignalEvent);
	GUI_SetWaitEventTimedFunc(GUI_X_WaitEventTimed);
#endif    
}

/*********************************************************************
* 
*      Multitasking:
*
*                 GUI_X_InitOS()
*                 GUI_X_GetTaskId()
*                 GUI_X_Lock()
*                 GUI_X_Unlock()
*
* Note:
*   The following routines are required only if emWin is used in a
*   true multi task environment, which means you have more than one
*   thread using the emWin API.
*   In this case the
*                       #define GUI_OS  (1)
*  needs to be in GUIConf.h
*/

/*! @brief Init OS.Creates the resource semaphore or mutex typically used by GUI_X_Lock() and GUI_X_Unlock().*/
void GUI_X_InitOS(void)
{
	/* Create a Mutex lock*/
	xQueueMutex = xSemaphoreCreateMutex();
	configASSERT(xQueueMutex !=NULL);
	
	vSemaphoreCreateBinary(xSemaTxDone);
	configASSERT(xSemaTxDone != NULL);
}

/*! @brief Locks the GUI*/
void GUI_X_Lock(void)
{
	xSemaphoreTake(xQueueMutex , portMAX_DELAY);
}

/*! @brief Unlocks the GUI */
void GUI_X_Unlock(void)
{
	xSemaphoreGive(xQueueMutex);
}

/*! @brief Get the Task handle */
U32 GUI_X_GetTaskId(void)
{
    return ((uint32_t)xTaskGetCurrentTaskHandle());
}

void GUI_X_WaitEvent (void)
{
    while (xSemaphoreTake(xSemaTxDone, portMAX_DELAY) != pdTRUE);
}

void GUI_X_SignalEvent ()
{
    xSemaphoreGive(xSemaTxDone);
}

void GUI_X_TimerCallBack(TimerHandle_t xTimer)
{
    GUI_X_SignalEvent();
}

void GUI_X_WaitEventTimed(int Period)
{
	if (Period > 0)
	{
		if (timer == NULL)
		{
			timer = xTimerCreate("GUI_X_T",
					pdMS_TO_TICKS(Period),
					pdFALSE,
					NULL,
					GUI_X_TimerCallBack);

			 if( timer == NULL )
			 {
				 /* The timer was not created. */
				 configASSERT(0);
			 }
			 else
			 {
				 /* Start the timer.  No block time is specified, and
				 even if one was it would be ignored because the RTOS
				 scheduler has not yet been started. */
				 if( xTimerStart( timer, 0 ) != pdPASS )
				 {
					 /* The timer could not be set into the Active
					 state. */
					 configASSERT(0);
				 }
			 }
		} // end of if (timer == NULL)
		else
		{
			//timer has been created
			if( xTimerChangePeriod( timer, pdMS_TO_TICKS(Period), 0 ) != pdPASS )
			{
			      // The command was successfully sent.
				configASSERT(0);
			}
		}

		//it's time to block itself here
		GUI_X_WaitEvent();
	}
}

void GUI_X_ExecIdle(void)
{
}

GUI_TIMER_TIME GUI_X_GetTime(void)
{
    return ((int) xTaskGetTickCount());
}

/*! @brief Returns after a specified time period in milliseconds. */
void GUI_X_Delay(int ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void *emWin_memcpy(void *pDst, const void *pSrc, long size)
{
    return memcpy(pDst, pSrc, size);
}
#endif
