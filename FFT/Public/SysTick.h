#ifndef _SysTick_H
#define _SysTick_H

#include "system.h"
#define SWD_ENABLE         0X01
void SysTick_Init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);
void JTAG_Set(u8 mode);


#endif
