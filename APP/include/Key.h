/*
 * @Description: 
 * @Author: mkx
 * @Date: 2022-04-11 14:30:18
 * @LastEditTime: 2022-04-11 15:53:54
 * @LastEditors: mkx
 * @FilePath: \GD32_FreeRTOS\APP\include\Key.h
 * @Reference: 
 */
#ifndef _KEY_H_
#define _KEY_H_

#include "UserConfig.h"
#include "typedef.h"


#define KEY1 PAin(0)

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

void Key_Init(void);
u8 KeyScan(u8 mode);

#endif
