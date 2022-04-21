/***************************************************
 * Description  :
 * LastEditTime : 2022-04-19 11:07:23
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\USER\UserFlag.c
 ***************************************************/

#include "FreeRTOS.h"
#include "typedef.h"
#include "queue.h"
#include "semphr.h"
#include "StoreTask.h"
#include "MotorTask.h"
TYPE_BYTE unFlag0;
TYPE_BYTE unFlag1;

u32 u32Usart0IRQSetTimes; // �����жϴ�������
float temperature;        // �¶�
float vref_value;         // ADֵ

TaskHandle_t Start_Task_Handler;         // ��ʼ������
TaskHandle_t LED_Task_Handler;           // IO�ڳ�ʼ��������
TaskHandle_t ADC_Task_Handler;           // AD������
TaskHandle_t KeyScan_Task_Handler;       // ����������
TaskHandle_t StateJudgment_Task_Handler; // ״̬�ж�������
TaskHandle_t SPITest_Task_Handler;       // SPI����������
TaskHandle_t PWMTest_Task_Handler;       // SPI����������

SemaphoreHandle_t xBinarySemaphore;    // �������ź�������
SemaphoreHandle_t xKeyMode;            // �������ź�������
SemaphoreHandle_t u32RuntimeSemaphore; // �����ź�������

QueueHandle_t xRunTimeQueue; // ����ʱ�����


//У׼��ɺ��г�
Trave_TypeDef Global_Trave;

uint16_t currentdata[2];
