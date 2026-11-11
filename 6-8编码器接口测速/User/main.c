#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "Encoder.h"

int16_t Speed;			//编码器速度：每秒转过的计数值

int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	Timer_Init();		//定时中断初始化（TIM2，1秒一次）
	Encoder_Init();		//编码器初始化（TIM3编码器接口）
	
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "Speed:");
	
	while (1)
	{
		OLED_ShowSignedNum(1, 7, Speed, 5);		//不断刷新显示速度
	}
}

/**
  * 函    数：TIM2中断函数（每1秒进入一次）
  * 参    数：无
  * 返 回 值：无
  * 注意事项：每秒读一次编码器的计数值，就得到了“每秒多少个计数”，也就是速度
  */
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)		//判断是否是TIM2的更新事件触发的中断
	{
		Speed = Encoder_Get();								//读取这1秒内编码器的增量，就是速度
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);			//清除TIM2更新事件的中断标志位
	}
}
