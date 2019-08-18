#ifndef _iic_h_
#define _iic_h_

#include <reg52.h>

sbit SCL=P2^1;
sbit SDA=P2^0;

void delayus(unsigned char x);
void iic_start(void);
void iic_stop(void);
void iic_ask(bit askbit);
unsigned char iic_waitask(void);
void iic_sendbyte(unsigned char byte);
unsigned char iic_readbyte(void);
void AT2402_SendByte(unsigned char saddr, unsigned char baddr, unsigned char byte);
unsigned char AT2402_ReceiveByte(unsigned char saddr,unsigned char baddr);

#endif