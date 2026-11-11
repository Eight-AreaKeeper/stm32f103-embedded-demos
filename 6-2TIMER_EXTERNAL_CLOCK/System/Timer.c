#include "stm32f10x.h"                  // Device header

/**
  * 函    数：定时器外部时钟初始化（TIM2，使用PA0作为外部时钟输入，每10个脉冲进一次中断）
  * 参    数：无
  * 返 回 值：无
  */
void Timer_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//开启PA0所在的GPIO时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);		//开启TIM2的时钟

	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//浮空输入，PA0用作TIM2_ETR外部时钟输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*配置时钟源*/
	TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0x00);
	//选择TIM2为外部时钟模式2，外部触发信号来自ETR引脚（PA0），不分频，不反相，不滤波

	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，不分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;					//计数周期，即ARR的值，每10个外部脉冲产生一次更新事件
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;					//预分频器，即PSC的值，外部时钟模式2不受PSC影响计数源，此处保持不分频
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才用
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	/*中断输出配置*/
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);		//清除更新标志位，避免初始化后立刻进一次中断
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	//开启TIM2的更新中断

	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//分组2（整个工程只需配置一次）

	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				//选择TIM2的中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//响应优先级
	NVIC_Init(&NVIC_InitStructure);

	/*TIM使能*/
	TIM_Cmd(TIM2, ENABLE);		//使能TIM2，定时器开始运行
}

/* 定时器中断函数模板，可复制到使用它的地方（如main.c）
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)		//判断是否是更新中断
	{
		//在这里写每次中断要执行的代码，例如 Num ++;

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);			//清除标志位，否则会一直进中断
	}
}
*/
