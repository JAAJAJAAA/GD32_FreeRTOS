/*
 * @Description:
 * @Author: mkx
 * @Date: 2022-04-12 14:59:57
 * @LastEditTime: 2022-04-12 15:11:14
 * @LastEditors: mkx
 * @FilePath: \GD32_FreeRTOS\APP\src\i2c.c
 * @Reference:
 */

#include "i2c.h"

/**
 * @description: 初始化I2C所需要的IO口
 * @param  {*}
 * @return {*}
 */
void I2cIo_Init(void)
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable I2C0 clock */
    rcu_periph_clock_enable(RCU_I2C0);
    /* connect PB6 to I2C0_SCL */
    /* connect PB7 to I2C0_SDA */
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
}

/**
 * @description: I2C初始化函数
 * @param  {*}
 * @return {*}
 */
void I2c_Init(void)
{
    I2cIo_Init();
    /* enable I2C clock */
    rcu_periph_clock_enable(RCU_I2C0);
    /* configure I2C clock */
    i2c_clock_config(I2C0, I2C0_SPEED, I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_SLAVE_ADDRESS7);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}
