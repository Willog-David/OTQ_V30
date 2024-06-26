
#ifndef OTQV2_TIMER_H_
#define OTQV2_TIMER_H_

#include "struct.h"
#include "nrf.h"
#include "nrf_drv_timer.h"
#include "shtc3.h"
#include "EEPROM.h"
#include "app_timer.h"

uint64 LedIndicatorTimer_GetTick(void);
app_timer_timeout_handler_t IntervalTimer_handler(void* p_context);

void ButtonPushTimer_Init(void);
void ButtonPushTimer_Start(void);
void ButtonPushTimer_Stop(void);
uint32 ButtonPushTimer_GetTick(void);

void ButtonReleaseTimer_Init(void);
void ButtonReleaseTimer_Start(void);
void ButtonReleaseTimer_Stop(void);
uint32 ButtonReleaseTimer_GetTick(void);

void BLEActiveTimer_Start(uint32 BLEActiveTimeMs);
void BLEActiveTimer_Stop(void);

void IntervalTimer_Init(stMain *pMain);
void IntervalTimer_Init_RTC(stMain *pMain);

void ReturnToRecordTimer_Init(void);
void ReturnToRecordTimer_Start(void);
void ReturnToRecordTimer_Stop(void);
uint32 ReturnToRecordTimer_GetTick(void);


void HiddenKeyWaitTimer_Init(void);
void HiddenKeyWaitTimer_Start_StepOne(void);
void HiddenKeyWaitTimer_Stop(void);
uint32 HiddenKeyWaitTimer_GetTick(void);

void OTQV2_Timer_RTCStart(stMain *pMain);
void OTQV2_Timer_AllStop(stMain *pMain);
void OTQV2_Timer_Start(stMain *pMain);
void timers_init(void);

#endif