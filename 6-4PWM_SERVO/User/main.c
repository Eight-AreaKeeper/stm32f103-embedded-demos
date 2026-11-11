#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Servo.h"
#include "Key.h"

uint8_t KeyNum;
float Angle;

int main(void)
{
	OLED_Init();
	Servo_Init();
	Key_init();
	
	OLED_ShowString(1, 1, "Angle:");
	Servo_SetAngle(Angle);
	OLED_ShowNum(1, 7, Angle, 3);
	
	while (1)
	{
		KeyNum = Key_GetNum();
		if (KeyNum == 1)
		{
			Angle += 30;               // 每按一次 PB1 的按键，角度加 30 度
			if (Angle > 180)
			{
				Angle = 0;             // 超过 180 度回到 0 度
			}
			Servo_SetAngle(Angle);
			OLED_ShowNum(1, 7, Angle, 3);
		}
	}
}
