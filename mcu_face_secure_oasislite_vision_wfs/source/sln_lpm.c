/*
 * Copyright 2018-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "sln_lpm.h"
#include "fsl_common.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "lpm.h"
#include "fsl_gpt.h"
#include "fsl_lpuart.h"
#include "specific.h"
#include "fsl_log.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CPU_NAME "iMXRT1062"

#if HAS_WAKEUP_PIN
#ifndef BOARD_USER_BUTTON_GPIO
#define BOARD_USER_BUTTON_GPIO GPIO5
#endif
#ifndef BOARD_USER_BUTTON_GPIO_PIN
#define BOARD_USER_BUTTON_GPIO_PIN (0U)
#endif
#ifndef BOARD_USER_BUTTON_IRQ
#define BOARD_USER_BUTTON_IRQ GPIO5_Combined_0_15_IRQn
#endif
#define BOARD_USER_BUTTON_IRQ_HANDLER GPIO5_Combined_0_15_IRQHandler
#define BOARD_USER_BUTTON_NAME "WL_HOST_WAKE"

#define APP_WAKEUP_BUTTON_GPIO BOARD_USER_BUTTON_GPIO
#define APP_WAKEUP_BUTTON_GPIO_PIN BOARD_USER_BUTTON_GPIO_PIN
#define APP_WAKEUP_BUTTON_IRQ BOARD_USER_BUTTON_IRQ
#define APP_WAKEUP_BUTTON_IRQ_HANDLER BOARD_USER_BUTTON_IRQ_HANDLER
#define APP_WAKEUP_BUTTON_NAME BOARD_USER_BUTTON_NAME
#endif

#define APP_WAKEUP_GPT_BASE GPT2
#define APP_WAKEUP_GPT_IRQn GPT2_IRQn
#define APP_WAKEUP_GPT_IRQn_HANDLER GPT2_IRQHandler

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static uint8_t s_wakeupTimeout;            /* Wakeup timeout. (Unit: Second) */
static app_wakeup_source_t s_wakeupSource; /* Wakeup source.                 */
static lpm_power_mode_t s_targetPowerMode;
static lpm_power_mode_t s_curRunMode = LPM_PowerModeOverRun;
static const char *s_modeNames[]     = {"Over RUN",    "Full Run",       "Low Speed Run", "Low Power Run",
                                    "System Idle", "Low Power Idle", "Suspend",
#if (HAS_WAKEUP_PIN)
                                    "SNVS"
#endif
};

/*******************************************************************************
 * Code
 ******************************************************************************/

void APP_WAKEUP_GPT_IRQn_HANDLER(void)
{
    GPT_ClearStatusFlags(APP_WAKEUP_GPT_BASE, kGPT_OutputCompare1Flag);
    GPT_StopTimer(APP_WAKEUP_GPT_BASE);
    LPM_DisableWakeupSource(APP_WAKEUP_GPT_IRQn);
}

#if HAS_WAKEUP_PIN
void APP_WAKEUP_BUTTON_IRQ_HANDLER(void)
{
    if ((1U << APP_WAKEUP_BUTTON_GPIO_PIN) & GPIO_GetPinsInterruptFlags(APP_WAKEUP_BUTTON_GPIO))
    {
        /* Disable interrupt. */
        GPIO_DisableInterrupts(APP_WAKEUP_BUTTON_GPIO, 1U << APP_WAKEUP_BUTTON_GPIO_PIN);
        GPIO_ClearPinsInterruptFlags(APP_WAKEUP_BUTTON_GPIO, 1U << APP_WAKEUP_BUTTON_GPIO_PIN);
        LPM_DisableWakeupSource(APP_WAKEUP_BUTTON_IRQ);
    }
}
#endif

/*!
 * @brief Get input from user about wakeup timeout
 */
static uint8_t APP_GetWakeupTimeout(void)
{
    uint8_t timeout;

    while (1)
    {
        PRINTF("Select the wake up timeout in seconds.\r\n");
        PRINTF("The allowed range is 1s ~ 9s.\r\n");
        PRINTF("Eg. enter 5 to wake up in 5 seconds.\r\n");
        PRINTF("\r\nWaiting for input timeout value...\r\n\r\n");

        timeout = GETCHAR();
        PRINTF("%c\r\n", timeout);
        if ((timeout > '0') && (timeout <= '9'))
        {
            return timeout - '0';
        }
        PRINTF("Wrong value!\r\n");
    }
}

/* Get wakeup source by user input. */
static app_wakeup_source_t APP_GetWakeupSource(lpm_power_mode_t targetMode)
{
    uint8_t ch;

    while (1)
    {
        PRINTF("Select the wake up source:\r\n");
        PRINTF("Press T for GPT - GPT Timer\r\n");
#if (HAS_WAKEUP_PIN)
        PRINTF("Press S for switch/button %s. \r\n", APP_WAKEUP_BUTTON_NAME);
#endif

        PRINTF("\r\nWaiting for key press..\r\n\r\n");

        ch = GETCHAR();

        if ((ch >= 'a') && (ch <= 'z'))
        {
            ch -= 'a' - 'A';
        }

        if (ch == 'T')
        {
            return kAPP_WakeupSourceGPT;
        }
#if (HAS_WAKEUP_PIN)
        else if (ch == 'S')
        {
            return kAPP_WakeupSourcePin;
        }
#endif
        else
        {
            PRINTF("Wrong value!\r\n");
        }
    }
}

/* Get wakeup timeout and wakeup source. */
static void APP_GetWakeupConfig(lpm_power_mode_t targetMode)
{
    if (targetMode == LPM_PowerModeSNVS)
    {
        /* In SNVS mode, only SNVS domain is powered, GPT could not work. */
        s_wakeupSource = kAPP_WakeupSourcePin;
    }
    else
    {
        /* Get wakeup source by user input. */
        s_wakeupSource = APP_GetWakeupSource(targetMode);
    }

    if (kAPP_WakeupSourceGPT == s_wakeupSource)
    {
        /* Wakeup source is GPT, user should input wakeup timeout value. */
        s_wakeupTimeout = APP_GetWakeupTimeout();
        PRINTF("Will wakeup in %d seconds.\r\n", s_wakeupTimeout);
    }
#if HAS_WAKEUP_PIN
    else
    {
        PRINTF("Switch %s from off to on to wake up.\r\n", APP_WAKEUP_BUTTON_NAME);
    }
#endif
}

static void APP_SetWakeupConfig(lpm_power_mode_t targetMode)
{
    /* Set GPT timeout value. */
    if (kAPP_WakeupSourceGPT == s_wakeupSource)
    {
        GPT_StopTimer(APP_WAKEUP_GPT_BASE);
        /* Update compare channel1 value will reset counter */
        GPT_SetOutputCompareValue(APP_WAKEUP_GPT_BASE, kGPT_OutputCompare_Channel1,
                                  (CLOCK_GetRtcFreq() * s_wakeupTimeout) - 1U);

        /* Enable GPT Output Compare1 interrupt */
        GPT_EnableInterrupts(APP_WAKEUP_GPT_BASE, kGPT_OutputCompare1InterruptEnable);
        NVIC_EnableIRQ(APP_WAKEUP_GPT_IRQn);
        EnableIRQ(APP_WAKEUP_GPT_IRQn);

        /* Restart timer */
        GPT_StartTimer(APP_WAKEUP_GPT_BASE);

        LPM_EnableWakeupSource(APP_WAKEUP_GPT_IRQn);
    }
#if HAS_WAKEUP_PIN
    else if(targetMode == LPM_PowerModeSNVS)
    {
        GPIO_ClearPinsInterruptFlags(APP_WAKEUP_BUTTON_GPIO, 1U << APP_WAKEUP_BUTTON_GPIO_PIN);
        /* Enable GPIO pin interrupt */
        GPIO_EnableInterrupts(APP_WAKEUP_BUTTON_GPIO, 1U << APP_WAKEUP_BUTTON_GPIO_PIN);
        NVIC_SetPriority(APP_WAKEUP_BUTTON_IRQ, configMAX_SYSCALL_INTERRUPT_PRIORITY + 2);
        /* Enable the Interrupt */
        EnableIRQ(APP_WAKEUP_BUTTON_IRQ);
        /* Enable GPC interrupt */
        LPM_EnableWakeupSource(APP_WAKEUP_BUTTON_IRQ);
    }
#endif
}

lpm_power_mode_t APP_GetRunMode(void)
{
    return s_curRunMode;
}

void APP_SetRunMode(lpm_power_mode_t powerMode)
{
    s_curRunMode = powerMode;
}

static void APP_ShowPowerMode(lpm_power_mode_t powerMode)
{
    if (powerMode <= LPM_PowerModeRunEnd)
    {
        PRINTF("    Power mode: %s\r\n", s_modeNames[powerMode]);
        APP_PrintRunFrequency(1);
    }
    else
    {
        assert(0);
    }
}

/*
 * Check whether could switch to target power mode from current mode.
 * Return true if could switch, return false if could not switch.
 */
bool APP_CheckPowerMode(lpm_power_mode_t originPowerMode, lpm_power_mode_t targetPowerMode)
{
    bool modeValid = true;

    /* If current mode is Lowpower run mode, the target mode should not be system idle mode. */
    if ((originPowerMode == LPM_PowerModeLowPowerRun) && (targetPowerMode == LPM_PowerModeSysIdle))
    {
        PRINTF("Low Power Run mode can't enter System Idle mode.\r\n");
        modeValid = false;
    }

    /* Don't need to change power mode if current mode is already the target mode. */
    if (originPowerMode == targetPowerMode)
    {
        PRINTF("Already in the target power mode.\r\n");
        modeValid = false;
    }

    return modeValid;
}

void APP_PowerPreSwitchHook(lpm_power_mode_t targetMode)
{
    if (targetMode == LPM_PowerModeSNVS)
    {
        PRINTF("Now shutting down the system...\r\n");
    }

    if (targetMode > LPM_PowerModeRunEnd)
    {
        /* Wait for debug console output finished. */
        while (!(kLPUART_TransmissionCompleteFlag & LPUART_GetStatusFlags((LPUART_Type *)BOARD_DEBUG_UART_BASEADDR)))
        {
        }
        DbgConsole_Deinit();

        /*
         * Set pin for current leakage.
         * Debug console RX pin: Set pinmux to GPIO input.
         * Debug console TX pin: Don't need to change.
         */
        ConfigUartRxPinToGpio();
    }
}

void APP_PowerPostSwitchHook(lpm_power_mode_t targetMode)
{
    if (targetMode > LPM_PowerModeRunEnd)
    {
        /*
         * Debug console RX pin is set to GPIO input, need to re-configure pinmux.
         * Debug console TX pin: Don't need to change.
         */
        ReConfigUartRxPin();
        BOARD_InitDebugConsole();

        /* recover to previous run mode */
        switch (APP_GetRunMode())
        {
            case LPM_PowerModeOverRun:
                LPM_OverDriveRun();
                break;
            case LPM_PowerModeFullRun:
                LPM_FullSpeedRun();
                break;
            case LPM_PowerModeLowSpeedRun:
                LPM_LowSpeedRun();
                break;
            case LPM_PowerModeLowPowerRun:
                LPM_LowPowerRun();
                break;
            default:
                break;
        }
    }
    else
    {
        /* update current run mode */
        APP_SetRunMode(targetMode);
    }
}

void APP_PowerModeSwitch(lpm_power_mode_t targetPowerMode)
{
    switch (targetPowerMode)
    {
        case LPM_PowerModeOverRun:
            LPM_OverDriveRun();
            break;
        case LPM_PowerModeFullRun:
            LPM_FullSpeedRun();
            break;
        case LPM_PowerModeLowSpeedRun:
            LPM_LowSpeedRun();
            break;
        case LPM_PowerModeLowPowerRun:
            LPM_LowPowerRun();
            break;
        case LPM_PowerModeSysIdle:
            LPM_PreEnterWaitMode();
            LPM_EnterSystemIdle();
            LPM_ExitSystemIdle();
            LPM_PostExitWaitMode();
            break;
        case LPM_PowerModeLPIdle:
            LPM_PreEnterWaitMode();
            LPM_EnterLowPowerIdle();
            LPM_ExitLowPowerIdle();
            LPM_PostExitWaitMode();
            break;
        case LPM_PowerModeSuspend:
            LPM_PreEnterStopMode();
            LPM_EnterSuspend();
            LPM_PostExitStopMode();
            break;
#if (HAS_WAKEUP_PIN)
        case LPM_PowerModeSNVS:
            LPM_EnterSNVS();
            break;
#endif
        default:
            assert(false);
            break;
    }
}

int SLN_PowerModeSwitch(lpm_power_mode_t targetPowerMode)
{
	assert(targetPowerMode == LPM_PowerModeSNVS);

    bool needSetWakeup; /* Need to set wakeup. */

    s_targetPowerMode = targetPowerMode;

    if (s_targetPowerMode <= LPM_PowerModeEnd)
    {
        /* If could not set the target power mode, loop continue. */
        if (!APP_CheckPowerMode(s_curRunMode, s_targetPowerMode))
        {
            return -1;
        }

        /* If target mode is run mode, don't need to set wakeup source. */
        if (s_targetPowerMode <= LPM_PowerModeLowPowerRun)
        {
            needSetWakeup = false;
        }
        else
        {
            needSetWakeup = true;
        }

        if (needSetWakeup)
        {
            APP_GetWakeupConfig(s_targetPowerMode);
            APP_SetWakeupConfig(s_targetPowerMode);
        }

        APP_PowerPreSwitchHook(s_targetPowerMode);
        APP_PowerModeSwitch(s_targetPowerMode);
        APP_PowerPostSwitchHook(s_targetPowerMode);
    }
    PRINTF("\r\nNext loop\r\n");

    return 0;
}

/*!
 * @brief main demo function.
 */
int SLN_LPM_Init(void)
{
    LOGD("[LPM]:start\r\n");

    /* Define the init structure for the input switch pin */
    gpio_pin_config_t config = {
        kGPIO_DigitalInput,
        0,
        kGPIO_IntRisingEdge,
    };

#if HAS_WAKEUP_PIN
    /* Init input switch GPIO. */
    GPIO_PinInit(APP_WAKEUP_BUTTON_GPIO, APP_WAKEUP_BUTTON_GPIO_PIN, &config);
#endif

//    gpt_config_t gptConfig;
//    /* Init GPT for wakeup */
//    GPT_GetDefaultConfig(&gptConfig);
//    gptConfig.clockSource     = kGPT_ClockSource_LowFreq; /* 32K RTC OSC */
//    gptConfig.enableMode      = true;                     /* Don't keep counter when stop */
//    gptConfig.enableRunInDoze = true;
//    /* Initialize GPT module */
//    GPT_Init(APP_WAKEUP_GPT_BASE, &gptConfig);
//    GPT_SetClockDivider(APP_WAKEUP_GPT_BASE, 1);

    LPM_Init();

    /* Set power mode to over run after power on */
//    APP_SetRunMode(LPM_PowerModeOverRun);

    return 0;
}

