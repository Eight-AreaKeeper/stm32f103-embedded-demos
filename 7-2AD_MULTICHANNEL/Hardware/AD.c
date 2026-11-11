#include "stm32f10x.h"                  // Device header

/* AD多通道：轮流读取PA0~PA3上的4路模拟电压
 * 硬件接线：PA0 - 电位器滑动端
 *           PA1 - 光敏电阻传感器模块的AO引脚
 *           PA2 - 热敏电阻传感器模块的AO引脚
 *           PA3 - 反射式红外传感器模块的AO引脚
 * 方法：非扫描 + 单次转换，每次读之前用ADC_RegularChannelConfig换一下通道
 * 转换结果范围：0~4095，对应电压0~3.3V
 */

/**
  * 函    数：AD初始化
  * 参    数：无
  * 返 回 值：无
  */
void AD_Init(void)
{
	/*C89要求变量声明放在函数开头*/
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//开启ADC1的时钟（ADC1挂在APB2总线上）
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟（PA0~PA3要用）
	
	/*设置ADC时钟*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);						//ADCCLK = 72MHz / 6 = 12MHz，不能超过14MHz
	
	/*GPIO初始化：4个引脚都设为模拟输入*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;			//模拟输入：断开施密特触发器，电压原样送进ADC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;	//PA0~PA3，对应ADC通道0~3
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//输入模式下这个参数没有作用，写上只是为了完整
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*和7-1的区别：这里不再配置规则组通道，改到AD_GetValue里每次读之前再配置*/
	
	/*ADC初始化*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					//独立模式：ADC1自己工作，不和ADC2配合
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//数据右对齐：读出来直接就是0~4095
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不用外部触发，用软件触发
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;					//单次转换：触发一次只转换一次
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;						//非扫描模式：只转换规则组的第1个通道
	ADC_InitStructure.ADC_NbrOfChannel = 1;								//规则组里通道的数量（非扫描模式下只用到1个）
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/*ADC使能*/
	ADC_Cmd(ADC1, ENABLE);									//打开ADC1，相当于给ADC上电
	
	/*ADC校准：上电后先校准一次，可以减小误差*/
	ADC_ResetCalibration(ADC1);								//复位校准
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);		//等待复位校准完成（完成后变成RESET）
	ADC_StartCalibration(ADC1);								//开始校准
	while (ADC_GetCalibrationStatus(ADC1) == SET);			//等待校准完成
}

/**
  * 函    数：获取指定通道的AD转换值
  * 参    数：ADC_Channel 要读取的通道，取值ADC_Channel_0~ADC_Channel_3（对应PA0~PA3）
  * 返 回 值：AD转换的值，范围：0~4095
  */
uint16_t AD_GetValue(uint8_t ADC_Channel)
{
	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_55Cycles5);	//把要读的通道放到规则组的第1个位置
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);					//软件触发一次AD转换
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);	//等待EOC（转换完成）标志位置1
	return ADC_GetConversionValue(ADC1);					//读取转换结果，读取后EOC标志位会自动清除
}
