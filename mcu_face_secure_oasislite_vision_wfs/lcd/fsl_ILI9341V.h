/*
 * Copyright 2020 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.d
 *
 * Created by: NXP China Solution Team.
 */

#ifndef FSL_ILI9341V_H_
#define FSL_ILI9341V_H_

#include "fsl_common.h"

typedef void (*ILI9341V_send_byte_t)(uint8_t);
typedef void (*ILI9341V_delay_ms_t)(uint32_t);

void ILI9341V_Init(ILI9341V_send_byte_t _writeData, ILI9341V_send_byte_t _writeCommand, ILI9341V_delay_ms_t _delay_ms);


#endif /* FSL_ILI9341V_H_ */
