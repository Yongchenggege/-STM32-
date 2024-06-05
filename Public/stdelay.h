#ifndef  __STDELAY_H
#define  __STDELAY_H

#include "stm32f10x.h"

void stdelay_init(uint8_t SYSCLK);
void delay_us(uint32_t nus);
void stdelay_ms(uint16_t nms);

#endif
