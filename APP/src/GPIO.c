/***************************************************
 * Description  :
 * LastEditTime : 2022-04-18 11:47:38
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\src\GPIO.c
 ***************************************************/
/*
 * @Description:
 * @Author: mkx
 * @Date: 2022-04-11 11:26:40
 * @LastEditTime: 2022-04-11 17:16:24
 * @LastEditors: mkx
 * @FilePath: \GD32_FreeRTOS\APP\src\GPIO.c
 * @Reference:
 */
#include "GPIO.h"

/**
 * @description: LED³õÊ¼»¯º¯Êý
 * @param  {*}
 * @return {*}
 */
void LED_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	// gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
	// gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
	gpio_bit_reset(GPIOB, GPIO_PIN_14);
	gpio_bit_reset(GPIOB, GPIO_PIN_15);
}
