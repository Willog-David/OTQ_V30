/*
 * Display.c
 *
 *  Created on: 2021. 1. 15.
 *      Author: kaohu
 */

#include "display.h"
#include "task.h"
#include "Display_EPD_W21.h"
#include "math.h"

extern QRCode QRtmp;
const uint8 WillogURL[]="HTTP://WILLOG.IO/";

#ifdef Sensing_P
const unsigned char VersionInform_1 = 'P';
#elif defined Sensing_O
const unsigned char VersionInform_1 = 'O';
#endif

#ifdef Recording_T
const unsigned char VersionInform_2 = 'T';
#elif defined Recording_H
const unsigned char VersionInform_2 = 'H';
#elif defined Recording_A
const unsigned char VersionInform_2 = 'A';
#elif defined Recording_S
const unsigned char VersionInform_2 = 'S';
#elif defined Recording_F
const unsigned char VersionInform_2 = 'F';
#endif

//const unsigned char VersionInform_3 = 'T';
#ifdef Scale_O
const unsigned char VersionInform_3 = 'O';
#elif defined Scale_I
const unsigned char VersionInform_3 = 'I';
#elif defined Scale_S
const unsigned char VersionInform_3 = 'S';
#endif

#ifdef Service_L
const unsigned char VersionInform_4 = 'L';
#elif defined Service_M
const unsigned char VersionInform_4 = 'M';
#elif defined Service_B
const unsigned char VersionInform_4 = 'B';
#elif defined Service_P
const unsigned char VersionInform_4 = 'P';
#endif

#ifdef DEBUG
 #ifdef ForceDataEnable
	#ifdef BLESleepEnable 
	 const unsigned char VersionInform_5 = 'F';		// EEPROM force download FW
	#else
	 const unsigned char VersionInform_5 = 'G';
	#endif
	#if (ForceIndexValue == 7199)
		const unsigned char VersionInform_6 = '1';
	#elif (ForceIndexValue == 14399)
		const unsigned char VersionInform_6 = '2';
	#elif (ForceIndexValue == 21599)
		const unsigned char VersionInform_6 = '3';
	#elif (ForceIndexValue == 28799)
		const unsigned char VersionInform_6 = '4';
	#endif
 #else
	#ifdef BLESleepEnable 
	 const unsigned char VersionInform_5 = 'D';
	#else
	 const unsigned char VersionInform_5 = 'B';
	#endif
 const unsigned char VersionInform_6 = '0';
 #endif
#else
const unsigned char VersionInform_5 = '2';
const unsigned char VersionInform_6 = '0';
#endif


const unsigned char stVersionString[11]=
	{VersionInform_1,VersionInform_2,VersionInform_3,VersionInform_4,VersionInform_5,VersionInform_6,'0','0','2'};

const unsigned char stSubVersionString[]={"0001"};



void Init_RecordStructure(stMain *pMain)
{
if(SizeOfSerial == 10)
	{
	pMain->pLogData->Start=0x3F55;
	}
else
	{
	pMain->pLogData->Start=0xAA55;
	//pMain->pLogData->Start=0x4055;
	}

pMain->pLogData->TimeIntervalSec=pMain->Sensor.SensorIntervalSec;
if(!pMain->pLogData->TimeIntervalSec) pMain->pLogData->TimeIntervalSec = 60;
pMain->pLogData->PageNumber=pMain->Page.CurrentPage;

pMain->pLogData->PageNumber |= MaxPageRef<<4;

pMain->pLogData->Time = pMain->Device.Time.UpTimeSec;
//if(!pMain->pLogData->Time) pMain->pLogData->Time = 0x11223344;
if(SizeOfSerial == 10)
	{
	memcpy(pMain->pLogData->SerialNumber,pMain->DevInfo.Serial,10);
	}
else
	{
	memcpy(pMain->pLogData->SerialNumber,pMain->DevInfo.Serial,8);
	}

pMain->pLogData->BatteryGauge = Batt_GetVoltage_Percent(pMain);

#ifdef Recording_A
pMain->pLogData->type=0xAD;
#elif defined Scale_I
pMain->pLogData->type=0x6A;
#elif defined Recording_F
pMain->pLogData->type=0xAF;
#else
//pMain->pLogData->type=0x5A;
pMain->pLogData->type=0xAD;		// reverse data syntax
#endif

if(SizeOfSerial == 10)
	{
	//pMain->pLogData->End=0x553F;
	pMain->pLogData->End=0x5540;			// QR syntax 1.2
	}
else
	{
	pMain->pLogData->End=0x55AA;
	}
}

void OTQ_GetValidTempCount(stMain *pMain,stLogData *pData)
{
//uint16 ValidDataCount=pData->NumberOfData;
uint16 ValidDataCount=0;
for(int i=0;i<pData->LastIndex;i++)
	{
	if(pData->Data[i] == 0xff)
		{

		}
	else if(pData->Data[i] & 0x80)
		{
		ValidDataCount++;
		i++;
		}
	else
		{
		ValidDataCount++;
		}
	}
pMain->Page.ValidDataCount = ValidDataCount;
}


uint16 IVLC_DataErase(stMain *pMain,uint8 *pData, uint8 EraseNumber, uint16 EraseLimitLength,uint16 *EraseCount)
{
uint8 *p=pData;
uint8 EraseData=0;
//*EraseCount=0;
for(int i=1 ; i<EraseLimitLength ; i++)
	{
	if(*p != 0x3f)
		{
		if(*p == 0xff)
		{
			pMain->Page.JumpCount-=1;
		}
		else if(*p!=0xff)
		{
			EraseData++;
			*EraseCount +=1;
		}
		*p=0x3f;
		}
	p--;
	if(*p & 0x80)
		{
		if(*p == 0xff)
		{
			pMain->Page.JumpCount-=1;
		}
		*p = 0x3f;
		//EraseData++;
		*EraseCount +=1;
		//i++;
		}

	if(EraseData >= EraseNumber)
		{
		if(i%2)		// i ��? ???��?��?�� = ?��?��?�� 짝수?��?��
		{
			if(p[-1] & 0x80)
			{
				p[-1] = 0x3f;
				i++;
			}
		}

		p++;
		*p=0xaa;
		p++;
		*p=0x55;
		return i;
		}
	}
return 0;
}


void Display_Temperature_OnLeft(stMain *pMain,uint8 *QrCode)
{
	  if(pMain->Sensor.PCBTemp>10000)EINK_WriteFont(QrCode,0,0,(pMain->Sensor.PCBTemp/10000)+0x30);
	  if(pMain->Sensor.PCBTemp<0)EINK_WriteFont(QrCode,0,0,'-');
	  EINK_WriteFont(QrCode,7,0,(abs(pMain->Sensor.PCBTemp)/1000%10)+0x30);
	  EINK_WriteFont(QrCode,14,0,(abs(pMain->Sensor.PCBTemp)/100%10)+0x30);
	  EINK_WriteString(QrCode,21,0,".");
	  EINK_WriteFont(QrCode,28,0,(abs(pMain->Sensor.PCBTemp)/10%10)+0x30);
	  EINK_WriteString(QrCode,35,0,"'");
	  EINK_WriteString(QrCode,40,0,"C");
}

void Display_Temperature(stMain *pMain,uint8 *QrCode)
{
	  if(pMain->Sensor.PCBTemp>10000)EINK_WriteFont(QrCode,100,0,(pMain->Sensor.PCBTemp/10000)+0x30);
	  if(pMain->Sensor.PCBTemp<0)EINK_WriteFont(QrCode,100,0,'-');
	  EINK_WriteFont(QrCode,107,0,(abs(pMain->Sensor.PCBTemp)/1000%10)+0x30);
	  EINK_WriteFont(QrCode,114,0,(abs(pMain->Sensor.PCBTemp)/100%10)+0x30);
	  EINK_WriteString(QrCode,121,0,".");
	  EINK_WriteFont(QrCode,128,0,(abs(pMain->Sensor.PCBTemp)/10%10)+0x30);
	  EINK_WriteString(QrCode,135,0,"'");
	  EINK_WriteString(QrCode,140,0,"C");

#if 0
	  //EINK_WriteString(QrCode,0,140,"TEMP: ");
	  if(pMain->CurrentFTemp>10000)EINK_WriteFont(QrCode,49,1,(pMain->CurrentFTemp/10000)+0x30);
	  if(pMain->CurrentFTemp<0)EINK_WriteFoont(QrCode,49,1,'-');
	  EINK_WriteFont(QrCode,56,1,(abs(pMain->CurrentFTemp)/1000%10)+0x30);
	  EINK_WriteFont(QrCode,63,1,(abs(pMain->CurrentFTemp)/100%10)+0x30);
	  EINK_WriteString(QrCode,70,1,".");
	  EINK_WriteFont(QrCode,77,1,(abs(pMain->CurrentFTemp)/10%10)+0x30);
	  EINK_WriteString(QrCode,84,1,"'");
	  EINK_WriteString(QrCode,89,1,"F");
#endif
}

void Display_Humidity(stMain *pMain,uint8 *QrCode)
{
	  EINK_WriteFont(QrCode,60,0,(pMain->Sensor.PCBHumi/10%10)+0x30);
	  EINK_WriteFont(QrCode,67,0,(pMain->Sensor.PCBHumi%10)+0x30);
	  EINK_WriteFont(QrCode,74,0,'%');
}


void Display_UpTime(stMain *pMain,uint8 *QrCode)
{
#if 1
	uint32 UptimeSec = pMain->Device.Time.UpTimeSec;
	EINK_WriteFontVertical(QrCode,140,143,UptimeSec / 10000000 % 10 + 0x30);
	EINK_WriteFontVertical(QrCode,140,136,UptimeSec / 1000000  % 10 + 0x30);
	EINK_WriteFontVertical(QrCode,140,129,UptimeSec / 100000   % 10 + 0x30);
	EINK_WriteFontVertical(QrCode,140,122,UptimeSec / 10000    % 10 + 0x30);
	EINK_WriteFontVertical(QrCode,140,115,UptimeSec / 1000     % 10 + 0x30);
	EINK_WriteFontVertical(QrCode,140,108,UptimeSec / 100      % 10 + 0x30);
	EINK_WriteFontVertical(QrCode,140,101,UptimeSec / 10       % 10 + 0x30);
	EINK_WriteFontVertical(QrCode,140, 94,UptimeSec / 1        % 10 + 0x30);

	  #else
	uint8 Hour=0,Minute=0;
	Hour   = pMain->Clock.UpTimeSec / 3600;
	Minute = (pMain->Clock.UpTimeSec % 3600) / 60;
	//EINK_WriteFontVertical(QrCode,140,73,'T');
	  EINK_WriteFontVertical(QrCode,140,73,Hour / 10 + 0x30);
	  EINK_WriteFontVertical(QrCode,140,66,Hour % 10 + 0x30);
	  EINK_WriteFontVertical(QrCode,140,59,':');
	  EINK_WriteFontVertical(QrCode,140,52,Minute / 10 + 0x30);
	  EINK_WriteFontVertical(QrCode,140,45,Minute % 10 + 0x30);
#endif
}


void Display_Page(stMain *pMain,uint8 *QrCode)
{
	uint8 TotalPage=0,CurrentPage=0;
	TotalPage=pMain->Page.MaxPage;
	CurrentPage=pMain->Page.CurrentPage;
	//CurrentPage=pMain->ViewPage;
	#if 1
	//EINK_WriteFont(QrCode,71,252,(CurrentPage/10)+0x30);
	if(CurrentPage >= 10)
		{
			EINK_WriteFont(QrCode,57,252,(CurrentPage/10)+0x30);
		}

	EINK_WriteFont(QrCode,64,252,(CurrentPage%10)+0x30);
	EINK_WriteString(QrCode,71,252,"Page");
	//EINK_WriteFont(QrCode,85,252,'/');
	//EINK_WriteFont(QrCode,92,252,(TotalPage/10)+0x30);
	//EINK_WriteFont(QrCode,99,252,(TotalPage%10)+0x30);

        #else
	EINK_WriteFontVertical(QrCode,140,9,(TotalPage%10)+0x30);
	EINK_WriteFontVertical(QrCode,140,16,(TotalPage/10)+0x30);
	EINK_WriteFontVertical(QrCode,140,23,'/');
	EINK_WriteFontVertical(QrCode,140,30,(CurrentPage%10)+0x30);
	EINK_WriteFontVertical(QrCode,140,37,(CurrentPage/10)+0x30);
        #endif
}

void Display_ButtonGuide(stMain *pMain,uint8 *QrCode)
{
	uint8 TotalPage=0,CurrentPage=0;
	TotalPage=3;
	CurrentPage=pMain->Page.CurrentPage;
	//CurrentPage=pMain->ViewPage;
        #if 1
        EINK_WriteString(QrCode,0,252,"Refresh");
        EINK_WriteString(QrCode,144,252,"Next");

        #else
	EINK_WriteFontVertical(QrCode,140,9,(TotalPage%10)+0x30);
	EINK_WriteFontVertical(QrCode,140,16,(TotalPage/10)+0x30);
	EINK_WriteFontVertical(QrCode,140,23,'/');
	EINK_WriteFontVertical(QrCode,140,30,(CurrentPage%10)+0x30);
	EINK_WriteFontVertical(QrCode,140,37,(CurrentPage/10)+0x30);
        #endif
}

void Display_Init(stMain *pMain, uint8 *QrCode)
{
	EINK_WriteString(QrCode,8,70,"Willog Initialize");
	EINK_WriteString(QrCode,8,82,"Please Wait");
	EPD_Display(pMain,QrCode,1,0);
}

void Display_Main(stMain *pMain)
{
	uint8 QrCode[2888];
}

void Display_Calib1(stMain *pMain,uint8 *QRbuf,uint16 ADCValue,uint8 Success)
{
	uint8 Number=0;

	EEPROM_GetCalibDataNumber(&Number);

	EINK_WriteString(QRbuf,0+0,12,"2. Self-calib step");
	EINK_WriteString(QRbuf,0+8,36,"ADC Value : ");
	//EINK_WriteString(QRbuf,0+8,72,"Check Seq : 00/10");

	EINK_WriteFont(QRbuf,96+0,36,(ADCValue / 1000 % 10)+0x30);
	EINK_WriteFont(QRbuf,96+8,36,(ADCValue / 100 % 10)+0x30);
	EINK_WriteFont(QRbuf,96+16,36,(ADCValue / 10 % 10)+0x30);
	EINK_WriteFont(QRbuf,96+24,36,(ADCValue % 10)+0x30);

	//EINK_WriteFont(QRbuf,96+8,72,((Number+1) /10 % 10)+0x30);
	//EINK_WriteFont(QRbuf,96+16,72,((Number+1) % 10)+0x30);

	if(Success==1)
	{
		EINK_WriteString(QRbuf,0+8,60,"    OKOK");
		EINK_WriteString(QRbuf,0+8,72,"    OKOK");
	}
	else if(Success==2)
	{
		EINK_WriteString(QRbuf,0+8,60,"    FAIL");
		EINK_WriteString(QRbuf,0+8,72,"    FAIL");
	}
	else
	{
		EINK_WriteString(QRbuf,0+8,60,"In Calibration");
	}

		EINK_WriteFont(QRbuf,64-40,152-51,pMain->DevInfo.Serial[0]);
		EINK_WriteFont(QRbuf,72-40,152-51,pMain->DevInfo.Serial[1]);
		EINK_WriteFont(QRbuf,80-40,152-51,pMain->DevInfo.Serial[2]);
		EINK_WriteFont(QRbuf,88-40,152-51,pMain->DevInfo.Serial[3]);
		EINK_WriteFont(QRbuf,96-40,152-51,pMain->DevInfo.Serial[4]);
		EINK_WriteFont(QRbuf,104-40,152-51,pMain->DevInfo.Serial[5]);
		EINK_WriteFont(QRbuf,112-40,152-51,pMain->DevInfo.Serial[6]);
		EINK_WriteFont(QRbuf,120-40,152-51,pMain->DevInfo.Serial[7]);
		EINK_WriteFont(QRbuf,128-40,152-51,pMain->DevInfo.Serial[8]);
		EINK_WriteFont(QRbuf,136-40,152-51,pMain->DevInfo.Serial[9]);
		//EINK_WriteString(QRbuf,8+24,94-36,"pressing this");
		//EINK_WriteString(QRbuf,8+32,106-36,"button twice");
	#ifdef THD
		EINK_WriteString(QRbuf,56-56,114,"THUR023 ");
	#elif defined TD
		EINK_WriteString(QRbuf,56-56 + 24,114,stVersionString);
	#endif
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
		}		//EINK_WriteIcon(QrCode,32,142,0x05);
		//EEPROM_WriteSerialNumber(pLogData->SerialNumber);

		BitShifter(QRbuf,2887,2);
EPD_Display(pMain,QRbuf,0,0);
}

void EPD_DisplayURL(stMain *pMain,uint8 *QRCodeBuf, uint8 *Huffman,uint16 length)
{
	//uint8 ActiveQRVersion=17;
        uint8 ActiveQRVersion=15;
	//uint8 URLBuf
	uint8 *p = calloc(length *2 + sizeof(WillogURL),1);
	uint8 *pp = &p[sizeof(WillogURL)-1];

#ifdef DualQR
	if(pMain->Key.KeyPushCount == 2)
	{
		ActiveQRVersion=DefineQRVersion;
	}
#else
	ActiveQRVersion=DefineQRVersion;
#endif

	memcpy(p,WillogURL,sizeof(WillogURL));



	for(int i=0;i<length;i++)
	{
		*pp = QRLogger_HextoAscii((*Huffman) >>4 & 0x0f);
		pp++;
		*pp = QRLogger_HextoAscii((*Huffman) & 0x0f);
		pp++;
		Huffman++;
	}
	qrcode_initBytes(&QRtmp,QRCodeBuf,ActiveQRVersion,0,p,length*2 + sizeof(WillogURL) -1);

	//DisplayEnable(pMain,QRCodeBuf,ActiveQRVersion);

}


