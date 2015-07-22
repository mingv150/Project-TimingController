/****************************************************************************
Copyright (C), 2015, Sharman Chen, All rights reserved
FileName: /F/TESTPIC20150417/TESTPIC/TEST.X/Code/Driver/BSP/BspTimer.c
Description:  
Version:  
Changelog:
Author: Sharman Chen
Email: mingv150@163.com
Mobile: 13720889921
*****************************************************************************/

#define _BspTimer_C_
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
#include "BspTimer.h"
#include "../Key/Key.h"

/****************************************************************************
Global Data Structure:
*****************************************************************************/
u8 flicker = 0;
u32 systemtick = 0;
/****************************************************************************
Function: Name
Description:
Input:
Output:
Notes:
*****************************************************************************/
void interrupt IRQ(void)
{
    static u16 timebase = 0;
    u16 KeyState;

    if(T0IF&&T0IE)
    {
        T0IF=0;
        TMR0=240;

        KeyState = Key_Scan();

        KeyState = Key_Debounce(KeyState);

        Key_PutEvent(KeyState);

        systemtick++;

        timebase++;

        if(timebase > 500)
        {
            timebase = 0;
            flicker = ~flicker;
        }

    }
}

/****************************************************************************
Function: FunctionName
Description: 
Input: 
Output:
Notes: 
*****************************************************************************/
void BspTimer_Timer0Init(void)
{

    /*Enable Prescale*/
    /*Clock = 4000000Hz*/
    OSCCON = 0x68;

    PSA=0;
    T0CS=0;
    /*1:64*/
    PS0=1;PS1=0;PS2=1;
    /*timeout time:997HZ*/
    TMR0=240;
    /*Tiemr0 int en*/
    T0IE=1;
    GIE=1;
}


/****************************************************************************
Function: FunctionName
Description: 
Input: 
Output:
Notes: 
*****************************************************************************/
void BspTimer_WDTInit(void)
{
    /*4s*/
    WDTCON = 0x19;
}