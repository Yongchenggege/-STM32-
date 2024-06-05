#include "main.h"

// 传感器读取标志
_Bool isready = 0;

// 传感器rgbc通道数据
COLOR_RGBC rgb[DEV_NUM];
uint32_t cct_buf[DEV_NUM];

// 颜色识别结果
uint8_t color_result[DEV_NUM]={COLOR_NONE,COLOR_NONE,COLOR_NONE,COLOR_NONE,COLOR_NONE};

// 线程（任务）
void read_color_data_task(void * pvParameters);

void uart2ToRS485_data_processing(void * pvParameters);

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	usart1_init(9600);
	
	usart2_init(9600);
	
	PARAM_INIT();
	
	TCS34725_Init();
	
	// 根据tcs34725设置的积分时间决定 积分时间(24ms) + 10ms 
	//32位 一个深度就是4个字节  256*4=1024字节
	TIM3_Int_Init(340-1,7200-1); // 34ms中断一次
	
	xTaskCreate(read_color_data_task, "Task_1", 256, NULL, 2, NULL);
	
	xTaskCreate(uart2ToRS485_data_processing, "Task_2", 512, NULL, 3, NULL);
	
	vTaskStartScheduler();
}

// 读取传感器数据
void read_color_data_task(void * pvParameters)
{
	while(1)
	{
		if(isready) 
		{
			for(int i=0;i<DEV_NUM;i++)
			{
				TCS34725_GetRawData(&rgb[i],i);
				
				COLOR_RECOGNITION();
				printf("%d\r\n",cct_buf[1]);
			}	
			isready = 0;
		}
		vTaskDelay(10);
	}
}

// 处理串口2收发的消息
void uart2ToRS485_data_processing(void * pvParameters)
{
	while(1)
	{
		usart2_receive_process();
		
		vTaskDelay(20);
	}
}

// 串口2消息处理
void usart2_callBack(void)
{
	RTU_t RTU_pack;
	
	RTU_unpack(usart2_buf,&RTU_pack);
	
	if(RTU_pack.valid == 0)
		printf("解包出错\n");
}
