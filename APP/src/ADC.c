/*
 * @Description: 
 * @Author: mkx
 * @Date: 2022-04-12 09:40:17
 * @LastEditTime: 2022-04-12 10:37:31
 * @LastEditors: mkx
 * @FilePath: \GD32_FreeRTOS\APP\src\ADC.c
 * @Reference: 
 */
#include "ADC.h"


void ADC_Init(void)
{
    /* ADC ����ģʽ */
    adc_mode_config(ADC_MODE_FREE);
    /* ����ADCx���ݶ��뷽ʽ */
    adc_data_alignment_config(ADC0,ADC_DATAALIGN_RIGHT);
    /* ʹ�ܻ����ADC���⹦�� */
    adc_special_function_config(ADC0,ADC_SCAN_MODE,ENABLE);  

    /* ���ù���ͨ�����ע��ͨ����ĳ��� */
    adc_channel_length_config(ADC0,ADC_INSERTED_CHANNEL,2);
    /* A����ADCע��ͨ���� */
    adc_inserted_channel_config(ADC0, 0, ADC_CHANNEL_16, ADC_SAMPLETIME_239POINT5);
    /* ����ADCע��ͨ���� */
    adc_inserted_channel_config(ADC0, 1, ADC_CHANNEL_17, ADC_SAMPLETIME_239POINT5);

    /* ����ADC�ⲿ���� */
    adc_external_trigger_config(ADC0,ADC_INSERTED_CHANNEL,ENABLE);
    /* ����ADC�ⲿ����Դ */
    adc_external_trigger_source_config(ADC0, ADC_INSERTED_CHANNEL, ADC0_1_2_EXTTRIG_INSERTED_NONE);

    /* �¶ȴ�������Vrefintͨ��ʹ�� */
    adc_tempsensor_vrefint_enable();
    
    /* enable ADC interface */
    adc_enable(ADC0);
    //vTaskDelay(1);    
    /* ADCxУ׼��λ */
    adc_calibration_enable(ADC0);
}

