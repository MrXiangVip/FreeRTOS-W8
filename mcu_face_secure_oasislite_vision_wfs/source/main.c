/*
 * Copyright (c) 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_pcal.h"
#include "fsl_dcp.h"
#include "board.h"
#include "pin_mux.h"
#include "FreeRTOS.h"
#include "task.h"

#include "sln_vizn_init_api.h"
#include "ble_hello_sensor.h"
#include "camera.h"
#include "pxp.h"
#include "database_api.h"
#include "display.h"
#include "oasis.h"
#include "switch.h"
#include "sln_lpm.h"

#include "sln_shell.h"
#include "sln_flash.h"
#include "sln_flash_mgmt.h"
#include "sln_file_table.h"
#include "sln_system_state.h"
#include "sln_dev_cfg.h"
#include "sln_usb_composite.h"
#include "sln_wdog.h"

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"
#include "serial_port_usb.h"
#include "usb_phy.h"
#include "clock_config.h"
#include "ui.h"
#include "fsl_stdisp.h"
#include "MCU_UART5.h"
#include "WIFI_UART8.h"
#include "fsl_log.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CONTROLLER_ID kSerialManager_UsbControllerEhci0


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void USB_DeviceClockInit(void);
static void FileSystem_Init();

/*******************************************************************************
 * Code
 ******************************************************************************/
/* The function sets the cacheable memory to shareable, this suggestion is referred from chapter 2.2.1 Memory regions,
 * types and attributes in Cortex-M7 Devices, Generic User Guide */
void BOARD_ConfigUSBMPU()
{
    /* Disable I cache and D cache */
    SCB_DisableICache();
    SCB_DisableDCache();

    /* Disable MPU */
    ARM_MPU_Disable();
    /* MPU configure:
     * Use ARM_MPU_RASR(DisableExec, AccessPermission, TypeExtField, IsShareable, IsCacheable, IsBufferable,
     * SubRegionDisable, Size)
     * API in core_cm7.h.
     * param DisableExec       Instruction access (XN) disable bit,0=instruction fetches enabled, 1=instruction fetches
     * disabled.
     * param AccessPermission  Data access permissions, allows you to configure read/write access for User and
     * Privileged mode.
     *      Use MACROS defined in core_cm7.h:
     * ARM_MPU_AP_NONE/ARM_MPU_AP_PRIV/ARM_MPU_AP_URO/ARM_MPU_AP_FULL/ARM_MPU_AP_PRO/ARM_MPU_AP_RO
     * Combine TypeExtField/IsShareable/IsCacheable/IsBufferable to configure MPU memory access attributes.
     *  TypeExtField  IsShareable  IsCacheable  IsBufferable   Memory Attribtue    Shareability        Cache
     *     0             x           0           0             Strongly Ordered    shareable
     *     0             x           0           1              Device             shareable
     *     0             0           1           0              Normal             not shareable   Outer and inner write
     * through no write allocate
     *     0             0           1           1              Normal             not shareable   Outer and inner write
     * back no write allocate
     *     0             1           1           0              Normal             shareable       Outer and inner write
     * through no write allocate
     *     0             1           1           1              Normal             shareable       Outer and inner write
     * back no write allocate
     *     1             0           0           0              Normal             not shareable   outer and inner
     * noncache
     *     1             1           0           0              Normal             shareable       outer and inner
     * noncache
     *     1             0           1           1              Normal             not shareable   outer and inner write
     * back write/read acllocate
     *     1             1           1           1              Normal             shareable       outer and inner write
     * back write/read acllocate
     *     2             x           0           0              Device              not shareable
     *  Above are normal use settings, if your want to see more details or want to config different inner/outter cache
     * policy.
     *  please refer to Table 4-55 /4-56 in arm cortex-M7 generic user guide <dui0646b_cortex_m7_dgug.pdf>
     * param SubRegionDisable  Sub-region disable field. 0=sub-region is enabled, 1=sub-region is disabled.
     * param Size              Region size of the region to be configured. use ARM_MPU_REGION_SIZE_xxx MACRO in
     * core_cm7.h.
     */
    MPU->RBAR = ARM_MPU_RBAR(7, 0x80000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 1, 1, 1, 0, ARM_MPU_REGION_SIZE_32MB);
    /* Enable MPU */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

    /* Enable I cache and D cache */
    SCB_EnableDCache();
    SCB_EnableICache();
}


void USB_DeviceClockInit(void)
{
#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U)
    usb_phy_config_struct_t phyConfig = {
        BOARD_USB_PHY_D_CAL,
        BOARD_USB_PHY_TXCAL45DP,
        BOARD_USB_PHY_TXCAL45DM,
    };
#endif
#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U)
    if (CONTROLLER_ID == kSerialManager_UsbControllerEhci0)
    {
        CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
        CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, 480000000U);
    }
    else
    {
        CLOCK_EnableUsbhs1PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
        CLOCK_EnableUsbhs1Clock(kCLOCK_Usb480M, 480000000U);
    }
    USB_EhciPhyInit(CONTROLLER_ID, BOARD_XTAL0_CLK_HZ, &phyConfig);
#endif
}

static void FileSystem_Init()
{
    /* Initialize DCP to enable CRC generation */
    dcp_config_t dcpConfig;
    DCP_GetDefaultConfig(&dcpConfig);
    DCP_Init(DCP, &dcpConfig);

    /* Initialize Flash to allow writing */
    SLN_Flash_Init();
    /* Initialize flash management */
    SLN_FLASH_MGMT_Init((sln_flash_entry_t *)g_fileTable , false);

    /* Initialize the user application config data */
    Cfg_AppDataInit();
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_BootClockRUN();

    /* Init pins (camera, display, pcal, ...) */
    BOARD_InitCSIPins();
    BOARD_InitCameraResource();
#if RTVISION_BOARD
    BOARD_InitLPSPI4Pins();
#endif

#if RTVISION_BOARD
    BOARD_InitA71CHPins();
    BOARD_InitPCALResource();
    BOARD_InitDEBUG_UARTPins();
#endif
    //BOARD_InitFlash();
#if RT106F_ELOCK_BOARD
#if BOARD_SUPPORT_PARALLEL_LCD
    BOARD_InitLPUART2Pins();
#else
    BOARD_InitDEBUG_UARTPins();
#endif
    BOARD_IniteLockDispPins();
//    BOARD_InitPIRPins();
//    BOARD_InitSwitchPins();

//    BOARD_InitWicedBLE();

#endif

   BOARD_InitLPUART5Pins();//mcu connect
	#if WIFI_SUPPORT != 0
	BOARD_InitLPUART8Pins();//WIFI connect
	#endif
    
    FileSystem_Init();
//    SysState_Init();
    DB_Init();

    vizn_api_init();
    uart_shell_init();
    StartUsbCompositeDualVCom();

    //SLN_LPM_Init();
    MCU_UART5_Start();
	#if WIFI_SUPPORT != 0
    WIFI_UART8_Start();
    #endif
    Camera_Start();
    APP_PXP_Start();
    Display_Start();
    Oasis_Start();
    //cmdSysInitOKSyncReq(SYS_VERSION);
    
//    WicedBLE_Start();

//    Switch_Start();
#if defined(WDOG_ENABLE) && WDOG_ENABLE
    SLN_WatchdogEnable(SLN_WDOG_TIMEOUT_SEC, SLN_WDOG_REFRESH_SEC);
#endif
    /* Run RTOS */
    vTaskStartScheduler();

    /* Should not reach this statement */
    while (1)
    {
    }
}
