#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Servo_Init(void)
{
	PWM_Init();
}

/* 角度 0~180 度 对应 高电平 0.5ms~2.5ms，即 CCR 500~2500 */
void Servo_SetAngle(float Angle)
{
	PWM_SetCompare3(Angle / 180 * 2000 + 500);
}
