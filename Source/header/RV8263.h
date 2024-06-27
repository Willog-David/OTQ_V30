
#ifndef RV8263_H_
#define RV8263_H_

#include "struct.h"


void RV8263_SetTime(stMain *pMain);
void RV8263_Init(stMain *pMain,nrf_drv_twi_t *i2cHandler);

#endif