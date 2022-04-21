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
    /* ADC 独立模式 */
    adc_mode_config(ADC_MODE_FREE);
    /* 配置ADCx数据对齐方式 */
    adc_data_alignment_config(ADC0,ADC_DATAALIGN_RIGHT);
    /* 使能或禁能ADC特殊功能 */
    adc_special_function_config(ADC0,ADC_SCAN_MODE,ENABLE);  

    /* 配置规则通道组或注入通道组的长度 */
    adc_channel_length_config(ADC0,ADC_INSERTED_CHANNEL,2);
    /* A配置ADC注入通道组 */
    adc_inserted_channel_config(ADC0, 0, ADC_CHANNEL_16, ADC_SAMPLETIME_239POINT5);
    /* 配置ADC注入通道组 */
    adc_inserted_channel_config(ADC0, 1, ADC_CHANNEL_17, ADC_SAMPLETIME_239POINT5);

    /* 配置ADC外部触发 */
    adc_external_trigger_config(ADC0,ADC_INSERTED_CHANNEL,ENABLE);
    /* 配置ADC外部触发源 */
    adc_external_trigger_source_config(ADC0, ADC_INSERTED_CHANNEL, ADC0_1_2_EXTTRIG_INSERTED_NONE);

    /* 温度传感器和Vrefint通道使能 */
    adc_tempsensor_vrefint_enable();
    
    /* enable ADC interface */
    adc_enable(ADC0);
    //vTaskDelay(1);    
    /* ADCx校准复位 */
    adc_calibration_enable(ADC0);
}

