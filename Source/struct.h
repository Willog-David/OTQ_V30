/*
 * struct.h
 *
 *  Created on: 2020. 9. 24.
 *      Author: Administrator
 */

#ifndef STRUCT_H_
#define STRUCT_H_

#include "types.h"
#include "EEPROM_Internal.h"
#include "huffman.h"
#include "nrf_drv_twi.h"
#include "nrfx_saadc.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "qrcode.h"
#include "e_ink.h"
#include "misc.h"


#include "time.h"
#include "nrf_calendar.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

//#define BLESleepEnable
//#define BELL_ENC
//#define ADC_BAT_MONITOR

#ifdef DEBUG
	//#define ForceDataEnable		// force data active in debug mode only
  #define ForceIndexValue 7199
	//#define ForceIndexValue 14399
	//#define ForceIndexValue 21599
	//#define ForceIndexValue 28799
	//#define ForceIndexValue 35999
#endif


#define UpTimeMode
//#define THD
#define TD

//-- board configuration

#define OTQV2_GPIO_LED_R        (0+8)
#define OTQV2_GPIO_LED_G        (0+11)
#define OTQV2_GPIO_LED_B        (0+12)

//-- board configuration done


/*-------------------nrf driver macro-------------------*/
#define HAL_Delay(x) nrf_delay_ms(x)


ret_code_t twi_write(nrf_drv_twi_t *twi_handler,uint8 TWIAddr,uint16 RegAddr,uint8 RegAddrLen, uint8 *data, uint16 len);
ret_code_t twi_read(nrf_drv_twi_t *twi_handler,uint8 TWIaddr,uint16 RegAddr,uint8 RegAddrLen, uint8 *data, uint16 len);
#define I2C_MEMADD_SIZE_16BIT 2
#define I2C_MEMADD_SIZE_8BIT 1

/*-------------------nrf driver macro end-------------------*/



/*-------------------FW configuration-------------------*/

#define CalInfoTimerSec 3600*6				// record 시작 후  캘리브레이션 정보 표시 시간


#ifdef BELL_ENC
	#define Sensing_O
#else
	#define Sensing_P
#endif

//#define Recording_T
#ifdef BELL_ENC
#define Recording_A					// 기기온도 , 기기습도
#else
#define Recording_F				// 프로브온도, 기기온도
#endif
//#define Recording_H
//#define Recording_S

#define Scale_O
//#define Scale_S
//#define Scale_I


//#define Service_L
#ifdef BELL_ENC
	#define Service_M
#else
 //#define Service_B
 #define Service_P				// dev url
 #endif



/*-------------------FW configuration end-------------------*/






/*-------------------Sub FW configuration -------------------*/
//#define Base41Encoding

#define BLE_AutoDisConnectionTimerSec 180

#define MaxEEPROMAddress 7200

#define ProbeCalibrationLoopCount 30

#define MaxPageData (TempDataLength/3)*2

#ifdef Base41Encoding
	#define TempDataLength 2310
	#define HumiDataLength 0
#else
	#define TempDataLength 1500
	#define HumiDataLength 0
#endif


// --------------------------------------------------------------

#ifdef Sensing_O
	#define Display_ButtonDownside
#else
	#define Display_ButtonUpSide
#endif

#ifdef Scale_S
#define SizeOfSerial 10
#else
#define SizeOfSerial 8
#endif

#ifdef Service_B
	#define OORModeEnable
	#define QRViewMode 4
#else
	#define QRViewMode 3
#endif

#ifdef BELL_ENC
	#define MaxPageRef 10
#else
	#define MaxPageRef 7
#endif

						// QR view mode 0 = QRv13, without inform
						// QR view mode 1 = QRv12, with only temperature inform
						// QR view mode 2 = QRv11 with temp & QRv13 without temp
						// QR view mode 3 = QRv13, with full inform
						// QR view mode 4 = QRv11 With temp & QRv13 with full inform



/*-------------------Sub FW configuration end-------------------*/


//-- enum define
typedef enum
{
	SAADC_Channel_Battery = 1,
	SAADC_Channel_Probe = 2,
}SAADC_Channel_Mode;

typedef enum
{
	KeyButton_Left,
	KeyButton_LeftLong,
	KeyButton_LeftTwice,
	KeyButton_LeftHidden,
	KeyButton_Right,
	KeyButton_RightLong,
	KeyButton_RightTwice,
	KeyButton_RightHidden,
	KeyButton_Both,
	KeyButton_BothLong,
}KeyButton;

typedef enum
{
	RunStat_Factory=1,
	RunStat_Idle,
	RunStat_StandBy,
	RunStat_Logging,
	RunStat_Logging_Connected,
	RunStat_SelfTest,
	RunStat_SelfTest2,
	RunStat_SelfTest2_Connected,
	RunStat_SelfCalib,
	RunStat_SelfCalib_Probe,
	RunStat_Correction,
	RunStat_DeviceConfig,
	RunStat_Booting,
	RunStat_Button
}RunStat;

typedef enum		// please refer nrf5240_bitfields.h
{
	BLE_TX_P00 = 0,
	BLE_TX_P20 = 2,
	BLE_TX_P30,
	BLE_TX_P40,
	BLE_TX_P50,
	BLE_TX_P60,
	BLE_TX_P70,
	BLE_TX_P80,
}BLE_TxPower;

typedef enum
{
  BLE_DataMode_SHORT = 1,
  BLE_DataMode_LONG = 2,
}BLE_DataMode;

/*
const uint16 IntervalValueSec[] =
	{
	0,
	60,
	300,
	600,
	900,
	1200,
	1800,
	3600
	};
*/
typedef enum
{
	DeviceConfig_Page_Main = 1,
	DeviceConfig_Page_Interval,
  DeviceConfig_Page_DisplayInterval,
	DeviceConfig_Page_EEPROMErase,
	DeviceConfig_Page_BLEActive,
	DeviceConfig_Page_BLEPower,
	DeviceConfig_Page_End,
}DeviceConfig_Page;

typedef enum
{
	DeviceConfig_Interval_30Sec = 1,
  DeviceConfig_Interval_1Min,
  DeviceConfig_Interval_5Min,
	DeviceConfig_Interval_10Min,
	DeviceConfig_Interval_15Min,
	DeviceConfig_Interval_20Min,
	DeviceConfig_Interval_30Min,
	DeviceConfig_Interval_60Min,
	DeviceConfig_Interval_End,
}DeviceConfig_Interval;

typedef enum
{
	DeviceConfig_DisplayInterval_Auto = 1,
	DeviceConfig_DisplayInterval_FU,
  DeviceConfig_DisplayInterval_1Min,
  DeviceConfig_DisplayInterval_5Min,
	DeviceConfig_DisplayInterval_10Min,
	DeviceConfig_DisplayInterval_15Min,
	DeviceConfig_DisplayInterval_20Min,
	DeviceConfig_DisplayInterval_30Min,
	DeviceConfig_DisplayInterval_60Min,
	DeviceConfig_DisplayInterval_None,
	DeviceConfig_DisplayInterval_End,
}DeviceConfig_DisplayInterval;

typedef enum
{
	DeviceConfig_BLEActive_Always = 1,
	DeviceConfig_BLEActive_NFC,
	DeviceConfig_BLEActive_Interval,
	DeviceConfig_BLEActive_Disabled,
	DeviceConfig_BLEActive_End,
}DeviceConfig_BLEActive;

typedef enum
{
	DeviceConfig_BLEPwr_0dBm = 1,
	DeviceConfig_BLEPwr_1dBm,		// not used. please refer nrf5240_bitfields.h
	DeviceConfig_BLEPwr_2dBm,
	DeviceConfig_BLEPwr_3dBm,
	DeviceConfig_BLEPwr_4dBm,
	DeviceConfig_BLEPwr_5dBm,
	DeviceConfig_BLEPwr_6dBm,
	DeviceConfig_BLEPwr_7dBm,
	DeviceConfig_BLEPwr_8dBm,
	DeviceConfig_BLEPwr_End,
}DeviceConfig_BLEPwr;

typedef enum
{
  Temperature_Celcius = 1,
  Temperature_Farenheit = 2,
}Temperature_Type;

typedef enum
{
  IdleMode_ADVOff_NFCOff = 1,
  IdleMode_ADVOn_NFCOff = 2,
  IdleMode_ADVOff_NFCOn = 3,
  IdleMode_ADVOn_NFCOn = 4,
}IdleMode;

typedef enum
{
  Calibration_Standby = 1,
  Calibration_InCalibration = 2,
  Calibration_Complete = 3,
	Calibration_Fail = 4,
}CalibMode;

typedef enum
{
  CalibView_OffsetApply = 1,
  CalibView_WithOutOffset= 2,
}CalibViewMode;

typedef enum
{
	RecordMode_DeviceInFactoryMode = 0,
	RecordMode_DeviceInRecordMode,
}RecordMode;

typedef enum
{
	RefreshTempAres_MMM_M40 = 1,
	RefreshTempAres_M40_M20,
	RefreshTempAres_M20_M01,
	RefreshTempAres_P00_P02,
	RefreshTempAres_P02_P08,
	RefreshTempAres_P08_P15,
	RefreshTempAres_P15_P30,
	RefreshTempAres_P30_P40,
	RefreshTempAres_P40_P60,
	RefreshTempAres_P60_P120,
	RefreshTempAres_P120_PPP,
	RefreshTempAres_end,
	
}RefreshTempArea;


typedef enum					// 대기모드 진입을 위한 타이머 플래그
	{
	HiddenKey_NotActive = 1,
	HiddenKey_Close,
	HiddenKey_Open
	}HiddenKeyMode;

//-- enum define done



//-- struct define
#pragma pack(push,1)
typedef struct
{
	uint16 Start;					// 55AA

	uint8 SerialNumber[SizeOfSerial];

	uint8 type;						// 0x4e

	uint32 Time;					// 업타임

	uint16 TimeIntervalSec;			// 데이터 기록 간격 (초)

	uint8 PageNumber;			// 페이지 번호

	uint8 BatteryGauge;				// 배터리 잔량

	uint16 FirstIndex;				//  페이지의 첫번째 데이터 인덱스
  uint16 LastIndex;				//  현재 페이지의 마지막 데이터 인덱스

	uint8 Data[TempDataLength];			// 데이터. 
	uint8 HumiData[HumiDataLength];			// 데이터. 

	uint16 End;
}stLogData;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
	{
	uint32 TimeStamp;
	sint16 Temp;
	sint8 Humi;
	}stTemperatureInBle;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
	{
	uint8 Battery;
	uint16 Index;
	stTemperatureInBle Data[3];
	}stBleAdv;
#pragma pack(pop)
#define BLEDataSyntaxVersion 12   			// rev 1.2

#pragma pack(push,1)
typedef struct
{
	uint16 SyntaxVer;
	DeviceConfig_Interval Interval;
	DeviceConfig_DisplayInterval DisplayInterval;
	uint8 DeviceType;
	uint8 FWName[4];
	uint16 FWVer;
	uint32 Serial;
	uint8 BatteryRemain;
	sint16 ProbeOffset;
	uint32 UpTimeSec;
	uint16 ErrorFlag;
	uint16 CalIndex;
	uint16 FirstIndex;
	uint16 LastIndex;
}stBleSettingReadData;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
	DeviceConfig_Interval Interval;
	DeviceConfig_DisplayInterval DisplayInterval;
	uint8 SleepFlag;
}stBleSettingWriteData;
#pragma pack(pop)

typedef struct
{
  sint16 X;
  sint16 Y;
  sint16 Z;
}Accelo;

typedef struct
{
  bool LEDEnable;         // LED active when interval
  uint16 LEDInterval;     // LED blink interval
  bool LowBattAlert;      // low battery alert enable (red LED indicator)
  bool LED_LogActive;     // LED log active (?)
  uint16 NFCTagInterval;  // NFC tag interval. 1S / 2S / 5S / 10S
}LEDSetting;

#ifdef BELL_ENC
#pragma pack(push,1)
typedef struct
{
  uint32 TimeStamp_UTC;
  uint16 Index;
  struct
  {
    sint16 PCBTemp;
    uint8 PCBHumi;
  }Sensor;
//  uint8 EventCode;
}EEPROM_Datastructure;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
	uint16 DataLength;
	struct
		{
		sint16 PCBTemp;
		uint8 PCBHumi;
		}Sensor[MaxPageData];
}EEPROM_RawDatastructure;
#pragma pack(pop)

#pragma pack(push,1)
#else
#pragma pack(push,1)
typedef struct
{
  uint32 TimeStamp_UTC;
  uint16 Index;
  struct
  {
    sint16 ProbeTemp;
    sint8 PCBTemp;
  }Sensor;
}EEPROM_Datastructure;
#pragma pack(pop)



#pragma pack(push,1)
typedef struct
{
	uint16 DataLength;
	struct
		{
		sint16 PCBTemp;
		uint8 PCBHumi;
		}Sensor[MaxPageData];
}EEPROM_RawDatastructure;
#pragma pack(pop)
#endif


#pragma pack(push,1)
typedef struct
{
  uint32 TimeStamp_UTC;
  uint16 Index;
  struct
  {
    sint16 ProbeTemp;
    sint16 PCBTemp;
		sint8 Humi;
  }Offset;
}CalibrationData;

#pragma pack(pop)

typedef struct        // main structure
{
  struct
  {
  uint8 DeviceName[30];    // BLE device name               default : WIL OTQN2
  uint8 MAC[6];            // BLE Mac address               default : 
  uint32 UUID[4];           // BLE UUID                     default : 
  uint32 Serial;            // BLE device Serial number     default : XXXX
  uint16 AdvPeriod;         // advertise period             default : 1000mS 
  BLE_TxPower Power;        // BLE tx power                 default : 0dBm
  uint8 PassWord[8];        // BLE connection password      default : 12345678
  BLE_DataMode Mode;        // BLE data mode. SHORT or LONG
	uint8 ReceiveData[100];
	uint16 CallStartPoint;
	uint16 CallLastPoint;
	uint16 SendStartPoint;
	uint16 SendLastPoint;
	uint32 SendStartTick;
	uint32 SendLastTick;
	uint16 SendBlockCount;
	uint8 AdvFlag;
	uint8 BLEIntervalSlot;
	uint32 BLEActiveUpTime;
	stBleAdv AdvPacket;
  }BLE;

  struct
  {
    uint32 SensorIntervalSec;
    struct
      {
				float fProbeTemp;
        sint16 ProbeTemp;
        uint16 ProbeBuffer[100];
        uint16 ProbeADCValue;
        sint16 ProbeCorrectionValue;
      }Probe;
    sint16 PCBTemp;
    sint16 PCBTemp_F;
    uint8 PCBHumi;
    struct
      {
        sint16 PCBTemp;
        sint16 PCBTemp_F;
        uint8 PCBHumi;
      }Prev;

    Temperature_Type TempType;
    Accelo Accelo;
    struct
      {
        bool TimeEnable;
        uint32 TimeStamp;     // UTC
        bool SensorOffset;
        uint16 TempOffset;
        uint16 HumiOffset;
      }Calib;

    uint16 DataNumber;
  }Sensor;

  struct
  {
    struct
      {
        uint32 LogStartTime;
        uint32 LogEndime;
        uint32 UpTimeSec;
        struct tm Calendar;
				uint32 RecordBeginTime;
      }Time;

    IdleMode IdleMode;
    LEDSetting LED;
    uint32 BatVolt;
    uint16 BatVoltBuffer[100];
    uint8 BatLevel;
    RunStat RunningStatus;
		uint32 DisplayIntervalSec;
  }Device;

  struct
    {
      uint16 HWVer;
      uint8 FWVer[12];
      uint16 BLEVer;
      uint16 SDVer;
      uint16 NFCTagVer;
      uint8 Serial[10];
			uint32 RawSerial;
    }DevInfo;

  struct
    {
			uint16 BootIndex;
			uint16 FirstIndex;
      uint16 CurrentPage;
      uint16 MaxPage;
      uint16 OriginPoint;
      uint16 ValidDataCount;
      uint16 DataCut;
      uint16 JumpCount;
			uint16 LastDataIndex;
			uint16 PageDataLength;
    }Page;
		
	struct
		{
			DeviceConfig_Page DeviceConfigPage;
			DeviceConfig_Interval Interval;
			DeviceConfig_DisplayInterval DisplayInterval;
			DeviceConfig_BLEActive BLEActive;
			DeviceConfig_BLEPwr BLEPower;
			uint16 CursorPosition;
			uint16 CursorMaxPosition;
			uint16 CursorMinPosition;
			uint16 CursorValue;
		}DeviceConfig;

  struct
    {
			bool KeyProcessCallFlag;
			bool IntervalProcess_CallFlag;
			bool IntervalProcess_EPD_CallFlag;
			bool IntervalProcess_EPD_ForceRefreshFlag;
			bool IntervalProcess_ADC_CallFlag;
			bool IntervalProcess_GetVBat_CallFlag;
			bool LEDIndicatorCallFlag;
      uint8 KeyPushCount;
      bool LeftKeyPushFlag;
			bool LeftHiddenKeyPushFlag;
			bool RightKeyPushFlag;
			bool RightHiddenKeyPushFlag;
      bool WakeupByKeyPush;
    }Key;

  struct
    {
      bool AlreadySleepFlag;
			RecordMode RecordModeFlag;
    }Status;
	struct
		{
			bool CalibrationFlag;
			CalibrationData CalData;
			uint16 ProbeCalibData[ProbeCalibrationLoopCount];
			CalibMode CalibrationMode;
			CalibViewMode CalibView;
			uint16 CalibrationWaitTimeMin;
			uint16 CalibrationWaitCount;
			uint16 CorrectionWaitTimeMin;
			uint16 CorrectionWaitCount;
		}Calib;
	struct
		{
		uint8 BatADCValidFlag;
		uint8 KeyActiveFlag;
		uint8 BLEActiveFlag;
		uint16 BLEGattHandler;
		uint8 BLEGetDataStatusFlag;
		uint8 NFCFieldOnFlag;
		uint8 NFCFieldClearFlag;
		uint8 SerialQR_OffsetViewFlag;
		HiddenKeyMode HiddenKeyActiveFlag;
		uint8 CallBleSleepFlag;
		}Flags;
	
	struct
		{
		float ProbeADC;
		float ProbeADCOffset;
		}SnapShot;
		
		stLogData *pLogData;
}stMain;                // main structure

//-- struct define done


uint32_t getTickCount();
#define HAL_GetTick() getTickCount() 
#endif /* STRUCT_H_ */

