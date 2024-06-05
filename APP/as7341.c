#include "as7341.h"

void as7341_write(uint8_t addr,uint8_t *data,uint32_t len)
{	
	uint8_t slave_addr = (AS7341_ADDRESS<<1)|0x01;
	
	iic_start();
	
	iic_send_byte(slave_addr);
	
	iic_wait_ack();
	
	iic_send_byte(addr);
	
	iic_wait_ack();
	
	for(uint32_t i=0;i<len;i++)
	{
		iic_send_byte(data[i]);
	
		iic_wait_ack();
	}
	
	iic_stop();
}

void as7341_write_byte(uint8_t addr,uint8_t data)
{
	as7341_write(addr,&data,1);
}

void as7341_read(uint8_t addr,uint8_t *data,uint32_t len)
{
	uint8_t data;
	uint8_t slave_addr = AS7341_ADDRESS<<1;
	
	iic_start();
	
	iic_send_byte(slave_addr);
	
	iic_wait_ack();
	
	iic_send_byte(AS7341_ADDRESS);
	
	iic_wait_ack();
	
	iic_start();
	
	iic_send_byte(slave_addr|0x01);
	
	iic_wait_ack();
	
	for(uint32_t i=0;i<len;i++)
	{
		data[i] = iic_read_byte(0);
	}
	
	iic_stop();
}

uint8_t as7341_read_byte(uint8_t addr)
{
	uint8_t data;
	
	as7341_read(addr,&data,1);
	
	return data;
}

void as7341_init(void)
{
	iic_init();
	
	as7341_write_byte(as7341_enable,0x01); // 使能as7341
	
	as7341_write_byte(as7341_config,0x08); // LED受as7341控制 不使用中断
	
	as7341_write_byte(as7341_led,0x0a); // 配置LED驱动电流为10*2mA,LED关闭
	
	/* 	
	*	t(int) = (ATIME + 1) × (ASTEP + 1) × 2.78us 积分时间计算
	*	adc(fullscale) = (ATIME + 1) × (ASTEP + 1) 当前积分时间下ADC可以到达的最大值 注意：ADC的计数值最大是2^16-1，配置积分时间需要注意搭配（即当实际光强的值大于65535时都是
	*	65535）
	*
	*	SYND 模式 积分时间由EDGE寄存器决定
	*	t(int) = ITIME × 2.78us
	*/
	uint8_t astep[2] = {0x57,0x02}; // {L,H}
	
	as7341_write(as7341_astep_l,astep,2); // set vlaue = 599
	
	as7341_write_byte(as7341_atime,0x1d); // set vlaue = 29
	
	as7341_write_byte(as7341_wtime,0x14); // set vlaue = 20 time = (vlaue+1)*2.78ms 需求配置as7341_enable WEN = 1
	
	
}
