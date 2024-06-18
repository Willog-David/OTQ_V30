

// -- Timer define

#include "OTQV2_Timer.h"


extern stMain Main;
nrf_drv_timer_t BUTTONPUSH_TIMER_INSTANCE = NRF_DRV_TIMER_INSTANCE(4); 
nrf_drv_timer_t BUTTONRELEASE_TIMER_INSTANCE = NRF_DRV_TIMER_INSTANCE(2); 
nrf_drv_timer_t INTERVAL_TIMER_INSTANCE = NRF_DRV_TIMER_INSTANCE(3); 

void ButtonPushTimer_handler(nrf_timer_event_t event_type, void* p_context)
{
    // 타이머 인터럽트 핸들러 함수의 내용을 여기에 작성합니다.
		HAL_Delay(10);
		OTQV2_GPIO_Init();
		//Init_TWI();
		HAL_Delay(10);
		KeyProcess(&Main,NULL);
		nrfx_timer_clear(&BUTTONPUSH_TIMER_INSTANCE);
		nrf_drv_timer_disable(&BUTTONPUSH_TIMER_INSTANCE);
		NRF_LOG_INFO("Button IRQ : %d",getTickCount());
		OTQV2_SystemSleep(&Main);
}
	

void ButtonPushTimer_Init(void)
{
// 타이머 구성을 위한 구조체 설정
nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
timer_cfg.frequency = 5;					     // 500khz
timer_cfg.mode = NRF_TIMER_MODE_TIMER; // 타이머 모드 설정
timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32; // 타이머 비트 너비 설정. 

// 타이머 초기화
nrf_drv_timer_init(&BUTTONPUSH_TIMER_INSTANCE, &timer_cfg, ButtonPushTimer_handler);
nrfx_timer_compare(&BUTTONPUSH_TIMER_INSTANCE,0,3000 * 500,true);
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

void ButtonReleaseTimer_handler(nrf_timer_event_t event_type, void* p_context)
{
    // 타이머 인터럽트 핸들러 함수의 내용을 여기에 작성합니다.
		HAL_Delay(10);
		OTQV2_GPIO_Init();
		//Init_TWI();
		HAL_Delay(10);
		KeyProcess(&Main,NULL);
		nrfx_timer_clear(&BUTTONRELEASE_TIMER_INSTANCE);
		nrf_drv_timer_disable(&BUTTONRELEASE_TIMER_INSTANCE);
		NRF_LOG_INFO("Button IRQ : %d",getTickCount());
		OTQV2_SystemSleep(&Main);
}
	

void ButtonReleaseTimer_Init(void)
{
// 타이머 구성을 위한 구조체 설정
nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
timer_cfg.frequency = 5;					     // 500khz
timer_cfg.mode = NRF_TIMER_MODE_TIMER; // 타이머 모드 설정
timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32; // 타이머 비트 너비 설정. 

// 타이머 초기화
nrf_drv_timer_init(&BUTTONRELEASE_TIMER_INSTANCE, &timer_cfg, ButtonReleaseTimer_handler);
nrfx_timer_compare(&BUTTONRELEASE_TIMER_INSTANCE,2,300 * 500,true);
nrfx_timer_compare(&BUTTONRELEASE_TIMER_INSTANCE,1,3000 * 500,true);
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



void IntervalTimer_handler(void)
{
	//IntervalTimer_Start();    
	Main.Device.Time.Calendar = *nrf_cal_get_time();    // 시간 get
	Main.Device.Time.UpTimeSec = mktime(&Main.Device.Time.Calendar);
	OTQV2_GPIO_Init();
	HAL_Delay(10);
	SHTC3_GetData(&Main);
	EEPROM_WriteSerialData(&Main);
	EEPROM_GetDataLength(&Main.Sensor.DataNumber);
	//HAL_Delay(100);
	DisplayRefresh(&Main);
	OTQV2_SystemSleep(&Main);
}

void IntervalTimer_Init_RTC(void)
{
	nrf_cal_set_callback(IntervalTimer_handler,60);
}

void IntervalTimer_Init(void)
{
// 타이머 구성을 위한 구조체 설정
nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
timer_cfg.frequency = 5;					     // 500khz
timer_cfg.mode = NRF_TIMER_MODE_TIMER; // 타이머 모드 설정
timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32; // 타이머 비트 너비 설정. 

// 타이머 초기화
nrf_drv_timer_init(&INTERVAL_TIMER_INSTANCE, &timer_cfg, IntervalTimer_handler);
nrfx_timer_compare(&INTERVAL_TIMER_INSTANCE,0,Main.Sensor.SensorIntervalSec * 1000 * 500,true);		// CC 0번, main 인터벌 만큼 타이머 설정

}

void IntervalTimer_Start(void)
{
nrf_drv_timer_disable(&INTERVAL_TIMER_INSTANCE);
nrfx_timer_clear(&INTERVAL_TIMER_INSTANCE);
nrf_drv_timer_enable(&INTERVAL_TIMER_INSTANCE);
}

void IntervalTimer_Stop(void)
{
nrf_drv_timer_disable(&INTERVAL_TIMER_INSTANCE);
nrfx_timer_clear(&INTERVAL_TIMER_INSTANCE);
}

uint32 IntervalTimer_GetTick(void)
{
return nrfx_timer_capture(&INTERVAL_TIMER_INSTANCE , 0);
}