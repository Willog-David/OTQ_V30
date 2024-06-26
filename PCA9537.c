
#include "PCA9537.h"

#define PCA9537_ADDR (0x49)

nrf_drv_twi_t *hi2c_handler_PCA9537;

uint8 PCA9537_Port;


ret_code_t PCA9537_I2CWrite(uint16 regaddr,uint8 *buf, uint16 len)
{
ret_code_t ret=-1;
if(regaddr>0xff)
  {
  ret = twi_write(hi2c_handler_PCA9537,PCA9537_ADDR,regaddr,I2C_MEMADD_SIZE_16BIT,buf,len);
  }
else
  {
  ret = twi_write(hi2c_handler_PCA9537,PCA9537_ADDR,regaddr,I2C_MEMADD_SIZE_8BIT,buf,len);
  }
HAL_Delay(1);
return ret;
}

ret_code_t PCA9537_I2CRead(uint16 regaddr,uint8 *buf, uint16 len)
{
ret_code_t ret=-1;
if(regaddr>0xff)
  {
  ret = twi_read(hi2c_handler_PCA9537,PCA9537_ADDR,regaddr,I2C_MEMADD_SIZE_16BIT,buf,len);
  }
else
  {
  ret = twi_read(hi2c_handler_PCA9537,PCA9537_ADDR,regaddr,I2C_MEMADD_SIZE_8BIT,buf,len);
  }
HAL_Delay(1);
return ret;
}

void PCA9537_ReadPort(void)
{
	PCA9537_I2CRead(0x00,&PCA9537_Port,1);
	PCA9537_Port &= 0x0f;			// only use lower 4bit in PCA9537
}

uint8 PCA9537_GetPort(void)
{
	PCA9537_ReadPort();
	return PCA9537_Port;
}

void PCA9537_SetPort(uint8 SetPort)
{
	PCA9537_I2CWrite(0x01,&SetPort,1);
}

void PCA9537_SetPortHigh(uint8 HighPort)
{
	PCA9537_Port |= HighPort & 0x0F;
	PCA9537_I2CWrite(0x01,&PCA9537_Port,1);
}

void PCA9537_SetPortLow(uint8 LowPort)
{
	PCA9537_Port &= ~(LowPort & 0x0F);
	PCA9537_I2CWrite(0x01,&PCA9537_Port,1);
}

void PCA9537_SetPort_RTDPWR_HIGH(void)
{
	PCA9537_SetPortHigh(0x08);
}

void PCA9537_SetPort_RTDPWR_LOW(void)
{
	PCA9537_SetPortLow(0x08);
}

void PCA9537_SetPort_RTDCS_HIGH(void)
{
	PCA9537_SetPortHigh(0x04);
}

void PCA9537_SetPort_RTDCS_LOW(void)
{
	PCA9537_SetPortLow(0x04);
}

void PCA9537_SetHandler(nrf_drv_twi_t *pHandler)
{
	hi2c_handler_PCA9537 = pHandler;
}

void PCA9537_Init(void)
{
	uint8 Wbuf[1] = {0x03};
		uint8 Rbuf[1] = {0};
	PCA9537_I2CWrite(0x03,Wbuf,1);			// bit 3 : RTD_PWR_EN
																			// bit 2 : RTD_CS_O
																			// bit 1 : reserved
																			// bit 0 : RTD_DRDY_O
	PCA9537_ReadPort();
}
