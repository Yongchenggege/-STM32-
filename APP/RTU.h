#ifndef __RTU_H
#define __RTU_H

#include "stm32f10x.h"
#include "main.h"
#include "usart.h"
#include "crc16.h"

#define MODBUS_RTU_ADDR		0x01

typedef struct {
	
	uint8_t valid;
	
	uint8_t addr;
	
	uint8_t code;
	
	uint32_t pack_len;
	
	uint16_t crc_m;
	
	uint16_t crc_s;
	
}RTU_t;

void RTU_unpack(uint8_t *buf,RTU_t *RTU_pack);

#endif
