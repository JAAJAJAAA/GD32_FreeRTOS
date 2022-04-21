/***************************************************
 * Description  :
 * LastEditTime : 2022-04-20 17:15:39
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\src\PWM.c
 ***************************************************/
#include "PWM.h"
#include "gd32f30x_timer.h"
#include "MotorTask.h"
/***************************************************
 * 名称:
 * 描述:
 * 参数: void
 * 返回: void
 ***************************************************/
void PWM_Init(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler = 120 - 1;                 // 预分频系数
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;    // 对齐方式
    timer_initpara.counterdirection = TIMER_COUNTER_UP; // 计数方式
    timer_initpara.period = 500;                        // 自动重装载值
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;    // 时钟源
    timer_initpara.repetitioncounter = 0;               // 重复计数器值
    timer_init(TIMER0, &timer_initpara);

    /* CH0 configuration in PWM mode */
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;          // 通道输出状态
    timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;        // 互补通道输出状态
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;     // 通道输出极性
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;   // 互补通道输出极性
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;   // 空闲状态下通道输出
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW; // 空闲状态下互补通道输出
    timer_channel_output_config(TIMER0, TIMER_CH_2, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, 250);                // 比较值设置
    timer_channel_output_mode_config(TIMER0, TIMER_CH_2, TIMER_OC_MODE_PWM0);        // 配置外设TIMERx通道输出比较模式
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE); // 配置TIMERx通道输出比较影子寄存器功能先决条件

    timer_primary_output_config(TIMER0, ENABLE); // 所有的通道输出使能
    timer_auto_reload_shadow_enable(TIMER0);     // TIMERx自动重载影子使能

    timer_interrupt_enable(TIMER0, TIMER_INT_UP);

    timer_enable(TIMER0); // 使能外设TIMERx
}

/***************************************************
 * 名称:
 * 描述:
 * 参数: void
 * 返回: void
 ***************************************************/
void PWMTestTask(void *pvParameters)
{
    u8 breathe_flag;
    int i = 0;
    while (1)
    {
        vTaskDelay(50);
        if (SET == breathe_flag)
        {
            i++;
            // printf("-");
        }
        else
        {
            i--;
            // printf("-");
        }
        if (50 <= i)
        {
            breathe_flag = RESET;
            // printf(" Breathe up \r\n");
        }
        if (0 >= i)
        {
            breathe_flag = SET;
            // printf(" Breathe down \r\n");
        }
        timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, i * 10);
    }
}

void TIMER0_UP_TIMER9_IRQHandler(void)
{
    if (timer_interrupt_flag_get(TIMER0, TIMER_INT_UP) != RESET)
    {
        printf("-");
        timer_interrupt_flag_clear(TIMER0, TIMER_INT_UP);
    }
}
