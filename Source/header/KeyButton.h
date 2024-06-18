
#ifndef KEY_PROCESS_H_
#define KEY_PROCESS_H_

#include "struct.h"
#include "nrf_drv_gpiote.h"

void KeyExec_Factory(stMain *pMain,KeyButton Button);
void KeyExec_FactoryIdle(stMain *pMain,KeyButton Button);
void KeyExec_Logging_Idle(stMain *pMain,KeyButton Button);
void KeyExec_Logging(stMain *pMain,KeyButton Button);
void KeyExec_SelfTest(stMain *pMain,KeyButton Button);
void KeyExec_SelfTest_Idle(stMain *pMain,KeyButton Button);
void KeyExec_SelfCalib(stMain *pMain,KeyButton Button);
void KeyExec_SelfCalib_Idle(stMain *pMain,KeyButton Button);
void KeyExec_Correction(stMain *pMain,KeyButton Button);
void KeyExec_Correction_Idle(stMain *pMain,KeyButton Button);
void KeyExcute(stMain *pMain,KeyButton Button);
void KeyProcess(stMain *pMain,nrf_drv_gpiote_pin_t pin);
void OTQV2_ApplyInterval(stMain *pMain);
void OTQV2_ApplyDisplayInterval(stMain *pMain);
void KeyProcessHandler(nrf_drv_gpiote_pin_t pin);

#endif