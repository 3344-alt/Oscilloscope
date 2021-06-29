#include "time.h"
#include "key.h"
/*******************************************************************************
* 功	能		   : 测量方波频率和脉宽
* 输    入         : 无
* 输    出         : 无
* 思	路		   : 正向脉宽=正向占空比*周期 捕获正向占空比和周期
* 定	时		器 :TIM2记录时间，TIM3计算
*******************************************************************************/













u8 Edge_Flag;  //高低电平的标志位
u16 Rising,Falling,Rising_Last;   

//定时器2输入捕获中断初始化
void TIM2_Cap_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct;   
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_ICInitTypeDef TIM_ICInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;   //浮空
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    TIM_InitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_InitStruct.TIM_CounterMode=TIM_CounterMode_Up;  //向上计数
    TIM_InitStruct.TIM_Period=0xffff;
    TIM_InitStruct.TIM_Prescaler=71;                //分频系数 当分频系数越大时，可测量频率最小值越小
    TIM_TimeBaseInit(TIM2,&TIM_InitStruct);
    
    TIM_ICInitStruct.TIM_Channel=TIM_Channel_3;
    TIM_ICInitStruct.TIM_ICFilter=0x00;         //不滤波
    TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising;     //第一次是检测上升沿进入中断
    TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPSC_DIV1;
    TIM_ICInitStruct.TIM_ICSelection=TIM_ICSelection_IndirectTI; //直接映射
    TIM_ICInit(TIM2,&TIM_ICInitStruct);
    TIM_ITConfig(TIM2,TIM_IT_CC3,ENABLE);
    
    NVIC_InitStruct.NVIC_IRQChannel=TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x00;    //抢占优先级
    NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x01;          //响应优先级
    NVIC_Init(&NVIC_InitStruct);
    
    TIM_Cmd(TIM2,ENABLE);     //使能
}

u16 cnt;
extern u8 t;
void TIM2_IRQHandler(void)
{ 
    if(TIM_GetITStatus(TIM2,TIM_IT_CC3)!=RESET)    //捕获到上升沿
    {
        if(Edge_Flag==1)
        {
            Rising=TIM2->CCR3;    //第一次检测到下降沿
            TIM_OC3PolarityConfig(TIM2,TIM_ICPolarity_Rising);  //再次改为上升沿触发
            Edge_Flag++; 
        }
        else if(Edge_Flag==2)
        {
            Rising_Last=TIM2->CCR3;
            TIM_OC3PolarityConfig(TIM2,TIM_ICPolarity_Rising);
            Edge_Flag=0;     //标志位清0
            TIM_SetCounter(TIM2,0); //定时器清0
        }
        else
        {
            Falling=TIM2->CCR3;     //第一次检测到上升沿
            TIM_OC3PolarityConfig(TIM2,TIM_ICPolarity_Falling);    //将上升沿触发改为下降沿触发
            Edge_Flag++;
			cnt++;
        }
    }
    TIM_ClearITPendingBit(TIM2,TIM_IT_CC3);    //清除标志位
}

//width滤波
u16 f;
differ diff[10];
u16 l=0;
float width;
float duty;
float maxv=0;
void frep_Filter (void)
{
	u16 m[10],min=0,max=0,x=0;
	u8 i=0;
	u8 flag=0,exist=0,t=0;
	if((int)((float)(Rising-Falling)/(float)(Rising_Last-Falling)*100)>10&&(int)((float)(Rising-Falling)/(float)(Rising_Last-Falling)*100)<90)
	duty=((float)(Rising-Falling)/(float)(Rising_Last-Falling)*100);
	x=1000000/(Rising_Last-Falling);
	if(x>=100&&x<=1000)
		t=1;
	else
		t=0;
	if(l>4000)
	{
		for(i=0;i<10;i++)
			diff[i].cnt=0;
			l=0;
			maxv=0;
	}
	if(t==1)
	{
		if(flag!=1)
			for( i=0;i<10;i++)
			{
				m[i]=x-diff[i].frep;
				if(m[i]==0)
				{
					diff[i].cnt++;
					i=9;
				}
				else if(m[i]==x)
				{
					diff[i].frep=x;
					break;
				}
				if(i==9)
					flag=1;
			}
		if(flag==1)
		{
			for( i=0;i<10;i++)
			{
				m[i]=x-diff[i].frep;
				if(m[i]==0)
				{
					diff[i].cnt++;
					i=9;
				}
			}
			for( i=0,min=diff[0].cnt;i<10;i++)
				if(diff[i].cnt<min)
				{
					min=diff[i].cnt;
					exist=i;
				}
			diff[exist].frep=x;
			diff[exist].cnt=0;
		}
		for( i=0;i<10;i++)
			if(diff[i].cnt>max)
			{
				max=diff[i].cnt;
				exist=i;
			}
			max=0;
		f=diff[exist].frep;
		width=((float)((1.000/f)*duty)*10000);
		}
		l++;
}


