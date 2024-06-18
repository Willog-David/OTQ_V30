/*
 * Display.h
 *
 *  Created on: 2021. 1. 15.
 *      Author: kaohu
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_
 
#include "struct.h"
#include "E_Ink.h"
#include "EPD_Page.h"

void Display_Temperature(stMain *pMain,uint8 *QrCode);
void Display_Humidity(stMain *pMain,uint8 *QrCode);
void Display_Serial(stMain *pMain,uint8 *QrCode);
void Display_Battery(stMain *pMain,uint8 *QrCode);
void Display_Page(stMain *pMain,uint8 *QrCode);
void Display_UpTime(stMain *pMain,uint8 *QrCode);
void EPD_DemoPage(stMain *pMain, uint8 *QrCode);
void EPD_Write_Battery(stMain *pMain,uint8 *QrCode);
void EPD_Write_Recording(stMain *pMain,uint8 *QrCode,PageMode Mode);
void EPD_Write_ProbeTemp(stMain *pMain,uint8 *QrCode,uint16 Loc_X,uint16 Loc_Y);
void EPD_Write_PCBTemp(stMain *pMain,uint8 *QrCode,uint16 Loc_X,uint16 Loc_Y);
void EPD_Write_Battery(stMain *pMain,uint8 *QrCode);
void EPD_Write_Serial(stMain *pMain,uint8 *QrCode,uint16 Loc_X,uint16 Loc_Y);
void EPD_Write_FWVer(stMain *pMain,uint8 *QrCode,uint16 Loc_X,uint16 Loc_Y);
void EPD_Write_Interval(stMain *pMain,uint8 *QrCode,uint16 Loc_X,uint16 Loc_Y);
void EPD_Write_Serial_QR(stMain *pMain,uint8 *QrCode,uint16 Loc_X,uint16 Loc_Y);
void OTQ_GetValidTempCount(stMain *pMain,stLogData *pData);
void Init_RecordStructure(stMain *pMain);

#endif