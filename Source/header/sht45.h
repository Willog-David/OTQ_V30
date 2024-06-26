
#ifndef SHT45_H_
#define SHT45_H_

#include "struct.h"

void SHT45_Init(stMain *pMain , nrf_drv_twi_t *i2cHandler);
void SHT45_GetTemp(stMain *pMain);

#endif