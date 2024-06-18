/*
 * Probe.c
 *
 *  Created on: 2021. 10. 8.
 *      Author: kaohu
 */

#include "probe.h"
#include "struct.h"
#include "EEPROM.h"
#include "EEPROM_Internal.h"
#include "OTQV2_Init.h"
#include "math.h"



#define ProbeTempCorrection (0x00000010 + 0x04)
#define ProbeTempForceCorrection (0x00000010 + 0x08)
#define ProbeTempForceCorrectionNegative (0x00000010 + 0x0C)
#define ProbeTempCorrectFlag 0x0000D000

float Willog_HOMME_Model(stMain *pMain,float ADCValue);
extern nrf_saadc_channel_config_t hadc1;

#if 1 // 1 = -40 ~ 90, 0 = -20 ~ 120
const uint16 ProbeTempTable[] =
{
	15997	,	15786	,	15575	,	15365	,	15154	,	// -40 ~ -36
	14943	,	14738	,	14534	,	14329	,	14125	,	// -35 ~ -31
	13920	,	13687	,	13453	,	13220	,	12987	,	// -30 ~ -26
	12754	,	12517	,	12281	,	12045	,	11809	,	// -25 ~ -21
	11573	,	11338	,	11102	,	10867	,	10631	,	// -20 ~ -16
	10396	,	10155	,	 9913	,	 9672	,	 9430	,	// -15 ~ -11
	 9189	,	 8957	,	 8725	,	 8494	,	 8262	,	// -10 ~ - 6
	 8030	,	 7823	,	 7615	,	 7408	,	 7201	,	// - 5 ~ - 1
	 6994	,	 6801	,	 6608	,	 6415	,	 6222	,	//   0 ~   4
	 6029	,	 5859	,	 5690	,	 5520	,	 5351	,	//   5 ~  14
	 5181	,	 5029	,	 4877	,	 4726	,	 4574	,	//  10 ~  14
	 4422	,	 4307	,	 4192	,	 4077	,	 3962	,	//  15 ~  19
	 3847	,	 3738	,	 3629	,	 3521	,	 3412	,	//  20 ~  24
	 3304	,	 3208	,	 3112	,	 3016	,	 2921	,	//  25 ~  29
	 2825	,	 2734	,	 2643	,	 2551	,	 2460	,	//  30 ~  34
	 2369	,	 2300	,	 2231	,	 2162	,	 2093	,	//  35 ~  39
	 2024	,	 1963	,	 1903	,	 1842	,	 1781	,	//  40 ~  44
	 1721	,	 1671	,	 1622	,	 1573	,	 1523	,	//  45 ~  49
	 1474	,	 1433	,	 1391	,	 1350	,	 1308	,	//  50 ~  54
	 1267	,	 1229	,	 1191	,	 1154	,	 1116	,	//  55 ~  59
	 1079	,	 1050	,	 1022	,	  993	,	  965	,	//  60 ~  64
	  936	,	  907	,	  877	,	  848	,	  819	,	//  65 ~  69
	  790	,	  768	,	  747	,	  726	,	  705	,	//  70 ~  74
	  684	,	  664	,	  645	,	  626	,	  607	,	//  75 ~  79
	  588	,	  572	,	  556	,	  541	,	  525	,	//  80 ~  84
	  509	,	  494	,	  479	,	  464	,	  449	,	//  85 ~  89
	  434	,																	//  90
};			// -40 ~ 90

#define TemperatureBegin 40

#elif 1

const uint16 ProbeTempTable[] =
{
		4096,		4014,		3882,		3824,		3621,		3497,		3373,		3252,		3141,		3030,		// -20 ~ -11
		2960,		2886,		2784,		2686,		2590,		2498,		2406,		2320,		2236,		2154,		// -10 ~ -1
		2076,		2010,		1938,		1870,		1802,		1738,		1676,		1616,		1559,		1502,		// 0 ~ 9
		1450,		1399,		1330,		1300,		1252,		1208,		1164,		1124,		1083,		1044,		// 10 ~ 19
		1008,		971,		936,		903,		871,		841,		812,		784,		757,		731,		// 20 ~ 29
		706,		681,		657,		635,		613,		593,		572,		552,		534,		516,		// 30 ~ 39
		498,		480,		464,		448,		433,		418,		405,		390,		378,		364,		// 40 ~ 49
		354,		340,		329,		317,		308,		297,		288,		277,		268,		259,		// 50 ~ 59
		251,		242,		234,		226,		209,		211,		205,		197,		190,		183,		// 60 ~ 69
		178,		170,		166,		160,		154,		148,		144,		137,		133,		128,		// 70 ~ 79
		124,		119,		115,		110,		105,		102,		98,			94,			90,			86,			// 80 ~ 89
		83,			78,			75,			72,			69,			65,			63,			60,			67,			54,			// 90 ~ 99
		51,			49,			46,			44,			41,			38,			37,			33,			32,			30,			// 100 ~ 109
		27,			25,			23,			21,			19,			17,			16,			13,			12,			10,			// 110 ~ 119
		9
};

#else
const uint16 ProbeTempTable[] =
{

4071,3942,3813,3689,3568,			// -19
3450,3334,3222,3114,3008,
2905,2806,2709,2615,2525,			// -9
2436,2351,2269,2189,2112,
2037,1966,1896,1830,1764,			// 1
1703,1641,1584,1528,1473,
1421,1372,1323,1277,1232,			//11
1189,1147,1107,1068,1031,
995,961,928,896,865,				//21
835,807,779,753,727,
700,679,656,635,613,				//31
593,573,555,536,519,
502,486,470,455,440,				//41
426,413,400,387,375,
363,352,341,330,320,				//51
311,301,292,283,275,
266,259,251,243,236,				//61
229,223,216,210,204,
198,192,187,182,177,				//71
172,167,162,158,153,
149,145,141,137,134,				//81
130,127,123,120,117,
114,111,108,105,102,				//91
100,97,95,92,90,
88,85,83,81,79,						//101
77,75,74,72,70,
68,67,65,64,62,						//111
61,59,58,57,55
};
#define TemperatureBegin 20

#endif


// ----- probe hardware control 

void EnableProbe(void)
{
	 nrf_gpio_pin_write(OTQV2_GPIO_ADC_PROBE_EN, 0);
}

void EnableProbe_HighTemp(void)
{
	EnableProbe();
}

void EnableProbe_LowTemp(void)
{

}




void DisableProbe(void)
{
	nrf_gpio_pin_write(OTQV2_GPIO_ADC_PROBE_EN, 1);
}

// ----- probe hardware control done


// Function to compare two floats for qsort
int compare(const void * a, const void * b) {
    float fa = *(const float*) a;
    float fb = *(const float*) b;
    return (fa > fb) - (fa < fb);
}

// Function to calculate standard deviation
float calculate_standard_deviation(float data[], int length) {
    float sum = 0.0, mean, standard_deviation = 0.0;

    for(int i = 0; i < length; ++i) {
        sum += data[i];
    }

    mean = sum/length;

    for(int i = 0; i < length; ++i) {
        standard_deviation += pow(data[i] - mean, 2);
    }

    return sqrt(standard_deviation / length);
}

	//float DataSample[100];
	
float AverageFilter(uint16 *DataArray,uint8 size) 
{
    //float data[DATA_LENGTH] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
		float *DataSample, tmp = 0;
		//float tmp = 0;
    float std_dev;
		float median;
		int slot = 0;
		uint16 ret = 0;

		DataSample = malloc(sizeof(float)*size);
		
		memset(DataSample,0x00,sizeof(float)*size);
		
		for(int i=0;i<size;i++)
			{
			DataSample[i] = DataArray[i];
			}
		
    // Sort data
    qsort(DataSample, size, sizeof(float), compare);

    // Calculate median
    if (size % 2 == 0) {
        median = (DataSample[size / 2 - 1] + DataSample[size / 2]) / 2;
    } else {
        median = DataSample[size / 2];
    }
		
		std_dev = calculate_standard_deviation(DataSample,size);			// get deviation

    // Filter data based on median
    for (int i = 0; i < size; i++) {
        if (abs(DataSample[i] - median) <= std_dev) 
				{
            //NRF_LOG_INFO("[%10d] Filtered data[%d] = %d ",getTickCount(), i, DataSample[i]);
						tmp += DataSample[i];
						slot++;
        }
    }
	tmp /= slot;
	
	
	
	//printf("[%10d] Filtered %d data. avg  = %f , dev = %f\n",getTickCount(), slot, tmp,std_dev);
	
	ret = tmp;
	
	free(DataSample);
	return tmp;
}


float AverageFilter_f(float *DataArray,uint8 size) 
{
    //float data[DATA_LENGTH] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
		float *DataSample, tmp = 0;
		//float tmp = 0;
    float std_dev;
		float median;
		int slot = 0;
		uint16 ret = 0;

		DataSample = malloc(sizeof(float)*size);
		
		memset(DataSample,0x00,sizeof(float)*size);
		
		for(int i=0;i<size;i++)
			{
			DataSample[i] = DataArray[i];
			}
		
    // Sort data
    qsort(DataSample, size, sizeof(float), compare);

    // Calculate median
    if (size % 2 == 0) {
        median = (DataSample[size / 2 - 1] + DataSample[size / 2]) / 2;
    } else {
        median = DataSample[size / 2];
    }

    // Filter data based on median
    for (int i = 0; i < size; i++) {
        if (abs(DataSample[i] - median) <= 10.0) 
				{
            //NRF_LOG_INFO("[%10d] Filtered data[%d] = %d ",getTickCount(), i, DataSample[i]);
						tmp += DataSample[i];
						slot++;
        }
    }
	tmp /= slot;
	
	std_dev = calculate_standard_deviation(DataSample,size);
	
	//printf("[%10d] Filtered %d data. avg  = %f , dev = %f\n",getTickCount(), slot, tmp,std_dev);
	
	ret = tmp;
	
	free(DataSample);
	return tmp;
}

void GetDigitalValue(stMain *pMain, uint16* ret)
{
uint32* pSetTemp=(uint32*)ProbeTempCorrection;
uint32 SetTemp=0;
uint16 CorrectValue=0,temp=0;
uint32 TempValue=0;

	for(int j=0;j<10;j++)
	{
	  TempValue+=pMain->Sensor.Probe.ProbeBuffer[j];
	}
	TempValue/=10;

	pSetTemp=&SetTemp;

CorrectValue = ProbeTempTable[*pSetTemp/100+40];

temp = 	(ProbeTempTable[*pSetTemp/100+40] - ProbeTempTable[*pSetTemp/100+41]);
temp *= *pSetTemp%100;
temp /= 100;

CorrectValue += temp;
//CorrectValue = CorrectValue + pSetTemp%100 * CorrectValue / 100;

*ret = TempValue - CorrectValue;
}

float GetTempFromADC(sint16 TempValue, float *ret)
{
	uint16 DiffValue=0,Chip=0,TempCorrection=0;
	uint32 *pp,*ppn;
	sint16 CorrectionDefault = 0;
	sint16 ProbeOffsetValue = 0;

	EEPROM_GetOffset(&ProbeOffsetValue);

	TempValue += ProbeOffsetValue;
	for(int i=0;i<sizeof(ProbeTempTable)/2;i++)
	{
		if( ProbeTempTable[i+1] <= TempValue && TempValue <= ProbeTempTable[i])
		{
			DiffValue = ProbeTempTable[i] - ProbeTempTable[i+1];
			Chip =  ProbeTempTable[i] - TempValue;
			*ret = Chip ;
			*ret /= DiffValue;
			*ret += i;
			*ret -= TemperatureBegin;
			*ret = (*ret / 10) * 10;
			//ret;
		}
	}
	if(TempValue>ProbeTempTable[0])*ret = -39.9;
//	return ret;
}

float ProbeADCMonitor[100];
float ProbeADCMonitor_Int[100];
uint16 slot;


void GetProbeTemp(stMain *pMain,uint8 OffsetEnabled)
{
	uint16 TempCorrection=0;
	uint32 TempValue=0;
	float fTempValue=0;
	uint32 *pp,*ppn;
	sint16 CorrectionDefault = 0;
	sint16 ProbeOffsetValue = 0;
	EEPROM_Datastructure stTempOffset;
	
	memset(&stTempOffset,0x00,sizeof(stTempOffset));

	//float ret=0;

	if(OffsetEnabled)
		{
		EEPROM_GetProbeOffsetData(&stTempOffset);
		}
	pp=(uint32*)ProbeTempForceCorrection;
	ppn=(uint32*)ProbeTempForceCorrectionNegative;

	//EEPROM_GetTempCorrection(&TempCorrection);
	//TempCorrection=0xff;
	//HAL_I2C_DeInit(&hi2c2);
if(TempCorrection>100)TempCorrection=CorrectionDefault;
if(!TempCorrection)TempCorrection=CorrectionDefault;
	  for(int j=0;j<GetSampleInBuf();j++)
	  {
		  TempValue+=pMain->Sensor.Probe.ProbeBuffer[j];
			fTempValue += pMain->Sensor.Probe.ProbeBuffer[j];
	  }
		fTempValue = AverageFilter(pMain->Sensor.Probe.ProbeBuffer,GetSampleInBuf());
	  TempValue/=GetSampleInBuf();
		if(slot<100)			// probe adc result monitoring
			{
			ProbeADCMonitor[slot] = fTempValue;
			ProbeADCMonitor_Int[slot] = TempValue;
			slot++;
			}
		pMain->Sensor.Probe.ProbeADCValue = fTempValue;
		fTempValue = GetADCValueWithBatteryAlign(pMain);
		#ifndef BELL_ENC
	  fTempValue += TempCorrection + stTempOffset.Sensor.ProbeTemp;
		#endif
		TempValue = fTempValue;
#if 0				// force offset
	  if(*pp)
	  {
		  TempValue -= *pp;
	  }
	  if(*ppn)
	  {
		  TempValue += *ppn;
	  }
#endif
#if 0
	pMain->Sensor.Probe.ProbeADCValue = TempValue;
	  GetTempFromADC(TempValue,&pMain->Sensor.Probe.fProbeTemp);
		pMain->Sensor.Probe.ProbeTemp = pMain->Sensor.Probe.fProbeTemp * 100;
#else
	pMain->Sensor.Probe.fProbeTemp = Willog_HOMME_Model(pMain, fTempValue);
	pMain->Sensor.Probe.ProbeTemp = pMain->Sensor.Probe.fProbeTemp * 100;
	//pMain->Sensor.Probe.ProbeADCValue = TempValue;
#endif
	//GetDigitalValue(pMain,&TempCorrection);
}

void ProbeCorrectionSequence(stMain *pMain)
{
     EnableProbe();           // probe enable 코드 추�
                // ADC ��가�오��코드 추�
		 DisableProbe();// probe disable 코드 추�
		  GetProbeTemp(pMain,1);
		  GetDigitalValue(pMain,&pMain->Sensor.Probe.ProbeCorrectionValue);
		  EEPROM_WriteTempCorrection(pMain->Sensor.Probe.ProbeCorrectionValue);
}

uint16 ProbeGetADCCapture(stMain *pMain)
{
uint16 ProbeADCValue=0;
EnableProbe_HighTemp();
HAL_Delay(1);

for(int i=0;i<10;i++)
	{
	ProbeADCValue += pMain->Sensor.Probe.ProbeBuffer[i%10];
	}

ProbeADCValue /= 10;

DisableProbe();
//HAL_GPIO_WritePin(OPAMP_SHDN_GPIO_Port, OPAMP_SHDN_Pin, GPIO_PIN_RESET);
return ProbeADCValue;
}

#define ProbeHysterisis 30				// 30 = about 1 degree. 10 = about 0.3 degree
#define ProbeRawADCMinTemp 20
	#define ProbeRawADCMin (ProbeTempTable[ProbeRawADCMinTemp + 20])
#define ProbeRawADCMaxTemp 30
	#define ProbeRawADCMax (ProbeTempTable[ProbeRawADCMaxTemp + 20])
	
uint16 Probe_CalibrationLoop(stMain *pMain)
{
	static uint8 LoopIndex = 0;
	uint32 ProbeADCTemp = 0;
	uint16 ProbeADCMin = 0, ProbeADCMax = 0;
	
	pMain->Calib.ProbeCalibData[LoopIndex] = ProbeGetADCCapture(pMain);
	LoopIndex++;
	if(LoopIndex >= ProbeCalibrationLoopCount) LoopIndex = 0;
	
	
	for(int i=0;i<ProbeCalibrationLoopCount;i++)
		{
		ProbeADCTemp += pMain->Calib.ProbeCalibData[i];
		}
	ProbeADCTemp /= ProbeCalibrationLoopCount;
	ProbeADCMin = ProbeADCTemp - ProbeHysterisis;
	ProbeADCMax = ProbeADCTemp + ProbeHysterisis;
	
	for(int i=0;i<ProbeCalibrationLoopCount;i++)
		{
		if((pMain->Calib.ProbeCalibData[i] < ProbeADCMin) || (pMain->Calib.ProbeCalibData[i] > ProbeADCMax))
			{
			return 0;
			}
		}
	memset(pMain->Calib.ProbeCalibData,0x00,ProbeCalibrationLoopCount);
	return ProbeADCTemp;
}

float GetADCValueWithBatteryAlign(stMain *pMain)
{
	float BatteryAlign = pMain->Device.BatVolt;
	float ret,fADCValue;
	uint16 ADCValue = pMain->Sensor.Probe.ProbeADCValue;
	#if 0
	BatteryAlign = BatteryAlign / 3000;
	if(BatteryAlign >= 3000)
		{
		BatteryAlign *= 1;
		}
	else
		{
			BatteryAlign *= 0.995;

		}
	#else
	BatteryAlign = 0.8;
	#endif
#if 1
	fADCValue = ADCValue / (BatteryAlign);
	ret = fADCValue;
#else
	if(pMain->Device.BatVolt > 3000)
		{
		fADCValue = ADCValue / (BatteryAlign);
		}
	else
		{
		fADCValue = ADCValue * (BatteryAlign);
		}
	ret = fADCValue;
#endif
	return ret;
}

#define RefADC_M27 13105
#define RefADC_M12 9735
#define RefADC_M02 7380
#define RefADC_23 3488
#define RefADC_42 1898

float GetOffsetCorrection(float *ADCValue, sint16 Offset)
{
  float Ref25C = RefADC_23;
	/*
	if(Offset < -300)
		{
		if(*ADCValue > Ref25C)
			{
			*ADCValue += (*ADCValue - Ref25C) * 0.2;
			}
		else
			{
			*ADCValue = *ADCValue + ((*ADCValue - Ref25C + Offset) * 0.16);
			}
		}
	else if(Offset > 300)
		{
		if(*ADCValue > Ref25C)
			{
			*ADCValue += (*ADCValue - Ref25C) * 0.2;
			}
		else
			{
			//Ref25C = (Ref25C);
			*ADCValue = *ADCValue + ((*ADCValue - Ref25C) * 0.16);
			}
		}
	else
		{
			if(*ADCValue > RefADC_M27)			// -25도 미만 기울기
				{
				*ADCValue += (*ADCValue - Ref25C) * 0.04;
				}
			else if(*ADCValue > RefADC_M12)			// -12도 미만 기울기
				{
				*ADCValue += (*ADCValue - Ref25C) * 0.015;
				}
			else if(*ADCValue > RefADC_M02)			// -2도 미만 기울기
				{
				*ADCValue += (*ADCValue - Ref25C) * 0.05;
				}
			else if(*ADCValue > Ref25C)			// 23도 미만 기울기
				{
				*ADCValue += (*ADCValue - Ref25C) * 0.03;
				}
			else if(*ADCValue < RefADC_42)		// 40도 이상 기울기
				{
				*ADCValue += (*ADCValue - Ref25C) * 0.023;
				}
			else						// 25 ~ 40도 기울기
				{
				*ADCValue += (*ADCValue - Ref25C) * 0.02;
				//*ADCValue += (*ADCValue - Ref25C);
				}
		}
		*/
		
}


float Willog_HOMME_Model_Int1(float ADCValue)			// 
{
	float ret,logTmp = log10f(ADCValue);
	ret = -1732.8017399884647602 + 962.0047974631895613*logTmp - 133.0240568672254540 * powf(logTmp,2);
	return ret;
}

float Willog_HOMME_Model_Int2(float ADCValue)
{
	float ret,logTmp = log10f(ADCValue);
	ret = 291.0537276363419892 -75.6655687080420591*logTmp;
	return ret;
}

float Willog_HOMME_Model_Int3(float ADCValue)
{
	float ret,logTmp = log10f(ADCValue);
	ret =  282.2375077015259421 -73.2779423532693812*logTmp;
	return ret;
}

float Willog_HOMME_AfterCorrection(float fProbeTemp)
{
	float ret;
	if(fProbeTemp < -15.15)			// 변화가 가장 적은 포인트
		{
		ret = fProbeTemp+(-0.00567 * powf(fProbeTemp,2)-0.31978 * fProbeTemp-3.74648);
		}
	else if(fProbeTemp < 1.11)
		{
		ret = -0.001*powf(fProbeTemp,3) - 0.0161*powf(fProbeTemp,2) + 0.9817*fProbeTemp + 0.237;
		}
	else
		{
		ret = fProbeTemp;
		}
	return ret;
}


float Willog_HOMME_Model(stMain *pMain,float ADCValue)
{
	uint8 ADCInt = 0;
	float ret,fADCValue;
	//pMain->Sensor.Probe.ProbeADCValue = ADCValue;
	

	//fADCValue  = 13427;
	//pMain->Calib.CalData.Offset.ProbeTemp = 379;
	fADCValue = ADCValue;
	GetOffsetCorrection(&fADCValue, pMain->Calib.CalData.Offset.ProbeTemp);
	
	pMain->SnapShot.ProbeADC = fADCValue;
	pMain->SnapShot.ProbeADCOffset = fADCValue - pMain->Sensor.Probe.ProbeADCValue;
	if(fADCValue > 7200)
		{
		ADCInt = 1;
		}
	else if(fADCValue > 3200)
		{
		ADCInt = 2;
		}
	else
		{
		ADCInt = 3;
		}
	switch(ADCInt)
		{
		 case 1:
		 ret = Willog_HOMME_Model_Int1(fADCValue);
		 break;

		 case 2:
		 ret = Willog_HOMME_Model_Int2(fADCValue);
		 break;

		 case 3:
		 ret = Willog_HOMME_Model_Int3(fADCValue);
		 break;
		}
	ret = Willog_HOMME_AfterCorrection(ret);
return ret;
}

sint16 GetOffsetValue(stMain *pMain)
{
	sint16 ret;
	
	ret = 3280 - GetADCValueWithBatteryAlign(pMain);
	return ret;
}