/*
 * Copyright 2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.d
 *
 * Created by: NXP China Solution Team.
 */

#include "fsl_pjdisp.h"
#if defined(FSL_RTOS_FREE_RTOS)
#include "FreeRTOS.h"
#include "task.h"
#endif

#include "fsl_disp_spi.h"

typedef enum _screen_model
{
    SCREEN_MODEL_TYPE_NONE = 0, /*!< Default  */
    SCREEN_MODEL_ZMA_IPS,              /*!< 众铭安科技 */
    SCREEN_MODEL_JSX_IPS,      /*!< 靖顺兴SPI */
    SCREEN_MODEL_JSX_TN,       /*!< 靖顺兴TN  */
} screen_model_t;
/*
CS GPIO3_IO27
SCL GPIO2_IO20
SDA GPIO2_IO21
RESET GPIO1_IO14
*/

#define CS_HI   GPIO2->DR |= (1U<<20)
#define CS_LO   GPIO2->DR &= ~(1U<<20)//GPIO2 20
#define SCL_HI  GPIO2->DR |= (1U<<23)
#define SCL_LO  GPIO2->DR &= ~(1U<<23)//GPIO2 23 
#define SDA_HI  GPIO2->DR |= (1U<<22)
#define SDA_LO  GPIO2->DR &= ~(1U<<22)//GPIO2 22 
#define RST_HI  GPIO1->DR |= (1U<<8)//GPIO1->DR |= (1U<<8)
#define RST_LO  GPIO1->DR &= ~(1U<<8)//GPIO1->DR &= ~(1U<<8)//GPIO 1 8 
//#define BL_HI   GPIO3->DR |= (1U<<26)     
//#define BL_LO   GPIO3->DR &= ~(1U<<26)//GPIO3 26   GPIO_EMC_40
#define BL_HI   GPIO3->DR |= (1U<<27)     
#define BL_LO   GPIO3->DR &= ~(1U<<27)//GPIO3 27   GPIO_EMC_41

typedef void (*ST77889V2_send_byte_t)(uint8_t);
typedef void (*ST77889V2_delay_ms_t)(uint32_t);

//
static screen_model_t  screenModel = SCREEN_MODEL_ZMA_IPS;

static void ST77889V2_Delay_Ms (uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

static void Delay(uint32_t ticks)
{
    //ST77889V2_Delay_Ms(ticks);
    while(ticks--)
    {
    	__NOP();
    }
}

#if 0
static void ST77889V2_Write_Cmd(uint8_t cmd)
{
	unsigned char n,m;
    
    CS_LO;   
    Delay(10);
    SCL_LO;
    SDA_LO;
    Delay(10);
    SCL_HI;
    Delay(10);
    m=0x80;
    for(n=0; n<8; n++)
    {           
           if(cmd&m)

            {    
                SCL_LO;
		  Delay(10);		
                SDA_HI;
		  Delay(10);		
                SCL_HI;
		  Delay(10);
            }
            else
            {     
                SCL_LO;
       	  Delay(10);	
                SDA_LO;
		  Delay(10);		
                SCL_HI;
		  Delay(10);
            }            
            m=m>>1;
            Delay(10);
           
     }  
     Delay(10);
     CS_HI;   	
     Delay(100);
	
}
static void ST77889V2_Write_Dat(uint8_t data)
{
    unsigned char n,m;
    
    CS_LO;   
    Delay(10);
    SCL_LO;
    SDA_HI;
    Delay(10);
    SCL_HI;
    Delay(10);
    m=0x80;
    for(n=0; n<8; n++)
    {           
           if(data&m)

            {    
                SCL_LO;
		  Delay(10);		
                SDA_HI;
		  Delay(10);		
                SCL_HI;
		  Delay(10);
            }
            else
            {     
                SCL_LO;
       	  Delay(10);	
                SDA_LO;
		  Delay(10);		
                SCL_HI;
		  Delay(10);
            }            
            m=m>>1;
            Delay(10);
           
     }  
     Delay(10);
     CS_HI;   	
     Delay(100);
}
#else
static void ST77889V2_Write_Cmd(uint8_t Data)
{
//    uint8_t tmp_data[2] = {0x00, 0x00};
//    tmp_data[1] = Data;
    uint16_t tmp_data = Data|0x000;
    Disp_Spi_Send_Data(&tmp_data, 2, 0);
}

static void ST77889V2_Write_Dat(uint8_t Data)
{
//    uint8_t tmp_data[2] = {0x01, 0x00};
//    tmp_data[1] = Data;
    uint16_t tmp_data = Data|0x100;
    Disp_Spi_Send_Data(&tmp_data, 2, 0);
}
#endif

static void ST77889V2_Init(ST77889V2_send_byte_t _writeData, ST77889V2_send_byte_t _writeCommand, ST77889V2_delay_ms_t _delay_ms)
{
    _writeCommand(0x11);     

    _delay_ms(120);                

    _writeCommand(0x36);     
    _writeData(0x00);

    _writeCommand(0x3A);     
    _writeData(0x55);  

    _writeCommand(0xB2);     
    _writeData(0x0C);    
    _writeData(0x0C);   
    _writeData(0x00);   
    _writeData(0x33);   
    _writeData(0x33);   

    _writeCommand(0xB7);     
    _writeData(0x71);   

    _writeCommand(0xBB);
    if( screenModel == SCREEN_MODEL_ZMA_IPS ) {
        _writeData(0x3B);
    }else{
        _writeData(0x3E); //0X3e
    }

    _writeCommand(0xC0);     
    _writeData(0x2C);   

    _writeCommand(0xC2);     
    _writeData(0x01);   

    _writeCommand(0xC3);     
    _writeData(0x08);   //0X08

    _writeCommand(0xC4);     
    _writeData(0x20);   

    _writeCommand(0xC6);     
    _writeData(0x0F);   

    _writeCommand(0xD0);     
    _writeData(0xA4);   
    _writeData(0xA1);   

    _writeCommand(0xD6);     
    _writeData(0xA1);   
    _writeCommand(0xE0);     
    _writeData(0xD0);
    _writeData(0x08);
    _writeData(0x0A);
    _writeData(0x0D);
    _writeData(0x0B);
    _writeData(0x07);
    _writeData(0x21);
    _writeData(0x33);
    _writeData(0x39);
    _writeData(0x39);
    _writeData(0x16);
    _writeData(0x16);
    _writeData(0x1F);
    _writeData(0x3C);  

    _writeCommand(0xE1);     
    _writeData(0xD0);
    _writeData(0x00);
    _writeData(0x03);
    _writeData(0x01);
    _writeData(0x00);
    _writeData(0x10);
    _writeData(0x21);
    _writeData(0x32);
    _writeData(0x38);
    _writeData(0x16);
    _writeData(0x14);
    _writeData(0x14);
    _writeData(0x20);
    _writeData(0x3D);
    /*_writeCommand(0xb0);
        _writeData(0x11);

         _writeCommand(0xb1);
        _writeData(0x42);*/

    _writeCommand(0xb0);
    _writeData(0x11);
    _writeData(0xF0);


    _writeCommand(0xb1);
    _writeData(0x42);
    _writeData(0x02);
    _writeData(0x14);

    _writeCommand(0x21);
    _writeCommand(0x29);
    _writeCommand(0x2C);
    _delay_ms(120); 

}

static void ST77889V3_Init(ST77889V2_send_byte_t _writeData, ST77889V2_send_byte_t _writeCommand, ST77889V2_delay_ms_t _delay_ms)
{

    uint16_t k,j;
    _writeCommand(0x11);

    _delay_ms(120);

    _writeCommand(0x36);
    _writeData(0x00);

    _writeCommand(0x3A);
    _writeData(0x55);

    _writeCommand(0xB2);
    _writeData(0x0C);
    _writeData(0x0C);
    _writeData(0x00);
    _writeData(0x33);
    _writeData(0x33);

    _writeCommand(0xB7);
    _writeData(0x75);

    _writeCommand(0xBB);
    _writeData(0x1F); //0X3B

    _writeCommand(0xC0);
    _writeData(0x2C);

    _writeCommand(0xC2);
    _writeData(0x01);

    _writeCommand(0xC3);
    _writeData(0x08);   //0X08

    _writeCommand(0xC4);
    _writeData(0x20);

    _writeCommand(0xC6);
    _writeData(0x0F);

    _writeCommand(0xD0);
    _writeData(0xA4);
    _writeData(0xA1);

    _writeCommand(0xE0);
    _writeData(0xD0);
    _writeData(0x08);
    _writeData(0x0A);
    _writeData(0x0D);
    _writeData(0x0B);
    _writeData(0x07);
    _writeData(0x21);
    _writeData(0x33);
    _writeData(0x39);
    _writeData(0x39);
    _writeData(0x16);
    _writeData(0x16);
    _writeData(0x1F);
    _writeData(0x3C);

    _writeCommand(0xE1);
    _writeData(0xD0);
    _writeData(0x0B);
    _writeData(0x13);
    _writeData(0x0A);
    _writeData(0x0A);
    _writeData(0x04);
    _writeData(0x3C);
    _writeData(0x54);
    _writeData(0x4A);
    _writeData(0x08);
    _writeData(0x16);
    _writeData(0x17);
    _writeData(0x1B);
    _writeData(0x1E);

    _writeCommand(0xb0);
    _writeData(0x11);
    _writeData(0xF0);


    _writeCommand(0xb1);
    _writeData(0xC2);
    _writeData(0x02);
    _writeData(0x14);

    _writeCommand(0x29);
    //_delay_ms(150);

    _writeCommand(0x2a);
    _writeData(0x00);
    _writeData(0x00);
    _writeData(0x00);
    _writeData(0xef);

    _writeCommand(0x2b);
    _writeData(0x00);
    _writeData(0x00);
    _writeData(0x01);
    _writeData(0x3f);

    _writeCommand(0x2C);

    for (k= 0;k<240; k++)
    {
        for (j= 0;j<320; j++)
        {
            _writeData(0x00);
            _writeData(0x00);
        }

    }

}

int PJDisp_Init(void)
{
    RST_HI;
    ST77889V2_Delay_Ms(20);
    RST_LO;
    ST77889V2_Delay_Ms(150);
    RST_HI;
    ST77889V2_Delay_Ms(120);
    Disp_Spi_Init(10000000U);

    if( screenModel ==  SCREEN_MODEL_JSX_TN ){

    	ST77889V3_Init(ST77889V2_Write_Dat, ST77889V2_Write_Cmd, ST77889V2_Delay_Ms);
    }else{
        ST77889V2_Init(ST77889V2_Write_Dat, ST77889V2_Write_Cmd, ST77889V2_Delay_Ms);
    }
    return 1;
}

int PJDisp_SendFrame(uint32_t DispBuffer)
{
    LCDIF->NEXT_BUF = DispBuffer;
    return 1;
}

int PJDisp_TurnOnBacklight(void)
{
    BL_HI;
    return 0;
}

int PJDisp_TurnOffBacklight(void)
{

    BL_LO;
    return 0;
}
