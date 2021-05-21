/*
 * Copyright 2019 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.d
 *
 * Created by: NXP China Solution Team.
 */

#ifndef LCD_FSL_ST7789_H_
#define LCD_FSL_ST7789_H_

#include "fsl_common.h"

typedef void (*st7789_send_byte_t)(uint8_t);
typedef void (*st7789_delay_ms_t)(uint32_t);

void ST7789_Init(st7789_send_byte_t _writeData, st7789_send_byte_t _writeCommand, st7789_delay_ms_t _delay_ms);


#endif /* LCD_FSL_ST7789_H_ */
