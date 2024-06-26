

// -- Timer define

#include "OTQV30_Timer.h"
#include "WillogService.h"
#include "OTQV30_BLEService.h"

// app timer structure define

APP_TIMER_DEF(m_IntervalTimer);  
APP_TIMER_DEF(m_IntervalTimer_ADC);  
APP_TIMER_DEF(m_IntervalTimer_EPD);  
APP_TIMER_DEF(m_IntervalTimer_BLE);
APP_TIMER_DEF(m_IntervalTimer_RTC);

APP_TIMER_DEF(m_ButtonTimer_Push);
APP_TIMER_DEF(m_ButtonTimer_Release);
APP_TIMER_DEF(m_HiddenKeyWaitTimer);

APP_TIMER_DEF(m_LEDIndicatorTimer);

APP_TIMER_DEF(m_BLEActiveTimer);
APP_TIMER_DEF(m_BLEDelayActiveTimer);
APP_TIMER_DEF(m_BLEConnectTimer);

APP_TIMER_DEF(m_EPDReturnTimer);
APP_TIMER_DEF(m_ReturnToRecordTimer);
APP_TIMER_DEF(m_NFCWatchDogTimer);



// app timer structure define

void BLEIntervalTimer_Check(void);

uint32 RecordIntervalTimeSec;

extern stMain Main;
nrf_drv_timer_t LEDINDICATOR_TIMER_INSTANCE		= NRF_DRV_TIMER_INSTANCE(3); 
//const nrf_drv_timer_t m_timer								= NRF_DRV_TIMER_INSTANCE(1);			 // timer 1 use in SAADC
//nrf_drv_timer_t BUTTONRELEASE_TIMER_INSTANCE	= NRF_DRV_TIMER_INSTANCE(2); 
//nrf_drv_timer_t INTERVAL_TIMER_INSTANCE				= NRF_DRV_TIMER_INSTANCE(0);		// interval timer active by RTC
//nrf_drv_timer_t BUTTONPUSH_TIMER_INSTANCE			= NRF_DRV_TIMER_INSTANCE(4); 


uint64 LedIndicatorTimer_GetTick(void)
{
	return m_LEDIndicatorTimer->end_val;
}

app_timer_timeout_handler_t LedIndicatorTimer_handler(void* p_context)
{
    // 타이머 인터럽트 핸들러 함수의 내용을 여기에 작성합니다.
	if(Main.Device.RunningStatus != RunStat_Booting)			// 부팅 중일땐  작동하지 않음
		{
		Main.Key.LEDIndicatorCallFlag = 1;
		}
//LedIndicatorTimer_Start();
}


	
#if 0			// 1 = rtc instance , 0 = app timer
	void ButtonPushTimer_Init(void)
	{
	// 타이머 구성을 위한 구조체 설정
	nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
	timer_cfg.frequency = 5;					     // 500khz
	timer_cfg.mode = NRF_TIMER_MODE_TIMER; // 타이머 모드 설정
	timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32; // 타이머 비트 너비 설정. 

	// 타이머 초기화
	nrf_drv_timer_init(&BUTTONPUSH_TIMER_INSTANCE, &timer_cfg, ButtonPushTimer_handler);
	nrfx_timer_compare(&BUTTONPUSH_TIMER_INSTANCE,0,3000 * 500,true);			// 버튼 롱 푸시 타이머
	}

	void ButtonPushTimer_Start(void)
	{
	nrf_drv_timer_disable(&BUTTONPUSH_TIMER_INSTANCE);
	nrfx_timer_clear(&BUTTONPUSH_TIMER_INSTANCE);
	nrf_drv_timer_enable(&BUTTONPUSH_TIMER_INSTANCE);
	}

	void ButtonPushTimer_Stop(void)
	{
	nrf_drv_timer_disable(&BUTTONPUSH_TIMER_INSTANCE);
	nrfx_timer_clear(&BUTTONPUSH_TIMER_INSTANCE);
	}

	uint32 ButtonPushTimer_GetTick(void)
	{
	return nrfx_timer_capture(&BUTTONPUSH_TIMER_INSTANCE , 4);
	}

	nrfx_timer_event_handler_t ButtonPushTimer_handler(void* p_context)
	{
			// 타이머 인터럽트 핸들러 함수의 내용을 여기에 작성합니다.
		if(Main.Device.RunningStatus != RunStat_Booting)			// 부팅 중일땐  작동하지 않음
			{
			if(Main.Key.KeyPushFlag)
				{
				Main.Key.KeyPushFlag = 0;
				Main.Key.KeyProcessCallFlag = 1;
				}
			}
	nrfx_timer_clear(&BUTTONRELEASE_TIMER_INSTANCE);
	nrf_drv_timer_disable(&BUTTONRELEASE_TIMER_INSTANCE);

	}

#else
	void ButtonPushTimer_Init(void)
	{

	}

	void ButtonPushTimer_Start(void)
	{
		app_timer_start(m_ButtonTimer_Push, APP_TIMER_TICKS(10000), NULL);
	}

	void ButtonPushTimer_Stop(void)
	{
		app_timer_stop(m_ButtonTimer_Push);	
	}

	uint32 ButtonPushTimer_GetTick(void)
	{
	return app_timer_cnt_get();
	}

	nrfx_timer_event_handler_t ButtonPushTimer_handler(void* p_context)
	{
			// 타이머 인터럽트 핸들러 함수의 내용을 여기에 작성합니다.
		if(Main.Device.RunningStatus != RunStat_Booting)			// 부팅 중일땐  작동하지 않음
			{
			if(Main.Key.LeftKeyPushFlag || Main.Key.RightKeyPushFlag)
				{
				Main.Key.LeftKeyPushFlag = 0;
				Main.Key.RightKeyPushFlag = 0;
				Main.Key.KeyProcessCallFlag = 1;
				}
			}
	}

#endif


#if 0
	void ButtonReleaseTimer_Init(void)
	{
	// 타이머 구성을 위한 구조체 설정
	nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
	timer_cfg.frequency = 5;					     // 500khz
	timer_cfg.mode = NRF_TIMER_MODE_TIMER; // 타이머 모드 설정
	timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32; // 타이머 비트 너비 설정. 

	// 타이머 초기화
	nrf_drv_timer_init(&BUTTONRELEASE_TIMER_INSTANCE, &timer_cfg, ButtonReleaseTimer_handler);
	nrfx_timer_compare(&BUTTONRELEASE_TIMER_INSTANCE,2,300 * 500,true);				// 버튼 릴리즈 타이머, 300 미리초
	nrfx_timer_compare(&BUTTONRELEASE_TIMER_INSTANCE,1,3000 * 500,true);			// 버튼 롱 릴리즈 타이머, 3초
	}

	void ButtonReleaseTimer_Start(void)
	{
	nrf_drv_timer_disable(&BUTTONRELEASE_TIMER_INSTANCE);
	nrfx_timer_clear(&BUTTONRELEASE_TIMER_INSTANCE);
	nrf_drv_timer_enable(&BUTTONRELEASE_TIMER_INSTANCE);
	}

	void ButtonReleaseTimer_Stop(void)
	{
	nrf_drv_timer_disable(&BUTTONRELEASE_TIMER_INSTANCE);
	nrfx_timer_clear(&BUTTONRELEASE_TIMER_INSTANCE);
	}

	uint32 ButtonReleaseTimer_GetTick(void)
	{
	return nrfx_timer_capture(&BUTTONRELEASE_TIMER_INSTANCE , 0);
	}

	void ButtonReleaseTimer_handler(nrf_timer_event_t event_type, void* p_context)
	{
			// 타이머 인터럽트 핸들러 함수의 내용을 여기에 작성합니다.
		if(Main.Device.RunningStatus != RunStat_Booting)			// 부팅 중일땐  작동하지 않음
			{
			Main.Key.KeyProcessCallFlag = 1;
			}
	nrfx_timer_clear(&BUTTONRELEASE_TIMER_INSTANCE);
	nrf_drv_timer_disable(&BUTTONRELEASE_TIMER_INSTANCE);

	}
#else
	void ButtonReleaseTimer_Init(void)
	{

	}

	void ButtonReleaseTimer_Start(void)
	{
		app_timer_start(m_ButtonTimer_Release, APP_TIMER_TICKS(300), NULL);
	}

	void ButtonReleaseTimer_Stop(void)
	{
		app_timer_stop(m_ButtonTimer_Release);
	}

	uint32 ButtonReleaseTimer_GetTick(void)
	{
	return app_timer_cnt_get();
	}

	void ButtonReleaseTimer_handler(nrf_timer_event_t event_type, void* p_context)
	{
			// 타이머 인터럽트 핸들러 함수의 내용을 여기에 작성합니다.
		if(Main.Device.RunningStatus != RunStat_Booting)			// 부팅 중일땐  작동하지 않음
			{
			Main.Key.KeyProcessCallFlag = 1;
			}
	}
#endif

	void BLEActiveTimer_Init(void)
	{

	}

	void BLEActiveTimer_Start(uint32 BLEActiveTimeMs)
	{
		if(Main.Device.RunningStatus == RunStat_Logging_Connected || Main.Device.RunningStatus == RunStat_SelfTest2_Connected)
			{
			NRF_LOG_INFO("[%10d] Advertise skip. BLE connected.",getTickCount());
			}
		else
			{
			HAL_Delay(150);
			OTQV2_SystemWakeUp();
			advertising_start();
			app_timer_start(m_BLEActiveTimer, APP_TIMER_TICKS(BLEActiveTimeMs), NULL);
			NRF_LOG_INFO("[%10d] Advertise Timer start. tick : %dmS",getTickCount(),BLEActiveTimeMs);
			HAL_Delay(150);
			}
	}

	void BLEActiveTimer_Stop(void)
	{
		advertising_stop();
		app_timer_stop(m_BLEActiveTimer);
		NRF_LOG_INFO("[%10d] Advertise Timer manual stop. ",getTickCount());
	}

	uint32 BLEActiveTimer_GetTick(void)
	{
	return app_timer_cnt_get();
	}

	void BLEActiveTimer_Handler(nrf_timer_event_t event_type, void* p_context)
	{
		NRF_LOG_INFO("[%10d] Advertise Timer expired. ",getTickCount());
		advertising_stop();
		//OTQV2_SystemSleep(&Main);
	}


void BLEDelayActiveTimer_Init(void)
{

}

void BLEDelayActiveTimer_Start(uint32 BLEDelayActiveTimeMs)
{
		app_timer_start(m_BLEDelayActiveTimer, APP_TIMER_TICKS(BLEDelayActiveTimeMs), NULL);
}

void BLEDelayActiveTimer_Stop(void)
{
	app_timer_stop(m_BLEDelayActiveTimer);
	NRF_LOG_INFO("[%10d] BLE active interval timer stop.",getTickCount());
}

uint32 BLEDelayActiveTimer_GetTick(void)
{
return app_timer_cnt_get();
}

void BLEDelayActiveTimer_Handler(nrf_timer_event_t event_type, void* p_context)
{
	BLEActiveTimer_Start(BLE_Interval_PerSlot * 1000);
	NRF_LOG_INFO("[%10d] BLE active by Interval. Slot : %d",getTickCount(),Main.BLE.BLEIntervalSlot);
}


void BLEConnectTimer_Start(uint32 BLEConnectTimeMs)
{
	app_timer_start(m_BLEConnectTimer, APP_TIMER_TICKS(BLEConnectTimeMs), NULL);
}

void BLEConnectTimer_Stop(void)
{
	advertising_stop();
	app_timer_stop(m_BLEConnectTimer);
}

uint32 BLEConnectTimer_GetTick(void)
{
return app_timer_cnt_get();
}

void BLEConnectTimer_Handler(nrf_timer_event_t event_type, void* p_context)
{
	OTQV2_BLE_Disconnect();
}

uint8 EPDReturnTimer_CheckActive(void)
{
	if(m_EPDReturnTimer->end_val == 0xffffffffffffffff)
		{
		return 0;
		}
	else return 1;
}

void EPDReturnTimer_Start(uint32 EPDReturnTimeMs)
{
	ret_code_t err_code;
	err_code = app_timer_start(m_EPDReturnTimer, APP_TIMER_TICKS(EPDReturnTimeMs), NULL);
	APP_ERROR_CHECK(err_code);
}

void EPDReturnTimer_Stop(void)
{
	app_timer_stop(m_EPDReturnTimer);
}

uint32 EPDReturnTimer_GetTick(void)
{
return app_timer_cnt_get();
}

void EPDReturnTimer_Handler(nrf_timer_event_t event_type, void* p_context)
{
	// EPD return 코드 추가
	Main.Key.IntervalProcess_EPD_CallFlag = 1;
	Main.Key.IntervalProcess_EPD_ForceRefreshFlag = 1;
}
	
void IntervalTimer_UpdateInterval(stMain *pMain)
{
RecordIntervalTimeSec = pMain->Sensor.SensorIntervalSec;
}

app_timer_timeout_handler_t IntervalTimer_ADChandler(void* p_context)
{
	Main.Key.IntervalProcess_ADC_CallFlag = 1;
}

app_timer_timeout_handler_t IntervalTimer_handler(void* p_context)
{
	Main.Key.IntervalProcess_CallFlag = 1;
	app_timer_start(m_IntervalTimer_ADC, APP_TIMER_TICKS((RecordIntervalTimeSec-6) * 1000), NULL);
}

app_timer_timeout_handler_t IntervalTimer_DisplayHandler(void* p_context)
{
	Main.Key.IntervalProcess_EPD_CallFlag = 1;
}

app_timer_timeout_handler_t IntervalTimer_RTCHandler(void* p_context)
{
	nrf_cal_rtc_handler();
	//stWriteData.TimeStamp_UTC = mktime(&pMain->Device.Time.Calendar);
}



//  --------------------------- Hiddne key wait timer ---------------------------


#define HiddenKey_StepOneTimeMs 3000			// 키 입력 금지 시간
#define HiddenKey_StepTwoTimeMs 5000			// 키 입력 허용 시간 제한

	void HiddenKeyWaitTimer_Init(void)
	{
	Main.Flags.HiddenKeyActiveFlag = HiddenKey_NotActive;
	}

	void HiddenKeyWaitTimer_Start_StepOne(void)
	{
		app_timer_start(m_HiddenKeyWaitTimer, APP_TIMER_TICKS(HiddenKey_StepOneTimeMs), NULL);			// 지정 시간 이내 추가조작 발생 시 타이머 중단
		Main.Flags.HiddenKeyActiveFlag = HiddenKey_Close;
	}

	static void HiddenKeyWaitTimer_Start_StepTwo(void)
	{
		TriColorLED_SetOff();				// LED 켜진체 유지되는 현상 방지
		app_timer_start(m_HiddenKeyWaitTimer, APP_TIMER_TICKS(HiddenKey_StepTwoTimeMs), NULL);			// 지정시간 이후 입력 된 키 허용
		Main.Flags.HiddenKeyActiveFlag = HiddenKey_Open;
	}
	
	void HiddenKeyWaitTimer_Stop(void)
	{
		#if 1
			app_timer_stop(m_HiddenKeyWaitTimer);	
			Main.Flags.HiddenKeyActiveFlag = HiddenKey_NotActive;
		#else
		if(Main.Flags.HiddenKeyActiveFlag != HiddenKey_NotActive)
			{
			app_timer_stop(m_HiddenKeyWaitTimer);	
			Main.Flags.HiddenKeyActiveFlag = HiddenKey_NotActive;
			}
		#endif
	}

	uint32 HiddenKeyWaitTimer_GetTick(void)
	{
	return app_timer_cnt_get();
	}

	nrfx_timer_event_handler_t HiddenKeyWaitTimer_handler(void* p_context)
	{
			// 타이머 인터럽트 핸들러 함수의 내용을 여기에 작성합니다.
			if(Main.Flags.HiddenKeyActiveFlag == HiddenKey_Close)
				{
				HiddenKeyWaitTimer_Start_StepTwo();			// 스텝1 종료 후 자동 시작
				}
			else
				{
				Main.Flags.HiddenKeyActiveFlag = HiddenKey_NotActive;		// 스텝 2 종료 후 중단
				}
	}

//  --------------------------- Hiddne key wait timer end ------------------------



//  --------------------------- Return to record mode timer ---------------------------

uint8 ReturnToRecordTimerActiveFlag = 0;

	void ReturnToRecordTimer_Init(void)
	{

	}

	void ReturnToRecordTimer_Start(void)
	{
		app_timer_start(m_ReturnToRecordTimer, APP_TIMER_TICKS(10000), NULL);			// 지정 시간 이내 추가조작 없는 경우 기록모드 복귀 
		ReturnToRecordTimerActiveFlag = 1;
	}

	void ReturnToRecordTimer_Stop(void)
	{
		if(ReturnToRecordTimerActiveFlag)
			{
			app_timer_stop(m_ReturnToRecordTimer);	
			ReturnToRecordTimerActiveFlag = 0;
			}
	}

	uint32 ReturnToRecordTimer_GetTick(void)
	{
	return app_timer_cnt_get();
	}

	nrfx_timer_event_handler_t ReturnToRecordTimer_handler(void* p_context)
	{
			// 타이머 인터럽트 핸들러 함수의 내용을 여기에 작성합니다.
		if(Main.Device.RunningStatus == RunStat_StandBy)			// 스탠바이 모드 일 때
			{
			Main.Device.RunningStatus = RunStat_Logging;		// 기록 모드로 복귀
			OTQV2_SystemWakeUp();
			OTQV2_Service_StartLogging(&Main);
			}
	}

//  --------------------------- Return to record mode timer end ------------------------

//  --------------------------- NFC Watchdog timer timer ------------------------


	void NFCWatchDogTimer_Init(void)
	{
	Main.Flags.NFCFieldClearFlag = 0;
	}

	void NFCWatchDogTimer_Start(void)
	{
		app_timer_start(m_NFCWatchDogTimer, APP_TIMER_TICKS(100), NULL);
		Main.Flags.NFCFieldClearFlag = 0;
	}

	void NFCWatchDogTimer_Stop(void)
	{
		#if 1
			app_timer_stop(m_NFCWatchDogTimer);	
			Main.Flags.NFCFieldClearFlag = 0;
		#else
		if(Main.Flags.NFCFieldClearFlag != 0)
			{
			app_timer_stop(m_NFCWatchDogTimer);	
			Main.Flags.NFCFieldClearFlag = 0;
			}
		#endif
	}

	uint32 NFCWatchDogTimer_GetTick(void)
	{
	return app_timer_cnt_get();
	}

	nrfx_timer_event_handler_t NFCWatchDogTimer_handler(void* p_context)
	{
		Main.Flags.NFCFieldClearFlag = 1;
			// 타이머 인터럽트 핸들러 함수의 내용을 여기에 작성합니다.
			//nfc_t2t_done();			// nfc 강제 종료
			
	}

//  --------------------------- NFC Watchdog timer end ------------------------


//  --------------------------- BLE interval timer ------------------------
uint8 BLEIntervalActiveFlag = 0;
	nrfx_timer_event_handler_t BLEIntervalTimer_handler(void* p_context);
	
	void BLEIntervalTimer_Init(void)
	{
	//Main.Flags.BLEIntervalFlag = 0;
	}

	void BLEIntervalTimer_Start(void)
	{
		if(!BLEIntervalActiveFlag)
			{
			app_timer_start(m_IntervalTimer_BLE, APP_TIMER_TICKS(BLE_Interval_TimeSec * 1000), NULL);
			BLEIntervalTimer_handler(NULL);
			BLEIntervalActiveFlag = 1;
			NRF_LOG_INFO("[%10d] BLE interval start. interval :  %dSec , BLE int flag : %d",getTickCount(),BLE_Interval_TimeSec,BLEIntervalActiveFlag);
			}
		//Main.Flags.BLEIntervalFlag = 0;
	}
	
	void BLEIntervalTimer_Stop(void)
	{
		#if 1
			if(BLEIntervalActiveFlag)
				{
				BLEIntervalActiveFlag = 0;
				app_timer_stop(m_IntervalTimer_BLE);	
				NRF_LOG_INFO("[%10d] BLE interval stop. BLE int flag : %d",getTickCount(), BLEIntervalActiveFlag);
				}
			//Main.Flags.BLEIntervalFlag = 0;
		#else
		if(Main.Flags.BLEIntervalFlag != 0)
			{
			app_timer_stop(m_IntervalTimer_BLE);	
			Main.Flags.BLEIntervalFlag = 0;
			}
		#endif
	}
	
	void BLEIntervalTimer_Sleep(uint32 SleepTimeSec)
	{
		if(BLEIntervalActiveFlag)
			{
			BLEIntervalTimer_Stop();
			app_timer_start(m_IntervalTimer_BLE, APP_TIMER_TICKS(SleepTimeSec * 1000), NULL);		// sleep 시간 이후 타이머 시작
			BLEIntervalActiveFlag = 2;
			advertising_stop();
			NRF_LOG_INFO("[%10d] BLE interval sleep in %dSec, BLE int flag : %d",getTickCount(),SleepTimeSec,BLEIntervalActiveFlag);
			}
		//Main.Flags.BLEIntervalFlag = 0;
	}

	void BLEIntervalTimer_Wakeup(void)
	{
		if(BLEIntervalActiveFlag == 2)			// sleep 상태 확인
			{
			BLEDelayActiveTimer_Start(Main.BLE.BLEIntervalSlot * BLE_Interval_PerSlot * 1000);
			BLEIntervalTimer_Stop();																														// sleep 시간 타이머 중단
			BLEIntervalTimer_Start();																														// interval 타이머 작동
			BLEIntervalActiveFlag = 1;
			NRF_LOG_INFO("[%10d] BLE interval wake up. interval :  %dSec, BLE int flag : %d",getTickCount(),BLE_Interval_TimeSec,BLEIntervalActiveFlag);
			}
		//Main.Flags.BLEIntervalFlag = 0;
	}
	
	uint32 BLEIntervalTimer_GetTick(void)
	{
	return app_timer_cnt_get();
	}

	nrfx_timer_event_handler_t BLEIntervalTimer_handler(void* p_context)
	{
		if(BLEIntervalActiveFlag == 2)				// sleep 상태인 경우
			{
			BLEIntervalTimer_Stop();
			BLEIntervalTimer_Start();
			BLEIntervalActiveFlag = 1;
			}
		Main.BLE.BLEIntervalSlot = OTQV2_BLE_GetTimeSlot();
		BLEDelayActiveTimer_Start(Main.BLE.BLEIntervalSlot * BLE_Interval_PerSlot * 1000);
		NRF_LOG_INFO("[%10d] BLE interval repeat. interval :  %dSec , slot : %d , slot size : %dSec",getTickCount(),BLE_Interval_TimeSec,Main.BLE.BLEIntervalSlot,BLE_Interval_PerSlot);
	}

//  --------------------------- BLE interval timer end ------------------------


uint8 TimerActivatedFlag = 0;

void OTQV2_Timer_Start(stMain *pMain)
{
	uint32 IntervalSec = 0;
	IntervalSec = pMain->Sensor.SensorIntervalSec;
	RecordIntervalTimeSec = IntervalSec;
	
	if(!TimerActivatedFlag)
		{
		app_timer_start(m_LEDIndicatorTimer, APP_TIMER_TICKS(250), NULL);
		if(pMain->DeviceConfig.BLEActive == DeviceConfig_BLEActive_Interval)
			{
			BLEIntervalTimer_Start();
			}
		app_timer_start(m_IntervalTimer, APP_TIMER_TICKS(IntervalSec * 1000), NULL);
		app_timer_start(m_IntervalTimer_ADC, APP_TIMER_TICKS((IntervalSec-6) * 1000), NULL);
		if(pMain->Device.DisplayIntervalSec > 3600)			// do not run timer in auto
			{
			//app_timer_start(m_IntervalTimer_EPD, APP_TIMER_TICKS((pMain->Device.DisplayIntervalSec) * 1000), NULL);
			app_timer_stop(m_IntervalTimer_EPD);
			}
		else if(pMain->Device.DisplayIntervalSec == 30)		// follow record timer if set in 30 secs
			{
			app_timer_start(m_IntervalTimer_EPD, APP_TIMER_TICKS((IntervalSec) * 1000), NULL);
			}
		else if(pMain->Device.DisplayIntervalSec)		// independent timer
			{
			app_timer_start(m_IntervalTimer_EPD, APP_TIMER_TICKS((pMain->Device.DisplayIntervalSec) * 1000), NULL);
			}
		else				// follow record timer
			{
			app_timer_start(m_IntervalTimer_EPD, APP_TIMER_TICKS((IntervalSec) * 1000), NULL);
			}
		}
	TimerActivatedFlag = 1;

}

void OTQV2_Timer_RTCStart(stMain *pMain)
{
		app_timer_start(m_IntervalTimer_RTC, APP_TIMER_TICKS(1000), NULL);
}
	
void OTQV2_Timer_AllStop(stMain *pMain)
{
	if(TimerActivatedFlag)
		{
		app_timer_stop(m_LEDIndicatorTimer);
		app_timer_stop(m_IntervalTimer);
		app_timer_stop(m_IntervalTimer_ADC);
		app_timer_stop(m_IntervalTimer_EPD);
		if(pMain->DeviceConfig.BLEActive == DeviceConfig_BLEActive_Interval)
			{
			BLEIntervalTimer_Stop();
			}
		}
	TimerActivatedFlag = 0;
}



uint8 TimerInitFlag = 0;
void IntervalTimer_Init(stMain *pMain)
{
	ret_code_t err_code;
	if(!TimerInitFlag)
		{
		RecordIntervalTimeSec = pMain->Sensor.SensorIntervalSec;
		
			err_code = app_timer_create(&m_BLEActiveTimer,
															APP_TIMER_MODE_SINGLE_SHOT,
															BLEActiveTimer_Handler);
			APP_ERROR_CHECK(err_code);

			err_code = app_timer_create(&m_EPDReturnTimer,
															APP_TIMER_MODE_SINGLE_SHOT,
															EPDReturnTimer_Handler);
			APP_ERROR_CHECK(err_code);

			err_code = app_timer_create(&m_BLEConnectTimer,
															APP_TIMER_MODE_SINGLE_SHOT,
															BLEConnectTimer_Handler);
			APP_ERROR_CHECK(err_code);
			
			err_code = app_timer_create(&m_LEDIndicatorTimer,
															APP_TIMER_MODE_REPEATED,
															LedIndicatorTimer_handler);
			APP_ERROR_CHECK(err_code);

			err_code = app_timer_create(&m_IntervalTimer_ADC,
															APP_TIMER_MODE_SINGLE_SHOT,
															IntervalTimer_ADChandler);
			APP_ERROR_CHECK(err_code);

			err_code = app_timer_create(&m_IntervalTimer,
															APP_TIMER_MODE_REPEATED,
															IntervalTimer_handler);
			APP_ERROR_CHECK(err_code);

			err_code = app_timer_create(&m_IntervalTimer_EPD,
															APP_TIMER_MODE_REPEATED,
															IntervalTimer_DisplayHandler);
			APP_ERROR_CHECK(err_code);		

			err_code = app_timer_create(&m_ButtonTimer_Push,
															APP_TIMER_MODE_SINGLE_SHOT,
															ButtonPushTimer_handler);
			APP_ERROR_CHECK(err_code);		

			err_code = app_timer_create(&m_ButtonTimer_Release,
															APP_TIMER_MODE_SINGLE_SHOT,
															ButtonReleaseTimer_handler);
			APP_ERROR_CHECK(err_code);				

			err_code = app_timer_create(&m_HiddenKeyWaitTimer,
															APP_TIMER_MODE_SINGLE_SHOT,
															HiddenKeyWaitTimer_handler);
			APP_ERROR_CHECK(err_code);				

			err_code = app_timer_create(&m_ReturnToRecordTimer,
															APP_TIMER_MODE_SINGLE_SHOT,
															ReturnToRecordTimer_handler);
			APP_ERROR_CHECK(err_code);				
			
			err_code = app_timer_create(&m_NFCWatchDogTimer,
															APP_TIMER_MODE_SINGLE_SHOT,
															NFCWatchDogTimer_handler);
			APP_ERROR_CHECK(err_code);					

			err_code = app_timer_create(&m_IntervalTimer_BLE,
															APP_TIMER_MODE_REPEATED,
															BLEIntervalTimer_handler);
			APP_ERROR_CHECK(err_code);					

			err_code = app_timer_create(&m_BLEDelayActiveTimer,
															APP_TIMER_MODE_SINGLE_SHOT,
															BLEDelayActiveTimer_Handler);
			APP_ERROR_CHECK(err_code);					
			
			
		}
		TimerInitFlag = 1;
}
	
void IntervalTimer_Init_RTC(stMain *pMain)
{
ret_code_t err_code;
		err_code = app_timer_create(&m_IntervalTimer_RTC,
													APP_TIMER_MODE_REPEATED,
													IntervalTimer_RTCHandler);
		APP_ERROR_CHECK(err_code);		
}





/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
void timers_init(void)
{
	ret_code_t err_code;
    // Initialize timer module, making it use the scheduler
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

}
	