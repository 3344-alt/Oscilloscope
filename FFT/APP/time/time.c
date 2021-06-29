#include "time.h"
#include "key.h"
/*******************************************************************************
* ��	��		   : ��������Ƶ�ʺ�����
* ��    ��         : ��
* ��    ��         : ��
* ˼	·		   : ��������=����ռ�ձ�*���� ��������ռ�ձȺ�����
* ��	ʱ		�� :TIM2��¼ʱ�䣬TIM3����
*******************************************************************************/













u8 Edge_Flag;  //�ߵ͵�ƽ�ı�־λ
u16 Rising,Falling,Rising_Last;   

//��ʱ��2���벶���жϳ�ʼ��
void TIM2_Cap_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct;   
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_ICInitTypeDef TIM_ICInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;   //����
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    TIM_InitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_InitStruct.TIM_CounterMode=TIM_CounterMode_Up;  //���ϼ���
    TIM_InitStruct.TIM_Period=0xffff;
    TIM_InitStruct.TIM_Prescaler=71;                //��Ƶϵ�� ����Ƶϵ��Խ��ʱ���ɲ���Ƶ����СֵԽС
    TIM_TimeBaseInit(TIM2,&TIM_InitStruct);
    
    TIM_ICInitStruct.TIM_Channel=TIM_Channel_3;
    TIM_ICInitStruct.TIM_ICFilter=0x00;         //���˲�
    TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising;     //��һ���Ǽ�������ؽ����ж�
    TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPSC_DIV1;
    TIM_ICInitStruct.TIM_ICSelection=TIM_ICSelection_IndirectTI; //ֱ��ӳ��
    TIM_ICInit(TIM2,&TIM_ICInitStruct);
    TIM_ITConfig(TIM2,TIM_IT_CC3,ENABLE);
    
    NVIC_InitStruct.NVIC_IRQChannel=TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x00;    //��ռ���ȼ�
    NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x01;          //��Ӧ���ȼ�
    NVIC_Init(&NVIC_InitStruct);
    
    TIM_Cmd(TIM2,ENABLE);     //ʹ��
}

u16 cnt;
extern u8 t;
void TIM2_IRQHandler(void)
{ 
    if(TIM_GetITStatus(TIM2,TIM_IT_CC3)!=RESET)    //����������
    {
        if(Edge_Flag==1)
        {
            Rising=TIM2->CCR3;    //��һ�μ�⵽�½���
            TIM_OC3PolarityConfig(TIM2,TIM_ICPolarity_Rising);  //�ٴθ�Ϊ�����ش���
            Edge_Flag++; 
        }
        else if(Edge_Flag==2)
        {
            Rising_Last=TIM2->CCR3;
            TIM_OC3PolarityConfig(TIM2,TIM_ICPolarity_Rising);
            Edge_Flag=0;     //��־λ��0
            TIM_SetCounter(TIM2,0); //��ʱ����0
        }
        else
        {
            Falling=TIM2->CCR3;     //��һ�μ�⵽������
            TIM_OC3PolarityConfig(TIM2,TIM_ICPolarity_Falling);    //�������ش�����Ϊ�½��ش���
            Edge_Flag++;
			cnt++;
        }
    }
    TIM_ClearITPendingBit(TIM2,TIM_IT_CC3);    //�����־λ
}

//width�˲�
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


