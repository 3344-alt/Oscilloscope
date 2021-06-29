#ifndef _UI_H
#define _UI_H

#include "stm32f10x.h"
#include "lcd.h"
#include "ui.h"
#include "SysTick.h"

#define LEVEL 0
#define VERTICAL 1
#define LEVEL_SENSITIVITY 2
#define VERTICAL_SENSITIVITY 3


#define SIN 1
#define FANG 0
#define ANGLE 2
//����ʾ��������
typedef struct 
{
	u16 buffer[125];//���ݴ����
	u8 	flag;		//�Ƿ����
	u8  tDiv;		//һ�����us
	u16 mvDiv;      //һ����ʾ����mv
	u32  time;      //ʱ��    
    //float resolut;  //ÿ��ADԭʼֵ������ĵ�ѹ(��λΪmv)
	int x;		//���������x��ƫ����
	int y;		//���������y��ƫ����
}_oscill_dev;

u16 LCD_BGR2RGB(u16 c);
void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc); 
void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color);  
void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc);
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode);
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2);
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2);
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num) ;
void Draw_GeZi(void);
void Draw_Wave (void);
void Draw_Menu (void);
void Draw_Oscilldev (void);
void Draw_Normal(void);
void Draw_Hz_Select(void);
void Draw_Width(void);
void Draw_maichong (u32 f,u16 vcc);
void Draw_Sin(u32 f,u16 vcc);
u8 my_abs (int x);
void move_Select(void);
u8 wave_judge(void);
#endif


