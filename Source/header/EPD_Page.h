

#ifndef EPD_PAGE_H_
#define EPD_PAGE_H_

#include "struct.h"
#include "string.h"
#include "E_Ink.h"
#include "OTQV2_Timer.h"
#include "Display_EPD_W21.h"

typedef enum
{
  PageMode_Standby = 1,
  PageMode_QRPage,
  PageMode_Recording,
  PageMode_Pause,
	PageMode_SelfTest,
	PageMode_SelfTest2,
	PageMode_CalibrationStandby,
	PageMode_Calibration,
	PageMode_CalibrationComplete,
	PageMode_CorrectionStandby,
	PageMode_Correction,
	PageMode_CorrectionComplete,
	PageMode_Configuration,
}PageMode;


void EPDPage_Service_QR(stMain *pMain);
void EPDPage_WaitMessage(stMain *pMain);
void EPDPage_SelfTest(stMain *pMain);
void EPDPage_Running(stMain *pMain);
void EPDPage_Recording(stMain *pMain);
void EPDPage_QRPage(stMain *pMain);
void EPDPage_Recording_BLE(stMain *pMain);
void EPDPage_Factory_Calibration(stMain *pMain);
void EPDPage_Factory_Calibration_Result(stMain *pMain);
void EPDPage_Service_Standby(stMain *pMain);

void EPDPage_Factory_Correction(stMain *pMain);
void EPDPage_DrawingCheckBoxArray(stMain *pMain,uint8 *DisplayBuf,uint16 X, uint16 Y);
void EPDPage_DrawingCurrentItem(stMain *pMain,uint8 *DisplayBuf, uint16 SelectedItem,uint16 X, uint16 Y);
void DeviceConfig_CursorInit(stMain *pMain, uint16 CursorInitPosition, uint16 MinPosition , uint16 MaxPosition);
void EPDPage_DeviceConfiguration(stMain *pMain);
void DisplayRefresh(stMain *pMain);

void EPDPage_BLE_DataResponse(stMain *pMain);
void EPDPage_BLE_SettingApply(stMain *pMain);
void EPDPage_BLE_DataSendResult(stMain *pMain);

void EPDPage_Factory_Calibration_Probe(stMain *pMain);
void EPDPage_Factory_Calibration_SHTC3(stMain *pMain);
void EPDPage_Factory_DeviceConfiguration(stMain *pMain);
void EPDPage_Factory_SelfTest(stMain *pMain);

#endif