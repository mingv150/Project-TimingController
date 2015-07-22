/****************************************************************************
Copyright (C), 2015, Sharman Chen, All rights reserved
FileName: /F/TESTPIC20150417/TESTPIC/TEST.X/DS1302.h
Description:  
Version:  
Changelog:
Author: Sharman Chen
Email: mingv150@163.com
Mobile: 13720889921
*****************************************************************************/

#ifndef _DS1302_H_
#define _DS1302_H_

/*For common use*/
/*Macro Definition*/
#define DS1302_PURPOSE 1
/*Data Type Specification*/
#define WRITE_SECOND  0x80
#define WRITE_MINUTE  0x82
#define WRITE_HOUR    0x84
#define READ_SECOND   0x81
#define READ_MINUTE   0x83
#define READ_HOUR     0x85
#define WRITE_PROTECT 0x8E

#ifdef _DS1302_C_
/*For local users*/
/*Macro Definition*/
#define HW_DS1302RSTC(x) (TRISE0=(x))
#define HW_DS1302RSTW(x) (RE0=(x))
#define HW_DS1302SCLC(x) (TRISA4=(x))
#define HW_DS1302SCLW(x) (RA4=(x))
#define HW_DS1302DATC(x) (TRISA5=(x))
#define HW_DS1302DATW(x) (RA5=(x))
#define HW_DS1302DATR()  (RA5)
/*Data Type Specification*/
#else
/*For external users*/
/*Macro Definition*/

/*Data Type Specification*/
extern u8 DS1302_hour;
extern u8 DS1302_minute;
extern u8 DS1302_second;

/*Interface Declarations*/
void DS1302_Init(void);
void DS1302_Readtime(void);
void DS1302_WriteReg (unsigned char addr,unsigned char dat);
#endif

#endif	