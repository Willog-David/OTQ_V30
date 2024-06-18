/*
 * Accelo.h
 *
 *  Created on: Sep 15, 2020
 *      Author: Administrator
 */

#ifndef SRC_DRIVER_ACCELO_H_
#define SRC_DRIVER_ACCELO_H_

#include "struct.h"
#include "types.h"


enum
{
	ACCELRANGE_2G,
	ACCELRANGE_4G,
	ACCELRANGE_8G,
	ACCELRANGE_16G,
	ACCELRANGE_12G
};

enum
{
	ACCEL_LPF_1 = 1,
	ACCEL_LPF_2 = 2,
	ACCEL_LPF_3 = 3,
	ACCEL_LPF_5 = 5,
};

void MC3479_GetAccelData(Accelo *p);
void MC3479_SetMotionThreshold(Accelo *p,uint16 Value);
uint8 Init_MC3479(stMain *pMain,nrf_drv_twi_t *i2cHandler);

#endif /* SRC_DRIVER_ACCELO_H_ */
