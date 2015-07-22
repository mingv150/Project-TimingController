/****************************************************************************
Copyright (C), 2014, Mingv150, All rights reserved
FileName: /Driver/Lcd/Lcd.h
Description:  
Author:  mingv150@163.com
Version:  
Changelog: 
*****************************************************************************/

#ifndef _Lcd_H_
#define _Lcd_H_

/*For common use*/
/*Macro Definition*/
#define Lcd_PURPOSE 1
/*Data Type Specification*/
typedef struct
{
    unsigned bit0:1;
    unsigned bit1:1;
    unsigned bit2:1;
    unsigned bit3:1;
    unsigned bit4:1;
    unsigned bit5:1;
    unsigned bit6:1;
    unsigned bit7:1;
}t_bitS;

typedef union
{
    t_bitS bits;
    unsigned char byte;
}t_byte;

//disnum1.bits.bit0 ->a
//disnum1.bits.bit1 ->b
//disnum1.bits.bit2 ->c
//disnum1.bits.bit3 ->d
//disnum1.bits.bit4 ->e
//disnum1.bits.bit5 ->f
//disnum1.bits.bit6 ->g
//disnum1.bits.bit7 ->Reserve

//dissym1.bits.bit0 ->P1
//dissym1.bits.bit1 ->P2
//dissym1.bits.bit2 ->P3
//dissym1.bits.bit3 ->P4
//dissym1.bits.bit4 ->P5
//dissym1.bits.bit5 ->P6
//dissym1.bits.bit6 ->P7
//dissym1.bits.bit7 ->P8

//dissym2.bits.bit0 ->COL
//dissym2.bits.bit1 ->T1
//dissym2.bits.bit2 ->T2
//dissym2.bits.bit3 ->T3
//dissym2.bits.bit4 ->T4
//dissym2.bits.bit5 ->T5
//dissym2.bits.bit6 ->T6
//dissym2.bits.bit7 ->Reserve

#ifdef _Lcd_C_
/*For local users*/
/*Macro Definition*/
#define LCD_PS                    LCD_PS1x                //lcd prescaler
#define LCD_COM                   LCD_COM3                //lcd com
#define LCD_COM0                0x00                        //com0 - static
#define LCD_COM1                0x01                        //com0..1 -> 1/2
#define LCD_COM2                0x02                        //com0..2 -> 1/3
#define LCD_COM3                0x03                        //com0..3 -> 1/4
#define LCD_PS1x                0x00
#define LCD_PS2x                0x01
#define LCD_PS3x                0x02
#define LCD_PS4x                0x03
#define LCD_PS5x                0x04
#define LCD_PS6x                0x05
#define LCD_PS7x                0x06
#define LCD_PS8x                0x07
#define LCD_PS9x                0x08
#define LCD_PS10x                0x09
#define LCD_PS11x                0x0a
#define LCD_PS12x                0x0b
#define LCD_PS13x                0x0c
#define LCD_PS14x                0x0d
#define LCD_PS15x                0x0e
#define LCD_PS16x                0x0f
/*Data Type Specification*/
#else
/*For external users*/
/*Macro Definition*/
/*Data Type Specification*/
extern const unsigned char SegNum[];
extern t_byte disnum1;
extern t_byte disnum2;
extern t_byte disnum3;
extern t_byte disnum4;
extern t_byte dissym1;
extern t_byte dissym2;
/*Interface Declarations*/
void Lcd_Init(void);
void Lcd_DisplayUpdate(void);
#endif

#endif