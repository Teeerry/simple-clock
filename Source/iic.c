#include "iic.h"

void delayus(unsigned char x)	//延时函数
{
	while(x)
	{
		x--;
	}
}

void iic_start(void)    //开始传输函数
{
	SDA = 1;			//先拉高信号线
	delayus(5);				
	SCL = 1;			//拉高时钟线
	delayus(5);				
	SDA = 0;			//拉低信号线，表示传输开始
	delayus(3);				
	SCL = 0;   			//拉低时钟线
}

void iic_stop(void)		//结束传输函数
{
	SCL = 0;			//拉低时钟线
	SDA = 0;			//拉低信号线
	SCL = 1;			//拉高时钟线，表示传输结束
	delayus(5);			
	SDA = 1;			//拉高信号线
}

void iic_ask(bit askbit)	//主机应答函数
{
	SCL = 0;				//拉低时钟线
  if(askbit)				//如果应答，就将信号线置零
		SDA = 0;
	else					//如果不应答，就将信号线置一
		SDA = 1;
	delayus(5);			
	SCL = 1;				//拉高时钟线
	delayus(5);			
	SCL = 0;				//拉低时钟线
	SDA = 1;				//拉高信号线
	delayus(5);			
}

unsigned char iic_waitask(void)		//主机等待从机应答信号
{
	SCL = 0;				//先拉低时钟线
	SDA = 1;				//释放总线，即拉高信号线
	delayus(5);			
	SCL = 1;				//拉高时钟线
	delayus(5);			
	if(SDA)					//查看信号线的电平
	{
		SCL = 0;			//拉低时钟线；如果信号线为高，即是从机不应答，发送传输结束指令，并返回0
		iic_stop();
		return 0;
	}
	else
	{
		SCL = 0;			//拉低时钟线；如果信号线为低，即从机应答，返回1
		return 1;
	}
}

void iic_sendbyte(unsigned char byte)			//主机传输数据到从机函数
{
	unsigned char i;
	
	SCL = 0;					//先拉低时钟线
	for (i=0;i<8;i++)			//数据由高位到低位传输
	{
		SDA = (byte & 0x80); 	//取数据的最高位,后将其移位到最低位，发送到从机
		delayus(5);				
		SCL = 1;				//拉高时钟线，产生一个上升沿
		byte <<= 1;				//数据左移一位
		delayus(5);				
		SCL = 0;				//拉低时钟线
	}
}

unsigned char iic_readbyte(void)//主机接收从机传输来的数据
{
	unsigned char i,byte;
	
	SCL=0;						//先拉低时钟线
	delayus(5);										
	for (i=0;i<8;i++)			//接收的数据由高位到低位
	 {
		 SCL = 1;				//拉高时钟线，产生上升沿
		 delayus(5);								
		 byte <<= 1;			//先将储存数据的变量左移一位
		 byte |= SDA;		    //再和接收到的数据做或运算
		 SCL = 0;				//拉低时钟线
		 delayus(5);			//拉低时钟线
	 }
	 return byte;				//返回接收到的数据
}

/******************************************************************************
函数功能：    向AT2402 EEPROM芯片写一个数据
入口参数：		saddr IIC设备的地址； baddr 要写的数据的地址； byte 要写的数据
******************************************************************************/
void AT2402_SendByte(unsigned char saddr, unsigned char baddr, unsigned char byte)
{
	unsigned char ask;
	
	saddr &= 0xfe;					//先将设备地址的最后一位清零，表示是主机要向从机写数据
	
	iic_start();					//发送开始传输指令
	iic_sendbyte(saddr);			//发送设备地址
	
	ask = iic_waitask();			//将从机的应答的情况放到存放应答标志变量
	if (ask)						//如果应答了，继续下面的操作，如果没有，直接结束
	{
		iic_sendbyte(baddr);		//发送要存放数据的地址
		ask = iic_waitask();		//将从机的应答的情况放到存放应答标志变量
		if (ask)					//如果应答了，继续下面的操作，如果没有，直接结束
		{
			iic_sendbyte(byte);		//发送要写到EEPROM的数据
			ask = iic_waitask();	//将从机的应答的情况放到存放应答标志变量
		}	
	}
	iic_stop();						//结束传输
}

unsigned char AT2402_ReceiveByte(unsigned char saddr,unsigned char baddr)
{
	unsigned char ask, receive_data;
	
	saddr &= 0xfe;
	
	iic_start();	
	iic_sendbyte(saddr);
	
	ask = 0;
	ask = iic_waitask();
	if (ask)
	{
		iic_sendbyte(baddr);

		ask = 0;
		ask = iic_waitask();
		if (ask)
		{
			iic_start();
			saddr |= 0x01;
			iic_sendbyte(saddr);
			
			ask = 0;
			ask = iic_waitask();
			if (ask)
			{
				receive_data = iic_readbyte();
				iic_ask(0);
				iic_stop();
				return receive_data;
			}
			else 
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	else 
	{
		return 0;
	}
}
