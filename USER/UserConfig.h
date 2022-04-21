/***************************************************
 * Description  : �û�����ͷ�ļ�
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

//�ö��н���ϵͳ����ʱ���ʱ
#ifndef SYSTEM_RUN_TIME_REMIND_QUEUE
#define SYSTEM_RUN_TIME_REMIND_QUEUE 0
#endif
//���ź�������ϵͳ����ʱ���ʱ
#ifndef SYSTEM_RUN_TIME_REMIND_SEMAPHORE
#define SYSTEM_RUN_TIME_REMIND_SEMAPHORE 1
#endif

#define u8TestMode unFlag0.stBit.b0 //����ģʽ
#define bADCEnable unFlag0.stBit.b1 // ADC��ȡ����λ

// #define WAIT_FOREVER 0xFFFFFFFF     //�ȴ�ʱ�䶨��
#define WAIT_FOREVER 0 //�ȴ�ʱ�䶨��

#define osThreadId TaskHandle_t
#endif
