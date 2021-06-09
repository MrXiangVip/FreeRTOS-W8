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
#include <string>

#include "util.h"
#include "commondef.h"

void util_RGB565ToRGB888(uint16_t n565Color, uint8_t* pR, uint8_t* pG, uint8_t* pB)
{
    uint8_t r   = (n565Color & RGB565_RED) >> 11;//>> 8;
    uint8_t g = (n565Color & RGB565_GREEN) >> 5;//>> 3;
    uint8_t b  = (n565Color & RGB565_BLUE) ;//<< 3;
    r = (r * 527 + 23) >> 6;
    g = (g * 259 + 33) >> 6;
    b = (b * 527 + 23) >> 6;
    *pR = r;
    *pG = g;
    *pB = b;
}

void util_RGB888ToRGB565(uint16_t* pOut, int oW, int oH, uint8_t* pIn, int iW, int iH)
{
    int x_offset = (oW - iW) / 2;
    int y_offset = (oH - iH) / 2;
    memset(pOut, 0x0, (oW * oH * 2));

    for (int i = 0; i < iH; i++) {
        for (int j = 0; j < iW; j++) {
            pOut[((i + y_offset) * oH + j + x_offset)] =  ((uint16_t)(pIn[(i * iH + j) * 3] & 0xF8) << 8)
                    | ((uint16_t)(pIn[(i * iH + j) * 3 + 1] & 0xFC) << 3)
                    | ((uint16_t)pIn[(i * iH + j) * 3 + 2] >> 3);
        }
    }
}

void util_Crop_C3(unsigned char* src, int srcw, int srch, unsigned char* dst, int dstw, int dsth, int top, int left, int elemsize)
{
    int dststride = dstw * elemsize;
    int srcstride = srcw * elemsize;

    const unsigned char* ptr = src + top * srcstride + left * elemsize;
    unsigned char* outptr = dst;

    for (int y = 0; y < dsth; y++) {
        if (dststride < 12) {
            for (int x = 0; x < dststride; x++) {
                outptr[x] = ptr[x];
            }
        } else {
            memcpy(outptr, ptr, dststride);
        }

        outptr += dststride;
        ptr += srcstride;
    }
}

void util_RGB2GRAY(const unsigned char* rgb, int w, int h, unsigned char* gray)
{
    unsigned char* ptr0 = gray;

    int remain = w * h;

    int g;

    for (; remain > 0; remain--) {
        g = (rgb[2] * 7472 + rgb[1] * 38469 + rgb[0] * 19595) >> 16;
        *ptr0 = g;
        rgb += 3;
        ptr0++;
    }
}

void util_GRAY16ToRGB565(const uint16_t* gray16, int w, int h, uint16_t* rgb565)
{
	uint16_t* ptr0 = rgb565;
    int remain = w * h;
    uint16_t temp;
    for (; remain > 0; remain--) {
        temp = (*gray16 & 0xF800) | ((*gray16 & 0xFC00)>> 5) | ((*gray16 & 0xF800) >> 11);
        *ptr0 = temp;
        gray16 ++;
        ptr0 ++;
    }
}

void util_GRAY16ToRGB888(const uint16_t* gray16, int w, int h, uint8_t* rgb)
{
    uint8_t* ptr0 = rgb;
    int remain = w * h;
    uint8_t temp;
    for (; remain > 0; remain--) {
        temp = *gray16 >> 8;
        *ptr0 = temp;
        *(ptr0+1) = temp;
        *(ptr0+2) = temp;
        gray16 ++;
        ptr0 +=3;
    }
}

void util_Raw16ToDepth12(const uint16_t* raw16, int w, int h, uint16_t* depth12)
{
    uint16_t* ptr0 = depth12;
    int remain = w * h;
    for (; remain > 0; remain--) {
       *depth12 = *raw16 >> 4;
    }
}

uint8_t StrGetUInt8( const uint8_t * i_pSrc )
{
    uint8_t u8Rtn = 0;
    memcpy(&u8Rtn, i_pSrc, 1);
    return u8Rtn;
}

uint16_t StrGetUInt16( const uint8_t * i_pSrc )
{
    uint16_t u16Rtn = 0;
    for ( uint8_t i=0; i<2; i++ )
    {
        uint16_t u16Temp = 0;
        memcpy(&u16Temp, i_pSrc, 1);
        u16Rtn += u16Temp << (2-1-i)*8;
        i_pSrc++;
    }
    return u16Rtn;
}

uint32_t StrGetUInt32( const uint8_t * i_pSrc )
{
    uint32_t u32Rtn = 0;
    for ( uint8_t i=0; i<4; i++ )
    {
        uint32_t u32Temp = 0;
        memcpy(&u32Temp, i_pSrc, 1);
        u32Rtn += u32Temp << (4-1-i)*8;
        i_pSrc++;
    }
    return u32Rtn;
}

void StrSetUInt8( uint8_t * io_pDst, const uint8_t i_u8Src )
{
    memcpy(io_pDst, &i_u8Src, 1);
    io_pDst++;
}

void StrSetUInt16( uint8_t * io_pDst, const uint16_t i_u16Src )
{
    for ( uint8_t i=0; i<2; i++ )
    {
        uint8_t u8Temp = (i_u16Src >> (2-1-i)*8) & 0xFF;
        memcpy(io_pDst, &u8Temp, 1);
        io_pDst++;
    }
}

void StrSetUInt32( uint8_t * io_pDst, const uint32_t i_u32Src )
{
    for ( uint8_t i=0; i<4; i++ )
    {
        uint8_t u8Temp = (i_u32Src >> (4-1-i)*8) & 0xFF;
        memcpy(io_pDst, &u8Temp, 1);
        io_pDst++;
    }
}

/*
// C prototype : void StrToHex(byte *pbDest, char *pszSrc, int nLen)
// parameter(s): [OUT] pbDest - 输出缓冲区
//	[IN] pszSrc - 字符串
//	[IN] nLen - 16进制数的字节数(字符串的长度/2)
// return value:
// remarks : 将字符串转化为16进制数
*/
void StrToHex(unsigned char *pbDest, char *pszSrc, int nLen)
{
    int i;
    char h1, h2;
    unsigned char s1, s2;
    for (i = 0; i < nLen; i++)
    {
        h1 = pszSrc[2 * i];
        h2 = pszSrc[2 * i + 1];

        s1 = toupper(h1) - 0x30;
        if (s1 > 9)
            s1 -= 7;

        s2 = toupper(h2) - 0x30;
        if (s2 > 9)
            s2 -= 7;

        pbDest[i] = s1 * 16 + s2;
    }
}

/*
// C prototype : void HexToStr(char *pszDest, byte *pbSrc, int nLen)
// parameter(s): [OUT] pszDest - 存放目标字符串
//	[IN] pbSrc - 输入16进制数的起始地址
//	[IN] nLen - 16进制数的字节数
// return value:
// remarks : 将16进制数转化为字符串
*/
void HexToStr(char *pszDest, unsigned char *pbSrc, int nLen)
{
    char	ddl, ddh;
    int i;
    for (i = 0; i < nLen; i++)
    {
        ddh = 48 + pbSrc[i] / 16;
        ddl = 48 + pbSrc[i] % 16;
        if (ddh > 57) ddh = ddh + 7;
        if (ddl > 57) ddl = ddl + 7;
        pszDest[i * 2] = ddh;
        pszDest[i * 2 + 1] = ddl;
    }

    pszDest[nLen * 2] = '\0';

}


