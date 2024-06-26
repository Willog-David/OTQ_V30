
#include "struct.h"
#include "string.h"
#include "WillogService.h"
#include "Display_EPD_W21.h"
#include "E_Ink.h"
#include "Display.h"
#include "nrf_pwr_mgmt.h"
#include "TriColorLED.h"
#include "nrf_calendar.h"
#include "OTQV30_Init.h"
#include "OTQV30_Timer.h"
#include "EEPROM.h"


const unsigned char WillogLogo[]=
{
0xFF, 0xFF, 0xF8, 0x01,0xFF, 0xFF, 0xE0, 0x00,
0xFF, 0xFF, 0x80, 0x00,0x3F, 0xFF, 0xE0, 0x00,
0xFF, 0xFE, 0x00, 0x00,0x0F, 0xFF, 0xE0, 0x00,
0xFF, 0xF8, 0x00, 0x00,0x03, 0xFF, 0xE0, 0x00,
0xFF, 0xE0, 0x00, 0x00,0x00, 0xFF, 0xE0, 0x00,
0xFF, 0xC0, 0x00, 0x00,0x00, 0x7F, 0xE0, 0x00,
0xFF, 0x80, 0x00, 0x00,0x00, 0x3F, 0xE0, 0x00,
0xFF, 0x00, 0x00, 0x00,0x00, 0x1F, 0xE0, 0x00,
0xFE, 0x00, 0x00, 0x00,0x00, 0x0F, 0xE0, 0x00,
0xFC, 0x00, 0x00, 0x00,0x00, 0x07, 0xE0, 0x00,
0xF8, 0x00, 0x00, 0x00,0x00, 0x03, 0xE0, 0x00,
0xF0, 0x00, 0x00, 0x00,0x00, 0x01, 0xE0, 0x00,
0xF0, 0x00, 0x00, 0x00,0x00, 0x01, 0xE0, 0x00,
0xE0, 0x00, 0x04, 0x00,0x20, 0x00, 0xE0, 0x00,
0xE0, 0x00, 0x07, 0x80,0x78, 0x00, 0xE0, 0x00,
0xC0, 0x00, 0x0F, 0x80,0x7C, 0x00, 0x60, 0x00,
0xC0, 0x00, 0x0F, 0xC0,0xFC, 0x00, 0x60, 0x00,
0x80, 0x00, 0x1F, 0xC0,0xFE, 0x00, 0x20, 0x00,
0x80, 0x00, 0x1F, 0xC0,0xFE, 0x00, 0x20, 0x00,
0x80, 0x00, 0x3F, 0xE1,0xFE, 0x00, 0x20, 0x00,
0x00, 0x00, 0x3D, 0xE1,0xEF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x3C, 0xF1,0xCF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x78, 0xF1,0xC7, 0x80, 0x00, 0x00,
0x00, 0x00, 0x78, 0xF9,0x87, 0x80, 0x00, 0x00,
0x00, 0x00, 0xF0, 0x78,0x87, 0xC0, 0x00, 0x00,
0x00, 0x00, 0xF0, 0x78,0x83, 0xC0, 0x00, 0x00,
0x00, 0x00, 0xF0, 0x3C,0x03, 0xC0, 0x00, 0x00,
0x00, 0x01, 0xE0, 0x3C,0x01, 0xE0, 0x00, 0x00,
0x00, 0x01, 0xE0, 0x1E,0x01, 0xE0, 0x00, 0x00,
0x00, 0x03, 0xC0, 0x1E,0x00, 0xF0, 0x00, 0x00,
0x00, 0x03, 0xC0, 0x1E,0x00, 0xF0, 0x20, 0x00,
0x80, 0x07, 0x80, 0x0F,0x00, 0xF0, 0x20, 0x00,
0x80, 0x07, 0x80, 0x0F,0x00, 0x78, 0x20, 0x00,
0x80, 0x07, 0x80, 0x07,0x80, 0x78, 0x20, 0x00,
0xC0, 0x0F, 0x00, 0x07,0x80, 0x3C, 0x60, 0x00,
0xC0, 0x0F, 0x00, 0x03,0xC0, 0x3C, 0x60, 0x00,
0xE0, 0x00, 0x00, 0x00,0x00, 0x1E, 0xE0, 0x00,
0xE0, 0x00, 0x00, 0x00,0x00, 0x1F, 0xE0, 0x00,
0xF0, 0x00, 0x00, 0x00,0x00, 0x1F, 0xE0, 0x00,
0xF0, 0x00, 0x00, 0x00,0x00, 0x0F, 0xE0, 0x00,
0xF8, 0x00, 0x00, 0x00,0x00, 0x0F, 0xE0, 0x00,
0xFC, 0x00, 0x00, 0x00,0x00, 0x07, 0xE0, 0x00,
0xFE, 0x00, 0x00, 0x00,0x00, 0x0F, 0xE0, 0x00,
0xFF, 0x00, 0x00, 0x00,0x00, 0x1F, 0xE0, 0x00,
0xFF, 0x80, 0x00, 0x00,0x00, 0x3F, 0xE0, 0x00,
0xFF, 0xC0, 0x00, 0x00,0x00, 0x7F, 0xE0, 0x00,
0xFF, 0xE0, 0x00, 0x00,0x00, 0xFF, 0xE0, 0x00,
0xFF, 0xF8, 0x00, 0x00,0x03, 0xFF, 0xE0, 0x00,
0xFF, 0xFE, 0x00, 0x00,0x0F, 0xFF, 0xE0, 0x00,
0xFF, 0xFF, 0x80, 0x00,0x3F, 0xFF, 0xE0, 0x00,
0xFF, 0xFF, 0xF0, 0x01,0xFF, 0xFF, 0xE0, 0x00,

};


void OTQ_EnterBootLoader(void)
{
  sd_power_gpregret_clr(0,0xffffffff);
  sd_power_gpregret_set(0,0xb1);
  nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_DFU);
}


void OTQ_LED_TestLoop(stMain *pMain)
{
static uint8 LEDCount=0;

switch(LEDCount)
  {
  case 0:
    TriColorLED_SetRed();
    LEDCount=1;
    break;

  case 1:
    TriColorLED_SetGreen();
    LEDCount=2;
    break;

  case 2:
    TriColorLED_SetBlue();
    LEDCount=3;
    break;

  case 3:
    TriColorLED_SetPurple();
    LEDCount=4;
    break;

  case 4:
    TriColorLED_SetYellow();
    LEDCount=5;
    break;

  case 5:
    TriColorLED_SetCyan();
    LEDCount=6;
    break;

  case 6:
    TriColorLED_SetWhite();
    LEDCount=0;
    break;

  default:
    LEDCount=0;
    break;
  }

}

void OTQV2_LEDIndicatorHandler(stMain *pMain)			// loop every 250mS
{
static uint32 IntervalCount=0;
static uint8 LEDTestLoopCount = 3;

if(!nrf_gpio_pin_read(OTQV2_GPIO_KEY_LEFT) || !nrf_gpio_pin_read(OTQV2_GPIO_KEY_RIGHT))		// LED loop no active in key press state
	{
	}
else if (pMain->Device.BatLevel <= 1)								// low battery alert
	{
	if((IntervalCount % 4) == 0)
		{
		TriColorLED_SetRed();
		}
	else if((IntervalCount % 4) == 2)
		{
		TriColorLED_SetOff();
		}
	}	
else if(0)						// system error
	{
	}
else if(pMain->Flags.HiddenKeyActiveFlag == HiddenKey_Close)
	{
	if((IntervalCount % 2) == 0)
		{
		TriColorLED_SetWhite();
		}
	else if((IntervalCount % 2) == 1)
		{
		TriColorLED_SetOff();
		}
	}
else if(pMain->Device.RunningStatus == RunStat_Logging_Connected)						// ble connection
	{
		TriColorLED_SetGreen();
	}
else if((pMain->Device.RunningStatus == RunStat_SelfTest2) && (LEDTestLoopCount >= 1))		// Selftest 2 모드  �이�스�중
	{
	if(LEDTestLoopCount)
		{
		if((IntervalCount % 20) == 0)
			{
			TriColorLED_SetRed();
			}
		else if((IntervalCount % 20) == 5)
			{
			TriColorLED_SetGreen();
			}
		else if((IntervalCount % 20) == 10)
			{
			TriColorLED_SetBlue();
			}		
		else if((IntervalCount % 20) == 15)
			{
			TriColorLED_SetOff();
			LEDTestLoopCount--;
			}
		else
			{
			}
		}
	}
else if(pMain->BLE.AdvFlag)																									// ble advertising
	{
	if((IntervalCount % 4) == 0)
		{
		TriColorLED_SetBlue();
		}
	else if((IntervalCount % 4) == 1)
		{
		TriColorLED_SetOff();
		}
	}
else if(pMain->Device.RunningStatus == RunStat_Logging)									// logging 모드
	{
	if(pMain->Sensor.PCBTemp < 0)				// 0미만 구간 ���이�동
		{
			if((IntervalCount % 20) == 0)
				{
				TriColorLED_SetYellow();
				}
			else if((IntervalCount % 20) == 1)
				{
				TriColorLED_SetOff();
				}
		}
	else if((pMain->Sensor.Probe.ProbeTemp < 800) && (pMain->Sensor.Probe.ProbeTemp > 0))  // �로��2~8�도 ���이�동
		{
			if((IntervalCount % 20) == 0)
				{
				TriColorLED_SetBlue();
				}
			else if((IntervalCount % 20) == 1)
				{
				TriColorLED_SetOff();
				}
		}
	else
		{
			if((IntervalCount % 20) == 0)				// �외구간 �색 �이�동
				{
				TriColorLED_SetGreen();
				}
			else if((IntervalCount % 20) == 1)
				{
				TriColorLED_SetOff();
				}
		}
	}
else if(pMain->Device.RunningStatus == RunStat_SelfCalib_Probe)
	{
	if((IntervalCount % 20) == 0)
		{
		TriColorLED_SetYellow();
		}
	else if((IntervalCount % 20) == 1)
		{
		TriColorLED_SetOff();
		}
	}
else
	{
	//TriColorLED_SetOff();
	}
if(pMain->Device.RunningStatus != RunStat_SelfTest2)
	{
	LEDTestLoopCount = 3;
	}
IntervalCount++;
}

void calendar_updated()
{
}
/*
void OTQV2_InitCalandar(void)
{
    nrf_cal_init();
    nrf_cal_set_callback(calendar_updated, 4);
}
*/
void OTQV2_SetCalendar(void)
{
//nrf_cal_set_time(year, month, day, hour, minute, second);
nrf_cal_set_time(2022, 3, 16, 18, 21, 10);
}

void OTQV2_GetTempData(stMain *pMain,stLogData *pLogData)
{

}

void OTQV2_Service_StartLogging(stMain *pMain)
{
		pMain->Device.Time.RecordBeginTime = pMain->Device.Time.UpTimeSec;			// serial QR information include cal. info
		if(!pMain->Sensor.SensorIntervalSec) pMain->Sensor.SensorIntervalSec = 60;
		EEPROM_GetDataLength(&pMain->Page.FirstIndex);
		pMain->Page.BootIndex = pMain->Page.FirstIndex;
		pMain->Device.RunningStatus = RunStat_Logging;
		EPDPage_Service_Standby(pMain);
		EEPROM_WriteRecordModeFlag(RecordMode_DeviceInRecordMode);
		pMain->Status.RecordModeFlag = RecordMode_DeviceInRecordMode;
		
		IntervalTimer_Init(pMain);
		OTQV2_Timer_Start(pMain);
		OTQV2_NFC_Start(pMain);
		if(pMain->Calib.CalData.Index)
			{
			if(!pMain->Sensor.PCBTemp)
				{
				pMain->Sensor.PCBTemp = pMain->Sensor.Probe.ProbeTemp;
				}
			EEPROM_WriteSerialData(pMain);
			}
		else
			{
			EEPROM_WriteSerialData(pMain);
			}

		EEPROM_GetDataLength(&pMain->Sensor.DataNumber);
		OTQV2_NFC_Stop(pMain);
		OTQV2_NFC_Update(pMain);
		OTQV2_NFC_Start(pMain);
		//advertising_start();
}
	
void OTQV2_Service_FactoryReset(stMain *pMain)
{
	pMain->Device.RunningStatus = RunStat_Factory;
	pMain->Page.CurrentPage = 0;
	EEPROM_WriteRecordModeFlag(RecordMode_DeviceInFactoryMode);
	pMain->Status.RecordModeFlag = RecordMode_DeviceInFactoryMode;
	pMain->Calib.CalibrationMode = Calibration_Standby;
	IntervalTimer_Init(pMain);
	OTQV2_Timer_AllStop(pMain);
	EPDPage_Factory_SelfTest(pMain);
	pMain->Device.RunningStatus = RunStat_SelfTest;
	advertising_stop();
	
}

void OTQV2_Service_CustomerReset(stMain *pMain)
{
	pMain->Device.RunningStatus = RunStat_StandBy;
	pMain->Page.CurrentPage = 0;
	EEPROM_WriteRecordModeFlag(RecordMode_DeviceInFactoryMode);
	pMain->Status.RecordModeFlag = RecordMode_DeviceInFactoryMode;
	OTQV2_Timer_AllStop(pMain);
	EPDPage_Factory_Calibration_Result(pMain);
	OTQV2_SerialQR_EnableOffset(pMain);
	advertising_stop();
}

void OTQV2_Service_RestoreParameter(stMain *pMain,stLogData *pLogData)
{
pMain->pLogData = pLogData;
EEPROM_GetOffsetData(&pMain->Calib.CalData);
//EEPROM_GetOffsetEnable(&pMain->Calib.CalibView);
pMain->Calib.CalibView = CalibView_OffsetApply;				// 23.05.08 calibration always enabled
EEPROM_GetInterval(&pMain->Sensor.SensorIntervalSec);
EEPROM_GetDisplayInterval(&pMain->Device.DisplayIntervalSec);
EEPROM_GetRecordModeFlag(&pMain->Status.RecordModeFlag);
}

void OTQV2_Service_SerialNumberBackup(stMain *pMain)
{
	uint8 SerialNumberTemp[10];
	memset(SerialNumberTemp,0x00,10);
	int64_t *pSerialUICRData = (uint32_t *) 0x10001080;
	sint64 SerialUICRDataTemp = *pSerialUICRData;
	
	if(SerialUICRDataTemp != -1)
		{
		EEPROM_WriteSerialNumber(&SerialUICRDataTemp);
		}
	else
		{
		EEPROM_GetSerialNumber(&SerialUICRDataTemp);
		if(SerialUICRDataTemp != -1)
			{
			memcpy(pMain->DevInfo.Serial,SerialNumberTemp,10);
			pMain->BLE.Serial = (SerialNumberTemp[0] - 0x30)*10000000 + 
													(SerialNumberTemp[1] - 0x30)*1000000 + 
													(SerialNumberTemp[2] - 0x30)*100000 +
													(SerialNumberTemp[3] - 0x30)*10000 + 
													(SerialNumberTemp[4] - 0x30)*1000 + 
													(SerialNumberTemp[5] - 0x30)*100 + 
													(SerialNumberTemp[6] - 0x30)*10 + 
													(SerialNumberTemp[7] - 0x30)*1 ;
													
			}
		}
}