/****************************************************************************
Copyright (C), 2014, Mingv150, All rights reserved
FileName: /Driver/Lcd/Lcd.c
Description:  
Author:  mingv150@163.com
Version:  
Changelog: 
*****************************************************************************/

#define _Lcd_C_
/****************************************************************************
Global header file:
*****************************************************************************/
#include <htc.h>
#include "../../Common.h"
#include "../../Model.h"
#include "../../oopc.h"

/****************************************************************************
Local header file:
*****************************************************************************/
#include "Lcd.h"

/****************************************************************************
Global Data Structure:
*****************************************************************************/
t_byte disnum1;
t_byte disnum2;
t_byte disnum3;
t_byte disnum4;
t_byte dissym1;
t_byte dissym2;

const unsigned char SegNum[]=
{
  0x3f,                                                                        //'0'
  0x06,                                                                        //'1'
  0x5b,                                                                        //'2'
  0x4f,                                                                        //'3'
  0x66,                                                                        //'4'
  0x6d,                                                                        //'5'
  0x7d,                                                                        //'6'
  0x07,                                                                        //'7'
  0x7f,                                                                        //'8'
  0x6f,                                                                        //'9'
  0x77,                                                                        //'A'
  0x7c,                                                                        //'b'
  0x39,                                                                        //'C'
  0x5e,                                                                        //'d'
  0x79,                                                                        //'E'
  0x71,                                                                        //'F'
  0x00,                                                                        //' '
  0x54,                                                                        //'n'//0x37
  
};

/****************************************************************************
Function: Name
Description:通用LCD操作延时函数
Input:
Output:
Notes:
*****************************************************************************/
void Lcd_Init(void)
{
    LCDEN = 0; //turn off the lcd
    SLPEN = 0; //no lcd drive in sleep mode
    LCDCST = 0x05;
    LCDIRE = 1;
    LCDIRS = 0;

    CS1=1, CS0=0;  //0b00 -> Fosc / 8192. 0b01 = Timer1/32, 0b1x = LFINTOSC(31khz)/32
    LMUX1=1;
    LMUX0=1; //0b00 -> static(COM0); 0b01 -> 1/2 (COM0..1); 0b10 -> 1/3 (COM0..2); 0b11 -> 1/4 (COM0..3)

    //set up lcdps
    WFT = 0; //1=wave form change on the boundary, 0=in between

    switch (LCD_COM)
    {
        case LCD_COM0:                BIASMD = 0;        break;        //has to be set to 0
        case LCD_COM1:                BIASMD = 1;       break;        //1=1/2 bias. 0=1/3 bias
        case LCD_COM2:                BIASMD = 0;        break;        //1=1/3 bias. 0=1/2 bias
        case LCD_COM3:                BIASMD = 0;        break;        //has to be set to 0
    }
        //LCDA=1, WA=1;                                                //set the status bits

        //set up the prescaler
        LP3=(LCD_PS & 0x08) >> 3;
        LP2=(LCD_PS & 0x04) >> 2;
        LP1=(LCD_PS & 0x02) >> 1;
        LP0=(LCD_PS & 0x01) >> 0;                                //lcd prescaler. 1:1(0b0000) -> 16:1 (0b1111)

        //LCDREF = 0b10000110;
        LCDREF = 0b10000000;

        //enable the segments - need customization
        LCDSE0 = 0b01001110;                                                //enable lcd0..7
        LCDSE1 = 0b00001111;                                                //enable lcd8..15
        LCDSE2 = 0b11000001;                                                //enable lcd16..23

        LCDRL = 0b11110100;
        LCDIE = 0;
        LCDEN = 1;//enable the lcd
        
    disnum1.byte = 0;
    disnum2.byte = 0;
    disnum3.byte = 0;
    disnum4.byte = 0;
    dissym1.byte = 0;
    dissym2.byte = 0;
}


/****************************************************************************
Function: Name
Description:
Input:
Output:
Notes:
*****************************************************************************/
void Lcd_DisplayUpdate(void)
{
    //com0
    LCDDATA9 = disnum2.bits.bit0<<6 | disnum2.bits.bit5<<3 | disnum1.bits.bit5<<2 | disnum1.bits.bit0<<1;
    LCDDATA10 = disnum3.bits.bit0<<3 | disnum4.bits.bit5<<2 | disnum4.bits.bit0<<1 | dissym1.bits.bit4<<0;
    LCDDATA11 = dissym1.bits.bit5<<7 | disnum3.bits.bit5<<0;
    //com1
    LCDDATA6 = disnum2.bits.bit1<<6 | disnum2.bits.bit6<<3 | disnum1.bits.bit6<<2 | disnum1.bits.bit1<<1;
    LCDDATA7 = disnum3.bits.bit1<<3 | disnum4.bits.bit6<<2 | disnum4.bits.bit1<<1 | dissym1.bits.bit3<<0;
    LCDDATA8 = dissym2.bits.bit3<<7 | dissym2.bits.bit4<<6 | disnum3.bits.bit6<<0;
    //com2
    LCDDATA3 = disnum2.bits.bit2<<6 | disnum2.bits.bit4<<3 | disnum1.bits.bit4<<2 | disnum1.bits.bit2<<1;
    LCDDATA4 = disnum3.bits.bit2<<3 | disnum4.bits.bit4<<2 | disnum4.bits.bit2<<1 | dissym1.bits.bit6<<0;
    LCDDATA5 = dissym2.bits.bit2<<7 | dissym2.bits.bit5<<6 | disnum3.bits.bit4<<0;
    //com3
    LCDDATA0 = dissym1.bits.bit1<<6 | disnum2.bits.bit3<<3 | disnum1.bits.bit3<<2 | dissym1.bits.bit0<<1;
    LCDDATA1 = dissym1.bits.bit2<<3 | disnum4.bits.bit3<<2 | dissym2.bits.bit0<<1 | dissym1.bits.bit7<<0;
    LCDDATA2 = dissym2.bits.bit1<<7 | dissym2.bits.bit6<<6 | disnum3.bits.bit3<<0;
}
