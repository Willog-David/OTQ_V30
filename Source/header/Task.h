/*
 * Task.h
 *
 *  Created on: 2021. 5. 20.
 *      Author: kaohu
 */

#ifndef SRC_DRIVER_TASK_H_
#define SRC_DRIVER_TASK_H_

#include "EEPROM.h"
#include "Display.h"
#include "qrcode.h"
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "struct.h"
#include "BatGauge.h"
#include "accelo.h"
#include "types.h"
#include "sequence.h"
#include "probe.h"

#if(QRViewMode==0)
	#define QR_Center			// QRv13 중앙�로 �동
	#define DefineQRVersion 13
#elif(QRViewMode==1)
	#define QR_Center			// QRv13 중앙�로 �동
	#define DefineQRVersion 12
#elif(QRViewMode==2)
	#define DualQR			// QR코드 �중
	#define QR_Center			// QRv13 중앙�로 �동
	#define DefineQRVersion 13
#elif(QRViewMode==3)
	#define DefineQRVersion 16
#elif(QRViewMode==4)
	#define DualQR			// QR코드 �중
	#define DefineQRVersion 13
#endif



//#define URL_LOGIS
//#define URL_MONITOR
extern const unsigned char stVersionString[];
extern const unsigned char stSubVersionString[];

uint8 QRLogger_HextoAscii(uint8 hex);
uint32 GetIntervalFromFlash(void);
void GetSerialNumberFromUID(stMain *pMain);
uint8 QRLogger_ViewTemp(stMain *pMain);
void QRLogger_GetTime(stMain *pMain);
uint8 GetTempSensor(stMain *pMain);
void RtcGetTimeSec(uint32 *currenttimesec);
uint8 QRLogger_TaskProcess(stMain *pMain);
void QRLogger_Init(stMain *pMain);
void QRLogger_DisplayURL(stMain *pMain, uint8 *Huffman,uint16 length);
void GetSerialNumberProcess(stMain *pMain);
#endif /* SRC_DRIVER_TASK_H_ */
