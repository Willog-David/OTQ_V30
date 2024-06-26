#include "OTQV30_WDT.h"
#include "nrf_drv_wdt.h"

nrf_drv_wdt_channel_id m_channel_id;

void wdt_event_handler(void)
{
	NRF_LOG_INFO("[%10d] WDT bited!!!",getTickCount());
	WDT_IRQHandlerInBootloader();
    //NOTE: The max amount of time we can spend in WDT interrupt is two cycles of 32768[Hz] clock - after that, reset occurs
}
	

// 워치독 타이머 초기화 함수
void wdt_init(void)
{
    ret_code_t err_code;
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);
		err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
}

// 워치독 타이머 초기화 함수. 동작하지 않음
void wdt_wait(void)
{
    //ret_code_t err_code;
		//NRF_WDT->RREN = 1;
		//NRF_WDT->CRV = 1000000;
}
	
// 워치독 타이머 시작 함수
void wdt_start(void)
{
    nrf_drv_wdt_enable();
}

// 워치독 타이머 정지 함수. 동작하지 않음
void wdt_stop(void)
{
    //nrf_drv_wdt_disable();
}

// 워치독 타이머 갱신 함수
void wdt_feed(void)
{
    nrf_drv_wdt_channel_feed(m_channel_id);
}