#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"

uint16_t Num;			//定义在定时器中断里自增的变量，每10个外部脉冲（PA0）自增1次

int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	Timer_Init();		//定时器外部时钟初始化

	/*显示静态字符串*/
	OLED_ShowString(1, 1, "Num:");		//中断计数：外部脉冲数/10
	OLED_ShowString(2, 1, "CNT:");		//TIM2当前计数值：外部脉冲数%10（0~9循环）

	while (1)
	{
		OLED_ShowNum(1, 5, Num, 5);					//不断刷新显示Num变量
		OLED_ShowNum(2, 5, TIM_GetCounter(TIM2), 5);	//不断刷新显示TIM2当前计数值
	}
}

/**
  * 函    数：TIM2中断函数（外部时钟每输入10个脉冲进入一次）
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  *           函数名为预留的指定名称，可以从启动文件复制
  */
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)		//判断是否是TIM2的更新事件触发的中断
	{
		Num ++;												//Num变量自增，用于测试定时中断
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);			//清除TIM2更新事件的中断标志位
	}
}