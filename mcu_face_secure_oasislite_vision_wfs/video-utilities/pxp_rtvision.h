
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

#ifndef _PXP_RTVISION_H_
#define _PXP_RTVISION_H_

#if RTVISION_BOARD

#include "fsl_elcdif.h"
#include "fsl_pxp.h"

#if RTFFI_JP_DISP_DRIVER
#include "fsl_jdisp.h"
#endif
#if RTFFI_RV_DISP_DRIVER
#include "fsl_rvdisp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
void APP_PXP_Start(void);
int PXP_SendQMsg(void* msg);

void ConvertRGB2YUV(uint16_t *sourceLcdBuffer, uint16_t *destLcdBuffer);
void RGB5652YUV(uint16_t source1, uint16_t source2, uint16_t *dest1, uint16_t *dest2);

#ifdef __cplusplus
}
#endif
#endif
