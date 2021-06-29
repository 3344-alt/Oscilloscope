#include "ui.h"
#include "font.h"
#include "key.h"
#include "time.h"
#include "adc.h"
extern filters filter[8];
extern filters *filter_Ready;
//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
  u16  r,g,b,rgb;   
  b=(c>>0)&0x1f;
  g=(c>>5)&0x3f;
  r=(c>>11)&0x1f;	 
  rgb=(b<<11)+(g<<5)+(r<<0);		 
  return(rgb);

}

int sin_Wave[100]={
	60,130,190,250,310,370,430,480,540,590,640,680,730,770,810,840,880,900,930,950,970,980,990,1000,1000,
	1000,1000,990,980,970,950,930,900,880,840,810,770,730,680,640,590,540,480,430,370,310,250,190,130,60,
	-60,-130,-190,-250,-310,-370,-430,-480,-540,-590,-640,-680,-730,-770,-810,-840,-880,-900,-930,-950,-970,-980,-990,-1000,-1000,
	-1000,-1000,-990,-980,-970,-950,-930,-900,-880,-840,-810,-770,-730,-680,-640,-590,-540,-480,-430,-370,-310,-250,-190,-130,-60
};


_oscill_dev oscilldev;
void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc) 
{//Bresenham算法 
    unsigned short  a,b; 
    int c; 
    a=0; 
    b=R; 
    c=3-2*R; 
    while (a<b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc);     //        7 
        Gui_DrawPoint(X-a,Y+b,fc);     //        6 
        Gui_DrawPoint(X+a,Y-b,fc);     //        2 
        Gui_DrawPoint(X-a,Y-b,fc);     //        3 
        Gui_DrawPoint(X+b,Y+a,fc);     //        8 
        Gui_DrawPoint(X-b,Y+a,fc);     //        5 
        Gui_DrawPoint(X+b,Y-a,fc);     //        1 
        Gui_DrawPoint(X-b,Y-a,fc);     //        4 

        if(c<0) c=c+4*a+6; 
        else 
        { 
            c=c+4*(a-b)+10; 
            b-=1; 
        } 
       a+=1; 
    } 
    if (a==b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y-b,fc); 
        Gui_DrawPoint(X-a,Y-b,fc); 
        Gui_DrawPoint(X+b,Y+a,fc); 
        Gui_DrawPoint(X-b,Y+a,fc); 
        Gui_DrawPoint(X+b,Y-a,fc); 
        Gui_DrawPoint(X-b,Y-a,fc); 
    } 
	
} 
//画线函数，使用Bresenham 画线算法
void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color)   
{
int dx,             // difference in x's
    dy,             // difference in y's
    dx2,            // dx,dy * 2
    dy2, 
    x_inc,          // amount in pixel space to move during drawing
    y_inc,          // amount in pixel space to move during drawing
    error,          // the discriminant i.e. error i.e. decision variable
    index;          // used for looping	


	Lcd_SetXY(x0,y0);
	dx = x1-x0;//计算x距离
	dy = y1-y0;//计算y距离

	if (dx>=0)
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
		dx    = -dx;  
	} 
	
	if (dy>=0)
	{
		y_inc = 1;
	} 
	else
	{
		y_inc = -1;
		dy    = -dy; 
	} 

	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)//x距离大于y距离，那么每个x轴上只有一个点，每个y轴上有若干个点
	{//且线的点数等于x距离，以x轴递增画点
		// initialize error term
		error = dy2 - dx; 

		// draw the line
		for (index=0; index <= dx; index++)//要画的点数不会超过x距离
		{
			//画点
			Gui_DrawPoint(x0,y0,Color);
			
			// test if error has overflowed
			if (error >= 0) //是否需要增加y坐标值
			{
				error-=dx2;
				// move to next line
				y0+=y_inc;//增加y坐标值
			} // end if error overflowed

			// adjust the error term
			error+=dy2;

			// move to the next pixel
			x0+=x_inc;//x坐标值每次画点后都递增1
		} // end for
	} // end if |slope| <= 1
	else//y轴大于x轴，则每个y轴上只有一个点，x轴若干个点
	{//以y轴为递增画点
		// initialize error term
		error = dx2 - dy; 

		// draw the line
		for (index=0; index <= dy; index++)
		{
			// set the pixel
			Gui_DrawPoint(x0,y0,Color);

			// test if error overflowed
			if (error >= 0)
			{
				error-=dy2;

				// move to next line
				x0+=x_inc;
			} // end if error overflowed

			// adjust the error term
			error+=dx2;

			// move to the next pixel
			y0+=y_inc;
		} // end for
	} // end else |slope| > 1
}



void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc)
{
	Gui_DrawLine(x,y,x+w,y,0xEF7D);
	Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
	Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
	Gui_DrawLine(x,y,x,y+h,0xEF7D);
    Gui_DrawLine(x+1,y+1,x+1+w-2,y+1+h-2,bc);
}
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode)
{
	if (mode==0)	{
		Gui_DrawLine(x,y,x+w,y,0xEF7D);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
		Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
		Gui_DrawLine(x,y,x,y+h,0xEF7D);
		}
	if (mode==1)	{
		Gui_DrawLine(x,y,x+w,y,0x2965);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xEF7D);
		Gui_DrawLine(x,y+h,x+w,y+h,0xEF7D);
		Gui_DrawLine(x,y,x,y+h,0x2965);
	}
	if (mode==2)	{
		Gui_DrawLine(x,y,x+w,y,0xffff);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xffff);
		Gui_DrawLine(x,y+h,x+w,y+h,0xffff);
		Gui_DrawLine(x,y,x,y+h,0xffff);
	}
}


/**************************************************************************************
功能描述: 在屏幕显示一凸起的按钮框
输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
输    出: 无
**************************************************************************************/
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2)
{
	Gui_DrawLine(x1,  y1,  x2,y1, GRAY2);  //H
	Gui_DrawLine(x1+1,y1+1,x2,y1+1, GRAY1);  //H
	Gui_DrawLine(x1,  y1,  x1,y2, GRAY2);  //V
	Gui_DrawLine(x1+1,y1+1,x1+1,y2, GRAY1);  //V
	Gui_DrawLine(x1,  y2,  x2,y2, WHITE);  //H
	Gui_DrawLine(x2,  y1,  x2,y2, WHITE);  //V
}

/**************************************************************************************
功能描述: 在屏幕显示一凹下的按钮框
输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
输    出: 无
**************************************************************************************/
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2)
{
	Gui_DrawLine(x1,  y1,  x2,y1, WHITE); //H
	Gui_DrawLine(x1,  y1,  x1,y2, WHITE); //V
	
	Gui_DrawLine(x1+1,y2-1,x2,y2-1, GRAY1);  //H
	Gui_DrawLine(x1,  y2,  x2,y2, GRAY2);  //H
	Gui_DrawLine(x2-1,y1+1,x2-1,y2, GRAY1);  //V
    Gui_DrawLine(x2  ,y1  ,x2,y2, GRAY2); //V
}


void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
	unsigned char i,j;
	unsigned short k,x0;
	x0=x;

	while(*s) 
	{	
		if((*s) < 128) 
		{
			k=*s;
			if (k==13) 
			{
				x=x0;
				y+=16;
			}
			else 
			{
				if (k>32) k-=32; else k=0;
	
			    for(i=0;i<16;i++)
				for(j=0;j<8;j++) 
					{
				    	if(asc16[k*16+i]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
						else 
						{
							if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
						}
					}
				x+=8;
			}
			s++;
		}
			
		else 
		{
		

			for (k=0;k<hz16_num;k++) 
			{
			  if ((hz16[k].Index[0]==*(s))&&(hz16[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<16;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
								else {
									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
								}
							}
				    }
				}
			  }
			s+=2;x+=16;
		} 
		
	}
}

void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
	unsigned char i,j;
	unsigned short k;

	while(*s) 
	{
		if( *s < 0x80 ) 
		{
			k=*s;
			if (k>32) k-=32; else k=0;

		    for(i=0;i<16;i++)
			for(j=0;j<8;j++) 
				{
			    	if(asc16[k*16+i]&(0x80>>j))	
					Gui_DrawPoint(x+j,y+i,fc);
					else 
					{
						if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
					}
				}
			s++;x+=8;
		}
		else 
		{

			for (k=0;k<hz24_num;k++) 
			{
			  if ((hz24[k].Index[0]==*(s))&&(hz24[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<24;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3]&(0x80>>j))
								Gui_DrawPoint(x+j,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
								else {
									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3+2]&(0x80>>j))	
								Gui_DrawPoint(x+j+16,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j+16,y+i,bc);
								}
							}
				    }
			  }
			}
			s+=2;x+=24;
		}
	}
}
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num)
{
	unsigned char i,j,k,c;
	//lcd_text_any(x+94+i*42,y+34,32,32,0x7E8,0x0,sz32,knum[i]);
//	w=w/8;

    for(i=0;i<32;i++)
	{
		for(j=0;j<4;j++) 
		{
			c=*(sz32+num*32*4+i*4+j);
			for (k=0;k<8;k++)	
			{
	
		    	if(c&(0x80>>k))	Gui_DrawPoint(x+j*2+k,y+i,fc);
				else {
					if (fc!=bc) Gui_DrawPoint(x+j*2+k,y+i,bc);
				}
			}
		}
	}
}



//虚线绘制
void Draw_GeZi(void)
{
	u8 i;
	for(i=24;i<104;i+=4)
	Gui_DrawPoint(14,i,20);
	for(i=24;i<104;i+=4)
	Gui_DrawPoint(34,i,20);
	for(i=24;i<104;i+=4)
	Gui_DrawPoint(54,i,20);
	for(i=24;i<104;i+=4)
	Gui_DrawPoint(74,i,20);
	for(i=24;i<104;i+=4)
	Gui_DrawPoint(94,i,20);
	for(i=24;i<104;i+=4)
	Gui_DrawPoint(114,i,20);
	for(i=24;i<104;i+=4)
	Gui_DrawPoint(114,i,20);
	for(i=14;i<114;i+=4)
	Gui_DrawPoint(i,64,40);
	
}


//波形绘制
extern u16 f;
extern u32 Freq;
extern float duty;
extern float width;
extern float vccd;
void Draw_Wave (void)
{
	u8 i,j;
	if(oscilldev.flag == 0)//判断是否采集完
	{
		for(i=14;i<114;i++)
	{
		clear_Line(i,0,i+1,255);
		if((i-14)%4==0)
		Gui_DrawPoint(i,64,40);
		if((i-14)%20==0)
		for(j=24;j<104;j+=4)
		Gui_DrawPoint(i,j,20);	
		Gui_DrawLine(i,((oscilldev.buffer[i]/50)/oscilldev.mvDiv)+oscilldev.y,i+1,((oscilldev.buffer[i+1]/50)/oscilldev.mvDiv)+oscilldev.y,0);
	}
		oscilldev.flag=1;
		oscilldev.x++;
	}
}



extern u8 menu;
extern u8 bi,bj;
u8 init_Flag=0,bk=0,width_Flag=0,clear_Flag=0,zx_flag=0;
extern u8 zx;
void Draw_Menu (void)
{
	if(bi==1)
	{
		if(width_Flag==0)
		Lcd_Clear(GRAY0);
		Draw_Width();
		width_Flag=1;
		clear_Flag=0;
		if(init_Flag==1)
		{
			TIM_Cmd(TIM1, DISABLE);
			TIM_Cmd(TIM2, ENABLE);
		}
		init_Flag=0;
	}
	if(bj==2)
	{
		if(bk==0)
			Lcd_Clear(GRAY0);			
		bk=1;
		Draw_Oscilldev();
	}
	if(bj==1)
	{
		if(clear_Flag==0)
		Lcd_Clear(GRAY0);
		if(init_Flag==0)
		{
			TIM_Cmd(TIM1, ENABLE);
			TIM_Cmd(TIM2, DISABLE);
		}
		init_Flag=1;
		Draw_Normal();
		clear_Flag=1;
		width_Flag=0;
		bk=0;
	}
	
}


u16 vccx;
void Draw_Oscilldev (void)
{
	vccx=(int)vccd;
	//Draw_Wave();
	move_Select();
	dataFilter(filter_Ready);
	vcc_Filter();
	if(bj==2)
	Draw_Sin(Freq,vccx);
//	if(bj==1)
//	Draw_maichong(Freq,vccx);
}





void Draw_Normal(void)
{
	char a[100];
	dataFilter(filter_Ready);
	vcc_Filter();
	if(clear_Flag==0)
	{
		Gui_DrawFont_GBK16(16,40,0,GRAY0,"VCC:");
		Gui_DrawFont_GBK16(16,20,0,GRAY0,"Freq:");
		Gui_DrawFont_GBK16(16,60,0,GRAY0,"cnt:");
	}
	sprintf(a,"%d",Freq);
	Gui_DrawFont_GBK16(56,20,0,GRAY0,a);
	sprintf(a,"%d",(int)vccd);
	Gui_DrawFont_GBK16(56,40,0,GRAY0,a);
	sprintf(a,"%d",zx);
	Gui_DrawFont_GBK16(56,60,0,GRAY0,a);
}



extern u8 ready_Flag;
enum  HZ{HZ100_250,HZ250_500,HZ500_1000,HZ1k_2k,HZ2k_5k,HZ5k_10k,HZ10k_100k,HZ100k_1M};
void Draw_Hz_Select (void)
{
	switch(ready_Flag)
	{
		case HZ100_250:
			Gui_DrawFont_GBK16(16,20,RED,GRAY0,"HZ100-250");
			break;
		case HZ250_500:
			Gui_DrawFont_GBK16(16,20,RED,GRAY0,"HZ250-500");
			break;
		case HZ500_1000:
			Gui_DrawFont_GBK16(16,20,RED,GRAY0,"HZ500-1000");
			break;
		case HZ1k_2k:
			Gui_DrawFont_GBK16(16,20,RED,GRAY0,"HZ1k-2k");
			break;
		case HZ2k_5k:
			Gui_DrawFont_GBK16(16,20,RED,GRAY0,"HZ2k-5k");
			break;
		case HZ5k_10k:
			Gui_DrawFont_GBK16(16,20,RED,GRAY0,"HZ5k-10k");
			break;
		case HZ10k_100k:
			Gui_DrawFont_GBK16(16,20,RED,GRAY0,"HZ10k-100k");
			break;
		case HZ100k_1M:
			Gui_DrawFont_GBK16(16,20,RED,GRAY0,"HZ100k-1M");
			break;
		default:
			break;
	}
}




void Draw_Width(void)
{
	char a[100];
	frep_Filter();
	if(width_Flag==0)
	{
		Gui_DrawFont_GBK16(16,20,0,GRAY0,"Duty:");
		Gui_DrawFont_GBK16(16,40,0,GRAY0,"Width:");
	}
	sprintf(a,"%.2f",duty);
	Gui_DrawFont_GBK16(56,20,0,GRAY0,a);
	sprintf(a,"%.3f",width);
	Gui_DrawFont_GBK16(64,40,0,GRAY0,a);
}




void Draw_maichong (u32 f,u16 vcc)
{
	int i=0,j=0,k=0,m=0;
	u8 t=(((float)(1000000/f))/oscilldev.time)*10;//获取对应的横坐标点数
	u8 y=((float)(vcc/2)/oscilldev.mvDiv)*13;
	for(j=1;j<51;j++)
	{
		if(j==1)
			i=14;
		for(m=i;i<(j*t)+14;i++)
		{
			clear_Line(i,0,i+1,255);
			if(i>114+my_abs(oscilldev.x))
				break;
			if((i-14)%4==0)
			Gui_DrawPoint(i,64,40);
			if((i-14)%20==0)
			for(k=24;k<104;k+=4)
			Gui_DrawPoint(i,k,20);
			Gui_DrawPoint(i+oscilldev.x,64-y+oscilldev.y,0);
			//Gui_DrawLine(i,64-y,i+1,64-y,0);
			if(i==(j*t)+13)
			Gui_DrawLine(i+oscilldev.x,64-y+oscilldev.y,i+oscilldev.x,64+y+oscilldev.y,0);
		}
		j++;
		for(m=i;i<j*t+14;i++)
		{
			clear_Line(i,0,i+1,255);
			if(i>114)
				break;
			if((i-14)%4==0)
			Gui_DrawPoint(i,64,40);
			if((i-14)%20==0)
			for(k=24;k<104;k+=4)
			Gui_DrawPoint(i,k,20);
			Gui_DrawPoint(i+oscilldev.x,64+y+oscilldev.y,0);
			//Gui_DrawLine(i,64-y,i+1,64-y,0);
			if(i==(j*t)+13)
			Gui_DrawLine(i+oscilldev.x,64-y+oscilldev.y,i+oscilldev.x,64+y+oscilldev.y,0);
		}
		m=i;
		if(i>114+oscilldev.x)
			break;
	}
	
	
}







void Draw_Sin(u32 f,u16 vcc)
{
	int i=0,j=0,k=0;
	float m=0,prop=0;
	u8 t=(((float)(1000000/f))/oscilldev.time)*20;//获取对应的横坐标点数
	u8 y=((float)(vcc/2)/oscilldev.mvDiv)*13;
	prop=(float)100/t;
	for(j=1;j<51;j++)
	{
		if(j==1)
			i=14+oscilldev.x;
		for(;i<j*t+14;i++)
		{
			m=(float)(sin_Wave[(int)(((j*t+14)-i)*prop)])/1000;
			m=(int)(m*y);
			clear_Line(i,0,i+1,255);
			if(i>114+my_abs(oscilldev.x))
				break;
			if((i-14)%4==0)
			Gui_DrawPoint(i,64,40);
			if((i-14)%20==0)
			for(k=24;k<104;k+=4)
			Gui_DrawPoint(i,k,20);
			Gui_DrawPoint(i+oscilldev.x,64+m+oscilldev.y,RED);
		}
	}
}







 u8 select=0,increase=0,reduce=0,flag=1;
void move_Select(void)
{
	int num=0;
	char a[10];
	zx_flag=zx;
	switch(menu)
		{
			case KEY0_PRES:
				reduce++;
				flag=0;
				Lcd_Clear(GRAY0);
				break;
			case  KEY1_PRES:
				select++;
				increase=0;
				reduce=0;
				break;		
			case  WKUP_PRES:
				increase++;
				flag=0;
				Lcd_Clear(GRAY0);
				break;
			default:
				break;
		}
	num=increase-reduce;
	Gui_DrawFont_Num32(0,0,0,GRAY0,select);
	if(select==LEVEL&&flag==0)//水平移动
	{
		oscilldev.x=num*10;
		flag=1;
	}
	if(select==VERTICAL&&flag==0)//竖直移动
	{
		oscilldev.y=num*10;
		flag=1;
	}
	if(select==LEVEL_SENSITIVITY&&flag==0)//竖直灵敏度
	{
		if(num==1)
			oscilldev.mvDiv=50;
		else
			oscilldev.mvDiv=500;
	}
	if(select==VERTICAL_SENSITIVITY&&flag==0)//水平灵敏度
	{
		if(num==1)
			oscilldev.time=10;
		if(num==2)
			oscilldev.time=100;
		else
			oscilldev.time=1;
		
	}
	if(select>=4)
		select=0;
}






u8 my_abs (int x)
{
	if(x>=0)
		x=x;
	if(x<0)
		x=-x;
	return x;
}









u16 max=0,min=0;
u8 wave_judge(void)
{
	u8 i=0;
	for(i=0,min=oscilldev.buffer[0];i<100;i++)
	{
		if(oscilldev.buffer[i]>max)
			max=oscilldev.buffer[i];
		if(oscilldev.buffer[i]<min)
			min=oscilldev.buffer[i];
	}
	return 0;
}