/***************************************************
 * Description  : ������
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
 * ����: main
 * ����: ������
 * ����: void
 * ����: void
 ***************************************************/
int main(void)
{
	/* ���ȼ��������� */
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
	xTaskCreate((TaskFunction_t)Start_Task,			  //������
				(const char *)"start_task",			  //��������
				(uint16_t)configMINIMAL_STACK_SIZE,	  //�����ջ��С
				(void *)NULL,						  //���ݸ��������Ĳ���
				(UBaseType_t)2,						  //�������ȼ�
				(TaskHandle_t *)&Start_Task_Handler); //������
	vTaskStartScheduler();
	printf(" Possible RAM overflow!!! \r\n");
}

/***************************************************
 * ����: Start_Task
 * ����: ����ʼ����
 * ����: void
 * ����: void
 ***************************************************/
static void Start_Task(void *pvParameters)
{
	taskENTER_CRITICAL(); //�����ٽ���
	//��������
	xTaskCreate((TaskFunction_t)LED_Task,			// ������
				(const char *)"LED_Task",			// ��������
				(uint16_t)configMINIMAL_STACK_SIZE, // �����ջ��С
				(void *)NULL,						// ���ݸ��������Ĳ���
				(UBaseType_t)0,						// �������ȼ�
				(TaskHandle_t *)&LED_Task_Handler); // ������
	printf(" The LED Task initialization succeeded \r\n");
	// ADC���񴴽�
	xTaskCreate((TaskFunction_t)ADC_Task,			// ������
				(const char *)"ADC_Task",			// ��������
				(uint16_t)configMINIMAL_STACK_SIZE, // �����ջ��С
				(void *)NULL,						// ���ݸ��������Ĳ���
				(UBaseType_t)1,						// �������ȼ�
				(TaskHandle_t *)&ADC_Task_Handler); // ������
	printf(" The ADC Task initialization succeeded \r\n");
	//������������
	xTaskCreate((TaskFunction_t)KeyScan_Task,			// ������
				(const char *)"KeyScan_Task",			// ��������
				(uint16_t)configMINIMAL_STACK_SIZE,		// �����ջ��С
				(void *)NULL,							// ���ݸ��������Ĳ���
				(UBaseType_t)2,							// �������ȼ�
				(TaskHandle_t *)&KeyScan_Task_Handler); // ������
	printf(" The KeyScan Task initialization succeeded \r\n");
	//״̬�ı�����
	xTaskCreate((TaskFunction_t)StateJudgment_Task,		// ������
				(const char *)"StateJudgment_Task",		// ��������
				(uint16_t)configMINIMAL_STACK_SIZE,		// �����ջ��С
				(void *)NULL,							// ���ݸ��������Ĳ���
				(UBaseType_t)1,							// �������ȼ�
				(TaskHandle_t *)&SPITest_Task_Handler); // ������
	printf(" The KeyScan Task initialization succeeded \r\n");
	// SPI��������
	xTaskCreate((TaskFunction_t)SPITest_Task,				  // ������
				(const char *)"SPITest_Task",				  // ��������
				(uint16_t)configMINIMAL_STACK_SIZE,			  // �����ջ��С
				(void *)NULL,								  // ���ݸ��������Ĳ���
				(UBaseType_t)1,								  // �������ȼ�
				(TaskHandle_t *)&StateJudgment_Task_Handler); // ������
	printf(" The KeyScan Task initialization succeeded \r\n");
	// PWM��������
	xTaskCreate((TaskFunction_t)PWMTestTask,			// ������
				(const char *)"PWMTestTask",			// ��������
				(uint16_t)configMINIMAL_STACK_SIZE,		// �����ջ��С
				(void *)NULL,							// ���ݸ��������Ĳ���
				(UBaseType_t)1,							// �������ȼ�
				(TaskHandle_t *)&PWMTest_Task_Handler); // ������
	printf(" The PWM Task initialization succeeded \r\n");
	xRunTimeQueue = xQueueCreate(2, sizeof(int32_t)); // �������Զ���
	printf(" Queue creation succeeded \r\n");

	xBinarySemaphore = xSemaphoreCreateBinary();			// �������Զ������ź���
	xKeyMode = xSemaphoreCreateBinary();					// ���������ƿ����ź���
	u32RuntimeSemaphore = xSemaphoreCreateCounting(255, 0); // �������Լ����ź���
	printf(" Semaphore creation succeeded \r\n");

	vTaskDelete(Start_Task_Handler); //ɾ����ʼ����
	printf(" The Start Task  is deleted successfully \r\n");
	printf(" System running...\r\n");
	taskEXIT_CRITICAL(); //�˳��ٽ���
}

/***************************************************
 * ����: LED_Task
 * ����: LED�����м�ʱ����
 * ����: void *pvParameters
 * ����: void
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
 * ����: SPITest_Task
 * ����: SPI��������
 * ����: void *pvParameters
 * ����: void
 ***************************************************/
static void SPITest_Task(void *pvParameters)
{
	while (1)
	{
		SPI_SendByte(0xA5);
	}
}

/***************************************************
 * ����: StateJudgment_Task
 * ����: ״̬�ж�������
 * ����: void *pvParameters
 * ����: void
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
 * ����: KeyScan_Task
 * ����: �����ж�������
 * ����: void *pvParameters
 * ����: void
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
 * ����: ADC_Task
 * ����: ADC������
 * ����: void
 * ����: void *pvParameters
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
