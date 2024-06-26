
#include "KeyButton.h"
#include "EEPROM.h"
#include "EPD_Page.h"
#include "OTQV30_Init.h"
#include "shtc3.h"
#include "KeyButton.h"
#include "OTQV30_Timer.h"
#include "TriColorLED.h"
#include "struct.h"
#include "WillogService.h"

const uint8 HiddenKeyArray[] = {KeyButton_Left,KeyButton_Left,KeyButton_Right,KeyButton_RightLong};


void Key_GetIntervalCursor(stMain *pMain);

uint16 LeftKeyPushTime = 0, RightKeyPushTime = 0, BothKeyPushTime = 0;

void KeyExec_ToggleOffset(stMain *pMain,KeyButton Button)
{
#if 0					// 23.05.08 calibration always enabled
if(pMain->Calib.CalibView == CalibView_OffsetApply)
	{
	pMain->Calib.CalibView = CalibView_WithOutOffset;
	}
else
	{
	pMain->Calib.CalibView = CalibView_OffsetApply;
	}
EEPROM_WriteOffsetEnable(pMain->Calib.CalibView);
SHTC3_GetData(pMain);
DisplayRefresh(pMain);

#else
pMain->Calib.CalibView = CalibView_OffsetApply;
#endif
}

void KeyExec_Factory(stMain *pMain,KeyButton Button)
{
	switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_LeftTwice:
			break;	
			
		case KeyButton_Right:
			break;

		case KeyButton_RightLong:
			break;
			
		case KeyButton_RightTwice:
			pMain->Device.RunningStatus = RunStat_SelfCalib_Probe;
			EPDPage_Factory_Calibration_Probe(pMain);
			pMain->Calib.CalibrationMode = Calibration_Standby;
			pMain->Calib.CalibrationWaitCount = 0;
			//OTQV2_Timer_AllStop(pMain);
			break;	

		case KeyButton_BothLong:
			//OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
			
		case KeyButton_RightHidden:
			//OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
			
		case KeyButton_LeftHidden:
			OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
		}
}

void KeyExec_FactoryIdle(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_LeftTwice:
			break;	
			
		case KeyButton_Right:
			OTQV2_Service_FactoryReset(pMain);
			break;

		case KeyButton_RightLong:
			break;
			
		case KeyButton_RightTwice:

			break;	

		case KeyButton_BothLong:
			
			break;
		}
}

void KeyExec_Logging_Idle(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			OTQV2_SAADC_UnInit();
			OTQV2_SAADC_Init_Probe();
			HAL_Delay(30);
			SHTC3_GetData(pMain);
			GetProbeTemp(pMain,1);
			DisplayRefresh(pMain);
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_LeftTwice:
			break;	
			
		case KeyButton_Right:
			break;

		case KeyButton_RightLong:
			OTQV2_Timer_Start(pMain);
			BLEActiveTimer_Start(30000);
			break;
			
		case KeyButton_RightTwice:
			OTQV2_SAADC_UnInit();
			OTQV2_SAADC_Init_Probe();
			HAL_Delay(30);
			pMain->Device.RunningStatus = RunStat_Logging;
			ReturnToRecordTimer_Stop();						// 레코드 복귀 타이머 중단
			OTQV2_Service_StartLogging(pMain);
			break;	
			
		case KeyButton_Both:
			Key_GetIntervalCursor(pMain);
			ReturnToRecordTimer_Stop();						// 레코드 복귀 타이머 중단
			pMain->Device.RunningStatus = RunStat_DeviceConfig;
			pMain->DeviceConfig.CursorPosition = DeviceConfig_Page_Interval;
			pMain->DeviceConfig.DeviceConfigPage = DeviceConfig_Page_Main;
			EPDPage_Factory_DeviceConfiguration(pMain);
			break;

		case KeyButton_BothLong:
			//OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
			
		case KeyButton_RightHidden:
			//OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
			
		case KeyButton_LeftHidden:
			ReturnToRecordTimer_Stop();							// 레코드 복귀 타이머 중단
			OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
		}
}

void KeyExec_Logging(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
		  pMain->Page.CurrentPage = 0;
			OTQV2_SAADC_UnInit();
			OTQV2_SAADC_Init_Probe();
			HAL_Delay(30);
			SHTC3_GetData(pMain);
			GetProbeTemp(pMain,1);
      DisplayRefresh(pMain);
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_LeftTwice:
		  pMain->Page.CurrentPage = 0;
			OTQV2_SAADC_UnInit();
			OTQV2_SAADC_Init_Probe();
			HAL_Delay(30);
			SHTC3_GetData(pMain);
			GetProbeTemp(pMain,1);
      DisplayRefresh(pMain);
			break;	
			
		case KeyButton_Right:
			if(!pMain->Page.CurrentPage)
				{
					OTQV2_SAADC_UnInit();
					OTQV2_SAADC_Init_Probe();
					HAL_Delay(30);
					SHTC3_GetData(pMain);
					GetProbeTemp(pMain,1);
				pMain->Page.CurrentPage = 1;
				DisplayRefresh(pMain);
				}
			break;

		case KeyButton_RightLong:
			BLEActiveTimer_Start(30000);
			BLEIntervalTimer_Wakeup();
			break;
			
		case KeyButton_RightTwice:
			if(!pMain->Page.CurrentPage)
				{
					OTQV2_SAADC_UnInit();
					OTQV2_SAADC_Init_Probe();
					HAL_Delay(30);
					SHTC3_GetData(pMain);
					GetProbeTemp(pMain,1);
					OTQV2_SerialQR_EnableOffset(pMain);			// serial qr에 오프셋 정보 표기
				pMain->Page.CurrentPage = 1;
				}
			else if(pMain->pLogData->FirstIndex < pMain->Sensor.DataNumber - 7200)		// 데이터 한바퀴 모두 돌았을 경우
				{
				pMain->Page.CurrentPage = 0;
				}
			else if(pMain->pLogData->FirstIndex < 2)				// 1번 데이터까지 출력 한 경우 다음 페이지 없음, 메인페이지로
				{
				pMain->Page.CurrentPage = 0;
				}
			else if(pMain->Page.CurrentPage<pMain->Page.MaxPage) 
				{
				pMain->Page.CurrentPage++;
				} // 데이터가 남았다면 다음 페이지로
			else																			// 10페이지 가 마지막 페이지
				{
				pMain->Page.CurrentPage = 0;
				}

      DisplayRefresh(pMain);
			break;	

		case KeyButton_BothLong:
			if(pMain->Page.CurrentPage >= 1)					// 23.11.28 대기모드 진입방법 변경
				{
				pMain->Flags.HiddenKeyActiveFlag = 1;
				HiddenKeyWaitTimer_Start_StepOne();
				//TriColorLED_Blink(LEDColor_White,6,250);		// 250mS * 2 * 6 = 3sec
				}
			else
				{
				#ifdef ForceDataEnable
				EPDPage_Service_ForceWrite_Begin(pMain);
				EEPROM_WriteForcelData(ForceIndexValue);
				EPDPage_Service_ForceWrite(pMain);
				pMain->Sensor.DataNumber = ForceIndexValue;
				#endif
				//EEPROM_WriteForcelData2(ForceIndexValue);
				//EEPROM_WriteForcelData3(ForceIndexValue);
				
				}
			//OTQV2_BLE_Disconnect();									// 23.11.28 대기모드 진입방법 변경
			//OTQV2_Service_CustomerReset(pMain);			// key execute 함수  hidden key 위치로 이동
			break;
		}
}

void KeyExec_SelfTest(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			pMain->Device.RunningStatus = RunStat_SelfTest2;
			//advertising_start();
			DisplayRefresh(pMain);
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_LeftTwice:
			break;	

		case KeyButton_Right:
			break;

		case KeyButton_RightLong:
			break;

		case KeyButton_RightTwice:
		#ifdef BELL_ENC
			pMain->Device.RunningStatus = RunStat_SelfCalib;
			EPDPage_Factory_Calibration_SHTC3(pMain);
			pMain->Calib.CalibrationMode = Calibration_Standby;
			pMain->Calib.CalibrationWaitCount = 0;
		#else
			pMain->Device.RunningStatus = RunStat_SelfCalib_Probe;
			EPDPage_Factory_Calibration_Probe(pMain);
			pMain->Calib.CalibrationMode = Calibration_Standby;
			pMain->Calib.CalibrationWaitCount = 0;
		#endif
			//OTQV2_Timer_AllStop(pMain);
			break;

		case KeyButton_BothLong:
			
			break;
		}
}

void KeyExec_SelfTest2(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			pMain->Device.RunningStatus = RunStat_SelfTest;
			advertising_stop();
			OTQV2_NFC_Stop();
			OTQV2_Timer_AllStop(pMain);
			DisplayRefresh(pMain);
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_LeftTwice:
			break;	

		case KeyButton_Right:
			BLEActiveTimer_Start(30000);
			break;

		case KeyButton_RightLong:
			break;

		case KeyButton_RightTwice:
			pMain->Device.RunningStatus = RunStat_SelfCalib_Probe;
			advertising_stop();
			OTQV2_NFC_Stop();
			OTQV2_Timer_AllStop(pMain);
			EPDPage_Factory_Calibration_Probe(pMain);
			break;

		case KeyButton_BothLong:
		/*																	// 23.11.20 초기모드 진입 시퀀스 변경
			OTQV2_NFC_Stop(pMain);
			advertising_stop();
			OTQV2_BLE_Disconnect();
			OTQV2_Timer_AllStop(pMain);
			OTQV2_Service_FactoryReset(pMain);
			*/
			break;

		case KeyButton_RightHidden:					// 23.11.20 초기모드 진입 시퀀스 변경
			//OTQV2_NFC_Stop(pMain);
			//advertising_stop();
			//OTQV2_BLE_Disconnect();
			//OTQV2_Timer_AllStop(pMain);
			//OTQV2_Service_FactoryReset(pMain);
			break;
			
		case KeyButton_LeftHidden:					// 23.11.20 초기모드 진입 시퀀스 변경
			OTQV2_NFC_Stop(pMain);
			advertising_stop();
			OTQV2_BLE_Disconnect();
			OTQV2_Timer_AllStop(pMain);
			OTQV2_Service_FactoryReset(pMain);
			break;
		}
}
	
void KeyExec_SelfTest_Idle(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_LeftTwice:
			break;	
			
		case KeyButton_Right:
			break;

		case KeyButton_RightLong:
			break;
			
		case KeyButton_RightTwice:
			break;	

		case KeyButton_BothLong:
			//OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
			
		case KeyButton_RightHidden:
			//OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
			
		case KeyButton_LeftHidden:
			OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;

		}
}

void KeyExec_SelfCalib(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			if(pMain->Calib.CalibrationMode == Calibration_Standby)
				{
				if(pMain->Device.RunningStatus == RunStat_SelfCalib_Probe)
					{
					pMain->Device.RunningStatus = RunStat_SelfCalib;
					}
				else if(pMain->Device.RunningStatus == RunStat_SelfCalib)
					{
					pMain->Device.RunningStatus = RunStat_SelfCalib_Probe;
					}
				}
			DisplayRefresh(pMain);
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_LeftTwice:
			break;	
			
		case KeyButton_Right:
			if(pMain->Calib.CalibrationMode == Calibration_Standby)
				{
				pMain->Calib.CalibrationMode = Calibration_InCalibration;
				DisplayRefresh(pMain);
				}
			break;

		case KeyButton_RightLong:
			break;
			
		case KeyButton_RightTwice:
			pMain->Device.RunningStatus = RunStat_Correction;
			pMain->Calib.CalibrationMode = Calibration_Standby;
			pMain->Calib.CorrectionWaitCount = 0;
			EPDPage_Factory_Correction(pMain);
			OTQV2_Timer_AllStop(pMain);
			break;	

		case KeyButton_BothLong:
			//OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
			
		case KeyButton_RightHidden:
			//OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
			
		case KeyButton_LeftHidden:
			OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;

		}
}


void KeyExec_Correction(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:

			break;

		case KeyButton_LeftTwice:
			break;	
			
		case KeyButton_Right:
		#ifdef BELL_ENC
			if(pMain->Calib.CalibrationMode == Calibration_Standby)
				{
				EEPROM_WriteDataLength(1);
				pMain->Sensor.DataNumber = 0;
				}
			if(pMain->Calib.CalibrationMode < Calibration_Complete)
				{
				pMain->Calib.CalibrationMode++;
				DisplayRefresh(pMain);
				}
		#else
			if(pMain->Calib.CalibrationMode == Calibration_Standby)
				{
				pMain->Calib.CalibrationMode++;
				}
			if(pMain->Calib.CalibrationMode < Calibration_Complete)
				{
				pMain->Calib.CorrectionWaitCount = 0;
				DisplayRefresh(pMain);
				}
		#endif
			break;

		case KeyButton_RightLong:
			BLEActiveTimer_Start(30000);
			break;
			
		case KeyButton_RightTwice:
			pMain->Device.RunningStatus = RunStat_StandBy;
			EEPROM_GetInterval(&pMain->Sensor.SensorIntervalSec);
			EEPROM_GetDisplayInterval(&pMain->Device.DisplayIntervalSec);
			//OTQV2_ApplyInterval(pMain);
			//OTQV2_ApplyDisplayInterval(pMain);
			OTQV2_Timer_AllStop(pMain);
			EPDPage_Factory_Calibration_Result(pMain);
			break;	

		case KeyButton_Both:
			break;
			
		case KeyButton_BothLong:
			//OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
			
		case KeyButton_RightHidden:
			//OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
			
		case KeyButton_LeftHidden:
			OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;

		}
}


void KeyExec_Correction_Idle(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_LeftTwice:
			break;	
			
		case KeyButton_Right:
			break;

		case KeyButton_RightLong:
			break;

		case KeyButton_RightTwice:
			pMain->Device.RunningStatus = RunStat_StandBy;
			EEPROM_GetInterval(&pMain->Sensor.SensorIntervalSec);
			EEPROM_GetDisplayInterval(&pMain->Device.DisplayIntervalSec);
			//OTQV2_ApplyInterval(pMain);
			//OTQV2_ApplyDisplayInterval(pMain);
			OTQV2_Timer_AllStop(pMain);
			EPDPage_Factory_Calibration_Result(pMain);
			break;	
		
		case KeyButton_BothLong:
			//OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
			
		case KeyButton_RightHidden:
			//OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
			
		case KeyButton_LeftHidden:
			OTQV2_Service_FactoryReset(pMain);			// 23.11.20 초기모드 진입 시퀀스 변경
			break;
		}
}

void OTQV2_ApplyInterval(stMain *pMain)
{
	switch(pMain->DeviceConfig.Interval)
		{
		case DeviceConfig_Interval_30Sec:
			pMain->Sensor.SensorIntervalSec = 30;
			break;
			
		case DeviceConfig_Interval_1Min:
			pMain->Sensor.SensorIntervalSec = 60;
			break;

		case DeviceConfig_Interval_5Min:
			pMain->Sensor.SensorIntervalSec = 300;
			break;

		case DeviceConfig_Interval_10Min:
			pMain->Sensor.SensorIntervalSec = 600;
			break;

		case DeviceConfig_Interval_15Min:
			pMain->Sensor.SensorIntervalSec = 900;
			break;

		case DeviceConfig_Interval_20Min:
			pMain->Sensor.SensorIntervalSec = 1200;
			break;

		case DeviceConfig_Interval_30Min:
			pMain->Sensor.SensorIntervalSec = 1800;
			break;

		case DeviceConfig_Interval_60Min:
			pMain->Sensor.SensorIntervalSec = 3600;
			break;
		}	
}

void OTQV2_ApplyDisplayInterval(stMain *pMain)
{
	switch(pMain->DeviceConfig.DisplayInterval)
		{
		case DeviceConfig_DisplayInterval_Auto:
			pMain->Device.DisplayIntervalSec = 0;
			break;

		case DeviceConfig_DisplayInterval_FU:
			pMain->Device.DisplayIntervalSec = 30;
			break;
			
		case DeviceConfig_DisplayInterval_1Min:
			pMain->Device.DisplayIntervalSec = 60;
			break;

		case DeviceConfig_DisplayInterval_5Min:
			pMain->Device.DisplayIntervalSec = 300;
			break;

		case DeviceConfig_DisplayInterval_10Min:
			pMain->Device.DisplayIntervalSec = 600;
			break;

		case DeviceConfig_DisplayInterval_15Min:
			pMain->Device.DisplayIntervalSec = 900;
			break;

		case DeviceConfig_DisplayInterval_20Min:
			pMain->Device.DisplayIntervalSec = 1200;
			break;

		case DeviceConfig_DisplayInterval_30Min:
			pMain->Device.DisplayIntervalSec = 1800;
			break;

		case DeviceConfig_DisplayInterval_60Min:
			pMain->Device.DisplayIntervalSec = 3600;
			break;

		case DeviceConfig_DisplayInterval_None:
			pMain->Device.DisplayIntervalSec = 0xfffffff0;
			break;
		}	
}

void Key_GetIntervalCursor(stMain *pMain)
{
		switch(pMain->Sensor.SensorIntervalSec)
		{
		case 30:
			pMain->DeviceConfig.Interval = DeviceConfig_Interval_30Sec;
			break;
			
		case 60:
			pMain->DeviceConfig.Interval = DeviceConfig_Interval_1Min;
			break;

		case 300:
			pMain->DeviceConfig.Interval = DeviceConfig_Interval_5Min;
			break;

		case 600:
			pMain->DeviceConfig.Interval = DeviceConfig_Interval_10Min;
			break;

		case 900:
			pMain->DeviceConfig.Interval = DeviceConfig_Interval_15Min;
			break;

		case 1200:
			pMain->DeviceConfig.Interval = DeviceConfig_Interval_20Min;
			break;

		case 1800:
			pMain->DeviceConfig.Interval = DeviceConfig_Interval_30Min;
			break;

		case 3600:
			pMain->DeviceConfig.Interval = DeviceConfig_Interval_60Min;
			break;
		}
}

void Key_GetDisplayIntervalCursor(stMain *pMain)
{
		switch(pMain->Device.DisplayIntervalSec)
		{
		case 0:
			pMain->DeviceConfig.DisplayInterval = DeviceConfig_DisplayInterval_Auto;
			break;
			
		case 30:
			pMain->DeviceConfig.DisplayInterval = DeviceConfig_DisplayInterval_FU;
			break;
			
		case 60:
			pMain->DeviceConfig.DisplayInterval = DeviceConfig_DisplayInterval_1Min;
			break;

		case 300:
			pMain->DeviceConfig.DisplayInterval = DeviceConfig_DisplayInterval_5Min;
			break;

		case 600:
			pMain->DeviceConfig.DisplayInterval = DeviceConfig_DisplayInterval_10Min;
			break;

		case 900:
			pMain->DeviceConfig.DisplayInterval = DeviceConfig_DisplayInterval_15Min;
			break;

		case 1200:
			pMain->DeviceConfig.DisplayInterval = DeviceConfig_DisplayInterval_20Min;
			break;

		case 1800:
			pMain->DeviceConfig.DisplayInterval = DeviceConfig_DisplayInterval_30Min;
			break;

		case 3600:
			pMain->DeviceConfig.DisplayInterval = DeviceConfig_DisplayInterval_60Min;
			break;
			
		case 0xfffffff0:
			pMain->DeviceConfig.DisplayInterval = DeviceConfig_DisplayInterval_None;
			break;	
			
		default:
			pMain->DeviceConfig.DisplayInterval = DeviceConfig_DisplayInterval_FU;
			break;
		}
}
	
void KeyExec_DeviceConfig(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			for(int i=0;i<pMain->Key.KeyPushCount;i++)
				{
				pMain->DeviceConfig.CursorPosition++;
					if(pMain->DeviceConfig.CursorPosition >= pMain->DeviceConfig.CursorMaxPosition)
						{
						pMain->DeviceConfig.CursorPosition = pMain->DeviceConfig.CursorMinPosition;
						}
				}

			EPDPage_Factory_DeviceConfiguration(pMain);
			break;

		case KeyButton_LeftLong:
			if(pMain->DeviceConfig.DeviceConfigPage == DeviceConfig_Page_EEPROMErase)
				{
				EPDPage_Factory_Reset(pMain);
				EEPROM_EraseAll();
				NVIC_SystemReset();
				}
			break;

		case KeyButton_LeftTwice:
			for(int i=0;i<pMain->Key.KeyPushCount;i++)
				{
				pMain->DeviceConfig.CursorPosition++;
					if(pMain->DeviceConfig.CursorPosition >= pMain->DeviceConfig.CursorMaxPosition)
						{
						pMain->DeviceConfig.CursorPosition = pMain->DeviceConfig.CursorMinPosition;
						}
				}

			EPDPage_Factory_DeviceConfiguration(pMain);
			break;	
			
		case KeyButton_Right:
			if(pMain->DeviceConfig.DeviceConfigPage == DeviceConfig_Page_Main)
				{
				pMain->DeviceConfig.DeviceConfigPage = pMain->DeviceConfig.CursorPosition;
				switch(pMain->DeviceConfig.DeviceConfigPage)
					{
					case DeviceConfig_Page_Interval:
						Key_GetIntervalCursor(pMain);
						pMain->DeviceConfig.CursorPosition = pMain->DeviceConfig.Interval;
						break;
				
					case DeviceConfig_Page_DisplayInterval:
						Key_GetDisplayIntervalCursor(pMain);
						pMain->DeviceConfig.CursorPosition = pMain->DeviceConfig.DisplayInterval;
						break;
						
					case DeviceConfig_Page_BLEActive:
						pMain->DeviceConfig.CursorPosition = pMain->DeviceConfig.BLEActive;
						break;

					case DeviceConfig_Page_BLEPower:
						pMain->DeviceConfig.CursorPosition = pMain->DeviceConfig.BLEPower;
						break;

					}
				DisplayRefresh(pMain);
				}
			else
				{
				switch(pMain->DeviceConfig.DeviceConfigPage)
					{
					case DeviceConfig_Page_Interval:
						pMain->DeviceConfig.Interval = pMain->DeviceConfig.CursorPosition;
						OTQV2_ApplyInterval(pMain);
						EEPROM_WriteInterval(&pMain->Sensor.SensorIntervalSec);
						EEPROM_WriteDataLength(0);
						break;
				
					case DeviceConfig_Page_DisplayInterval:
						pMain->DeviceConfig.DisplayInterval = pMain->DeviceConfig.CursorPosition;
						OTQV2_ApplyDisplayInterval(pMain);
						EEPROM_WriteDisplayInterval(&pMain->Device.DisplayIntervalSec);
						break;
						
					case DeviceConfig_Page_BLEActive:
						pMain->DeviceConfig.BLEActive = pMain->DeviceConfig.CursorPosition;
						OTQV2_BLE_ActiveModeApply();
						EEPROM_WriteBLEMode(pMain->DeviceConfig.BLEActive);
						break;	

					case DeviceConfig_Page_BLEPower:
						pMain->DeviceConfig.BLEPower = pMain->DeviceConfig.CursorPosition;
						OTQV2_BLE_TxPowerApply();
						EEPROM_WriteBLEMode(pMain->DeviceConfig.BLEPower);
						break;	
					}
				pMain->DeviceConfig.DeviceConfigPage = DeviceConfig_Page_Main;
				pMain->DeviceConfig.CursorPosition = 2;
				DisplayRefresh(pMain);
				}

			break;

		case KeyButton_RightLong:
			break;

		case KeyButton_RightTwice:
			pMain->Device.RunningStatus = RunStat_StandBy;
			EPDPage_Factory_Calibration_Result(pMain);	
			break;	
			
		case KeyButton_Both:
			//pMain->Device.RunningStatus = RunStat_StandBy;
			//EEPROM_GetInterval(&pMain->Sensor.SensorIntervalSec);
			//EEPROM_GetDisplayInterval(&pMain->Device.DisplayIntervalSec);
			pMain->DeviceConfig.DeviceConfigPage = DeviceConfig_Page_Main;
			EPDPage_Factory_DeviceConfiguration(pMain);(pMain);
			break;
	
		case KeyButton_BothLong:								// 23.11.20 초기모드 진입 시퀀스 변경
		/*
			pMain->Device.RunningStatus = RunStat_Factory;
			EEPROM_WriteRecordModeFlag(RecordMode_DeviceInFactoryMode);
			pMain->Status.RecordModeFlag = RecordMode_DeviceInFactoryMode;
			OTQV2_Timer_AllStop(pMain);
			OTQV2_NFC_Stop(pMain);
			EPDPage_Factory_SelfTest(pMain);
			*/
			break;
			
		case KeyButton_RightHidden:								// 23.11.20 초기모드 진입 시퀀스 변경
			//pMain->Device.RunningStatus = RunStat_Factory;
			//EEPROM_WriteRecordModeFlag(RecordMode_DeviceInFactoryMode);
			//pMain->Status.RecordModeFlag = RecordMode_DeviceInFactoryMode;
			//OTQV2_Timer_AllStop(pMain);
			//OTQV2_NFC_Stop(pMain);
			//EPDPage_Factory_SelfTest(pMain);
			break;	
			
		case KeyButton_LeftHidden:								// 23.11.20 초기모드 진입 시퀀스 변경
			pMain->Device.RunningStatus = RunStat_Factory;
			EEPROM_WriteRecordModeFlag(RecordMode_DeviceInFactoryMode);
			pMain->Status.RecordModeFlag = RecordMode_DeviceInFactoryMode;
			OTQV2_Timer_AllStop(pMain);
			OTQV2_NFC_Stop(pMain);
			EPDPage_Factory_SelfTest(pMain);
			break;	
		}
}
	

#define MaxHiddenKeyCount sizeof(HiddenKeyArray)	

void KeyExcute(stMain *pMain,KeyButton Button)
{
	KeyFlagClear(pMain);
	TriColorLED_SetWhite();
	/*
	if(Button == KeyButton_LeftLong)				// left long press will change offset mode
		{
		KeyExec_ToggleOffset(pMain,Button);
		}
		*/

		switch(pMain->Device.RunningStatus)
			{
			case RunStat_Factory:
				KeyExec_Factory(pMain,Button);
				break;	

			case RunStat_Idle:
				KeyExec_FactoryIdle(pMain,Button);
				break;	

			case RunStat_StandBy:
				KeyExec_Logging_Idle(pMain,Button);
				break;	

			case RunStat_Logging:
				KeyExec_Logging(pMain,Button);
				break;	

			case RunStat_SelfTest:
				KeyExec_SelfTest(pMain,Button);
				break;	

			case RunStat_SelfTest2:
				KeyExec_SelfTest2(pMain,Button);
				break;	

			
			case RunStat_SelfCalib:
			case RunStat_SelfCalib_Probe:
				KeyExec_SelfCalib(pMain,Button);
				break;	

			case RunStat_Correction:
				KeyExec_Correction(pMain,Button);
				break;	

			case RunStat_DeviceConfig:
				KeyExec_DeviceConfig(pMain,Button);
				break;	
			}

}

#define ButtonDetectCountMs 2
#define ButtonWaitDelayMs 2
#define ButtonLongPressMs 2000
uint16 NextButtonWaitDelay;
uint8 ButtonReleaseFlag = 0;
uint16 LeftPressCount = 0, RightPressCount = 0 , RightHiddenPressCount = 0, LeftHiddenPressCount = 0;
uint32 LeftPressTick = 0,LeftReleaseTick = 0, RightPressTick = 0,RightReleaseTick = 0 ;


void KeyProcess(stMain *pMain,nrf_drv_gpiote_pin_t pin)
{
static uint8 HiddenKeyPressCount = 0;
uint8 KeyExecFlag = 0;
uint32 PushCurrentTick = ButtonPushTimer_GetTick() / 25;
uint32 ReleaseCurrentTick = ButtonReleaseTimer_GetTick() / 25;

switch(pin) 
	{
		case OTQV2_GPIO_KEY_RIGHT :				// 우측 키 이벤트
			if(!nrf_gpio_pin_read(OTQV2_GPIO_KEY_RIGHT))
				{
				RightPressTick = PushCurrentTick;
				RightReleaseTick = PushCurrentTick;
				ButtonPushTimer_Start();
				ButtonReleaseTimer_Stop();
				NRF_LOG_INFO("[%10d] R - Button count : %d, %d, %d, %d, %d",getTickCount(),LeftPressCount,LeftHiddenPressCount,RightPressCount,RightHiddenPressCount,RightKeyPushTime);
				if(!LeftKeyPushTime)
					{
					TriColorLED_SetGreen();
					}
				if(!nrf_gpio_pin_read(OTQV2_GPIO_KEY_LEFT))					// 좌측 키 누른 상태에서 우측 키 동작
					{
					pMain->Key.RightHiddenKeyPushFlag = 1;
					}
				else
					{
					pMain->Key.RightKeyPushFlag = 1;
					}
				}
			else if(pMain->Key.RightKeyPushFlag)								// 일반 우측 키 동작
			//else //if(RightPressTick + ButtonDetectCountMs <= getTickCount())
				{
				RightKeyPushTime = PushCurrentTick - RightPressTick;
				RightPressCount++;
				RightReleaseTick = ReleaseCurrentTick;
				NRF_LOG_INFO("[%10d] R - Button count : %d, %d, %d, %d, %d",getTickCount(),LeftPressCount,LeftHiddenPressCount,RightPressCount,RightHiddenPressCount,RightKeyPushTime);
				if(!LeftKeyPushTime)
					{
					TriColorLED_SetOff();
					}
				ButtonPushTimer_Stop();
				ButtonReleaseTimer_Start();
				pMain->Key.RightKeyPushFlag = 0;
				if(!nrf_gpio_pin_read(OTQV2_GPIO_KEY_LEFT))
					{
					BothKeyPushTime = RightKeyPushTime > LeftKeyPushTime ? RightKeyPushTime : LeftKeyPushTime;
					}
				}
			else if(pMain->Key.RightHiddenKeyPushFlag)					// 좌측 키 누른 상태에서 우측 키 동작
				{
				RightKeyPushTime = PushCurrentTick - RightPressTick;
				RightHiddenPressCount++;
				RightReleaseTick = ReleaseCurrentTick;
				NRF_LOG_INFO("[%10d] R - Button count : %d, %d, %d, %d, %d",getTickCount(),LeftPressCount,LeftHiddenPressCount,RightPressCount,RightHiddenPressCount,RightKeyPushTime);
				if(!LeftKeyPushTime)
					{
					TriColorLED_SetOff();
					}
				ButtonPushTimer_Stop();
				ButtonReleaseTimer_Start();
				pMain->Key.RightHiddenKeyPushFlag = 0;
				if(!nrf_gpio_pin_read(OTQV2_GPIO_KEY_LEFT))
					{
					BothKeyPushTime = RightKeyPushTime > LeftKeyPushTime ? RightKeyPushTime : LeftKeyPushTime;
					}
				}
		break;
		
		case OTQV2_GPIO_KEY_LEFT :				// 좌측 키 이벤트
			if(!nrf_gpio_pin_read(OTQV2_GPIO_KEY_LEFT))        // 좌측 키 누름
				{
				LeftPressTick = PushCurrentTick;
				LeftReleaseTick = 0;
				ButtonPushTimer_Start();
				ButtonReleaseTimer_Stop();
				if(!RightKeyPushTime)
					{
					TriColorLED_SetGreen();
					}
				if(!nrf_gpio_pin_read(OTQV2_GPIO_KEY_RIGHT))		// 우측키 누른 상태에서 좌측키 누른 후 뗐을때 
					{
					pMain->Key.LeftHiddenKeyPushFlag = 1;
					}
				else
					{
					pMain->Key.LeftKeyPushFlag = 1;
					}
				NRF_LOG_INFO("[%10d] L - Button count : %d, %d, %d, %d, %d",getTickCount(),LeftPressCount,LeftHiddenPressCount,RightPressCount,RightHiddenPressCount,RightKeyPushTime);
				}
			else if(pMain->Key.LeftKeyPushFlag)								// 일반 좌측 키 동작
			//else //if(LeftPressTick + ButtonDetectCountMs <= getTickCount())
				{
				LeftKeyPushTime = PushCurrentTick - LeftPressTick;
				if(LeftKeyPushTime < ButtonDetectCountMs)LeftKeyPushTime = ButtonDetectCountMs;
				LeftPressCount++;
				//LeftKeyPushTime = ReleaseCurrentTick - LeftReleaseTick;
				LeftReleaseTick = ReleaseCurrentTick;
				NRF_LOG_INFO("[%10d] L - Button count : %d, %d, %d, %d, %d",getTickCount(),LeftPressCount,LeftHiddenPressCount,RightPressCount,RightHiddenPressCount,RightKeyPushTime);
				if(!RightKeyPushTime)
					{
					TriColorLED_SetOff();
					}
				ButtonPushTimer_Stop();
				ButtonReleaseTimer_Start();
				pMain->Key.LeftKeyPushFlag = 0;
				if(!nrf_gpio_pin_read(OTQV2_GPIO_KEY_RIGHT))
					{
					BothKeyPushTime = RightKeyPushTime > LeftKeyPushTime ? RightKeyPushTime : LeftKeyPushTime;
					}
				}
			else if (pMain->Key.LeftHiddenKeyPushFlag)				// 우측키 누른 상태에서 좌측 키 누른 후 뗐을때
				{
					{
					LeftKeyPushTime = PushCurrentTick - LeftPressTick;
					if(LeftKeyPushTime < ButtonDetectCountMs)LeftKeyPushTime = ButtonDetectCountMs;
					LeftHiddenPressCount++;
					//LeftKeyPushTime = ReleaseCurrentTick - LeftReleaseTick;
					LeftReleaseTick = ReleaseCurrentTick;
					NRF_LOG_INFO("[%10d] L - Button count : %d, %d, %d, %d, %d",getTickCount(),LeftPressCount,LeftHiddenPressCount,RightPressCount,RightHiddenPressCount,RightKeyPushTime);
					if(!RightKeyPushTime)
						{
						TriColorLED_SetOff();
						}
					ButtonPushTimer_Stop();
					ButtonReleaseTimer_Start();
					pMain->Key.LeftHiddenKeyPushFlag = 0;
					if(!nrf_gpio_pin_read(OTQV2_GPIO_KEY_RIGHT))
						{
						BothKeyPushTime = RightKeyPushTime > LeftKeyPushTime ? RightKeyPushTime : LeftKeyPushTime;
						}
					}
				}

		break;
		
		default:						// 키 뗐을때
			if(BothKeyPushTime)
				{
				LeftKeyPushTime = 0;
				RightKeyPushTime = 0;
				NRF_LOG_INFO("Both key press time : %d",BothKeyPushTime);
				}
				KeyExecFlag=1;
			break;
	}
/*
if(((RightReleaseTick > RightPressTick) && (RightReleaseTick + ButtonWaitDelayMs < getTickCount())) || ((LeftReleaseTick > LeftPressTick) && (LeftReleaseTick + ButtonWaitDelayMs < getTickCount())))
  {
  if(ButtonReleaseFlag == 1)
    {
    ButtonReleaseFlag = 0;
		NRF_LOG_INFO("Button count : %d, %d",LeftPressCount,RightPressCount);
    }
	if(NextButtonWaitDelay)
		{
		NextButtonWaitDelay--;
		}
	
  }
*/
if(pMain->Flags.HiddenKeyActiveFlag == HiddenKey_Open)			// 히든키 입력  오픈 상태일 경우에만 실행 (대기시간 이후)
	{
		if(LeftPressCount >=1 || RightPressCount >=1)			// 버튼 입력 시점 기준
		{
			switch(HiddenKeyArray[HiddenKeyPressCount])
				{
				case KeyButton_Left:
					if(LeftPressCount)
						{
						HiddenKeyPressCount++;
						LeftPressCount = 0;
						}
					else
						{
						HiddenKeyPressCount = 0;
						HiddenKeyWaitTimer_Stop();
						}
					break;
					
				case KeyButton_LeftLong:
					if(LeftKeyPushTime >= ButtonLongPressMs)
						{
						HiddenKeyPressCount++;
						}
					else
						{
						HiddenKeyPressCount = 0;
						HiddenKeyWaitTimer_Stop();
						}
					break;

				case KeyButton_Right:
					if(RightPressCount)
						{
						HiddenKeyPressCount++;
						RightPressCount = 0;
						}
					else
						{
						HiddenKeyPressCount = 0;
						HiddenKeyWaitTimer_Stop();
						}
					break;

				case KeyButton_RightLong:
					if(RightKeyPushTime >= ButtonLongPressMs)
						{
						HiddenKeyPressCount++;
						}
					else
						{
						HiddenKeyPressCount = 0;
						HiddenKeyWaitTimer_Stop();
						}
					break;
				}
			NRF_LOG_INFO("[%10d] Hidden key count : %d",getTickCount() , HiddenKeyPressCount);
			if(HiddenKeyPressCount >= MaxHiddenKeyCount)
				{
				OTQV2_SystemWakeUp();
				OTQV2_BLE_Disconnect();									// 23.11.28 대기모드 진입방법 변경
				OTQV2_Service_CustomerReset(pMain);
				ReturnToRecordTimer_Start();						// 대기모드 진입시 복귀 타이머 시작
				HiddenKeyPressCount = 0;
				}
		}
	}
else if(KeyExecFlag)
	{

		if((RightPressCount == 1) || (LeftPressCount == 1))
		{
			SHTC3_GetData(pMain);                             // 현재 온도 업데이트
			pMain->Key.KeyPushCount = 1;
			//KeyFlagClear(pMain);
			if(BothKeyPushTime >= ButtonLongPressMs)                        // 양쪽키 동시에 지정시간 이상
				{
				KeyExcute(pMain,KeyButton_BothLong);
				}


			else if(BothKeyPushTime >= ButtonDetectCountMs)                        // 양쪽키 동시에
				{
				KeyExcute(pMain,KeyButton_Both);
				NRF_LOG_INFO("Both key detection");
				}	
			else if(RightKeyPushTime >= ButtonLongPressMs && !LeftKeyPushTime)       // 우측 키 길게
				{
				KeyExcute(pMain,KeyButton_RightLong);
				NRF_LOG_INFO("Right long key detection");
				}

			else if(RightKeyPushTime >= ButtonDetectCountMs && !LeftKeyPushTime)      // 우측 키 동작
				{
				KeyExcute(pMain,KeyButton_Right);
				NRF_LOG_INFO("Right short detection");
				}

			else if(LeftKeyPushTime >= ButtonLongPressMs && !RightKeyPushTime)				// 좌측 키 길게
				{
				KeyExcute(pMain,KeyButton_LeftLong);
				NRF_LOG_INFO("Left long key detection");
				}

			else if(LeftKeyPushTime >= ButtonDetectCountMs && !RightKeyPushTime)			// 좌측 키 동작
				{
				KeyExcute(pMain,KeyButton_Left);
				NRF_LOG_INFO("Left short key detection");
				}
			else
				{
				KeyFlagClear(pMain);
				}

      // 2023.07.27 kyyun : Test
      //KeyFlagClear(pMain);
			//KeyFlagClear(pMain);

		}
		else if((RightPressCount >= 2) || (LeftPressCount >= 2) || (RightHiddenPressCount >= 3) || (LeftHiddenPressCount >= 3))
		{
			pMain->Key.KeyPushCount = RightPressCount>LeftPressCount?RightPressCount:LeftPressCount;
			SHTC3_GetData(pMain);                             // 현재 온도 업데이트
			//KeyFlagClear(pMain);
			if(BothKeyPushTime >= ButtonLongPressMs)                        // 양쪽키 동시에 지정시간 이상
				{
				KeyExcute(pMain,NULL);
			#if 0
				pMain->Device.RunningStatus = RunStat_Factory;
				EEPROM_WriteRecordModeFlag(RecordMode_DeviceInFactoryMode);
				pMain->Status.RecordModeFlag = RecordMode_DeviceInFactoryMode;
				OTQV2_Timer_AllStop(pMain);
				EPDPage_SelfTest(pMain);
				#endif	
				}
			else if(LeftHiddenPressCount >= 3)								// 우측 키 누른 상태에서 좌측 키 3회 동작
				{
				KeyExcute(pMain,KeyButton_LeftHidden);
				NRF_LOG_INFO("Left Hidden key detection : %d",LeftHiddenPressCount);
				}
			else if(RightHiddenPressCount >= 3)								// 좌측 키 누른 상태에서 우측 키 3회 동작
				{
				KeyExcute(pMain,KeyButton_RightHidden);
				NRF_LOG_INFO("Right Hidden key detection : %d",RightHiddenPressCount);
				}
			else if(RightKeyPushTime >= ButtonLongPressMs && !LeftKeyPushTime)       // 우측 키 2회 이상, 길게 동작
				{
				KeyExcute(pMain,KeyButton_RightLong);
				NRF_LOG_INFO("Right twice long key detection");
				}

			else if(RightKeyPushTime >= ButtonDetectCountMs && !LeftKeyPushTime)        // 우측 키 동작 2회 이상 동작
				{
				KeyExcute(pMain,KeyButton_RightTwice);
				NRF_LOG_INFO("Right twice short key detection");
				}

			else if(LeftKeyPushTime >= ButtonLongPressMs && !RightKeyPushTime)					// 좌측 키 2회이상, 길게 동작
				{
				KeyExcute(pMain,KeyButton_LeftLong);
				NRF_LOG_INFO("Left twice long key detection");
				}

			else if(LeftKeyPushTime >= ButtonDetectCountMs && !RightKeyPushTime)				// 좌측 키 2회이상 동작
				{
				KeyExcute(pMain,KeyButton_LeftTwice);
				NRF_LOG_INFO("Left twice short key detection");
				}
			else
				{
				KeyFlagClear(pMain);
				//KeyExcute(pMain,NULL);
				}
			
		}
		else
			{
			KeyFlagClear(pMain);									
			NRF_LOG_INFO("unknown key detection");				// 모든 조건에 맞지 않는 키 실행시
			}
	}
}

void KeyFlagClear(stMain *pMain)
{
		ButtonReleaseFlag=0;
		RightPressCount = 0;
		LeftPressCount = 0;
		LeftHiddenPressCount = 0;
		RightHiddenPressCount = 0;
		LeftKeyPushTime = 0;
		RightKeyPushTime = 0;
		BothKeyPushTime = 0;
		pMain->Key.LeftKeyPushFlag = 0;
		pMain->Key.RightKeyPushFlag = 0;
		pMain->Key.RightHiddenKeyPushFlag = 0;
}
		
extern stMain Main;
void KeyProcessHandler(nrf_drv_gpiote_pin_t pin)
{
	KeyProcess(&Main,pin);
}
