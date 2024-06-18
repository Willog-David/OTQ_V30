/*
 * Sequence.h
 *
 *  Created on: 2021. 12. 23.
 *      Author: kaohu
 */

#ifndef INC_DRIVER_SEQUENCE_H_
#define INC_DRIVER_SEQUENCE_H_

#include "struct.h"

typedef enum
{
	TestSequence_Pass = 1,
	TestSequence_Fail = -1
}TestSequence_Ret;

void ClearPowerFlag(void);
void Sequence_SelfInit(stMain *pMain);
void Sequence_SelfTest(stMain *pMain);
void Sequence_SelfCalibration(stMain *pMain);
void Sequence_SelfCalibration_Step1(stMain *pMain);
void Sequence_SelfCalibration_Step2(stMain *pMain);
void Sequence_CheckFirstFlag(void);
void Sequence_KeyProcess(stMain *pMain);
#endif /* INC_DRIVER_SEQUENCE_H_ */
