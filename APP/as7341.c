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
	
	as7341_write_byte(as7341_enable,0x01); // ʹ��as7341
	
	as7341_write_byte(as7341_config,0x08); // LED��as7341���� ��ʹ���ж�
	
	as7341_write_byte(as7341_led,0x0a); // ����LED��������Ϊ10*2mA,LED�ر�
	
	/* 	
	*	t(int) = (ATIME + 1) �� (ASTEP + 1) �� 2.78us ����ʱ�����
	*	adc(fullscale) = (ATIME + 1) �� (ASTEP + 1) ��ǰ����ʱ����ADC���Ե�������ֵ ע�⣺ADC�ļ���ֵ�����2^16-1�����û���ʱ����Ҫע����䣨����ʵ�ʹ�ǿ��ֵ����65535ʱ����
	*	65535��
	*
	*	SYND ģʽ ����ʱ����EDGE�Ĵ�������
	*	t(int) = ITIME �� 2.78us
	*/
	uint8_t astep[2] = {0x57,0x02}; // {L,H}
	
	as7341_write(as7341_astep_l,astep,2); // set vlaue = 599
	
	as7341_write_byte(as7341_atime,0x1d); // set vlaue = 29
	
	as7341_write_byte(as7341_wtime,0x14); // set vlaue = 20 time = (vlaue+1)*2.78ms ��������as7341_enable WEN = 1
	
	
}
