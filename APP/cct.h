#ifndef __cct_H
#define __cct_H

#include "stm32f10x.h"
#include "main.h"
#include "tcs34725.h"

#define FLASH_ENABLE 	0
#define CCT_ENABLE		1

#if	CCT_ENABLE

typedef struct {
	uint32_t cct_m_min;
	uint32_t cct_m_max;
	uint32_t cct_w_min;
	uint32_t cct_w_max;
	uint32_t cct_y_min;
	uint32_t cct_y_max;
}param_t;

void upload_color_sensor_data(uint16_t base_addr,uint16_t n);

void upload_sampling_data(uint32_t *cct,uint32_t len);

void color_recognition(uint8_t num,COLOR_RGBC *rgbc,uint32_t *cct,uint8_t *color);

void param_init(void);

void param_updata(uint32_t num,uint8_t *data);

void wirte_color_parameters(uint16_t index,uint8_t *data);

#define COLOR_RECOGNITION()		color_recognition(i,&rgb[i],&cct_buf[i],&color_result[i]);

#define PARAM_INIT()			param_init()

#define PARAM_UPDATA(x,y)			param_updata(x,y)

#define UPLOAD_SAMPLING_DATA()	upload_sampling_data(cct_buf,DEV_NUM)

#endif
#endif
