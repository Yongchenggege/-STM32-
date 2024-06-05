#include "rgb_ratio.h"

#if RGB_RATIO_ENABLE

param_t param[5] = {0};

param_t param_default[5] __attribute__((at(PARAM_ADDRESS))) ={ // 参数放在最后一个扇区
															{0.101,0.150,0.029, 	// k1,k2,k3
															 1.066, 1.426, 1.337, 	// r_g,r_b,g_b middle_light
															 0.861,1.060,1.232, 	// r_g,r_b,g_b white_light
															1.336,2.044,1.530},		// r_g,r_b,g_b yellow_light

															{0.195,0.326,0.048, 	// k1,k2,k3
															 1.219, 1.685, 1.384, 	// r_g,r_b,g_b middle_light
															 0.990,1.261,1.275, 	// r_g,r_b,g_b white_light
															1.501,2.351,1.565},		// r_g,r_b,g_b yellow_light
															
															{0.125,0.981,0.567, 	// k1,k2,k3
															 1.202, 1.679, 1.398, 	// r_g,r_b,g_b middle_light
															 0.970,1.247,1.285, 	// r_g,r_b,g_b white_light
															1.500,2.018,1.322},		// r_g,r_b,g_b yellow_light
															
															{0.152,0.245,0.036, 	// k1,k2,k3
															 1.074, 1.465, 1.365, 	// r_g,r_b,g_b middle_light
															 0.877,1.117,1.275, 	// r_g,r_b,g_b white_light
															1.323,2.031,1.535},		// r_g,r_b,g_b yellow_light
															
															{0.108,1.038,0.708, 	// k1,k2,k3
															 1.074, 1.471, 1.370, 	// r_g,r_b,g_b middle_light
															 0.872,1.106,1.268, 	// r_g,r_b,g_b white_light
															1.338,1.643,1.199},		// r_g,r_b,g_b yellow_light
															};

/* 测试数据 min(小光强) 中间色 r:g:b = 6:5:4   r:g = 6/5 = 1.2   r:b = 6/4 = 1.5   g:b = 5/4 = 1.25
												黄色	 r:g:b = 11:7:5  r:g = 11/7 = 1.57 r:b = 11/5 = 2.2  g:b = 7/5 = 1.4
												白色	 r:g:b = 8:8:7	 r:g = 8/8 = 1     r:b = 8/7 = 1.14  g:b = 8/7 = 1.14
						max(大光强) 中间色 r:g:b = 5:4:3	 r:g = 5/4 = 1.25  r:b = 5/3 = 1.67  g:b = 4/3 = 1.33
												黄色	 r:g:b = 16:10:7 r:g = 16/10 = 1.6 r:b = 16/7 = 2.29 g:b = 10/7 = 1.43
												白色   r:g:b = 6:6:5   r:g = 6/6 = 1     r:b = 6/5 = 1.2   g:b = 6/5 = 1.2
中间色|min - max| r:g = 0.05 r:b = 0.17 g:b = 0.08
黄色|min - max|   r:g = 0.03 r:b = 0.09 g:b = 0.03
白色|min - max|		r:g = 0		 r:b = 0.06 g:b = 0.06
k1=max(中间色,黄色,白色) // 允许误差取三者最大 r:g
k2=max(中间色,黄色,白色) // 允许误差取三者最大 r:b
k3=max(中间色,黄色,白色) // 允许误差取三者最大 g:b
阈值: (max+min)/2;
中间色 (1.2+1.25)/2 = 1.225 (1.5+1.67)/2 = 1.585 (1.25+1.33)/2 = 1.29 范围+-误差+-k1 +-k2 +-k3
黄色	 (1.57+1.6)/2 = 1.585 (2.2+2.29)/2 = 2.245 (1.4+1.43)/2 = 1.415 范围+-误差+-k1 +-k2 +-k3
白色   (1+1)/2 = 1					(1.14+1.2)/2 = 1.17	 (1.14+1.2)/2 = 1.17  范围+-误差+-k1 +-k2 +-k3
*/


void color_recognition(uint8_t num,COLOR_RGBC *rgbc,uint8_t *color)
{
	float k1 = param[num].k1; // r:g
	float k2 = param[num].k2; // r:b
	float k3 = param[num].k3; // g:b
	
	float r_g = rgbc->r/(rgbc->g*1.0); // r和g的比值
	float r_b = rgbc->r/(rgbc->b*1.0); // r和b的比值
	float g_b = rgbc->g/(rgbc->b*1.0); // g和b的比值

	if((r_g >= (param[num].r_g_m-k1) && r_g <= (param[num].r_g_m+k1)) && (r_b >= (param[num].r_b_m-k2) && r_b <= (param[num].r_b_m+k2)) && (g_b >= (param[num].g_b_m-k3) && g_b <= (param[num].g_b_m+k3))) 
	{
		*color = COLOR_MIDDLE;// middle_light
	} 
	else if((r_g >= (param[num].r_g_w-k1) && r_g <= (param[num].r_g_w+k1)) && (r_b >= (param[num].r_b_w-k2) && r_b <= (param[num].r_b_w+k2)) && (g_b >= (param[num].g_b_w-k3) && g_b <= (param[num].g_b_w+k3))) 
	{ 
		*color = COLOR_WHITE;// white_light	
	}		
	else if((r_g >= (param[num].r_g_y-k1) && r_g <= (param[num].r_g_y+k1)) && (r_b >= (param[num].r_b_y-k2) && r_b <= (param[num].r_b_y+k2)) && (g_b >= (param[num].g_b_y-k3) && g_b <= (param[num].g_b_y+k3))) 
	{ 
		*color = COLOR_YELLOW;// yellow_light
	}
	else {
		*color = COLOR_NONE;
	}
}

/*	指令
*	checksum = len_H + ... + checksum前一字节 结果取低8位
*	len = cmd_code + ... + checksum前一字节
*	主机 7B len_H len_L 00(cmd_code) regID_H regID_L checksum 7D 读取传感器x
*	从机 7B len_H len_L 00(cmd_code) regID_H regID_L result checksum 7D
*
*	主机 7B len_H len_L 01(cmd_code) checksum 7D 读取所有传感器
* 	从机 7B len_H len_L 01(cmd_code) result_0 ... result_n checksum 7D
*
*/

/*	功能 连续读取n个连续地址的寄存器
*	参数 base_addr	起始地址
*	参数 n			几个寄存器
*/
void upload_color_sensor_data(uint16_t base_addr,uint16_t n)
{
	uint8_t buf[256];
	uint16_t send_len = 0;
	
	if((base_addr + n - 1) > 0x04) // 读取的寄存器存在无效地址
		return;
	
	buf[0] = MODBUS_RTU_ADDR; // 地址码
	
	buf[1] = 0x03; // 功能码
	
	buf[2] = n*2; // 有效长度
	
	send_len += 3;
	
	int start = base_addr;
	
	for(int i = 0;i<n;i++)
	{
		buf[3+i*2] = 0;
		buf[3+i*2+1] = color_result[start++];
		send_len += 2;
	}
	
	// 计算CRC16
	uint16_t crc = 0;
	
	crc = crc16(buf,send_len);
	
	buf[3+n*2] = crc; // 校验位低8位
	buf[3+n*2+1] = crc>>8; // 校验位高8位
	
	send_len += 2;
	
	usart2_send_buf(buf,send_len);
}

void upload_sampling_data(COLOR_RGBC *rgbc,uint32_t len)
{
	uint8_t buf[256];
	uint16_t send_len = 0;
	
	buf[0] = MODBUS_RTU_ADDR; // 地址码
	
	buf[1] = 0x04; // 功能码
	
	buf[2] = len*6; // 有效长度
	
	send_len += 3;
	
	for(int i = 0;i<len;i++)
	{
		buf[3+i*6] = rgbc[i].r>>8;
		buf[3+i*6+1] = rgbc[i].r;
		buf[3+i*6+2] = rgbc[i].g>>8;
		buf[3+i*6+3] = rgbc[i].g;
		buf[3+i*6+5] = rgbc[i].b>>8;
		buf[3+i*6+6] = rgbc[i].b;
		send_len += 6;
	}
	
	// 计算CRC16
	uint16_t crc = 0;
	
	crc = crc16(buf,send_len);
	
	buf[3+len*6] = crc; // 校验位低8位
	buf[3+len*6+1] = crc>>8; // 校验位高8位
	
	send_len += 2;
	
	usart2_send_buf(buf,send_len);
}

void wirte_color_parameters(uint16_t index,uint8_t *data)
{
	uint32_t temp = 0;
	float ftemp = 0;
	uint32_t *ptemp = (uint32_t *)&ftemp;
	uint32_t buf[256];
	uint32_t addr = PARAM_ADDRESS;
	int start = index*12;
	int end = start + 12;
	
	for(int i=0;i<256;i++)
	{
		buf[i] = *((uint32_t *)addr);
		addr += 4;
	}
	
	FLASH_Unlock();
	
	FLASH_ErasePage(PARAM_ADDRESS);//擦除这个扇区
	
	// k1
	temp = data[0];
	ftemp = temp/100.0;
	
	FLASH_ProgramWord((uint32_t)&param_default[index].k1,*ptemp);
	
	// k2
	temp = data[1];
	ftemp = temp/100.0;
	
	FLASH_ProgramWord((uint32_t)&param_default[index].k2,*ptemp);
	
	// k3
	temp = data[2];
	ftemp = temp/100.0;
	
	FLASH_ProgramWord((uint32_t)&param_default[index].k3,*ptemp);
	
	// r_g_m
	temp = data[3];
	ftemp = temp/100.0;
	
	FLASH_ProgramWord((uint32_t)&param_default[index].r_g_m,*ptemp);
	
	// r_b_m
	temp = data[4];
	ftemp = temp/100.0;
	
	FLASH_ProgramWord((uint32_t)&param_default[index].r_b_m,*ptemp);
	
	// g_b_m
	temp = data[5];
	ftemp = temp/100.0;
	
	FLASH_ProgramWord((uint32_t)&param_default[index].g_b_m,*ptemp);
	
	// r_g_w
	temp = data[6];
	ftemp = temp/100.0;
	
	FLASH_ProgramWord((uint32_t)&param_default[index].r_g_w,*ptemp);
	
	// r_b_w
	temp = data[7];
	ftemp = temp/100.0;
	
	FLASH_ProgramWord((uint32_t)&param_default[index].r_b_w,*ptemp);
	
	// g_b_w
	temp = data[8];
	ftemp = temp/100.0;
	
	FLASH_ProgramWord((uint32_t)&param_default[index].g_b_w,*ptemp);
	
	// r_g_y
	temp = data[9];
	ftemp = temp/100.0;
	
	FLASH_ProgramWord((uint32_t)&param_default[index].r_g_y,*ptemp);
	
	// r_b_y
	temp = data[10];
	ftemp = temp/100.0;
	
	FLASH_ProgramWord((uint32_t)&param_default[index].r_b_y,*ptemp);
	
	// g_b_y
	temp = data[11];
	ftemp = temp/100.0;
	
	FLASH_ProgramWord((uint32_t)&param_default[index].g_b_y,*ptemp);
	
	addr = PARAM_ADDRESS;
	
	for(int i=0;i<start;i++)
	{
		FLASH_ProgramWord(addr,buf[i]);
		addr += 4;
	}
	
	addr = PARAM_ADDRESS + end*4;
	
	for(int i=end;i<256;i++)
	{
		FLASH_ProgramWord(addr,buf[i]);
		addr += 4;
	}
	
	FLASH_Lock();//上锁
}

void param_init(void)
{
	for(int i=0;i<DEV_NUM;i++)
	{
		param[i].k1 = param_default[i].k1;
		printf("param[%d].k1 = %f\n",i,param[i].k1);
		param[i].k2 = param_default[i].k2;
		param[i].k3 = param_default[i].k3;
		param[i].r_g_m = param_default[i].r_g_m;
		param[i].r_b_m = param_default[i].r_b_m;
		param[i].g_b_m = param_default[i].g_b_m;
		param[i].r_g_w = param_default[i].r_g_w;
		param[i].r_b_w = param_default[i].r_b_w;
		param[i].g_b_w = param_default[i].g_b_w;
		param[i].r_g_y = param_default[i].r_g_y;
		param[i].r_b_y = param_default[i].r_b_y;
	    param[i].g_b_y = param_default[i].g_b_y;
	}
}

void param_updata(uint32_t num,uint8_t *data)
{
	uint32_t temp;
	float ftemp;

	if(1)
	{
		wirte_color_parameters(num,data);
		
		param[num].k1 = param_default[num].k1;
		param[num].k2 = param_default[num].k2;
		param[num].k3 = param_default[num].k3;
		param[num].r_g_m = param_default[num].r_g_m;
		param[num].r_b_m = param_default[num].r_b_m;
		param[num].g_b_m = param_default[num].g_b_m;
		param[num].r_g_w = param_default[num].r_g_w;
		param[num].r_b_w = param_default[num].r_b_w;
		param[num].g_b_w = param_default[num].g_b_w;
		param[num].r_g_y = param_default[num].r_g_y;
		param[num].r_b_y = param_default[num].r_b_y;
		param[num].g_b_y = param_default[num].g_b_y;
	}else {
		
		temp = data[0];
		ftemp = temp/100.0;
		param[num].k1 = ftemp;
		temp = data[1];
		ftemp = temp/100.0;
		param[num].k2 = ftemp;
		temp = data[2];
		ftemp = temp/100.0;
		param[num].k3 = ftemp;
		temp = data[3];
		ftemp = temp/100.0;
		param[num].r_g_m = ftemp;
		temp = data[4];
		ftemp = temp/100.0;
		param[num].r_b_m = ftemp;
		temp = data[5];
		ftemp = temp/100.0;
		param[num].g_b_m = ftemp;
		temp = data[6];
		ftemp = temp/100.0;
		param[num].r_g_w = ftemp;
		temp = data[7];
		ftemp = temp/100.0;
		param[num].r_b_w = ftemp;
		temp = data[8];
		ftemp = temp/100.0;
		param[num].g_b_w = ftemp;
		temp = data[9];
		ftemp = temp/100.0;
		param[num].r_g_y = ftemp;
		temp = data[10];
		ftemp = temp/100.0;
		param[num].r_b_y = ftemp;
		temp = data[11];
		ftemp = temp/100.0;
		param[num].g_b_y = ftemp;
	}
}

#endif
