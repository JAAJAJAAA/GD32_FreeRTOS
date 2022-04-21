/***************************************************
 * Description  : 用户配置头文件
 * LastEditTime : 2022-04-21 09:10:44
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\USER\UserConfig.h
 ***************************************************/

#ifndef _USERCONFIG_H_
#define _USERCONFIG_H_

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "gd32f30x.h"
#include "gd32f303c_start.h" 
#include "systick.h"
#include "FreeRTOS.h" 
#include "task.h"
#include "typedef.h"
#include "queue.h"
#include "semphr.h"
#include "usart.h"

extern TYPE_BYTE unFlag0;
extern TYPE_BYTE unFlag1;

//用队列进行系统运行时间计时
#ifndef SYSTEM_RUN_TIME_REMIND_QUEUE
#define SYSTEM_RUN_TIME_REMIND_QUEUE 0
#endif
//用信号量进行系统运行时间计时
#ifndef SYSTEM_RUN_TIME_REMIND_SEMAPHORE
#define SYSTEM_RUN_TIME_REMIND_SEMAPHORE 1
#endif

#define u8TestMode unFlag0.stBit.b0 //测试模式
#define bADCEnable unFlag0.stBit.b1 // ADC读取允许位

// #define WAIT_FOREVER 0xFFFFFFFF     //等待时间定义
#define WAIT_FOREVER 0 //等待时间定义

#define osThreadId TaskHandle_t
#endif
