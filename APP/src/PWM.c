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
 * ����:
 * ����:
 * ����: void
 * ����: void
 ***************************************************/
void PWM_Init(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler = 120 - 1;                 // Ԥ��Ƶϵ��
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;    // ���뷽ʽ
    timer_initpara.counterdirection = TIMER_COUNTER_UP; // ������ʽ
    timer_initpara.period = 500;                        // �Զ���װ��ֵ
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;    // ʱ��Դ
    timer_initpara.repetitioncounter = 0;               // �ظ�������ֵ
    timer_init(TIMER0, &timer_initpara);

    /* CH0 configuration in PWM mode */
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;          // ͨ�����״̬
    timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;        // ����ͨ�����״̬
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;     // ͨ���������
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;   // ����ͨ���������
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;   // ����״̬��ͨ�����
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW; // ����״̬�»���ͨ�����
    timer_channel_output_config(TIMER0, TIMER_CH_2, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, 250);                // �Ƚ�ֵ����
    timer_channel_output_mode_config(TIMER0, TIMER_CH_2, TIMER_OC_MODE_PWM0);        // ��������TIMERxͨ������Ƚ�ģʽ
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE); // ����TIMERxͨ������Ƚ�Ӱ�ӼĴ��������Ⱦ�����

    timer_primary_output_config(TIMER0, ENABLE); // ���е�ͨ�����ʹ��
    timer_auto_reload_shadow_enable(TIMER0);     // TIMERx�Զ�����Ӱ��ʹ��

    timer_interrupt_enable(TIMER0, TIMER_INT_UP);

    timer_enable(TIMER0); // ʹ������TIMERx
}

/***************************************************
 * ����:
 * ����:
 * ����: void
 * ����: void
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
