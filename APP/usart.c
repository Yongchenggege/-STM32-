#include "usart.h"
#include "stdio.h"

uint8_t usart2_buf[256]={0};
uint16_t usart2_cnt=0;
uint16_t usart2_cnt_last=0;
uint16_t usart2_valid_data_len = 0;


void usart1_init(uint32_t BaudRate)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	USART_InitTypeDef  USART_InitStruct;
	
	USART_InitStruct.USART_BaudRate=BaudRate;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	
	USART_Init(USART1,&USART_InitStruct);
	
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	
	NVIC_Init(&NVIC_InitStruct);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	USART_ClearFlag(USART1,USART_FLAG_TC);
	
	USART_Cmd(USART1,ENABLE);
}

void usart2_init(uint32_t BaudRate)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	USART_InitTypeDef USART_InitStruct;
	
	USART_InitStruct.USART_BaudRate = BaudRate;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	USART_Init(USART2,&USART_InitStruct);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	
	NVIC_Init(&NVIC_InitStruct);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	USART_ClearFlag(USART2,USART_IT_TC);
	
	USART_Cmd(USART2,ENABLE);
}



void USART1_IRQHandler()
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==1)
	{
		USART_ReceiveData(USART1);
		USART_ClearFlag(USART1,USART_IT_RXNE);
	}
}

void USART2_IRQHandler()
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE)==1)
	{
		if(usart2_cnt < 256)
			usart2_buf[usart2_cnt++] = USART_ReceiveData(USART2);
		else
			USART_ReceiveData(USART2);
		
		USART_ClearFlag(USART2,USART_IT_RXNE);
	}
}

__weak void usart2_callBack(void)
{
	
}

void usart2_receive_process(void)
{
	if(usart2_cnt == 0)
		return;
	else
	{
		if(usart2_cnt == usart2_cnt_last)
		{
			usart2_valid_data_len = usart2_cnt;
			usart2_cnt = 0;
			usart2_cnt_last = 0;
			
			usart2_callBack();
		}else
		{
			usart2_cnt_last = usart2_cnt;
		}
	}
}

void usart2_send_buf(uint8_t *buf,uint32_t len)
{
	for(int i=0;i<len;i++)
	{
		USART_SendData(USART2,buf[i]);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==0);
	}
}

int fputc(int ch,FILE *p)
{
	USART_SendData(USART1,(uint8_t)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==0);
	return ch;
}
