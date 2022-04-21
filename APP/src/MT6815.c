/***************************************************
 * Description  : MT6815������ú����ļ�
 * LastEditTime : 2022-04-15 16:28:19
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\src\MT6815.c
 ***************************************************/
#include "MT6815.h"
#include "MotorTask.h"

/***************************************************
 * ����: spi_flash_init
 * ����: SPI��ʼ������
 * ����: void
 * ����: void
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
    spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;         // ȫ˫��ģʽ
    spi_init_struct.device_mode = SPI_MASTER;                      // ����
    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;               // 8λ
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE; // ��λ�ͼ������ã�SCK����Ϊ�͵�ƽ��������SCK�������ر�����
    spi_init_struct.nss = SPI_NSS_SOFT;                            // NSS���Ʒ�ʽ
    spi_init_struct.prescale = SPI_PSC_8;                          // Ԥ��Ƶϵ��
    spi_init_struct.endian = SPI_ENDIAN_MSB;                       // ���
    spi_init(SPI0, &spi_init_struct);
    spi_enable(SPI0); //ʹ��SPI0
}

/***************************************************
 * ����: SPI_ReadByte
 * ����: SPI���պ���
 * ����: void
 * ����: uint8_t
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
 * ����: SPI_I2sDataTansmit
 * ����:
 * ����: spi_periph-SPIx;data-����
 * ����: void
 ***************************************************/
void SPI_I2sDataTansmit(uint32_t spi_periph, uint16_t data)
{
    SPI_DATA(spi_periph) = (uint32_t)data;
}

/***************************************************
 * ����: SPI_I2sDataReceive
 * ����: SPI receive data
 * ����: spi_periph: SPIx(x=0,1,2)
 * ����: uint16_t
 ***************************************************/
uint16_t SPI_I2sDataReceive(uint32_t spi_periph)
{
    return ((uint16_t)SPI_DATA(spi_periph));
}
/***************************************************
 * ����: SpiFlashSendByte
 * ����: SPI���ͺ���
 * ����: byte Ҫ���͵��ֽ�
 * ����: uint8_t
 ***************************************************/
uint8_t SPI_SendByte(uint8_t byte)
{
    while (RESET == SPI_I2sFlagGet(SPI0, SPI_FLAG_TBE))
        ;                           //�ȴ��Ĵ������
    SPI_I2sDataTansmit(SPI0, byte); //��������
    while (RESET == SPI_I2sFlagGet(SPI0, SPI_FLAG_RBNE))
        ;                              //�ȴ��ش�
    return (SPI_I2sDataReceive(SPI0)); //���ؽ�������
}

/***************************************************
 * ����: SPI_I2sFlagGet
 * ����: SPI��־λ��ȡ
 * ����: void
 * ����: void
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
 * ����: MT6815_READ_NEW_DATA
 * ����: ��ʼһ���µĶ�
 * ����: void
 * ����: uint16_t
 ***************************************************/
uint16_t MT6815_READ_NEW_DATA(void)
{
    uint16_t data[2];
    uint16_t tempdata;
    uint16_t ret_data;
    taskDISABLE_INTERRUPTS();
    ClearDR(); //��DR�Ĵ���
    CsON();
    tempdata = (0x1 << 7) | (0x3);
    // Global_hspi2.Instance->DR = tempdata; //дDR�Ĵ���
    // while(__HAL_SPI_GET_FLAG(&Global_hspi2,SPI_FLAG_RXNE) == pdFALSE);
    ClearDR(); //��DR�Ĵ���
    tempdata = 0;
    // Global_hspi2.Instance->DR = tempdata;
    // while(__HAL_SPI_GET_FLAG(&Global_hspi2,SPI_FLAG_RXNE)== pdFALSE);
    // tempdata = Global_hspi2.Instance->DR; //��DR�Ĵ���
    data[0] = tempdata & 0xff;
    tempdata = 0;
    // Global_hspi2.Instance->DR = tempdata; //дDR�Ĵ���
    //  while(__HAL_SPI_GET_FLAG(&Global_hspi2,SPI_FLAG_RXNE) == pdFALSE);
    //  tempdata =  Global_hspi2.Instance->DR;//��DR�Ĵ���
    data[1] = tempdata & 0xff;
    CsOFF();
    ret_data = ((data[1]) >> 2) | ((data[0]) << 6);
    taskENABLE_INTERRUPTS();
    return ret_data;
}
