#ifndef _adc_H
#define _adc_H

#include "system.h"

#define ARR 72-1
#define PSC 1000-1
#define ADC_LEN    256

typedef struct filter{  
		u16 arr;
		u16 psc;
		u32 min;
		u32 max;
		u32 now;
		u32 average;
} filters;




typedef struct vccfilter{  
		u16 cnt;
		float frep;
} vccfilters;
void ADC_Configuration(void);
void ADC1_Init(void);
void ADC1_DMA1_Init(void);
void TIM1_Init(filters *filter);
u8 dataFilter( filters *filter);
void vcc_Filter (void);
#endif
