
#include "RV8263.h"


#define RV8263_ADDR (0x51)

nrf_drv_twi_t *hi2c_handler_RV8263;



ret_code_t RV8263_I2CWrite(uint16 regaddr,uint8 *buf, uint16 len)
{
ret_code_t ret=-1;
if(regaddr>0xff)
  {
  ret = twi_write(hi2c_handler_RV8263,RV8263_ADDR,regaddr,I2C_MEMADD_SIZE_16BIT,buf,len);
  }
else
  {
  ret = twi_write(hi2c_handler_RV8263,RV8263_ADDR,regaddr,I2C_MEMADD_SIZE_8BIT,buf,len);
  }
HAL_Delay(1);
return ret;
}

ret_code_t RV8263_I2CRead(uint16 regaddr,uint8 *buf, uint16 len)
{
ret_code_t ret=-1;
if(regaddr>0xff)
  {
  ret = twi_read(hi2c_handler_RV8263,RV8263_ADDR,regaddr,I2C_MEMADD_SIZE_16BIT,buf,len);
  }
else
  {
  ret = twi_read(hi2c_handler_RV8263,RV8263_ADDR,regaddr,I2C_MEMADD_SIZE_8BIT,buf,len);
  }
HAL_Delay(1);
return ret;
}

uint8 RV8263_Wbuf[7] = {0,26,19,26,3,6,24};
uint8 RV8263_Rbuf[7]={0,0,0,0,0,0,0};

int BcdToDec(uint8 BCD)
{
	uint8 ret = 0;
	if(BCD > 0x90 || BCD%0x10 > 0x09) return 0xff;			// not BCD data
	ret = (BCD / 0x10) * 10 + (BCD%0x10);
	return ret;
}
	
uint8 DecToBcd(uint8 DCB)
{
	uint8 ret = 0;
	if(DCB > 99) return 0xff;			// mac 99
	ret = (DCB/10 * 0x10) + (DCB%10);
	return ret;
}	

void ApplyCalendar(stMain *pMain)
{
	RV8263_GetTime(pMain);
	pMain->Device.Time.Calendar.tm_year = BcdToDec(RV8263_Rbuf[6]);
	pMain->Device.Time.Calendar.tm_mon = BcdToDec(RV8263_Rbuf[5]);
	pMain->Device.Time.Calendar.tm_wday = BcdToDec(RV8263_Rbuf[4]);
	pMain->Device.Time.Calendar.tm_mday = BcdToDec(RV8263_Rbuf[3]);
	pMain->Device.Time.Calendar.tm_hour = BcdToDec(RV8263_Rbuf[2]);
	pMain->Device.Time.Calendar.tm_min = BcdToDec(RV8263_Rbuf[1]);
	pMain->Device.Time.Calendar.tm_sec = BcdToDec(RV8263_Rbuf[0]);
}
	
void RV8263_GetTime(stMain *pMain)
{
RV8263_I2CRead(0x04,RV8263_Rbuf,7);
//pMain->Device.Time.Calendar.
}

void RV8263_SetTime(stMain *pMain)
{
RV8263_Wbuf[0] = DecToBcd(30);				// sec
RV8263_Wbuf[1] = DecToBcd(01);			// min
RV8263_Wbuf[2] = DecToBcd(16);			// hour
RV8263_Wbuf[3] = DecToBcd(27);			// date
RV8263_Wbuf[4] = DecToBcd(3);				// weekday
RV8263_Wbuf[5] = DecToBcd(6);				// month
RV8263_Wbuf[6] = DecToBcd(24);			// year
								
RV8263_I2CWrite(0x04,RV8263_Wbuf,7);
}


void RV8263_ClearInterrupt(void)
{
RV8263_I2CWrite(0x04,RV8263_Wbuf,7);
}

void RV8263_SetTimer(void)
{
		RV8263_Wbuf[0] = 0x80;
RV8263_I2CWrite(0x01,RV8263_Wbuf,1);
	RV8263_Wbuf[0] = 60;		// timer count 60
RV8263_I2CWrite(0x10,RV8263_Wbuf,1);
	RV8263_Wbuf[0] = 0x16;		// timer clock 1hz, timer enable, timer interrupt enable, interval mode
RV8263_I2CWrite(0x11,RV8263_Wbuf,1);
}


void RV8263_Init(stMain *pMain,nrf_drv_twi_t *i2cHandler)
{
	hi2c_handler_RV8263 = i2cHandler;
}

