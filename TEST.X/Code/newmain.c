/****************************************************************************
Copyright (C), 2015, Sharman Chen, All rights reserved
FileName: /F/TESTPIC20150417/TESTPIC/TEST.X/newmain.c
Description:  
Version:  
Changelog:
Author: Sharman Chen
Email: mingv150@163.com
Mobile: 13720889921
*****************************************************************************/

#define _newmain_C_
/****************************************************************************
Global header file:
*****************************************************************************/
#include "Common.h"
#include "Model.h"
#include "oopc.h"
#include <htc.h>

/****************************************************************************
Local header file:
*****************************************************************************/
#include "newmain.h"
#include "Driver/LCD/Lcd.h"
#include "Driver/DS1302/DS1302.h"
#include "Driver/Key/Key.h"
#include "Driver/BSP/BspTimer.h"
#include "Driver/BSP/BspEeprom.h"

/****************************************************************************
Global Data Structure:
*****************************************************************************/
__CONFIG(FOSC_INTOSC & PWRTE_ON & WDTE_SWDTEN & MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_ON & IESO_OFF & FCMEN_ON & LVP_OFF);
__CONFIG(WRT_OFF);

u8 SystemState = SYSTEM_IDLE;
u8 System_SetOnOff_SubState = 1;
u8 System_SetTime_SubState = 1;
u8 System_OnOffState = 35;
u8 System_ForceOnOff = 0;
u8 System_ForceOnOffState = 0;

/****************************************************************************
Function: Name
Description:
Input:
Output:
Notes:
*****************************************************************************/
u16 TimetoDec(u8 hour,u8 minute)
{
    u16 temp;
    
    temp = hour*100+minute;
    return temp;
}



/****************************************************************************
Function: Name
Description:
Input:
Output:
Notes:
*****************************************************************************/
u8 Action_on(void)
{
    static u32 TimePre = 0;
    static u8 Step = 5;
	u8 Ret = 0;
    
    if(System_OnOffState != 1 )
    {
        System_OnOffState = 1;
        Step = 0;
    }

    switch(Step)
    {
    case 0:
        TimePre = systemtick;
        Key_PutEventDirect(EVENT_WAKEUP);
		if(HW_KRINR() == 0)
		{
			HW_OUTDCBW(1);  //Power off
			Step = 4;
		}
		else
		{
			HW_OUTDCBW(0);  //Power on
			Step = 1;
		}
        break;
        
    case 1:
        if(systemtick - TimePre > 1500)
        {
            if(HW_KRINR() == 0)
                HW_INFBW(0);
            else
                HW_INFBW(1);
            HW_INRBW(0);
            Step = 2;
        }
        break;
    case 2:
        if((systemtick - TimePre > 6000) || (HW_KRINR() == 0))
        {
            HW_INFBW(0);
            HW_INRBW(0);
            Step = 3;
        }
        break;
    case 3:
        if((systemtick - TimePre) > 8000 || (HW_KRINR() == 0))
        {
            HW_OUTDCBW(1);
            Step = 4;    
        }
        break;
    case 4:
		Ret = 1;
        break;
    default:
        break;
    }
	return Ret;
}


/****************************************************************************
Function: Name
Description:
Input:
Output:
Notes:
*****************************************************************************/
u8 Action_off(void)
{
    static u32 TimePre = 0;
    static u8 Step = 5;
	u8 Ret = 0;

    if(System_OnOffState != 0)
    {
        System_OnOffState = 0;
        Step = 0;
    }

    switch(Step)
    {
    case 0:
        TimePre = systemtick;
        Key_PutEventDirect(EVENT_WAKEUP);
		if(HW_KFINR() == 0)
		{
			HW_OUTDCBW(1);  //Power off
			Step = 4;
		}
		else
		{
			HW_OUTDCBW(0); //Power on
			Step = 1;
		}
        break;
    case 1:
        if(systemtick - TimePre > 1500)
        {
            HW_INFBW(0);
            if(HW_KFINR() == 0)
                HW_INRBW(0);
            else
                HW_INRBW(1);
            Step = 2;
        }
        break;
    case 2:
        if((systemtick - TimePre > 6000) || (HW_KFINR() == 0))
        {
            HW_INFBW(0);
            HW_INRBW(0);
            Step = 3;
        }
        break;
    case 3:
        if((systemtick - TimePre) > 8000 || (HW_KFINR() == 0))
        {
            HW_OUTDCBW(1);
            Step = 4;    
        }
        break;
    case 4:
		Ret = 1;
        break;
    default:
        break;
    }
	return Ret;
}


/****************************************************************************
Function: Name
Description:
Input:
Output:
Notes:
*****************************************************************************/
int main(void)
{
    u8 i;
    u8 Event;
    u8 User_OnOffIndex = 0;
    u8 User_UnitIndex = 0;
    u8 Hour_temp = 0;
    u8 Minute_temp = 0;
    u8 Magic_num = 0;
    u32 idle_tick = 0;
    t_TimeUnit TimeUnit[UNIT_NUM];
    u16 TimeCompNowTemp = 5678;
    u16 TimeCompNow = 0;
    u16 TimeCompOnSet[UNIT_NUM] = {0};
    u16 TimeCompOffSet[UNIT_NUM] = {0};
    u16 TimeScan = 0;
    u8 OnCount = 0;
    u8 OffCount = 0;
    u8 ScanFlag = 1;


    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELD = 0x00;
    ANSELE = 0x00;
    
    TRISA = 0x00;
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;

    PORTA = 0xFF;
    PORTB = 0xFF;
    PORTC = 0xFF;
    PORTD = 0xFF;
    PORTE = 0xFF;

    HW_OUTDCBC(0);
    HW_INFBC(0);
    HW_INRBC(0);
    HW_KRINC(1);
    HW_KFINC(1);

    HW_OUTDCBW(1);
    HW_INFBW(0);
    HW_INRBW(0);

    DS1302_Init();

    Lcd_Init();

    BspTimer_Timer0Init();

    BspTimer_WDTInit();

    //Get UserTime from eeprom
    BspEeprom_GetParam(0x00,&Magic_num,sizeof(Magic_num));

    if(Magic_num == MAGIC_NUM)
    {
        for(i=0;i<UNIT_NUM;i++)
        {
            BspEeprom_GetParam(0x04+i*sizeof(t_TimeUnit),(u8 *)&TimeUnit[i].User_Time[0][0],sizeof(t_TimeUnit));

            //Limite Condition 1
            if(TimeUnit[i].User_Time[INDEX_ON][INDEX_HOUR] > 23)
            {
                TimeUnit[i].User_Time[INDEX_ON][INDEX_HOUR] = 23;
            }
            if(TimeUnit[i].User_Time[INDEX_ON][INDEX_MINUTE] > 59)
            {
                TimeUnit[i].User_Time[INDEX_ON][INDEX_MINUTE] = 58;
            }
            if(TimeUnit[i].User_Time[INDEX_OFF][INDEX_HOUR] > 23)
            {
                TimeUnit[i].User_Time[INDEX_OFF][INDEX_HOUR] = 23;
            }
            if(TimeUnit[i].User_Time[INDEX_OFF][INDEX_MINUTE] > 59)
            {
                TimeUnit[i].User_Time[INDEX_OFF][INDEX_MINUTE] = 59;
            }

            #if 0
            //Limite condition 2
            TimeCompOnSet = TimetoDec(TimeUnit[i].User_Time[INDEX_ON][INDEX_HOUR],TimeUnit[i].User_Time[INDEX_ON][INDEX_MINUTE]);
            TimeCompOffSet = TimetoDec(TimeUnit[i].User_Time[INDEX_OFF][INDEX_HOUR],TimeUnit[i].User_Time[INDEX_OFF][INDEX_MINUTE]);

            if(TimeCompOnSet > TimeCompOffSet)
            {
                TimeUnit[i].User_Time[INDEX_ON][INDEX_HOUR] = TimeUnit[i].User_Time[INDEX_OFF][INDEX_MINUTE];
                TimeUnit[i].User_Time[INDEX_ON][INDEX_MINUTE] =  TimeUnit[i].User_Time[INDEX_OFF][INDEX_MINUTE];
                BspEeprom_SaveParam(0x04+i*sizeof(t_TimeUnit),(u8 *)&TimeUnit[i].User_Time[0][0],sizeof(t_TimeUnit));
            }

            //Limite Condition 3
            if(i > 0)
            {
                TimeCompOffSet = TimetoDec(TimeUnit[i-1].User_Time[INDEX_OFF][INDEX_HOUR],TimeUnit[i-1].User_Time[INDEX_OFF][INDEX_MINUTE]);
                if(TimeCompOnSet < TimeCompOffSet)
                {
                    TimeUnit[i].User_Time[INDEX_ON][INDEX_HOUR] = TimeUnit[i-1].User_Time[INDEX_OFF][INDEX_HOUR];
                    TimeUnit[i].User_Time[INDEX_ON][INDEX_MINUTE] = TimeUnit[i-1].User_Time[INDEX_OFF][INDEX_MINUTE];
                    BspEeprom_SaveParam(0x04+i*sizeof(t_TimeUnit),(u8 *)&TimeUnit[i].User_Time[0][0],sizeof(t_TimeUnit));
                }
            }
            #endif
        }
    }
    else
    {
        //Reinit the UserTime
        for(i=0;i<UNIT_NUM;i++)
        {
            TimeUnit[i].User_Time[INDEX_ON][INDEX_HOUR] = 23;
            TimeUnit[i].User_Time[INDEX_ON][INDEX_MINUTE] = 58;
            TimeUnit[i].User_Time[INDEX_OFF][INDEX_HOUR] = 23;
            TimeUnit[i].User_Time[INDEX_OFF][INDEX_MINUTE] = 59;
        }
        
        Magic_num = MAGIC_NUM;
        
        BspEeprom_SaveParam(0x00,&Magic_num,sizeof(Magic_num));

        for(i=0;i<UNIT_NUM;i++)
        {
            BspEeprom_SaveParam(0x04+i*sizeof(t_TimeUnit),(u8 *)&TimeUnit[i].User_Time[0][0],sizeof(t_TimeUnit));
        }
    }

    while(1)
    {  
        CLRWDT();
                
        DS1302_Readtime();

        Lcd_DisplayUpdate();
        
        Event = Key_GetEvent();

        if(Event != EVENT_NONE)
        {
            idle_tick = systemtick;
        }
        else
        {
            if((systemtick - idle_tick) > 15000)
            {
                idle_tick = systemtick;
                Key_PutEventDirect(EVENT_5STIMEOUT);
            }
        }


		/*System State Process*/
		switch(SystemState)
		{
		case SYSTEM_IDLE:
		    /*Process Event in IDLE*/
			switch(Event)
			{
            case EVENT_KEYSETUPS:
                System_SetOnOff_SubState = 1;
                User_OnOffIndex = 0;
                User_UnitIndex = 0;
                SystemState = SYSTEM_SETONOFF;
                break;

            case EVENT_KEYSETUPL:
                System_SetTime_SubState = 1;
                Hour_temp = DS1302_hour;
                Minute_temp = DS1302_minute;
                SystemState = SYSTEM_SETTIME;
                break;
                
            case EVENT_KEYUPL:
                System_ForceOnOff = 1;
                System_ForceOnOffState = 1;
                break;
            case EVENT_KEYDOWNL:
                System_ForceOnOff = 1;
                System_ForceOnOffState = 0;
                break;
            case EVENT_KEYENTERL:
                System_ForceOnOff = 0;
                System_ForceOnOffState = 0;
                break;
            case EVENT_5STIMEOUT:
                SystemState = SYSTEM_SLEEP;
                break;
            default:
                break;
			}
            /*Task In IDLE*/
			disnum1.byte = SegNum[DS1302_hour/10];
			disnum2.byte = SegNum[DS1302_hour%10];
			disnum3.byte = SegNum[DS1302_minute/10];
			disnum4.byte = SegNum[DS1302_minute%10];
			dissym2.bits.bit0 = DS1302_second%2;
			dissym1.bits.bit1 = System_ForceOnOff;
			break;

		
		case SYSTEM_SETONOFF:
            /*Process Event in SETONOFState*/
		    switch(Event)
			{
            case EVENT_KEYSETUPS:
                System_SetOnOff_SubState++;
                if(System_SetOnOff_SubState > 6*UNIT_NUM)
                    System_SetOnOff_SubState = 1;
                break;
            case EVENT_KEYENTERS:
                /*Save User_Time to eeprom*/
                for(i=0;i<UNIT_NUM;i++)
                {
                    BspEeprom_SaveParam(0x04+i*sizeof(t_TimeUnit),(u8 *)&TimeUnit[i].User_Time[0][0],sizeof(t_TimeUnit));
                }
                ScanFlag = 1;
                SystemState = SYSTEM_IDLE;
                break;
            case EVENT_KEYSETUPL:
                System_SetTime_SubState = 1;
                Hour_temp = DS1302_hour;
                Minute_temp = DS1302_minute;
                SystemState = SYSTEM_SETTIME;
                break;
            case EVENT_5STIMEOUT:
                SystemState = SYSTEM_SLEEP;
                break;
            default:
                break;
            }

            /*Task In SETONOFState*/
            dissym2.bits.bit0 = 1;
            dissym1.bits.bit1 = 0;

            User_UnitIndex = (System_SetOnOff_SubState-1)/6;
            User_OnOffIndex = (((System_SetOnOff_SubState-1)/3)%2) ? 1 : 0;

            switch((System_SetOnOff_SubState-1)%3)
            {
            case 0:
                disnum1.byte = SegNum[0];
                if(User_OnOffIndex)
                    disnum2.byte = SegNum[15];
                else
                    disnum2.byte = SegNum[17];
                disnum3.byte = SegNum[0];
                disnum4.byte = SegNum[User_UnitIndex+1];
                break;
            case 1:
                if(Event == EVENT_KEYUPS)
                {
                    TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_HOUR]++;
                                      
                    if(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_HOUR]>23)
                    {
                        TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_HOUR] = 0;
                    }
                }
                
                if(Event == EVENT_KEYDOWNS)
                {
                    if(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_HOUR] == 0)
                        TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_HOUR] = 23;
                    else
                        TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_HOUR]--;
                }

                #if 0
                //Limite condition 1
                if(TimeUnit[User_UnitIndex].User_Time[INDEX_ON][INDEX_HOUR] > TimeUnit[User_UnitIndex].User_Time[INDEX_OFF][INDEX_HOUR])
                {
                    TimeUnit[User_UnitIndex].User_Time[INDEX_OFF][INDEX_HOUR] = TimeUnit[User_UnitIndex].User_Time[INDEX_ON][INDEX_HOUR];     
                }

                //Limite condition 2
                if(User_UnitIndex > 0)
                {
                    if(TimeUnit[User_UnitIndex].User_Time[INDEX_ON][INDEX_HOUR] < TimeUnit[User_UnitIndex-1].User_Time[INDEX_OFF][INDEX_HOUR])
                    {
                        TimeUnit[User_UnitIndex].User_Time[INDEX_ON][INDEX_HOUR] = TimeUnit[User_UnitIndex-1].User_Time[INDEX_OFF][INDEX_HOUR];
                    }
                }
                #endif
                
                disnum1.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_HOUR])/10];
                disnum2.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_HOUR])%10];
                disnum3.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_MINUTE])/10];
                disnum4.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_MINUTE])%10];

                if(flicker)
                {
                    disnum1.byte = SegNum[16];
                    disnum2.byte = SegNum[16];
                }
                break;
            case 2:
                if(Event == EVENT_KEYUPS)
                {
                    TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_MINUTE]++;
                    if(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_MINUTE]>59)
                    {
                        TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_MINUTE] = 0;
                    }
                }
                
                if(Event == EVENT_KEYDOWNS)
                {
                    if(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_MINUTE] == 0)
                        TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_MINUTE] = 59;
                    else
                        TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_MINUTE]--;    
                }

                #if 0
                //Limite condition 3
                TimeCompOnSet = TimetoDec(TimeUnit[User_UnitIndex].User_Time[INDEX_ON][INDEX_HOUR],TimeUnit[User_UnitIndex].User_Time[INDEX_ON][INDEX_MINUTE]);
                TimeCompOffSet = TimetoDec(TimeUnit[User_UnitIndex].User_Time[INDEX_OFF][INDEX_HOUR],TimeUnit[User_UnitIndex].User_Time[INDEX_OFF][INDEX_MINUTE]);
                if(TimeCompOnSet > TimeCompOffSet)
                {
                    TimeUnit[User_UnitIndex].User_Time[INDEX_OFF][INDEX_MINUTE] =  TimeUnit[User_UnitIndex].User_Time[INDEX_ON][INDEX_MINUTE];   
                }

                //Limite Condition 4
                if(User_UnitIndex > 0)
                {
                    TimeCompOffSet = TimetoDec(TimeUnit[User_UnitIndex-1].User_Time[INDEX_OFF][INDEX_HOUR],TimeUnit[User_UnitIndex-1].User_Time[INDEX_OFF][INDEX_MINUTE]);
                    if(TimeCompOnSet < TimeCompOffSet)
                    {
                        TimeUnit[User_UnitIndex].User_Time[INDEX_ON][INDEX_MINUTE] = TimeUnit[User_UnitIndex-1].User_Time[INDEX_OFF][INDEX_MINUTE];
                    }
                }
                #endif
                
                disnum1.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_HOUR])/10];
                disnum2.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_HOUR])%10];
                disnum3.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_MINUTE])/10];
                disnum4.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][INDEX_MINUTE])%10];
                
                if(flicker)
                {
                    disnum3.byte = SegNum[16];
                    disnum4.byte = SegNum[16];
                }
                break;

            default:
                break;
            }

            #if 0
            switch(System_SetOnOff_SubState)
            {
            case 1:
                if(Event == EVENT_KEYUPS)
                {
                    User_OnOffIndex++;
                    if(User_OnOffIndex>1)
                    {
                        User_OnOffIndex = 0;
                    }
                }

                if(Event == EVENT_KEYDOWNS)
                {
                    if(User_OnOffIndex == 0)
                        User_OnOffIndex = 1;
                    else
                        User_OnOffIndex--;    
                }

                
                disnum1.byte = SegNum[0];
                if(User_OnOffIndex)
                disnum2.byte = SegNum[15];
                else
                disnum2.byte = SegNum[17];
                disnum3.byte = SegNum[0];
                disnum4.byte = SegNum[User_UnitIndex+1];

                if(flicker)
                {
                    disnum1.byte = SegNum[16];
                    disnum2.byte = SegNum[16];
                }
                break;
                    
            case 2:
                if(Event == EVENT_KEYUPS)
                {
                    User_UnitIndex++;
                    if(User_UnitIndex>(UNIT_NUM-1))
                    {
                        User_UnitIndex = 0;
                    }
                }

                if(Event == EVENT_KEYDOWNS)
                {
                    if(User_UnitIndex == 0)
                        User_UnitIndex = (UNIT_NUM-1);
                    else
                        User_UnitIndex--;    
                }
     
                disnum1.byte = SegNum[0];
                if(User_OnOffIndex)
                disnum2.byte = SegNum[15];
                else
                disnum2.byte = SegNum[17];
                disnum3.byte = SegNum[0];
                disnum4.byte = SegNum[User_UnitIndex+1];

                if(flicker)
                {
                    disnum3.byte = SegNum[16];
                    disnum4.byte = SegNum[16];
                }     
                break;
                    
            case 3:
                if(Event == EVENT_KEYUPS)
                {
                    TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][0]++;
                    
                    if(TimeUnit[User_UnitIndex].User_Time[1][0] > TimeUnit[User_UnitIndex].User_Time[0][0])
                    {
                        TimeUnit[User_UnitIndex].User_Time[1][0] = TimeUnit[User_UnitIndex].User_Time[0][0];     
                    }
                    
                    if(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][0]>23)
                    {
                        TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][0] = 0;
                    }
                }
                if(Event == EVENT_KEYDOWNS)
                {
                    if(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][0] == 0)
                        TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][0] = 23;
                    else
                        TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][0]--;
                    
                    if(TimeUnit[User_UnitIndex].User_Time[1][0] > TimeUnit[User_UnitIndex].User_Time[0][0])
                    {
                        TimeUnit[User_UnitIndex].User_Time[1][0] = TimeUnit[User_UnitIndex].User_Time[0][0];     
                    }
                }
                disnum1.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][0])/10];
                disnum2.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][0])%10];
                disnum3.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][1])/10];
                disnum4.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][1])%10];

                if(flicker)
                {
                    disnum1.byte = SegNum[16];
                    disnum2.byte = SegNum[16];
                }
                break;

            case 4:
                if(Event == EVENT_KEYUPS)
                {
                    TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][1]++;
                    if(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][1]>59)
                    {
                        TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][1] = 0;
                    }
                }
                if(Event == EVENT_KEYDOWNS)
                {
                    if(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][1] == 0)
                        TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][1] = 59;
                    else
                        TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][1]--;    
                }
                disnum1.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][0])/10];
                disnum2.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][0])%10];
                disnum3.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][1])/10];
                disnum4.byte = SegNum[(TimeUnit[User_UnitIndex].User_Time[User_OnOffIndex][1])%10];
                
                if(flicker)
                {
                    disnum3.byte = SegNum[16];
                    disnum4.byte = SegNum[16];
                }
                break;

            default:
                break;
            }
            #endif
            
			break;
			
		case SYSTEM_SETTIME:

		    /*Process Event in SETIMEState*/
		    switch(Event)
			{
            case EVENT_KEYSETUPS:
                System_SetTime_SubState++;
                if(System_SetTime_SubState > 2)
                    System_SetTime_SubState = 1;
                break;
            case EVENT_KEYENTERS:

                Minute_temp = (Minute_temp/10)<<4|(Minute_temp%10);
                Hour_temp = (Hour_temp/10)<<4|(Hour_temp%10);
                /*Save User_Time to ds1302*/
                DS1302_WriteReg(WRITE_PROTECT,0x00);        
                DS1302_WriteReg(WRITE_SECOND,0x00);        
                DS1302_WriteReg(WRITE_MINUTE,Minute_temp);       
                DS1302_WriteReg(WRITE_HOUR,Hour_temp);                
                DS1302_WriteReg(WRITE_PROTECT,0x80);
                ScanFlag = 1;
                SystemState = SYSTEM_IDLE;
                break;
            case EVENT_5STIMEOUT:
                SystemState = SYSTEM_SLEEP;
                break;
            default:
                break;
            }

            /*Task In SETIMEState*/
            disnum1.byte = SegNum[Hour_temp/10];
		    disnum2.byte = SegNum[Hour_temp%10];
		    disnum3.byte = SegNum[Minute_temp/10];
	        disnum4.byte = SegNum[Minute_temp%10];
            dissym2.bits.bit0 = 1;
            dissym1.bits.bit1 = 0;
			 
            switch(System_SetTime_SubState)
            {
            case 1:
                if(Event == EVENT_KEYUPS)
                {
                    Hour_temp++;
                    if(Hour_temp>23)
                    {
                        Hour_temp = 0;
                    }
                }
            
                if(Event == EVENT_KEYDOWNS)
                {
                    if(Hour_temp == 0)
                        Hour_temp = 23;
                    else
                        Hour_temp--;    
                }
            
			    if(flicker)
                {
                    disnum1.byte = SegNum[16];
                    disnum2.byte = SegNum[16];
                }
                break;
               
            case 2:
                if(Event == EVENT_KEYUPS)
                {
                    Minute_temp++;
                    if(Minute_temp>59)
                    {
                        Minute_temp = 0;
                    }
                }
            
                if(Event == EVENT_KEYDOWNS)
                {
                    if(Minute_temp == 0)
                        Minute_temp = 59;
                    else
                        Minute_temp--;    
                }
          
			    if(flicker)
                {
                    disnum3.byte = SegNum[16];
                    disnum4.byte = SegNum[16];
                }
			    break;
			default:
			    break;
			}            
			break;

		case SYSTEM_SLEEP:
		    /*Task In SETIMEState*/
		    if(Event == EVENT_WAKEUP)
		    {
                SystemState = SYSTEM_IDLE;
		    }
		    else
		    {
		        disnum1.byte = SegNum[16];
    			disnum2.byte = SegNum[16];
    			disnum3.byte = SegNum[16];
    			disnum4.byte = SegNum[16];
    			dissym1.byte = 0x00;
    			dissym2.byte = 0x00;
    			Lcd_DisplayUpdate();
    			CLRWDT();
    		    IOCBP = 0x00;
    		    IOCBN = 0x0F;
    		    IOCBF = 0x00;
    		    IOCIF = 0;
    		    INTCON = 0x00;
    		    IOCIE = 1;
    		    
                SLEEP();

                if(IOCIF)
                {
                    IOCIF = 0;
                    SystemState = SYSTEM_IDLE;
                }
                IOCBF = 0x00;
                IOCBP = 0x00;
                IOCBN = 0x00;
                IOCIE = 0;
                T0IE = 1;
                GIE = 1;
                BspTimer_WDTInit();
                CLRWDT();
                DS1302_Readtime();
            }
            break;

	    default:
			break;
		}

		/*GLOBAL State Process*/
		TimeCompNow = TimetoDec(DS1302_hour,DS1302_minute);

        if(System_ForceOnOff)
        {
            if(System_ForceOnOffState)
                Action_on();
            else
                Action_off();
        }
        else
        {   
            for(i=0;i<UNIT_NUM;i++)
            {
                TimeCompOnSet[i] = TimetoDec(TimeUnit[i].User_Time[INDEX_ON][INDEX_HOUR],TimeUnit[i].User_Time[INDEX_ON][INDEX_MINUTE]);
                TimeCompOffSet[i] = TimetoDec(TimeUnit[i].User_Time[INDEX_OFF][INDEX_HOUR],TimeUnit[i].User_Time[INDEX_OFF][INDEX_MINUTE]);

                if(TimeCompNow == TimeCompOnSet[i])
                {
                    OnCount++;
                }
                
                if(TimeCompNow == TimeCompOffSet[i])
                {
                    OffCount++;
                }
            }

            if(OnCount != 0 && OffCount != 0)
            {
                if(TimeCompNowTemp != TimeCompNow)
                {
                    Key_PutEventDirect(EVENT_WAKEUP);
                    TimeCompNowTemp = TimeCompNow;
                }
				OnCount = 0;
				OffCount = 0;
            }
            else if(OnCount == 0 && OffCount == 0 && ScanFlag)
            {
                TimeScan = TimeCompNow;
                ScanFlag = 0;
                while(1)
                {
                    if(TimeScan == 0)
                    {
                        TimeScan = 2359;
                    }
                    else if(TimeScan%100 == 0)
                    {
                        TimeScan = TimeScan - 41;
                    }
                    else
                    {
                        TimeScan = TimeScan - 1;
                    }

                    for(i=0;i<UNIT_NUM;i++)
                    {
                        if(TimeScan == TimeCompOnSet[i])
                        {
                            OnCount++;
                        }
                        
                        if(TimeScan == TimeCompOffSet[i])
                        {
                            OffCount++;
                        }
                    }
                    
                    if(OffCount)
                        break;

                    if(OnCount)
                        break;
                }
            }
            else
            {
                TimeCompNowTemp = 5678;
                if(OnCount)
				{
                    if(Action_on())
					{
						OnCount = 0;
						OffCount = 0;
					}
				}
                
                if(OffCount)
				{
                    if(Action_off())
					{
						OffCount = 0;
						OnCount = 0;
					}
				}
            }
        }
    }

    return 0;
}

