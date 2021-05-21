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

#include "fsl_ILI9341V.h"
#include "GUI.h"

void ILI9341V_Init(ILI9341V_send_byte_t _writeData, ILI9341V_send_byte_t _writeCommand, ILI9341V_delay_ms_t _delay_ms)
{
    _writeCommand(0xE8);
    _writeData(0x84);
    _writeData(0x00);
    _writeData(0x7a);

    _writeCommand(0xCF);
    _writeData(0x00);
    _writeData(0x81);
    _writeData(0X30);

    _writeCommand(0xEA);
    _writeData(0x00);
    _writeData(0x00);

    _writeCommand(0xC0);   //Power control
    _writeData(0x21);	   //VRH[5:0]

    _writeCommand(0xC1);   //Power control
    _writeData(0x11);	   //SAP[2:0];BT[3:0]

    _writeCommand(0xC5);   //VCM control
    _writeData(0x41);
    _writeData(0x3c);

    _writeCommand(0xC7);    //VCM control2
    _writeData(0xac);

    _writeCommand(0x36);    // Memory Access Control
    _writeData(0x48);

    _writeCommand(0x3a);    //
    _writeData(0x66);

    _writeCommand(0xF2);    // 3Gamma Function Disable
    _writeData(0x00);

    _writeCommand(0x26);    //Gamma curve selected
    _writeData(0x01);

    _writeCommand(0xE0);    //Set Gamma
    _writeData(0x0F);
    _writeData(0x38);
    _writeData(0x32);
    _writeData(0x0C);
    _writeData(0x0e);
    _writeData(0x07);
    _writeData(0x4f);
    _writeData(0X98);
    _writeData(0x39);
    _writeData(0x07);
    _writeData(0x11);
    _writeData(0x04);
    _writeData(0x13);
    _writeData(0x11);
    _writeData(0x00);

    _writeCommand(0xE1);    //Set Gamma
    _writeData(0x00);
    _writeData(0x0d);
    _writeData(0x0e);
    _writeData(0x04);
    _writeData(0x12);
    _writeData(0x08);
    _writeData(0x31);
    _writeData(0x78);
    _writeData(0x46);
    _writeData(0x08);
    _writeData(0x10);
    _writeData(0x0d);
    _writeData(0x2b);
    _writeData(0x33);
    _writeData(0x0F);

    _writeCommand(0xb0);    //Set
    _writeData(0xc0);

    _writeCommand(0xF6);    //Set
    _writeData(0x01);
    _writeData(0x00);
    _writeData(0x06);

    _writeCommand(0x11);    //Exit Sleep
    _delay_ms(800000);
    _writeCommand(0x29);    //Display on
    _delay_ms(10000);

}
