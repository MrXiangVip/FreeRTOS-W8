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

#ifndef _UTIL_H_
#define _UTIL_H_

#include "fsl_common.h"

#define RGB565_RED      0xf800
#define RGB565_GREEN    0x07e0
#define RGB565_BLUE     0x001f

void util_RGB565ToRGB888(uint16_t n565Color, uint8_t* pR, uint8_t* pG, uint8_t* pB);
void util_RGB888ToRGB565(uint16_t* pOut, int oW, int oH, uint8_t* pIn, int iW, int iH);
void util_RGB2GRAY(const unsigned char* rgb, int w, int h, unsigned char* gray);
void util_GRAY16ToRGB565(const uint16_t* gray16, int w, int h, uint16_t* rgb565);
void util_GRAY16ToRGB888(const uint16_t* gray16, int w, int h, uint8_t* rgb);
void util_Raw16ToDepth12(const uint16_t* raw16, int w, int h, uint16_t* depth12);
void util_Crop_C3(unsigned char* src, int srcw, int srch, unsigned char* dst, int dstw, int dsth, int top, int left, int elemsize);

int FileExist(const char* filename);
#endif /* _FACEREC_UTIL_H_ */
