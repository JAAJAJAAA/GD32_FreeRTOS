/***************************************************
 * Description  : 按键相关函数文件
 * LastEditTime : 2022-04-13 16:50:11
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\src\Key.c
***************************************************/

#include "Key.h"
#include "GPIO.h"

extern SemaphoreHandle_t xKeyMode;

/***************************************************
 * 名称: Key_Init
 * 描述: 按键初始换函数
 * 参数: void
 * 返回: void
***************************************************/
void Key_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);                                     /* 使能GPIOA外设时钟 */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_0); /* A0浮空输入 */

    rcu_periph_clock_enable(RCU_AF);
    nvic_irq_enable(EXTI0_IRQn, 2U, 0U);
    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_0);
    /* configure key EXTI line */
    exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_0);
}



/***************************************************
 * 名称: EXTI0_IRQHandler
 * 描述: 外部中断0中断服务函数
 * 参数: void
 * 返回: void
***************************************************/
void EXTI0_IRQHandler(void)
{
    if (RESET != exti_interrupt_flag_get(EXTI_0))
    {
        printf(" Enter external interrupt 0\r\n");
        LED2 = !LED2;
        if(!xSemaphoreGiveFromISR(xKeyMode, (BaseType_t *)pdTRUE))
        {
            printf(" Give semaphore failure\r\n");
        }
        printf(" Send key semaphore\r\n");
        exti_interrupt_flag_clear(EXTI_0);
        printf(" Clearing external interrupt 0\r\n");
    }
}
