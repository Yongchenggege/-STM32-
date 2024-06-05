#include "crc16.h"

uint16_t crc16(uint8_t *addr,uint8_t num)
{
	int i,j,temp;
	uint16_t crc=0xFFFF;	
	for(i=0;i<num;i++)
	{
		crc=crc^(*addr);
		for(j=0;j<8;j++)
		{
			temp=crc&0x0001;
			crc=crc>>1;
			if(temp)
			{
				crc=crc^0xA001;
			}
		}
		addr++;
	}
	return crc;
}
