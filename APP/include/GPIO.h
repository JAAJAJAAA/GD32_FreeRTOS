/*
 * @Description: 
 * @Author: mkx
 * @Date: 2022-04-11 11:26:40
 * @LastEditTime: 2022-04-11 14:21:08
 * @LastEditors: mkx
 * @FilePath: \GD32_FreeRTOS\APP\include\GPIO.h
 * @Reference: 
 */
#ifndef _GPIO_H_
#define _GPIO_H_

#include "UserConfig.h"

#define LED1 PBout(14)
#define LED2 PBout(15)




void LED_Init(void);

#endif
