#include <stdio.h>
#include "string.h"
#include "adc.h"
#include "fft_calculate.h"
#include "ui.h"
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
filters filter[8]={{72-1,2000-1,100,250},//100-250
{72-1,1000-1,250,500},//250-500
{72-1,500-1,500,1000},//500-1000
{72-1,250-1,1000,2000},//1000-2000
{72-1,100-1,2000,5000},//2000-5000
{72-1,50-1,5000,10000},//5k-10k
{72-1,5-1,10000,100000},//10k-100k
{36-1,1-1,100000,1000000}};//100k-1M
filters *filter_Ready;
/*******************************************************************************
* 功	能		   : 测量正弦波频率
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
uint16_t ADC_Value[ADC_LEN];
void ADC1_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

 //   ADC_TempSensorVrefintCmd(ENABLE);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_1Cycles5);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);

    ADC_DMACmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));  
}

void ADC1_DMA1_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_Value;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = ADC_LEN;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC | DMA_IT_HT, ENABLE);

    DMA_Cmd(DMA1_Channel1, ENABLE);
}

void TIM1_Init(filters *filter)
{
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    TIM_OCInitTypeDef         TIM_OCInitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
    TIM_TimeBaseStructure.TIM_Period = filter->arr;          
    TIM_TimeBaseStructure.TIM_Prescaler = filter->psc;       
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
    TIM_OCInitStructure.TIM_Pulse = 8000; 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_Cmd(TIM1, DISABLE);
}
extern _oscill_dev oscilldev;
u16 ave[NPT+10];
void DMA1_Channel1_IRQHandler(void)
{
	u16 i = 0;
	
  DMA_Cmd(DMA1_Channel1, DISABLE);

  DMA_ClearITPendingBit(DMA_IT_HT);
  DMA_ClearITPendingBit(DMA1_IT_TC1);
	for(i=0;i<NPT;i++)
	{
		InBufArray[i] = ((signed short)(ADC_Value[i])) << 16;//
		ave[i]=ADC_Value[i];
	}
	oscilldev.flag=0;	
	//cr4_fft_1024_stm32(OutBufArray, InBufArray, NPT);
	cr4_fft_256_stm32(OutBufArray, InBufArray, NPT);
	GetPowerMag();	
	DMA_Cmd(DMA1_Channel1, ENABLE);
}


/*******************************************************************************
* 功	能		   : 滤除超过1%的信号
* 输    入         : 结构体filter
* 输    出         : 满足了几个条件
* 条	件		   : 1.当前值小于最大值加上当前值的1% 2.当前值大于最小值减去当前值的1%
*******************************************************************************/
u32 Freq;
u16 Max_Val;
u16 Row;
u32 average[10]={0};
u8 del,del1,del2;
u8 dataFilter( filters *filter)
{
	u16 i=0;
	u8 status=0;//标志位
	u32 sum=0;
	TIM_SetCompare1(TIM1,10);
	for( i=2;i<NPT/2;i++)
		{
			if(MagBufArray[i]>Max_Val)
			{
				Max_Val=MagBufArray[i];
				Row=i;
			}
		}
	filter->now=(72000000)/(NPT*(filter->arr+1)*(filter->psc+1)/Row);
	/*average[m]=filter->now;
	sum+=average[m];
	m++;
	if(m==10)
	{
		fill=1;//是否填充完
		m=0;
	}*/
		filter->average=sum/10;
		if(filter->now<(filter->max+filter->now/100))
			status++;
		if(filter->now>(filter->min-filter->now/100))
			status++;
		/*if(status==2)
		{
			error=filter->now/100;//获取误差范围
			if(filter->now<(filter->average+error))
				status++;
			if(filter->now>(filter->average-error))
				status++;
		}*/
		if(status==2)
			Freq=filter->now;
	status=0;
	Max_Val=0;
	return status;
}


float vccd;
//vcc滤波
void vcc_Filter (void)
{
	int i=0,j=0,k=0;
	u32 num=0;
	u16 max[8]={0},min[8]={0};
	for(i=0;i<8;i++)
	{
		max[i]=0;
		min[i]=0;
	for(j=0;j<32;j++)
	{
		if(ave[i*32+j]>max[i])
			max[i]=ave[i*32+j];
		if(ave[i*32+j]<min[i])
			min[i]=ave[i*32+j];
	}
		num=num+(max[i]-min[i]);
	}
	num=num/8;
	vccd=((float)(num*6)/4095)*1000;
	vccd=vccd-1970;
}













