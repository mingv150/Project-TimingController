/****************************************************************************
Copyright (C), 2015, Sharman Chen, All rights reserved
FileName: /F/TESTPIC20150417/TESTPIC/TEST.X/Driver/DS1302/DS1302.c
Description:  
Version:  
Changelog:
Author: Sharman Chen
Email: mingv150@163.com
Mobile: 13720889921
*****************************************************************************/

#define _DS1302_C_
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
#include "DS1302.h"

/****************************************************************************
Global Data Structure:
*****************************************************************************/
u8 DS1302_hour;
u8 DS1302_minute;
u8 DS1302_second;

/****************************************************************************
Function: FunctionName
Description: 
Input: 
Output:
Notes: 
*****************************************************************************/
void DS1302_WriteReg (unsigned char addr,unsigned char dat)
{         
	unsigned char i,temp;         

	HW_DS1302RSTC(0);
	HW_DS1302RSTW(0);
	HW_DS1302SCLC(0);
	HW_DS1302SCLW(0);
	HW_DS1302RSTW(1);
	HW_DS1302DATC(0);   

	for (i=8; i>0; i--)        
	{                    
		HW_DS1302SCLW(0);               
		temp = addr;               
		HW_DS1302DATW(temp&0x01);
                addr = addr >> 1;
		HW_DS1302SCLW(1);        
	} 
              
	for (i=8; i>0; i--)        
	{                    
	   HW_DS1302SCLW(0);              
	   temp = dat;               
	   HW_DS1302DATW(temp&0x01);                        
	   dat >>= 1;                                 
	   HW_DS1302SCLW(1);       
	}         
	HW_DS1302RSTW(0);         
}    
	   
/****************************************************************************
Function: Name
Description:
Input:
Output:
Notes:
*****************************************************************************/
unsigned char DS1302_ReadReg ( unsigned char addr ) 
{         
	unsigned char i,temp,dat1,dat2;
	
	HW_DS1302RSTC(0);
	HW_DS1302RSTW(0);
	HW_DS1302SCLC(0);
	HW_DS1302SCLW(0);
	HW_DS1302RSTW(1);
	HW_DS1302DATC(0);
	
      
	for ( i=8; i>0; i-- )       
	{                    
		HW_DS1302SCLW(0);               
		temp = addr;               
		HW_DS1302DATW(temp&0x01);              
		addr >>= 1;                      
		HW_DS1302SCLW(1);         
	}
	
	temp = 0;
	HW_DS1302DATC(1);
	
	for ( i=8; i>0; i-- )        
	{                
		temp |= HW_DS1302DATR()<<7;		
		HW_DS1302SCLW(1); 		
		temp>>=1;               
		HW_DS1302SCLW(0);       
	} 
	
	HW_DS1302RSTW(0);   

	dat1=temp;         
	dat2=dat1/16;                           //数据进制转换 
 
	dat1=dat1%16;                         //十六进制转十进制         
	dat1=dat1+dat2*10;        
	return (dat1); 
}    

        
/****************************************************************************
Function: Name
Description:
Input:
Output:
Notes:
*****************************************************************************/
void DS1302_Init(void)
{
	DS1302_WriteReg(WRITE_PROTECT,0x00);         //禁止写保护
	DS1302_WriteReg(WRITE_SECOND,0x00);          //秒位初始化
	DS1302_WriteReg(WRITE_MINUTE,0x00);         //分钟初始化
	DS1302_WriteReg(WRITE_HOUR,0x12);                     //小时初始化
	DS1302_WriteReg(WRITE_PROTECT,0x80);                 //允许写保护
}

/****************************************************************************
Function: Name
Description:
Input:
Output:
Notes:
*****************************************************************************/
void DS1302_Readtime(void)
{
	DS1302_second = DS1302_ReadReg(READ_SECOND);
	DS1302_minute = DS1302_ReadReg(READ_MINUTE);
	DS1302_hour = DS1302_ReadReg(READ_HOUR);
}



#if 0
unsigned char CommunicationByte(unsigned char value)
{
    unsigned char i=0, temp=0;
	
	HW_DS1302SCLW(0);

    for(i = 0; i < 8; i++)
    {
		HW_DS1302DATC(1);
        asm("NOP");asm("NOP");asm("NOP");asm("NOP");
		
        temp >>= 1;
        if (HW_DS1302DATR() == 1)
        {
            temp |= 0x80;
        }


        if ((value & 0x01) != 0)
        {
			HW_DS1302DATC(0);
			HW_DS1302DATW(1);
        }
        else
        {
			HW_DS1302DATC(0);
			HW_DS1302DATW(0);
        }
		
		HW_DS1302SCLW(1);
		
        value >>= 1;
        asm("NOP");asm("NOP");asm("NOP");asm("NOP");
		
		HW_DS1302DATW(1);
		HW_DS1302SCLW(0);
    }

    return temp;
}

void DS1302WriteRegister(unsigned char reg, unsigned char value)
{
	HW_DS1302SCLW(0);
	HW_DS1302RSTW(1);
    CommunicationByte(reg);
    CommunicationByte(value);
	HW_DS1302RSTW(0);
}

unsigned char DS1302ReadRegister(unsigned char reg)
{
    unsigned char temp;
	
	HW_DS1302SCLW(0);
	HW_DS1302RSTW(1);
    CommunicationByte(reg);
    temp = CommunicationByte(0xff);
	HW_DS1302SCLW(0);
	HW_DS1302RSTW(0);
    return temp;
}

void DS1302WriteTime(unsigned char *data)
{
    unsigned char i;

	HW_DS1302SCLW(0);
	HW_DS1302RSTW(1);
    CommunicationByte(0xbe);

    for (i = 0; i < 8; i++)
    {
        CommunicationByte(data[i]);
    }
	HW_DS1302SCLW(0);
	HW_DS1302RSTW(0);
}

void DS1302ReadTime(unsigned char *buffer)
{
    unsigned char i;

	HW_DS1302SCLW(0);
	HW_DS1302RSTW(1);
    CommunicationByte(0xbf);
    for (i = 0; i < 8; i++)
    {
        buffer[i] = CommunicationByte(0xff);
    }
	HW_DS1302SCLW(0);
	HW_DS1302RSTW(0);
}

void DS1302TimeConfig(unsigned char hour,unsigned char min,unsigned char sec)
{
    //unsigned char temp;
	
    //temp = DS1302ReadRegister(0xc1);//读取ds1302ram 数据
    //if (temp != 0xaa)//判断是否第一次上电
    {
        DS1302WriteRegister(0x8e, 0x00);//允许写操作
        DS1302WriteRegister(0xc0, 0xaa);//置标志位

        //DS1302WriteTime(data);
		DS1302WriteRegister(0x80,sec);//秒
        DS1302WriteRegister(0x82,min);//分
        DS1302WriteRegister(0x84,hour);//时

        DS1302WriteRegister(0x8e, 0x80);//禁止写操作
    }
}

void DS1302TimeUpdate(unsigned char *data)
{

    DS1302WriteRegister(0x8e, 0x00);//允许写操作
    DS1302WriteRegister(0xc0, 0xaa);//置标志位

    DS1302WriteTime(data);//连续写入时钟、日期
    DS1302WriteRegister(0x8e, 0x80);//禁止写操作
}


void DS1302Init(void)
{
	HW_DS1302RSTC(0);
	HW_DS1302SCLC(0);
	HW_DS1302DATC(0);
}
#endif
