/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "led.h"
#include "fsl_pcal.h"
#include "board.h"

/*******************************************************************************
 * Start Implementation
 ******************************************************************************/
void RGB_LED_SetColor(rgb_led_color_t color)
{
#if RTVISION_BOARD
    switch (color)
    {
        case LED_COLOR_OFF:
            set_iox_port_pin(OUTPUT_PORT2, RGB_LED_RED_PIN, 1);
            set_iox_port_pin(OUTPUT_PORT2, RGB_LED_GREEN_PIN, 1);
            set_iox_port_pin(OUTPUT_PORT2, RGB_LED_BLUE_PIN, 1);
            break;
        case LED_COLOR_RED:
            set_iox_port_pin(OUTPUT_PORT2, RGB_LED_RED_PIN, 0);
            set_iox_port_pin(OUTPUT_PORT2, RGB_LED_GREEN_PIN, 1);
            set_iox_port_pin(OUTPUT_PORT2, RGB_LED_BLUE_PIN, 1);
            break;
        case LED_COLOR_GREEN:
            set_iox_port_pin(OUTPUT_PORT2, RGB_LED_GREEN_PIN, 0);
            set_iox_port_pin(OUTPUT_PORT2, RGB_LED_RED_PIN, 1);
            set_iox_port_pin(OUTPUT_PORT2, RGB_LED_BLUE_PIN, 1);
            break;
        case LED_COLOR_BLUE:
            set_iox_port_pin(OUTPUT_PORT2, RGB_LED_BLUE_PIN, 0);
            set_iox_port_pin(OUTPUT_PORT2, RGB_LED_RED_PIN, 1);
            set_iox_port_pin(OUTPUT_PORT2, RGB_LED_GREEN_PIN, 1);
            break;
        default: break;
    }
#endif
}
