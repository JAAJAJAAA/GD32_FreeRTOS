/***************************************************
 * Description  : ������غ����ļ�
 * LastEditTime : 2022-04-13 16:48:48
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\USER\usart.c
***************************************************/

#include "usart.h"
#include "GPIO.h"
#include "Key.h"
#include "usart.h"

extern u32 u32Usart0IRQSetTimes;


/***************************************************
 * ����: fputc
 * ����: �ض���printf����
 * ����: void
 * ����: void
***************************************************/
int fputc(int ch, FILE *f)
{
	usart_data_transmit(EVAL_COM1, (uint8_t)ch);
	while (RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE))
		;
	return ch;
}


/***************************************************
 * ����: Usart_Init
 * ����: ���ڳ�ʼ������
 * ����: bound-������
 * ����: void
***************************************************/
void Usart_Init(u32 bound)
{
	rcu_periph_clock_enable(EVAL_COM1_GPIO_CLK);												//ʹ��GPIOAʱ��
	rcu_periph_clock_enable(EVAL_COM1_CLK);														//ʹ��USARTʱ��
	gpio_init(EVAL_COM1_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, EVAL_COM1_TX_PIN);		//��ʼ�������PA9
	gpio_init(EVAL_COM1_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, EVAL_COM1_RX_PIN); //��ʼ�������PA10
	usart_deinit(USART0);																		//�ض���USART0
	usart_baudrate_set(USART0, bound);															//������ 115200
	usart_word_length_set(USART0, USART_WL_8BIT);												// 8λ����λ
	usart_stop_bit_set(USART0, USART_STB_1BIT);													// 1λֹͣλ
	usart_parity_config(USART0, USART_PM_NONE);													//����żУ��
	usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);									//����USART RTSӲ��������
	usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);									//����USART CTSӲ��������
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);											//ʹ��USART����
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);										//ʹ��USART����
	usart_interrupt_enable(USART0, USART_INT_RBNE);												//ʹ�ܽ����ж�
	nvic_irq_enable(USART0_IRQn, 1, 0);															//ѡ��USART0�ж���,�ж����ȼ��������ȼ�
	usart_enable(USART0);
}


/***************************************************
 * ����: Usart0_Send_Byte
 * ����: �ֽڷ��ͺ���
 * ����: data-�����ֽ�
 * ����: void
***************************************************/
void Usart0_Send_Byte(u8 data)
{
	usart_data_transmit(USART0, data);
}


/***************************************************
 * ����: USART0_IRQHandler
 * ����: ����0�жϷ�����
 * ����: void
 * ����: void
***************************************************/
void USART0_IRQHandler(void)
{
	if (usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) != RESET)
	{
		LED2 = !LED2;
		u32Usart0IRQSetTimes++;
		printf("USART0_IRQHandler has set %d times \r\n", u32Usart0IRQSetTimes);
		usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
	}
}
