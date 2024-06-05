#ifndef __rgb_ratio_H
#define __rgb_ratio_H

#include "stm32f10x.h"
#include "stdio.h"
#include "tcs34725.h"
#include "RTU.h"

#define RGB_RATIO_ENABLE	0

#if RGB_RATIO_ENABLE

typedef struct {
	float k1;
	float k2;
	float k3;
	float r_g_m;
	float r_b_m;
	float g_b_m;
	float r_g_w;
	float r_b_w;
	float g_b_w;
	float r_g_y;
	float r_b_y;
	float g_b_y;
}param_t;

extern param_t param[5];
extern uint8_t color_result[DEV_NUM];

void color_recognition(uint8_t num,COLOR_RGBC *rgbc,uint8_t *color);

void upload_color_sensor_data(uint16_t base_addr,uint16_t n);

void upload_sampling_data(COLOR_RGBC *rgbc,uint32_t len);

void wirte_color_parameters(uint16_t index,uint8_t *data);

void param_init(void);

void param_updata(uint32_t num,uint8_t *data);

#define COLOR_RECOGNITION()		color_recognition(i,&rgb[i],&color_result[i])

#define PARAM_INIT()			param_init()

#define PARAM_UPDATA(x,y)			param_updata(x,y)

#define UPLOAD_SAMPLING_DATA()	upload_sampling_data(rgb,DEV_NUM)

#endif
#endif
