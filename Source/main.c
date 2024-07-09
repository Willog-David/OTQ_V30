
#include <stdint.h>
#include <string.h>

#include "OTQV30_WDT.h"
#include "OTQV30_Timer.h"
#include "OTQV30_NFC.h"
#include "BatGauge.h"
#include "EPD_Page.h"
#include "probe.h"
#include "lcd.h"
#include "RV8263.h"
#include "max31865.h"

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_sdh.h"
#include "boards.h"
#include "app_timer.h"
#include "app_button.h"

#include "nrf_gpio.h"

#include "nrf_mbr.h"
#include "nrf_bootloader.h"
#include "nrf_bootloader_app_start.h"
#include "nrf_bootloader_dfu_timers.h"
#include "nrf_dfu.h"
#include "nrf_bootloader_info.h"


#include "OTQV30_Init.h"

#include "struct.h"
#include "WillogService.h"
#include "EEPROM.h"
#include "shtc3.h"
#include "OTQV30_nrfRTC.h"
#include "Accelo.h"
#include "KeyButton.h"


QRCode QRtmp;
stMain Main;
stLogData LogData;


uint32_t timeMs = 0;

void SysTick_Handler(void)  
{
    timeMs++;
}

uint32_t getTickCount()
{
  return timeMs; 
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

uint16 LEDLoopCount=0; 

void Main_KeyHandler(stMain *pMain)
{
	OTQV2_SystemWakeUp();						// device wake up
	HAL_Delay(10);
	pMain->Device.Time.Calendar = *nrf_cal_get_time();    // get uptime
	pMain->Device.Time.UpTimeSec = mktime(&Main.Device.Time.Calendar); // get uptime
	KeyProcess(pMain,NULL);					// delayed key process
	//ButtonPushTimer_Stop();
	//ButtonReleaseTimer_Stop();
	NRF_LOG_INFO("Button IRQ : %d",getTickCount());
	pMain->Flags.BatADCValidFlag = 0;								// ADC중 키 동작시 해당 인터벌 스킵
	if(pMain->Page.BootIndex != pMain->Sensor.DataNumber)		// 최초 부팅이 아닌경우
		{
		pMain->Flags.KeyActiveFlag = 1;									// 키 동작시 해당 인터벌의 화면 갱신 금지
		}
	else
		{
		pMain->Flags.KeyActiveFlag = 0;
		}
	OTQV2_SystemSleep(pMain);				// device sleep
	
}

void Main_RecordIntervalHandler_Interval(stMain *pMain)
{
	NRF_LOG_INFO("[%10d] record interval activated",timeMs);

	OTQV2_SystemWakeUp();						// device wake up
	if(Main.Flags.BatADCValidFlag)									// ADC 활성화 중 키 동작이 없는 경우
		{
		Main.Device.BatVolt = Batt_GetVoltage_int(&Main);
		Main.Flags.BatADCValidFlag = 0;
		}
	
	Main.Device.Time.Calendar = *nrf_cal_get_time();    // get uptime
	Main.Device.Time.UpTimeSec = mktime(&Main.Device.Time.Calendar);		// get uptime
	OTQV2_GPIO_Init();
	OTQV2_SAADC_UnInit();
	OTQV2_SAADC_Init_Probe();
	HAL_Delay(300);
	SHTC3_GetData(&Main);
	GetProbeTemp(pMain,1);

	
	if((Main.Device.RunningStatus == RunStat_Logging) || (Main.Device.RunningStatus == RunStat_Correction) || (Main.Device.RunningStatus == RunStat_Logging_Connected))
		{
		if(pMain->Calib.CalData.Index)
			{
			//pMain->Sensor.Probe = pMain->Sensor.Probe.ProbeTemp;
			}
		EEPROM_WriteSerialData(pMain);
		EEPROM_GetDataLength(&pMain->Sensor.DataNumber);
		}
	UpdateBleAdvertisePacket(pMain);
	OTQV2_SystemSleep(pMain);				// device sleep
}

void Main_ADCIntervalHandler(stMain *pMain)			// early wake up handler for battery ADC
{
				// Battery ADC readings will stabilize in 5 seconds
NRF_LOG_INFO("[%10d] ADC interval activated",timeMs);
nrf_gpio_pin_write(OTQV2_GPIO_ADC_PROBE_EN, 0);					// Probe ADC FET enable
nrf_gpio_pin_write(OTQV2_GPIO_ADC_BATT_EN, 1);					// battery ADC input activated
OTQV2_SAADC_Init_Battery();															// battery ADC activated
Main.Flags.BatADCValidFlag = 1;
}

uint8 Main_DisplayAutoRefresh(stMain *pMain)
{
static sint16 oldProbeTemp = 16384;
static RefreshTempArea oldRefreshTempArea = 0;
static uint8 SubArea = 0;
RefreshTempArea Area = 0;

if(pMain->Sensor.Probe.ProbeTemp <= -4000)
	{
	Area = RefreshTempAres_MMM_M40;
	}
else if(pMain->Sensor.Probe.ProbeTemp >= -4000 && pMain->Sensor.Probe.ProbeTemp <= -2000)
	{
	Area = RefreshTempAres_M40_M20;
	}
else if(pMain->Sensor.Probe.ProbeTemp >= -2000 && pMain->Sensor.Probe.ProbeTemp <= -10)
	{
	Area = RefreshTempAres_M20_M01;
	}
else if(pMain->Sensor.Probe.ProbeTemp >= -10 && pMain->Sensor.Probe.ProbeTemp <= 200)
	{
	Area = RefreshTempAres_P00_P02;
	}
else if(pMain->Sensor.Probe.ProbeTemp >= 200 && pMain->Sensor.Probe.ProbeTemp <= 800)
	{
	Area = RefreshTempAres_P02_P08;
	}
else if(pMain->Sensor.Probe.ProbeTemp >= 800 && pMain->Sensor.Probe.ProbeTemp <= 1500)
	{
	Area = RefreshTempAres_P08_P15;
	}
else if(pMain->Sensor.Probe.ProbeTemp >= 1500 && pMain->Sensor.Probe.ProbeTemp <= 3000)
	{
	Area = RefreshTempAres_P15_P30;
	}
else if(pMain->Sensor.Probe.ProbeTemp >= 3000 && pMain->Sensor.Probe.ProbeTemp <= 4000)
	{
	Area = RefreshTempAres_P30_P40;
	}
else if(pMain->Sensor.Probe.ProbeTemp >= 4000 && pMain->Sensor.Probe.ProbeTemp <= 6000)
	{
	Area = RefreshTempAres_P40_P60;
	}
else if(pMain->Sensor.Probe.ProbeTemp >= 6000 && pMain->Sensor.Probe.ProbeTemp <= 12000)
	{
	Area = RefreshTempAres_P60_P120;
	}
else if (pMain->Sensor.Probe.ProbeTemp == 16384)
	{
	Area = RefreshTempAres_P120_PPP;
	}
else
	{
	Area = RefreshTempAres_P120_PPP;
	}

switch(Area)
	{
	case RefreshTempAres_P02_P08:				// 2 ~ 8'C
			NRF_LOG_INFO("[%10d] Probe : %d oldProbe : %d Area : %d",timeMs,pMain->Sensor.Probe.ProbeTemp,oldProbeTemp,Area);
		if(abs(abs(pMain->Sensor.Probe.ProbeTemp) - abs(oldProbeTemp)) >= 200)
			{
			oldProbeTemp = pMain->Sensor.Probe.ProbeTemp;
			oldRefreshTempArea = Area;
			return 1;
			}
		break;
	
	case RefreshTempAres_P00_P02:				// 0 ~ 2'C
			NRF_LOG_INFO("[%10d] Probe : %d oldProbe : %d Area : %d",timeMs,pMain->Sensor.Probe.ProbeTemp,oldProbeTemp,Area);
			oldProbeTemp = pMain->Sensor.Probe.ProbeTemp;
			oldRefreshTempArea = Area;
			return 1;
		break;

	case RefreshTempAres_P08_P15:				// 8 ~ 15'C
			NRF_LOG_INFO("[%10d] Probe : %d oldProbe : %d Area : %d",timeMs,pMain->Sensor.Probe.ProbeTemp,oldProbeTemp,Area);
		if(abs(abs(pMain->Sensor.Probe.ProbeTemp) - abs(oldProbeTemp)) >= 300)
			{
			oldProbeTemp = pMain->Sensor.Probe.ProbeTemp;
			oldRefreshTempArea = Area;
			return 1;
			}
		break;

	case RefreshTempAres_P15_P30:				// 15 ~ 30'C
			NRF_LOG_INFO("[%10d] Probe : %d oldProbe : %d Area : %d",timeMs,pMain->Sensor.Probe.ProbeTemp,oldProbeTemp,Area);
			#if 1				
			// 1 = 디스플레이 표시온도 기준, 0 = 절대온도 기준
			if(abs(abs(pMain->Sensor.Probe.ProbeTemp) - abs(oldProbeTemp)) >= 500)
				{
				oldProbeTemp = pMain->Sensor.Probe.ProbeTemp;
				oldRefreshTempArea = Area;
				return 1;
				}
			#else
			if(pMain->Sensor.Probe.ProbeTemp >= 2900)
				{
				if(SubArea != 1)
					{
					oldProbeTemp = pMain->Sensor.Probe.ProbeTemp;
					oldRefreshTempArea = Area;
					SubArea = 1;
					return 1;
					}
				}
			else if(pMain->Sensor.Probe.ProbeTemp >= 2600)
				{
				if(SubArea != 2)
					{
					oldProbeTemp = pMain->Sensor.Probe.ProbeTemp;
					oldRefreshTempArea = Area;
					SubArea = 2;
					return 1;
					}
				}
			else if(pMain->Sensor.Probe.ProbeTemp <= 2400)
				{
				if(SubArea != 3)
					{
					oldProbeTemp = pMain->Sensor.Probe.ProbeTemp;
					oldRefreshTempArea = Area;
					SubArea = 3;
					return 1;
					}
				}
			else if(pMain->Sensor.Probe.ProbeTemp <= 1600)
				{
				if(SubArea != 4)
					{
					oldProbeTemp = pMain->Sensor.Probe.ProbeTemp;
					oldRefreshTempArea = Area;
					SubArea = 4;
					return 1;
					}
				}
			else
				{
				if(SubArea != 5)
					{
					oldProbeTemp = pMain->Sensor.Probe.ProbeTemp;
					oldRefreshTempArea = Area;
					SubArea = 5;
					return 1;
					}
				}
			#endif
		break;

	
	default:				// refresh only once
			NRF_LOG_INFO("[%10d] Probe : %d oldProbe : %d Area : %d",timeMs,pMain->Sensor.Probe.ProbeTemp,oldProbeTemp,Area);
			oldProbeTemp = pMain->Sensor.Probe.ProbeTemp;
			//oldRefreshTempArea = Area;
			//return 1;
		break;

	case RefreshTempAres_P120_PPP:				// always refresh
			NRF_LOG_INFO("[%10d] Probe : %d oldProbe : %d Area : %d",timeMs,pMain->Sensor.Probe.ProbeTemp,oldProbeTemp,Area);
			oldProbeTemp = pMain->Sensor.Probe.ProbeTemp;
			oldRefreshTempArea = Area;
			return 1;
		break;
		
	}
if(oldRefreshTempArea != Area)
	{
	oldProbeTemp = pMain->Sensor.Probe.ProbeTemp;
	oldRefreshTempArea = Area;
	return 1;
	}
return 0;
}

void Main_DisplayIntervalHandler(stMain *pMain)
{

NRF_LOG_INFO("[%10d] EPD interval activated",timeMs);


if((pMain->Device.RunningStatus == RunStat_Logging_Connected) || (pMain->Device.RunningStatus == RunStat_SelfTest2))
	{
	return;
	}
if(!pMain->Key.IntervalProcess_EPD_ForceRefreshFlag)
	{
	if(!pMain->Device.DisplayIntervalSec)
		{
		#ifndef BELL_ENC				// 벨이엔씨 서비스에서는 오토 미사용. 오토 설정시 갱신 안함
		// auto epd refresh 코드 추가
		if(!Main_DisplayAutoRefresh(pMain))
			{
			return;
			}
		#else
		return;
		#endif
		}
	else if(pMain->Flags.KeyActiveFlag)			// 버튼 동작시 대기화면 표시기능 1회 무시
		{
		pMain->Flags.KeyActiveFlag = 0;
		return;
		}
	}
else
	{
	pMain->Key.IntervalProcess_EPD_ForceRefreshFlag = 0;
	}
if(EPDReturnTimer_CheckActive())			// QR 생성 후 복귀시간이 오기 전까진 인터벌 갱신 금지
	{
	return;
	}

NRF_LOG_INFO("[%10d] EPD interval executed",timeMs);

OTQV2_SystemWakeUp();						// device wake up sequence

Main.Page.CurrentPage = 0;

SHTC3_GetData(&Main);						// get temp & humi from on board sensor
DisplayRefresh(&Main);					// display update
OTQV2_SystemSleep(&Main);				// device sleep
}
/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */

 static uint32 CorrectCount = 0;

 
static void idle_state_handle(void)
{
static uint64 oldLEDTimeStamp = 0;

    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
				wdt_feed();
				if(Main.Key.KeyProcessCallFlag)
					{
					Main.Key.KeyProcessCallFlag = 0;
					Main_KeyHandler(&Main);

					}
				if(Main.Key.IntervalProcess_CallFlag)
					{
					Main_RecordIntervalHandler_Interval(&Main);
					OTQV2_NFC_Stop(&Main);
					OTQV2_NFC_Update(&Main);
					OTQV2_NFC_Start(&Main);
					Main.Key.IntervalProcess_CallFlag = 0;
					}
				
				if(Main.Key.LEDIndicatorCallFlag)
					{
					OTQV2_LEDIndicatorHandler(&Main);
					Main.Key.LEDIndicatorCallFlag = 0;
					}
					
				if(Main.Key.IntervalProcess_ADC_CallFlag)
					{
					Main_ADCIntervalHandler(&Main);
					Main.Key.IntervalProcess_ADC_CallFlag = 0;
					}
				
				if(Main.Key.IntervalProcess_EPD_CallFlag)
					{
					Main_DisplayIntervalHandler(&Main);
					Main.Key.IntervalProcess_EPD_CallFlag = 0;
					}	
				if(Main.Flags.BLEActiveFlag)
					{
					wdt_feed();
					Main.Flags.KeyActiveFlag = 1;
					BLEActive_Handler(&Main);
					Main.Flags.BLEActiveFlag = 0;
					}
				if(Main.Flags.BLEGetDataStatusFlag)
					{
					#ifdef DEBUG
					EPDPage_BLE_DataResponse(&Main);
					#else
					HAL_Delay(100);
					#endif
					Main.Flags.BLEGetDataStatusFlag = 0;
					}
				if(Main.Flags.NFCFieldOnFlag)
					{
						//BLEActiveTimer_Start(30000);
						NFCWatchDogTimer_Start();
						Main.Flags.NFCFieldOnFlag = 0;
					}
				if(oldLEDTimeStamp == LedIndicatorTimer_GetTick())			// 23.11.28 LED app timer 모니터링 코드
					{
					if(Main.Device.RunningStatus == RunStat_Logging)			// 기록 모드에서만 작동
						{
						CorrectCount++;
						}
					if(CorrectCount > 1000)																// 1000회 반복 시점까지 틱 증가 없을 경우 타이머 초기화
						{
						OTQV2_Timer_AllStop(&Main);
						OTQV2_Timer_Start(&Main);
						}
					}
				else
					{
					oldLEDTimeStamp = LedIndicatorTimer_GetTick();				// 앱타이머 틱 변화 감지 시 카운트 초기화
					CorrectCount = 0;
					}
				if(Main.Flags.NFCFieldClearFlag)
					{
					#if 0			// NFC 초기화 함수. 인식률 향상을 위해 계속 초기화 해야 할 경우 활성화
					OTQV2_NFC_Stop(&Main);
					OTQV2_NFC_Update(&Main);
					OTQV2_NFC_Start(&Main);
					#endif
					Main.Flags.NFCFieldClearFlag = 0;
					}
		}
}

		uint8 GPIOFlag = 0;
/**@brief Function for application main entry.
 */
 
ret_code_t err_code;

int main(void)
{
    // Initialize.
    log_init();

		//err_code = ble_dfu_buttonless_async_svci_init();
    //APP_ERROR_CHECK(err_code);

		HAL_Delay(100);				// delay for power stabilization
		wdt_init();
		wdt_start();

		OTQV2_BLE_SetMainStructure(&Main);
		OTQV2_Init(&Main);                   // OTQV2 device init
		OTQV2_Service_RestoreParameter(&Main,&LogData);


		// Start application.
		//rng_init();


		//SHTC3_GetData(&Main);
		//SHT45_GetTemp(&Main);
		
		PCA9537_Init();
		MAX31865_Init(&Main);
		MAX31865_Test(&Main);
		TWI_EEPROMTestCode();

		//RV8263_SetTime(&Main);
		RV8263_SetTimer();
		RV8263_GetTime(&Main);
		
		while(0)
		{
			wdt_feed();
			if(GPIOFlag)
				{
				PCA9537_GetPort();
				GPIOFlag = 0;
				}
					ApplyCalendar(&Main);
			//MAX31865_Test(&Main);
			HAL_Delay(1000);
		//SHT45_GetTemp(&Main);
		MC3479_GetAccelData(&Main.Sensor.Accelo);
		}
		#if 1		// OLED test code
				/* Configure the LCD Control pins -LCD ���ؿ��ƶ˿ڳ�ʼ��----------------------*/

		EPD_W21_RST_1 ;
		HAL_Delay(5);
		EPD_W21_RST_0;//Ӳ����λ
		EPD_W21_CS_1; 
		EPD_W21_CLK_1;
		EPD_W21_DC_1;
		HAL_Delay(5);
		EPD_W21_RST_1;
		HAL_Delay(15);
   
		MLCM_Initial();
//		LCDTest();
			wdt_feed();
			//photo2();
		while(0)
			{
			wdt_feed();
			photo1();
			HAL_Delay(5000);
			wdt_feed();
			photo2();
			HAL_Delay(5000);
			wdt_feed();
			photo3();
			HAL_Delay(5000);
			}
		//lrc();
		//cmcc();
			//clear_screen();
			//image_all_file();
			//Open_imageFile();
		#endif
		EPDPage_Factory_WaitMessage(&Main);				// system initialize
		HAL_Delay(3000);									// Wait for the battery voltage to stabilize
		MAX31865_Test(&Main);

		EPDPage_GetRawData(&Main);
		wdt_feed();
		Main.Device.BatVolt = Batt_GetVoltage_int(&Main);
		Main.Device.BatVolt = Batt_OffsetInSystemReset(&Main);					// Adjust the battery ADC voltage for voltage drop
		OTQV2_GPIO_Init();
		
		// RTC init
		IntervalTimer_Init_RTC(&Main);
		OTQV2_Timer_RTCStart(&Main);
		Main.Device.Time.Calendar = *nrf_cal_get_time();
		
		
		// last service mode bring up

			EEPROM_GetProbeOffsetData(&Main.Calib.CalData);
			GetProbeTemp(&Main,1);
			
			
		if(Main.Status.RecordModeFlag == RecordMode_DeviceInFactoryMode)
			{
			IntervalTimer_Init(&Main);
			OTQV2_Timer_RTCStart(&Main);
			Main.Device.RunningStatus = RunStat_Idle;
			/*
			OTQV2_Service_FactoryReset(&Main);
			*/			// move to Right key process
			}
		else
			{
			OTQV2_SerialQR_DisableOffset(&Main);							// 재부팅시 시리얼코드에 오프셋 안보이게 설정
			EEPROM_GetDataLength(&Main.Page.FirstIndex);				// 첫 부팅시 이전에 기록되어있던 데이터 갯수 표시 (디버그용)
			Main.Page.BootIndex = Main.Page.FirstIndex + 1;			// 첫 레코드를 고려하여 값 1 증가
			OTQV2_Service_StartLogging(&Main);
			}
		if(Main.DeviceConfig.BLEActive == DeviceConfig_BLEActive_Always)
			{
			advertising_start(&Main);
			UpdateBleAdvertisePacket(&Main);
			}

			OTQV2_SystemSleep(&Main);															// prepare to sleep mode
    while(1)
    {
    idle_state_handle();
    }    
}


/**
 * @}
 */
