/***************************************************
 * Description  : MT6815相关配置函数文件
 * LastEditTime : 2022-04-15 16:28:19
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\src\MT6815.c
 ***************************************************/
#include "MT6815.h"
#include "MotorTask.h"

/***************************************************
 * 名称: spi_flash_init
 * 描述: SPI初始化函数
 * 参数: void
 * 返回: void
 ***************************************************/
void SPI_Init(void)
{
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_SPI0);

    /* SPI0_SCK(PA5), SPI0_MISO(PA6) and SPI0_MOSI(PA7) GPIO pin configuration */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    /* SPI0_CS(PE3) GPIO pin configuration */
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    /* chip select invalid*/
    SPI_FLASH_CS_HIGH();

    /* SPI0 parameter config */
    spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;         // 全双工模式
    spi_init_struct.device_mode = SPI_MASTER;                      // 主机
    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;               // 8位
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE; // 相位和极性配置，SCK空闲为低电平，数据在SCK的上升沿被采样
    spi_init_struct.nss = SPI_NSS_SOFT;                            // NSS控制方式
    spi_init_struct.prescale = SPI_PSC_8;                          // 预分频系数
    spi_init_struct.endian = SPI_ENDIAN_MSB;                       // 大端
    spi_init(SPI0, &spi_init_struct);
    spi_enable(SPI0); //使能SPI0
}

/***************************************************
 * 名称: SPI_ReadByte
 * 描述: SPI接收函数
 * 参数: void
 * 返回: uint8_t
 ***************************************************/
uint8_t SPI_ReadByte(void)
{
    return (SPI_SendByte(DUMMY_BYTE));
}
/*!
    \brief      SPI transmit data
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  data: 16-bit data
    \param[out] none
    \retval     none
*/
/***************************************************
 * 名称: SPI_I2sDataTansmit
 * 描述:
 * 参数: spi_periph-SPIx;data-数据
 * 返回: void
 ***************************************************/
void SPI_I2sDataTansmit(uint32_t spi_periph, uint16_t data)
{
    SPI_DATA(spi_periph) = (uint32_t)data;
}

/***************************************************
 * 名称: SPI_I2sDataReceive
 * 描述: SPI receive data
 * 参数: spi_periph: SPIx(x=0,1,2)
 * 返回: uint16_t
 ***************************************************/
uint16_t SPI_I2sDataReceive(uint32_t spi_periph)
{
    return ((uint16_t)SPI_DATA(spi_periph));
}
/***************************************************
 * 名称: SpiFlashSendByte
 * 描述: SPI发送函数
 * 参数: byte 要发送的字节
 * 返回: uint8_t
 ***************************************************/
uint8_t SPI_SendByte(uint8_t byte)
{
    while (RESET == SPI_I2sFlagGet(SPI0, SPI_FLAG_TBE))
        ;                           //等待寄存器清空
    SPI_I2sDataTansmit(SPI0, byte); //发送数据
    while (RESET == SPI_I2sFlagGet(SPI0, SPI_FLAG_RBNE))
        ;                              //等待回传
    return (SPI_I2sDataReceive(SPI0)); //返回接收数据
}

/***************************************************
 * 名称: SPI_I2sFlagGet
 * 描述: SPI标志位读取
 * 参数: void
 * 返回: void
 ***************************************************/
FlagStatus SPI_I2sFlagGet(uint32_t spi_periph, uint32_t flag)
{
    if (SPI_STAT(spi_periph) & flag)
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}
/***************************************************
 * 名称: MT6815_READ_NEW_DATA
 * 描述: 开始一次新的读
 * 参数: void
 * 返回: uint16_t
 ***************************************************/
uint16_t MT6815_READ_NEW_DATA(void)
{
    uint16_t data[2];
    uint16_t tempdata;
    uint16_t ret_data;
    taskDISABLE_INTERRUPTS();
    ClearDR(); //清DR寄存器
    CsON();
    tempdata = (0x1 << 7) | (0x3);
    // Global_hspi2.Instance->DR = tempdata; //写DR寄存器
    // while(__HAL_SPI_GET_FLAG(&Global_hspi2,SPI_FLAG_RXNE) == pdFALSE);
    ClearDR(); //清DR寄存器
    tempdata = 0;
    // Global_hspi2.Instance->DR = tempdata;
    // while(__HAL_SPI_GET_FLAG(&Global_hspi2,SPI_FLAG_RXNE)== pdFALSE);
    // tempdata = Global_hspi2.Instance->DR; //读DR寄存器
    data[0] = tempdata & 0xff;
    tempdata = 0;
    // Global_hspi2.Instance->DR = tempdata; //写DR寄存器
    //  while(__HAL_SPI_GET_FLAG(&Global_hspi2,SPI_FLAG_RXNE) == pdFALSE);
    //  tempdata =  Global_hspi2.Instance->DR;//读DR寄存器
    data[1] = tempdata & 0xff;
    CsOFF();
    ret_data = ((data[1]) >> 2) | ((data[0]) << 6);
    taskENABLE_INTERRUPTS();
    return ret_data;
}
