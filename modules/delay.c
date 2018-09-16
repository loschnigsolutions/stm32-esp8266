/*
 * delay.c
 *
 *  Created on: 17-Sep-2018
 *      Author: loschnigsolutions
 */

#include <stdio.h>
#include <stm32f4xx.h>

#ifndef	getbit
#define getbit(n,b)		((n  &  (1  <<  b)) >> b)
#endif

TIM_TypeDef *t2	= TIM2;

void delay_init()
{
	RCC->APB1ENR	|=	RCC_APB1ENR_TIM2EN;

	t2->CR1	|=	TIM_CR1_CEN;

	t2->ARR	=	0xF;
	t2->PSC	=	0xFFFF;
	t2->SR &=  ~TIM_SR_UIF;
}

void delay()
{
	t2->CNT	= 0x00000000;

	while(1)
	{
		//trace_printf("CNT: %d  ARR: %d\n",t2->CNT,t2->ARR);

		if(getbit(t2->SR,0))
		{
			t2->SR	&=	~TIM_SR_UIF;
			break;
		}
	}

}


