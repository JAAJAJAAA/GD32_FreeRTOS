/***************************************************
 * Description  : MT6815相关配置函数头文件
 * LastEditTime : 2022-04-15 16:28:36
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\include\MT6815.h
 ***************************************************/
#ifndef _MT6815_H_
#define _MT6815_H_

#include "UserConfig.h"
#include "gd32f30x.h"
//片选信号宏定义
#define SPI_FLASH_CS_LOW() gpio_bit_reset(GPIOE, GPIO_PIN_3)
#define SPI_FLASH_CS_HIGH() gpio_bit_set(GPIOE, GPIO_PIN_3)
#define SPI0_NSS PEout(3)
#define WRITE 0x02 /* write to memory instruction */
#define WRSR 0x01  /* write status register instruction */
#define WREN 0x06  /* write enable instruction */

#define READ 0x03 /* read from memory instruction */
#define RDSR 0x05 /* read status register instruction  */
#define RDID 0x9F /* read identification */
#define SE 0x20   /* sector erase instruction */
#define BE 0xC7   /* bulk erase instruction */

#define WIP_FLAG 0x01 /* write in progress(wip)flag */
#define DUMMY_BYTE 0xA5

uint8_t SPI_SendByte(uint8_t byte);
FlagStatus SPI_I2sFlagGet(uint32_t spi_periph, uint32_t flag);
void SPI_Init(void);
uint8_t SPI_ReadByte(void);
void SPI_I2sDataTansmit(uint32_t spi_periph, uint16_t data);
uint16_t SPI_I2sDataReceive(uint32_t spi_periph);
uint16_t MT6815_READ_NEW_DATA(void);

#endif
