#include"main.h"

void time_up();
void time_down();

extern uchar alarm1_10;
extern uchar menu_10;
extern uchar long_press;
extern uchar flag;

//---´æ´¢Ë³Ðò·Ö±ðÊÇÊ±·ÖÃëÊ®Î»¡¢¸öÎ»//
uchar alarm_clock[] = {0x01, 0x00, 0x00};
//ÄÖÖÓ¿ª¹Ø
bit on_off = 0;
uchar clock_keytemp = 0,i_c = 0;

//¼üÅÌÉ¨Ãè×´Ì¬»ú
void clock_scan()
{
	static char clock_state = 0; //ÄÖÖÓÉ¨Ãè×´Ì¬
	
	if (key2 == 0)
		clock_keytemp = 2;
	else if (key3 == 0)
		clock_keytemp = 3;
	else if (key4 == 0)
		clock_keytemp = 4;
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
					{
					 	time_up();	
					}
					else if(clock_keytemp == 3)
					{
					 	time_down();
					}
					else if(clock_keytemp == 4)
					{
					 	i_c++;
					}
					clock_state = 0;
				}
				else
					clock_state = 2;
				break;
		
	}
}


void time_up()
{
	if(i_c == 0)
	{
		if(alarm_clock[i_c] >=24)
			alarm_clock[i_c] = 1;
		else 
			alarm_clock[i_c]++;
	}
	else if(i_c == 1)
	{
		 if(alarm_clock[i_c] >=60)
			alarm_clock[i_c] = 0;
		else 
			alarm_clock[i_c]++;
	}
	else if(i_c == 2)
	{
		 if(alarm_clock[i_c] >=60)
			alarm_clock[i_c] = 0;
		else 
			alarm_clock[i_c]++;
	}
}

void time_down()
{
	if(i_c == 0)
	{
		if(alarm_clock[i_c] == 1)
			alarm_clock[i_c] = 24;
		else 
			alarm_clock[i_c]--;
	}
	else if(i_c == 1)
	{
		 if(alarm_clock[i_c] == 0)
			alarm_clock[i_c] = 59;
		else 
			alarm_clock[i_c]--;
	}
	else if(i_c == 2)
	{
		 if(alarm_clock[i_c] == 0)
			alarm_clock[i_c] = 59;
		else 
			alarm_clock[i_c]--;
	}
}
/*-----------------------------------------------
					ÄÖÖÓÖ÷º¯Êý
-----------------------------------------------*/
void alarm_clock1()
{
	
	while(flag == 3)
	{
		display(8,alarm_clock[0]/10);
		display(7,alarm_clock[0]%10);
		display(6,17);
		display(5,alarm_clock[1]/10);
		display(4,alarm_clock[1]%10);
		display(3,17);
		display(2,alarm_clock[2]/10);
		display(1,alarm_clock[2]%10);	

		if(alarm1_10 >=10)
		{
			alarm1_10 = 0;
			clock_scan();
		}
		

		if(menu_10 >= 10)
		{
			menu_10 = 0;
			menuscan();
		}	
	}

}