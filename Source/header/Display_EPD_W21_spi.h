#ifndef _DISPLAY_EPD_W21_SPI_
#define _DISPLAY_EPD_W21_SPI_
//#include "stm32g0xx_hal.h"
#include "struct.h"
#include "OTQV30_Init.h"


#define EPD_W21_MOSI_0	nrf_gpio_pin_write(OTQV2_GPIO_EPD_DA, 0)
#define EPD_W21_MOSI_1	nrf_gpio_pin_write(OTQV2_GPIO_EPD_DA, 1)

#define EPD_W21_CLK_0	nrf_gpio_pin_write(OTQV2_GPIO_EPD_CLK, 0)
#define EPD_W21_CLK_1	nrf_gpio_pin_write(OTQV2_GPIO_EPD_CLK, 1)

#define EPD_W21_CS_0	nrf_gpio_pin_write(OTQV2_GPIO_EPD_CS, 0)
#define EPD_W21_CS_1	nrf_gpio_pin_write(OTQV2_GPIO_EPD_CS, 1)

#define EPD_W21_DC_0	nrf_gpio_pin_write(OTQV2_GPIO_EPD_DC, 0)
#define EPD_W21_DC_1	nrf_gpio_pin_write(OTQV2_GPIO_EPD_DC, 1)

#define EPD_W21_RST_0	nrf_gpio_pin_write(OTQV2_GPIO_EPD_RST, 0)
#define EPD_W21_RST_1	nrf_gpio_pin_write(OTQV2_GPIO_EPD_RST, 1)

#define isEPD_W21_BUSY nrf_gpio_pin_read(OTQV2_GPIO_EPD_BUSY)
#define isEPD_W21_Read nrf_gpio_pin_read(OTQV2_GPIO_EPD_BUSY) // for solomen solutions


void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char data);
void EPD_W21_WriteCMD(unsigned char command);
void EPD_GPIO_Init(void);
uint8_t EPD_W21_ReadDATA(void);
#endif  //#ifndef _MCU_SPI_H_

/***********************************************************
						end file
***********************************************************/
