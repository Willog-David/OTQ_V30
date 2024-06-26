
#ifndef PCA9537_H_
#define PCA9537_H_

#include "struct.h"


uint8 PCA9537_GetPort(void);
void PCA9537_SetPort_RTDPWR_HIGH(void);
void PCA9537_SetPort_RTDPWR_LOW(void);
void PCA9537_SetPort_RTDCS_HIGH(void);
void PCA9537_SetPort_RTDCS_LOW(void);
void PCA9537_SetHandler(nrf_drv_twi_t *pHandler);
void PCA9537_Init(void);


#endif
