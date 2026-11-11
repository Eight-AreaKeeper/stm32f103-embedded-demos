#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "AD.h"

uint16_t AD0, AD1, AD2, AD3;	//4个通道的AD值，范围0~4095

int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	AD_Init();			//AD初始化
	
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "AD0:");		//PA0 电位器
	OLED_ShowString(2, 1, "AD1:");		//PA1 光敏电阻
	OLED_ShowString(3, 1, "AD2:");		//PA2 热敏电阻
	OLED_ShowString(4, 1, "AD3:");		//PA3 反射式红外
	
	while (1)
	{
		AD0 = AD_GetValue(ADC_Channel_0);		//读取通道0（PA0）
		AD1 = AD_GetValue(ADC_Channel_1);		//读取通道1（PA1）
		AD2 = AD_GetValue(ADC_Channel_2);		//读取通道2（PA2）
		AD3 = AD_GetValue(ADC_Channel_3);		//读取通道3（PA3）
		
		OLED_ShowNum(1, 5, AD0, 4);				//显示4个通道的AD值
		OLED_ShowNum(2, 5, AD1, 4);
		OLED_ShowNum(3, 5, AD2, 4);
		OLED_ShowNum(4, 5, AD3, 4);
		
		Delay_ms(100);							//每100ms刷新一次
	}
}
