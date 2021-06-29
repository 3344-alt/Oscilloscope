#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "fft_calculate.h"
#include "adc.h"
#include "time.h"
#include "lcd.h"
#include "ui.h"
#include "key.h"
extern _oscill_dev oscilldev;
u8 menu;
u8 bi=0,bj=0,ready_Flag=0,zx=0;
extern filters filter[8];
extern filters *filter_Ready;
int main()
{
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	Lcd_Init();
	KEY_Init();
	oscilldev.mvDiv=500;
	oscilldev.x=0;
	oscilldev.y=0;
	oscilldev.time=1;
	Lcd_Clear(GRAY0);
	while(menu!=KEY1_PRES&&menu!=KEY0_PRES)//测量频率选择
	{
		LCD_LED_SET;//通过IO控制背光亮
		Draw_Hz_Select();
		menu=KEY_Scan(0);
		if(menu==WKUP_PRES)
		{
			ready_Flag++;
			Lcd_Clear(GRAY0);
		}
		if(ready_Flag>=7)
			ready_Flag=7;
	}
	filter_Ready=&filter[ready_Flag];
	TIM1_Init(filter_Ready);
	ADC1_DMA1_Init();
	ADC1_Init();
	TIM2_Cap_Init();
	while(1)
	{
		switch(menu)
		{
			case KEY0_PRES:
				if(bj<=1)
				{
					bj=0;
					bi=1;
				}
				break;
			case  KEY1_PRES:
				bj=1;
				bi=0;
				break;
			case  WKUP_PRES:
				bj=2;
				bi=0;
				break;
			default:
				break;
		}
		menu=KEY_Scan(0);
		LCD_LED_SET;//通过IO控制背光亮
		Draw_Menu();
	}
}

