/*
 * EEPROM_Internal.c
 *
 *  Created on: Jul 19, 2020
 *      Author: Administrator
 */

#include "EEPROM_Internal.h"

#define EEPROM_BASE_ADDR	0x0800FF00
#define EEPROM_BASE_ADDRPAGE	0x08000FF00


void EEPROM_Internal_Write(uint32 address,uint32 data)
{
	uint8 ret;
}

void EEPROM_Internal_WriteBeforeThread(uint32 address,uint32 data)
{
	uint8 ret;
}

void EEPROM_Internal_Read(uint32 address,uint32 *data)
{
	uint32 *p;
	p=(uint32*)address;
	if(*p!=0xffffffff)*data=*p;
}

void EEPROM_Internal_EraseAll(stMain *pMain)
{

}

void EEPROM_Internal_EraseAllBeforeThread(void)
{

}

void EEPROM_WriteAll(stMain *pMain)
{
	uint32 *p;
	uint8 tmp=1;
	p=(uint32*)pMain;

	if(tmp==1)EEPROM_Internal_EraseAll(pMain);
	//FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
	for(int i=0;i<(sizeof(stMain))/4;i++)
	{
		EEPROM_Internal_Write(i*4+EEPROM_BASE_ADDR,*p);
		p++;
	}
//	vTaskDelete( NULL );
}

void EEPROM_WriteAllBeforeThread(uint8 *pdata,uint32 len)
{
	uint32 *p;
	uint8 tmp=1;
	p=(uint32*)pdata;
	//HAL_FLASH_Unlock();
	if(len<4)len=4;

	if(tmp==1)EEPROM_Internal_EraseAllBeforeThread();
	//FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
	for(int i=0;i<(len)/4;i++)
	{
		EEPROM_Internal_WriteBeforeThread(i*4+EEPROM_BASE_ADDR,*p);
		p++;
	}
	//HAL_FLASH_Lock();     // edit here
//	vTaskDelete( NULL );
}

void EEPROM_ReadAll(uint8 *pdata,uint32 len)
{
	uint32 *p;
	p=(uint32*)pdata;
	if(len<4)len=4;
	for(int i=0;i<(len)/4;i++)
	{
		EEPROM_Internal_Read(i*4+EEPROM_BASE_ADDR,p);
		p++;
	}

//	vTaskDelete( NULL );
}
