#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "AD.h"

uint16_t ADValue;		//AD转换的原始值，0~4095
float Voltage;			//换算后的电压，0~3.3V

int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	AD_Init();			//AD初始化
	
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "ADValue:");
	OLED_ShowString(2, 1, "Voltage:0.00V");
	
	while (1)
	{
		ADValue = AD_GetValue();					//读取AD值
		Voltage = (float)ADValue / 4095 * 3.3;		//换算成电压：读数/4095*3.3V
		
		OLED_ShowNum(1, 9, ADValue, 4);				//显示AD值
		OLED_ShowNum(2, 9, Voltage, 1);				//显示电压的整数部分
		OLED_ShowNum(2, 11, (uint16_t)(Voltage * 100) % 100, 2);	//显示电压的小数部分（两位）
		
		Delay_ms(100);								//每100ms刷新一次
	}
}
