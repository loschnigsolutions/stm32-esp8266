#include <stdio.h>
#include <string.h>
#include <stm32f4xx.h>
#include "diag/Trace.h"

#define getbit(n,b)		((n  &  (1 << b)) >> b)

void net_init();
void net_send(char *);

USART_TypeDef *u1 = USART1;

char msg[1024];
char *bp = msg;

DMA_Stream_TypeDef *d2 = DMA2_Stream2;	//DMA2 Channel:4 Stream:2 (USART1)

int main(int argc,char *args[])
{
	delay_init();
	net_init();
	while(1);
}

void net_init()
{
	GPIO_TypeDef  *pa = GPIOA;

	RCC->AHB1ENR	|=	RCC_AHB1ENR_DMA2EN;
	RCC->AHB1ENR	|=	RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR	|=	RCC_APB2ENR_USART1EN;

	//PA_9 (USART_TX)	PA_10 (USART_RX)

	pa->MODER	|=	GPIO_MODER_MODER9_1;
	pa->MODER	|=	GPIO_MODER_MODER10_1;

	pa->AFR[1]	|=	(7 << 4)|(7 << 8);

	u1->BRR	 =  (84000000/115200);

	u1->CR1	|=	USART_CR1_UE;
	u1->CR1	|=	USART_CR1_TE|USART_CR1_RE;
	u1->CR3	|=	USART_CR3_DMAR;

	memset(msg,'\0',1024);

	d2->CR	|=	(4 << 25);	//Channel:4

	d2->CR	|=	(1 << 4);	//TCIE
	d2->CR	|=	(1 << 5);	//PFCTRL
	d2->CR	|=	(1 << 10);	//MemInc

	d2->M0AR =  (uint32_t) &msg[0];
	d2->PAR	 =  (uint32_t) &u1->DR;

	d2->CR	|=	(1 << 0);

	net_send("AT");

	net_send("AT");

	//net_send("AT");
	//net_send("AT+GMR");
	//net_send("AT+RST");
	//net_send("AT+CWJAP?");
	//net_send("AT+CWLAP");
	net_send("AT+CWJAP=\"viola\",\"vimala_wilfred\"");
	net_send("AT+CIFSR");

	while(1);

}

void net_send(char *p)
{
	char *s = p;

	for(;*bp != '\0'; bp++);

	while(*p != '\0')
	{
		u1->DR	=	*p;
		while(!getbit(u1->SR,6));
		p++;
	}

	u1->DR	=	'\r';
	while(!getbit(u1->SR,6));

	u1->DR	=	'\n';
	while(!getbit(u1->SR,6));

	if(*s == ' ')
		return;

	s = bp;

	while((*s != 'O') && (*s != 'E'))
	{
		if(*(s+1) != '\0')
		{
			s++;
		}
		delay();
	}

	while((*s != 'K') && (*s != 'R'))
	{
		if(*(s+1) != '\0')
		{
			s++;
		}
		delay();
	}

	if(*s != 'R')
	trace_printf("Recieved: %s\n",bp);
}
