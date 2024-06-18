/*
 * Probe.h
 *
 *  Created on: 2021. 10. 8.
 *      Author: kaohu
 */

#ifndef SRC_DRIVER_PROBE_H_
#define SRC_DRIVER_PROBE_H_

#include "struct.h"
#include "types.h"

void GetProbeTemp(stMain *pMain,uint8 OffsetEnabled);
void ProbeCorrectionSequence(stMain *pMain);
uint16 Probe_CalibrationLoop(stMain *pMain);
float Willog_HOMME_Model(stMain *pMain,float ADCValue);
float GetADCValueWithBatteryAlign(stMain *pMain);

#endif /* SRC_DRIVER_PROBE_H_ */
