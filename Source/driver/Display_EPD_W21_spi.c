#include "Display_EPD_W21_spi.h"


void EPD_GPIO_Init(void)
{
  EPD_W21_CS_1;
  EPD_W21_CLK_1;
  EPD_W21_MOSI_1;
}

void EPD_GPIO_ReadInit(void)
{
        nrf_gpio_cfg_input(3,NRF_GPIO_PIN_NOPULL);
}

void SPI_Delay(unsigned char xrate)
{
	while(xrate--);

}

unsigned char SPI_Read(void)
{
	unsigned char i;
	unsigned char ret=0;

	EPD_GPIO_ReadInit();
	HAL_Delay(1);
	for(i=0; i<8; i++)
	{
		ret<<=1;
		EPD_W21_CLK_0;
		HAL_Delay(1);
			if(isEPD_W21_Read)
			{
				ret|=0x01;
			}
			HAL_Delay(1);
		EPD_W21_CLK_1;
		HAL_Delay(1);
	}
	EPD_GPIO_Init();
	return ret;
}

void SPI_Write(unsigned char value)                                    
{                                                           
	unsigned char i;
	 SPI_Delay(1);
	for(i=0; i<8; i++)   
	{
		EPD_W21_CLK_0;
		SPI_Delay(1);
		if(value & 0x80)
			EPD_W21_MOSI_1;
		else
			EPD_W21_MOSI_0;		
		value = (value << 1);
		SPI_Delay(1);				
		EPD_W21_CLK_1;
		SPI_Delay(1);				
	}
}

void EPD_W21_WriteCMD(unsigned char command)
{
	SPI_Delay(1);
  EPD_W21_CS_0;                   
	EPD_W21_DC_0;		// command write
	SPI_Write(command);
	EPD_W21_CS_1;
}
void EPD_W21_WriteDATA(unsigned char data)
{
	SPI_Delay(1);
  EPD_W21_CS_0;                   
	EPD_W21_DC_1;		// data write
	SPI_Write(data);
	EPD_W21_CS_1;
}

uint8_t EPD_W21_ReadDATA(void)
{
	uint8_t ret=0;
	SPI_Delay(1);
	EPD_W21_CS_0;
	EPD_W21_DC_1;		// data read
	ret = SPI_Read();
	EPD_W21_CS_1;
	return ret;
}


/***********************************************************
						end file
***********************************************************/
