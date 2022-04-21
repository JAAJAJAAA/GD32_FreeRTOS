/***************************************************
 * Description  : 主函数
 * LastEditTime : 2022-04-21 08:59:34
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\USER\main.c
 ***************************************************/

#include "UserConfig.h"
#include "typedef.h"
#include "GPIO.h"
#include "Key.h"
#include "ADC.h"
#include "i2c.h"
#include "semphr.h"
#include "MT6815.h"
#include "PWM.h"
#include "StoreTask.h"

static void ADC_Task(void *pvParameters);
static void Start_Task(void *pvParameters);
static void LED_Task(void *pvParameters);
static void KeyScan_Task(void *pvParameters);
static void StateJudgment_Task(void *pvParameters);
static void SPITest_Task(void *pvParameters);

extern QueueHandle_t xRunTimeQueue;
extern float temperature;
extern float vref_value;

extern SemaphoreHandle_t xBinarySemaphore;
extern SemaphoreHandle_t u32RuntimeSemaphore;
extern SemaphoreHandle_t xKeyMode;

extern TaskHandle_t Start_Task_Handler;
extern TaskHandle_t LED_Task_Handler;
extern TaskHandle_t ADC_Task_Handler;
extern TaskHandle_t KeyScan_Task_Handler;
extern TaskHandle_t StateJudgment_Task_Handler;
extern TaskHandle_t SPITest_Task_Handler;
extern TaskHandle_t PWMTest_Task_Handler;
/***************************************************
 * 名称: main
 * 描述: 主函数
 * 参数: void
 * 返回: void
 ***************************************************/
int main(void)
{
	/* 优先级分组设置 */
	nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
	Usart_Init(256000);
	printf(" \r\n");
	printf(" USART Init succeeded\r\n");
	LED_Init();
	printf(" LED Init succeeded\r\n");
	Key_Init();
	printf(" Key Init succeeded\r\n");
	ADC_Init();
	printf(" ADC Init succeeded\r\n");
	SPI_Init();
	printf(" SPI Init succeeded\r\n");
	PWM_Init();
	printf(" PWM Init succeeded\r\n");
	Store_Task_Init();
	printf(" Store Init succeeded\r\n");
	Motor_Task_Init();
	// printf(" Motor Init succeeded\r\n");
	xTaskCreate((TaskFunction_t)Start_Task,			  //任务函数
				(const char *)"start_task",			  //任务名称
				(uint16_t)configMINIMAL_STACK_SIZE,	  //任务堆栈大小
				(void *)NULL,						  //传递给任务函数的参数
				(UBaseType_t)2,						  //任务优先级
				(TaskHandle_t *)&Start_Task_Handler); //任务句柄
	vTaskStartScheduler();
	printf(" Possible RAM overflow!!! \r\n");
}

/***************************************************
 * 名称: Start_Task
 * 描述: 任务开始函数
 * 参数: void
 * 返回: void
 ***************************************************/
static void Start_Task(void *pvParameters)
{
	taskENTER_CRITICAL(); //进入临界区
	//创建任务
	xTaskCreate((TaskFunction_t)LED_Task,			// 任务函数
				(const char *)"LED_Task",			// 任务名称
				(uint16_t)configMINIMAL_STACK_SIZE, // 任务堆栈大小
				(void *)NULL,						// 传递给任务函数的参数
				(UBaseType_t)0,						// 任务优先级
				(TaskHandle_t *)&LED_Task_Handler); // 任务句柄
	printf(" The LED Task initialization succeeded \r\n");
	// ADC任务创建
	xTaskCreate((TaskFunction_t)ADC_Task,			// 任务函数
				(const char *)"ADC_Task",			// 任务名称
				(uint16_t)configMINIMAL_STACK_SIZE, // 任务堆栈大小
				(void *)NULL,						// 传递给任务函数的参数
				(UBaseType_t)1,						// 任务优先级
				(TaskHandle_t *)&ADC_Task_Handler); // 任务句柄
	printf(" The ADC Task initialization succeeded \r\n");
	//按键处理任务
	xTaskCreate((TaskFunction_t)KeyScan_Task,			// 任务函数
				(const char *)"KeyScan_Task",			// 任务名称
				(uint16_t)configMINIMAL_STACK_SIZE,		// 任务堆栈大小
				(void *)NULL,							// 传递给任务函数的参数
				(UBaseType_t)2,							// 任务优先级
				(TaskHandle_t *)&KeyScan_Task_Handler); // 任务句柄
	printf(" The KeyScan Task initialization succeeded \r\n");
	//状态改变任务
	xTaskCreate((TaskFunction_t)StateJudgment_Task,		// 任务函数
				(const char *)"StateJudgment_Task",		// 任务名称
				(uint16_t)configMINIMAL_STACK_SIZE,		// 任务堆栈大小
				(void *)NULL,							// 传递给任务函数的参数
				(UBaseType_t)1,							// 任务优先级
				(TaskHandle_t *)&SPITest_Task_Handler); // 任务句柄
	printf(" The KeyScan Task initialization succeeded \r\n");
	// SPI测试任务
	xTaskCreate((TaskFunction_t)SPITest_Task,				  // 任务函数
				(const char *)"SPITest_Task",				  // 任务名称
				(uint16_t)configMINIMAL_STACK_SIZE,			  // 任务堆栈大小
				(void *)NULL,								  // 传递给任务函数的参数
				(UBaseType_t)1,								  // 任务优先级
				(TaskHandle_t *)&StateJudgment_Task_Handler); // 任务句柄
	printf(" The KeyScan Task initialization succeeded \r\n");
	// PWM测试任务
	xTaskCreate((TaskFunction_t)PWMTestTask,			// 任务函数
				(const char *)"PWMTestTask",			// 任务名称
				(uint16_t)configMINIMAL_STACK_SIZE,		// 任务堆栈大小
				(void *)NULL,							// 传递给任务函数的参数
				(UBaseType_t)1,							// 任务优先级
				(TaskHandle_t *)&PWMTest_Task_Handler); // 任务句柄
	printf(" The PWM Task initialization succeeded \r\n");
	xRunTimeQueue = xQueueCreate(2, sizeof(int32_t)); // 创建测试队列
	printf(" Queue creation succeeded \r\n");

	xBinarySemaphore = xSemaphoreCreateBinary();			// 创建测试二进制信号量
	xKeyMode = xSemaphoreCreateBinary();					// 创建二进制开关信号量
	u32RuntimeSemaphore = xSemaphoreCreateCounting(255, 0); // 创建测试计数信号量
	printf(" Semaphore creation succeeded \r\n");

	vTaskDelete(Start_Task_Handler); //删除开始任务
	printf(" The Start Task  is deleted successfully \r\n");
	printf(" System running...\r\n");
	taskEXIT_CRITICAL(); //退出临界区
}

/***************************************************
 * 名称: LED_Task
 * 描述: LED及运行计时任务
 * 参数: void *pvParameters
 * 返回: void
 ***************************************************/
static void LED_Task(void *pvParameters)
{
#if SYSTEM_RUN_TIME_REMIND
	u32 u32LedTaskRunTime;
#endif
	while (1)
	{
		LED1 = !LED1;
#if SYSTEM_RUN_TIME_REMIND_SEMAPHORE
		if (!xSemaphoreGive(u32RuntimeSemaphore))
		{
			printf(" Check whether the queue was successfully read\r\n");
		}
		else
		{
			printf(" The system has been running for %d seconds \r\n", uxSemaphoreGetCount(u32RuntimeSemaphore));
		}

#endif

#if SYSTEM_RUN_TIME_REMIND_QUEUE
		if (!xQueuePeek(xRunTimeQueue, &u32LedTaskRunTime, 0))
		{
			printf("Check whether the queue was successfully read\r\n");
		}
		u32LedTaskRunTime += 1;
		xQueueOverwrite(xRunTimeQueue, &u32LedTaskRunTime);
		printf("The system has been running for %d seconds \r\n", u32LedTaskRunTime);
#endif
		vTaskDelay(994);
	}
}

/***************************************************
 * 名称: SPITest_Task
 * 描述: SPI测试任务
 * 参数: void *pvParameters
 * 返回: void
 ***************************************************/
static void SPITest_Task(void *pvParameters)
{
	while (1)
	{
		SPI_SendByte(0xA5);
	}
}

/***************************************************
 * 名称: StateJudgment_Task
 * 描述: 状态判断任务函数
 * 参数: void *pvParameters
 * 返回: void
 ***************************************************/
static void StateJudgment_Task(void *pvParameters)
{
	while (1)
	{
		if (bADCEnable)
		{
			if (uxSemaphoreGetCount(xBinarySemaphore) == 1)
			{
			}
			else
			{
				xSemaphoreGive(xBinarySemaphore);
			}
		}
		vTaskDelay(10);
	}
}

/***************************************************
 * 名称: KeyScan_Task
 * 描述: 按键判断任务函数
 * 参数: void *pvParameters
 * 返回: void
 ***************************************************/
static void KeyScan_Task(void *pvParameters)
{
	// u8 u8KeyMode;
	while (1)
	{
		if (xSemaphoreTake(xKeyMode, 100))
		{
			bADCEnable = !bADCEnable;
			if (bADCEnable)
			{
				printf(" AD Switch on\r\n");
			}
			else
			{
				printf(" AD Switch off\r\n");
				xSemaphoreTake(xBinarySemaphore, 0);
			}
		}
		vTaskDelay(10);
	}
}

/***************************************************
 * 名称: ADC_Task
 * 描述: ADC任务函数
 * 参数: void
 * 返回: void *pvParameters
 ***************************************************/
static void ADC_Task(void *pvParameters)
{
	while (1)
	{
		// printf("run\r\n");
		if (xSemaphoreTake(xBinarySemaphore, 1000))
		{
			adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
			/* delay a time in milliseconds */
			vTaskDelay(1000);
			if (xSemaphoreTake(xBinarySemaphore, 0))
			{
				/* value convert  */
				temperature = (1.43 - ADC_IDATA0(ADC0) * 3.3 / 4096) * 1000 / 4.3 + 25;
				vref_value = (ADC_IDATA1(ADC0) * 3.3 / 4096);
				/* value print */
				printf("\r\n the temperature data is %2.0f degrees Celsius\r\n", temperature);
				printf(" the reference voltage data is %5.3fV \r\n", vref_value);
				printf(" \r\n");
			}
		}
	}
}
