#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
	/* Keil 默认用 C89 标准，变量要定义在函数开头 */
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	/* 1. 开时钟：TIM2 挂在 APB1，GPIOA 挂在 APB2 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* 2. 配置 PA0 为复用推挽输出，让定时器来控制这个引脚 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 3. 选择内部时钟（72MHz）作为 TIM2 的时钟源 */
	TIM_InternalClockConfig(TIM2);
	
	/* 4. 配置时基单元：频率 = 72MHz / (PSC+1) / (ARR+1) = 72MHz / 720 / 100 = 1kHz */
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;      // 不分频（只影响滤波，和频率无关）
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;                  // ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;               // PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;             // 只有高级定时器用，填 0
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	/* 5. 配置输出比较通道1为 PWM 模式 */
	TIM_OCStructInit(&TIM_OCInitStructure);                          // 先给结构体赋默认值，避免有成员没初始化
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                // PWM 模式1：CNT < CCR 时输出有效电平
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;        // 有效电平为高电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    // 使能输出
	TIM_OCInitStructure.TIM_Pulse = 10;                               // CCR 初值，占空比 0%
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	
	/* 6. 启动定时器 */
	TIM_Cmd(TIM2, ENABLE);
}

/* 设置占空比：Compare 取 0~100，对应 0%~100% */
void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM2, Compare);
}