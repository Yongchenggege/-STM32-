#include "cct.h"
#include "math.h"

#if CCT_ENABLE

param_t param[5] = {0};

param_t param_default[5] __attribute__((at(PARAM_ADDRESS))) ={ // 参数放在最后一个扇区
															{0,0,0,0,0,0},
															{0,0,0,0,0,0},
															{0,0,0,0,0,0},
															{0,0,0,0,0,0},
															{0,0,0,0,0,0},
															};

uint32_t srgb2cct(COLOR_RGBC rgbc)
{
	float sr,sg,sb; // srgb
	float lr,lg,lb; // liner rgb
	float X,Y,Z; // xyz
	float x,y; // CIE 坐标
	float n; // 
	uint32_t cct; // 色温
	
	// srgb 归一化处理
	sr = rgbc.r/(float)rgbc.c;
	sg = rgbc.g/(float)rgbc.c;
	sb = rgbc.b/(float)rgbc.c;
	
	// srgb转lrgb
	if(sr > 0.04045)
		lr = pow(((sr + 0.055)/1.055),2.4);
	else
		lr = sr/12.92;
	
	if(sg > 0.04045)
		lg = pow(((sg + 0.055)/1.055),2.4);
	else
		lg = sg/12.92;
	
	if(sb > 0.04045)
		lb = pow(((sb + 0.055)/1.055),2.4);
	else
		lb = sb/12.92;
	
	// lrgb转xyz
	X = 0.4124564*lr + 0.3575761*lg + 0.1804375*lb;
	Y = 0.2126729*lr + 0.7151522*lg + 0.0721750*lb;
	Z = 0.0193339*lr + 0.1191920*lg + 0.9503041*lb;
	
	x = X/(X+Y+Z);
	y = Y/(X+Y+Z);
	
	// 计算CCT
	n=(x-0.3320)/(0.1858-y);
	cct=437*n*n*n + 3601 *n*n + 6831 *n + 5517;
	
	return cct;
}

void color_recognition(uint8_t num,COLOR_RGBC *rgbc,uint32_t *cct,uint8_t *color)
{
	*cct = srgb2cct(*rgbc);
	
	if((*cct > param[num].cct_m_min) && (*cct < param[num].cct_m_max))
		*color = COLOR_MIDDLE;
	else if((*cct > param[num].cct_w_min) && (*cct < param[num].cct_w_max))
		*color = COLOR_WHITE;
	else if((*cct > param[num].cct_y_min) && (*cct < param[num].cct_y_max))
		*color = COLOR_YELLOW;
	else
	{
		*color = COLOR_NONE;
	}
}

void param_init(void)
{
	for(int i=0;i<DEV_NUM;i++)
	{
		param[i].cct_m_max = param_default[i].cct_m_max;
		printf("param[%d].k1 = %d\n",i,param[i].cct_m_max);
		param[i].cct_m_min = param_default[i].cct_m_min;
		param[i].cct_w_max = param_default[i].cct_w_max;
	    param[i].cct_w_min = param_default[i].cct_w_min;
		param[i].cct_y_max = param_default[i].cct_y_max;
		param[i].cct_y_min = param_default[i].cct_y_max;
	}
}

void param_updata(uint32_t num,uint8_t *data)
{
	uint32_t temp;
	
	if(FLASH_ENABLE)
	{
		wirte_color_parameters(num,data);
	
		param[num].cct_m_max = param_default[num].cct_m_max;
		param[num].cct_m_min = param_default[num].cct_m_min;
		param[num].cct_w_max = param_default[num].cct_w_max;
		param[num].cct_w_min = param_default[num].cct_w_min;
		param[num].cct_y_max = param_default[num].cct_y_max;
		param[num].cct_y_min = param_default[num].cct_y_max;
	}
	else {
		temp = data[0]<<24;
		temp |= data[1]<<16;
		temp |= data[2]<<8;
		temp |= data[3];
		param[num].cct_m_max = temp;
		temp = data[4]<<24;
		temp |= data[5]<<16;
		temp |= data[6]<<8;
		temp |= data[7];
		param[num].cct_m_min = temp;
		temp = data[8]<<24;
		temp |= data[9]<<16;
		temp |= data[10]<<8;
		temp |= data[11];
		param[num].cct_w_max = temp;
		temp = data[12]<<24;
		temp |= data[13]<<16;
		temp |= data[14]<<8;
		temp |= data[15];
		param[num].cct_w_min = temp;
		temp = data[16]<<24;
		temp |= data[17]<<16;
		temp |= data[18]<<8;
		temp |= data[19];
		param[num].cct_y_max = temp;
		temp = data[20]<<24;
		temp |= data[21]<<16;
		temp |= data[22]<<8;
		temp |= data[23];
		param[num].cct_y_min = temp;
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

void upload_sampling_data(uint32_t *cct,uint32_t len)
{
	uint8_t buf[256];
	uint16_t send_len = 0;
	
	buf[0] = MODBUS_RTU_ADDR; // 地址码
	
	buf[1] = 0x04; // 功能码
	
	buf[2] = len*4; // 有效长度
	
	send_len += 3;
	
	for(int i = 0;i<len;i++)
	{
		buf[3+i*4] = cct[i]>>24;
		buf[3+i*4+1] = cct[i]>>16;
		buf[3+i*4+2] = cct[i]>>8;
		buf[3+i*4+3] = cct[i];
		send_len += 4;
	}
	
	// 计算CRC16
	uint16_t crc = 0;
	
	crc = crc16(buf,send_len);
	
	buf[3+len*4] = crc; // 校验位低8位
	buf[3+len*4+1] = crc>>8; // 校验位高8位
	
	send_len += 2;
	
	usart2_send_buf(buf,send_len);
}

void wirte_color_parameters(uint16_t index,uint8_t *data)
{
	uint32_t temp = 0;
	uint32_t buf[256];
	uint32_t addr = PARAM_ADDRESS;
	int start = index*6;
	int end = start + 6;
	
	for(int i=0;i<256;i++)
	{
		buf[i] = *((uint32_t *)addr);
		addr += 4;
	}
	
	FLASH_Unlock();
	
	FLASH_ErasePage(PARAM_ADDRESS);//擦除这个扇区
	
	// 
	temp = data[0]<<24;
	temp |= data[1]<<16;
	temp |= data[2]<<8;
	temp |= data[3];
	
	FLASH_ProgramWord((uint32_t)&param_default[index].cct_m_max,temp);
	
	// 
	temp = data[4]<<24;
	temp |= data[5]<<16;
	temp |= data[6]<<8;
	temp |= data[7];
	
	FLASH_ProgramWord((uint32_t)&param_default[index].cct_m_min,temp);
	
	// 
	temp = data[8]<<24;
	temp |= data[9]<<16;
	temp |= data[10]<<8;
	temp |= data[11];
	
	FLASH_ProgramWord((uint32_t)&param_default[index].cct_w_max,temp);
	
	//
	temp = data[12]<<24;
	temp |= data[13]<<16;
	temp |= data[14]<<8;
	temp |= data[15];
	
	FLASH_ProgramWord((uint32_t)&param_default[index].cct_w_min,temp);
	
	// 
	temp = data[16]<<24;
	temp |= data[17]<<16;
	temp |= data[18]<<8;
	temp |= data[19];
	
	FLASH_ProgramWord((uint32_t)&param_default[index].cct_y_max,temp);
	
	// 
	temp = data[20]<<24;
	temp |= data[21]<<16;
	temp |= data[22]<<8;
	temp |= data[23];
	
	FLASH_ProgramWord((uint32_t)&param_default[index].cct_y_min,temp);
	
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

#endif
