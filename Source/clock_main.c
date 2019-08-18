#include"ds1302.h"		 //显示时间
#include"stopwatch.h"	 //秒表
#include"alarm1.h"		 //闹钟1
#include"alarm2.h"		 //闹钟2

extern uchar flag;

void main()
{
	Ds1302Init();		 //初始化DS1302
	Timer0_init();		 //初始化定时器0

	while(1)
	{		
		switch(flag)
		{
			case 1:Ds1302ShowTime();break;	   //时钟
			case 2:alarm1();break;			   //闹钟1
			case 3:alarm2();break;			   //闹钟2
			case 4:stop_watch();break;		   //秒表
		}
		
	}
}
