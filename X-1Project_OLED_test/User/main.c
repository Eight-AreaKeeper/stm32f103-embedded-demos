#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Gorilla.h"

/* 舞步表：每一步播放第几帧图片，大猩猩往左/右挪多少像素 */
#define STEP_COUNT	8
const uint8_t StepFrame[STEP_COUNT] = {0, 1, 2, 3, 0, 3, 2, 1};
const int8_t  StepMove[STEP_COUNT]  = {0, -4, -6, -4, 0, 4, 6, 4};

int main(void)
{
	uint8_t Step = 0;			//现在跳到舞步表的第几步
	int16_t GorillaX;			//大猩猩左上角的横坐标
	int16_t NoteY;				//音符的纵坐标（会往上飘）

	OLED_Init();

	while(1)
	{
		/* 1. 根据舞步表算出这一帧的位置 */
		GorillaX = (128 - GORILLA_WIDTH) / 2 + StepMove[Step];	//以屏幕中间为基准左右摇摆
		NoteY = 40 - (Step % 4) * 10;								//音符每4步从下往上飘一次

		/* 2. 画这一帧：清空显存 -> 画大猩猩 -> 画音符 -> 发送到屏幕 */
		OLED_ClearBuffer();
		OLED_ShowImage(GorillaX, 64 - GORILLA_HEIGHT, GORILLA_WIDTH, GORILLA_HEIGHT, Gorilla_Frames[StepFrame[Step]]);
		if (Step < 4)
		{
			OLED_ShowImage(8, NoteY, NOTE_WIDTH, NOTE_HEIGHT, Music_Note);		//前4步音符在左边
		}
		else
		{
			OLED_ShowImage(112, NoteY, NOTE_WIDTH, NOTE_HEIGHT, Music_Note);	//后4步音符在右边
		}
		OLED_Update();

		/* 3. 下一步，跳完一轮再从头开始 */
		Step++;
		if (Step >= STEP_COUNT)
		{
			Step = 0;
		}

		/* 4. 停一下，控制跳舞的速度 */
		Delay_ms(150);
	}
}
