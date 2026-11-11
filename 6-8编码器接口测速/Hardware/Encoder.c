#include "stm32f10x.h"                  // Device header

/* 旋转编码器测速（定时器编码器接口）
 * 硬件接线：编码器A相 - PA6（TIM3_CH1），B相 - PA7（TIM3_CH2）
 */

/**
  * 函    数：编码器初始化（TIM3编码器接口模式）
  * 参    数：无
  * 返 回 值：无
  */
void Encoder_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);		//开启TIM3的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;				//上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*时基单元初始化*/
	/*编码器接口会托管时钟，CNT由编码器的A、B相信号驱动，所以不用选时钟源*/
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，不分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数方向由编码器决定，这里写什么都一样
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;				//ARR取最大值，计数范围最大
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;				//PSC不分频，编码器每个脉冲都计数
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才用
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	/*输入捕获初始化（只用到滤波器和极性）*/
	TIM_ICStructInit(&TIM_ICInitStructure);						//先给结构体赋默认值
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;			//通道1（PA6）
	TIM_ICInitStructure.TIM_ICFilter = 0xF;						//输入滤波，滤掉抖动
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;			//通道2（PA7）
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	/*编码器接口配置*/
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	/*TI12：A、B两相的上升沿和下降沿都计数（4倍频）
	  两个都是Rising表示不反相；如果转的方向和数值正负相反，把其中一个改成Falling即可*/
	
	/*TIM使能*/
	TIM_Cmd(TIM3, ENABLE);
}

/**
  * 函    数：获取编码器的增量值（读出CNT后清零）
  * 参    数：无
  * 返 回 值：从上次调用到现在，编码器转过的计数值，正数正转，负数反转
  */
int16_t Encoder_Get(void)
{
	int16_t Temp;
	Temp = TIM_GetCounter(TIM3);	//CNT是16位无符号数，转成int16_t后，反转时就是负数
	TIM_SetCounter(TIM3, 0);		//清零，下次从0开始计
	return Temp;
}
