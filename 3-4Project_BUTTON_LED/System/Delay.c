#include "stm32f10x.h"

/**
  * 使用SysTick定时器实现delay，不占用其他外设资源
  * 时钟源为AHB时钟8分频（HCLK/8）
  */

void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 9 * xus;
	SysTick->VAL = 0x00;
	SysTick->CTRL = 0x00000001;
	while (!(SysTick->CTRL & 0x00010000));
	SysTick->CTRL = 0x00000000;
}

void Delay_ms(uint32_t xms)
{
	while (xms--)
	{
		Delay_us(1000);
	}
}

void Delay_s(uint32_t xs)
{
	while (xs--)
	{
		Delay_ms(1000);
	}
}
