/*
 * Accelo.c
 *
 *  Created on: Sep 15, 2020
 *      Author: Administrator
 */

#include "Accelo.h"
//#define USE_DMA

nrf_drv_twi_t *hi2c_handler_accelo;
uint8 MC3479I2CAddr=0x6C;


void MC3479_SetBit(uint8 regaddr,uint8 SetBit)
{
twi_write(hi2c_handler_accelo,MC3479I2CAddr,regaddr,I2C_MEMADD_SIZE_8BIT,&SetBit,1);
HAL_Delay(1);

}

uint8 MC3479_GetBit(uint8 regaddr)
{
  uint8 ReadBuf=0;
  twi_read(hi2c_handler_accelo,MC3479I2CAddr,regaddr,I2C_MEMADD_SIZE_8BIT,&ReadBuf,1);
  HAL_Delay(1);
  return ReadBuf;
}

void MC3479_GetBitStream(uint8 regaddr,uint8 *Data,uint16 len)
{
twi_read(hi2c_handler_accelo,MC3479I2CAddr,regaddr,I2C_MEMADD_SIZE_8BIT,Data,len);
	HAL_Delay(1);
}

void MC3479_SetBitHigh(uint8 Addr,uint8 HighBit)
{
	uint8 BitBuf;
	BitBuf=MC3479_GetBit(Addr);
	BitBuf|=HighBit;
	MC3479_SetBit(Addr,BitBuf);
}

void MC3479_SetBitLow(uint8 Addr,uint8 LowBit)
{
	uint8 BitBuf;
	BitBuf=MC3479_GetBit(Addr);
	BitBuf&=~LowBit;
	MC3479_SetBit(Addr,BitBuf);
}

void MC3479_GetAccelData(Accelo *p)
{
uint8 *Array = (uint8*)p;
uint8 readtmp[6] = {0};
	/*
	uint16 readtmp=0;
	readtmp=MC3479_GetBit(0x0d);
	readtmp<<=8;
	readtmp|=MC3479_GetBit(0x0e);
	p->X=readtmp;

	readtmp=MC3479_GetBit(0x0f);
	readtmp<<=8;
	readtmp|=MC3479_GetBit(0x10);
	p->Y=readtmp;

	readtmp=MC3479_GetBit(0x11);
	readtmp<<=8;
	readtmp|=MC3479_GetBit(0x12);
	p->Z=readtmp;
	*/
	MC3479_GetBitStream(0x0e,readtmp,6);
			p->X = readtmp[0]<<8 | readtmp[1] ;
			p->Y = readtmp[2]<<8 | readtmp[3] ;
			p->Z = readtmp[4]<<8 | readtmp[5] ;
      HAL_Delay(1);
	/*
	uint8 tmp;
	tmp=MC3479_GetBit(0x13);
	if(tmp&0x04)
		{
		MC3479_SetBit(0x14,tmp);
		tmp=MC3479_GetBit(0x14);
		p->X++;
		}
		*/
}

void MC3479_SetAnyMotionThreshold(Accelo *p,uint16 Value)
{
	MC3479_SetBit(0x44,(Value>>8)&0xff);
	MC3479_SetBit(0x43,Value&0xff);
}

void MC3479_SetShakeDuration(Accelo *p,uint16 Value)
{
	MC3479_SetBit(0x49,(Value>>8)&0xff);
	MC3479_SetBit(0x48,Value&0xff);
}

uint8 Init_MC3479(stMain *pMain,nrf_drv_twi_t *i2cHandler)
{
	uint8 tmp=0;
	uint8 tmp2[2]={0,0};
	hi2c_handler_accelo=i2cHandler;

/*	if(HAL_I2C_Mem_Write(hi2c_handler_accelo,0x4C<<1,0x07,I2C_MEMADD_SIZE_8BIT,&tmp,1,1000)==HAL_OK)
	{
		MC3479I2CAddr=0x4C;
	}
	else if(HAL_I2C_Mem_Write(hi2c_handler_accelo,0x6C<<1,0x07,I2C_MEMADD_SIZE_8BIT,&tmp,1,1000)==HAL_OK)
	{
		MC3479I2CAddr=0x6C;
	}
        */
	//MC3479_SetBitLow(0x05,0x03);		// device state set to 0 (clear mode)
	MC3479_SetBit(0x07,0x00);		// device state set to 1 (wake mode)
	MC3479_SetBit(0x20,0x31);		// accelo range 16g, LPF disable
	//MC3479_SetBit(0x08,0x00);		// set ODR to 25Hz
	MC3479_SetBit(0x08,0x13);		// set ODR to 100Hz
	MC3479_SetBit(0x06,0x04);		// Shake interrupt enable
	MC3479_SetBit(0x09,0x04);		// Shake function enable
	MC3479_SetAnyMotionThreshold(&pMain->Sensor.Accelo,6000);
	//MC3479_SetShakeDuration(&pMain->Accelo,0x1010);
	MC3479_GetBitStream(0x46,tmp2,2);
	//MC3479_SetBit(0x31,0x10);		// swap interrupt
	MC3479_SetBit(0x31,0x00);		// no swap interrupt
	MC3479_SetBit(0x33,0x88);		// int 2 set push pull(for current leakage), active low. int 1 set open drain, active low
	MC3479_SetBit(0x4A,0x80);		// temporary latch enable
	//MC3479_SetBit(0x07,0x00);		// device state set to 1 (wake mode)
	tmp=MC3479_GetBit(0x14);
	tmp=MC3479_GetBit(0x09);
	//tmp=MC3479_GetBit(0x33);
	//pMain->Accelo.X=tmp;
	//MC3479_SetBit(0x07,0x00);		// device state set to 0 (sleep mode)
	MC3479_SetBit(0x07,0x01);		// device state set to 1 (wake mode)

	if(tmp == 0x04)
	{
		return 1;
	}
	else
	{
		return 0xff;
	}

}

