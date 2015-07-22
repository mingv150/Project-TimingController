/****************************************************************************
Copyright (C), 2015, Sharman Chen, All rights reserved
FileName: /F/TESTPIC20150417/TESTPIC/TEST.X/newmain.h
Description:  
Version:  
Changelog:
Author: Sharman Chen
Email: mingv150@163.com
Mobile: 13720889921
*****************************************************************************/

#ifndef _newmain_H_
#define _newmain_H_

/*For common use*/
/*Macro Definition*/
#define newmain_PURPOSE 1
/*Data Type Specification*/

#ifdef _newmain_C_
/*For local users*/
/*Macro Definition*/
#define HW_OUTDCBC(x) (TRISA0=(x))
#define HW_OUTDCBW(x) (RA0=(x))

#define HW_INFBC(x) (TRISD5=(x))
#define HW_INFBW(x) (RD5=(x))

#define HW_INRBC(x) (TRISD4=(x))
#define HW_INRBW(x) (RD4=(x))

#define HW_KRINC(x) (TRISD7=(x))
#define HW_KRINR() (RD7)
#define HW_KFINC(x) (TRISD6=(x))
#define HW_KFINR() (RD6)

#define UNIT_NUM 6
#define MAGIC_NUM 0xAA
/*Data Type Specification*/
enum
{
	SYSTEM_IDLE = 0,
	SYSTEM_SETONOFF,
	SYSTEM_SETTIME,
	SYSTEM_SLEEP
};

#define INDEX_ON  0
#define INDEX_OFF 1
#define INDEX_HOUR 0
#define INDEX_MINUTE 1

typedef struct
{
    u8 User_Time[2][2];
}t_TimeUnit;

#else
/*For external users*/
/*Macro Definition*/
/*Data Type Specification*/
/*Interface Declarations*/
#endif

#endif