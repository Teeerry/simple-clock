/***********************************************************
					   实现闹钟2功能
***********************************************************/
#ifndef __ALARM2_H__
#define __ALARM2_H__
#include"main.h"
#include"iic.h"

void save_clock2();			 //保存时间
void Key4_Handler2();		 //按键4的处理

extern uchar flag;
extern uchar alarm_10;		 
extern bit long_press;
extern bit alarm_2;
extern uchar alarm_clock2[];

/***************************************************************
						设置闹钟的键盘扫描
***************************************************************/
uchar clock2_keytemp = 0,i2_c = 0;			  //clock2_keytemp为按键键值，alarm_clock2[i2_c]						  
void clock2_scan()							 //key2、key3按键扫描
{
	static char clock_state = 0; 
	
	if (key2 == 0)
		clock2_keytemp = 2;
	else if (key3 == 0)
		clock2_keytemp = 3;
	else
		clock2_keytemp = 0;

 	switch(clock_state)
	{
		case 0: if(clock2_keytemp == 0)
				{
					clock_state = 1;
				}
				else
					clock_state = 0;
				break;		
		case 1: if(clock2_keytemp == 0)
				{
					clock_state = 2;
				}
				else
					clock_state = 0;
				break;
		case 2: if(clock2_keytemp != 0) 
				{
					if(clock2_keytemp == 2)
					 	time_up(i2_c,alarm_clock2);	
					else if(clock2_keytemp == 3)
						time_down(i2_c,alarm_clock2);
					clock_state = 0;
				}
				else
					clock_state = 2;
				break;
	}
}

void Key4_Scan2()					   		//Key4扫描
{
	static uint Key4_State = 0; 			//Key4状态

	switch (Key4_State)
	{
	case 0: if (key4 == 0)
			{
				Key4_State = 1;
			}
			else
				Key4_State = 0;
			break;

	case 1: if (key4 == 0)
			{
				Key4_State = 2;
			}
			else
				Key4_State = 0;
			break;

	case 2: if (key4 != 0)
			{
				
				Key4_Handler2(); 			//对按键进行处理
				Key4_State = 0;
			}
			else
				Key4_State = 3;
			break;

	case 3: if (key4 == 0)
			{
				last_time++;
				if (last_time >= 200)		//确认是长按，进入长按的操作
				{
					last_time = 0;
					long_press = 1;
				}
			}
			else
				Key4_State = 2;				 
			break;
	   		}
}

void Key4_Handler2()					//key4按键处理
{
	if(long_press == 1)
	{
		long_press = 0;
		led_long = 0;				    //长按指示灯
		save_clock2();					//保存数据
		flag = 1;						//回到时钟显示模式
	}
	else if(long_press == 0)
	{
	 	led_short = ~led_short;		    //短按指示灯
		i2_c++;
		if(i2_c > 2)
			i2_c = 0;
	}
}
/****************************************************************
						 保存数据
****************************************************************/

uchar flash_read2 = 0;
void save_clock2()		  //保存数据
{
	AT2402_SendByte(0xa6,*alarm_clock2,alarm_clock2[0]);	
	AT2402_SendByte(0xa8,*alarm_clock2+1,alarm_clock2[1]);
	AT2402_SendByte(0xaa,*alarm_clock2+2,alarm_clock2[2]);
	flash_read2 = 1;
}

void read_clock2()		  //读取设定的闹钟时间
{
	alarm_clock2[0] = AT2402_ReceiveByte(0xa7,*alarm_clock2);
	alarm_clock2[1] = AT2402_ReceiveByte(0xa9,*alarm_clock2+1);
	alarm_clock2[2] = AT2402_ReceiveByte(0xab,*alarm_clock2+2);
	flash_read2 = 0;
}

void alarm2_led()		  //闹钟响应函数，LED亮
{
	if(alarm_clock2[0] == 1 && (TIME[1]/16)*10 + (TIME[1]&0x0f) == alarm_clock2[1] && (TIME[2]/16)*10 + (TIME[2]&0x0f) ==alarm_clock2[2])
		alarm_2 = 1;
	else 
	{
		led_alarm1 = 1;
		alarm_2 = 0;
	}
}

/***************************************************************
							闹钟2主函数
****************************************************************/
void alarm2()
{
	ms_300 = 0;				 		//闪烁时间片置零
	while(flag == 3)
	{
		if(flash_read2)				//刷新，读取新保存闹钟的时间
			read_clock2();
		shanshuo_display(i2_c,alarm_clock2);//闪烁显示需要调整的位
				
		if(alarm_10 >=10)	 		//定时扫描按键
		{
			alarm_10 = 0;
			clock2_scan();			//key2、key3扫描
			Key4_Scan2();			//key4扫描
			menu_scan();		 	//菜单扫描
		}
	}
}
#endif