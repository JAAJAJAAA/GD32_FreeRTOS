/***************************************************
 * Description  :
 * LastEditTime : 2022-04-18 09:42:06
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\include\PWM.h
 ***************************************************/
#ifndef _PWM_H_
#define _PWM_H_

void PWM_Init(void);
void PWMTestTask(void *pvParameters);
void TIMER0_UP_TIMER9_IRQHandler(void);
#endif
