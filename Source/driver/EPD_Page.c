#include "EPD_Page.h"
#include "BatGauge.h"
#include "huffman.h"
#include "EEPROM.h"
#include "Accelo.h"
#include "Display.h"
#include "Probe.h"
#include "LOGO.h"
#include "OTQV30_BLEservice.h"


extern QRCode QRtmp;
extern const unsigned char stVersionString[];
extern const unsigned char WillogLogo1[];

#ifdef Service_L
//const uint8 HuffmanURL[]="b.willog.io/otq/";
const uint8 HuffmanURL[]="HTTP://L.OTQ.KR/A/";
#elif defined Service_M
const uint8 HuffmanURL[]="HTTP://M.OTQ.KR/A/";
#elif defined Service_B
const uint8 HuffmanURL[]="HTTP://B.OTQ.KR/A/";
#elif defined Service_P
const uint8 HuffmanURL[]="HTTP://P.OTQ.KR/A/";
//const uint8 HuffmanURL[]="HTTP://P-DEV.OTQ.KR/A/";


#endif

#define DefaultQRVersion 16

uint8 DefineQRVersion = DefaultQRVersion;
#if (DefaultQRVersion == 16)
	uint16 QR_HuffmanURL_LENGTH_MAX = 854/2 - 5 - sizeof(HuffmanURL);    // QR v16
	uint16 QR_Base41URL_LENGTH_MAX = (854 - 5 - sizeof(HuffmanURL))*2/3;    // QR v16
	#ifdef Base41Encoding
		#define QR_URL_LENGTH_MAX QR_Base41URL_LENGTH_MAX
	#else
		#define QR_URL_LENGTH_MAX QR_HuffmanURL_LENGTH_MAX
	#endif
#elif (DefaultQRVersion == 17)
uint16 QR_URL_LENGTH_MAX = 938/2 - 5 - sizeof(HuffmanURL);    // QR v17
#endif
uint16 MaxTempDateLength = TempDataLength - 20;
uint16 DeltaZeroMinimum = 250;			// 초기 ?��??0 ��???�� 기�?��??

extern nrf_drv_twi_t m_twi;          // OTQV2_Init.c

void EPD_Drawing_DataNo(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint16 BaseAddress_X = Loc_X, BaseAddress_Y = Loc_Y;

EINK_WriteString(DisplayBuf,Loc_X,Loc_Y,"Data");
if(pMain->Sensor.DataNumber > 99)
  {
    EINK_WriteFont(DisplayBuf,Loc_X + 35,Loc_Y,pMain->Sensor.DataNumber / 100 % 10 + 0x30);
  }

if(pMain->Sensor.DataNumber > 9)
  {
    EINK_WriteFont(DisplayBuf,Loc_X + 42,Loc_Y,pMain->Sensor.DataNumber / 10  % 10 + 0x30);
  }


    EINK_WriteFont(DisplayBuf,Loc_X + 49,Loc_Y,pMain->Sensor.DataNumber / 1   % 10 + 0x30);

    EINK_WriteString(DisplayBuf,Loc_X+57,Loc_Y,"Points");
}


void EPD_Write_Serial(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint16 BaseAddress_X = Loc_X, BaseAddress_Y = Loc_Y;

EINK_WriteString(DisplayBuf,Loc_X,Loc_Y,"S/N");
for(int i=0;i<SizeOfSerial;i++)
  {
  EINK_WriteFont(DisplayBuf,Loc_X+28+i*7,Loc_Y,pMain->DevInfo.Serial[i]);
  }
}

void EPD_Drawing_Serial_16(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint16 BaseAddress_X = Loc_X, BaseAddress_Y = Loc_Y;

EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,"S/N");
if(SizeOfSerial == 10)
	{
	for(int i=0;i<SizeOfSerial;i++)
		{
		EINK_WriteFont16(DisplayBuf,Loc_X+54+i*11,Loc_Y,pMain->DevInfo.Serial[i]);
		}
	}
else
	{
	for(int i=0;i<SizeOfSerial;i++)
		{
		EINK_WriteFont16(DisplayBuf,Loc_X+76+i*11,Loc_Y,pMain->DevInfo.Serial[i]);
		}
	}
}

void EPD_Drawing_FWVer(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint8 BLAddressNumber = 0;
uint16 BaseAddress_X = Loc_X, BaseAddress_Y = Loc_Y;
uint32 *FICR_BLStartAddress = (uint32*)0x10001014;
BLAddressNumber = (*FICR_BLStartAddress >>12) & 0xf;

EINK_WriteString(DisplayBuf,Loc_X,Loc_Y,"F/V");
	EINK_WriteFont(DisplayBuf,Loc_X + 30,Loc_Y,BLAddressNumber + 0x30);
EINK_WriteString(DisplayBuf,Loc_X + 42,Loc_Y,stVersionString);
}


void EPD_Drawing_DPInterval(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint8 StringBuf[100];

uint16 BaseAddress_X = Loc_X, BaseAddress_Y = Loc_Y;
uint16 IntervalValue = pMain->Device.DisplayIntervalSec/60;

memset(StringBuf,0x00,100);

if(!IntervalValue) IntervalValue = 1;

if(pMain->Device.DisplayIntervalSec == 30)
	{
		sprintf(StringBuf,"Refresh  Record");
	}
else if(pMain->Device.DisplayIntervalSec == 0)
	{
		sprintf(StringBuf,"Refresh    Auto",IntervalValue);
	}
else if(pMain->Device.DisplayIntervalSec == 0xfffffff0)
	{
		sprintf(StringBuf,"Refresh  Manual",IntervalValue);
	}
else if(pMain->Device.DisplayIntervalSec < 100)
	{
		sprintf(StringBuf,"Refresh  %2d Min",IntervalValue);
	}
else
	{
		sprintf(StringBuf,"Refresh %3d Min",IntervalValue);
	}

EINK_WriteString(DisplayBuf,Loc_X,Loc_Y,StringBuf);
}


void EPD_Drawing_Interval(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint8 StringBuf[100];
uint16 BaseAddress_X = Loc_X, BaseAddress_Y = Loc_Y;
uint16 IntervalValue = pMain->Sensor.SensorIntervalSec/60;

memset(StringBuf,0x00,100);

if(!IntervalValue) IntervalValue = 1;

if(pMain->Sensor.SensorIntervalSec == 30)
	{
		sprintf(StringBuf,"Interval 30 Sec");
	}
else if(pMain->Sensor.SensorIntervalSec < 100)
	{
		sprintf(StringBuf,"Interval %2d Min",IntervalValue);
	}
else
	{
		sprintf(StringBuf,"Interval%3d Min",IntervalValue);
	}
EINK_WriteString(DisplayBuf,Loc_X,Loc_Y,StringBuf);
}

void EPD_Drawing_PointNumber(uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y,sint16 Number)
{
	uint16 Xpointer=Loc_X, NumberTemp = 0;
	uint32 NumberSlice = 100;
	uint8 NumberValidFlag = 0;

	if(!Number)			// number 가 0인경우 0을 표시하고 종료
		{
		EINK_WriteFont(DisplayBuf,Xpointer,Loc_Y,Number + 0x30);
		}
	else						// number가 0이 아닌경우  모든 값을 표시
		{
		if(Number < 0)		// number가 음수인 경우 - 표시
			{
			EINK_WriteString(DisplayBuf,Xpointer,Loc_Y,"-");
			Xpointer+=7;
			}
		for(int i=0;i<2;i++)
			{
			NumberTemp = (abs(Number) / NumberSlice) % 10;
			if(NumberTemp)
				{
				NumberValidFlag = 1;
				}
			if(NumberValidFlag)
				{
				EINK_WriteFont(DisplayBuf,Xpointer,Loc_Y,NumberTemp + 0x30);
				Xpointer+=7;
				}
			NumberSlice/=10;
			}
			EINK_WriteString(DisplayBuf,Xpointer,Loc_Y,".");
			Xpointer+=7;
			EINK_WriteFont(DisplayBuf,Xpointer,Loc_Y,abs(Number) % 10 + 0x30);
		}

	
}

void EPD_Drawing_SignedNumber(uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y,sint16 Number)
{
	uint16 Xpointer=Loc_X, NumberTemp = 0;
	uint32 NumberSlice = 100000000;
	uint8 NumberValidFlag = 0;

	if(!Number)			// number 가 0인경우 0을 표시하고 종료
		{
		EINK_WriteFont(DisplayBuf,Xpointer,Loc_Y,Number + 0x30);
		}
	else						// number가 0이 아닌경우  모든 값을 표시
		{
		if(Number < 0)		// number가 음수인 경우 - 표시
			{
			EINK_WriteString(DisplayBuf,Xpointer,Loc_Y,"-");
			Xpointer+=7;
			}
		for(int i=0;i<8;i++)
			{
			NumberTemp = (abs(Number) / NumberSlice) % 10;
			if(NumberTemp)
				{
				NumberValidFlag = 1;
				}
			if(NumberValidFlag)
				{
				EINK_WriteFont(DisplayBuf,Xpointer,Loc_Y,NumberTemp + 0x30);
				Xpointer+=7;
				}
			NumberSlice/=10;
			}
			EINK_WriteFont(DisplayBuf,Xpointer,Loc_Y,abs(Number) % 10 + 0x30);
		}

	
}

void EPD_Drawing_Index(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint16 BaseAddress_X = Loc_X, BaseAddress_Y = Loc_Y;
uint16 IntervalValue = pMain->Sensor.SensorIntervalSec/60;
if(!IntervalValue) IntervalValue = 1;

  EINK_WriteString(DisplayBuf,Loc_X,Loc_Y,"Boot Index");
  EINK_WriteFont(DisplayBuf,Loc_X + 91,Loc_Y,pMain->Page.BootIndex/10000 % 10 + 0x30);
	EINK_WriteFont(DisplayBuf,Loc_X + 98,Loc_Y,pMain->Page.BootIndex/1000 % 10 + 0x30);
	EINK_WriteFont(DisplayBuf,Loc_X + 105,Loc_Y,pMain->Page.BootIndex/100 % 10 + 0x30);
	EINK_WriteFont(DisplayBuf,Loc_X + 112,Loc_Y,pMain->Page.BootIndex/10 % 10 + 0x30);
	EINK_WriteFont(DisplayBuf,Loc_X + 119,Loc_Y,pMain->Page.BootIndex/1 % 10 + 0x30);
	
	EINK_WriteString(DisplayBuf,Loc_X,Loc_Y+12,"CurrentIndex");
	EINK_WriteFont(DisplayBuf,Loc_X + 91,Loc_Y+12,pMain->Sensor.DataNumber/10000 % 10 + 0x30);
	EINK_WriteFont(DisplayBuf,Loc_X + 98,Loc_Y+12,pMain->Sensor.DataNumber/1000 % 10 + 0x30);
	EINK_WriteFont(DisplayBuf,Loc_X + 105,Loc_Y+12,pMain->Sensor.DataNumber/100 % 10 + 0x30);
	EINK_WriteFont(DisplayBuf,Loc_X + 112,Loc_Y+12,pMain->Sensor.DataNumber/10 % 10 + 0x30);
	EINK_WriteFont(DisplayBuf,Loc_X + 119,Loc_Y+12,pMain->Sensor.DataNumber/1 % 10 + 0x30);
}

void EPD_Drawing_Serial_QR(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint16 BaseAddress = Loc_X + Loc_Y;
uint8 SampleCode[600];
uint8 QRbuf[1000];
uint8 SerialStringTmp[25];
sint16 Temperature = 0;
uint32 RecordTime = pMain->Device.Time.UpTimeSec - pMain->Device.Time.RecordBeginTime;

memset(SerialStringTmp,0x00,25);

Temperature = pMain->Sensor.Probe.ProbeTemp / 10;

switch(pMain->Device.RunningStatus)				// device status 별 Serial QR 표시 문구 설정
	{
	case RunStat_Factory :		// 공장 초기
	sprintf(SerialStringTmp,"%sT-%s",pMain->DevInfo.Serial,"FAC");
		break;

	case RunStat_Idle :				// 비활성화
	sprintf(SerialStringTmp,"%sT-%s",pMain->DevInfo.Serial,"IDL");
		break;

	case RunStat_StandBy :		// 기록 대기
	sprintf(SerialStringTmp,"%sT-%s",pMain->DevInfo.Serial,"STB");
		break;

	case RunStat_SelfTest :		// 자가진단
	sprintf(SerialStringTmp,"%sT-%s",pMain->DevInfo.Serial,"SFT");
		break;

	case RunStat_SelfTest2 :	// BLE 자가진단
	sprintf(SerialStringTmp,"%sT-%s",pMain->DevInfo.Serial,"SFT");
		break;

	case RunStat_SelfTest2_Connected :  // BLE 자가진단 중 기기 연결
	sprintf(SerialStringTmp,"%sT-%s",pMain->DevInfo.Serial,"SFT");
		break;

	case RunStat_SelfCalib :		// 캘리브레이션
	sprintf(SerialStringTmp,"%sT-%s",pMain->DevInfo.Serial,"CALS");
		break;

	case RunStat_SelfCalib_Probe :		// 프로브 캘리브레이션
	sprintf(SerialStringTmp,"%sT-%s",pMain->DevInfo.Serial,"CALP");
		break;

	case RunStat_Correction :			// 캘 결과 수집 모드
	sprintf(SerialStringTmp,"%sT-%s",pMain->DevInfo.Serial,"COR");
		break;

	case RunStat_DeviceConfig :		// 디바이스 설정 변경
	sprintf(SerialStringTmp,"%sT-%s",pMain->DevInfo.Serial,"CFG");
		break;

	case RunStat_Booting :				// 디바이스 부팅중
	sprintf(SerialStringTmp,"%sT-%s",pMain->DevInfo.Serial,"BOT");
		break;

	case RunStat_Logging :				// 디바이스 기록모드
	case RunStat_Logging_Connected :		// BLE 연결된 디바이스 기록 모드
		if(Temperature < 0)			// T옆에 항상 + 혹은 -를 표시 해 주기 위해 분기
			{
			sprintf(SerialStringTmp,"%sT-%03d",pMain->DevInfo.Serial,abs(Temperature));
			}
		else
			{
			sprintf(SerialStringTmp,"%sT+%03d",pMain->DevInfo.Serial,abs(Temperature));
			}
		if((RecordTime < CalInfoTimerSec) && (!pMain->Flags.SerialQR_OffsetViewFlag))			// 레코드 모드 진입 후 지정 시간 이내의 경우 프로브 캘리브레이션 정보 표시
			{
			sprintf(&SerialStringTmp[strlen(SerialStringTmp)] , ":%05d:%d" , pMain->Calib.CalData.Offset.ProbeTemp , pMain->Calib.CalData.Index);
			}
		break;
	}




qrcode_initBytes(&QRtmp,QRbuf,1,0,SerialStringTmp,strlen(SerialStringTmp));
memcpy(SampleCode,QRbuf,56);

for(int i=0;i<21;i++)
  {
    BitCopy(DisplayBuf,Loc_Y * 176 + ((i)*176)*2 + Loc_X , SampleCode,i*21,21);
    BitCopy(DisplayBuf,Loc_Y * 176 + ((i)*176)*2+176 + Loc_X , SampleCode,i*21,21);
    BitDoubler(DisplayBuf,Loc_Y * 22 + ((i)*22)*2 + Loc_X/8 , 3);
    BitDoubler(DisplayBuf,Loc_Y * 22 + ((i)*22)*2+22 + Loc_X/8 , 3);
  }
}


void EPD_Drawing_CheckBox(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y, uint8 Enable)
{
	if(Enable)
		{
		EINK_WriteIcon(DisplayBuf,Loc_X,Loc_Y,0x22);
    EINK_WriteIcon(DisplayBuf,Loc_X+8,Loc_Y,0x24);
		}
	else
		{
		EINK_WriteIcon(DisplayBuf,Loc_X,Loc_Y,0x23);
    EINK_WriteIcon(DisplayBuf,Loc_X+8,Loc_Y,0x24);
		}
}

void EPD_Drawing_Recording(stMain *pMain,uint8 *DisplayBuf,PageMode Mode)
{
  if(Mode == PageMode_Standby)
  {
    EINK_WriteIcon(DisplayBuf,0,2,0x1B);
    EINK_WriteIcon(DisplayBuf,8,2,0x1C);
    EINK_WriteString(DisplayBuf,13,2,"StandBy");
  }
  else if(Mode == PageMode_Recording)
  {
    EINK_WriteIcon(DisplayBuf,0,2,0x19);
    EINK_WriteIcon(DisplayBuf,8,2,0x1A);
    EINK_WriteString(DisplayBuf,13,2,"Recording");
  }
  else if(Mode == PageMode_Pause)
  {
    EINK_WriteIcon(DisplayBuf,0,2,0x1B);
    EINK_WriteIcon(DisplayBuf,8,2,0x1C);
    EINK_WriteString(DisplayBuf,13,2,"Pause");
  }
  else if(Mode == PageMode_SelfTest)
  {
    EINK_WriteIcon(DisplayBuf,0,2,0x1B);
    EINK_WriteIcon(DisplayBuf,8,2,0x1C);
    EINK_WriteString(DisplayBuf,13,2,"SelfTest");
  }
  else if(Mode == PageMode_SelfTest2)
  {
    EINK_WriteIcon(DisplayBuf,0,2,0x1B);
    EINK_WriteIcon(DisplayBuf,8,2,0x1C);
    EINK_WriteString(DisplayBuf,13,2,"SelfTest 2");
  }
	else if(Mode == PageMode_CalibrationStandby)
  {
    EINK_WriteIcon(DisplayBuf,0,2,0x1B);
    EINK_WriteIcon(DisplayBuf,8,2,0x1C);
    EINK_WriteString(DisplayBuf,13,2,"Cal. Standby");
  }
	else if(Mode == PageMode_Calibration)
  {
    EINK_WriteIcon(DisplayBuf,0,2,0x19);
    EINK_WriteIcon(DisplayBuf,8,2,0x1A);
    EINK_WriteString(DisplayBuf,13,2,"Calibration");
  }
	else if(Mode == PageMode_CalibrationComplete)
  {
    EINK_WriteIcon(DisplayBuf,0,2,0x1B);
    EINK_WriteIcon(DisplayBuf,8,2,0x1C);
    EINK_WriteString(DisplayBuf,13,2,"Cal. end");
  }
	else if(Mode == PageMode_CorrectionStandby)
  {
    EINK_WriteIcon(DisplayBuf,0,2,0x1B);
    EINK_WriteIcon(DisplayBuf,8,2,0x1C);
    EINK_WriteString(DisplayBuf,13,2,"Cor. Standby");
  }
	else if(Mode == PageMode_Correction)
  {
    EINK_WriteIcon(DisplayBuf,0,2,0x19);
    EINK_WriteIcon(DisplayBuf,8,2,0x1A);
    EINK_WriteString(DisplayBuf,13,2,"Correction");
  }
	else if(Mode == PageMode_CorrectionComplete)
  {
    EINK_WriteIcon(DisplayBuf,0,2,0x1B);
    EINK_WriteIcon(DisplayBuf,8,2,0x1C);
    EINK_WriteString(DisplayBuf,13,2,"Cor. end");
  }
	else if(Mode == PageMode_Configuration)
  {
    EINK_WriteIcon(DisplayBuf,0,2,0x25);
    EINK_WriteIcon(DisplayBuf,8,2,0x26);
    EINK_WriteString(DisplayBuf,13,2,"Setting");
  }
}

void EPD_Drawing_Battery(stMain *pMain,uint8 *DisplayBuf)
{
	uint8 BatPercent = Batt_GetVoltage_Percent(pMain);
	
	pMain->Device.BatLevel = BatPercent / 20;
    switch(pMain->Device.BatLevel)
    {
    default:
            EINK_WriteIcon(DisplayBuf,144,2,0x21);
            EINK_WriteIcon(DisplayBuf,152,2,0x20);
            EINK_WriteIcon(DisplayBuf,159,2,0x18);
            break;
						
    case 1:
            EINK_WriteIcon(DisplayBuf,144,2,0x1D);
            EINK_WriteIcon(DisplayBuf,152,2,0x20);
            EINK_WriteIcon(DisplayBuf,159,2,0x18);
            break;

		case 2:
            EINK_WriteIcon(DisplayBuf,144,2,0x16);
            EINK_WriteIcon(DisplayBuf,152,2,0x20);
            EINK_WriteIcon(DisplayBuf,159,2,0x18);
            break;

    case 3:
            EINK_WriteIcon(DisplayBuf,144,2,0x16);
            EINK_WriteIcon(DisplayBuf,152,2,0x1F);
            EINK_WriteIcon(DisplayBuf,159,2,0x18);
            break;

    case 4 :
            EINK_WriteIcon(DisplayBuf,144,2,0x16);
            EINK_WriteIcon(DisplayBuf,152,2,0x1E);
            EINK_WriteIcon(DisplayBuf,159,2,0x18);
            break;


    case 5:
            EINK_WriteIcon(DisplayBuf,144,2,0x16);
            EINK_WriteIcon(DisplayBuf,152,2,0x17);
            EINK_WriteIcon(DisplayBuf,159,2,0x18);
            break;
    }
}

void EPD_Drawing_BatteryADC(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
  uint32 BattADCValue = pMain->Device.BatVolt;

  #if 1
  if(BattADCValue > 9999)
  {
    EINK_WriteFont(DisplayBuf,Loc_X,Loc_Y,BattADCValue/10000 % 10 + 0x30);
  }
  if(BattADCValue > 999)
  {
    EINK_WriteFont(DisplayBuf,Loc_X + 7,Loc_Y,BattADCValue/1000 % 10 + 0x30);
  }

  if(BattADCValue > 99)
  {
    EINK_WriteFont(DisplayBuf,Loc_X + 14,Loc_Y,BattADCValue/100  % 10 + 0x30);
  }

  if(BattADCValue > 9)
  {
    EINK_WriteFont(DisplayBuf,Loc_X + 21,Loc_Y,BattADCValue/10   % 10 + 0x30);
  }
  
  EINK_WriteFont(DisplayBuf,Loc_X + 28,Loc_Y,BattADCValue  % 10 + 0x30);
  #else
  EINK_WriteString(DisplayBuf,Loc_X,Loc_Y,"BATT : ");
  
  if(BattVoltageInt > 999)
  {
    EINK_WriteFont(DisplayBuf,Loc_X + 42,Loc_Y,BattVoltageInt/1000 % 10 + 0x30);
  }

  if(BattVoltageInt > 99)
  {
    EINK_WriteFont(DisplayBuf,Loc_X + 49,Loc_Y,BattVoltageInt/100  % 10 + 0x30);
  }

  if(BattVoltageInt > 9)
  {
    EINK_WriteFont(DisplayBuf,Loc_X + 56,Loc_Y,BattVoltageInt/10   % 10 + 0x30);
  }
  
  EINK_WriteFont(DisplayBuf,Loc_X + 63,Loc_Y,BattVoltageInt  % 10 + 0x30);
  #endif
}        


void EPD_Drawing_BatteryPercent(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
  uint8 BattADCPercent = Batt_GetVoltage_Percent(pMain);

  if(BattADCPercent > 99)
  {
    EINK_WriteFont(DisplayBuf,Loc_X + 14,Loc_Y,BattADCPercent/100  % 10 + 0x30);
  }

  if(BattADCPercent > 9)
  {
    EINK_WriteFont(DisplayBuf,Loc_X + 21,Loc_Y,BattADCPercent/10   % 10 + 0x30);
  }
  
  EINK_WriteFont(DisplayBuf,Loc_X + 28,Loc_Y,BattADCPercent  % 10 + 0x30);
}     

void EPD_Drawing_BatteryVoltage(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
	uint8 StrBuf[100];
  uint32 BattADCValue = 0;
	//uint16 BattVoltageInt = Batt_GetVoltage_int(pMain);
	uint16 BattVoltageInt = pMain->Device.BatVolt;
	float VBatTmp = BattVoltageInt;
	VBatTmp /= 1000;
	
	memset(StrBuf,0x00,100);
	
  #if 1

	//sprintf(StrBuf,"BATT V : %4d",BattVoltageInt);
	sprintf(StrBuf,"BATT    : %.3fV",VBatTmp);
	
	EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,StrBuf);
	
  #else
  EINK_WriteString(DisplayBuf,Loc_X,Loc_Y,"BATT : ");
  
  if(BattVoltageInt > 999)
  {
    EINK_WriteFont(DisplayBuf,Loc_X + 42,Loc_Y,BattVoltageInt/1000 % 10 + 0x30);
  }

  if(BattVoltageInt > 99)
  {
    EINK_WriteFont(DisplayBuf,Loc_X + 49,Loc_Y,BattVoltageInt/100  % 10 + 0x30);
  }

  if(BattVoltageInt > 9)
  {
    EINK_WriteFont(DisplayBuf,Loc_X + 56,Loc_Y,BattVoltageInt/10   % 10 + 0x30);
  }
  
  EINK_WriteFont(DisplayBuf,Loc_X + 63,Loc_Y,BattVoltageInt  % 10 + 0x30);
  #endif
}        

void EPD_Drawing_ProbePass(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
	uint8 StrBuf[100];
	uint32 ProbeADCTemp=0;
	
	memset(StrBuf,0x00,100);
	
	for(int i=0;i<10;i++)
		{
		ProbeADCTemp += pMain->Sensor.Probe.ProbeBuffer[i];
		}
	ProbeADCTemp/=10;
	pMain->Sensor.Probe.ProbeADCValue = ProbeADCTemp;
	#ifdef Probe_4Wire
	if (pMain->Sensor.Probe.fProbeTemp > 20 && pMain->Sensor.Probe.fProbeTemp < 30 )
	{
    EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,"Probe(4): Pass");
	}
	else if(pMain->Sensor.Probe.ProbeADCValue < 100 || pMain->Sensor.Probe.ProbeADCValue > 16000 )
	#else
	if(pMain->Sensor.Probe.ProbeADCValue < 100 || pMain->Sensor.Probe.ProbeADCValue > 16000 )
	#endif
  {
    EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,"Probe   :  Fail");
  }
  else
  {
		EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,"Probe   : Pass");
  }
}

void EPD_Drawing_ProbeTest(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
	uint8 StrBuf[100];
	uint32 ProbeADCTemp=0;
	
	memset(StrBuf,0x00,100);
	
	for(int i=0;i<10;i++)
		{
		ProbeADCTemp += pMain->Sensor.Probe.ProbeBuffer[i];
		}
	ProbeADCTemp/=10;
	pMain->Sensor.Probe.ProbeADCValue = ProbeADCTemp;
	#ifdef Probe_4Wire
	if (pMain->Sensor.Probe.fProbeTemp > 20 && pMain->Sensor.Probe.fProbeTemp < 30 )
	{
	sprintf(StrBuf,"TEMP-PB : %.3f",pMain->Sensor.Probe.fProbeTemp);
	EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,StrBuf);
	}
	else if(pMain->Sensor.Probe.ProbeADCValue < 100 || pMain->Sensor.Probe.ProbeADCValue > 16000 )
	#else
  if(pMain->Sensor.Probe.ProbeADCValue < 100 || pMain->Sensor.Probe.ProbeADCValue > 16000 )
	#endif
  {
    EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,"TEMP-PB :  Fail");
  }
  else
  {
		sprintf(StrBuf,"TEMP-PB : %.3f",pMain->Sensor.Probe.fProbeTemp);
		EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,StrBuf);

  }
}

void EPD_Drawing_DeviceInfo(stMain *pMain, uint8* DisplayBuf)
{
		EPD_Drawing_Serial_16(pMain,DisplayBuf,8,206);
    EPD_Drawing_FWVer(pMain,DisplayBuf,8,244);
		EPD_Drawing_DPInterval(pMain,DisplayBuf,9,232);
    EPD_Drawing_Interval(pMain,DisplayBuf,8,222);

    EPD_Drawing_Serial_QR(pMain,DisplayBuf,132,220);
}

void EPD_Write_EPDTest(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
  EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,"EPD     : Pass");
}

void EPD_Write_EEPROMTest(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint8 EEPROMTestResult=0;

  EEPROMTestResult = TWI_EEPROMTestCode();

  if(EEPROMTestResult == 0xff)
  {
    EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,"EEPROM  :  Fail");
  }
  else if (EEPROMTestResult ==1)
  {
    EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,"EEPROM  : Pass");
  }
  else
  {
    EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,"EEPROM  : Error");
  }
}

void EPD_Write_MC3479Test(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint8 TestResult=0;

  TestResult = Init_MC3479(pMain,&m_twi);

  if(TestResult == 0xff)
  {
    EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,"SEN-ACC :  Fail");
  }
  else if (TestResult ==1)
  {
    EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,"SEN-ACC : Pass");
  }
  else
  {
    EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,"SEN-ACC : Error");
  }
}


void EPD_Write_TempSensorPass(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint8 StrBuf[100];
memset(StrBuf,0x00,100);

  if(pMain->Sensor.PCBTemp > 1500 && pMain->Sensor.PCBTemp < 3500)      // �보�서  측정값이 15 ~ 35�내�때 �스
  {
    //EPD_Write_PCBTemp(pMain,DisplayBuf,Loc_X,Loc_Y);
		sprintf(StrBuf,"SEN-TEMP: Pass");
  }
  else
  {
    sprintf(StrBuf,"SEN-TEMP:  Fail");
  }
	EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,StrBuf);
}

void EPD_Write_TempSensorTest(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint8 StrBuf[100];
float fTemp = pMain->Sensor.PCBTemp;

fTemp /= 100;
memset(StrBuf,0x00,100);

  if(pMain->Sensor.PCBTemp > 1500 && pMain->Sensor.PCBTemp < 3500)      // �보�서  측정값이 15 ~ 35�내�때 �스
  {
    //EPD_Write_PCBTemp(pMain,DisplayBuf,Loc_X,Loc_Y);
		//sprintf(StrBuf,"TEMP-BD : %4d",abs(pMain->Sensor.PCBTemp));
		sprintf(StrBuf,"TEMP-BD : %.2f",fTemp);
		EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,StrBuf);
  }
  else
  {
    EINK_WriteString16(DisplayBuf,Loc_X,Loc_Y,"TEMP-BD :  Fail");
  }
}

void EPD_Write_AvailableData(stMain *pMain)
{
}

void EPD_Write_PCBTemp(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint16 BaseAddress_X = Loc_X , BaseAddress_Y = Loc_Y;

  EINK_WriteString(DisplayBuf,BaseAddress_X,BaseAddress_Y,"Device");

  if(pMain->Sensor.PCBTemp>10000)EINK_WriteFont(DisplayBuf,BaseAddress_X + 46,BaseAddress_Y,(pMain->Sensor.PCBTemp/10000)+0x30);
  if(pMain->Sensor.PCBTemp<0)EINK_WriteFont(DisplayBuf,BaseAddress_X,BaseAddress_Y,'-');
  EINK_WriteFont(DisplayBuf,BaseAddress_X+53,BaseAddress_Y,(abs(pMain->Sensor.PCBTemp)/1000%10)+0x30);
  EINK_WriteFont(DisplayBuf,BaseAddress_X+60,BaseAddress_Y,(abs(pMain->Sensor.PCBTemp)/100%10)+0x30);
  EINK_WriteString(DisplayBuf,BaseAddress_X+67,BaseAddress_Y,".");
  EINK_WriteFont(DisplayBuf,BaseAddress_X+74,BaseAddress_Y,(abs(pMain->Sensor.PCBTemp)/10%10)+0x30);
  EINK_WriteString(DisplayBuf,BaseAddress_X+81,BaseAddress_Y,"'");
  EINK_WriteString(DisplayBuf,BaseAddress_X+88,BaseAddress_Y,"C");
}

void EPD_Write_PCBTemp_LargeDetail(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint16 BaseAddress_X = Loc_X , BaseAddress_Y = Loc_Y;


  if(pMain->Sensor.PCBTemp < 0)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X,BaseAddress_Y,14);
  }
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+88,BaseAddress_Y,12);     // '
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+96,BaseAddress_Y,13);     // C
  if(abs(pMain->Sensor.PCBTemp) / 10000 %10)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X,BaseAddress_Y,abs(pMain->Sensor.PCBTemp) / 10000 %10);
  }
  if(abs(pMain->Sensor.PCBTemp) / 1000 %10)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+16,BaseAddress_Y,abs(pMain->Sensor.PCBTemp) / 1000 %10);
  }
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+32,BaseAddress_Y,abs(pMain->Sensor.PCBTemp) / 100 % 10);
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+48,BaseAddress_Y,15);
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+56,BaseAddress_Y,abs(pMain->Sensor.PCBTemp) / 10 % 10);
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+72,BaseAddress_Y,abs(pMain->Sensor.PCBTemp) / 1 % 10);
	
if(pMain->Calib.CalibView == CalibView_WithOutOffset)
	{
	EINK_WriteFont(DisplayBuf,BaseAddress_X+112,BaseAddress_Y,'X');
	}
}

void EPD_Write_PCBTemp_Large(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint16 BaseAddress_X = Loc_X , BaseAddress_Y = Loc_Y;


  if(pMain->Sensor.PCBTemp < 0)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X,BaseAddress_Y,14);
  }
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+72,BaseAddress_Y,12);     // '
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+80,BaseAddress_Y,13);     // C
  if(abs(pMain->Sensor.PCBTemp) / 10000 %10)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X,BaseAddress_Y,abs(pMain->Sensor.PCBTemp) / 10000 %10);
  }
  if(abs(pMain->Sensor.PCBTemp) / 1000 %10)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+16,BaseAddress_Y,abs(pMain->Sensor.PCBTemp) / 1000 %10);
  }
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+32,BaseAddress_Y,abs(pMain->Sensor.PCBTemp) / 100 % 10);
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+48,BaseAddress_Y,15);
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+56,BaseAddress_Y,abs(pMain->Sensor.PCBTemp) / 10 % 10);
  //EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+72,BaseAddress_Y,abs(pMain->Sensor.PCBTemp) / 1 % 10);
	
if(pMain->Calib.CalibView == CalibView_WithOutOffset)
	{
	EINK_WriteFont(DisplayBuf,BaseAddress_X+112,BaseAddress_Y,'X');
	}
}

void EPD_Write_PCBTemp_Large_Int(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint16 BaseAddress_X = Loc_X , BaseAddress_Y = Loc_Y;


  if(pMain->Sensor.PCBTemp < 0)
  {
		EINK_WriteLargeFont(DisplayBuf,BaseAddress_X,BaseAddress_Y,14);
  }
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+48,BaseAddress_Y,12);     // '
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+56,BaseAddress_Y,13);     // C
  if(abs(pMain->Sensor.PCBTemp) / 1000 %10)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+16,BaseAddress_Y,abs(pMain->Sensor.PCBTemp) / 1000 %10);
  }
  if(abs(pMain->Sensor.PCBTemp) / 100 %10)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+32,BaseAddress_Y,abs(pMain->Sensor.PCBTemp) / 100 %10);
  }
	
if(pMain->Calib.CalibView == CalibView_WithOutOffset)
	{
	EINK_WriteFont(DisplayBuf,BaseAddress_X+112,BaseAddress_Y,'X');
	}
}

void EPD_Write_Humidity(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
	  //EINK_WriteString(DisplayBuf,Loc_X,Loc_Y,"Humi : ");
	  EINK_WriteFont(DisplayBuf,Loc_X,Loc_Y,(pMain->Sensor.PCBHumi/10%10)+0x30);
	  EINK_WriteFont(DisplayBuf,Loc_X+7,Loc_Y,(pMain->Sensor.PCBHumi%10)+0x30);
	  EINK_WriteFont(DisplayBuf,Loc_X+14,Loc_Y,'%');
}

void EPD_Write_Humidity_LargeFont(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
	  EINK_WriteLargeFont(DisplayBuf,Loc_X,Loc_Y,(pMain->Sensor.PCBHumi/10%10));
	  EINK_WriteLargeFont(DisplayBuf,Loc_X+16,Loc_Y,(pMain->Sensor.PCBHumi%10));
	  EINK_WriteLargeFont(DisplayBuf,Loc_X+38,Loc_Y,29);
}

void EPD_Write_ProbeTempDetail(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint16 BaseAddress_X = Loc_X , BaseAddress_Y = Loc_Y;
uint32 ProbeCalcTemp=0;

  //EINK_WriteString(DisplayBuf,BaseAddress_X,BaseAddress_Y,"Probe");
  if(pMain->Sensor.Probe.ProbeTemp < 0)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+0,BaseAddress_Y,14);
  }
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+88,BaseAddress_Y,12);     // '
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+96,BaseAddress_Y,13);     // C
  if(abs(pMain->Sensor.Probe.ProbeTemp) / 10000 %10)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+0,BaseAddress_Y,abs(pMain->Sensor.Probe.ProbeTemp) / 10000 %10);
  }
  if(abs(pMain->Sensor.Probe.ProbeTemp) / 1000 %10)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+16,BaseAddress_Y,abs(pMain->Sensor.Probe.ProbeTemp) / 1000 %10);
  }
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+32,BaseAddress_Y,abs(pMain->Sensor.Probe.ProbeTemp) / 100 % 10);
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+48,BaseAddress_Y,15);
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+56,BaseAddress_Y,abs(pMain->Sensor.Probe.ProbeTemp) / 10 % 10);
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+72,BaseAddress_Y,abs(pMain->Sensor.Probe.ProbeTemp) / 1 % 10);
  //EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+80,BaseAddress_Y,HumiValue /10 %10);
  //EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+96,BaseAddress_Y,HumiValue %10);
  //EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+112,BaseAddress_Y,29);
}
	
void EPD_Write_ProbeTemp(stMain *pMain,uint8 *DisplayBuf,uint16 Loc_X, uint16 Loc_Y)
{
uint16 BaseAddress_X = Loc_X , BaseAddress_Y = Loc_Y;
uint32 ProbeCalcTemp=0;

  //EINK_WriteString(DisplayBuf,BaseAddress_X,BaseAddress_Y,"Probe");
  if(pMain->Sensor.Probe.ProbeTemp < 0)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+0,BaseAddress_Y,14);
  }
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+72,BaseAddress_Y,12);     // '
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+80,BaseAddress_Y,13);     // C
  if(abs(pMain->Sensor.Probe.ProbeTemp) / 10000 %10)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+0,BaseAddress_Y,abs(pMain->Sensor.Probe.ProbeTemp) / 10000 %10);
  }
  if(abs(pMain->Sensor.Probe.ProbeTemp) / 1000 %10)
  {
          EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+16,BaseAddress_Y,abs(pMain->Sensor.Probe.ProbeTemp) / 1000 %10);
  }
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+32,BaseAddress_Y,abs(pMain->Sensor.Probe.ProbeTemp) / 100 % 10);
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+48,BaseAddress_Y,15);
  EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+56,BaseAddress_Y,abs(pMain->Sensor.Probe.ProbeTemp) / 10 % 10);
  //EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+72,BaseAddress_Y,abs(pMain->Sensor.Probe.ProbeTemp) / 1 % 10);
  //EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+80,BaseAddress_Y,HumiValue /10 %10);
  //EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+96,BaseAddress_Y,HumiValue %10);
  //EINK_WriteLargeFont(DisplayBuf,BaseAddress_X+112,BaseAddress_Y,29);
}

void EPD_Write_ProbeADC(stMain *pMain,uint8 *DisplayBuf)
{
#ifdef ADC_BAT_MONITOR
EINK_WriteString(DisplayBuf,12,64,"Bat");
#else
EINK_WriteString(DisplayBuf,12,64,"Probe");
#endif
  EINK_WriteLargeFont(DisplayBuf,66,64,abs(pMain->Sensor.Probe.ProbeADCValue) / 10000 % 10);
  EINK_WriteLargeFont(DisplayBuf,82,64,abs(pMain->Sensor.Probe.ProbeADCValue) / 1000 % 10);
  EINK_WriteLargeFont(DisplayBuf,98,64,abs(pMain->Sensor.Probe.ProbeADCValue) / 100 % 10);
  EINK_WriteLargeFont(DisplayBuf,114,64,abs(pMain->Sensor.Probe.ProbeADCValue) / 10 % 10);
  EINK_WriteLargeFont(DisplayBuf,130,64,abs(pMain->Sensor.Probe.ProbeADCValue) / 1 % 10);
}

void DisplayEnable(stMain *pMain,uint8 *QRbuf,uint8 QRVersion)
{
	uint8 BufferClearDummy[8]={0,0,0,0,0,0,0,0};
	uint8 SampleCode[1000];

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


void QRLogger_DisplayURL(stMain *pMain,uint8 *DisplayBufBuf, uint8 *Huffman,uint16 length)
{
	uint8 ActiveQRVersion;
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
	qrcode_initBytes(&QRtmp,DisplayBufBuf,ActiveQRVersion,0,p,length*2 + sizeof(HuffmanURL) -1);

	DisplayEnable(pMain,DisplayBufBuf,ActiveQRVersion);

        free(p);
	//EPD_Display(pMain,DisplayBufBuf,0,0);
}


void QRLogger_DisplayBase41URL(stMain *pMain,uint8 *DisplayBufBuf, uint8 *Base41,uint16 length)
{
	uint8 ActiveQRVersion;
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

	length = length *3 /2;
	memcpy(pp,Base41,length);

	qrcode_initBytes(&QRtmp,DisplayBufBuf,ActiveQRVersion,0,p,length + sizeof(HuffmanURL) -1);

	DisplayEnable(pMain,DisplayBufBuf,ActiveQRVersion);

        free(p);
	//EPD_Display(pMain,DisplayBufBuf,0,0);
}

void EPDPage_Factory_Reset(stMain *pMain)
{
uint8 DisplayBuf[5888];
memset(DisplayBuf,0x00,5888);

	// page information
    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_Recording);
    //EPD_Drawing_BatteryPercent(pMain,DisplayBuf,105,0);
    //EPD_Drawing_BatteryADC(pMain,DisplayBuf,68,184);

		EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
		
	// factory reset comment
		EINK_WriteString(DisplayBuf,8,60,"The device is ");
		EINK_WriteString(DisplayBuf,16,72,"in factory reset.");
		
		EINK_WriteString(DisplayBuf,16,96,"please wait....");
		
		EPD_WhiteScreen_ALL(pMain,DisplayBuf);
}


EEPROM_RawDatastructure EEPRaw;
stLogData Huffman_Decode;
extern uint32_t timeMs;
#define HuffmanMaxLength 2000
uint8 Huffman[HuffmanMaxLength];
volatile uint16 HuffmanLength=0xffff;

void EPDPage_GetRawData(stMain *pMain)
{
uint16 DataLength=sizeof(stLogData);

	if(pMain->Page.CurrentPage <= 1)			// first page
		{
			EEPROM_GetDataLength(&DataLength);
			pMain->Sensor.DataNumber = DataLength;
			pMain->Page.LastDataIndex = pMain->Sensor.DataNumber;
			pMain->Page.OriginPoint = DataLength;
			EEPROM_GetRawData(pMain,&EEPRaw);
		}
	else
		{
			pMain->Page.OriginPoint = pMain->pLogData->FirstIndex;
			pMain->Page.LastDataIndex = pMain->Page.OriginPoint;
			pMain->pLogData->LastIndex = pMain->Page.LastDataIndex;
			EEPROM_GetRawData(pMain,&EEPRaw);
		}
}

void EPDPage_MakeQRWithHuffman(stMain *pMain,uint8 *QRBuff)
{

uint8 *p = (uint8*)pMain->pLogData;
volatile uint16 OriginDataPick = 0,DataPickPoint = 0;
uint16 EraseCount = 0;
uint16 ByteLength = 0;
uint8 Base41[1500];

memset(Huffman,0x00,HuffmanMaxLength);
memset(Base41,0x00,1500);
memset(&Huffman_Decode,0x00,sizeof(stLogData));

if(DefineQRVersion == 12)
{
	QR_URL_LENGTH_MAX = 257;
}


if(pMain->Page.CurrentPage == 1)			// first page
	{
		EPDPage_GetRawData(pMain);
		ByteLength = EEPROM_GetSerialDataRef(pMain,&pMain->pLogData->Data[0],&EEPRaw); // �이�택�성
		//ByteLength = EEPROM_GetSerialData(pMain,&pData->Data[0]);			// ?��?��?�� 불러?����??
		pMain->pLogData->LastIndex = pMain->Page.LastDataIndex;
		pMain->pLogData->FirstIndex = pMain->Page.FirstIndex;
		pMain->pLogData->PageNumber = pMain->Page.CurrentPage - 1;
		DataPickPoint = pMain->pLogData->LastIndex ;
		while(HuffmanLength > QR_URL_LENGTH_MAX)
			{
			//wdt_feed();
			pMain->Page.PageDataLength = EEPROM_GetSerialDataRef(pMain,&pMain->pLogData->Data[0],&EEPRaw); // �이�택�성
			pMain->pLogData->FirstIndex = pMain->Page.FirstIndex;
			pMain->pLogData->LastIndex = pMain->Page.LastDataIndex;

			if(pMain->pLogData->FirstIndex < 2 || pMain->pLogData->PageNumber == MaxPageRef) 
				{
				pMain->pLogData->PageNumber |= 0xf0;
				}
			else
				{
				pMain->pLogData->PageNumber &= 0x0f;
				pMain->pLogData->PageNumber |= MaxPageRef << 4;
				}
			
			OTQ_GetValidTempCount(pMain,pMain->pLogData);
			DataPickPoint = pMain->Page.ValidDataCount;
			memset(Huffman,0x00,HuffmanMaxLength);
			HuffmanLength=Huffman_Compress((unsigned char*)pMain->pLogData,Huffman,pMain->Page.PageDataLength + 24);		// ?��?����?? 코드 ?��?��
			#ifdef Base41Encoding
			if(HuffmanLength < HuffmanMaxLength)
				{
				memset(Base41,0x00,1500);
				base41_encode(Huffman,HuffmanLength,Base41);
				}
			#endif
			if(EEPRaw.DataLength > 10)
				{
				EEPRaw.DataLength -= 10;			// �이갈� 조절
				}
			//pMain->Page.FirstIndex = pMain->Page.LastDataIndex - EEPRaw.DataLength;
			}
		//EEPRaw.DataLength +=10;
		NRF_LOG_INFO("[%10d] Last : %4d First : %4d Page : %d",timeMs,pMain->pLogData->LastIndex,pMain->pLogData->FirstIndex,pMain->pLogData->PageNumber);
		EEPROM_WriteLastDataNumber(DataPickPoint);
	}
else
	{
	EPDPage_GetRawData(pMain);
	/*
		ByteLength = EEPROM_GetSerialDataRef(pMain,&pMain->pLogData->Data[0],&EEPRaw); // �이�택�성
		//ByteLength = EEPROM_GetSerialData(pMain,&pMain->pLogData->Data[0]);			// ?��?��?�� 불러?����??
		pMain->pLogData->LastIndex = pMain->Page.LastDataIndex;
		pMain->pLogData->FirstIndex = pMain->Page.FirstIndex; */
		DataPickPoint = pMain->pLogData->LastIndex ;
		pMain->pLogData->PageNumber = pMain->Page.CurrentPage - 1;
		while(HuffmanLength > QR_URL_LENGTH_MAX)
			{
			//wdt_feed();
			ByteLength = EEPROM_GetSerialDataRef(pMain,&pMain->pLogData->Data[0],&EEPRaw); // �이�택�성
			//pMain->Page.FirstIndex = pMain->Page.LastDataIndex - EEPRaw.DataLength;
			pMain->pLogData->FirstIndex = pMain->Page.FirstIndex;
			pMain->pLogData->LastIndex = pMain->Page.LastDataIndex;

			if(pMain->pLogData->FirstIndex < pMain->Sensor.DataNumber - 7200)
				{
				pMain->pLogData->FirstIndex = pMain->Sensor.DataNumber - 7201;
				pMain->pLogData->PageNumber |= 0xf0;
				}
			else if(pMain->pLogData->FirstIndex < 2 || pMain->pLogData->PageNumber == MaxPageRef) 
				{
				pMain->pLogData->PageNumber |= 0xf0;
				}
			else
				{
				pMain->pLogData->PageNumber |= MaxPageRef << 4;
				}
			
			OTQ_GetValidTempCount(pMain,pMain->pLogData);
			DataPickPoint = pMain->Page.ValidDataCount;
			memset(Huffman,0x00,HuffmanMaxLength);
			HuffmanLength=Huffman_Compress((unsigned char*)pMain->pLogData,Huffman,ByteLength + 24);		// ?��?����?? 코드 ?��?��
			#ifdef Base41Encoding
			if(HuffmanLength < HuffmanMaxLength)
				{
				memset(Base41,0x00,1500);
				base41_encode(Huffman,HuffmanLength,Base41);
				}
			#endif
			if(EEPRaw.DataLength > 10) EEPRaw.DataLength -= 10;			// �이갈� 조절
			
			}
		//EEPRaw.DataLength +=10;
		NRF_LOG_INFO("[%10d] Last : %4d First : %4d Page : %d",timeMs,pMain->pLogData->LastIndex,pMain->pLogData->FirstIndex,pMain->pLogData->PageNumber);
		//Huffman_Uncompress(Huffman,&Huffman_Decode,HuffmanLength,ByteLength + 24); for debug
		EEPROM_WriteLastDataNumber(DataPickPoint);
	}
EEPROM_AddEPDRefresh_QR();
SetLUTSecond();
NRF_LOG_INFO("Huffman Length : %d",HuffmanLength);
#ifdef Base41Encoding
QRLogger_DisplayBase41URL(pMain,QRBuff,Base41,HuffmanLength);
#else
QRLogger_DisplayURL(pMain,QRBuff,Huffman,HuffmanLength);		// URL QR코드 출력
//EPDPage_GetRawData(pMain);
#endif



	HuffmanLength = 0xffff;
}

void EPDPage_Service_QR(stMain *pMain)
{
uint8 DisplayBuf[5888];

memset(DisplayBuf,0x00,5888);

uint8 Dummy=0;

		EPDReturnTimer_Stop();
		EPDReturnTimer_Start(300000);						// 대기화면 복구 타이머 작동

    pMain->Page.MaxPage = MaxPageRef;

		// generate QR code
		Init_RecordStructure(pMain);
    EPDPage_MakeQRWithHuffman(pMain,DisplayBuf);

		
		// common infomation
    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_Recording);
		#ifdef DEBUG
    EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
		#endif
    EPD_Drawing_Battery(pMain,DisplayBuf);
    EPD_Write_Serial(pMain,DisplayBuf,8,24);
    Display_Page(pMain,DisplayBuf);
		if((pMain->pLogData->FirstIndex < 2) || (pMain->Page.CurrentPage == pMain->Page.MaxPage) || (pMain->pLogData->FirstIndex < pMain->Sensor.DataNumber - 7200))
			{
			EINK_WriteString(DisplayBuf,103,252,"- Last");
			}
		
		// page infomation
		#ifdef BELL_ENC
		EINK_WriteString(DisplayBuf,22,36,"Device");
			EPD_Write_PCBTemp_Large(pMain,DisplayBuf,3,52);
			EPD_Write_Humidity_LargeFont(pMain,DisplayBuf,118,52);
		#else
		EINK_WriteString(DisplayBuf,22,36,"Probe");
			EPD_Write_ProbeTemp(pMain,DisplayBuf,3,52);
		#endif
		
		//EINK_WriteString(DisplayBuf,122,36,"Int");
		//EPD_Write_PCBTemp_Large_Int(pMain,DisplayBuf,102,52);
		//EPD_DisplayURL(pMain,DisplayBuf,&Dummy,0);
    //EPD_Write_DataNo(pMain,DisplayBuf,8,36);
    //EPD_Write_ProbeTemp(pMain,DisplayBuf,8,50);
    EPD_WhiteScreen_ALL(pMain,DisplayBuf);
}

	


void EPDPage_DrawingCheckBoxArray(stMain *pMain,uint8 *DisplayBuf,uint16 X, uint16 Y)
{
	for(int i=pMain->DeviceConfig.CursorMinPosition ; i<pMain->DeviceConfig.CursorMaxPosition ; i++)
		{
		if(pMain->DeviceConfig.CursorPosition == i)
			{
			EPD_Drawing_CheckBox(pMain,DisplayBuf,X,Y + (i-1) * 12,1);
			}
		else
			{
			EPD_Drawing_CheckBox(pMain,DisplayBuf,X,Y + (i-1) * 12,0);
			}
		}
}

void EPDPage_DrawingCurrentItem(stMain *pMain,uint8 *DisplayBuf, uint16 SelectedItem,uint16 X, uint16 Y)
{
for (int i=0;i<20;i++)
	{
	if(SelectedItem == i)
		{
		EINK_WriteFont(DisplayBuf,X,Y + (i-1) * 12, 'v');
		break;
		}
	else
		{
		//EINK_WriteFont(DisplayBuf,100,120 + (i-1) * 12, 'v');
		}	
	}
}

void DeviceConfig_CursorInit(stMain *pMain, uint16 CursorInitPosition, uint16 MinPosition , uint16 MaxPosition)
{
	pMain->DeviceConfig.CursorMaxPosition = MaxPosition;
	pMain->DeviceConfig.CursorMinPosition = MinPosition;
	if(CursorInitPosition <= MaxPosition)
		{
		pMain->DeviceConfig.CursorPosition = CursorInitPosition;
		}
	else
		{
		pMain->DeviceConfig.CursorPosition = MinPosition;
		}
	
}

void EPDPage_BLE_ReadyToService(stMain *pMain)
{
	uint8 DisplayBuf[5888];
	uint8 StringBuf[100] = {0};
	memset(DisplayBuf,0x00,5888);
	
	EINK_WriteString(DisplayBuf,8,56,"Data transfer complete");
	EINK_WriteString(DisplayBuf,8,70,"ready to service!");

	sprintf(StringBuf,"Recv Data : %02x %02x %02x",pMain->BLE.ReceiveData[0] , pMain->BLE.ReceiveData[1] , pMain->BLE.ReceiveData[2]);
	EINK_WriteString(DisplayBuf,8,84,StringBuf);
	//EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
	//EPD_Drawing_Battery(pMain,DisplayBuf);

	
	EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
	EPD_WhiteScreen_ALL(pMain,DisplayBuf);	
}

void EPDPage_BLE_DataResponse(stMain *pMain)
{
	uint8 DisplayBuf[5888];
	uint8 StringBuf[100] = {0};
	stBleSettingReadData data = OTQV2_BLE_BuildSettingResponse();
	
	memset(DisplayBuf,0x00,5888);
	
	EINK_WriteString(DisplayBuf,8,14,"Setting value");
	EINK_WriteString(DisplayBuf,8,28,"Data req detected");

	sprintf(StringBuf,"Interval : %02x",data.Interval);
	EINK_WriteString(DisplayBuf,8,42,StringBuf);
	sprintf(StringBuf,"DP Interval : %02x",data.DisplayInterval);
	EINK_WriteString(DisplayBuf,8,56,StringBuf);
	sprintf(StringBuf,"Device Type : %02x",data.DeviceType);
	EINK_WriteString(DisplayBuf,8,70,StringBuf);
	sprintf(StringBuf,"FW NAME : %.4s",data.FWName);
	EINK_WriteString(DisplayBuf,8,84,StringBuf);
	sprintf(StringBuf,"FW Ver : %05d 0x%04x",data.FWVer , data.FWVer);
	EINK_WriteString(DisplayBuf,8,98,StringBuf);
	#if BLEDataSyntaxVersion == 12
	sprintf(StringBuf,"Serial M : %08d",data.Serial);
	EINK_WriteString(DisplayBuf,8,112,StringBuf);
	#else
	sprintf(StringBuf,"Serial M : %05d 0x%04x",data.Serial_Major , data.Serial_Major);
	EINK_WriteString(DisplayBuf,8,112,StringBuf);
	sprintf(StringBuf,"Serial m : %05d 0x%04x",data.Serial_Minor , data.Serial_Minor);
	EINK_WriteString(DisplayBuf,8,126,StringBuf);
	#endif
	sprintf(StringBuf,"Battery : %03d 0x%02x",data.BatteryRemain , data.BatteryRemain);
	EINK_WriteString(DisplayBuf,8,140,StringBuf);
	sprintf(StringBuf,"P Offset : %05d 0x%04x",data.ProbeOffset , (uint16)data.ProbeOffset);
	EINK_WriteString(DisplayBuf,8,154,StringBuf);
	sprintf(StringBuf,"U : %010d %08x",data.UpTimeSec , data.UpTimeSec);
	EINK_WriteString(DisplayBuf,8,168,StringBuf);
	sprintf(StringBuf,"ErrorFlag : %04x",data.ErrorFlag);
	EINK_WriteString(DisplayBuf,8,182,StringBuf);
	//EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
	//EPD_Drawing_Battery(pMain,DisplayBuf);

	
	EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
	EPD_WhiteScreen_ALL(pMain,DisplayBuf);	
}

void EPDPage_BLE_SettingApply(stMain *pMain)
{
	uint8 DisplayBuf[5888];
	uint8 StringBuf[100] = {0};
	memset(DisplayBuf,0x00,5888);
	
	EINK_WriteString(DisplayBuf,8,56,"Setting Value");
	EINK_WriteString(DisplayBuf,8,70,"Command detected");

	sprintf(StringBuf,"Recv Data : %02x %02x %02x",pMain->BLE.ReceiveData[0] , pMain->BLE.ReceiveData[1] , pMain->BLE.ReceiveData[2]);
	EINK_WriteString(DisplayBuf,8,84,StringBuf);
	//EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
	//EPD_Drawing_Battery(pMain,DisplayBuf);

	
	EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
	EPD_WhiteScreen_ALL(pMain,DisplayBuf);	
}

void EPDPage_BLE_DataSendResult(stMain *pMain)
{
	#ifdef DEBUG
	uint8 DisplayBuf[5888];
	uint8 StringBuf[100] = {0};
	memset(DisplayBuf,0x00,5888);
	
	EINK_WriteString(DisplayBuf,8,56,"BLE Data transfer");
	EINK_WriteString(DisplayBuf,8,70,"Complete");

	sprintf(StringBuf,"Recv Data : %05d %05d",pMain->BLE.ReceiveData[0] <<8 | pMain->BLE.ReceiveData[1] , pMain->BLE.ReceiveData[2] <<8 | pMain->BLE.ReceiveData[3]);
	EINK_WriteString(DisplayBuf,8,84,StringBuf);
	
	sprintf(StringBuf,"Try Data : %05d %05d",pMain->BLE.CallStartPoint , pMain->BLE.CallLastPoint - 1);
	EINK_WriteString(DisplayBuf,8,96,StringBuf);
	
	sprintf(StringBuf,"Send Data : %05d %05d",pMain->BLE.SendStartPoint , pMain->BLE.SendLastPoint);
	EINK_WriteString(DisplayBuf,8,108,StringBuf);

	sprintf(StringBuf,"Spend time : %dmS",pMain->BLE.SendLastTick - pMain->BLE.SendStartTick);
	EINK_WriteString(DisplayBuf,8,120,StringBuf);
	
	sprintf(StringBuf,"Send block : %d",pMain->BLE.SendBlockCount);
	EINK_WriteString(DisplayBuf,8,132,StringBuf);
	
	EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
	EPD_WhiteScreen_ALL(pMain,DisplayBuf);	
	
	pMain->BLE.SendStartTick = 0xffffffff;
	pMain->BLE.SendBlockCount = 0;

	#endif
}

void EPDPage_Factory_Correction(stMain *pMain)
{
uint8 DisplayBuf[5888];
uint8 StringBuffer[100];

memset(DisplayBuf,0x00,5888);

uint8 Dummy=0;

    pMain->Page.MaxPage = 1;
		GetProbeTemp(pMain,1);

		EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
		EPD_Drawing_Battery(pMain,DisplayBuf);
		
		//EPD_Write_DataNo(pMain,DisplayBuf,8,36);
		//EPD_Write_Humidity(pMain,DisplayBuf,8,36);
		//EPD_Write_PCBTemp_Large(pMain,DisplayBuf,8,50);
		//EPD_Write_Humidity_LargeFont(pMain,DisplayBuf,112,50);
		
		switch(pMain->Calib.CalibrationMode)
			{
			case Calibration_Standby:
				EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_Correction);

				#ifdef BELL_ENC
				EPD_Write_PCBTemp_Large(pMain,DisplayBuf,8,50);
				EINK_WriteString(DisplayBuf,8,92,"Correction mode");
				EINK_WriteString(DisplayBuf,8,104,"press QR button to start");
				EINK_WriteString(DisplayBuf,8,116,"data collection");
				memset(StringBuffer,0x00,100);
				sprintf(StringBuffer,"Collection time :%dHours",pMain->Calib.CorrectionWaitTimeMin / 60);
				EINK_WriteString(DisplayBuf,8,140,StringBuffer);				
				EINK_WriteString(DisplayBuf,8,164,"Target : IC Sensor");
				#else
				EPD_Write_ProbeTemp(pMain,DisplayBuf,8,50);
				EINK_WriteString(DisplayBuf,8,92,"Correction mode");
				EINK_WriteString(DisplayBuf,8,104,"press QR button to start");
				EINK_WriteString(DisplayBuf,8,116,"data collection");
				EINK_WriteString(DisplayBuf,8,140,"Collection time :25Hours");
				EINK_WriteString(DisplayBuf,8,164,"Target : Probe");
				#endif


				EPD_Drawing_DeviceInfo(pMain,DisplayBuf);

				EPD_WhiteScreen_ALL(pMain,DisplayBuf);
				break;
				
			case Calibration_InCalibration:
				EEPROM_GetDataLength(&pMain->Sensor.DataNumber);
				EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_Standby);
				#ifdef BELL_ENC
				EINK_WriteString(DisplayBuf,2,84,"Device");
				EPD_Write_PCBTemp_Large(pMain,DisplayBuf,74,84);
				EINK_WriteString(DisplayBuf,2,120,"Humi      ");
				EPD_Write_Humidity_LargeFont(pMain,DisplayBuf,108,120);
				#else
				EINK_WriteString(DisplayBuf,2,96,"Probe Temp");
				EPD_Write_ProbeTempDetail(pMain,DisplayBuf,58,84);
				//EINK_WriteString(DisplayBuf,2,120,"Device Temp");
				EPD_Write_PCBTemp(pMain,DisplayBuf,74,120);
				#endif
				#if 0				// display probe adc value
				EINK_WriteString(DisplayBuf,64,136,"Probe     ");
					EPD_Drawing_SignedNumber(DisplayBuf,113,136,pMain->Sensor.Probe.ProbeADCValue / 100);
					EINK_WriteString(DisplayBuf,134,136,".");
					EPD_Drawing_SignedNumber(DisplayBuf,141,136,pMain->Sensor.Probe.ProbeADCValue % 100);
					#endif
				//EPD_Drawing_BatteryADC(pMain,DisplayBuf,68,184);

				EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
				EPD_Drawing_Battery(pMain,DisplayBuf);
		
				EINK_WriteString(DisplayBuf,8,152,"Correction mode");
				EINK_WriteString(DisplayBuf,8,164,"it will refresh every");
				EINK_WriteString(DisplayBuf,8,176,"                minute");

				//EINK_WriteFont(DisplayBuf,57,176,pMain->Calib.CorrectionWaitCount + 0x30);
				memset(StringBuffer,0x00,100);
				sprintf(StringBuffer,"%2d/ %d",pMain->Calib.CorrectionWaitCount , pMain->Calib.CorrectionWaitTimeMin);
				EINK_WriteString(DisplayBuf,8,188,StringBuffer);
		
				EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
				
				EINK_WriteString(DisplayBuf,64,24,"Final ADC    ");
					EPD_Drawing_SignedNumber(DisplayBuf,130,24,pMain->SnapShot.ProbeADC);
				EINK_WriteString(DisplayBuf,64,36,"Raw ADC    ");
					EPD_Drawing_SignedNumber(DisplayBuf,130,36,pMain->Sensor.Probe.ProbeADCValue);
				EINK_WriteString(DisplayBuf,64,48,"Offset    ");
					EPD_Drawing_SignedNumber(DisplayBuf,130,48,pMain->SnapShot.ProbeADCOffset);
				EINK_WriteString(DisplayBuf,64,60,"R Offset");
					EPD_Drawing_SignedNumber(DisplayBuf,130,60,pMain->Calib.CalData.Offset.ProbeTemp);
				EINK_WriteString(DisplayBuf,64,72,"BATT VOLT");
					EPD_Drawing_SignedNumber(DisplayBuf,130,72,pMain->Device.BatVolt);				
		
				EPD_WhiteScreen_ALL(pMain,DisplayBuf);
				pMain->Sensor.SensorIntervalSec = 60;
				pMain->Device.DisplayIntervalSec = 60;
				OTQV2_Timer_Start(pMain);
				pMain->Key.IntervalProcess_EPD_ForceRefreshFlag = 1;
				pMain->Calib.CorrectionWaitCount++;
				if(pMain->Calib.CorrectionWaitTimeMin < pMain->Calib.CorrectionWaitCount)
					{
					pMain->Calib.CalibrationMode = Calibration_Complete;
					//OTQV2_Timer_AllStop(pMain);
					}
				break;
				
			case Calibration_Complete:
				EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_CorrectionComplete);

				#ifdef BELL_ENC
				EINK_WriteString(DisplayBuf,2,72,"Device");
				EPD_Write_PCBTemp_Large(pMain,DisplayBuf,74,72);
				EINK_WriteString(DisplayBuf,2,108,"Humi      ");
				EPD_Write_Humidity_LargeFont(pMain,DisplayBuf,114,108);
				#else
				EPD_Write_ProbeTemp(pMain,DisplayBuf,8,50);
				#endif
				Init_RecordStructure(pMain);
				pMain->Page.CurrentPage = 1;
				EPDPage_MakeQRWithHuffman(pMain,DisplayBuf);
				EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
				EPD_Drawing_Battery(pMain,DisplayBuf);
				EPD_Write_Serial(pMain,DisplayBuf,8,24);
				#ifdef BELL_ENC
				//EPD_Write_DataNo(pMain,DisplayBuf,8,36);
				//EPD_Write_Humidity(pMain,DisplayBuf,8,36);
				EPD_Write_PCBTemp_Large(pMain,DisplayBuf,8,50);
				EPD_Write_Humidity_LargeFont(pMain,DisplayBuf,112,50);
				#else
				EPD_Write_ProbeTemp(pMain,DisplayBuf,8,50);
				#endif
				EINK_WriteString(DisplayBuf,29,252,"Correction finished");
				EEPROM_WriteDataLength(1);
				EPD_WhiteScreen_ALL(pMain,DisplayBuf);
				OTQV2_Timer_AllStop(pMain);
				pMain->Page.CurrentPage = 0;
				break;
			}
}
	
void EPDPage_Factory_WaitMessage(stMain *pMain)
{
	uint8 DisplayBuf[5888];
	memset(DisplayBuf,0x00,5888);
	
	//EINK_WriteString(DisplayBuf,8,120,"System Initialize");
	//EINK_WriteString(DisplayBuf,8,132,"Please Wait");

	//EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
	//EPD_Drawing_Battery(pMain,DisplayBuf);
	EINK_DrawingImage(DisplayBuf,35,15,WillogLogo1,GetWillogLogo1Width() /8 ,GetWillogLogo1Height());
	EINK_DrawingImage(DisplayBuf,13,140,WillogLogo2,GetWillogLogo2Width() /8 ,GetWillogLogo2Height());
	
	EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
	EPD_WhiteScreen_ALL(pMain,DisplayBuf);	
}
	
void EPDPage_Factory_SelfTest(stMain *pMain)
{
uint8 DisplayBuf[5888];

pMain->Calib.CalibrationMode = Calibration_Standby;

memset(DisplayBuf,0x00,5888);
    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_SelfTest);
    //EPD_Write_Humidity(pMain,DisplayBuf,68,172);
		//pMain->Device.BatVolt = Batt_GetVoltage_int(pMain);
		EINK_WriteString16(DisplayBuf,8,30,"Diagnostic");
    EPD_Write_EPDTest(pMain,DisplayBuf,8,46);
    EPD_Write_EEPROMTest(pMain,DisplayBuf,8,59);
			SHTC3_GetData(pMain);
		EPD_Write_TempSensorPass(pMain,DisplayBuf,8,72);
    EPD_Write_MC3479Test(pMain,DisplayBuf,8,85);
		EPD_Drawing_ProbePass(pMain,DisplayBuf,8,98);
			
    EPD_Drawing_ProbeTest(pMain,DisplayBuf,8,124);
    EPD_Write_TempSensorTest(pMain,DisplayBuf,8,137);
		EPD_Drawing_BatteryVoltage(pMain,DisplayBuf,8,150);

    EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
    EPD_Drawing_Battery(pMain,DisplayBuf);
		
		EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
		
    EPD_WhiteScreen_ALL(pMain,DisplayBuf);
}

void EPDPage_Factory_SelfTest_2(stMain *pMain)
{
uint8 DisplayBuf[5888];

pMain->Calib.CalibrationMode = Calibration_Standby;

memset(DisplayBuf,0x00,5888);
    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_SelfTest2);

		EINK_WriteString16(DisplayBuf,8,30,"Part Initialize");
    EPD_Write_EPDTest(pMain,DisplayBuf,8,46);
    EPD_Write_EEPROMTest(pMain,DisplayBuf,8,59);
			SHTC3_GetData(pMain);
		EPD_Write_TempSensorPass(pMain,DisplayBuf,8,72);
    EPD_Write_MC3479Test(pMain,DisplayBuf,8,85);
		EPD_Drawing_ProbePass(pMain,DisplayBuf,8,98);
			
    EPD_Drawing_ProbeTest(pMain,DisplayBuf,8,124);
    EPD_Write_TempSensorTest(pMain,DisplayBuf,8,137);
		EPD_Drawing_BatteryVoltage(pMain,DisplayBuf,8,150);
		
		EINK_WriteString16(DisplayBuf,29,180,"NFC & BLE");
			EINK_WriteString16(DisplayBuf,42,193,"Connection");
		
    EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
    EPD_Drawing_Battery(pMain,DisplayBuf);
		
		EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
		
    EPD_WhiteScreen_ALL(pMain,DisplayBuf);
		OTQV2_Timer_Start(pMain);
		OTQV2_NFC_Start(pMain);
		BLEActiveTimer_Start(60000);
}
	
void EPDPage_Factory_DeviceConfiguration(stMain *pMain)
{
	uint8 DisplayBuf[5888];

memset(DisplayBuf,0x00,5888);

		// update status
		
		//pMain->Page.CursorPosition = pMain->Page.Interval;
		// common information
    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_Configuration);
    //EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
    EPD_Drawing_Battery(pMain,DisplayBuf);

		EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
    //EPD_Drawing_BatteryADC(pMain,DisplayBuf,68,184);

		// page information
		switch(pMain->DeviceConfig.DeviceConfigPage)
			{
			case DeviceConfig_Page_Main:
			EINK_WriteString(DisplayBuf,8,60,"Select config");
			EINK_WriteString(DisplayBuf,52,132,"Record Interval");
			EINK_WriteString(DisplayBuf,52,144,"Display Interval");
			EINK_WriteString(DisplayBuf,52,156,"Factory Reset");
			EINK_WriteString(DisplayBuf,52,168,"BLE Active mode");
			EINK_WriteString(DisplayBuf,52,180,"BLE Power");
			DeviceConfig_CursorInit(pMain,pMain->DeviceConfig.CursorPosition,DeviceConfig_Page_Interval,DeviceConfig_Page_End);
			EPDPage_DrawingCheckBoxArray(pMain,DisplayBuf,36,120);
				break;

			
			case DeviceConfig_Page_Interval:
				EINK_WriteString(DisplayBuf,8,60,"Measure Interval change");
				EINK_WriteString(DisplayBuf,8,72,"* Interval change will");
				EINK_WriteString(DisplayBuf,8,84,"    erase whole data");
		
				EINK_WriteString(DisplayBuf,52,120-12,"30 Sec");
				EINK_WriteString(DisplayBuf,52,132-12," 1 min");
				EINK_WriteString(DisplayBuf,52,144-12," 5 min");
				EINK_WriteString(DisplayBuf,52,156-12,"10 min");
				EINK_WriteString(DisplayBuf,52,168-12,"15 min");
				EINK_WriteString(DisplayBuf,52,180-12,"20 min");
				EINK_WriteString(DisplayBuf,52,192-12,"30 min");
				EINK_WriteString(DisplayBuf,52,204-12,"60 min");


				DeviceConfig_CursorInit(pMain,pMain->DeviceConfig.CursorPosition,DeviceConfig_Interval_30Sec,DeviceConfig_Interval_End);
				EPDPage_DrawingCheckBoxArray(pMain,DisplayBuf,36,120-12);
				EPDPage_DrawingCurrentItem(pMain,DisplayBuf,pMain->DeviceConfig.Interval,26,120-12);
				break;

			case DeviceConfig_Page_DisplayInterval:
				EINK_WriteString(DisplayBuf,8,60,"EPD interval change");
				//EINK_WriteString(DisplayBuf,8,72,"EPD interval can not set");
				//EINK_WriteString(DisplayBuf,8,84,"below record interval");
		
				EINK_WriteString(DisplayBuf,52,96-12,"Auto");
				EINK_WriteString(DisplayBuf,52,108-12,"Record");
				EINK_WriteString(DisplayBuf,52,120-12," 1 min");
				EINK_WriteString(DisplayBuf,52,132-12," 5 min");
				EINK_WriteString(DisplayBuf,52,144-12,"10 min");
				EINK_WriteString(DisplayBuf,52,156-12,"15 min");
				EINK_WriteString(DisplayBuf,52,168-12,"20 min");
				EINK_WriteString(DisplayBuf,52,180-12,"30 min");
				EINK_WriteString(DisplayBuf,52,192-12,"60 min");
				EINK_WriteString(DisplayBuf,52,204-12,"No update");
				DeviceConfig_CursorInit(pMain,pMain->DeviceConfig.CursorPosition,DeviceConfig_DisplayInterval_Auto,DeviceConfig_DisplayInterval_End);
				EPDPage_DrawingCheckBoxArray(pMain,DisplayBuf,36,96-12);
				EPDPage_DrawingCurrentItem(pMain,DisplayBuf,pMain->DeviceConfig.DisplayInterval,26,96-12);
				break;
				
			case DeviceConfig_Page_EEPROMErase:
				EINK_WriteString(DisplayBuf,8,60,"Factory reset ");
				//EINK_WriteString(DisplayBuf,8,72,"");
				EINK_WriteString(DisplayBuf,8,84,"Press and hold the ");
				EINK_WriteString(DisplayBuf,16,96,"left key for 3 seconds");
				EINK_WriteString(DisplayBuf,48,108,"to reset");
				 
				EINK_WriteString(DisplayBuf,48,132,"Caution!!");
				EINK_WriteString(DisplayBuf,8,144,"factory reset will erase");
				EINK_WriteString(DisplayBuf,8,156,"whole data.");
				//EINK_WriteString(DisplayBuf,8,180,"if you have problem with");
				//EINK_WriteString(DisplayBuf,8,192,"device, call to engineer");
				break;
				
			case DeviceConfig_Page_BLEActive:
				EINK_WriteString(DisplayBuf,8,60,"BLE active setting");

				EINK_WriteString(DisplayBuf,52,108-12,"Always BLE Active");
				EINK_WriteString(DisplayBuf,52,120-12,"NFC active");
				EINK_WriteString(DisplayBuf,52,132-12,"interval active");
				EINK_WriteString(DisplayBuf,52,144-12,"BLE disabled");
				DeviceConfig_CursorInit(pMain,pMain->DeviceConfig.CursorPosition,DeviceConfig_BLEActive_Always,DeviceConfig_BLEActive_End);
				EPDPage_DrawingCheckBoxArray(pMain,DisplayBuf,36,108-12);
				EPDPage_DrawingCurrentItem(pMain,DisplayBuf,pMain->DeviceConfig.BLEActive,26,108-12);
				break;				

			case DeviceConfig_Page_BLEPower:
				EINK_WriteString(DisplayBuf,8,60,"BLE Power setting");

				EINK_WriteString(DisplayBuf,52,108-12,"+0 dBm");
				EINK_WriteString(DisplayBuf,52,120-12,"+1 dBm (Not used)");
				EINK_WriteString(DisplayBuf,52,132-12,"+2 dBm");
				EINK_WriteString(DisplayBuf,52,144-12,"+3 dBm");
				EINK_WriteString(DisplayBuf,52,156-12,"+4 dBm");
				EINK_WriteString(DisplayBuf,52,168-12,"+5 dBm");
				EINK_WriteString(DisplayBuf,52,180-12,"+6 dBm");
				EINK_WriteString(DisplayBuf,52,192-12,"+7 dBm");
				EINK_WriteString(DisplayBuf,52,204-12,"+8 dBm");

				DeviceConfig_CursorInit(pMain,pMain->DeviceConfig.CursorPosition,DeviceConfig_BLEPwr_0dBm,DeviceConfig_BLEPwr_End);
				EPDPage_DrawingCheckBoxArray(pMain,DisplayBuf,36,108-12);
				EPDPage_DrawingCurrentItem(pMain,DisplayBuf,pMain->DeviceConfig.BLEPower,26,108-12);
				break;				
			}
    EPD_WhiteScreen_ALL(pMain,DisplayBuf);	
}

void EPDPage_Factory_Calibration_Probe(stMain *pMain)
{
uint8 DisplayBuf[5888];
	
memset(DisplayBuf,0x00,5888);

		// common information
    //EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_Calibration);
    EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
    EPD_Drawing_Battery(pMain,DisplayBuf);

		EPD_Drawing_DeviceInfo(pMain,DisplayBuf);

		// page information
		//pMain->Sensor.PCBTemp += pMain->Calib.OffsetData.Sensor.PCBTemp;
		//pMain->Sensor.PCBHumi += pMain->Calib.OffsetData.Sensor.PCBHumi;
switch(pMain->Calib.CalibrationMode)
	{
		case Calibration_Standby:
			GetProbeTemp(pMain,1);
			EPD_Write_ProbeTempDetail(pMain,DisplayBuf,40,20);
	    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_CalibrationStandby);
			EINK_WriteString(DisplayBuf,8,60,"Calibration ready");
			EINK_WriteString(DisplayBuf,8,72,"Press QR button to");
			EINK_WriteString(DisplayBuf,8,84,"  start calibration");

			EINK_WriteString(DisplayBuf,8,108,"Temp Offset : ");
				EPD_Drawing_SignedNumber(DisplayBuf,105,108,pMain->Calib.CalData.Offset.ProbeTemp);
			EINK_WriteString(DisplayBuf,8,132,"Calibration status : ");
				EINK_WriteString(DisplayBuf,155,132,pMain->Calib.CalData.Index ? "O" : "X");
			EINK_WriteString(DisplayBuf,8,144,"Index : ");
				EPD_Drawing_SignedNumber(DisplayBuf,71,144,pMain->Calib.CalData.Index);
			EINK_WriteString(DisplayBuf,8,156,"Time stamp : ");
				EPD_Drawing_SignedNumber(DisplayBuf,99,156,pMain->Calib.CalData.TimeStamp_UTC);
			EINK_WriteString(DisplayBuf,8,180,"Target : Probe");

			EINK_WriteString(DisplayBuf,8,192,"BATT VOLT");
				EPD_Drawing_SignedNumber(DisplayBuf,74,192,pMain->Device.BatVolt);

			EPD_WhiteScreen_ALL(pMain,DisplayBuf);	
			break;
			
		case Calibration_InCalibration:
			//
			//pMain->Calib.CalData.Index = 0;
			GetProbeTemp(pMain,0);
		    EPD_Write_ProbeTempDetail(pMain,DisplayBuf,40,20);

	    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_Calibration);
			EINK_WriteString(DisplayBuf,8,60,"Device in calibration");
			EINK_WriteString(DisplayBuf,8,72,"It requires 5min");
				EINK_WriteString(DisplayBuf,22,84,"in stable environment");
			EINK_WriteString(DisplayBuf,8,108,"ADC : ");
				EPD_Drawing_SignedNumber(DisplayBuf,50,108,pMain->Sensor.Probe.ProbeADCValue);
			EINK_WriteString(DisplayBuf,8,120,"TEMP : ");
				EPD_Drawing_SignedNumber(DisplayBuf,105,120,pMain->Sensor.Probe.ProbeTemp);

			EINK_WriteString(DisplayBuf,8,192,"BATT VOLT");
				EPD_Drawing_SignedNumber(DisplayBuf,74,192,pMain->Device.BatVolt);
				
				pMain->Calib.CalibrationWaitCount++;
				if(Probe_CalibrationLoop(pMain))
					{
					NRF_LOG_INFO("T : %d",pMain->Sensor.Probe.ProbeTemp);
					//pMain->Calib.ProbeOffsetData.Sensor.PCBTemp = ProbeTempTable[45] - pMain->Sensor.Probe.ProbeADCValue;
					
					pMain->Calib.CalData.Offset.ProbeTemp = GetOffsetValue(pMain);
					GetProbeTemp(pMain,1);
					if(1)		// always calibration success
					//if((pMain->Sensor.Probe.fProbeTemp) < 25.2f && (pMain->Sensor.Probe.fProbeTemp) > 24.8f) // 0.2degree range
						{
						if(pMain->Device.BatVolt > 2899)
							{
							pMain->Calib.CalibrationFlag = 1;
							pMain->Calib.CalData.Index++;
							pMain->Calib.CalData.TimeStamp_UTC = pMain->Device.Time.UpTimeSec;
							EEPROM_WriteProbeOffsetData(pMain->Calib.CalData);
							pMain->Calib.CalibrationWaitCount = 0;
							pMain->Calib.CalibrationMode = Calibration_Complete;
							}
						else
							{
							pMain->Calib.CalibrationMode = Calibration_Fail;
							}
						}
					}
				EINK_WriteString(DisplayBuf,8,140,"Reference env.");
				EINK_WriteString(DisplayBuf,77,152,"T : 25'C");
				pMain->Sensor.SensorIntervalSec = 10;
				pMain->Device.DisplayIntervalSec = 10;
				OTQV2_Timer_Start(pMain);
				if(pMain->Calib.CalibrationWaitCount==1)			// cal 시작
					{
					EPD_WhiteScreen_ALL(pMain,DisplayBuf);
					}
				else if(pMain->Calib.CalibrationWaitCount > 60)		// cal 실패
					{
					pMain->Calib.CalibrationMode = Calibration_Fail;
					}
				/*
				else if(pMain->Calib.CalibrationWaitCount==0)		// cal 성공
					{
					//GetProbeTemp(pMain,1);
					memset(DisplayBuf,0x00,5888);
						EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
						EPD_Drawing_Battery(pMain,DisplayBuf);
						EPD_Drawing_DeviceInfo(pMain,DisplayBuf);

						EPD_Write_ProbeTempDetail(pMain,DisplayBuf,40,20);
						EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_CalibrationComplete);
						EINK_WriteString(DisplayBuf,8,60,"Calibration Complete");
						EINK_WriteString(DisplayBuf,8,72,"Press QR button twice to");
						EINK_WriteString(DisplayBuf,8,84,"  next process");

						EINK_WriteString(DisplayBuf,8,108,"Temp Offset : ");
							EPD_Drawing_SignedNumber(DisplayBuf,105,108,pMain->Calib.CalData.Offset.ProbeTemp);
						EINK_WriteString(DisplayBuf,8,132,"Calibration status : ");
							EINK_WriteString(DisplayBuf,155,132,pMain->Calib.CalData.Index ? "O" : "X");
							EINK_WriteString16(DisplayBuf,20,174,"Complete");
				
						EINK_WriteString(DisplayBuf,8,192,"BATT VOLT");
							EPD_Drawing_SignedNumber(DisplayBuf,74,192,pMain->Device.BatVolt);
						OTQV2_Timer_AllStop(pMain);
						TriColorLED_SetOff();
						pMain->Calib.CalibrationMode = Calibration_Standby;
						EPD_WhiteScreen_ALL(pMain,DisplayBuf);	
					}
					*/
			break;
			
		case Calibration_Complete:
		
			pMain->Device.BatVolt += 40;
		  GetProbeTemp(pMain,1);
			EPD_Write_ProbeTempDetail(pMain,DisplayBuf,40,20);
	    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_CalibrationComplete);
			EINK_WriteString(DisplayBuf,8,60,"Calibration Complete");
			EINK_WriteString(DisplayBuf,8,72,"Press QR button twice to");
			EINK_WriteString(DisplayBuf,8,84,"  next process");

			EINK_WriteString(DisplayBuf,8,108,"Temp Offset : ");
				EPD_Drawing_SignedNumber(DisplayBuf,105,108,pMain->Calib.CalData.Offset.ProbeTemp);
			EINK_WriteString(DisplayBuf,8,132,"Calibration status : ");
				EINK_WriteString(DisplayBuf,155,132,pMain->Calib.CalData.Index ? "O" : "X");
				EINK_WriteString16(DisplayBuf,20,174,"Complete");
				
			EINK_WriteString(DisplayBuf,8,192,"BATT VOLT");
				EPD_Drawing_SignedNumber(DisplayBuf,74,192,pMain->Device.BatVolt);
			OTQV2_Timer_AllStop(pMain);
			TriColorLED_SetOff();
			pMain->Calib.CalibrationMode = Calibration_Standby;
			EPD_WhiteScreen_ALL(pMain,DisplayBuf);	
			
				break;

		case Calibration_Fail:
		GetProbeTemp(pMain,1);
			EPD_Write_ProbeTempDetail(pMain,DisplayBuf,40,20);
	    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_CalibrationComplete);
			EINK_WriteString(DisplayBuf,8,60,"Calibration Complete");
			EINK_WriteString(DisplayBuf,8,72,"Press QR button twice to");
			EINK_WriteString(DisplayBuf,8,84,"  next process");

			EINK_WriteString(DisplayBuf,8,108,"Temp Offset : ");
				EPD_Drawing_SignedNumber(DisplayBuf,105,108,pMain->Calib.CalData.Offset.ProbeTemp);
			EINK_WriteString(DisplayBuf,8,132,"Calibration status : ");
				EINK_WriteString(DisplayBuf,155,132,pMain->Calib.CalData.Index ? "O" : "X");
				if(pMain->Calib.CalibrationWaitCount > 60)
					{
					EINK_WriteString16(DisplayBuf,20,174,"Timeout Fail");
					}
				else
					{
					EINK_WriteString16(DisplayBuf,20,174,"Voltage Fail");
					}
				
				
			EINK_WriteString(DisplayBuf,8,192,"BATT VOLT");
				EPD_Drawing_SignedNumber(DisplayBuf,74,192,pMain->Device.BatVolt);
			OTQV2_Timer_AllStop(pMain);
			TriColorLED_SetOff();
			pMain->Calib.CalibrationMode = Calibration_Standby;
			EPD_WhiteScreen_ALL(pMain,DisplayBuf);	
				break;			
	}
    //EPD_Write_ProbeTemp(pMain,DisplayBuf,36,120);
    //EPD_Drawing_BatteryADC(pMain,DisplayBuf,68,184);

    

}

void EPDPage_Factory_Calibration_SHTC3(stMain *pMain)
{
uint8 DisplayBuf[5888];
sint8 HumiOffsetTemp8 = pMain->Calib.CalData.Offset.Humi;
		sint16 HumiOffsetTemp = pMain->Calib.CalData.Offset.Humi;
		
memset(DisplayBuf,0x00,5888);

		// common information
    //EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_Calibration);
    EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
    EPD_Drawing_Battery(pMain,DisplayBuf);

		EPD_Drawing_DeviceInfo(pMain,DisplayBuf);

		// page information
		//pMain->Sensor.PCBTemp += pMain->Calib.OffsetData.Sensor.PCBTemp;
		//pMain->Sensor.PCBHumi += pMain->Calib.OffsetData.Sensor.PCBHumi;
switch(pMain->Calib.CalibrationMode)
	{
		case Calibration_Standby:
			EPD_Write_PCBTemp_Large(pMain,DisplayBuf,2,20);
			EPD_Write_Humidity_LargeFont(pMain,DisplayBuf,112,20);
	    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_CalibrationStandby);
			EINK_WriteString(DisplayBuf,8,60,"Calibration ready");
			EINK_WriteString(DisplayBuf,8,72,"Press QR button to");
			EINK_WriteString(DisplayBuf,8,84,"  start calibration");

			EINK_WriteString(DisplayBuf,8,108,"Temp Offset : ");
				EPD_Drawing_SignedNumber(DisplayBuf,105,108,pMain->Calib.CalData.Offset.PCBTemp);
			EINK_WriteString(DisplayBuf,8,120,"Humi Offset : ");
				EPD_Drawing_SignedNumber(DisplayBuf,105,120,pMain->Calib.CalData.Offset.Humi);
			EINK_WriteString(DisplayBuf,8,132,"Calibration status : ");
				EINK_WriteString(DisplayBuf,155,132,pMain->Calib.CalData.Index ? "O" : "X");
			EINK_WriteString(DisplayBuf,8,144,"Index : ");
				EPD_Drawing_SignedNumber(DisplayBuf,71,144,pMain->Calib.CalData.Index);
			EINK_WriteString(DisplayBuf,8,156,"Time stamp : ");
				EPD_Drawing_SignedNumber(DisplayBuf,99,156,pMain->Calib.CalData.TimeStamp_UTC);
			EINK_WriteString(DisplayBuf,8,180,"Target : On board sensor");

			break;
			
		case Calibration_InCalibration:
			pMain->Calib.CalData.Index = 0;			// SHTC3 offset disable
			SHTC3_GetData(pMain);
		    EPD_Write_PCBTemp_Large(pMain,DisplayBuf,2,20);
				EPD_Write_Humidity_LargeFont(pMain,DisplayBuf,112,20);

	    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_Calibration);
			EINK_WriteString(DisplayBuf,8,60,"Device in calibration");
			EINK_WriteString(DisplayBuf,8,72,"It takes 60min");
			//EINK_WriteString(DisplayBuf,8,84,"do not touch the device");
			EINK_WriteString(DisplayBuf,8,108,"Remain time : ");
				EPD_Drawing_SignedNumber(DisplayBuf,105,108,pMain->Calib.CalibrationWaitTimeMin - pMain->Calib.CalibrationWaitCount);
				pMain->Calib.CalibrationWaitCount++;
				if(pMain->Calib.CalibrationWaitTimeMin == pMain->Calib.CalibrationWaitCount)
					{
					NRF_LOG_INFO("T : %d, H : %d",pMain->Sensor.PCBTemp,pMain->Sensor.PCBHumi);
					pMain->Calib.CalData.Offset.PCBTemp = 2500 - pMain->Sensor.PCBTemp;
					pMain->Calib.CalData.Offset.Humi = 35 - pMain->Sensor.PCBHumi;
					pMain->Calib.CalibrationMode = Calibration_Complete;
					if(1)		// always calibration success
					//if(abs(pMain->Calib.OffsetData.Sensor.PCBTemp) < 50 && abs(pMain->Calib.OffsetData.Sensor.PCBHumi < 2)) // normal mode
						{
						pMain->Calib.CalibrationFlag = 1;
						pMain->Calib.CalData.Index++;
						pMain->Calib.CalData.TimeStamp_UTC = pMain->Device.Time.UpTimeSec;
						EEPROM_WriteOffsetData(pMain->Calib.CalData);
						}
					}
				EINK_WriteString(DisplayBuf,8,140,"Reference env.");
				EINK_WriteString(DisplayBuf,77,152,"T : 25'C");
				EINK_WriteString(DisplayBuf,77,164,"H : 35%");
				pMain->Sensor.SensorIntervalSec = 60;
				pMain->Device.DisplayIntervalSec = 60;
				OTQV2_Timer_Start(pMain);
			break;
			
		case Calibration_Complete:
			EPD_Write_PCBTemp_Large(pMain,DisplayBuf,2,20);
			EPD_Write_Humidity_LargeFont(pMain,DisplayBuf,112,20);
	    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_CalibrationComplete);
			EINK_WriteString(DisplayBuf,8,60,"Calibration Complete");
			EINK_WriteString(DisplayBuf,8,72,"Press QR button twice to");
			EINK_WriteString(DisplayBuf,8,84,"  next process");

			EINK_WriteString(DisplayBuf,8,108,"Temp Offset : ");
				EPD_Drawing_SignedNumber(DisplayBuf,105,108,pMain->Calib.CalData.Offset.PCBTemp);
			EINK_WriteString(DisplayBuf,8,120,"Humi Offset : ");
				EPD_Drawing_SignedNumber(DisplayBuf,105,120,HumiOffsetTemp8);
			EINK_WriteString(DisplayBuf,8,132,"Calibration status : ");
				EINK_WriteString(DisplayBuf,155,132,pMain->Calib.CalData.Index ? "O" : "X");
			OTQV2_Timer_AllStop(pMain);
				break;
			
	}
    //EPD_Write_ProbeTemp(pMain,DisplayBuf,36,120);
    //EPD_Drawing_BatteryADC(pMain,DisplayBuf,68,184);

    EPD_WhiteScreen_ALL(pMain,DisplayBuf);	

}

void EPDPage_Factory_Calibration_Result(stMain *pMain)
{
	
uint8 DisplayBuf[5888];

memset(DisplayBuf,0x00,5888);

		EEPROM_GetDataLength(&pMain->Sensor.DataNumber);
    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_Standby);
		#ifdef BELL_ENC
		EINK_WriteString(DisplayBuf,2,72,"Device");
    EPD_Write_PCBTemp_Large(pMain,DisplayBuf,74,72);
		EINK_WriteString(DisplayBuf,2,108,"Humi      ");
    EPD_Write_Humidity_LargeFont(pMain,DisplayBuf,114,108);
		#else
		EINK_WriteString(DisplayBuf,2,72,"Probe Temp");
    EPD_Write_ProbeTempDetail(pMain,DisplayBuf,58,72);
		EINK_WriteString(DisplayBuf,2,108,"Device Temp");
    EPD_Write_PCBTemp_Large(pMain,DisplayBuf,74,108);
		#endif
		EPD_Drawing_Index(pMain,DisplayBuf,30,30);
		#if 0				// display probe adc value
		EINK_WriteString(DisplayBuf,64,136,"Probe     ");
			EPD_Drawing_SignedNumber(DisplayBuf,113,136,pMain->Sensor.Probe.ProbeADCValue / 100);
			EINK_WriteString(DisplayBuf,134,136,".");
			EPD_Drawing_SignedNumber(DisplayBuf,141,136,pMain->Sensor.Probe.ProbeADCValue % 100);
			#endif
    //EPD_Drawing_BatteryADC(pMain,DisplayBuf,68,184);

    EPD_Drawing_BatteryADC(pMain,DisplayBuf,105,2);
    EPD_Drawing_Battery(pMain,DisplayBuf);
		
		EINK_WriteString(DisplayBuf,8,140,"Press both buttons to");
		EINK_WriteString(DisplayBuf,8,152,"Change Setting");

		EINK_WriteString(DisplayBuf,8,164,"Press QR button twice");
		EINK_WriteString(DisplayBuf,8,176,"to start");
		
		EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
		
    EPD_WhiteScreen_ALL(pMain,DisplayBuf);
}


	
void EPDPage_Service_Standby(stMain *pMain)
{
uint8 DisplayBuf[5888];
uint8 StringBuf[100];
float DeviceTemp = 0;


memset(StringBuf,0x00,100);
memset(DisplayBuf,0x00,5888);

if(EPDReturnTimer_CheckActive())				// EPD return timer 작동중일 경우, 중단
	{
	EPDReturnTimer_Stop();
	}

// page infomation
    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_Recording);
    //EPD_Drawing_BatteryPercent(pMain,DisplayBuf,105,0);
    //EPD_Drawing_BatteryADC(pMain,DisplayBuf,68,184);
    EPD_Drawing_Battery(pMain,DisplayBuf);
		EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
		
		
// recording infomation	
		/*				view probe by calibration status
		if(pMain->Calib.CalData.Index)
			{
			EPD_Write_ProbeTemp(pMain,DisplayBuf,74,84);
			EINK_WriteString(DisplayBuf,2,168,"Board Temp.      ");
			EPD_Drawing_PointNumber(DisplayBuf,124,168,pMain->Sensor.PCBTemp/10);
			}
		else
			{
			EPD_Write_PCBTemp_Large(pMain,DisplayBuf,74,84);
			}
    */
		
		#ifdef BELL_ENC
			EINK_WriteString(DisplayBuf,2,96,"Temp.");
			EPD_Write_PCBTemp_Large(pMain,DisplayBuf,74,84);
			EINK_WriteString(DisplayBuf,2,132,"Humi.      ");
			EPD_Write_Humidity_LargeFont(pMain,DisplayBuf,114,120);
		#else
			EINK_WriteString(DisplayBuf,2,96,"Probe Temp.");
			EPD_Write_ProbeTemp(pMain,DisplayBuf,74,84);
			EINK_WriteString(DisplayBuf,2,132,"Device Temp.");
				DeviceTemp = pMain->Sensor.PCBTemp;
				DeviceTemp /= 100;
				sprintf(StringBuf,"%.1f",DeviceTemp);
				EINK_WriteString(DisplayBuf,124,132,StringBuf);
				//EPD_Drawing_PointNumber(DisplayBuf,124,132,pMain->Sensor.PCBTemp/10);
			EINK_WriteString(DisplayBuf,2,144,"Device Humi.");	
				EPD_Write_Humidity(pMain,DisplayBuf,124,144);
		#endif

		#ifdef DEBUG			// display index infomation
		EPD_Drawing_Index(pMain,DisplayBuf,30,160);
		sprintf(StringBuf,"BLE power : %ddBm",pMain->BLE.Power);
		EINK_WriteString(DisplayBuf,30,184,StringBuf);
		#endif

		#ifdef DEBUG
		EINK_WriteString(DisplayBuf,64,24,"Final ADC    ");
			EPD_Drawing_SignedNumber(DisplayBuf,130,24,pMain->SnapShot.ProbeADC);
		EINK_WriteString(DisplayBuf,64,36,"Raw ADC    ");
			EPD_Drawing_SignedNumber(DisplayBuf,130,36,pMain->Sensor.Probe.ProbeADCValue);
		EINK_WriteString(DisplayBuf,64,48,"Offset    ");
			EPD_Drawing_SignedNumber(DisplayBuf,130,48,pMain->SnapShot.ProbeADCOffset);
		EINK_WriteString(DisplayBuf,64,60,"R Offset");
			EPD_Drawing_SignedNumber(DisplayBuf,130,60,pMain->Calib.CalData.Offset.ProbeTemp);
		EINK_WriteString(DisplayBuf,64,72,"BATT VOLT");
			EPD_Drawing_SignedNumber(DisplayBuf,130,72,pMain->Device.BatVolt);
			//EINK_WriteString(DisplayBuf,134,60,".");
			//EPD_Drawing_SignedNumber(DisplayBuf,141,60,pMain->Sensor.Probe.ProbeADCValue % 100);
		#endif


    EPD_WhiteScreen_ALL(pMain,DisplayBuf);
}

#ifdef ForceDataEnable
void EPDPage_Service_ForceWrite_Begin(stMain *pMain)
{
uint8 DisplayBuf[5888];
uint8 StringBuf[100];
float DeviceTemp = 0;


memset(StringBuf,0x00,100);
memset(DisplayBuf,0x00,5888);

if(EPDReturnTimer_CheckActive())				// EPD return timer 작동중일 경우, 중단
	{
	EPDReturnTimer_Stop();
	}

// page infomation
    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_Recording);
    //EPD_Drawing_BatteryPercent(pMain,DisplayBuf,105,0);
    //EPD_Drawing_BatteryADC(pMain,DisplayBuf,68,184);
    EPD_Drawing_Battery(pMain,DisplayBuf);
		EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
		
		

		EINK_WriteString(DisplayBuf,8,24,"EEPROM force download");
		EINK_WriteString(DisplayBuf,8,36,"begin");
		EINK_WriteString(DisplayBuf,8,48,"target index : ");
		EINK_WriteString(DisplayBuf,8,72,"It will take time..");
			EPD_Drawing_SignedNumber(DisplayBuf,130,48,ForceIndexValue);


    EPD_WhiteScreen_ALL(pMain,DisplayBuf);
}
	
void EPDPage_Service_ForceWrite(stMain *pMain)
{
uint8 DisplayBuf[5888];
uint8 StringBuf[100];
float DeviceTemp = 0;


memset(StringBuf,0x00,100);
memset(DisplayBuf,0x00,5888);

if(EPDReturnTimer_CheckActive())				// EPD return timer 작동중일 경우, 중단
	{
	EPDReturnTimer_Stop();
	}

// page infomation
    EPD_Drawing_Recording(pMain,DisplayBuf,PageMode_Recording);
    //EPD_Drawing_BatteryPercent(pMain,DisplayBuf,105,0);
    //EPD_Drawing_BatteryADC(pMain,DisplayBuf,68,184);
    EPD_Drawing_Battery(pMain,DisplayBuf);
		EPD_Drawing_DeviceInfo(pMain,DisplayBuf);
		
		

		EINK_WriteString(DisplayBuf,8,24,"EEPROM force download");
		EINK_WriteString(DisplayBuf,8,36,"complete");
		EINK_WriteString(DisplayBuf,8,48,"index : ");
			EPD_Drawing_SignedNumber(DisplayBuf,130,48,ForceIndexValue);


    EPD_WhiteScreen_ALL(pMain,DisplayBuf);
}

#endif
	
void DisplayRefresh(stMain *pMain)
{
	if(pMain->Device.RunningStatus == RunStat_SelfCalib)			// �˱�� ������
		{
		EPDPage_Factory_Calibration_SHTC3(pMain);
		}
	else if(pMain->Device.RunningStatus == RunStat_SelfCalib_Probe)
		{
		EPDPage_Factory_Calibration_Probe(pMain);
		}
	else if(pMain->Device.RunningStatus == RunStat_Correction)
		{
		EPDPage_Factory_Correction(pMain);
		}
	else if(pMain->Device.RunningStatus == RunStat_SelfTest)
		{
		EPDPage_Factory_SelfTest(pMain);
		}
	else if(pMain->Device.RunningStatus == RunStat_SelfTest2)
		{
		EPDPage_Factory_SelfTest_2(pMain);
		}
	else if(pMain->Device.RunningStatus == RunStat_DeviceConfig)
		{
		EPDPage_Factory_DeviceConfiguration(pMain);
		}
	else if(pMain->Device.RunningStatus == RunStat_StandBy)
		{
		EPDPage_Factory_Calibration_Result(pMain);
		}
  else if(!pMain->Page.CurrentPage)
    {
    EPDPage_Service_Standby(pMain);                 // �기화멜시
    }
	else
    {
    EPDPage_Service_QR(pMain);             // QR�면 �시
    }
}