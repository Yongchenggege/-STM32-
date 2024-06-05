#ifndef __as7341_H
#define __as7341_H

#include "stm32f10x.h"
#include "i2c.h"

// as7341 I2C �ӻ���ַ
#define AS7341_ADDRESS		0x39

// as7341�Ĵ�����ַ
typedef enum {
	as7341_astatus			= 0x60, // 
	as7341_ch0_data_l		= 0x61, //
	as7341_ch0_data_h		= 0x62, //
	as7341_itime_l			= 0x63, //
	as7341_itime_m			= 0x64, //
	as7341_itime_h			= 0x65, //
	as7341_ch1_data_l		= 0x66, //
	as7341_ch1_data_h		= 0x67, //
	as7341_ch2_data_l		= 0x68, //
	as7341_ch2_data_h		= 0x69, //
	as7341_ch3_data_l		= 0x6a, //
	as7341_ch3_data_h		= 0x6b, //
	as7341_ch4_data_l		= 0x6c, //
	as7341_ch4_data_h		= 0x6d, //
	as7341_ch5_data_l		= 0x6e, //
	as7341_ch5_data_h		= 0x6f, //
	as7341_config			= 0x70, // ����LED�Ŀ��Ʒ�ʽ�ͻ���ģʽ
	as7341_stat				= 0x71, // �豸״̬
	as7341_edge				= 0x72, // ����SYNDģʽ�µĻ���ʱ��
	as7341_gpio				= 0x73, // ����PD��GPIO���ӷ�ʽ
	as7341_led				= 0x74, // ����LED�Ŀ���״̬��LED����������(LED�Ĳ���ǿ��)��LED�Ŀ��Ʒ�ʽ�����ó�as7341��ģʽ
	as7341_enable			= 0x80, // ����as7341���ɹ��ܵ�ʹ��
	as7341_atime			= 0x81, // ����as7341����ʱ���atime����
	as7341_wtime			= 0x83, // ����as7341ÿ��������֮��ĵȴ�����
	as7341_sp_th_l_lsb		= 0x84, // ����ADC0�ĵ��ж���ֵ д˳���ȵͺ�� M-L
	as7341_sp_th_l_msb		= 0x85, //
	as7341_sp_th_h_lsb		= 0x86, // ����ADC0�ĸ��ж���ֵ д˳���ȵͺ�� M-L
	as7341_sp_th_h_msb		= 0x87, //
	as7341_auxid			= 0x90, // ����ID
	as7341_revid			= 0x91, // �޶�ID
	as7341_id				= 0x92, // �豸ID
	as7341_status			= 0x93, // �ж�״̬
	as7341__ch0_data_l		= 0x95, //
	as7341__ch0_data_h		= 0x96, //
	as7341__ch1_data_l		= 0x97, //
	as7341__ch1_data_h		= 0x98, //
	as7341__ch2_data_l		= 0x99, //
	as7341__ch2_data_h		= 0x9a, //
	as7341__ch3_data_l		= 0x9b, //
	as7341__ch3_data_h		= 0x9c, //
	as7341__ch4_data_l		= 0x9d, //
	as7341__ch4_data_h		= 0x9e, //
	as7341__ch5_data_l		= 0x9f, //
	as7341__ch5_data_h		= 0xa0, //
	as7341_status2			= 0xa3, // ���ײ���״̬
	as7341_status3			= 0xa4, // ���ײ���״̬
	as7341_status4			= 0xa5, // SP�ж�״̬
	as7341_status5			= 0xa6, // FD���״̬
	as7341_status6			= 0xa7, // ����״̬
	as7341_cfg0				= 0xa9, // ���õ͹��ĺͷ���Ȩ�޺�WTIME�ӳ�
	as7341_cfg1				= 0xaa, // ����ADC������
	as7341_cfg3				= 0xac, // ���������Ƿ����ж�״̬����
	as7341_cfg6				= 0xaf, // 
	as7341_cfg8				= 0xb1, // ����FIFO��ֵ����˸�����Զ����ơ����������Զ�����
	as7341_cfg9				= 0xb2, // ���� SIEN_FD SIEN_SMUX
	as7341_cfg10			= 0xb3, // ����AGCģʽ��,����ֵ�Զ�����Ϊ�����̵İٷֱ�;����AGCģʽ��,����ֵ�Զ�����Ϊ�����̵İٷֱȺ���˸���Ĵ���
	as7341_cfg12			= 0xb5, // ÿ����
	as7341_pers				= 0xbd, // ���� APERS
	as7341_gpio2			= 0xbe, // ����as7341��gpio���������ģʽ
	as7341_astep_l			= 0xca, // ����as7341����ʱ���astep����
	as7341_astep_h			= 0xcb, //
	as7341_agc_gain_max		= 0xcf, // ������˸����
	as7341_az_config		= 0xd6, // �����²�����
	as7341_fd_time1			= 0xd8, // ����FD(adc5)�Ļ���ʱ�䣬��Ҫas7341_enable FDEN = 1,��FDEN = 0 ʱ��FD�Ļ���ʱ���������ADC0-ADC4һ��
	as7341_fd_time2			= 0xda, // ����FD(adc5)�Ļ���ʱ������� FD(adc5)�Ļ���ʱ����11λ�ģ���8λ��time1,����λ��time2
	as7341_fd_cfg0			= 0xd7, // δ����ԭʼ��˸�����Ƿ����FIFO
	as7341_fd_status		= 0xd8, // FD״̬
	as7341_intenab			= 0xf9, // ���������ж�ʹ��
	as7341_control			= 0xfa, // �����־λ
	as7341_fifo_map			= 0xfc, // δ�������Ƿ�д��FIFO
	as7341_fifo_lvl			= 0xfd, // FIFO��δ��������
	as7341_fdata_l			= 0xfe, // FIFO������
	as7341_fdata_h			= 0xff, //
}as7341_reg_addr;



#endif
