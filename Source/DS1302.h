/***************************************************************************
                         DS1302显示时间
****************************************************************************/
#ifndef __STCREG_H__
#define __STCREG_H__

#include"reg52.h"
#include"main.h"

void Ds1302ShowTime();		  //时间显示主函数
void DStime_up();			  //按键时间增加函数
void DStime_down();			  //按键时间减少函数
void Key4_TimeScan();		  //key4扫描函数
void Key4_TimeHandler();	  //key4按键处理函数

extern bit long_press;
extern uchar flag;
extern bit readtime;	  	  //时钟显示开关
extern bit set_time;		  //设置时间的开关

sbit SCLK = P3^6;
sbit DSIO = P3^4;
sbit RST = P3^5;
						   	  //DS1302写入和读取时分秒的地址命令
uchar code READ_RTC_ADDR[7] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d};
							  //秒分时日月周年 最低位读写位 
uchar code WRITE_RTC_ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};
							  //DS1302时钟初始化0点00分00秒，存储顺序是秒分时，存储格式是用BCD码
uchar TIME[3] = {0x00, 0x00, 0x00};


/*******************************************************************************
* 函 数 名         : Ds1302Write
* 函数功能		   : 向DS1302命令（地址+数据）
* 输    入         : addr,dat
* 输    出         : 无
*******************************************************************************/

void Ds1302Write(uchar addr, uchar dat)
{
	uchar n;
	RST = 0;
	_nop_();

	SCLK = 0;				//先将SCLK置低电平。
	_nop_();
	RST = 1; 				//然后将RST(CE)置高电平。
	_nop_();

	for (n=0; n<8; n++)		//开始传送八位地址命令
	{
		DSIO = addr & 0x01; //数据从低位开始传送
		addr >>= 1;
		SCLK = 1;			//数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;
		_nop_();
	}
	for (n=0; n<8; n++)		//写入8位数据
	{
		DSIO = dat & 0x01;
		dat >>= 1;
		SCLK = 1;			//数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;
		_nop_();	
	}	
		 
	RST = 0;				//传送数据结束
	_nop_();
}

/*******************************************************************************
* 函 数 名         : Ds1302Read
* 函数功能		   : 读取一个地址的数据
* 输    入         : addr
* 输    出         : dat
*******************************************************************************/

uchar Ds1302Read(uchar addr)
{
	uchar n,dat,dat1;
	RST = 0;
	_nop_();

	SCLK = 0;				//SCLK置低电平。
	_nop_();
	RST = 1;				//RST(CE)置高电平。
	_nop_();

	for(n=0; n<8; n++)		//传送八位地址命令
	{
		DSIO = addr & 0x01; //数据从低位开始传送
		addr >>= 1;
		SCLK = 1;			//数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;			//DS1302下降沿时，放置数据
		_nop_();
	}
	_nop_();
	for(n=0; n<8; n++)		//读取8位数据
	{
		dat1 = DSIO;		//从最低位开始接收
		dat = (dat>>1) | (dat1<<7);
		SCLK = 1;
		_nop_();
		SCLK = 0;			//DS1302下降沿时，放置数据
		_nop_();
	}

	RST = 0;
	_nop_();				//DS1302复位的稳定时间
	SCLK = 1;
	_nop_();
	DSIO = 0;
	_nop_();
	DSIO = 1;
	_nop_();
	return dat;	
}

/*******************************************************************************
* 函 数 名         : Ds1302Init
* 函数功能		   : 初始化DS1302.
*******************************************************************************/

void Ds1302Init()
{
	uchar n;
	Ds1302Write(0x8E,0x00);		//关闭写保护功能
	for (n=0; n<3; n++)			//写入7个字节的时钟信号：分秒时
	{
		Ds1302Write(WRITE_RTC_ADDR[n],TIME[n]);	
	}
	Ds1302Write(0x8E,0x80);		//打开写保护功能
}

/*******************************************************************************
* 函 数 名         : Ds1302ReadTime
* 函数功能		   : 读取时钟信息
*******************************************************************************/

void Ds1302ReadTime()
{
	uchar n;
	for (n=0; n<3; n++)//读取7个字节的时钟信号：分秒时日月周年
	{
		TIME[n] = Ds1302Read(READ_RTC_ADDR[n]);
	}
		
}

/***************************************************************
		  			        按键扫描
***************************************************************/

uchar time_keytemp = 0, i_t = 0;		 //time_keytemp为按键值，i_t为切换调整的位

void time_scan()						 //key2、key3按键扫描
{
	static char time_state = 0; 
	
	if (key2 == 0)
		time_keytemp = 2;
	else if (key3 == 0)
		time_keytemp = 3;
	else
		time_keytemp = 0;

 	switch(time_state)
	{
		case 0: if(time_keytemp == 0)
				{
					time_state = 1;
				}
				else
					time_state = 0;
				break;		
		case 1: if(time_keytemp == 0)
				{
					time_state = 2;
				}
				else
					time_state = 0;
				break;
		case 2: if(time_keytemp != 0) 
				{
					if(time_keytemp == 2)
					{
					 	DStime_up();	
					}
					else if(time_keytemp == 3)
					{
					 	DStime_down();
					}
					time_state = 0;
				}
				else
					time_state = 2;
				break;
	}
}

void Key4_TimeScan()						//key4扫描
{
	static uchar Key4_Timestate = 0; 		//key4的状态

	switch (Key4_Timestate)
	{
	case 0: if (key4 == 0)
			{
				Key4_Timestate = 1;
			}
			else
				Key4_Timestate = 0;
			break;

	case 1: if (key4 == 0)
			{
				Key4_Timestate = 2;
			}
			else
				Key4_Timestate = 0;
			break;

	case 2: if (key4 != 0)
			{
				
				Key4_TimeHandler(); 		//对按键进行处理
				Key4_Timestate = 0;
			}
			else
				Key4_Timestate = 3;
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
				Key4_Timestate = 2;				 
			break;
	   		}
}

void Key4_TimeHandler()					//key4按键处理
{
	if(long_press == 1)
	{
		long_press = 0;
		led_long = 0;				    //长按指示灯
		readtime = 1;				   	//打开时间显示
		set_time = 0;
	}	
	else if(long_press == 0)
	{
	 	led_short = ~led_short;		    //短按指示灯
		i_t++;							//切换调整的位
		if(i_t >= 3)
			i_t = 0;
	}
}
/****************************************************************
						数码管显示的数字调整
*****************************************************************/
void DStime_up()							 //时间增加
{
	if(i_t == 2)							 //调整时
	{
		if(TIME[i_t] >=0x23)
			TIME[i_t] = 0x00;
		else 
		{	
			TIME[i_t]++;
			if((TIME[i_t]&0x0f) >= 0x0a)
				TIME[i_t] += 6; 		
		}
	}
	else if(i_t == 1||i_t == 0)				 //调整分、秒
	{
		 if(TIME[i_t] >=0x59)
			TIME[i_t] = 0x00;
		 else 
		 {	
			TIME[i_t]++;
			if((TIME[i_t]&0x0f) >= 0x0a)
				TIME[i_t] += 6; 		
		 } 
	}
}

void DStime_down()							  //时间减小
{
	if(i_t == 2)						   	  //调整时
	{
		if(TIME[i_t] == 0x00)
			TIME[i_t] = 0x23;
		else 
		{	
			TIME[i_t]--;
			if((TIME[i_t]&0x0f) == 0x00)
				TIME[i_t] -= 7; 		
		}
	}
	else if(i_t == 1||i_t == 0)				  //调整分、秒
	{
		 if(TIME[i_t] == 0x00)
			TIME[i_t] = 0x59;
		 else 
		 {	
		 	 TIME[i_t]--;
		 	 if((TIME[i_t]&0x0f) == 0x00)
				 TIME[i_t] -= 7; 		
		 }
	}
}
/********************************************************
					数码管显示时间
********************************************************/
void display_time()				   //数码管显示时间
{
	Ds1302ReadTime();		 	   //读取时间
	display(1,TIME[0]&0x0f);	 
	display(2,TIME[0]/16);
	display(3,17);
	display(4,TIME[1]&0x0f);	
	display(5,TIME[1]/16);
	display(6,17);
	display(7,TIME[2]&0x0f);	
	display(8,TIME[2]/16);
}
								  //闪烁显示时间
void shanshuo_time(uchar position, uchar time[3])
{
	switch (position)
	{
	case 0: 					  //除了需要闪烁的位外，其他位直接显示
			display(3, 17);
			display(4, time[1] & 0x0f);
			display(5, time[1] / 16);
			display(6, 17);
			display(7, time[2] & 0x0f);
			display(8, time[2] / 16);

			if (ms_300 >= 150)						//秒闪烁
			{
				display(1, time[0] & 0x0f);
				display(2, time[0] / 16);
			}
	
			if (ms_300 >= 300)
			{
				display(1, 16);
				display(2, 16);
				ms_300 = 0;						    //闪烁时间片置零
			}
			break;

	case 1:
			display(1, time[0] & 0x0f);
			display(2, time[0] / 16);
			display(3, 17);
			display(6, 17);
			display(7, time[2] & 0x0f);
			display(8, time[2] / 16);
	
			if (ms_300 >= 150)				        //分钟闪烁
			{
				display(4, time[1] & 0x0f);
				display(5, time[1] / 16);
			}
			if (ms_300 >= 300)
			{
				display(4, 16);
				display(5, 16);
				ms_300 = 0;							//闪烁时间片置零
			}
			break;

	case 2:
			display(1, time[0] & 0x0f);
			display(2, time[0] / 16);
			display(3, 17);
			display(4, time[1] & 0x0f);
			display(5, time[1] / 16);
			display(6, 17);
			if (ms_300 >= 150)						//时闪烁
			{
				display(7, time[2] & 0x0f);
				display(8, time[2] / 16);
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

/*******************************************************************************
						显示时钟信息主函数
*******************************************************************************/

void Ds1302ShowTime()
{
	led_alarm2 = 1 ;				 //将闹钟响应关闭
	led_alarm1 = 1 ;
	alarm1_100 = 0 ;				 //闹钟闪烁时间片置零
	alarm2_100 = 0 ;
	ms_300 = 0 ;					 //数码管闪烁时间片置零

	while(flag == 1)
	{								 
		if(set_time)				 //设置时间过程，数字闪烁显示
			shanshuo_time(i_t,TIME);
		if(menu_10 >= 10)			 //定时扫描按键
		{
			menu_10 = 0;
			menu_scan();			 //菜单扫描
			time_scan();			 //时间调整扫描
			Key4_TimeScan();		 //key4扫描（保存、返回时间显示模式）
			alarm1_led();			 //闹钟1提醒
			alarm2_led();			 //闹钟2提醒
		}
		
		if(readtime == 1)			 //打开时钟显示	
			display_time();			 //显示时间
		else						 //重新初始化时间
			Ds1302Init();


		if(alarm_1&&alarm1_100 >= 100)//闹钟1设定时间到，LED灯开始闪烁
		{	
			led_alarm1 = ~led_alarm1;
			alarm1_100 = 0;
		}
		if(alarm_2&&alarm2_100 >= 100)//闹钟2设定时间到，LED灯开始闪烁
		{	
			led_alarm2 = ~led_alarm2;
			alarm2_100 = 0;	
		}

	}	
}

#endif
