#ifndef __main_H
#define __main_H

#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "timer.h"
#include "tcs34725.h"
#include "rgb_ratio.h"
#include "cct.h"
#include "RTU.h"

#define PARAM_ADDRESS		0x08007800

#define COLOR_NONE			0
#define COLOR_MIDDLE		1
#define COLOR_WHITE			2
#define COLOR_YELLOW		3

extern uint8_t color_result[DEV_NUM];
extern COLOR_RGBC rgb[DEV_NUM];
extern uint32_t cct_buf[DEV_NUM];

#endif
