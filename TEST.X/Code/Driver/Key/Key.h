/****************************************************************************
Copyright (C), Year, Mingv150, All rights reserved
FileName: Key.h
Description:  
Author:  mingv150@163.com
Version:  
Changelog: 
*****************************************************************************/

#ifndef _Key_H_
#define _Key_H_

/*Macro Definition*/
/*Data Type Specification*/
enum event
{
    EVENT_NONE = 0,
    EVENT_KEYUPS,
    EVENT_KEYUPL,
    EVENT_KEYDOWNS,
    EVENT_KEYDOWNL,
    EVENT_KEYSETUPS,
    EVENT_KEYSETUPL,
    EVENT_KEYENTERS,
    EVENT_KEYENTERL,
    EVENT_5STIMEOUT,
    EVENT_WAKEUP,
    /*添加按键事件请在这边添加*/
    EVENT_MAX
};

#ifdef _Key_C_
/*Macro Definition*/
#define HW_KEYSETC(x) (TRISB3=(x))
#define HW_KEYSETR() (RB3)

#define HW_KEYUPC(x) (TRISB2=(x))
#define HW_KEYUPR() (RB2)

#define HW_KEYDOWNC(x) (TRISB1=(x))
#define HW_KEYDOWNR() (RB1)

#define HW_KEYENTERC(x) (TRISB0=(x))
#define HW_KEYENTERR()  (RB0)

/*修改抗抖动时间请在这边修改*/
/*短按键抗抖动时间*/
#define SHORTDEBOUNCE 20
/*长按键抗抖动时间*/
#define LONGDEBOUNCE  1000

/*添加或修改按键定义请在下面进行*/
#define KEYSETSHORT 0x0001
#define KEYSETLONG (KEYSETSHORT+0x8000)

#define KEYUPSHORT 0x0002
#define KEYUPLONG (KEYUPSHORT+0x8000)

#define KEYDOWNSHORT 0x0004
#define KEYDOWNLONG (KEYDOWNSHORT+0x8000)

#define KEYENTERSHORT 0x0008
#define KEYENTERLONG (KEYENTERSHORT+0x8000)

#define EVENT_FIFOLEN 8

/*Data Type Specification*/
/*按键缓冲区*/
typedef struct EventMage
{
    u8 EventFiFo[EVENT_FIFOLEN];    
    u8 FiFoHead;
    u8 FiFoTail;
}t_EventMage;

#else
/*Macro Definition*/
/*Data Type Specification*/
/*Interface Declarations*/
u16 Key_Scan(void);
u16 Key_Debounce(u16 KeyState);
void Key_PutEvent(u16 KeyValue);
void Key_PutEventDirect(u8 Event);
u8 Key_GetEvent(void);
#endif

#endif