/*
 * EEPROM.h
 *
 *  Created on: Nov 19, 2020
 *      Author: kaohu
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include "struct.h"
#include "types.h"
#include "EEPROM_Internal.h"

typedef struct
{
  uint16 index;
  uint8 Value;
}EEPROM_Context;

void EEPROM_WriteData(uint8* buf, uint16 StartAddr,uint16 len);
void EEPROM_ReadData(uint8 *buf, uint16 StartAddr, uint16 len);
void EEPROM_Init(stMain *pMain, nrf_drv_twi_t *i2cHandler);
void Init_EEPROM(stMain *pMain, nrf_drv_twi_t *i2cHandler);
void EEPROM_WriteTempCorrection(uint16 Data);
void EEPROM_GetTempCorrection(uint16* Data);
void EEPROM_ClearData(void);
void EEPROM_GetInterval(uint32 *Data);
void EEPROM_WriteInterval(uint32 *Data);
void EEPROM_GetRunningStat(uint8 *Data);
void EEPROM_WriteRunningStat(uint8 Data);
void EEPROM_GetNumberOfRecord(uint16 *Data);
void EEPROM_WriteNumberOfRecord(uint16 *Data);
void EEPROM_WriteCurrentPage(uint16 *Data);
void EEPROM_GetCurrentPage(uint16 *Data);
void EEPROM_WriteIntervalCount(uint32 *Data);
void EEPROM_GetIntervalCount(uint32 *Data);
void EEPROM_GetSerialNumber(uint8 *Data);
void EEPROM_WriteSerialNumber(uint8 *Data);
void EEPROM_WritePrevTemp(uint16 Data);
void EEPROM_GetPrevTemp(uint16 *Data);
void EEPROM_WritePrevHumi(uint8 Data);
void EEPROM_GetPrevHumi(uint8 *Data);
void EEPROM_WriteKeyFlag(uint8 Data);
void EEPROM_GetPrevKeyFlag(uint8 *Data);
void EEPROM_WriteDataLength(uint16 Data);
void EEPROM_GetDataLength(uint16 *Data);
uint16 EEPROM_GetSerialData(stMain *pMain,sint8 *Data);
uint16 EEPROM_GetSerialDataRef(stMain *pMain,sint8 *Data,EEPROM_RawDatastructure *RefData);
#ifdef Recording_H
void EEPROM_WriteSerialData(uint16 Data,uint8 humi);
#elif defined Recording_A
void EEPROM_WriteSerialData(stMain *pMain);
#else
void EEPROM_WriteSerialData(stMain *pMain);
#endif
void EEPROM_WriteOffset(sint16 Data);
void EEPROM_GetOffset(sint16 *Data);
void EEPROM_WriteCalibDataNumber(uint8 Data);
void EEPROM_GetCalibDataNumber(uint8 *Data);
void EEPROM_WriteTempDataForCalib(sint16 Data);
void EEPROM_GetTempDataForCalib(sint16 *Data);
void EEPROM_WriteProbeCorrectCount(uint8 Data);
void EEPROM_GetProbeCorrectCount(uint8 *Data);
void EEPROM_WriteProbeCorrectFailCount(uint8 Data);
void EEPROM_GetProbeCorrectFailCount(uint8 *Data);
void EEPROM_WriteSubCount(uint8 Data);
void EEPROM_GetSubCount(uint8 *Data);
void EEPROM_AddEPDRefresh_QR(void);
uint8 TWI_EEPROMTestCode(void);
void DisplayRefresh(stMain *pMain);

void EEPROM_GetRawDataForBle(stMain *pMain);
void EEPROM_GetRawData(stMain *pMain,EEPROM_RawDatastructure *Data);

void EEPROM_GetLastDataNumber(uint16 *Data);
void EEPROM_WriteLastDataNumber(uint16 Data);
void EEPROM_WriteOffsetData(CalibrationData stData);
void EEPROM_GetOffsetData(CalibrationData *stData);
void EEPROM_GetOffsetEnable(uint8 *Data);
void EEPROM_WriteOffsetEnable(uint8 Data);
void EEPROM_WriteOffsetEnable(uint8 Data);
void EEPROM_GetRecordModeFlag(uint8 *Data);
void EEPROM_WriteRecordModeFlag(uint8 Data);
void EEPROM_GetDisplayInterval(uint32 *Data);
void EEPROM_WriteDisplayInterval(uint32 *Data);
void EEPROM_WriteProbeOffsetData(CalibrationData stData);
void EEPROM_GetProbeOffsetData(CalibrationData *stData);
void EEPROM_WriteBLEMode(uint8 Data);
void EEPROM_GetBLEMode(uint8 *Data);


void EEPROM_WriteForcelData(uint16 IndexValue);
#endif