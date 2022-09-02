/*
 * Copyright (c) 2019-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dcp.h"
#include "app.h"
#include "board.h"
#include "pin_mux.h"
#include "FreeRTOS.h"
#include "task.h"

#include "camera.h"
#include "database_api.h"
#include "display.h"
#include "oasis.h"
#include "switch.h"
#include "pxp.h"

#if WIFI_ENABLE
#include "sln_connection.h"
#include "sln_tcp_connection.h"
#endif
#include "sln_lpm.h"
#include "sln_api_init.h"
#include "sln_shell.h"
#include "sln_flash.h"
#include "sln_flash_mgmt.h"
#include "sln_file_table.h"
#include "sln_system_state.h"
#include "sln_dev_cfg.h"
#include "sln_usb_composite.h"
#include "sln_wdog.h"
#include "MCU_UART5.h"
#include "sln_audio.h"
#include "UART_FAKER.h"
#include "wifi_main.h"
#include "fsl_log.h"
#define RELOCATE_VECTOR_TABLE 1

const unsigned int gFWVersionNumber = APP_VERSION_NUMBER;

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void FileSystem_Init();

/*******************************************************************************
 * Code
 ******************************************************************************/

static void FileSystem_Init()
{
    /* Initialize DCP to enable CRC generation */
    dcp_config_t dcpConfig;
    DCP_GetDefaultConfig(&dcpConfig);
    DCP_Init(DCP, &dcpConfig);

    /* Initialize Flash to allow writing */
    SLN_Flash_Init();
    /* Initialize flash management */
    SLN_FLASH_MGMT_Init((sln_flash_entry_t *)g_fileTable, false);

    /* Initialize the user application config data */
    Cfg_AppDataInit();

    //move from FeatureDB
    fatfs_mount_with_mkfs();
}

/*!
 * @brief Main function
 */
int main(void)
{
#if defined(RELOCATE_VECTOR_TABLE) && RELOCATE_VECTOR_TABLE
    BOARD_RelocateVectorTableToRam();
#endif

    /* Init board hardware. */
    BOARD_InitHardware();

    FileSystem_Init();
    //SysState_Init();

    uart_shell_init();
#if	USB_OTG_SUPPORT != 0
    USB_CompositeInit();
#endif

    DB_Init();
    init_config();
    MCU_UART5_Start();
    uFakeUartTaskStart( );

//#if WIFI_SUPPORT != 0
//    WIFI_UART8_Start();
//#endif

    WIFI_Start();

    Camera_Start();
    APP_PXP_Start();
    //Display_Start();
    //Oasis_Start();

    vizn_api_init();

#if defined(WDOG_ENABLE) && WDOG_ENABLE
    SLN_WatchdogEnable(SLN_WDOG_TIMEOUT_SEC, SLN_WDOG_REFRESH_SEC);
#endif

    /* Run RTOS */
    vTaskStartScheduler();
    LOGD("FreeRtos Ready! \r\n");
    /* Should not reach this statement */
    while (1)
    {
    }
}
