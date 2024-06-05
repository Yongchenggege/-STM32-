#include "RTU.h"

void RTU_unpack(uint8_t *buf,RTU_t *RTU_pack)
{
	uint32_t pack_len = 0; // ������ĳ���
	uint16_t reg = 0; // �Ĵ�����ַ
	uint16_t reg_value = 0; // д��Ĵ�������
	uint16_t nregs = 0; // �����Ĵ�������
	uint16_t crc_m = 0; // ����CRCУ����
	uint16_t crc_s = 0; // �ӻ�CRCУ����
	uint16_t index;
	
	// �����˳�����ַ����
	RTU_pack->addr = buf[0];
	if(buf[0] != MODBUS_RTU_ADDR)
	{
		RTU_pack->valid = 0;
		usart2_buf_clean();
		return;
	}
	
	RTU_pack->code = buf[1];
	
	// ��չ��case
	switch(buf[1])
	{
		case 0x03:
			pack_len = 6+2; // 6�ֽ�֡ͷ 2�ֽ�У����
			break;
		case 0x04:
			pack_len = 6+2; // 6�ֽ�֡ͷ 2�ֽ�У����
			break;
		case 0x10:
#if 0
			pack_len = 6+12+2; // 6�ֽ�֡ͷ 12�ֽ����� 2�ֽ�У���� rgb
#else 
			pack_len = 6+24+2; // 6�ֽ�֡ͷ 24�ֽ����� 2�ֽ�У���� cct
#endif
			break;
		default:
			break;
	}
	
	// �����˳������ճ��ȴ���
	RTU_pack->pack_len = usart2_valid_data_len;
	if(RTU_pack->pack_len != pack_len)
	{
		RTU_pack->valid = 0;
		usart2_buf_clean();
		return;
	}
	
	crc_m = buf[pack_len-2];
	crc_m |= buf[pack_len-1]<<8;
	
	crc_s = crc16(buf,(pack_len-2));
	
	RTU_pack->crc_m = crc_m;
	RTU_pack->crc_s = crc_s;
	// �����˳���CRCУ�����
	if(crc_s != crc_m)
	{
		printf("CRCУ�����\n");
		RTU_pack->valid = 0;
		usart2_buf_clean();
		return;
	}
	
	// ��ȡ�����ļĴ�����ַ��ʼ��ַ
	reg = buf[2]<<8;
	reg |= buf[3];
	
	// ��չ��case
	switch(buf[1])
	{
		case 0x03:
			// ��ȡ��Ҫ�����ļĴ�������
			nregs = buf[4]<<8;
			nregs |= buf[5];
			
			switch(reg)
			{
				case 0x00:
					// master	01 03 00 00 00 05 85 C9
					// salve	01 03 len color1_H color_L ... color5_H color5_L CRC_L CRC_H
				case 0x01:
				case 0x02:
				case 0x03:
				case 0x04:
					upload_color_sensor_data(reg,nregs);
					break;
				default:
					break;
			}
			
			usart2_buf_clean(); // ���������
			break;
		case 0x04:
			// д��Ĵ���������
			reg_value = buf[4]<<8;
			reg_value |= buf[5];
			
			switch(reg)
			{
				// ��ȡһ�β�������
				// master	01 04 00 05 00 01 21 CB
				// salve	01 04 len r_H r_L g_H g_L b_H b_L ... CRC_L CRC_H
				case 0x05:
					if(reg_value)
						UPLOAD_SAMPLING_DATA();
					break;
				default:
					break;
			}
			
			usart2_buf_clean(); // ���������
			break;
		case 0x10:
			//01 10 00 07 00 01 00 00 16 A8 00 00 14 B4 00 00 1B BC 00 00 18 9C 00 00 10 04 00 00 0D AC DA 83
			switch(reg)
			{
				case 0x06:index = 0;goto func10_1;
				case 0x07:index = 1;goto func10_1;
				case 0x08:index = 2;goto func10_1;
				case 0x09:index = 3;goto func10_1;
				case 0x0A:index = 4;goto func10_1;
				default:
					goto func_10_exit;
			}
// �Ĵ���06-0aִ����
func10_1:
			// ��ȡ��Ҫ�����ļĴ�������
			nregs = buf[4]<<8;
			nregs |= buf[5];
			
			PARAM_UPDATA(index,&buf[6]);	
			
			crc_s = crc16(buf,6);
			
			buf[6] = crc_s;
			buf[7] = crc_s>>8;
			
			usart2_send_buf(buf,8);
			

			
func_10_exit:
			usart2_buf_clean(); // ���������
			break;
		default:
			break;
	}
}
