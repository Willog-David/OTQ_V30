/*
 * BatGauge.c
 *
 *  Created on: Sep 15, 2020
 *      Author: Administrator
 */

#include "BatGauge.h"

uint8 GetSampleInBuf(void);

uint16 BattGauge_LevelTable[20][2] = // {Percentage , voltage *100)
	{
	#if 1
		{100,285},				
		{90,280},
		{80,275},
		{70,273},
		{65,270},
		{60,268},
		{40,260},
		{30,255},
		{20,253},
		{10,250},
		{9,248},
		{8,245},
		{7,240},
		{6,235},
		{5,230},
		{4,225},
		{3,220},
		{2,215},
		{1,210},
		{0,205},

	#else
		{100,300},				
		{90,295},
		{80,290},
		{70,285},
		{60,280},
		{50,275},
		{40,270},
		{30,265},
		{20,260},
		{10,255},
		{9,250},
		{8,245},
		{7,240},
		{6,235},
		{5,230},
		{4,225},
		{3,220},
		{2,215},
		{1,210},
		{0,205},
	#endif
	};


float Batt_GetVoltage_f(stMain *pMain)
{
	uint32 BattADCValue = 0;
	float BattVoltage = 0;
	
	for(int i=0;i<10;i++)
	{
	BattADCValue += pMain->Device.BatVoltBuffer[i];
	}
  BattADCValue/=10;
	
	BattVoltage = BattADCValue*3.6/16384*(6.79/1.8);

	return BattVoltage;
}

uint16 Batt_OffsetInSystemReset(stMain *pMain)
{
	uint16 ret = pMain->Device.BatVolt;
	if(ret > 3000)
		{
		ret += 80;
		}
	else if(ret > 2900)
		{
		ret += 80;
		}
	else if(ret > 2800)
		{
		ret += 130;
		}
	else if(ret > 2700)		
		{
		ret += 145;
		}
	else if(ret > 2600)		
		{
		ret += 160;
		}
	else
		{
		ret += 190;
		}
	return ret;
}

uint16 Batt_GetVoltage_int(stMain *pMain)
{
	float BattADCValue = 0;
	float BattVoltage = 0;
	uint16 BattVoltageInt = 0;
	
	for(int i=0;i<GetSampleInBuf();i++)
	{
	BattADCValue += pMain->Device.BatVoltBuffer[i];
	}
  BattADCValue/=GetSampleInBuf();
	
	BattVoltage = BattADCValue*3.6/16384*(6.79/1.8);
	
	BattVoltageInt = BattVoltage*1000;
	return BattVoltageInt;
}

uint16 Batt_GetVoltage_Percent(stMain *pMain)
{
	uint32 BattADCValue = 0;
	uint16 BattVoltageInt = 0;
	uint8 BattLevelPercent = 0;
	
	BattVoltageInt = pMain->Device.BatVolt / 10;
	
	for(int i=0;i<20;i++)
		{
		if(BattVoltageInt > BattGauge_LevelTable[i][1])
			{
			BattLevelPercent = BattGauge_LevelTable[i][0];
			break;
			}
		}
	
	return BattLevelPercent;
}