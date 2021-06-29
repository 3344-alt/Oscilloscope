#ifndef _TIME_H
#define _TIME_H

#include "stm32f10x.h"

typedef struct diff{  
		u16 cnt;
		u16 frep;
} differ;
void TIM2_Cap_Init(void);
void frep_Filter (void);
#endif

