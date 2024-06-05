#ifndef _i2c_H
#define _i2c_H

#include "stm32f10x.h"

#define DEV_NUM		5

typedef struct i2c_t {
	GPIO_TypeDef* GPIOx_SCL;
	GPIO_TypeDef* GPIOx_SDA;
	uint32_t RCC_SCL;
	uint32_t RCC_SDA;
	GPIO_InitTypeDef SCL;
	GPIO_InitTypeDef SDA;
}i2c;

void select_i2c(uint8_t num);

void iic_init(void);
void SDA_OUT(void);
void SDA_IN(void);
void iic_start(void);
void iic_stop(void);
uint8_t iic_wait_ack(void);
void iic_ack(void);
void iic_nack(void);
void iic_send_byte(uint8_t data);
uint8_t iic_read_byte(uint8_t ack);
uint8_t iic_read_byte(uint8_t ack);

#endif
