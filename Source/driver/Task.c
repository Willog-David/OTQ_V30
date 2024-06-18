/*
 * Task.c
 *
 *  Created on: 2021. 5. 20.
 *      Author: kaohu
 */

#include "Task.h"

nrfx_twi_t *i2cHandle;
//RTC_HandleTypeDef *RTCHandle;
//uint8 *QRCodeBuf;
extern QRCode QRtmp;
extern uint8 WillogURL[];
/*
#ifdef Service_L
//const uint8 HuffmanURL[]="b.willog.io/otq/";
const uint8 HuffmanURL[]="HTTP://L.OTQ.KR/A/";
#elif defined Service_M
const uint8 HuffmanURL[]="HTTP://M.OTQ.KR/A/";
#elif defined Service_B
const uint8 HuffmanURL[]="HTTP://B.OTQ.KR/A/";
#endif
*/
//sint16 TempStuff=-30;

#define SerialNumberAddress 0x0000CC90
#define TimeIntervalAddress 0x00000010
#define VersionString stVersionString

void DeInit_I2C(void);
void ReInit_I2C(void);
void EnterStandbyMode(stMain *pMain);
void Record_WriteData(stMain *pMain);
void Init_RecordStructure(stMain *pMain);

void TempSensor_Init(nrfx_twi_t *Handler)
{
	i2cHandle=Handler;
}


void RtcGetTimeSec(uint32 *currenttimesec)
{
//  *currenttimesec =
		  //RTC_DateStructure.Date * 86400 +
		  //((RTC_TimeStructure.Hours * 3600) +(RTC_TimeStructure.Minutes * 60) +    RTC_TimeStructure.Seconds);

}

void DisplayEnable(stMain *pMain,uint8 *QRbuf,uint8 QRVersion)
{
	uint8 BufferClearDummy[8]={0,0,0,0,0,0,0,0};
	uint8 SampleCode[1000];
	uint32 *FirstStartFlag = 0x0800FF00;

        memset(SampleCode,0x00,1000);
	memcpy(SampleCode,QRbuf,1000);
	memset(QRbuf,0x00,5808);
	if(QRVersion == 13)
	{
#ifdef QR_Center
		  for(int i=0;i<69;i++)
		  {
			  BitCopy(QRbuf,((i+3)*176)*2,SampleCode,i*69,69);
			  BitCopy(QRbuf,((i+3)*176)*2+176,SampleCode,i*69,69);
			  BitDoubler(QRbuf,((i+3)*22)*2,9);
			  BitDoubler(QRbuf,((i+3)*22)*2+22,9);
		  }
		  BitShifter(QRbuf,2887,7);
#else
		  for(int i=0;i<69;i++)
		  {
			  BitCopy(QRbuf,((i+6)*176)*2,SampleCode,i*69,69);
			  BitCopy(QRbuf,((i+6)*176)*2+176,SampleCode,i*69,69);
			  BitDoubler(QRbuf,((i+6)*22)*2,9);
			  BitDoubler(QRbuf,((i+6)*22)*2+22,9);
		  }

	#ifdef Service_L
		  Display_Page(pMain, QRbuf);
	#endif
		  //Display_UpTime(pMain, QRbuf);
		  //Display_Temperature_OnLeft(pMain, QRbuf);
		  BitShifter(QRbuf,5807,2);

#endif
	}
        else if(QRVersion == 17)
	{
		  for(int i=0;i<85;i++)
		  {
			  BitCopy(QRbuf,((i+40)*176)*2,SampleCode,i*85,85);
			  BitCopy(QRbuf,((i+40)*176)*2+176,SampleCode,i*85,85);
			  BitDoubler(QRbuf,((i+40)*22)*2,11);
			  BitDoubler(QRbuf,((i+40)*22)*2+22,11);
		  }
		  BitShifter(QRbuf,5807,3);
	}
        else if(QRVersion == 16)
	{
		  for(int i=0;i<81;i++)
		  {
			  BitCopy(QRbuf,((i+42)*176)*2,SampleCode,i*81,81);
			  BitCopy(QRbuf,((i+42)*176)*2+176,SampleCode,i*81,81);
			  BitDoubler(QRbuf,((i+42)*22)*2,11);
			  BitDoubler(QRbuf,((i+42)*22)*2+22,11);
		  }
		  BitShifter(QRbuf,5807,7);
	}
	else if(QRVersion == 12)
	{
		  for(int i=0;i<69;i++)
		  {
			  BitCopy(QRbuf,((i+6)*176)*2,SampleCode,i*65,65);
			  BitCopy(QRbuf,((i+6)*176)*2+176,SampleCode,i*65,65);
			  BitDoubler(QRbuf,((i+6)*22)*2,9);
			  BitDoubler(QRbuf,((i+6)*22)*2+22,9);
		  }
		  BitShifter(QRbuf,5807,4);
		  BitShifter(QRbuf,5807,7);
	}
	else if(QRVersion == 11)
	{
		  for(int i=0;i<61;i++)
		  {
			  BitCopy(QRbuf,((i+9)*176)*2+8,SampleCode,i*61,61);
			  BitCopy(QRbuf,((i+9)*176)*2+176+8,SampleCode,i*61,61);
			  //BitCopy(QRbuf,((i+15)*176)*2+32+52,BufferClearDummy,0,8);
			  //BitCopy(QRbuf,((i+15)*176)*2+176+32+52,BufferClearDummy,0,8);

			  BitDoubler(QRbuf,((i+9)*22)*2+1,8);
			  BitDoubler(QRbuf,((i+9)*22)*2+22+1,8);
		  }
		  BitShifter(QRbuf,5807,6);
	}
	else if(QRVersion == 10)
	{
		  for(int i=0;i<57;i++)
		  {
			  BitCopy(QRbuf,((i+11)*176)*2+16,SampleCode,i*57,57);
			  BitCopy(QRbuf,((i+11)*176)*2+176+16,SampleCode,i*57,57);
			  //BitCopy(QRbuf,((i+15)*176)*2+32+52,BufferClearDummy,0,8);
			  //BitCopy(QRbuf,((i+15)*176)*2+176+32+52,BufferClearDummy,0,8);

			  BitDoubler(QRbuf,((i+11)*22)*2+2,8);
			  BitDoubler(QRbuf,((i+11)*22)*2+22+2,8);
		  }
		  BitShifter(QRbuf,5807,0);
	}
#if 0				// QR ver 13
	  for(int i=0;i<69;i++)
	  {
		  BitCopy(QRbuf,((i+3)*176)*2,SampleCode,i*69,69);
		  BitCopy(QRbuf,((i+3)*176)*2+176,SampleCode,i*69,69);
		  BitDoubler(QRbuf,((i+3)*22)*2,9);
		  BitDoubler(QRbuf,((i+3)*22)*2+22,9);
	  }
	  BitShifter(QRbuf,2887,7);

#elif 0				// QR ver 9
	  for(int i=0;i<53;i++)
	  {
		  BitCopy(QRbuf,((i+13)*176)*2+24,SampleCode,i*53,53);
		  BitCopy(QRbuf,((i+13)*176)*2+176+24,SampleCode,i*53,53);
		  //BitCopy(QRbuf,((i+15)*176)*2+32+52,BufferClearDummy,0,8);
		  //BitCopy(QRbuf,((i+15)*176)*2+176+32+52,BufferClearDummy,0,8);

		  BitDoubler(QRbuf,((i+13)*22)*2+3,7);
		  BitDoubler(QRbuf,((i+13)*22)*2+22+3,7);
	  }
	  BitShifter(QRbuf,2887,0);
#elif 0				// QR ver 10
	  for(int i=0;i<57;i++)
	  {
		  BitCopy(QRbuf,((i+11)*176)*2+16,SampleCode,i*57,57);
		  BitCopy(QRbuf,((i+11)*176)*2+176+16,SampleCode,i*57,57);
		  //BitCopy(QRbuf,((i+15)*176)*2+32+52,BufferClearDummy,0,8);
		  //BitCopy(QRbuf,((i+15)*176)*2+176+32+52,BufferClearDummy,0,8);

		  BitDoubler(QRbuf,((i+11)*22)*2+2,8);
		  BitDoubler(QRbuf,((i+11)*22)*2+22+2,8);
	  }
	  BitShifter(QRbuf,2887,0);
#elif 0				// QR ver 7
	  for(int i=0;i<49;i++)
	  {
		  BitCopy(QRbuf,((i+15)*176)*2+32,SampleCode,i*49,49);
		  BitCopy(QRbuf,((i+15)*176)*2+176+32,SampleCode,i*49,49);
		  //BitCopy(QRbuf,((i+15)*176)*2+32+52,BufferClearDummy,0,8);
		  //BitCopy(QRbuf,((i+15)*176)*2+176+32+52,BufferClearDummy,0,8);

		  BitDoubler(QRbuf,((i+15)*22)*2+4,9);
		  BitDoubler(QRbuf,((i+15)*22)*2+22+4,9);
	  }
	  BitShifter(QRbuf,2887,0);


#else

#endif

}

void Display_ClearEEPROM(stMain *pMain,uint8 *QRbuf)
{

	memset(QRbuf,0x00,2888);
	EINK_WriteString(QRbuf,8,70,"Clear EEPROM");
	EINK_WriteString(QRbuf,8,82,"Please Wait...");
	EINK_WriteString(QRbuf,8,106,"It will take about ");
	EINK_WriteString(QRbuf,8,118,"50Secs");
}

uint32 GetIntervalFromFlash(void)
{
uint32 *pp=(uint32*)TimeIntervalAddress;
uint32 ret=0;
ret=
		((*pp / 0x100000 % 0x10)*100000 )+
		((*pp / 0x10000 % 0x10) * 10000 )+
		((*pp / 0x1000 % 0x10) * 1000) +
		((*pp / 0x100 % 0x10) * 100) +((*pp / 0x10 % 0x10) * 10)
		+ *pp % 0x10;

if(!ret)ret=60;

return ret;
}


void GetSerialNumber(stMain *pMain)
{
uint32 *p=0;
uint32 UID[3]={0,0,0};

UID[0] = HAL_GetUIDw0();
UID[1] = HAL_GetUIDw1();
UID[2] = HAL_GetUIDw2();

p=(uint32*)&UID[0];

pMain->DevInfo.Serial[0] = ((*p / 10000000 % 10) + 0x30);
pMain->DevInfo.Serial[1] = ((*p / 1000000  % 10) + 0x30);
pMain->DevInfo.Serial[2] = ((*p / 100000   % 10) + 0x30);
pMain->DevInfo.Serial[3] = ((*p / 10000    % 10) + 0x30);
pMain->DevInfo.Serial[4] = ((*p / 1000     % 10) + 0x30);
pMain->DevInfo.Serial[5] = ((*p / 100      % 10) + 0x30);
pMain->DevInfo.Serial[6] = ((*p / 10       % 10) + 0x30);
pMain->DevInfo.Serial[7] = ((*p / 1        % 10) + 0x30);

}

void GetSerialNumberFromFlash(stMain *pMain)
{
	uint32 *p=0;
	uint32 UID[3]={0,0,0};

	p=(uint32*)0x0800fff0;

	pMain->DevInfo.Serial[6] = ((*p / 0x10000000 % 0x10) + 0x30);
	pMain->DevInfo.Serial[7] = ((*p / 0x1000000  % 0x10) + 0x30);
	pMain->DevInfo.Serial[4] = ((*p / 0x100000   % 0x10) + 0x30);
	pMain->DevInfo.Serial[5] = ((*p / 0x10000    % 0x10) + 0x30);
	pMain->DevInfo.Serial[2] = ((*p / 0x1000     % 0x10) + 0x30);
	pMain->DevInfo.Serial[3] = ((*p / 0x100      % 0x10) + 0x30);
	pMain->DevInfo.Serial[0] = ((*p / 0x10       % 0x10) + 0x30);
	pMain->DevInfo.Serial[1] = ((*p / 0x1        % 0x10) + 0x30);
}

void GetSerialNumberProcess(stMain *pMain)
{
	GetSerialNumber(pMain);
	if(pMain->DevInfo.Serial[0] == '2' || pMain->DevInfo.Serial[0] == '3')
	{
		EEPROM_WriteSerialNumber(pMain->DevInfo.Serial);
	}
	else
	{
		EEPROM_GetSerialNumber(pMain->DevInfo.Serial);
		if(!(pMain->DevInfo.Serial[0] == '2' || pMain->DevInfo.Serial[0] == '3'))
		{
			GetSerialNumberFromUID(pMain);
		}

	}

}


void Display_PartialTest(stMain *pMain,uint8 *QRbuf)
{
		//pLogData->DevInfo.Serial[8]=0x30;
		//pLogData->DevInfo.Serial[9]=0x30;
	uint8 SampleCode[600];
	qrcode_initBytes(&QRtmp,QRbuf,1,0,pMain->DevInfo.Serial,8);
	memcpy(SampleCode,QRbuf,56);
		memset(QRbuf,0x00,2888);

		for(int i=0;i<21;i++)
		{
			BitCopy(QRbuf,((i+50)*176)*2+88,SampleCode,i*21,21);
			BitCopy(QRbuf,((i+50)*176)*2+88+176,SampleCode,i*21,21);
			BitDoubler(QRbuf,((i+50)*22)*2+11,3);
			BitDoubler(QRbuf,((i+50)*22)*2+22+11,3);
		}
		BitShifter(QRbuf,2887,8);

		if(pMain->Sensor.PCBTemp < 0)
		{
			EINK_WriteLargeFont(QRbuf,64,70,14);
		}
 		EINK_WriteLargeFont(QRbuf,112,70,12);
		EINK_WriteLargeFont(QRbuf,120,70,13);
		if(abs(pMain->Sensor.PCBTemp) / 1000 %10)
		{
			EINK_WriteLargeFont(QRbuf,80,70,abs(pMain->Sensor.PCBTemp) / 1000 %10);
		}
		EINK_WriteLargeFont(QRbuf,96,70,abs(pMain->Sensor.PCBTemp) / 100 % 10);

		EINK_WriteFont(QRbuf,64-40,176-51,pMain->DevInfo.Serial[0]);
		EINK_WriteFont(QRbuf,72-40,176-51,pMain->DevInfo.Serial[1]);
		EINK_WriteFont(QRbuf,80-40,176-51,pMain->DevInfo.Serial[2]);
		EINK_WriteFont(QRbuf,88-40,176-51,pMain->DevInfo.Serial[3]);
		EINK_WriteFont(QRbuf,96-40,176-51,pMain->DevInfo.Serial[4]);
		EINK_WriteFont(QRbuf,104-40,176-51,pMain->DevInfo.Serial[5]);
		EINK_WriteFont(QRbuf,112-40,176-51,pMain->DevInfo.Serial[6]);
		EINK_WriteFont(QRbuf,120-40,176-51,pMain->DevInfo.Serial[7]);
		EINK_WriteString(QRbuf,0+8,70-36,"Press the button");
		EINK_WriteString(QRbuf,0+16,82-36,"to scan QR code");
		//EINK_WriteString(QRbuf,8+24,94-36,"pressing this");
		//EINK_WriteString(QRbuf,8+32,106-36,"button twice");
	#ifdef THD
		EINK_WriteString(QRbuf,56-56,114,"THUR023 ");
	#elif defined TD
		EINK_WriteString(QRbuf,56-56 + 32,114,VersionString);
	#endif
		/*
		EINK_WriteFont(QRbuf,88+16,126,(pMain->TempCorrectValue / 10000 % 10)+0x30);
		EINK_WriteFont(QRbuf,96+16,126,(pMain->TempCorrectValue / 1000 % 10)+0x30);
		EINK_WriteFont(QRbuf,104+16,126,(pMain->TempCorrectValue / 100 % 10)+0x30);
		EINK_WriteFont(QRbuf,112+16,126,(pMain->TempCorrectValue / 10 % 10)+0x30);
		EINK_WriteFont(QRbuf,120+16,126,(pMain->TempCorrectValue % 10)+0x30);
	*/
		EINK_WriteString(QRbuf,56-40,126,"Intv.");
		EINK_WriteFont(QRbuf,56+8,126,(pMain->Sensor.SensorIntervalSec/60 / 100 % 10)+0x30);
		EINK_WriteFont(QRbuf,64+8,126,(pMain->Sensor.SensorIntervalSec/60 / 10 % 10)+0x30);
		EINK_WriteFont(QRbuf,72+8,126,(pMain->Sensor.SensorIntervalSec/60 % 10)+0x30);
		EINK_WriteIcon(QRbuf,176-42,176-142,17);
	uint8 AAAA=16;
		EINK_WriteIcon(QRbuf,176-34,176-142,AAAA);

		EINK_WriteIcon(QRbuf,176-26,176-142,14);

		//EINK_WriteIcon(QrCode,32,142,0x05);
		//EEPROM_WriteDevInfo.Serial(pLogData->DevInfo.Serial);

		BitShifter(QRbuf,2887,2);
		EPD_Dis_Part(100,100,QRbuf,50,50,0);

}

void Display_Correction(stMain *pMain,uint8 *QRbuf)
{
	uint8 SampleCode[600];
	qrcode_initBytes(&QRtmp,QRbuf,1,0,pMain->DevInfo.Serial,8);
	memcpy(SampleCode,QRbuf,56);
		memset(QRbuf,0x00,2888);

		for(int i=0;i<21;i++)
		{
			BitCopy(QRbuf,((i+50)*176)*2+88,SampleCode,i*21,21);
			BitCopy(QRbuf,((i+50)*176)*2+88+176,SampleCode,i*21,21);
			BitDoubler(QRbuf,((i+50)*22)*2+11,3);
			BitDoubler(QRbuf,((i+50)*22)*2+22+11,3);
		}

		BitShifter(QRbuf,2887,8);

		EINK_WriteString(QRbuf,0+0,12,"3. Certi step");

#if defined Recording_H|defined Recording_A
		if(pMain->Sensor.PCBTemp < 0)
		{
			EINK_WriteLargeFont(QRbuf,16,70,14);
		}
 		EINK_WriteLargeFont(QRbuf,48,70,12);
		EINK_WriteLargeFont(QRbuf,60,70,13);
		if(abs(pMain->Sensor.PCBTemp) / 1000 %10)
		{
			EINK_WriteLargeFont(QRbuf,16,70,abs(pMain->Sensor.PCBTemp) / 1000 %10);
		}
		EINK_WriteLargeFont(QRbuf,32,70,abs(pMain->Sensor.PCBTemp) / 100 % 10);

 		EINK_WriteLargeFont(QRbuf,92,70,pMain->Sensor.PCBHumi /10 %10);
		EINK_WriteLargeFont(QRbuf,108,70,pMain->Sensor.PCBHumi %10);
		EINK_WriteLargeFont(QRbuf,124,70,29);
#else
		if(pMain->Sensor.PCBTemp < 0)
		{
			EINK_WriteLargeFont(QRbuf,24,70,14);
		}
 		EINK_WriteLargeFont(QRbuf,112,70,12);
		EINK_WriteLargeFont(QRbuf,120,70,13);
		if(abs(pMain->Sensor.PCBTemp) / 1000 %10)
		{
			EINK_WriteLargeFont(QRbuf,40,70,abs(pMain->Sensor.PCBTemp) / 1000 %10);
		}
		EINK_WriteLargeFont(QRbuf,56,70,abs(pMain->Sensor.PCBTemp) / 100 % 10);
		EINK_WriteLargeFont(QRbuf,72,70,15);
		EINK_WriteLargeFont(QRbuf,80,70,abs(pMain->Sensor.PCBTemp) / 10 % 10);
		EINK_WriteLargeFont(QRbuf,96,70,abs(pMain->Sensor.PCBTemp) / 1 % 10);
#endif
		EINK_WriteFont(QRbuf,64-40,176-51,pMain->DevInfo.Serial[0]);
		EINK_WriteFont(QRbuf,72-40,176-51,pMain->DevInfo.Serial[1]);
		EINK_WriteFont(QRbuf,80-40,176-51,pMain->DevInfo.Serial[2]);
		EINK_WriteFont(QRbuf,88-40,176-51,pMain->DevInfo.Serial[3]);
		EINK_WriteFont(QRbuf,96-40,176-51,pMain->DevInfo.Serial[4]);
		EINK_WriteFont(QRbuf,104-40,176-51,pMain->DevInfo.Serial[5]);
		EINK_WriteFont(QRbuf,112-40,176-51,pMain->DevInfo.Serial[6]);
		EINK_WriteFont(QRbuf,120-40,176-51,pMain->DevInfo.Serial[7]);
	#ifdef THD
		EINK_WriteString(QRbuf,56-56,114,"THUR023 ");
	#elif defined TD
		EINK_WriteString(QRbuf,56-56 + 24,114,VersionString);
	#endif
		/*
		EINK_WriteFont(QRbuf,88+16,126,(pMain->TempCorrectValue / 10000 % 10)+0x30);
		EINK_WriteFont(QRbuf,96+16,126,(pMain->TempCorrectValue / 1000 % 10)+0x30);
		EINK_WriteFont(QRbuf,104+16,126,(pMain->TempCorrectValue / 100 % 10)+0x30);
		EINK_WriteFont(QRbuf,112+16,126,(pMain->TempCorrectValue / 10 % 10)+0x30);
		EINK_WriteFont(QRbuf,120+16,126,(pMain->TempCorrectValue % 10)+0x30);
	*/
		EINK_WriteString(QRbuf,56-40,126,"Intv.");
		if(pMain->Sensor.SensorIntervalSec)
		{
			EINK_WriteFont(QRbuf,56+8,126,(pMain->Sensor.SensorIntervalSec/60 / 100 % 10)+0x30);
			EINK_WriteFont(QRbuf,64+8,126,(pMain->Sensor.SensorIntervalSec/60 / 10 % 10)+0x30);
			EINK_WriteFont(QRbuf,72+8,126,(pMain->Sensor.SensorIntervalSec/60 % 10)+0x30);
		}
		else
		{
			EINK_WriteString(QRbuf,56+8,126,"Deb");
		}
		BitShifter(QRbuf,2887,2);
EPD_Display(pMain,QRbuf,0,0);
}

#ifdef Display_ButtonUpSide
void Display_LargeNumber(stMain *pMain,uint8 *QRbuf,uint8 QuickRefresh)
{
		//pLogData->DevInfo.Serial[8]=0x30;
		//pLogData->DevInfo.Serial[9]=0x30;
	uint8 SampleCode[600];
	uint8 Serial[20];
	sint16 TempValue=0;

	memcpy(Serial,pMain->DevInfo.Serial,8);

	TempValue = pMain->Sensor.PCBTemp;

	if(TempValue < 1)
	{
		//TempValue = -100;
	}
	else if(TempValue % 100 > 49)
	{
		TempValue+=100;
		TempValue = TempValue - TempValue % 100;
	}
	else
	{
		TempValue = TempValue - TempValue % 100;
	}

	Serial[8] = 'T';
	if(pMain->Sensor.PCBTemp >= 0)
	{
		Serial[9] = '+';
	}
	else
	{
		Serial[9] = '-';
	}



	Serial[10] = abs(pMain->Sensor.PCBTemp) / 1000 % 10 + 0x30;
	Serial[11] = abs(pMain->Sensor.PCBTemp) / 100 % 10 + 0x30;
	Serial[12] = abs(pMain->Sensor.PCBTemp) / 10 % 10 + 0x30;

	qrcode_initBytes(&QRtmp,QRbuf,1,1,Serial,13);
	memcpy(SampleCode,QRbuf,56);
		memset(QRbuf,0x00,2888);
		for(int i=0;i<8;i++)
		{
			if(!pMain->DevInfo.Serial[i])pMain->DevInfo.Serial[i]=i+0x30;
		}

		for(int i=0;i<21;i++)
		{
			BitCopy(QRbuf,((i+50)*176)*2+88,SampleCode,i*21,21);
			BitCopy(QRbuf,((i+50)*176)*2+88+176,SampleCode,i*21,21);
			BitDoubler(QRbuf,((i+50)*22)*2+11,3);
			BitDoubler(QRbuf,((i+50)*22)*2+22+11,3);
		}
		BitShifter(QRbuf,2887,8);
#ifdef OORModeEnable			// detail text
		if((TempValue < 0) | (TempValue > 1550))
		{
			EINK_WriteString(QRbuf,0+56,94-36,"For detail");
		}
		else
		{
			if(abs(TempValue) / 1000 %10)
			{
				EINK_WriteLargeFont(QRbuf,80,70,abs(TempValue) / 1000 %10);
			}
			EINK_WriteLargeFont(QRbuf,96,70,abs(TempValue) / 100 % 10);
	 		EINK_WriteLargeFont(QRbuf,112,70,12);
			EINK_WriteLargeFont(QRbuf,120,70,13);
		}
#elif 0			// korean
		if((TempValue < 0) | (TempValue > 1500))
		{
			EINK_WriteLargeFont(QRbuf,24,70,21);
			EINK_WriteLargeFont(QRbuf,40,70,22);
			EINK_WriteLargeFont(QRbuf,48,70,23);
			EINK_WriteLargeFont(QRbuf,64,70,24);
			EINK_WriteLargeFont(QRbuf,72,70,25);
			EINK_WriteLargeFont(QRbuf,88,70,26);
			EINK_WriteLargeFont(QRbuf,96,70,27);
			EINK_WriteLargeFont(QRbuf,112,70,28);
		}
		else
		{
			if(abs(TempValue) / 1000 %10)
			{
				EINK_WriteLargeFont(QRbuf,80,70,abs(TempValue) / 1000 %10);
			}
			EINK_WriteLargeFont(QRbuf,96,70,abs(TempValue) / 100 % 10);
	 		EINK_WriteLargeFont(QRbuf,112,70,12);
			EINK_WriteLargeFont(QRbuf,120,70,13);
		}
#elif 0			// large arrow + temp
		if(TempValue < 0)
		{
			//EINK_WriteLargeFont(QRbuf,80,70,0);
			EINK_WriteLargeFont(QRbuf,96,70,0);
			EINK_WriteLargeFont(QRbuf,72,70,20);
			EINK_WriteLargeFont(QRbuf,56,70,22);

		}
		else if(TempValue > 1500)
		{
			EINK_WriteLargeFont(QRbuf,80,70,1);
			EINK_WriteLargeFont(QRbuf,96,70,5);
			EINK_WriteLargeFont(QRbuf,72,70,20);
			EINK_WriteLargeFont(QRbuf,56,70,22);
		}
		else
		{
			if(abs(TempValue) / 1000 %10)
			{
				EINK_WriteLargeFont(QRbuf,80,70,abs(TempValue) / 1000 %10);
			}
			EINK_WriteLargeFont(QRbuf,96,70,abs(TempValue) / 100 % 10);
		}
 		EINK_WriteLargeFont(QRbuf,112,70,12);
		EINK_WriteLargeFont(QRbuf,120,70,13);
#elif 0				// arrow + temperature
		if(TempValue < 0)
		{
			//EINK_WriteLargeFont(QRbuf,80,70,0);
			EINK_WriteLargeFont(QRbuf,96,70,0);
			EINK_WriteLargeFont(QRbuf,72,70,18);

		}
		else if(TempValue > 1500)
		{
			EINK_WriteLargeFont(QRbuf,80,70,1);
			EINK_WriteLargeFont(QRbuf,96,70,5);
			EINK_WriteLargeFont(QRbuf,72,70,17);
		}
		else
		{
			if(abs(TempValue) / 1000 %10)
			{
				EINK_WriteLargeFont(QRbuf,80,70,abs(TempValue) / 1000 %10);
			}
			EINK_WriteLargeFont(QRbuf,96,70,abs(TempValue) / 100 % 10);
		}
 		EINK_WriteLargeFont(QRbuf,112,70,12);
		EINK_WriteLargeFont(QRbuf,120,70,13);
#elif 0				// OOR
		if((TempValue < 0) | (TempValue > 1500))
		{
			EINK_WriteLargeFont(QRbuf,88,70,0);
			EINK_WriteLargeFont(QRbuf,104,70,0);
			EINK_WriteLargeFont(QRbuf,120,70,16);
		}
		else
		{
			if(abs(TempValue) / 1000 %10)
			{
				EINK_WriteLargeFont(QRbuf,80,70,abs(TempValue) / 1000 %10);
			}
			EINK_WriteLargeFont(QRbuf,96,70,abs(TempValue) / 100 % 10);
	 		EINK_WriteLargeFont(QRbuf,112,70,12);
			EINK_WriteLargeFont(QRbuf,120,70,13);
		}
#elif 0				// -- temp
		if((TempValue < 0) | (TempValue > 1500))
		{
			//EINK_WriteLargeFont(QRbuf,64,70,14);
			EINK_WriteLargeFont(QRbuf,80,70,14);
			EINK_WriteLargeFont(QRbuf,96,70,14);
			//EINK_WriteLargeFont(QRbuf,120,70,14);
		}
		else
		{
			if(abs(TempValue) / 1000 %10)
			{
				EINK_WriteLargeFont(QRbuf,80,70,abs(TempValue) / 1000 %10);
			}
			EINK_WriteLargeFont(QRbuf,96,70,abs(TempValue) / 100 % 10);

		}
 		EINK_WriteLargeFont(QRbuf,112,70,12);
		EINK_WriteLargeFont(QRbuf,120,70,13);
#else
		if(TempValue < 0)
		{
			EINK_WriteLargeFont(QRbuf,64,70,14);
		}
 		EINK_WriteLargeFont(QRbuf,112,70,12);
		EINK_WriteLargeFont(QRbuf,120,70,13);
		if(abs(TempValue) / 1000 %10)
		{
			EINK_WriteLargeFont(QRbuf,80,70,abs(TempValue) / 1000 %10);
		}
		EINK_WriteLargeFont(QRbuf,96,70,abs(TempValue) / 100 % 10);
#endif


		EINK_WriteFont(QRbuf,64-40,176-51,pMain->DevInfo.Serial[0]);
		EINK_WriteFont(QRbuf,72-40,176-51,pMain->DevInfo.Serial[1]);
		EINK_WriteFont(QRbuf,80-40,176-51,pMain->DevInfo.Serial[2]);
		EINK_WriteFont(QRbuf,88-40,176-51,pMain->DevInfo.Serial[3]);
		EINK_WriteFont(QRbuf,96-40,176-51,pMain->DevInfo.Serial[4]);
		EINK_WriteFont(QRbuf,104-40,176-51,pMain->DevInfo.Serial[5]);
		EINK_WriteFont(QRbuf,112-40,176-51,pMain->DevInfo.Serial[6]);
		EINK_WriteFont(QRbuf,120-40,176-51,pMain->DevInfo.Serial[7]);
		EINK_WriteString(QRbuf,0+8,70-36,"Press the button");
		EINK_WriteString(QRbuf,0+16,82-36,"to scan QR code");
		//EINK_WriteString(QRbuf,8+24,94-36,"pressing this");
		//EINK_WriteString(QRbuf,8+32,106-36,"button twice");
		EINK_WriteString(QRbuf,56-56 + 24,114,VersionString);
		/*
		EINK_WriteFont(QRbuf,88+16,126,(pMain->TempCorrectValue / 10000 % 10)+0x30);
		EINK_WriteFont(QRbuf,96+16,126,(pMain->TempCorrectValue / 1000 % 10)+0x30);
		EINK_WriteFont(QRbuf,104+16,126,(pMain->TempCorrectValue / 100 % 10)+0x30);
		EINK_WriteFont(QRbuf,112+16,126,(pMain->TempCorrectValue / 10 % 10)+0x30);
		EINK_WriteFont(QRbuf,120+16,126,(pMain->TempCorrectValue % 10)+0x30);
	*/
		EINK_WriteString(QRbuf,56-40,126,"Intv.");
		EINK_WriteFont(QRbuf,56+8,126,(pMain->Sensor.SensorIntervalSec/60 / 100 % 10)+0x30);
		EINK_WriteFont(QRbuf,64+8,126,(pMain->Sensor.SensorIntervalSec/60 / 10 % 10)+0x30);
		EINK_WriteFont(QRbuf,72+8,126,(pMain->Sensor.SensorIntervalSec/60 % 10)+0x30);
		EINK_WriteIcon(QRbuf,176-42,176-142,17);
	uint8 AAAA=16;
		EINK_WriteIcon(QRbuf,176-34,176-142,AAAA);

		EINK_WriteIcon(QRbuf,176-26,176-142,14);

		//EINK_WriteIcon(QrCode,32,142,0x05);
		//EEPROM_WriteDevInfo.Serial(pLogData->DevInfo.Serial);

		BitShifter(QRbuf,2887,2);
EPD_Display(pMain,QRbuf,QuickRefresh,0);
}

#else

void Display_LargeNumber(stMain *pMain,uint8 *QRbuf,uint8 QuickRefresh)
{
		//pLogData->DevInfo.Serial[8]=0x30;
		//pLogData->DevInfo.Serial[9]=0x30;
	uint8 SampleCode[600];
	uint8 SerialNumber[20];
	sint16 TempValue=0;
	sint8 HumiValue=0;

	memcpy(SerialNumber,pMain->DevInfo.Serial,8);

	TempValue = pMain->Sensor.PCBTemp;

	if(TempValue % 100 > 49)
	{
		TempValue+=100;
		TempValue = TempValue - TempValue % 100;
	}
	else
	{
		TempValue = TempValue - TempValue % 100;
	}
	HumiValue = pMain->Sensor.PCBHumi;

	SerialNumber[8] = 'T';
	if(pMain->Sensor.PCBTemp >= 0)
	{
		SerialNumber[9] = '+';
	}
	else
	{
		SerialNumber[9] = '-';
	}



	SerialNumber[10] = abs(pMain->Sensor.PCBTemp) / 1000 % 10 + 0x30;
	SerialNumber[11] = abs(pMain->Sensor.PCBTemp) / 100 % 10 + 0x30;
	SerialNumber[12] = abs(pMain->Sensor.PCBTemp) / 10 % 10 + 0x30;

	qrcode_initBytes(&QRtmp,QRbuf,1,1,SerialNumber,13);
	memcpy(SampleCode,QRbuf,56);
		memset(QRbuf,0x00,2888);

		for(int i=0;i<21;i++)
		{
			BitCopy(QRbuf,((i+6)*176)*2+88,SampleCode,i*21,21);
			BitCopy(QRbuf,((i+6)*176)*2+88+176,SampleCode,i*21,21);
			BitDoubler(QRbuf,((i+6)*22)*2+11,3);
			BitDoubler(QRbuf,((i+6)*22)*2+22+11,3);
		}
		BitShifter(QRbuf,2887,8);
#if (defined Recording_H|defined Recording_A)
		if(TempValue < 0)
		{
			EINK_WriteLargeFont(QRbuf,16,70,14);
		}
 		EINK_WriteLargeFont(QRbuf,48,70,12);
		EINK_WriteLargeFont(QRbuf,60,70,13);
		if(abs(TempValue) / 1000 %10)
		{
			EINK_WriteLargeFont(QRbuf,16,70,abs(TempValue) / 1000 %10);
		}
		EINK_WriteLargeFont(QRbuf,32,70,abs(TempValue) / 100 % 10);

 		EINK_WriteLargeFont(QRbuf,92,70,HumiValue /10 %10);
		EINK_WriteLargeFont(QRbuf,108,70,HumiValue %10);
		EINK_WriteLargeFont(QRbuf,124,70,29);
#else
		if(TempValue < 0)
		{
			EINK_WriteLargeFont(QRbuf,64,70,14);
		}
 		EINK_WriteLargeFont(QRbuf,112,70,12);
		EINK_WriteLargeFont(QRbuf,120,70,13);
		if(abs(TempValue) / 1000 %10)
		{
			EINK_WriteLargeFont(QRbuf,80,70,abs(TempValue) / 1000 %10);
		}
		EINK_WriteLargeFont(QRbuf,96,70,abs(TempValue) / 100 % 10);
#endif
		EINK_WriteFont(QRbuf,64-40,14,pMain->DevInfo.Serial[0]);
		EINK_WriteFont(QRbuf,72-40,14,pMain->DevInfo.Serial[1]);
		EINK_WriteFont(QRbuf,80-40,14,pMain->DevInfo.Serial[2]);
		EINK_WriteFont(QRbuf,88-40,14,pMain->DevInfo.Serial[3]);
		EINK_WriteFont(QRbuf,96-40,14,pMain->DevInfo.Serial[4]);
		EINK_WriteFont(QRbuf,104-40,14,pMain->DevInfo.Serial[5]);
		EINK_WriteFont(QRbuf,112-40,14,pMain->DevInfo.Serial[6]);
		EINK_WriteFont(QRbuf,120-40,14,pMain->DevInfo.Serial[7]);
		EINK_WriteString(QRbuf,0+8,151-51,"Press the button");
		EINK_WriteString(QRbuf,0+16,151-39,"to scan QR code");
		//EINK_WriteString(QRbuf,8+24,94-36,"pressing this");
		//EINK_WriteString(QRbuf,8+32,106-36,"button twice");
		EINK_WriteString(QRbuf,56-56 + 24,37,VersionString);
		/*
		EINK_WriteFont(QRbuf,88+16,126,(pMain->TempCorrectValue / 10000 % 10)+0x30);
		EINK_WriteFont(QRbuf,96+16,126,(pMain->TempCorrectValue / 1000 % 10)+0x30);
		EINK_WriteFont(QRbuf,104+16,126,(pMain->TempCorrectValue / 100 % 10)+0x30);
		EINK_WriteFont(QRbuf,112+16,126,(pMain->TempCorrectValue / 10 % 10)+0x30);
		EINK_WriteFont(QRbuf,120+16,126,(pMain->TempCorrectValue % 10)+0x30);
	*/
		EINK_WriteString(QRbuf,56-40,25,"Intv.");
		EINK_WriteFont(QRbuf,56+8,25,(pMain->Sensor.SensorIntervalSec/60 / 100 % 10)+0x30);
		EINK_WriteFont(QRbuf,64+8,25,(pMain->Sensor.SensorIntervalSec/60 / 10 % 10)+0x30);
		EINK_WriteFont(QRbuf,72+8,25,(pMain->Sensor.SensorIntervalSec/60 % 10)+0x30);
		EINK_WriteIcon(QRbuf,18,137,21);
	uint8 AAAA=16;
		EINK_WriteIcon(QRbuf,26,137,20);

		EINK_WriteIcon(QRbuf,34,137,18);

		//EINK_WriteIcon(QrCode,32,142,0x05);
		//EEPROM_WriteDevInfo.Serial(pLogData->DevInfo.Serial);

		BitShifter(QRbuf,2887,2);
		EPD_Display(pMain,QRbuf,QuickRefresh,0);
}
#endif


void Display_Factory(stMain *pMain)
{
uint8 QRbuf[2888];
uint8 SampleCode[600];
	//pLogData->DevInfo.Serial[8]=0x30;
	//pLogData->DevInfo.Serial[9]=0x30;
qrcode_initBytes(&QRtmp,QRbuf,1,0,pMain->DevInfo.Serial,8);
memcpy(SampleCode,QRbuf,56);

#ifdef Display_ButtonUpSide			// 1 = display inverse
	memset(QRbuf,0x00,2888);
	for(int i=0;i<21;i++)
	{
		BitCopy(QRbuf,((i+50)*176)*2+88,SampleCode,i*21,21);
		BitCopy(QRbuf,((i+50)*176)*2+88+176,SampleCode,i*21,21);
		BitDoubler(QRbuf,((i+50)*22)*2+11,3);
		BitDoubler(QRbuf,((i+50)*22)*2+22+11,3);
	}
	BitShifter(QRbuf,2887,8);

	EINK_WriteFont(QRbuf,64-40,176-51,pMain->DevInfo.Serial[0]);
	EINK_WriteFont(QRbuf,72-40,176-51,pMain->DevInfo.Serial[1]);
	EINK_WriteFont(QRbuf,80-40,176-51,pMain->DevInfo.Serial[2]);
	EINK_WriteFont(QRbuf,88-40,176-51,pMain->DevInfo.Serial[3]);
	EINK_WriteFont(QRbuf,96-40,176-51,pMain->DevInfo.Serial[4]);
	EINK_WriteFont(QRbuf,104-40,176-51,pMain->DevInfo.Serial[5]);
	EINK_WriteFont(QRbuf,112-40,176-51,pMain->DevInfo.Serial[6]);
	EINK_WriteFont(QRbuf,120-40,176-51,pMain->DevInfo.Serial[7]);
	EINK_WriteString(QRbuf,0+0,70-36,"Press button 3times");
	EINK_WriteString(QRbuf,0+80,82-36,"TO START");
	//EINK_WriteString(QRbuf,8+24,94-36,"pressing this");
	//EINK_WriteString(QRbuf,8+32,106-36,"button twice");
#ifdef THD
	EINK_WriteString(QRbuf,56-16,114,"THUR002");
#elif defined TD
	EINK_WriteString(QRbuf,56-32,114,VersionString);
#endif
	/*
	EINK_WriteFont(QRbuf,88+16,126,(pMain->TempCorrectValue / 10000 % 10)+0x30);
	EINK_WriteFont(QRbuf,96+16,126,(pMain->TempCorrectValue / 1000 % 10)+0x30);
	EINK_WriteFont(QRbuf,104+16,126,(pMain->TempCorrectValue / 100 % 10)+0x30);
	EINK_WriteFont(QRbuf,112+16,126,(pMain->TempCorrectValue / 10 % 10)+0x30);
	EINK_WriteFont(QRbuf,120+16,126,(pMain->TempCorrectValue % 10)+0x30);
*/
	EINK_WriteString(QRbuf,56-40,126,"Intv.");
	EINK_WriteFont(QRbuf,56+8,126,(pMain->Sensor.SensorIntervalSec/60 / 100 % 10)+0x30);
	EINK_WriteFont(QRbuf,64+8,126,(pMain->Sensor.SensorIntervalSec/60 / 10 % 10)+0x30);
	EINK_WriteFont(QRbuf,72+8,126,(pMain->Sensor.SensorIntervalSec/60 % 10)+0x30);
	EINK_WriteIcon(QRbuf,176-42,176-142,17);
uint8 AAAA=16;
	EINK_WriteIcon(QRbuf,176-34,176-142,AAAA);

	EINK_WriteIcon(QRbuf,176-26,176-142,14);

	//EINK_WriteIcon(QrCode,32,142,0x05);
	//EEPROM_WriteDevInfo.Serial(pLogData->DevInfo.Serial);

	BitShifter(QRbuf,2887,2);
#else
	memset(QRbuf,0x00,2888);
	for(int i=0;i<21;i++)
	{
		BitCopy(QRbuf,((i+6)*176)*2+88,SampleCode,i*21,21);
		BitCopy(QRbuf,((i+6)*176)*2+88+176,SampleCode,i*21,21);
		BitDoubler(QRbuf,((i+6)*22)*2+11,3);
		BitDoubler(QRbuf,((i+6)*22)*2+22+11,3);
	}
	BitShifter(QRbuf,2887,8);

	EINK_WriteFont(QRbuf,64-40,14,pMain->DevInfo.Serial[0]);
	EINK_WriteFont(QRbuf,72-40,14,pMain->DevInfo.Serial[1]);
	EINK_WriteFont(QRbuf,80-40,14,pMain->DevInfo.Serial[2]);
	EINK_WriteFont(QRbuf,88-40,14,pMain->DevInfo.Serial[3]);
	EINK_WriteFont(QRbuf,96-40,14,pMain->DevInfo.Serial[4]);
	EINK_WriteFont(QRbuf,104-40,14,pMain->DevInfo.Serial[5]);
	EINK_WriteFont(QRbuf,112-40,14,pMain->DevInfo.Serial[6]);
	EINK_WriteFont(QRbuf,120-40,14,pMain->DevInfo.Serial[7]);

	EINK_WriteString(QRbuf,0,88,"Press button 3times");
	EINK_WriteString(QRbuf,0,98,"TO START");

	EINK_WriteString(QRbuf,56-56 + 24,37,VersionString);

	/*
	EINK_WriteFont(QRbuf,88+16,126,(pMain->TempCorrectValue / 10000 % 10)+0x30);
	EINK_WriteFont(QRbuf,96+16,126,(pMain->TempCorrectValue / 1000 % 10)+0x30);
	EINK_WriteFont(QRbuf,104+16,126,(pMain->TempCorrectValue / 100 % 10)+0x30);
	EINK_WriteFont(QRbuf,112+16,126,(pMain->TempCorrectValue / 10 % 10)+0x30);
	EINK_WriteFont(QRbuf,120+16,126,(pMain->TempCorrectValue % 10)+0x30);
*/
	EINK_WriteString(QRbuf,56-40,25,"Intv.");
	EINK_WriteFont(QRbuf,56+8,25,(pMain->Sensor.SensorIntervalSec/60 / 100 % 10)+0x30);
	EINK_WriteFont(QRbuf,64+8,25,(pMain->Sensor.SensorIntervalSec/60 / 10 % 10)+0x30);
	EINK_WriteFont(QRbuf,72+8,25,(pMain->Sensor.SensorIntervalSec/60 % 10)+0x30);
	EINK_WriteIcon(QRbuf,16,130,0x02);

	EINK_WriteIcon(QRbuf,24,130,0x05);

	EINK_WriteIcon(QRbuf,32,130,0x03);

	//EINK_WriteIcon(QrCode,32,142,0x05);
	EEPROM_WriteSerialNumber(pMain->DevInfo.Serial);

	BitShifter(QRbuf,2887,2);
	#endif

EPD_Display(pMain,QRbuf,1,0);
}
void QRLogger_SetTime(stMain *pMain)
{
	uint32 *p;
	p=(uint32*)0x00076E0;

#if 1
    // edit point
    /*
		pMain->Clock.Date.Year=0;
		pMain->Clock.Date.Month=0;
		pMain->Clock.Date.Date=0;
		pMain->Clock.Time.Hours=0;
		pMain->Clock.Time.Minutes=0;
		pMain->Clock.Time.Seconds=0;

		HAL_RTC_SetDate(pMain->Clock.Handler,&pMain->Clock.Date,RTC_FORMAT_BIN);
		HAL_RTC_SetTime(pMain->Clock.Handler,&pMain->Clock.Time,RTC_FORMAT_BIN);
*/
#else
	if(*p)
	{
		pMain->Clock.Date.Year=((*p / 0x10)*10 + *p % 0x10);
		p=(uint32*)0x000079D4;
		pMain->Clock.Date.Month=((*p / 0x10)*10 + *p % 0x10);
		p=(uint32*)0x000079D8;
		pMain->Clock.Date.Date=((*p / 0x10)*10 + *p % 0x10);
		//pMain->Clock.Date.WeekDay=21;
		p=(uint32*)0x000079E0;
		pMain->Clock.Time.Hours=((*p / 0x10)*10 + *p % 0x10);
		p=(uint32*)0x000079E4;
		pMain->Clock.Time.Minutes=((*p / 0x10)*10 + *p % 0x10);
		p=(uint32*)0x000079E8;
		pMain->Clock.Time.Seconds=((*p / 0x10)*10 + *p % 0x10);

		HAL_RTC_SetDate(pMain->Clock.Handler,&pMain->Clock.Date,RTC_FORMAT_BIN);
		HAL_RTC_SetTime(pMain->Clock.Handler,&pMain->Clock.Time,RTC_FORMAT_BIN);
		for(int i=0;i<8;i++)
		{
			EEPROM_Internal_WriteBeforeThread(0x0007960+i*4,0x00000000);		// erase flash
		}
	}
#endif
}

void QRLogger_GetTime(stMain *pMain)
{
//edit point

	  //HAL_RTC_GetDate(pMain->Clock.Handler,&pMain->Clock.Date,RTC_FORMAT_BIN);
	  //HAL_RTC_GetTime(pMain->Clock.Handler,&pMain->Clock.Time,RTC_FORMAT_BIN);
#ifdef UpTimeMode
	  //pData->UpTimeSec=pMain->Clock.UpTimeSec;
#else
	  pData->Year=pMain->Clock.Date.Year;
	  pData->Month=pMain->Clock.Date.Month;
	  pData->Days=pMain->Clock.Date.Date;
	  pData->Hour=pMain->Clock.Time.Hours;
	  pData->Minute=pMain->Clock.Time.Minutes;
	  pData->Seconds=pMain->Clock.Time.Seconds;
#endif
}


void QRLogger_SetWakeupTimer(stMain *pMain)
{
//edit point
}

void QRLogger_UpdateFTemp(stMain *pMain)
{

}


uint8 QRLogger_HextoAscii(uint8 hex)
{
	if(hex<10) return hex + 48;
	else return hex + 55;
}

void QRLogger_DisplayFail(stMain *pMain, uint16 Length)
{
	uint8 QRCodeBuf[2888];
	memset(QRCodeBuf,0x00,2888);
	//DisplayEnable(pMain,QRCodeBuf);
	EINK_WriteString(QRCodeBuf,8,12,"Huffman length");
	EINK_WriteString(QRCodeBuf,8,24,"over limit");
	EINK_WriteFont(QRCodeBuf,8,36,Length/1000%10 + 0x30);
	EINK_WriteFont(QRCodeBuf,16,36,Length/100%10 + 0x30);
	EINK_WriteFont(QRCodeBuf,24,36,Length/10%10 + 0x30);
	EINK_WriteFont(QRCodeBuf,32,36,Length%10 + 0x30);
	EPD_Display(pMain,QRCodeBuf,1,1);

}
/*

void QRLogger_DisplayURL(stMain *pMain, uint8 *Huffman,uint16 length)
{
	uint8 QRCodeBuf[2888];
	uint8 ActiveQRVersion=11;
	memset(QRCodeBuf,0x00,2888);
	//uint8 URLBuf
	uint8 *p = calloc(length *2 + sizeof(HuffmanURL),1);
	uint8 *pp = &p[sizeof(HuffmanURL)-1];

#ifdef DualQR
	if(pMain->KeyPushCount == 2)
	{
		ActiveQRVersion=DefineQRVersion;
	}
#else
	ActiveQRVersion=DefineQRVersion;
#endif

	memcpy(p,HuffmanURL,sizeof(HuffmanURL));



	for(int i=0;i<length;i++)
	{
		*pp = QRLogger_HextoAscii((*Huffman) >>4 & 0x0f);
		pp++;
		*pp = QRLogger_HextoAscii((*Huffman) & 0x0f);
		pp++;
		Huffman++;
	}
	qrcode_initBytes(&QRtmp,QRCodeBuf,ActiveQRVersion,0,p,length*2 + sizeof(HuffmanURL) -1);

	DisplayEnable(pMain,QRCodeBuf,ActiveQRVersion);


	EPD_Display(pMain,QRCodeBuf,0,0);
}
*/
uint8 QRLogger_ViewTemp(stMain *pMain)
{
float TempValue=0;
//uint8 *QRCodeBuf=NULL;
uint8 QRCodeBuf[2888];
sint16 LastTempBackup=0;
sint16 OBSensorTemp=0;

memset(QRCodeBuf,0x00,2888);
#ifdef Sensing_P							// P 버전 �웨
ProbeGetADCCapture(pMain);
GetProbeTemp(pMain,&TempValue);
pMain->Sensor.PCBTemp = TempValue * 100;
#elif defined Sensing_O						// O 버전 �웨(�로븬용 �함)
	//pMain->Sensor.PCBTemp = pMain->Sensor.PCBTemp;			// �수�째�리까� �용
#endif
Display_Correction(pMain,QRCodeBuf);
return 0;
}

uint8 QRLogger_TaskProcess(stMain *pMain)
{
	uint8 QRCount;
	uint8 DisplayRefreshFlag=0;
	float ProbeTemp=0;
	EEPROM_GetPrevTemp(&pMain->Sensor.Prev.PCBTemp);					// �전 �도 로드
	EEPROM_GetPrevHumi(&pMain->Sensor.Prev.PCBHumi);					// �전 �도 로드
	EEPROM_GetNumberOfRecord(&pMain->Sensor.DataNumber);		// �이�갯로드
	EEPROM_GetCurrentPage(&pMain->Page.CurrentPage);			// 마�맜시 �이지 로드
#ifdef Sensing_P							// P 버전 �웨
	ProbeGetADCCapture(pMain);				// ADC��가�오��
	GetProbeTemp(pMain,&ProbeTemp);			// �로븨도계산
	pMain->Sensor.PCBTemp = ProbeTemp * 100;			// �수�째�리까� �용
#elif defined Sensing_O						// O 버전 �웨(�로븬용 �함)
	//pMain->Sensor.PCBTemp = pMain->Sensor.PCBTemp;			// �수�째�리까� �용
#endif
// edit point
/*
	if((pMain->Sensor.PCBTemp < 850) && (pMain->Sensor.PCBTemp > 150))		// �로븨도가 2~ 8�이��(반올��
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);  //LED 켜기
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);		// LED �기
	}
*/
	if(pMain->Key.WakeupByKeyPush)		// 버튼�러 wake up �는지 �인
	{								// 버튼�러 wake up 경우
		//EEPROM_GetSubStatus(&pMain->SubStatus);
#ifdef Service_L
		if(pMain->KeyPushCount==1)
		{
			if(pMain->CurrentPage==0)
			{
				pMain->CurrentPage=1;
				pMain->SubStatus=0;
			}
			else if(pMain->CurrentPage==1)
			{
				pMain->CurrentPage=0;
				pMain->SubStatus=1;
			}
			else
			{
				return 0;
			}
		}
		else if(pMain->KeyPushCount==2)
		{
			pMain->CurrentPage++;
			if(pMain->CurrentPage>3)
			{
				pMain->CurrentPage=0;
				pMain->SubStatus=1;
			}
			else
			{
				pMain->SubStatus=0;
			}

		}
		EEPROM_WriteCurrentPage(&pMain->CurrentPage);
#else
		if(pMain->Key.KeyPushCount>1)
		{
			//pMain->SubStatus=0;
		}
#endif
		if(1)		// edit point
                //if(pMain->SubStatus)		// QR�시 �래귕인
		{							// QR�시 �래규성�태경우
			uint8 QRCodeBuf[2888];
			memset(QRCodeBuf,0x00,2888);
			EEPROM_WriteSubStatus(0);	// QR�시 �래��비활�화 �
			//Display_LargeNumber(pMain,QRCodeBuf,1);		// �스�레� 리프�시
			EEPROM_GetEPDRefresh_QR(&QRCount);
			if(QRCount>100)
			{
				EEPROM_ClearEPDRefresh_QR();
				Display_LargeNumber(pMain,QRCodeBuf,1);				// � 리프�시
			}
			else
			{
				SetLongLUT();
				Display_LargeNumber(pMain,QRCodeBuf,0);				// 리프�시
			}
			return 0;

		}
		else
		{							// QR�시 �래��비활�화 �태경우
			EEPROM_WriteSubStatus(1);	// QR�시 �래규성�
			RtcGetTimeSec(&pMain->Device.Time.UpTimeSec);		// ��기록
			Init_RecordStructure(pMain);			// 메��이�성
			return 1;									// 2-2-D-3�로 �동 (�프만코��QR �시)
		}

	}
	else							// �터��wakeup
	{
		uint8 QRCodeBuf[2888];
		memset(QRCodeBuf,0x00,2888);
		//EEPROM_GetSubStatus(&pMain->SubStatus);
		//RtcGetTimeSec(&pMain->Clock.UpTimeSec);						// edit point
		Init_RecordStructure(pMain);							// 메��이�성
#ifdef Recording_H
		EEPROM_WriteSerialData(pMain->Sensor.PCBTemp,pMain->Sensor.PCBHumi);					// �이기록(�재�도)
#elif defined Recording_A
		EEPROM_WriteSerialData(pMain->Sensor.PCBTemp,pMain->Sensor.PCBHumi);					// �이기록(�재�도)
#else
		EEPROM_WriteSerialData(pMain->Sensor.PCBTemp);					// �이기록(�재�도)
#endif
		EEPROM_WriteNumberOfRecord(&pMain->Sensor.DataNumber);		// �이�갯�
		EEPROM_WritePrevTemp(pMain->Sensor.PCBTemp);					// �도 �
		EEPROM_WritePrevHumi(pMain->Sensor.PCBHumi);					// �도 �
		//EEPROM_WriteSubStatus(0);									// QR�시 �래��비활�화 �
		pMain->Page.CurrentPage=0;										// �도�면 �시경우 �이지 0�로 고정
		EEPROM_WriteCurrentPage(&pMain->Page.CurrentPage);				// �이지 �
		uint8 QRCount=0;
		if(1)       // edit point
                //if(pMain->SubStatus)
		{
			EEPROM_ClearEPDRefresh_QR();
			EEPROM_GetEPDRefresh_QR(&QRCount);
			if(QRCount>100)
			{
				EEPROM_ClearEPDRefresh_QR();
				Display_LargeNumber(pMain,QRCodeBuf,1);				// � 리프�시
			}
			else
			{
				SetLongLUT();
				Display_LargeNumber(pMain,QRCodeBuf,0);				// 리프�시
			}
			EEPROM_WriteLastDataNumber(0);
		}
		else
		{
			EEPROM_GetEPDRefresh_QR(&QRCount);
	#ifndef OORModeEnable
			DisplayRefreshFlag = 1 ;
	#else
			if(!((pMain->Sensor.PCBTemp > 1550) || (pMain->Sensor.PCBTemp < 0)))			// �재 �도가 0�상 15�하�때
			{
				DisplayRefreshFlag = 1 ;
			}
			else if(!((pMain->PrevTemp > 1550) || (pMain->PrevTemp < 0)))			// �전 �도가 0�상 15�하�때
			{
				pMain->PrevTemp = pMain->Sensor.PCBTemp - 100;							// �수�리 차이 발생 조건 강제 발생.
																					// �전 �도, �재�도 모두 15경우 OOR�면갱신 �하�상 발생
				DisplayRefreshFlag = 1 ;
				QRCount=101;														// full refresh �용

			}

	#endif
			if(DisplayRefreshFlag)
			{
#ifdef Recording_H
				if(pMain->Sensor.PCBHumi != pMain->Sensor.Prev.PCBHumi)			// �수�리 차이 발생
#else
				if(pMain->Sensor.PCBTemp/100 != pMain->Sensor.Prev.PCBTemp/100)			// �수�리 차이 발생
#endif
				{
					if(pMain->Sensor.PCBTemp > -1000)								// PCB �도�서가 -10�상경우�만
					{

						if(QRCount>100)
						{
							EEPROM_ClearEPDRefresh_QR();
							Display_LargeNumber(pMain,QRCodeBuf,1);				// � 리프�시
						}
						else
						{
							EEPROM_AddEPDRefresh_QR();
							SetLUT();
							Display_LargeNumber(pMain,QRCodeBuf,0);				// 리프�시
						}
					}
					else
					{
						//EPD_sleep();
					}
				}
				else
				{
					//EPD_sleep();
				}
				EEPROM_WriteLastDataNumber(0);
			}
		}
		return 0;
	}
}

/*      // edit point
void QRLogger_SetPointer(stMain *pMain,stQRLoggerData *p,RTC_HandleTypeDef *RTCHand)
{
	RTCHandle=RTCHand;
	pLogData=p;
}
*/

void QRLogger_Init(stMain *pMain)
{
	RunStat RunningStat;
	uint32 *pp;

// edit point
	//HAL_RTCEx_DeactivateWakeUpTimer(RTCHandle);
	//Init_LC709203(pMain,i2cHandle);
	Init_MC3479(pMain,i2cHandle);
	pp=(uint32*)TimeIntervalAddress;


	pMain->Sensor.DataNumber=0;

	//EEPROM_GetIntervalCount(&pMain->IntervalCount);
	EEPROM_WriteNumberOfRecord(&pMain->Sensor.DataNumber);

	pMain->Sensor.Prev.PCBTemp=0;
	pMain->Sensor.Prev.PCBHumi=0;
	EEPROM_WritePrevTemp(pMain->Sensor.Prev.PCBTemp);
	EEPROM_WritePrevHumi(pMain->Sensor.Prev.PCBHumi);

	RunningStat=RunStat_LoggingIdle;
	EEPROM_WriteRunningStat(RunningStat);

	//Display_ClearEEPROM(pMain);
	//EPD_Display(QRCodeBuf);
	for(int i=0;i<0x7fff/4;i++)
	{
	//EEPROM_WriteData(&dummy,i*4,4);
	}

	ProbeCorrectionSequence(pMain);
	//GetDevInfo.SerialFromUID(pMain);
	EEPROM_WriteSerialNumber(pMain->DevInfo.Serial);
/*
	p->DevInfo.Serial[0]='1';
	p->DevInfo.Serial[1]='0';
	p->DevInfo.Serial[2]='3';
	p->DevInfo.Serial[3]='0';
	p->DevInfo.Serial[4]='1';
	p->DevInfo.Serial[5]='0';
	p->DevInfo.Serial[6]='1';
	p->DevInfo.Serial[7]='0';
*/

	EEPROM_WriteDataLength(0);
	Display_Factory(pMain);
	HAL_Delay(100);



	ClearPowerFlag();
	//HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
        //edit point
	//HAL_RTCEx_SetWakeUpTimer_IT(RTCHandle, 1, RTC_WAKEUPCLOCK_RTCCLK_DIV16);		// 32768 / 16 = 2048Hz
	EnterStandbyMode(pMain);
}

