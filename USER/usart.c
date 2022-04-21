/***************************************************
 * Description  : 串口相关函数文件
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
 * 名称: fputc
 * 描述: 重定义printf函数
 * 参数: void
 * 返回: void
***************************************************/
int fputc(int ch, FILE *f)
{
	usart_data_transmit(EVAL_COM1, (uint8_t)ch);
	while (RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE))
		;
	return ch;
}


/***************************************************
 * 名称: Usart_Init
 * 描述: 串口初始换函数
 * 参数: bound-波特率
 * 返回: void
***************************************************/
void Usart_Init(u32 bound)
{
	rcu_periph_clock_enable(EVAL_COM1_GPIO_CLK);												//使能GPIOA时钟
	rcu_periph_clock_enable(EVAL_COM1_CLK);														//使能USART时钟
	gpio_init(EVAL_COM1_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, EVAL_COM1_TX_PIN);		//初始化输出脚PA9
	gpio_init(EVAL_COM1_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, EVAL_COM1_RX_PIN); //初始化输入脚PA10
	usart_deinit(USART0);																		//重定义USART0
	usart_baudrate_set(USART0, bound);															//波特率 115200
	usart_word_length_set(USART0, USART_WL_8BIT);												// 8位数据位
	usart_stop_bit_set(USART0, USART_STB_1BIT);													// 1位停止位
	usart_parity_config(USART0, USART_PM_NONE);													//无奇偶校验
	usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);									//配置USART RTS硬件控制流
	usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);									//配置USART CTS硬件控制流
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);											//使能USART接收
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);										//使能USART发送
	usart_interrupt_enable(USART0, USART_INT_RBNE);												//使能接收中断
	nvic_irq_enable(USART0_IRQn, 1, 0);															//选择USART0中断线,中断优先级和子优先级
	usart_enable(USART0);
}


/***************************************************
 * 名称: Usart0_Send_Byte
 * 描述: 字节发送函数
 * 参数: data-发送字节
 * 返回: void
***************************************************/
void Usart0_Send_Byte(u8 data)
{
	usart_data_transmit(USART0, data);
}


/***************************************************
 * 名称: USART0_IRQHandler
 * 描述: 串口0中断服务函数
 * 参数: void
 * 返回: void
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
