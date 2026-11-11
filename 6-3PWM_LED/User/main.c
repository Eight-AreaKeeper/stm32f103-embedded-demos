#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#define PWM_MAX     50   // 最亮 50%
#define STEP_DELAY  30   // 每步延时(ms)，越大呼吸越慢
  

uint8_t i;

int main(void)
{
	OLED_Init();
	PWM_Init();


	while(1)
	{
		for (i = 0; i <= PWM_MAX; i++)
		{
			PWM_SetCompare1(i);
			Delay_ms(STEP_DELAY);
		}
		for (i = 0; i <= PWM_MAX; i++)
		{
			PWM_SetCompare1(PWM_MAX - i);
			Delay_ms(STEP_DELAY);
		}
	}
}
