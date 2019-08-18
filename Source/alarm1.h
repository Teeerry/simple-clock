/***********************************************************
					闹钟1功能头文件
***********************************************************/
#ifndef __ALARM1_H__
#define __ALARM1_H__
#include"main.h"
#include"iic.h"

void save_clock();			 //保存时间
void Key4_Handler();		 //按键4的处理

extern uchar flag;
extern uchar alarm_10;		 
extern bit long_press;
extern bit alarm_1;
extern uchar alarm_clock[];
extern uchar alarm1_100;

/**************************************************************
						设置闹钟的键盘扫描
**************************************************************/

uchar clock_keytemp = 0,i_c = 0;	//clock_keytemp为按键键值，i_c为调整的位					  						 	    	
void clock_scan()					//key2、key3按键扫描
{
	static char clock_state = 0; 
	
	if (key2 == 0)
		clock_keytemp = 2;
	else if (key3 == 0)
		clock_keytemp = 3;
	else
		clock_keytemp = 0;

 	switch(clock_state)
	{
		case 0: if(clock_keytemp == 0)
				{
					clock_state = 1;
				}
				else
					clock_state = 0;
				break;		
		case 1: if(clock_keytemp == 0)
				{
					clock_state = 2;
				}
				else
					clock_state = 0;
				break;
		case 2: if(clock_keytemp != 0) 
				{
					if(clock_keytemp == 2)
					 	time_up(i_c,alarm_clock);	  //时间增加
					else if(clock_keytemp == 3)		  
					 	time_down(i_c,alarm_clock);	  //时间减小
					clock_state = 0;
				}
				else
					clock_state = 2;
				break;
	}
}

void Key4_Scan()					    //key4扫描
{
	static uint Key4_State = 0; 

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
				
				Key4_Handler(); 		//对按键进行处理
				Key4_State = 0;
			}
			else
				Key4_State = 3;
			break;

	case 3: if (key4 == 0)
			{
				last_time++;
				if (last_time >= 200)	//确认是长按，进入长按的操作
				{
					last_time = 0;
					long_press = 1;		//长按标志
				}
			}
			else
				Key4_State = 2;				 
			break;
	   		}
}

void Key4_Handler()						//key4按键处理
{
	if(long_press == 1)
	{
		long_press = 0;
		save_clock();					//保存数据
		led_long = 0;				    //长按指示灯
		flag = 1;						//回到时钟显示模式
	}
	else if(long_press == 0)
	{
	 	led_short = ~led_short;		    //短按指示灯
		i_c++;
		if(i_c >= 3)
			i_c = 0;
	}
}

/****************************************************************
						 保存数据
****************************************************************/

uchar flash_read = 1;		//保存数据时，进行刷新
void save_clock()			//保存数据函数
{
	AT2402_SendByte(0xa0,*alarm_clock,alarm_clock[0]);	
	AT2402_SendByte(0xa2,*alarm_clock+1,alarm_clock[1]);
	AT2402_SendByte(0xa4,*alarm_clock+2,alarm_clock[2]);
	flash_read = 1;
}

void read_clock()			//读取设定的闹钟时间
{
	alarm_clock[0] = AT2402_ReceiveByte(0xa1,*alarm_clock);
	alarm_clock[1] = AT2402_ReceiveByte(0xa3,*alarm_clock+1);
	alarm_clock[2] = AT2402_ReceiveByte(0xa5,*alarm_clock+2);
	flash_read = 0;
}


/**************************************************************
						闹钟响应，LED灯亮
**************************************************************/
										
void alarm1_led()						//闹钟响应函数，LED亮
{
	if(alarm_clock[0] == 1 &&(TIME[1]/16)*10 + (TIME[1]&0x0f) == alarm_clock[1] && (TIME[2]/16)*10 + (TIME[2]&0x0f) ==alarm_clock[2])
		alarm_1 = 1;					//闹钟响应标志
	else 
	{
		led_alarm1 = 1;
		alarm_1 = 0;
	}
}


/**********************************************************
						闹钟1主函数
***********************************************************/
void alarm1()
{
	ms_300 = 0;						//闪烁时间片置零
	while(flag == 2)
	{
		if(flash_read)				//刷新，读取新保存闹钟的时间
			read_clock();
		shanshuo_display(i_c,alarm_clock);//闪烁显示需要调整的位
		
		if(alarm_10 >=10)			//定时扫描按键
		{
			alarm_10 = 0;
			clock_scan();			//key2、key3扫描
			Key4_Scan();			//key4扫描
			menu_scan();		 	//菜单扫描
		}
	}
}

#endif