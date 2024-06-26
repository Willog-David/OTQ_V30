
#include "struct.h"
#include "sht45.h"

#define SHT45_ADDR (0x44)

nrf_drv_twi_t *hi2c_handler_SHT45;



ret_code_t SHT45_I2CWrite(uint16 regaddr,uint8 *buf, uint16 len)
{
ret_code_t ret=-1;
if(regaddr>0xff)
  {
  ret = twi_write(hi2c_handler_SHT45,SHT45_ADDR,regaddr,I2C_MEMADD_SIZE_16BIT,buf,len);
  }
else
  {
  ret = twi_write(hi2c_handler_SHT45,SHT45_ADDR,regaddr,I2C_MEMADD_SIZE_8BIT,buf,len);
  }
HAL_Delay(1);
return ret;
}

ret_code_t SHT45_I2CRead(uint16 regaddr,uint8 *buf, uint16 len)
{
ret_code_t ret=-1;
if(regaddr>0xff)
  {
  ret = twi_read(hi2c_handler_SHT45,SHT45_ADDR,regaddr,I2C_MEMADD_SIZE_16BIT,buf,len);
  }
else
  {
  ret = twi_read(hi2c_handler_SHT45,SHT45_ADDR,regaddr,I2C_MEMADD_SIZE_8BIT,buf,len);
  }
HAL_Delay(1);
return ret;
}

void SHT45_Init(stMain *pMain , nrf_drv_twi_t *i2cHandler)
{
   hi2c_handler_SHT45=i2cHandler;
}
 
 uint8 Rbuf[6]={0,0,0,0,0,0};
	 
void SHT45_GetTemp(stMain *pMain)
{
uint16 TempSensorFailCount=0;
uint8 Wbuf[2];
uint16 CalcBuf;
float Temp,Humi;
sint16 HumiTemp;
Wbuf[0]=0x17;
SHT45_I2CWrite(0xFD,Wbuf,0);		// wake up
HAL_Delay(15);
SHT45_I2CRead(0xFD,Rbuf,6);		// read data dump
HAL_Delay(15);
SHT45_I2CRead(0x89,Rbuf,6);		// read data dump
HAL_Delay(15);
SHT45_I2CRead(0x94,Rbuf,6);		// read data dump
Wbuf[0]=0x98;
}