#include "tcs34725.h"
#include "stdio.h"

void TCS34725_Write(uint8_t addr,uint8_t data)
{
		addr = addr|TCS34725_COMMAND_BIT;
	
		iic_start();
		iic_send_byte(0x52);
		iic_wait_ack();
		iic_send_byte(addr);
		iic_wait_ack();
		iic_send_byte(data);
		iic_wait_ack();
		iic_stop();
}

uint8_t TCS34725_Read(uint8_t addr)
{
		uint8_t data;
		addr = addr|TCS34725_COMMAND_BIT;
		
		iic_start();
		iic_send_byte(0x52);
		iic_wait_ack();
		iic_send_byte(addr);
		iic_wait_ack();
		iic_start();
		iic_send_byte(0x53);
		iic_wait_ack();
		data=iic_read_byte(0);
		iic_stop();
		return data;
}

/*******************************************************************************
 * @brief TCS34725设置积分时间
 *
 * @return None
*******************************************************************************/
void TCS34725_SetIntegrationTime(uint8_t time)
{
	TCS34725_Write(TCS34725_ATIME,time);
}

/*******************************************************************************
 * @brief TCS34725设置增益
 *
 * @return None
*******************************************************************************/
void TCS34725_SetGain(uint8_t gain)
{
	TCS34725_Write(TCS34725_CONTROL,gain);
}

/*******************************************************************************
 * @brief TCS34725使能
 *
 * @return None
*******************************************************************************/
void TCS34725_Enable(void)
{
	uint8_t cmd = TCS34725_ENABLE_PON;
	
	TCS34725_Write(TCS34725_ENABLE,cmd);
	cmd = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
	TCS34725_Write(TCS34725_ENABLE,cmd);
}

/*******************************************************************************
 * @brief TCS34725失能
 *
 * @return None
*******************************************************************************/
void TCS34725_Disable(void)
{
	uint8_t cmd = 0;
	
	cmd = TCS34725_Read(TCS34725_ENABLE);
	cmd = cmd & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
	TCS34725_Write(TCS34725_ENABLE, cmd);
}

/*******************************************************************************
 * @brief TCS34725初始化
 *
 * @return ID - ID寄存器中的值
*******************************************************************************/
uint8_t TCS34725_Init(void)
{
	uint8_t id=0,i;
	
	iic_init();

	for(i=0;i<DEV_NUM;i++)
	{
		select_i2c(i);
		
		id = TCS34725_Read(TCS34725_ID);  //TCS34725 的 ID 是 0x44 可以根据这个来判断是否成功连接,0x4D是TCS34727;

		printf("id[%d]=0x%x\n",i,id);
		
		if(id==0x4D | id==0x44)
		{
			TCS34725_SetIntegrationTime(TCS34725_INTEGRATIONTIME_24MS);
			TCS34725_SetGain(TCS34725_GAIN_1X);
			TCS34725_Enable();
		}
	}
	
	return 0;
}

/*******************************************************************************
 * @brief TCS34725获取单个通道数据
 *
 * @return data - 该通道的转换值
*******************************************************************************/
uint16_t TCS34725_GetChannelData(uint8_t reg)
{
	uint8_t tmp[2] = {0,0};
	uint16_t data;
	
	tmp[0] = TCS34725_Read(reg);
	tmp[1] = TCS34725_Read(reg+1);
	data = (tmp[1] << 8) | tmp[0];
	
	return data;
}

/*******************************************************************************
 * @brief TCS34725获取各个通道数据
 *
 * @return 1 - 转换完成，数据可用
 *   	   0 - 转换未完成，数据不可用
*******************************************************************************/
uint8_t TCS34725_GetRawData(COLOR_RGBC *rgbc,uint8_t dev_index)
{
	uint8_t status = TCS34725_STATUS_AVALID;
	
	status = TCS34725_Read(TCS34725_STATUS);
	
	if(status & TCS34725_STATUS_AVALID)
	{
		// 选择i2c
		select_i2c(dev_index);
		
		rgbc->c = TCS34725_GetChannelData(TCS34725_CDATAL);	
		rgbc->r = TCS34725_GetChannelData(TCS34725_RDATAL);	
		rgbc->g = TCS34725_GetChannelData(TCS34725_GDATAL);	
		rgbc->b = TCS34725_GetChannelData(TCS34725_BDATAL);
		return 1;
	}
	return 0;
}

//RGB转HSL
void RGBtoHSL(COLOR_RGBC *Rgb, COLOR_HSL *Hsl)
{
	u8 maxVal,minVal,difVal;
	u8 r = Rgb->r*100/Rgb->c;   //[0-100]
	u8 g = Rgb->g*100/Rgb->c;
	u8 b = Rgb->b*100/Rgb->c;
	
	maxVal = max3v(r,g,b);
	minVal = min3v(r,g,b);
	difVal = maxVal-minVal;
	
	//计算亮度
	Hsl->l = (maxVal+minVal)/2;   //[0-100]
	
	if(maxVal == minVal)//若r=g=b,灰度
	{
		Hsl->h = 0; 
		Hsl->s = 0;
	}
	else
	{
		//计算色调
		if(maxVal==r)
		{
			if(g>=b)
				Hsl->h = 60*(g-b)/difVal;
			else
				Hsl->h = 60*(g-b)/difVal+360;
		}
		else
			{
				if(maxVal==g)Hsl->h = 60*(b-r)/difVal+120;
				else
					if(maxVal==b)Hsl->h = 60*(r-g)/difVal+240;
			}
		
		//计算饱和度
		if(Hsl->l<=50)Hsl->s=difVal*100/(maxVal+minVal);  //[0-100]
		else
			Hsl->s=difVal*100/(200-(maxVal+minVal));
	}
}
