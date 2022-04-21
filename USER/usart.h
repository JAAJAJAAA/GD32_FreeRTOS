/*
 * @Description: 
 * @Author: mkx
 * @Date: 2022-04-11 15:47:35
 * @LastEditTime: 2022-04-12 15:10:26
 * @LastEditors: mkx
 * @FilePath: \GD32_FreeRTOS\USER\usart.h
 * @Reference: 
 */
#ifndef _USART_H_
#define _USART_H_

#include "UserConfig.h"


#define EVAL_COM1                        USART0
#define EVAL_COM1_CLK                    RCU_USART0
#define EVAL_COM1_TX_PIN                 GPIO_PIN_9
#define EVAL_COM1_RX_PIN                 GPIO_PIN_10
#define EVAL_COM1_GPIO_PORT              GPIOA
#define EVAL_COM1_GPIO_CLK               RCU_GPIOA

#define EVAL_COM2                        USART1
#define EVAL_COM2_CLK                    RCU_USART1
#define EVAL_COM2_TX_PIN                 GPIO_PIN_2
#define EVAL_COM2_RX_PIN                 GPIO_PIN_3
#define EVAL_COM2_GPIO_PORT              GPIOA
#define EVAL_COM2_GPIO_CLK               RCU_GPIOA



void Usart_Init(u32 bound);
void Usart0_Send_Byte(u8 data);



#endif
