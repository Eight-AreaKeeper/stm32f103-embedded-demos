#include "stm32f10x.h"                  // Device header

void IC_Init(void)
{
	/* Keil 默认用 C89 标准，变量要定义在函数开头 */
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	/* 1. 开时钟：TIM3 挂在 APB1，GPIOA 挂在 APB2 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* 2. 配置 PA6（TIM3_CH1）为上拉输入，接收被测信号 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 3. 选择内部时钟（72MHz）作为 TIM3 的时钟源 */
	TIM_InternalClockConfig(TIM3);
	
	/* 4. 配置时基单元：PSC=72-1，计数频率 fc = 72MHz / 72 = 1MHz（每 1us 加一）
	      ARR 设成最大 65536-1，防止计数溢出 */
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;                // ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;                // PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	/* 5. 配置输入捕获通道1 */
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;                 // 通道1
	TIM_ICInitStructure.TIM_ICFilter = 0xF;                          // 输入滤波，滤掉毛刺
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;      // 上升沿触发捕获
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;            // 每个边沿都捕获，不分频
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  // 直连通道：TI1 -> IC1
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	/* 6. 从模式：触发源选 TI1FP1，每次上升沿把 CNT 清零
	      这样每次捕获到的 CCR1 就是两个上升沿之间的计数值（一个周期） */
	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
	
	/* 7. 启动定时器 */
	TIM_Cmd(TIM3, ENABLE);
}

/* 测周法：频率 = fc / N = 1MHz / (CCR1 + 1)，单位 Hz */
uint32_t IC_GetFreq(void)
{
	return 1000000 / (TIM_GetCapture1(TIM3) + 1);
}
