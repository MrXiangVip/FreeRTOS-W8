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

#include "fsl_st7789.h"
#include "GUI.h"

void ST7789_Init(st7789_send_byte_t _writeData, st7789_send_byte_t _writeCommand, st7789_delay_ms_t _delay_ms)
{
//Driving ability Setting
    _writeCommand(0x11);
    _delay_ms(120);
//------------------------------display and color format setting--------------------------------//
    _writeCommand(0x36);
    _writeData(0x00);
    _writeCommand(0x3a);
    _writeData(0x55);

    _writeCommand(0xb0);
    _writeData(0x00);
    _writeData(0xf8);
//--------------------------------ST7789V Frame rate setting----------------------------------//
    _writeCommand(0xb2);
    _writeData(0x0c);
    _writeData(0x0c);
    _writeData(0x00);
    _writeData(0x33);
    _writeData(0x33);
    _writeCommand(0xb7);
    _writeData(0x35);
//---------------------------------ST7789V Power setting--------------------------------------//
    _writeCommand(0xbb);
    _writeData(0x28);
    _writeCommand(0xc0);
    _writeData(0x2c);
    _writeCommand(0xc2);
    _writeData(0x01);
    _writeCommand(0xc3);
    _writeData(0x10);
    _writeCommand(0xc4);
    _writeData(0x20);
    _writeCommand(0xc6);
    _writeData(0x0f);
    _writeCommand(0xd0);
    _writeData(0xa4);
    _writeData(0xa1);
//--------------------------------ST7789V gamma setting---------------------------------------//
    _writeCommand(0xe0);
    _writeData(0xd0);
    _writeData(0x00);
    _writeData(0x02);
    _writeData(0x07);
    _writeData(0x0a);
    _writeData(0x28);
    _writeData(0x32);
    _writeData(0x44);
    _writeData(0x42);
    _writeData(0x06);
    _writeData(0x0e);
    _writeData(0x12);
    _writeData(0x14);
    _writeData(0x17);
    _writeCommand(0xe1);
    _writeData(0xd0);
    _writeData(0x00);
    _writeData(0x02);
    _writeData(0x07);
    _writeData(0x0a);
    _writeData(0x28);
    _writeData(0x31);
    _writeData(0x54);
    _writeData(0x47);
    _writeData(0x0e);
    _writeData(0x1c);
    _writeData(0x17);
    _writeData(0x1b);
    _writeData(0x1e);

    _writeCommand(0x13);
    _writeCommand(0x29);
}
