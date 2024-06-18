/*
 * BatGauge.h
 *
 *  Created on: Sep 15, 2020
 *      Author: Administrator
 */

#ifndef SRC_DRIVER_BATGAUGE_H_
#define SRC_DRIVER_BATGAUGE_H_

#include "struct.h"
#include "types.h"

float Batt_GetVoltage_f(stMain *pMain);
uint16 Batt_GetVoltage_int(stMain *pMain);
uint16 Batt_GetVoltage_Percent(stMain *pMain);

#endif /* SRC_DRIVER_BATGAUGE_H_ */
