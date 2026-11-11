#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "Key.h"

uint8_t KeyNum;

int main(void)
{
	LED_Init();
	Key_init();
	
	while(1)
	{
		KeyNum = Key_GetNum();
		if (KeyNum == 1)
		{
			LED1_ON();
		}
		if (KeyNum == 2)
		{
			LED1_OFF();
		}
	}
}
