#ifndef __as7341_H
#define __as7341_H

#include "stm32f10x.h"
#include "i2c.h"

// as7341 I2C 从机地址
#define AS7341_ADDRESS		0x39

// as7341寄存器地址
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
	as7341_config			= 0x70, // 配置LED的控制方式和积分模式
	as7341_stat				= 0x71, // 设备状态
	as7341_edge				= 0x72, // 配置SYND模式下的积分时间
	as7341_gpio				= 0x73, // 配置PD的GPIO连接方式
	as7341_led				= 0x74, // 配置LED的开关状态和LED的驱动电流(LED的补光强度)，LED的控制方式得配置成as7341的模式
	as7341_enable			= 0x80, // 配置as7341若干功能的使能
	as7341_atime			= 0x81, // 配置as7341积分时间的atime参数
	as7341_wtime			= 0x83, // 配置as7341每两个测量之间的等待周期
	as7341_sp_th_l_lsb		= 0x84, // 配置ADC0的低中断阈值 写顺序先低后高 M-L
	as7341_sp_th_l_msb		= 0x85, //
	as7341_sp_th_h_lsb		= 0x86, // 配置ADC0的高中断阈值 写顺序先低后高 M-L
	as7341_sp_th_h_msb		= 0x87, //
	as7341_auxid			= 0x90, // 辅助ID
	as7341_revid			= 0x91, // 修订ID
	as7341_id				= 0x92, // 设备ID
	as7341_status			= 0x93, // 中断状态
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
	as7341_status2			= 0xa3, // 光谱测量状态
	as7341_status3			= 0xa4, // 光谱测量状态
	as7341_status4			= 0xa5, // SP中断状态
	as7341_status5			= 0xa6, // FD相关状态
	as7341_status6			= 0xa7, // 功能状态
	as7341_cfg0				= 0xa9, // 配置低功耗和访问权限和WTIME延迟
	as7341_cfg1				= 0xaa, // 配置ADC的增益
	as7341_cfg3				= 0xac, // 配置振荡器是否受中断状态控制
	as7341_cfg6				= 0xaf, // 
	as7341_cfg8				= 0xb1, // 配置FIFO阈值和闪烁增益自动控制、光谱增益自动控制
	as7341_cfg9				= 0xb2, // 配置 SIEN_FD SIEN_SMUX
	as7341_cfg10			= 0xb3, // 配置AGC模式下,高阈值自动计算为满量程的百分比;配置AGC模式下,低阈值自动计算为满量程的百分比和闪烁检测的次数
	as7341_cfg12			= 0xb5, // 每看懂
	as7341_pers				= 0xbd, // 配置 APERS
	as7341_gpio2			= 0xbe, // 配置as7341上gpio的输入输出模式
	as7341_astep_l			= 0xca, // 配置as7341积分时间的astep参数
	as7341_astep_h			= 0xcb, //
	as7341_agc_gain_max		= 0xcf, // 配置闪烁增益
	as7341_az_config		= 0xd6, // 配置温补周期
	as7341_fd_time1			= 0xd8, // 配置FD(adc5)的积分时间，需要as7341_enable FDEN = 1,当FDEN = 0 时，FD的积分时间和增益于ADC0-ADC4一致
	as7341_fd_time2			= 0xda, // 配置FD(adc5)的积分时间和增益 FD(adc5)的积分时间是11位的，低8位在time1,高三位在time2
	as7341_fd_cfg0			= 0xd7, // 未读的原始闪烁数据是否进入FIFO
	as7341_fd_status		= 0xd8, // FD状态
	as7341_intenab			= 0xf9, // 配置若干中断使能
	as7341_control			= 0xfa, // 清除标志位
	as7341_fifo_map			= 0xfc, // 未读数据是否写入FIFO
	as7341_fifo_lvl			= 0xfd, // FIFO中未读的数据
	as7341_fdata_l			= 0xfe, // FIFO缓冲区
	as7341_fdata_h			= 0xff, //
}as7341_reg_addr;



#endif
