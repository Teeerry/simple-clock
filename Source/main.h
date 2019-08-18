/*****************************************************
				对常用的函数进行声明、定义
*****************************************************/
#ifndef __MAIN_H__
#define __MAIN_H__

#include"reg52.h"
#include"intrins.h"

#define uint  unsigned int
#define uchar unsigned char

void save_clock();			//保存时间
void save_clock2();			//保存时间
void Ds1302ReadTime();		//读取时间
void alarm1_led();			//闹钟1提醒
void alarm2_led();			//闹钟2提醒

uchar flag = 1;				//菜单flag
bit set_time = 1;			//设置时间的开关
bit readtime = 0;			//时钟显示开关
bit alarm_1 = 0;			//闹钟响应开关
bit alarm_2 = 0;			//闹钟响应开关
sbit led_long = P2^7;		//长按、短按指示灯
sbit led_short = P2^6;
sbit led_alarm1 = P2^0;		//闹钟1响应，LED亮
sbit led_alarm2 = P2^1;		//闹钟2响应，LED亮

uchar ms_10 = 0,menu_10 = 0,alarm_10 = 0,ms_300 = 0,alarm1_100 = 0,alarm2_100 = 0;//时间片定义
 
uchar alarm_clock[3] = {0x00, 0x00,0x00};	//闹钟1时间，存储顺序分别是 开关、分、时	 
uchar alarm_clock2[3] = {0x00, 0x00,0x00};	//闹钟2时间，存储顺序分别是 开关、分、时	 

sbit key1 = P3^1; 			//独立按键
sbit key2 = P3^0; 
sbit key3 = P3^2;
sbit key4 = P3^3;

/**************************************************************
						数码管显示函数
***************************************************************/
sbit wei_A = P2^2;
sbit wei_B = P2^3;
sbit wei_C = P2^4;

uchar duan_table[] = {		   //0-15;16不显示;17显示'-';18显示字母n
						0x3f,0x06,0x5b,0x4f,0x66,0x6d,
						0x7d,0x07,0x7f,0x6f,0x77,0x7c,
						0x39,0x5e,0x79,0x71,0x00,0x40,0x37};	

void display(uchar i, uchar j) //数码管显示函数，i为位选，j为段选
{
	P0 = duan_table[16];	   //消隐
	switch (i)
	{
		case 1: wei_C = 0; wei_B = 0; wei_A = 0; break;
		case 2: wei_C = 0; wei_B = 0; wei_A = 1; break;
		case 3: wei_C = 0; wei_B = 1; wei_A = 0; break;
		case 4: wei_C = 0; wei_B = 1; wei_A = 1; break;
		case 5: wei_C = 1; wei_B = 0; wei_A = 0; break;
		case 6: wei_C = 1; wei_B = 0; wei_A = 1; break;
		case 7: wei_C = 1; wei_B = 1; wei_A = 0; break;
		case 8: wei_C = 1; wei_B = 1; wei_A = 1; break;
	}
	P0 = duan_table[j];
	_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_(); //避免扫描频率过高，显示不够清楚
	P0 = duan_table[16];
}
							   //闹钟闪烁显示，输入位、地址
							   //除指定位闪烁，其他直接显示
void shanshuo_display(uchar position,uchar alarm_time[3])	
{															
	switch (position)
	{
		case 0: display(8, alarm_time[2] / 10);
				display(7, alarm_time[2] % 10);
				display(6, 17);
				display(5, alarm_time[1] / 10);
				display(4, alarm_time[1] % 10);
				display(3, 17);
	
				if (ms_300 >= 150)						//开关闪烁
				{
					if (alarm_time[0])					//显示on
					{
						display(2, 0);
						display(1, 18);
					}
					else								//显示off
					{
						display(2, 15);
						display(1, 15);
					}
				}
		
				if (ms_300 >= 300)
				{
					display(1, 16);
					display(2, 16);
					ms_300 = 0;						    //闪烁时间片置零
				}
				break;
		
		case 1: display(8, alarm_time[2] / 10);
				display(7, alarm_time[2] % 10);
				display(6, 17);
				display(3, 17);
				if (alarm_time[0])				 	    //显示on
				{
					display(2, 0);
					display(1, 18);
				}
				else								    //显示off
				{
					display(2, 15);
					display(1, 15);
				}
		
				if (ms_300 >= 150)				        //分钟闪烁
				{
					display(5, alarm_time[1] / 10);
					display(4, alarm_time[1] % 10);
				}
				if (ms_300 >= 300)
				{
					display(4, 16);
					display(5, 16);
					ms_300 = 0;							//闪烁时间片置零
				}								   		
				break;

		case 2: display(6, 17);
				display(5, alarm_time[1] / 10);
				display(4, alarm_time[1] % 10);
				display(3, 17);
		
				if (alarm_time[0])						//显示on
				{
					display(2, 0);
					display(1, 18);
				}
				else									//显示off
				{
					display(2, 15);
					display(1, 15);
				}
				if (ms_300 >= 150)						//时闪烁
				{
					display(8, alarm_time[2] / 10);
					display(7, alarm_time[2] % 10);
				}
				if (ms_300 >= 300)
				{
					display(7, 16);
					display(8, 16);
					ms_300 = 0;							//闪烁时间片置零
				}
				break;

	}
}
/***************************************************************
			时间调整函数（输入调整的位，调整数据的地址）
***************************************************************/

void time_up(uchar position,uchar alarm_time[3])					 //时间增加
{
	if (position == 2)
	{
		if (alarm_time[position] >=23)
			alarm_time[position] = 0;
		else
			alarm_time[position]++;
	}
	else if (position == 1)
	{
		if (alarm_time[position] >= 59)
			alarm_time[position] = 0;
		else
			alarm_time[position]++;
	}
	else if (position == 0)
		alarm_time[0] = 1;
}

void time_down(uchar position, uchar alarm_time[3])						//时间减小
{
	if (position == 2)
	{
		if (alarm_time[position] == 0)
			alarm_time[position] = 23;
		else
			alarm_time[position]--;
	}
	else if (position == 1)
	{
		if (alarm_time[position] == 0)
			alarm_time[position] = 59;
		else
			alarm_time[position]--;
	}
	else if (position == 0)
		alarm_time[0] = 0x00;
}
/***************************************************************
					    	菜单响应函数
***************************************************************/
void menu_handler();						//菜单响应函数

uchar last_time = 0;   						//长按的时间标记
bit  long_press = 0,change_menu = 0;		//长按标记、改变菜单允许标记

void menu_scan()							//菜单扫描函数
{
	static uchar menu_state = 0;   		

	switch (menu_state)
	{
		case 0: if (key1 == 0)
				{
					menu_state = 1;
				}
				else
					menu_state = 0;
				break;
	
		case 1: if (key1 == 0)
				{
					menu_state = 2;
				}
				else
					menu_state = 0;
				break;
	
		case 2: if (key1 != 0)
				{
					
					menu_handler(); 		//响应按键
					menu_state = 0;
				}
				else
					menu_state = 3;			//进入状态3，判断是否被长按
				break;
	
		case 3: if (key1 == 0)
				{
					last_time++;
					if (last_time >= 200)	//长按
					{
						last_time = 0;
						long_press = 1;		//确认是长按
						if(flag == 1)		//在其他功能中，不会停止时间走动
						{
							readtime = 0;	//停止读取时钟
							set_time = 1;	//进入时间设置
						}
					}
				}
				else
					menu_state = 2;				 
				break;
	}
}

void menu_handler()							//菜单响应函数
{
	if(long_press)
	{
		long_press = 0;
		led_long = ~led_long;				//长按指示灯
		change_menu = ~change_menu;			
	}
	
	else if(change_menu)
	{
		led_short = ~led_short; 			//短按指示灯
		flag++;								//切换菜单
		if(flag >= 5)
			flag = 1;
		if(flag == 1)						//进入时间设置
		{
			Ds1302ReadTime();
			readtime = 0;
			set_time = 1;
		}
		else if(flag == 3)
				save_clock();			 	//菜单切换过程，保存时间
		else if(flag == 4)
				save_clock2();			    //菜单切换过程，保存时间
		if(flag != 1)
			{
				readtime = 1;				//打开时间读取
				set_time = 0;				//关闭时间设置
			}
	}		
}

/**************************************************************
					初始化定时器与中断函数
**************************************************************/

void Timer0_init()						//定时器0的初始化  
{
		
	TMOD = 0x01;		  
	TH0 = (65536 - 1000)/256;			//初始化1ms
	TL0 = (65536 - 1000)%256;
	EA  = 1;
	ET0 = 1;
	TR0 = 1;							//启动定时器
}

void T0_time() interrupt 1	 			//定时器中断函数
{
	TH0 = (65536 - 1000)/256;
	TL0 = (65536 - 1000)%256;

	ms_10++;							//键盘扫描时间片
	ms_300++;							//闪烁显示时间片
	menu_10++;							//菜单扫描时间片
   	alarm_10++;							//闹钟时间片
	alarm1_100++;
	alarm2_100++;
}


#endif