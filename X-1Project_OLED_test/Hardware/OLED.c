#include "stm32f10x.h"
#include "Delay.h"
#include "OLED_Font.h"

/* 0.96寸 OLED（SSD1306，128*64，I2C接口）软件I2C驱动
 * 硬件接线：VCC - 面包板正极(3.3V)，GND - 面包板负极，SCL - PB8，SDA - PB9
 * OLED插在PB6~PB9那几列，PB6、PB7所在列用导线接到面包板电源轨，所以程序里不能驱动PB6、PB7
 * 从机地址：0x78
 */

/* OLED写SCL/SDA高低电平，参数BitValue范围：0/1 */
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(x));
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)(x));

/**
  * 函    数：OLED的GPIO初始化
  */
void OLED_I2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * 函    数：OLED写I2C起始位
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * 函    数：OLED写I2C停止位
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * 函    数：OLED写I2C一个字节（不处理应答）
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
}

/**
  * 函    数：OLED写命令
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command);
	OLED_I2C_Stop();
}

/**
  * 函    数：OLED写数据
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * 函    数：OLED设置光标位置
  * 参    数：Y 以左上角为原点，向下方向的坐标，范围：0~7
  * 参    数：X 以左上角为原点，向右方向的坐标，范围：0~127
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * 函    数：OLED清屏
  */
void OLED_Clear(void)
{
	uint8_t i, j;
	for (i = 0; i < 8; i++)
	{
		OLED_SetCursor(i, 0);
		for (j = 0; j < 128; j++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * 函    数：OLED显示一个字符
  * 参    数：Line 行位置，范围：1~4
  * 参    数：Column 列位置，范围：1~16
  * 参    数：Char 要显示的一个字符，范围：ASCII可见字符
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * 函    数：OLED显示字符串
  * 参    数：Line 起始行位置，范围：1~4
  * 参    数：Column 起始列位置，范围：1~16
  * 参    数：String 要显示的字符串，范围：ASCII可见字符组成的字符串
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * 函    数：OLED次方函数
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * 函    数：OLED显示无符号数字
  * 参    数：Line 起始行位置，范围：1~4
  * 参    数：Column 起始列位置，范围：1~16
  * 参    数：Number 要显示的数字，范围：0~4294967295
  * 参    数：Length 要显示数字的长度，范围：1~10
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * 函    数：OLED显示有符号数字
  * 参    数：Line 起始行位置，范围：1~4
  * 参    数：Column 起始列位置，范围：1~16
  * 参    数：Number 要显示的数字，范围：-2147483648~2147483647
  * 参    数：Length 要显示数字的长度，范围：1~10
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * 函    数：OLED显示十六进制数字（大写字母）
  * 参    数：Line 起始行位置，范围：1~4
  * 参    数：Column 起始列位置，范围：1~16
  * 参    数：Number 要显示的数字，范围：0~0xFFFFFFFF
  * 参    数：Length 要显示数字的长度，范围：1~8
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * 函    数：OLED显示二进制数字
  * 参    数：Line 起始行位置，范围：1~4
  * 参    数：Column 起始列位置，范围：1~16
  * 参    数：Number 要显示的数字，范围：0~1111111111111111
  * 参    数：Length 要显示数字的长度，范围：1~16
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * 函    数：OLED初始化
  */
void OLED_Init(void)
{
	OLED_I2C_Init();			//先调用底层的驱动，配置GPIO

	Delay_ms(100);				//等OLED上电稳定后再发初始化命令

	OLED_WriteCommand(0xAE);	//关闭显示

	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);

	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);

	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);

	OLED_WriteCommand(0x40);	//设置显示开始行

	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置

	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);

	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭
	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示

	OLED_Clear();				//OLED清屏
}

/*********************** 以下为画图（动画）用的显存函数 ***********************/

/* 显存缓冲区：128列 * 8页，每个字节竖着存8个像素点
 * 画图时先改这个数组，最后调用OLED_Update()一次性发到屏幕上
 */
uint8_t OLED_DisplayBuf[8][128];

/**
  * 函    数：OLED连续写多个数据（一次I2C通信发多个字节，比逐个发快很多）
  * 参    数：Data 要写入数据的首地址
  * 参    数：Count 要写入数据的数量
  */
void OLED_WriteDataArray(uint8_t *Data, uint8_t Count)
{
	uint8_t i;
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	for (i = 0; i < Count; i++)
	{
		OLED_I2C_SendByte(Data[i]);
	}
	OLED_I2C_Stop();
}

/**
  * 函    数：把显存数组的内容发送到OLED屏幕
  */
void OLED_Update(void)
{
	uint8_t j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		OLED_WriteDataArray(OLED_DisplayBuf[j], 128);
	}
}

/**
  * 函    数：把显存数组全部清零（只清数组，调用OLED_Update()后屏幕才会变黑）
  */
void OLED_ClearBuffer(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		for (i = 0; i < 128; i++)
		{
			OLED_DisplayBuf[j][i] = 0x00;
		}
	}
}

/**
  * 函    数：在显存中画一个点
  * 参    数：X 横坐标，范围：0~127（向右）
  * 参    数：Y 纵坐标，范围：0~63（向下）
  */
void OLED_DrawPoint(int16_t X, int16_t Y)
{
	if (X < 0 || X > 127 || Y < 0 || Y > 63) {return;}	//超出屏幕的点不画
	OLED_DisplayBuf[Y / 8][X] |= 0x01 << (Y % 8);		//Y/8是第几页，Y%8是页内第几位
}

/**
  * 函    数：在显存中画一个矩形
  * 参    数：X Y 左上角坐标
  * 参    数：Width Height 宽和高
  * 参    数：IsFilled 1：实心，0：空心（只画边框）
  */
void OLED_DrawRectangle(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint8_t IsFilled)
{
	int16_t i, j;
	if (IsFilled)
	{
		for (i = X; i < X + Width; i++)
		{
			for (j = Y; j < Y + Height; j++)
			{
				OLED_DrawPoint(i, j);
			}
		}
	}
	else
	{
		for (i = X; i < X + Width; i++)
		{
			OLED_DrawPoint(i, Y);					//上边
			OLED_DrawPoint(i, Y + Height - 1);		//下边
		}
		for (j = Y; j < Y + Height; j++)
		{
			OLED_DrawPoint(X, j);					//左边
			OLED_DrawPoint(X + Width - 1, j);		//右边
		}
	}
}

/**
  * 函    数：在显存中画一张图片（图片数据是取模得到的数组）
  * 参    数：X Y 图片左上角坐标
  * 参    数：Width Height 图片的宽和高
  * 参    数：Image 图片数组的首地址（取模方式：纵向8点、低位在上）
  */
void OLED_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image)
{
	int16_t i, j;
	for (j = 0; j < Height; j++)
	{
		for (i = 0; i < Width; i++)
		{
			if (Image[(j / 8) * Width + i] & (0x01 << (j % 8)))		//取出这个点对应的那一位
			{
				OLED_DrawPoint(X + i, Y + j);
			}
		}
	}
}
