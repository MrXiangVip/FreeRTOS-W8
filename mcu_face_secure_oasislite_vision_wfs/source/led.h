/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef _LED_H_
#define _LED_H_

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * GLobal  Declarations
 ******************************************************************************/
/*! @brief LED Colors */
typedef enum _rgb_led_color
{
    LED_COLOR_OFF,              /*!< LED Off */
    LED_COLOR_RED,              /*!< LED Red Color */
    LED_COLOR_GREEN,            /*!< LED Green Color */
    LED_COLOR_BLUE,             /*!< LED Blue Color */
} rgb_led_color_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Set RGB color from enum
 *
 * @param color Target color to set RGB LED
 */
void RGB_LED_SetColor(rgb_led_color_t color);

#if defined(__cplusplus)
}
#endif

#endif /* _LED_H_ */
