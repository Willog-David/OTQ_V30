/*
 * EEPROM_Internal.h
 *
 *  Created on: Jul 19, 2020
 *      Author: Administrator
 */

#ifndef EEPROM_INTERNAL_H_
#define EEPROM_INTERNAL_H_

#include "struct.h"

//void EEPROM_Internal_WriteTimeRecord(stMain* pMain);
void EEPROM_WriteAllBeforeThread(uint8 *pdata,uint32 len);
void EEPROM_ReadAll(uint8 *pdata,uint32 len);
void EEPROM_Internal_WriteBeforeThread(uint32 address,uint32 data);
#endif /* EEPROM_INTERNAL_H_ */
