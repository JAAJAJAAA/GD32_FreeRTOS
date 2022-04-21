/*
 * @Description: 
 * @Author: mkx
 * @Date: 2022-04-11 11:26:40
 * @LastEditTime: 2022-04-11 17:56:03
 * @LastEditors: mkx
 * @FilePath: \GD32_FreeRTOS\LIB\Include\gd32f30x_it.h
 * @Reference: 
 */


#ifndef GD32F30X_IT_H
#define GD32F30X_IT_H

#include "gd32f30x.h"
#include "UserConfig.h"
/* function declarations */
/* this function handles NMI exception */
void NMI_Handler(void);
/* this function handles HardFault exception */
void HardFault_Handler(void);
/* this function handles MemManage exception */
void MemManage_Handler(void);
/* this function handles BusFault exception */
void BusFault_Handler(void);
/* this function handles UsageFault exception */
void UsageFault_Handler(void);
/* this function handles SVC exception */
void SVC_Handler(void);
/* this function handles DebugMon exception */
void DebugMon_Handler(void);
/* this function handles PendSV exception */
void PendSV_Handler(void);
/* this function handles SysTick exception */
void SysTick_Handler(void);

#endif /* GD32F30X_IT_H */
