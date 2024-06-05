#include "stdelay.h"

static uint8_t    delay_nus=0;
static uint16_t  delay_nms=0;

void stdelay_init(uint8_t SYSCLK)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	delay_nus=SYSCLK/8;
	delay_nms=(uint16_t)(delay_nus)*1000;
}


void delay_us(uint32_t nus)
{
	uint32_t temp;
	SysTick->LOAD=nus*delay_nus;
	SysTick->VAL=0x00;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL=0x00;
}


void stdelay_ms(uint16_t nms)
{
	uint32_t temp;
	SysTick->LOAD=nms*delay_nms;
	SysTick->VAL=0x00;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL=0x00;
}
