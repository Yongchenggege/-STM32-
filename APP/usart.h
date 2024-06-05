#ifndef _usart_H
#define _usart_H

#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"

void usart1_init(uint32_t BaudRate);
void usart2_init(uint32_t BaudRate);

void usart2_callBack(void);
void usart2_receive_process(void);

extern uint8_t usart2_buf[256];
extern uint16_t usart2_valid_data_len;
void usart2_send_buf(uint8_t *buf,uint32_t len);

#define usart2_buf_clean()	memset(usart2_buf,0,sizeof(usart2_buf)/sizeof(usart2_buf[0]))

#endif
