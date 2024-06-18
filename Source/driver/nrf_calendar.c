/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
 
#include "nrf_calendar.h"
#include "nrf.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_timer.h"

static struct tm time_struct, m_tm_return_time; 
static time_t m_time, m_last_calibrate_time = 0;
static float m_calibrate_factor = 0.0f;
static uint32_t m_rtc_increment = 60;
static void (*cal_event_callback[4])(void) = {0,0,0,0};
//static void (*cal_event_callback_CC1)(void) = 0;
//static void (*cal_event_callback_CC2)(void) = 0;
 
void nrf_cal_init(void)
{
    // Select the 32 kHz crystal and start the 32 kHz clock
    NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos;
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    while(NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
    
    // Configure the RTC for 1 minute wakeup (default)
    CAL_RTC->PRESCALER = 0xFFF;
    CAL_RTC->EVTENSET = RTC_EVTENSET_COMPARE0_Msk;
    CAL_RTC->INTENSET = RTC_INTENSET_COMPARE0_Msk;
		CAL_RTC->EVTENSET = RTC_EVTENSET_COMPARE1_Msk;
    CAL_RTC->INTENSET = RTC_INTENSET_COMPARE1_Msk;
    CAL_RTC->CC[0] = m_rtc_increment * 8;
    CAL_RTC->TASKS_START = 1;
    NVIC_SetPriority(CAL_RTC_IRQn, CAL_RTC_IRQ_Priority);
    NVIC_EnableIRQ(CAL_RTC_IRQn);  
}

#if 0						// 0 = 23.05.02 backup
void nrf_cal_set_callback(void (*callback)(void),void (*callback_CC1)(void),void (*callback_CC2)(void),uint32_t interval)
{
    // Set the calendar callback, and set the callback interval in seconds
    cal_event_callback = callback;
		cal_event_callback_CC1 = callback_CC1;
		cal_event_callback_CC2 = callback_CC2;
    m_rtc_increment = interval;
    m_time += CAL_RTC->COUNTER /  1024;
    CAL_RTC->TASKS_CLEAR = 1;
    CAL_RTC->CC[0] = interval *  1024;  
		CAL_RTC->CC[1] = (interval-5) *  1024;  // adc enable timer
}
#else

void nrf_cal_set_callback(void (*callback)(void),uint8_t CCNumber,uint32_t interval)
{
    // Set the calendar callback, and set the callback interval in seconds
    cal_event_callback[CCNumber] = callback;
		if(!CCNumber)
			{
			m_rtc_increment = interval;
			}
    
    m_time += CAL_RTC->COUNTER /  1024;
    CAL_RTC->TASKS_CLEAR = 1;
		if(interval)
			{
			CAL_RTC->CC[CCNumber] = interval *  1024;  
			app_timer_setCC(interval *  1024 , CCNumber);
			}
		else								// minimum interval, 250mS
			{
			CAL_RTC->CC[CCNumber] = 256;  
			app_timer_setCC(256 , CCNumber);
			}
    
}
#endif

void nrf_cal_rtc_handler(void)
{
	m_time++;
}
	
void nrf_cal_clear_callback(void)
{
	for(int i=0;i<4;i++)
		{
		cal_event_callback[i] = NULL;
		}
	
}
 
void nrf_cal_set_time(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second)
{
    static time_t uncal_difftime, difftime, newtime;
    time_struct.tm_year = year - 1900;
    time_struct.tm_mon = month;
    time_struct.tm_mday = day;
    time_struct.tm_hour = hour;
    time_struct.tm_min = minute;
    time_struct.tm_sec = second;   
    newtime = mktime(&time_struct);
    CAL_RTC->TASKS_CLEAR = 1;  
    
    // Calculate the calibration offset 
    if(m_last_calibrate_time != 0)
    {
        difftime = newtime - m_last_calibrate_time;
        uncal_difftime = m_time - m_last_calibrate_time;
        m_calibrate_factor = (float)difftime / (float)uncal_difftime;
    }
    
    // Assign the new time to the local time variables
    m_time = m_last_calibrate_time = newtime;
}    

struct tm *nrf_cal_get_time(void)
{
static time_t oldTimeStamp=0;
    time_t return_time;

    //return_time = m_time + CAL_RTC->COUNTER / 32768;
    return_time = m_time ;
    m_tm_return_time = *localtime(&return_time);
    return &m_tm_return_time;
}

struct tm *nrf_cal_get_time_calibrated(void)
{
    time_t uncalibrated_time, calibrated_time;
    if(m_calibrate_factor != 0.0f)
    {
        uncalibrated_time = m_time + CAL_RTC->COUNTER / 8;
        calibrated_time = m_last_calibrate_time + (time_t)((float)(uncalibrated_time - m_last_calibrate_time) * m_calibrate_factor + 0.5f);
        m_tm_return_time = *localtime(&calibrated_time);
        return &m_tm_return_time;
    }
    else return nrf_cal_get_time();
}

char *nrf_cal_get_time_string(bool calibrated)
{
    static char cal_string[80];
    strftime(cal_string, 80, "%x - %H:%M:%S", (calibrated ? nrf_cal_get_time_calibrated() : nrf_cal_get_time()));
    return cal_string;
}
 /*
void CAL_RTC_IRQHandler(void)
{
	static uint32_t TimeTick250mS[4] = {0,0,0,0};
	static uint8_t TimerProcessFlag[4] = {0,0,0,0};
	
    if((TimeTick250mS[0] == (CAL_RTC->CC[0] / 256)) && !TimerProcessFlag[0])
    {
        CAL_RTC->EVENTS_COMPARE[0] = 0;
        
        CAL_RTC->TASKS_CLEAR = 1;
        
        m_time += TimeTick250mS[0] / 4;
				TimeTick250mS[0] = 0;
				TimerProcessFlag[0] = 1;
				
        if(cal_event_callback[0]) cal_event_callback[0]();
				//NRF_LOG_INFO("RTC compare 0 interrupt detected");
    }
    else if((TimeTick250mS[1] == (CAL_RTC->CC[1] / 256)) && !TimerProcessFlag[1])
    {
        CAL_RTC->EVENTS_COMPARE[1] = 0;
        TimerProcessFlag[1]=1;
        //CAL_RTC->TASKS_CLEAR = 1;
        
				if(cal_event_callback[1]) cal_event_callback[1]();

				//NRF_LOG_INFO("RTC compare 1 interrupt detected");
    }
		else if((TimeTick250mS[2] == (CAL_RTC->CC[2] / 256)) && !TimerProcessFlag[2])
    {
        CAL_RTC->EVENTS_COMPARE[2] = 0;
        TimerProcessFlag[2]=1;
        //CAL_RTC->TASKS_CLEAR = 1;
        
				if(cal_event_callback[2]) cal_event_callback[2]();

				//NRF_LOG_INFO("RTC compare 1 interrupt detected");
    }
    else if(CAL_RTC->EVENTS_COMPARE[3])
    {
        CAL_RTC->EVENTS_COMPARE[3] = 0;
        
        CAL_RTC->TASKS_CLEAR = 1;
        
        //m_time += m_rtc_increment;
				TimeTick250mS[0]++;
				TimeTick250mS[1]++;
				TimeTick250mS[2]++;
				TimeTick250mS[3]++;
				
				TimerProcessFlag[0] = 0;
				TimerProcessFlag[1] = 0;
				TimerProcessFlag[2] = 0;
				TimerProcessFlag[3] = 0;
				
        if(cal_event_callback[2]) cal_event_callback[2]();
				//NRF_LOG_INFO("RTC compare 2 interrupt detected");
    }
		else
    {
		NRF_LOG_INFO("RTC ETC interrupt detected");
    //drv_rtc_rtc_1_irq_handler();
    }
}
*/