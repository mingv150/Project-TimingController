/****************************************************************************
Copyright (C), 2014, Mingv150, All rights reserved
FileName: Key.c
Description:  
Author:  mingv150@163.com
Version:  
Changelog: 
*****************************************************************************/

#define _Key_C_
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
#include "Key.h"


/****************************************************************************
Global Data Structure:
*****************************************************************************/
/*定义按键缓冲区*/
static t_EventMage st_Event_EventMage = 
{
    {0,0,0,0,0,0},
    0,
    0
};


/*******************************************************************************
*Function:
*Description:从缓冲区获取按键事件
*Input:
*Output:EVENT_NONE为没有按键，其它为其它按键事件
*******************************************************************************/
u8 Key_GetEvent(void)
{
    u8 key;
    GIE = 0;
    if(st_Event_EventMage.FiFoHead == st_Event_EventMage.FiFoTail)
    {
        GIE = 1;
        return EVENT_NONE;
    }
    key = st_Event_EventMage.EventFiFo[st_Event_EventMage.FiFoHead];       
    st_Event_EventMage.FiFoHead = (++st_Event_EventMage.FiFoHead)%EVENT_FIFOLEN;
    GIE = 1;
    return key;
}


/****************************************************************************
Function: Name
Description:将按键值转换为按键事件并放入按键缓冲区
Input:
Output:
*****************************************************************************/
void Key_PutEvent(u16 KeyValue)
{
    u8 temp = EVENT_NONE;
    
    switch(KeyValue)
    {
        case KEYUPSHORT:
            temp = EVENT_KEYUPS;
            break;

        case KEYUPLONG:
            temp = EVENT_KEYUPL;
            break;

        case KEYDOWNSHORT:
            temp = EVENT_KEYDOWNS;
            break;

        case KEYDOWNLONG:
            temp = EVENT_KEYDOWNL;
            break;

        case KEYSETSHORT:
            temp = EVENT_KEYSETUPS;
            break;

        case KEYSETLONG:
            temp = EVENT_KEYSETUPL;
            break;

        case KEYENTERSHORT:
            temp = EVENT_KEYENTERS;
            break;

        case KEYENTERLONG:
            temp = EVENT_KEYENTERL;
            break;

        default:
            return;
    }

    /*Key_PutEventToTail*/
    /*将按键放入按键缓冲区*/
    if(st_Event_EventMage.FiFoHead == (st_Event_EventMage.FiFoTail+1)%EVENT_FIFOLEN)
    {
        return;
    }
    st_Event_EventMage.EventFiFo[st_Event_EventMage.FiFoTail] = temp;
    st_Event_EventMage.FiFoTail = (++st_Event_EventMage.FiFoTail)%EVENT_FIFOLEN;

}



void Key_PutEventDirect(u8 Event)
{
    GIE = 0;
    /*Key_PutEventToTail*/
    /*将按键放入按键缓冲区*/
    if(st_Event_EventMage.FiFoHead == (st_Event_EventMage.FiFoTail+1)%EVENT_FIFOLEN)
    {
        GIE = 1;
        return;
    }
    st_Event_EventMage.EventFiFo[st_Event_EventMage.FiFoTail] = Event;
    st_Event_EventMage.FiFoTail = (++st_Event_EventMage.FiFoTail)%EVENT_FIFOLEN;
    GIE = 1;
}
/*******************************************************************************
*Function:
*Description:进行按键扫描
*Input: KeyState = 8 Key 0->down 1->up
*Output:
*******************************************************************************/
u16 Key_Scan(void)
{
    u16 State = 0xffff;
    u8 i;

    HW_KEYSETC(1);
    HW_KEYUPC(1);
    HW_KEYDOWNC(1);
    HW_KEYENTERC(1);

    for(i=0;i<20;i++)
    {}

    if(HW_KEYSETR() == 0)
        CLRBIT(State,0);
    if(HW_KEYUPR() == 0)
        CLRBIT(State,1);
    if(HW_KEYDOWNR() == 0)
        CLRBIT(State,2);
    if(HW_KEYENTERR() == 0)
        CLRBIT(State,3);

    return State;
}


/****************************************************************************
Function: Name
Description:进行按键抗抖动处理
Input:
Output:
*****************************************************************************/
u16 Key_Debounce(u16 KeyState)
{
    u16 TrigKey = 0x0000;
    static u16 KeyStateBuf = 0x0000;
    static u16 ContKey = 0x0000;
    static u16 ContKeyBuf = 0x0000;
    static u8 ShortDebounce;
    static u16 LongDebounce;
    u16 Status = 0;

    if(KeyState != KeyStateBuf)
    {
        KeyStateBuf = KeyState;
        ShortDebounce = 0;
        return 0;
    }
    else if(ShortDebounce < SHORTDEBOUNCE)
    {
        ShortDebounce++;
        return 0;
    }
    else
    {
        //ShortDebounce = 0;
        KeyState = KeyStateBuf^0xffff;
        TrigKey = KeyState&(KeyState^ContKey);
        ContKey = KeyState;

        /*Just execute One time*/
        Status = TrigKey;

        #if 1
        
        if(ContKey != ContKeyBuf)
        {
            ContKeyBuf = ContKey;
            LongDebounce = 0;
        }
        else if(LongDebounce < LONGDEBOUNCE)
        {
            LongDebounce++;
        }
        else
        {
            /**/
            //LongDebounce = LongDebounce>>1;
            //LongDebounce = LONGDEBOUNCE - 50;
            LongDebounce = 0;  
            SETBIT(ContKey,15);
            //Key_PutEvent(ContKey);
            Status = ContKey;
        }
        #endif
    }

    return Status;
}
