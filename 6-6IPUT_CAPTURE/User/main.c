#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#include "IC.h"

int main(void)
{
	OLED_Init();
	PWM_Init();
	IC_Init();
	
	/* PA0 输出被测信号：频率 = 72MHz / (PSC+1) / 100，占空比 = CCR / 100 */
	PWM_SetPrescaler(720 - 1);         // 72MHz / 720 / 100 = 1000Hz
	PWM_SetCompare1(50);               // 占空比 50%
	
	OLED_ShowString(1, 1, "Freq:00000Hz");
	
	while (1)
	{
		OLED_ShowNum(1, 6, IC_GetFreq(), 5);   // 显示 PA6 测到的频率
	}
}
