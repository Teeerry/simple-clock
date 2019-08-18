/**************************************************************
						秒表头文件
**************************************************************/

#ifndef __STOPWATCH_H__
#define __STOPWATCH_H__

#include"main.h"
#include"reg52.h"

void stop_watch();		 //秒表主函数
void ShowTime();		 //数码管显示时间 
void stopwatch_scan();	 //key2、key3按键扫描
void Key4Stop_Scan(); 	 //key4扫描函数
void Key4Stop_Handler(); //key4按键处理函数

extern uchar flag;
extern bit long_press;

uchar keytemp;						  //按键值
bit  stop_flag = 1;					  //秒表的暂停标记
uchar stop_time[]={0x00,0x00,0x00};	  //秒表的10毫秒，秒，分

/********************************************************
						秒表主函数
********************************************************/

void stop_watch()
{	
	Timer0_init();		   //初始化定时器0
	ms_10 = 0;			   //时间片置0

	while(flag == 4)	   //选择秒表功能
	{
		ShowTime();

		if(ms_10 >= 10)				   //定时10毫秒进行扫描
		{
		 	ms_10 = 0;
			menu_scan();			   //菜单扫描
			stopwatch_scan();		   //key2,key3扫描
			Key4Stop_Scan();		   //key4扫描
			
			if(!stop_flag)			   //时间增加，如果暂停则停止增加
				stop_time[0]++;
		}
		
		if(stop_time[0] >= 99)		   //对时间增加的处理
		{
			stop_time[0] = 0;
		 	stop_time[1]++;

			if(stop_time[1] >= 60)
			{
				stop_time[1] = 0;
				stop_time[2]++;

				if(stop_time[2] > 99)  //时间达到最大值后重新赋值
				{					   
					stop_time[0] = 0x00;
					stop_time[1] = 0x00;
					stop_time[2] = 0x00;
				}
			}
		}
		
	}
}

/**************************************************************
						  按键扫描
**************************************************************/
void stopwatch_scan()					//按键2、3扫描
{
	static char state = 0; 

	if (key2 == 0)
		keytemp = 2;
	else if (key3 == 0)
		keytemp = 3;
	else
		keytemp = 0;

 	switch(state)
	{
		case 0: if(keytemp == 0)
				{
					state = 1;
				}
				else
					state = 0;
				break;		
		case 1: if(keytemp == 0)
				{
					state = 2;
				}
				else
					state = 0;
				break;
		case 2: if(keytemp != 0) 
				{							 	//对按键进行处理
					if(keytemp == 2)
						stop_flag = ~stop_flag;	//暂停
					else if(keytemp == 3)
						stop_flag = 1;	 		//停止
					state = 0;
				}
				else
					state = 2;
				break;
		
	}
}

void Key4Stop_Scan()			   //按键4扫描
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
					
					Key4Stop_Handler(); 		//对按键进行处理
					Key4_State = 0;
				}
				else
					Key4_State = 3;				//进入状态3，判断是否被长按
				break;
	
		case 3: if (key4 == 0)
				{
					last_time++;
					if (last_time >= 200)		//长按
					{
						last_time = 0;
						long_press = 1;			//确认是长按
					}
				}
				else
					Key4_State = 2;				 
				break;

		   		}
}

void Key4Stop_Handler()				//key4按键处理
{
	if(long_press == 1)
	{
		long_press = 0;
		led_long = 0;				//长按指示灯
		flag = 1;					//回到时钟显示模式
	}

	else if(long_press == 0)
	{
	 	led_short = ~led_short; 	//短按指示灯，秒表置零
		stop_time[2] = 0;
		stop_time[1] = 0;
		stop_time[0] = 0;
	}
}

/**************************************************************
						数码管显示时间
***************************************************************/
void ShowTime()
{
	display(1,stop_time[0]%10);	
	display(2,stop_time[0]/10);
	display(3,17);
	display(4,stop_time[1]%10);	
	display(5,stop_time[1]/10);
	display(6,17);
	display(7,stop_time[2]%10);	
	display(8,stop_time[2]/10);	
}
#endif