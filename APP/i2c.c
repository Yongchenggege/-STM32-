#include "i2c.h"

// 粗延时函数，微秒
static void delay_us(uint16_t time)
{    
   uint16_t i=0;  
   while(time--)
   {
      i=10;  //自己定义
      while(i--) ;    
   }
}

static uint8_t IIC_SDA_IN(void);
static void IIC_SDA_OUT_High(void);
static void IIC_SDA_OUT_Low(void);
static void IIC_SCL_Low(void);
static void IIC_SCL_High(void);
static void i2c_config(void);

static uint8_t i2c_select = 0;
static i2c Atcs34725[DEV_NUM];

//初始化
void iic_init(void)
{
	i2c_config();
	
	// 使能时钟
	for(int i=0;i<DEV_NUM;i++)
	{
		// SCL
		if(IS_RCC_APB2_PERIPH(Atcs34725[i].RCC_SCL))
			RCC_APB2PeriphClockCmd(Atcs34725[i].RCC_SCL,ENABLE);
		else
			RCC_APB1PeriphClockCmd(Atcs34725[i].RCC_SCL,ENABLE);
		
		// SDA
		if(IS_RCC_APB2_PERIPH(Atcs34725[i].RCC_SDA))
			RCC_APB2PeriphClockCmd(Atcs34725[i].RCC_SDA,ENABLE);
		else
			RCC_APB1PeriphClockCmd(Atcs34725[i].RCC_SDA,ENABLE);
	}
	
	// 初始化GPIO
	for(int i=0;i<DEV_NUM;i++)
	{
		// SCL
		GPIO_Init(Atcs34725[i].GPIOx_SCL,&Atcs34725[i].SCL);
		
		// SDA
		GPIO_Init(Atcs34725[i].GPIOx_SDA,&Atcs34725[i].SDA);
	}
	
	// 初始化IIC总线电平
	for(int i=0;i<DEV_NUM;i++)
	{
		select_i2c(i);
		IIC_SCL_High();
		IIC_SDA_OUT_High();		
	}
}
//SDA输出
void SDA_OUT(void)
{
	Atcs34725[i2c_select].SDA.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(Atcs34725[i2c_select].GPIOx_SDA,&Atcs34725[i2c_select].SDA);
}
//SDA输入
void SDA_IN(void)
{
	Atcs34725[i2c_select].SDA.GPIO_Mode = GPIO_Mode_IPU;
	
	GPIO_Init(Atcs34725[i2c_select].GPIOx_SDA,&Atcs34725[i2c_select].SDA);
}
//起始信号
void iic_start(void)
{
	SDA_OUT();
	IIC_SDA_OUT_High();
	IIC_SCL_High();
	delay_us(5);
	IIC_SDA_OUT_Low();
	delay_us(6);
	IIC_SCL_Low();
}
//停止信号
void iic_stop(void)
{
	SDA_OUT();
	IIC_SCL_Low();
	IIC_SDA_OUT_Low();
	IIC_SCL_High();
	delay_us(6);
	IIC_SDA_OUT_High();
	delay_us(6);
}
//等待应答
uint8_t iic_wait_ack(void)
{
	uint8_t temptime=0;
	SDA_IN();
	IIC_SDA_OUT_High();
	delay_us(1);
	IIC_SCL_High();
	delay_us(1);
	while(IIC_SDA_IN())
	{
		temptime++;
		if(temptime>250)
		{
			iic_stop();
			return 1;
		}
	}
	IIC_SCL_Low();
	return 0;
}
//应答
void iic_ack(void)
{
	IIC_SCL_Low();
	SDA_OUT();
	IIC_SDA_OUT_Low();
	delay_us(2);
	IIC_SCL_High();
	delay_us(5);
	IIC_SCL_Low();
}
//非应答
void iic_nack(void)
{
	IIC_SCL_Low();
	SDA_OUT();
	IIC_SDA_OUT_High();
	delay_us(2);
	IIC_SCL_High();
	delay_us(5);
	IIC_SCL_Low();
}
//数据发送
void iic_send_byte(uint8_t data)
{
	uint8_t t;
	SDA_OUT();
	IIC_SCL_Low();
	for(t=0;t<8;t++)
	{
		if((data&0x80)>0)
			IIC_SDA_OUT_High();
		else
			IIC_SDA_OUT_Low();
		data<<=1;
		delay_us(2);
		IIC_SCL_High();
		delay_us(2);
		IIC_SCL_Low();
		delay_us(2);
	}
}
//数据读取
uint8_t iic_read_byte(uint8_t ack)
{
	uint8_t i=0,data=0;
	SDA_IN();
	for(i=0;i<8;i++)
	{
		IIC_SCL_Low();
		delay_us(2);
		IIC_SCL_High();
		data<<=1;
		if(IIC_SDA_IN())
			data++;
		delay_us(1);
	}
	if(!ack)
		iic_nack();
	else
		iic_ack();
	return data;
}

void select_i2c(uint8_t num)
{//DEV_NUM
	if(num > DEV_NUM)
		return;
	
	i2c_select = num;
}

static uint8_t IIC_SDA_IN()
{
	return GPIO_ReadInputDataBit(Atcs34725[i2c_select].GPIOx_SDA,Atcs34725[i2c_select].SDA.GPIO_Pin);
}

static void IIC_SDA_OUT_High()
{
	GPIO_SetBits(Atcs34725[i2c_select].GPIOx_SDA,Atcs34725[i2c_select].SDA.GPIO_Pin);
}

static void IIC_SDA_OUT_Low()
{
	GPIO_ResetBits(Atcs34725[i2c_select].GPIOx_SDA,Atcs34725[i2c_select].SDA.GPIO_Pin);
}

static void IIC_SCL_Low()
{
	GPIO_ResetBits(Atcs34725[i2c_select].GPIOx_SCL,Atcs34725[i2c_select].SCL.GPIO_Pin);
}

static void IIC_SCL_High()
{
	GPIO_SetBits(Atcs34725[i2c_select].GPIOx_SCL,Atcs34725[i2c_select].SCL.GPIO_Pin);
}

static void i2c_config(void)
{
	// ###
	Atcs34725[0].GPIOx_SCL = GPIOB;
	Atcs34725[0].GPIOx_SDA = GPIOB;
	
	Atcs34725[0].RCC_SCL = RCC_APB2Periph_GPIOB;
	Atcs34725[0].RCC_SDA = RCC_APB2Periph_GPIOB;
	
	Atcs34725[0].SCL.GPIO_Mode = GPIO_Mode_Out_PP;
	Atcs34725[0].SCL.GPIO_Pin = GPIO_Pin_8;
	Atcs34725[0].SCL.GPIO_Speed = GPIO_Speed_50MHz;
	
	Atcs34725[0].SDA.GPIO_Mode = GPIO_Mode_Out_PP;
	Atcs34725[0].SDA.GPIO_Pin = GPIO_Pin_9;
	Atcs34725[0].SDA.GPIO_Speed = GPIO_Speed_50MHz;
	
	// ###
	Atcs34725[1].GPIOx_SCL = GPIOB;
	Atcs34725[1].GPIOx_SDA = GPIOB;
	
	Atcs34725[1].RCC_SCL = RCC_APB2Periph_GPIOB;
	Atcs34725[1].RCC_SDA = RCC_APB2Periph_GPIOB;
			  
	Atcs34725[1].SCL.GPIO_Mode = GPIO_Mode_Out_PP;
	Atcs34725[1].SCL.GPIO_Pin = GPIO_Pin_10;
	Atcs34725[1].SCL.GPIO_Speed = GPIO_Speed_50MHz;
			  
	Atcs34725[1].SDA.GPIO_Mode = GPIO_Mode_Out_PP;
	Atcs34725[1].SDA.GPIO_Pin = GPIO_Pin_11;
	Atcs34725[1].SDA.GPIO_Speed = GPIO_Speed_50MHz;
	
	// ###
	Atcs34725[2].GPIOx_SCL = GPIOB;
	Atcs34725[2].GPIOx_SDA = GPIOB;
	
	Atcs34725[2].RCC_SCL = RCC_APB2Periph_GPIOB;
	Atcs34725[2].RCC_SDA = RCC_APB2Periph_GPIOB;
			  
	Atcs34725[2].SCL.GPIO_Mode = GPIO_Mode_Out_PP;
	Atcs34725[2].SCL.GPIO_Pin = GPIO_Pin_12;
	Atcs34725[2].SCL.GPIO_Speed = GPIO_Speed_50MHz;
			  
	Atcs34725[2].SDA.GPIO_Mode = GPIO_Mode_Out_PP;
	Atcs34725[2].SDA.GPIO_Pin = GPIO_Pin_13;
	Atcs34725[2].SDA.GPIO_Speed = GPIO_Speed_50MHz;
	
//	// ###
	Atcs34725[3].GPIOx_SCL = GPIOB;
	Atcs34725[3].GPIOx_SDA = GPIOB;
	
	Atcs34725[3].RCC_SCL = RCC_APB2Periph_GPIOB;
	Atcs34725[3].RCC_SDA = RCC_APB2Periph_GPIOB;
			  
	Atcs34725[3].SCL.GPIO_Mode = GPIO_Mode_Out_PP;
	Atcs34725[3].SCL.GPIO_Pin = GPIO_Pin_14;
	Atcs34725[3].SCL.GPIO_Speed = GPIO_Speed_50MHz;
			  
	Atcs34725[3].SDA.GPIO_Mode = GPIO_Mode_Out_PP;
	Atcs34725[3].SDA.GPIO_Pin = GPIO_Pin_15;
	Atcs34725[3].SDA.GPIO_Speed = GPIO_Speed_50MHz;
//	
//	// ###
	Atcs34725[4].GPIOx_SCL = GPIOB;
	Atcs34725[4].GPIOx_SDA = GPIOB;
	
	Atcs34725[4].RCC_SCL = RCC_APB2Periph_GPIOB;
	Atcs34725[4].RCC_SDA = RCC_APB2Periph_GPIOB;
			  
	Atcs34725[4].SCL.GPIO_Mode = GPIO_Mode_Out_PP;
	Atcs34725[4].SCL.GPIO_Pin = GPIO_Pin_0;
	Atcs34725[4].SCL.GPIO_Speed = GPIO_Speed_50MHz;
			  
	Atcs34725[4].SDA.GPIO_Mode = GPIO_Mode_Out_PP;
	Atcs34725[4].SDA.GPIO_Pin = GPIO_Pin_1;
	Atcs34725[4].SDA.GPIO_Speed = GPIO_Speed_50MHz;
	
}
