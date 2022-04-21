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

u32 u32Usart0IRQSetTimes; // 串口中断触发数量
float temperature;        // 温度
float vref_value;         // AD值

TaskHandle_t Start_Task_Handler;         // 起始任务句柄
TaskHandle_t LED_Task_Handler;           // IO口初始化任务句柄
TaskHandle_t ADC_Task_Handler;           // AD任务句柄
TaskHandle_t KeyScan_Task_Handler;       // 按键任务句柄
TaskHandle_t StateJudgment_Task_Handler; // 状态判断任务句柄
TaskHandle_t SPITest_Task_Handler;       // SPI测试任务句柄
TaskHandle_t PWMTest_Task_Handler;       // SPI测试任务句柄

SemaphoreHandle_t xBinarySemaphore;    // 二进制信号量测试
SemaphoreHandle_t xKeyMode;            // 二进制信号量测试
SemaphoreHandle_t u32RuntimeSemaphore; // 计数信号量测试

QueueHandle_t xRunTimeQueue; // 运行时间队列


//校准完成后行程
Trave_TypeDef Global_Trave;

uint16_t currentdata[2];
