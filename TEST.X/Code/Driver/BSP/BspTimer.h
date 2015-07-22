/****************************************************************************
Copyright (C), 2015, Sharman Chen, All rights reserved
FileName: /F/TESTPIC20150417/TESTPIC/TEST.X/Code/Driver/BSP/BspTimer.h
Description:  
Version:  
Changelog:
Author: Sharman Chen
Email: mingv150@163.com
Mobile: 13720889921
*****************************************************************************/

#ifndef _BspTimer_H_
#define _BspTimer_H_

/*For common use*/
/*Macro Definition*/
#define BspTimer_PURPOSE 1
/*Data Type Specification*/

#ifdef _BspTimer_C_
/*For local users*/
/*Macro Definition*/
/*Data Type Specification*/
#else
/*For external users*/
/*Macro Definition*/
/*Data Type Specification*/
extern u8 flicker;
extern u32 systemtick;
/*Interface Declarations*/
void BspTimer_Timer0Init(void);
void BspTimer_WDTInit(void);
#endif

#endif