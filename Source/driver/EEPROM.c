/*
 * EEPROM.c
 *
 *  Created on: Nov 19, 2020
 *      Author: kaohu
 */

#include "EEPROM.h"
#include "nrf_twi_mngr.h"
#include "OTQV30_WDT.h"

nrf_drv_twi_t *hi2c_handler_eeprom;
#define EEPROM_ICAddr 0x50
//#define EEPROM_ICAddr 0x55			// fake address. for debug
#define EEPROM_MAX_SIZE_BIT 524288
#define EEPROM_MAX_SIZE_BYTE (EEPROM_MAX_SIZE_BIT/8)
#define EEPROM_PAGE_SIZE 64
//#define ConstTestMode
#define MaxRecord MaxEEPROMAddress



#ifdef ConstTestMode
const sint16 TestTempValue[]=
{
};
#endif

void EEPROM_WriteData(uint8* buf, uint16 StartAddr,uint16 len)
{
#if 1       // original code
uint16_t page_addr=0;
uint16_t bytes_to_write=0;
uint16_t bytes_written=0;
ret_code_t TWIRet=0;
    
page_addr = StartAddr / EEPROM_PAGE_SIZE;
bytes_to_write = len;
bytes_written = 0;

if ((StartAddr + bytes_to_write) / EEPROM_PAGE_SIZE != page_addr)     // 데이터가 페이지를 넘어가는 경우
  {
  bytes_to_write = EEPROM_PAGE_SIZE - StartAddr%EEPROM_PAGE_SIZE;
	//twi_master_write_bytes(EEPROM_ICAddr,StartAddr,buf,bytes_to_write);
  TWIRet = twi_write(hi2c_handler_eeprom,EEPROM_ICAddr,StartAddr,I2C_MEMADD_SIZE_16BIT,buf,bytes_to_write);
	APP_ERROR_CHECK(TWIRet);
  HAL_Delay(6);
  bytes_written = bytes_to_write;
  bytes_to_write = len - bytes_written;
  //NRF_LOG_INFO("ADDR : %x length : %d, Cut length : %d\n",StartAddr,len,bytes_to_write);
  }

//twi_master_write_bytes(EEPROM_ICAddr,StartAddr + bytes_written,&buf[bytes_written],bytes_to_write);
TWIRet = twi_write(hi2c_handler_eeprom,EEPROM_ICAddr,StartAddr + bytes_written,I2C_MEMADD_SIZE_16BIT,&buf[bytes_written],bytes_to_write);
APP_ERROR_CHECK(TWIRet);
HAL_Delay(6);

#else   
    uint8_t buffer[EEPROM_PAGE_SIZE + 2];
    uint16_t page_addr;
    uint16_t bytes_to_write;
    uint16_t bytes_written;
    
    page_addr = addr / EEPROM_PAGE_SIZE;
    bytes_to_write = len;
    bytes_written = 0;
    
    while (bytes_to_write > 0)
    {
        // 페이지 경계에 도달하면 남은 데이터만큼만 쓰고, 나머지는 다음 페이지에 쓴다.
        if ((addr + bytes_written) / EEPROM_PAGE_SIZE != page_addr)
        {
            bytes_to_write = len - bytes_written;
            page_addr = (addr + bytes_written) / EEPROM_PAGE_SIZE;
        }
        
        memcpy(&buffer[0], &data[bytes_written], bytes_to_write);
        
        //ret_code_t err_code = twi_write(hi2c_handler_eeprom, EEPROM_ICAddr, addr, buffer, bytes_to_write , false);
        //APP_ERROR_CHECK(err_code);
        twi_write(hi2c_handler_eeprom, EEPROM_ICAddr, addr, buffer, bytes_to_write , false);
        
        nrf_delay_ms(10); // EEPROM의 쓰기 완료 시간을 고려하여 지연
        
        bytes_written += bytes_to_write;
        addr += bytes_to_write;
        bytes_to_write = 0;
    }
#endif
}

void EEPROM_ReadData(uint8 *buf, uint16 StartAddr, uint16 len)
{
  //NRF_TWI_MNGR_WRITE(EEPROM_ICAddr,buf,len,NRF_TWI_MNGR_NO_STOP);
  //NRF_TWI_MNGR_READ(EEPROM_ICAddr,buf,len,0);
  //EEPROM_WriteData(&writetmp,i,1);
  //twi_write(hi2c_handler_eeprom,EEPROM_ICAddr,StartAddr,I2C_MEMADD_SIZE_16BIT,buf,0);   // memory address write
  //HAL_Delay(10);
	//twi_master_read_bytes(EEPROM_ICAddr,StartAddr,buf,len);
    twi_read(hi2c_handler_eeprom,EEPROM_ICAddr,StartAddr,I2C_MEMADD_SIZE_16BIT,buf,len);
}

void EEPROM_Init(stMain *pMain, nrf_drv_twi_t *i2cHandler)
{
	hi2c_handler_eeprom=i2cHandler;
}


void Init_EEPROM(stMain *pMain, nrf_drv_twi_t *i2cHandler)
{
	hi2c_handler_eeprom=i2cHandler;
}

#define EEPROM_BlockSize 4

void EEPROM_ClearData(void)
{
	uint8 dummy[1000];
	memset(dummy,0x00,1000);
	for(int i=0;i<32;i++)
	{
		EEPROM_WriteData(dummy,i*1000,1000);
	}
}

void EEPROM_GetInterval(uint32 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xfff9,4);
#ifdef BELL_ENC
	if(*Data==0)*Data=600;
	if(*Data==0xffffffff)*Data=600;
	if(*Data>86400)*Data=600;
#else
	if(*Data==0)*Data=900;
	if(*Data==0xffffffff)*Data=900;
	if(*Data>86400)*Data=900;
#endif
	//*Data=600;
}

void EEPROM_WriteInterval(uint32 *Data)
{
	EEPROM_WriteData((uint8*)Data,0xfff9,4);
}


void EEPROM_GetRunningStat(uint8 *Data)
{
	EEPROM_ReadData(Data,0xfffd,1);
	if(*Data==0xff)*Data=0;
}

void EEPROM_WriteRunningStat(uint8 Data)
{
	RunStat Verify=0;
	for(int i=0;i<10;i++)
	{
		EEPROM_WriteData(&Data,0xfffd,1);
		EEPROM_GetRunningStat(&Verify);
		if(Verify == Data)i=0xffff;
	}

}

void EEPROM_GetNumberOfRecord(uint16 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xfffe,2);
}

void EEPROM_WriteNumberOfRecord(uint16 *Data)
{
	EEPROM_WriteData((uint8*)Data,0xfffe,2);
}

void EEPROM_WriteCurrentPage(uint16 *Data)
{
	EEPROM_WriteData((uint8*)Data,0xffed,2);
}

void EEPROM_GetCurrentPage(uint16 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffed,2);
	if(*Data==0xffff)*Data=1;
}

void EEPROM_WriteLastPhoto(uint8 *Data)
{
	EEPROM_WriteData((uint8*)Data,0xffec,1);
}

void EEPROM_GetLastPhoto(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffec,1);
	if(*Data==0xff)*Data=1;
}

void EEPROM_WritePrevTemp(uint16 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffe3,2);
}

void EEPROM_GetPrevTemp(uint16 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffe3,2);
	if(*Data==0xffff)*Data=1;
}

void EEPROM_WriteTempCorrection(uint16 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffe1,2);
}

void EEPROM_GetTempCorrection(uint16 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffe1,2);
	if(*Data==0xffff)*Data=1;
}

void EEPROM_WritePrevHumi(uint8 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffe5,1);
}

void EEPROM_GetPrevHumi(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffe5,1);
	if(*Data==0xff)*Data=1;
}

void EEPROM_WriteIntervalCount(uint32 *Data)
{
	EEPROM_WriteData((uint8*)Data,0xffe6,4);
}

void EEPROM_GetIntervalCount(uint32 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffe6,4);
	if(*Data==0xffffffff)*Data=1;
}

void EEPROM_WriteKeyFlag(uint8 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffe0,1);
}

void EEPROM_GetPrevKeyFlag(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffe0,1);
	if(*Data==0xff)*Data=1;
}


void EEPROM_WriteDataLength(uint16 Data)
{
	uint16 ReadData=0;
	for(int i=0;i<100;i++)
		{
		EEPROM_WriteData((uint8*)&Data,0xffde,2);
		EEPROM_ReadData((uint8*)&ReadData,0xffde,2);
		if(Data == ReadData)
			{
			break;
			}
		}
	
}

void EEPROM_GetDataLength(uint16 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffde,2);
	if(*Data==0xffff)*Data=1;
}

void EEPROM_WriteUpTime(uint32 *Data)
{
	EEPROM_WriteData((uint8*)Data,0xffda,4);
}

void EEPROM_GetUpTime(uint32 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffda,4);
	//if(*Data==0xffffffff)*Data=1;
}


#define DefaultSerialDataAddress 0		//
//#define DefaultSerialDataAddress 1000
#if defined Recording_T
uint16 EEPROM_GetSerialData(stMain *pMain,sint8 *Data)
{
	sint16 CurrentData=0,oldData=0;
	sint16 DeltaData=0;
	uint16 DataLength=0;
	sint8 *pStartPoint=Data;
	sint8 *pData=Data;
	uint32 StartAddress=0;
	uint16 ret;
	uint16 LastJumpAddress=0,JumpCount=0,FirstJumpAddress = 0;
	uint16 DataLimit=TempDataLength-1;
	uint16 LastData;
        EEPROM_Datastructure TempData;

        memset(&TempData,0x00,sizeof(TempData));

	EEPROM_GetDataLength(&DataLength);

	LastData = DataLength;

	memset(Data,63,TempDataLength);

#ifdef ConstTestMode		// 고정�수 �이�용
	DataLength = sizeof(TestTempValue)/2;
	if(DataLength > TempDataLength - 20)
		{
		StartAddress = (DataLength - (TempDataLength - 20))*2 + pMain->Page.DataCut*2;
		DataLength = TempDataLength - 20 - pMain->Page.DataCut;
		}
	else
		{
		StartAddress = pMain->Page.DataCut*2;
		DataLength = DataLength - pMain->Page.DataCut;
		}
	ret=DataLength;
	CurrentData = TestTempValue[sizeof(TestTempValue) / 2 -1];
#else						// EEPROM �이로드 
	if(DataLength > DataLimit - 20)							// �이갈�가 �재 가�한 범위 �상경우
		{
		DataLength = DataLimit - 20;						// �재�이갈� 최���지
		}
	else													// �이갈�가 �재 가�한 범위 �내경우
		{
		DataLength = DataLength;							// �재�이갈� 지
		}
	ret=DataLength;											// 반환 �이�길지
	CurrentData = pMain->Sensor.PCBTemp;
	//EEPROM_ReadData((uint8*)&CurrentData, (LastData *2 ) % (MaxRecord*2),2);		// 첰이로드
#endif

	// --------------------  첰이기록  -------------------- //
	CurrentData = CurrentData - CurrentData %10;		// �수2째자릜거
	*pData = (CurrentData>>8) & 0xff;					// �온 �위 갅력
	*pData |=0x80;										// �온 �디케�터 추�
	pData++;											// �인증�
	*pData = (CurrentData) & 0xff;						// �온 �위 갅력
	pData++;											// �인증�
	// --------------------  첰이기록 종료 -------------------- //

	for(int i=1;i<DataLength;i++)						// �이길이반큼 반복
	{
		oldData = CurrentData;							// �재 측정�도 추� 기록

#ifdef ConstTestMode									// �수 모드�때
		CurrentData = TestTempValue[sizeof(TestTempValue)/2 -2 - i];					// �수 갅력
#else
		//if(i == DataLength)								// �이�의 마���부분일
		//{
			//CurrentData = pMain->CurrentTemp;			// 마���측정갅력
		//}
		//else											// �이마�막이 �닐
		//{
                EEPROM_ReadData((uint8*)&TempData,(((LastData - i)%MaxRecord) * sizeof(TempData)),sizeof(TempData));
                 CurrentData = TempData.Sensor.PCBTemp;
                 //NRF_LOG_INFO("%d : %d",i,TempData.Sensor.PCBTemp);

                //EEPROM_ReadData((uint8*)&CurrentData,((LastData - i) * 2) % (MaxRecord*2),2);  // EEPROM����불러�기
//		}

#endif

		CurrentData = CurrentData - CurrentData %10;	// �력�이�수2째자릜거
		DeltaData = CurrentData - oldData ;				// ����계산
		if(DeltaData >= -630 && DeltaData <= 630)			// ���도 검��
		{
			*pData = (DeltaData / 10) + 63;				// ���도 �용����계산
		}
		else											// �측�도 검��
		{
			ret++;
			if((pData - pStartPoint)%2)			// ��번��때
			{
				*pData = 0xff;					// �이공간 버림
				pData++;
			}
			*pData = CurrentData>>8 & 0xff;
			*pData |=0x80;						// �측�도 �시
			pData++;
			*pData = CurrentData & 0xff;

		}
		//pMain->CurrentTemp = CurrentData;
		pData++;								// �인증�
		if( (pData - pStartPoint) > DataLimit)i = DataLength + 1;	// �인�� 최� �이�한 범위류는경우, 반복��종료
	}
	ret = pData - pStartPoint;
	if(ret > DataLimit) ret = DataLimit;


	return ret;
}
#elif defined Recording_A  | defined Recording_F
uint16 EEPROM_GetSerialData(stMain *pMain,sint8 *Data)
{
	sint16 CurrentData,oldData=0;
	uint8 CurrentHumi, oldHumi=0;
	sint16 DeltaData=0;
	sint8 DeltaHumi=0;
	uint16 DataLength=0;
	sint8 *pStartPoint=Data;
	sint8 *pData=Data;
	uint16 StartAddress=0;
	volatile uint16 ret;
	uint16 LastJumpAddress=0,JumpCount=0,FirstJumpAddress = 0;
	uint8 JumpEnable=0;
  EEPROM_Datastructure TempData;
  uint16 LastData = 0;

	EEPROM_GetDataLength(&DataLength);
	//DataLength=8308;
#if 0						// for data dump
	for(int i=0;i<DataLength;i+=10)
	{
		EEPROM_ReadData((uint8*)&CurrentData[0],DefaultSerialDataAddress + (i * 3),2);
		EEPROM_ReadData((uint8*)&CurrentHumi[0],DefaultSerialDataAddress + (i * 3) + 2,1);
		EEPROM_ReadData((uint8*)&CurrentData[1],DefaultSerialDataAddress + ((i+1) * 3),2);
		EEPROM_ReadData((uint8*)&CurrentHumi[1],DefaultSerialDataAddress + ((i+1) * 3) + 2,1);
		EEPROM_ReadData((uint8*)&CurrentData[2],DefaultSerialDataAddress + ((i+2) * 3),2);
		EEPROM_ReadData((uint8*)&CurrentHumi[2],DefaultSerialDataAddress + ((i+2) * 3) + 2,1);
		EEPROM_ReadData((uint8*)&CurrentData[3],DefaultSerialDataAddress + ((i+3) * 3),2);
		EEPROM_ReadData((uint8*)&CurrentHumi[3],DefaultSerialDataAddress + ((i+3) * 3) + 2,1);
		EEPROM_ReadData((uint8*)&CurrentData[4],DefaultSerialDataAddress + ((i+4) * 3),2);
		EEPROM_ReadData((uint8*)&CurrentHumi[4],DefaultSerialDataAddress + ((i+4) * 3) + 2,1);
		EEPROM_ReadData((uint8*)&CurrentData[5],DefaultSerialDataAddress + ((i+5) * 3),2);
		EEPROM_ReadData((uint8*)&CurrentHumi[5],DefaultSerialDataAddress + ((i+5) * 3) + 2,1);
		EEPROM_ReadData((uint8*)&CurrentData[6],DefaultSerialDataAddress + ((i+6) * 3),2);
		EEPROM_ReadData((uint8*)&CurrentHumi[6],DefaultSerialDataAddress + ((i+6) * 3) + 2,1);
		EEPROM_ReadData((uint8*)&CurrentData[7],DefaultSerialDataAddress + ((i+7) * 3),2);
		EEPROM_ReadData((uint8*)&CurrentHumi[7],DefaultSerialDataAddress + ((i+7) * 3) + 2,1);
		EEPROM_ReadData((uint8*)&CurrentData[8],DefaultSerialDataAddress + ((i+8) * 3),2);
		EEPROM_ReadData((uint8*)&CurrentHumi[8],DefaultSerialDataAddress + ((i+8) * 3) + 2,1);
		EEPROM_ReadData((uint8*)&CurrentData[9],DefaultSerialDataAddress + ((i+9) * 3),2);
		EEPROM_ReadData((uint8*)&CurrentHumi[9],DefaultSerialDataAddress + ((i+9) * 3) + 2,1);
	}
#endif

 	memset(Data,0x3f,TempDataLength);

#ifdef ConstTestMode
	DataLength = sizeof(TestTempValue)/2;
	if(DataLength > TempDataLength - 20)
		{
		DataLength = TempDataLength - 20;
		}
	ret=DataLength;
	CurrentData = TestTempValue[0];
#else
	if(DataLength > TempDataLength - 20)
		{
		StartAddress = (DataLength);
		DataLength = TempDataLength - 20;
		}
	else
	{
		StartAddress = (DataLength);
		DataLength = StartAddress;
	}
	pMain->Page.LastDataIndex = StartAddress;
	ret=0;

	//EEPROM_ReadData((uint8*)&CurrentHumi, DefaultSerialDataAddress+ + StartAddress +2 ,1);
	CurrentData = pMain->Sensor.PCBTemp;
	CurrentHumi = pMain->Sensor.PCBHumi;

	CurrentData = CurrentData;
	*pData = CurrentData>>8 & 0xff;
	*pData |=0x80;
	pData++;
	*pData = CurrentData & 0xff;
	pData++;
	*pData = CurrentHumi;
	pData++;
#endif
uint8 FullBlockFlag=0;

	for(int i=1;i<DataLength+1;i++)
	{
		oldData = CurrentData;
		oldHumi = CurrentHumi;

		EEPROM_ReadData((uint8*)&TempData,(((StartAddress - i)%MaxRecord) * sizeof(TempData)),sizeof(TempData));
		#ifdef BELL_ENC
		CurrentData = TempData.Sensor.PCBTemp;
		CurrentHumi = TempData.Sensor.PCBHumi;
		#else
		CurrentData = TempData.Sensor.ProbeTemp;
		CurrentHumi = TempData.Sensor.PCBTemp;
		#endif
		NRF_LOG_INFO("[%5d] Temp : %d,Humi : %d, Origin Addr : %d",i,CurrentData,CurrentHumi,StartAddress);

		CurrentData ;
		DeltaData = (CurrentData/10 - oldData/10) ;
		DeltaHumi = CurrentHumi - oldHumi ;

    if(!FullBlockFlag)
      {
      if(DeltaData >= -63 && DeltaData <= 63)
        {
					FullBlockFlag=1;
          *pData = DeltaData + 63;
					pData++;
        }
			else JumpEnable|=1;
				
			if(DeltaHumi >= -7 && DeltaHumi <= 7)
        {
					FullBlockFlag=1;
          *(pData) = ((DeltaHumi + 7) & 0x0f) <<4;
        }
			else 
					{
					pData--;
					JumpEnable|=1;
					}
      }
    else
      {
			if(DeltaHumi >= -7 && DeltaHumi <= 7)
        {
          *pData |= (DeltaHumi + 7) & 0x0f;
					FullBlockFlag=0;
					pData++;
					if(DeltaData > -63 && DeltaData < 63)
						{
							*(pData) = DeltaData + 63;
							pData++;
							FullBlockFlag=0;
						}
					else 
							{
							pData--;
							JumpEnable|=1;
							}
        }
			else 
				{
				JumpEnable|=1;
				}
				


				
      }


		if(JumpEnable)					// temperature data jump
		{
			if(!FullBlockFlag)
				{
				*pData |= 0x0f;			// block close
				pData++;
				}
			
			*pData = CurrentData >>8 & 0xff;
				*pData |= 0x80;
			pData++;
			*pData = CurrentData & 0xff;
			pData++;
			*pData = CurrentHumi;
			pData++;
			JumpEnable=0;
			FullBlockFlag=0;
		}
		ret++;
		//pMain->CurrentTemp = CurrentData;
		//pData++;
		if( (pData - pStartPoint) > TempDataLength)i = DataLength + 1;
	}
	//ret = pData - pStartPoint;
	if(FullBlockFlag)*pData |= 0x0f;			// 마지막 데이터가 홀수 자리면 블록 종료 처리
	if(ret > TempDataLength) ret = TempDataLength;

	pMain->Page.LastDataIndex;
  pMain->Page.FirstIndex = pMain->Page.LastDataIndex - ret;

	ret = pData - pStartPoint;

	return ret;
}
	
	
uint16 EEPROM_GetSerialDataRef(stMain *pMain,sint8 *Data,EEPROM_RawDatastructure *RefData)
{
	uint16 ret;
	uint16 StartAddress = 0, GetDataLength = 0;
	sint16 CurrentData = 0, oldData = 0, DeltaData = 0;
	sint8 CurrentHumi = 0, oldHumi = 0, DeltaHumi = 0;
	
	sint8 *pData = Data;
	uint8 JumpEnable = 0;
	
	StartAddress = pMain->Page.LastDataIndex;
	GetDataLength = RefData->DataLength;

	memset(Data,0x00,TempDataLength);
	ret=0;

	//EEPROM_ReadData((uint8*)&CurrentHumi, DefaultSerialDataAddress+ + StartAddress +2 ,1);
	CurrentData = RefData->Sensor[0].PCBTemp - RefData->Sensor[0].PCBTemp % 10;
	CurrentHumi = RefData->Sensor[0].PCBHumi;

	CurrentData = CurrentData;
	*pData = CurrentData>>8 & 0xff;
	*pData |=0x80;
	pData++;
	*pData = CurrentData & 0xff;
	pData++;
	*pData = CurrentHumi;
	pData++;

uint8 FullBlockFlag=0;

	for(int i=1;i<GetDataLength+1;i++)
	{
		oldData = CurrentData;
		oldHumi = CurrentHumi;

		CurrentData = RefData->Sensor[i].PCBTemp - RefData->Sensor[i].PCBTemp % 10;
		CurrentHumi = RefData->Sensor[i].PCBHumi;
		//NRF_LOG_INFO("[%5d] Temp : %d,Humi : %d, Origin Addr : %d",i,CurrentData,CurrentHumi,StartAddress);

		CurrentData ;
		DeltaData = (CurrentData/10 - oldData/10) ;
		DeltaHumi = CurrentHumi - oldHumi ;

    if(!FullBlockFlag)
      {
			if((DeltaData <= -63 || DeltaData >= 63) || (DeltaHumi <= -7 || DeltaHumi >= 7))
				{
				JumpEnable|=1;
				}
			else
				{
				if(DeltaData >= -63 && DeltaData <= 63)
					{
						FullBlockFlag=1;
						*pData = DeltaData + 63;
						pData++;
					}
				else JumpEnable|=1;
				
			if(DeltaHumi >= -7 && DeltaHumi <= 7)
        {
					FullBlockFlag=1;
          *(pData) = ((DeltaHumi + 7) & 0x0f) <<4;
        }
			else 
					{
					FullBlockFlag=1;
					pData--;
					JumpEnable|=1;
					}

				}
      }
    else
      {
			if((DeltaData <= -63 || DeltaData >= 63) || (DeltaHumi <= -7 || DeltaHumi >= 7))
				{
				JumpEnable|=1;
				//FullBlockFlag=0;
				}
			else
				{
				if(DeltaHumi >= -7 && DeltaHumi <= 7)
					{
						*pData |= (DeltaHumi + 7) & 0x0f;
						FullBlockFlag=0;
						pData++;
						if(DeltaData > -63 && DeltaData < 63)
							{
								*(pData) = DeltaData + 63;
								pData++;
								FullBlockFlag=0;
							}
						else 
								{
								pData--;
								JumpEnable|=1;
								}
					}
				else 
					{
					JumpEnable|=1;
					}
				}
      }


		if(JumpEnable)					// temperature data jump
		{
			if(FullBlockFlag)
				{
				*pData |= 0x0f;			// block close
				pData++;
				}
			
			*pData = CurrentData >>8 & 0xff;
				*pData |= 0x80;
			pData++;
			*pData = CurrentData & 0xff;
			pData++;
			*pData = CurrentHumi;
			pData++;
			JumpEnable=0;
			FullBlockFlag=0;
		}
		ret++;
		if(pData > Data + TempDataLength)
			{
			break;
			}
	}
	//ret = pData - pStartPoint;
	if(FullBlockFlag)*pData |= 0x0f;			// 마지막 데이터가 홀수 자리면 블록 종료 처리
	pData++;
	*pData = 0xAA;
	pData++;
	*pData = 0x55;
	if(ret > TempDataLength) ret = TempDataLength;

	pMain->Page.LastDataIndex;
  pMain->Page.FirstIndex = pMain->Page.LastDataIndex - ret;

	ret = pData - Data;

	return ret;
}	
#endif
uint16 DataLenghBackup=0;
  
void EEPROM_WriteSerialData(stMain *pMain)
{
uint16 DataLength=0;
EEPROM_Datastructure stWriteData,stReadData;
memset(&stWriteData,0x00,sizeof(stWriteData));

EEPROM_GetDataLength(&DataLength);

NRF_LOG_INFO("[03-%d %d:%d:%d] Write serial data : %d, Temp : %d",pMain->Device.Time.Calendar.tm_mday,pMain->Device.Time.Calendar.tm_hour,pMain->Device.Time.Calendar.tm_min,pMain->Device.Time.Calendar.tm_sec,DataLength,pMain->Sensor.PCBTemp);

//DataLength %= 30000;

if(!DataLenghBackup) DataLenghBackup = DataLength;

DataLength++;

EEPROM_WriteDataLength(DataLength);


DataLength--;

stWriteData.Index = DataLength;
stWriteData.TimeStamp_UTC = mktime(&pMain->Device.Time.Calendar);
#ifndef BELL_ENC
stWriteData.Sensor.ProbeTemp = pMain->Sensor.Probe.ProbeTemp;
stWriteData.Sensor.PCBTemp = pMain->Sensor.PCBTemp/100;
//stWriteData.Sensor.Accelo = pMain->Sensor.Accelo;
#else
stWriteData.Sensor.PCBTemp = pMain->Sensor.PCBTemp;
stWriteData.Sensor.PCBHumi = pMain->Sensor.PCBHumi;
#endif

for(int i=0;i<10;i++)
	{
	EEPROM_WriteData((uint8*)&stWriteData,(DataLength % MaxEEPROMAddress) * sizeof(EEPROM_Datastructure),sizeof(EEPROM_Datastructure));
	EEPROM_ReadData((uint8*)&stReadData,(DataLength % MaxEEPROMAddress) * sizeof(EEPROM_Datastructure),sizeof(EEPROM_Datastructure));
	if(!memcmp(&stWriteData,&stReadData,sizeof(EEPROM_Datastructure)))break;
	}


//DataLength%=MaxRecord;
}

#ifdef ForceDataEnable

extern EEPROM_Datastructure ForcedData[];
extern EEPROM_Datastructure ForcedData2[];
extern EEPROM_Datastructure ForcedData3[];

void EEPROM_WriteForcelData(uint16 SetIndex)
{
uint8 LEDActiveFlag = 0;
uint16 DataLength=0;
EEPROM_Datastructure stWriteData,stReadData;
memset(&stWriteData,0x00,sizeof(stWriteData));



//DataLength %= 30000;

TriColorLED_SetOff();

if(!DataLenghBackup) DataLenghBackup = DataLength;

for(int i = 0 ; i < ForceIndexValue ; i ++)
	{
	wdt_feed();
	if(!(i%5))
		{
		TriColorLED_ActiveGreen(LEDActiveFlag ^= 1);
		}
	
	stWriteData.Index = i;
	stWriteData.TimeStamp_UTC = ForcedData[i%MaxEEPROMAddress].TimeStamp_UTC;
	#ifdef BELL_ENC
	stWriteData.Sensor.PCBTemp = ForcedData[i%MaxEEPROMAddress].Sensor.PCBTemp;
	stWriteData.Sensor.PCBHumi = ForcedData[i%MaxEEPROMAddress].Sensor.PCBHumi;
	#else
	stWriteData.Sensor.ProbeTemp = ForcedData[i%MaxEEPROMAddress].Sensor.ProbeTemp;
	stWriteData.Sensor.PCBTemp = ForcedData[i%MaxEEPROMAddress].Sensor.PCBTemp;
	#endif


	for(int j=0;j<10;j++)
		{
		EEPROM_WriteData((uint8*)&stWriteData,(i % MaxEEPROMAddress) * sizeof(EEPROM_Datastructure),sizeof(EEPROM_Datastructure));
		EEPROM_ReadData((uint8*)&stReadData,(i % MaxEEPROMAddress) * sizeof(EEPROM_Datastructure),sizeof(EEPROM_Datastructure));
		if(!memcmp(&stWriteData,&stReadData,sizeof(EEPROM_Datastructure)))break;
		}
	}

EEPROM_WriteDataLength(SetIndex);

//DataLength%=MaxRecord;
}


void EEPROM_WriteForcelData2(uint16 SetIndex)
{
uint16 DataLength=0;
EEPROM_Datastructure stWriteData,stReadData;
memset(&stWriteData,0x00,sizeof(stWriteData));

EEPROM_WriteDataLength(SetIndex);


//DataLength %= 30000;

if(!DataLenghBackup) DataLenghBackup = DataLength;

for(int i = 0 ; i < 7200 ; i ++)
	{
	wdt_feed();
	
	stWriteData.Index = ForcedData2[i].Index;
	stWriteData.TimeStamp_UTC = ForcedData2[i].TimeStamp_UTC;
	#ifdef BELL_ENC
	stWriteData.Sensor.PCBTemp = ForcedData2[i].Sensor.PCBTemp;
	stWriteData.Sensor.PCBHumi = ForcedData2[i].Sensor.PCBHumi;
	#else
	stWriteData.Sensor.ProbeTemp = ForcedData2[i].Sensor.ProbeTemp;
	stWriteData.Sensor.PCBTemp = ForcedData2[i].Sensor.PCBTemp;
	#endif


	for(int j=0;j<10;j++)
		{
		EEPROM_WriteData((uint8*)&stWriteData,(i % MaxEEPROMAddress) * sizeof(EEPROM_Datastructure),sizeof(EEPROM_Datastructure));
		EEPROM_ReadData((uint8*)&stReadData,(i % MaxEEPROMAddress) * sizeof(EEPROM_Datastructure),sizeof(EEPROM_Datastructure));
		if(!memcmp(&stWriteData,&stReadData,sizeof(EEPROM_Datastructure)))break;
		}
	}


//DataLength%=MaxRecord;
}


void EEPROM_WriteForcelData3(uint16 SetIndex)
{
uint16 DataLength=0;
EEPROM_Datastructure stWriteData,stReadData;
memset(&stWriteData,0x00,sizeof(stWriteData));

EEPROM_WriteDataLength(SetIndex);


//DataLength %= 30000;

if(!DataLenghBackup) DataLenghBackup = DataLength;

for(int i = 0 ; i < 7200 ; i ++)
	{
	wdt_feed();
	
	stWriteData.Index = ForcedData3[i].Index;
	stWriteData.TimeStamp_UTC = ForcedData3[i].TimeStamp_UTC;
	#ifdef BELL_ENC
	stWriteData.Sensor.PCBTemp = ForcedData3[i].Sensor.PCBTemp;
	stWriteData.Sensor.PCBHumi = ForcedData3[i].Sensor.PCBHumi;
	#else
	stWriteData.Sensor.ProbeTemp = ForcedData3[i].Sensor.ProbeTemp;
	stWriteData.Sensor.PCBTemp = ForcedData3[i].Sensor.PCBTemp;
	#endif


	for(int j=0;j<10;j++)
		{
		EEPROM_WriteData((uint8*)&stWriteData,(i % MaxEEPROMAddress) * sizeof(EEPROM_Datastructure),sizeof(EEPROM_Datastructure));
		EEPROM_ReadData((uint8*)&stReadData,(i % MaxEEPROMAddress) * sizeof(EEPROM_Datastructure),sizeof(EEPROM_Datastructure));
		if(!memcmp(&stWriteData,&stReadData,sizeof(EEPROM_Datastructure)))break;
		}
	}


//DataLength%=MaxRecord;
}

#endif


void EEPROM_GetRawData(stMain *pMain,EEPROM_RawDatastructure *Data)
{
	uint16 StartAddress = 0;
	uint8 FirstPageFlag = 0;
	uint16 ZeroAddress = 0;
	uint16 ChipAddress = 0;
	
	if(pMain->Page.CurrentPage <= 1) 
		{
		#ifndef BELL_ENC
				Data->Sensor[0].PCBTemp = pMain->Sensor.Probe.ProbeTemp;
				Data->Sensor[0].PCBHumi = pMain->Sensor.PCBTemp / 100;
		#else
				Data->Sensor[0].PCBTemp = pMain->Sensor.PCBTemp;
				Data->Sensor[0].PCBHumi = pMain->Sensor.PCBHumi;
		#endif
		
		FirstPageFlag = 1;
		}

	StartAddress = pMain->Page.OriginPoint;
		
	EEPROM_Datastructure TempData[MaxPageData + 1];
	memset(&TempData,0x00,sizeof(TempData));
	
	
	if((StartAddress > MaxEEPROMAddress) && ((StartAddress % MaxEEPROMAddress) < MaxPageData) && FirstPageFlag)		// 데이터 갯수가 7200 ~ 8200개 사이이며 첫페이지 일때
		{
			EEPROM_ReadData((uint8*)&TempData[MaxPageData - ((StartAddress + FirstPageFlag) % MaxEEPROMAddress)], 0 ,sizeof(EEPROM_Datastructure) * ((StartAddress) % MaxEEPROMAddress));
			EEPROM_ReadData((uint8*)&TempData[0],(((7200 -(MaxPageData - ((StartAddress + FirstPageFlag) % MaxEEPROMAddress)))%MaxRecord) * sizeof(EEPROM_Datastructure)),sizeof(EEPROM_Datastructure) * (MaxPageData - (StartAddress + FirstPageFlag)% MaxEEPROMAddress));
			for(int i = 0 ; i + FirstPageFlag < MaxPageData; i++)
				{
				if(StartAddress < i)break;
				
				#ifdef BELL_ENC
				Data->Sensor[MaxPageData - i - FirstPageFlag].PCBTemp = TempData[i].Sensor.PCBTemp;
				Data->Sensor[MaxPageData - i - FirstPageFlag].PCBHumi = TempData[i].Sensor.PCBHumi;
				#else
				Data->Sensor[MaxPageData - 1 - i].PCBTemp = TempData[i].Sensor.ProbeTemp;
				Data->Sensor[MaxPageData - 1 - i].PCBHumi = TempData[i].Sensor.PCBTemp;
				#endif
				}
			Data->DataLength = MaxPageData;
		}
	else if((StartAddress > MaxEEPROMAddress) && ((StartAddress % MaxEEPROMAddress) < MaxPageData))		// 데이터 갯수가 7200 ~ 8200개 사이
		{
			EEPROM_ReadData((uint8*)&TempData[MaxPageData - ((StartAddress + 1) % MaxEEPROMAddress)], 0 ,sizeof(EEPROM_Datastructure) * ((StartAddress + 1) % MaxEEPROMAddress));
			EEPROM_ReadData((uint8*)&TempData[0],(((7200 -(MaxPageData - ((StartAddress) % MaxEEPROMAddress)))%MaxRecord) * sizeof(EEPROM_Datastructure)),sizeof(EEPROM_Datastructure) * (MaxPageData - (StartAddress + 1)% MaxEEPROMAddress));
			for(int i = 0 ; i < MaxPageData; i++)
				{
				if(StartAddress < i)break;
				
				#ifdef BELL_ENC
				Data->Sensor[MaxPageData - 1 - i].PCBTemp = TempData[i].Sensor.PCBTemp;
				Data->Sensor[MaxPageData - 1 - i].PCBHumi = TempData[i].Sensor.PCBHumi;
				#else
				Data->Sensor[MaxPageData - 1 - i].PCBTemp = TempData[i].Sensor.ProbeTemp;
				Data->Sensor[MaxPageData - 1 - i].PCBHumi = TempData[i].Sensor.PCBTemp;
				#endif
				}
			Data->DataLength = MaxPageData;
		}
	else if(StartAddress >= MaxPageData)				// 데이터가 페이지 최대 이상 ~ 메모리 최대 갯수 미만일 경우. 조건 2
		{
			EEPROM_ReadData((uint8*)&TempData[FirstPageFlag],(((StartAddress - (MaxPageData - FirstPageFlag))%MaxRecord) * sizeof(EEPROM_Datastructure)),sizeof(EEPROM_Datastructure) * (MaxPageData - FirstPageFlag + 1));
			for(int i=FirstPageFlag ; i<MaxPageData-FirstPageFlag + 1 ; i++)
				{
				if(StartAddress < i)break;
				
				#ifdef BELL_ENC
				Data->Sensor[MaxPageData - i].PCBTemp = TempData[i].Sensor.PCBTemp;
				Data->Sensor[MaxPageData - i].PCBHumi = TempData[i].Sensor.PCBHumi;
				#else
				Data->Sensor[MaxPageData - i].PCBTemp = TempData[i].Sensor.ProbeTemp;
				Data->Sensor[MaxPageData - i].PCBHumi = TempData[i].Sensor.PCBTemp;
				#endif
				}
			Data->DataLength = MaxPageData;
		}
	else if ((StartAddress < MaxPageData) && FirstPageFlag)				// 데이터 갯수가 1000개 미만이고 첫페이지 일때
			{
			EEPROM_ReadData((uint8*)&TempData[FirstPageFlag],(((0)%MaxRecord) * sizeof(EEPROM_Datastructure)),sizeof(EEPROM_Datastructure) * (StartAddress - FirstPageFlag + 1));
			NRF_LOG_INFO("[%10d] EEP Start : 0 , EEP End : %d",getTickCount() , StartAddress - FirstPageFlag);
			for(int i = FirstPageFlag ; i<StartAddress + FirstPageFlag ; i++)
				{
				if(StartAddress < i)break;
				#ifdef BELL_ENC
				Data->Sensor[StartAddress  - i + FirstPageFlag].PCBTemp = TempData[i].Sensor.PCBTemp;
				Data->Sensor[StartAddress  - i + FirstPageFlag].PCBHumi = TempData[i].Sensor.PCBHumi;
				#else
				Data->Sensor[StartAddress -  i].PCBTemp = TempData[i].Sensor.ProbeTemp;
				Data->Sensor[StartAddress -  i].PCBHumi = TempData[i].Sensor.PCBTemp;
				#endif
				}
			Data->DataLength = StartAddress;
			}
	else if (StartAddress < MaxPageData)				// 데이터 갯수가 1000개 미만일때
			{
			EEPROM_ReadData((uint8*)&TempData[0],(((0)%MaxRecord) * sizeof(EEPROM_Datastructure)),sizeof(EEPROM_Datastructure) * (StartAddress + 1));
			NRF_LOG_INFO("[%10d] EEP Start : 0 , EEP End : %d",getTickCount() , StartAddress - FirstPageFlag);
			for(int i = FirstPageFlag ; i <= StartAddress ; i++)
				{
				if(StartAddress < i)break;
				#ifdef BELL_ENC
				Data->Sensor[StartAddress  - i].PCBTemp = TempData[i].Sensor.PCBTemp;
				Data->Sensor[StartAddress  - i].PCBHumi = TempData[i].Sensor.PCBHumi;
				#else
				Data->Sensor[StartAddress -  i].PCBTemp = TempData[i].Sensor.ProbeTemp;
				Data->Sensor[StartAddress -  i].PCBHumi = TempData[i].Sensor.PCBTemp;
				#endif
				}
			Data->DataLength = StartAddress;
			}
	else																					// 데이터가 메모리 최대 갯수 이상이면서 페이지 최대 갯수 미만 인덱스. 조건 3
		{
			EEPROM_ReadData((uint8*)&TempData[FirstPageFlag],(((StartAddress - FirstPageFlag)%MaxRecord) * sizeof(EEPROM_Datastructure)),sizeof(EEPROM_Datastructure) * (StartAddress - FirstPageFlag));
			for(int i = 0 ; i<StartAddress-FirstPageFlag ; i++)
				{
				if(StartAddress < i)break;
				#ifdef BELL_ENC
				Data->Sensor[StartAddress - 1 - i].PCBTemp = TempData[i].Sensor.PCBTemp;
				Data->Sensor[StartAddress - 1 - i].PCBHumi = TempData[i].Sensor.PCBHumi;
				#else
				Data->Sensor[StartAddress - 1 - i].PCBTemp = TempData[i].Sensor.ProbeTemp;
				Data->Sensor[StartAddress - 1 - i].PCBHumi = TempData[i].Sensor.PCBTemp;
				#endif
				}
			Data->DataLength = StartAddress;
		}
/*
	if(pMain->Sensor.DataNumber > 7199)			// 데이터 기록 한바퀴 이상 발생
		{
		ZeroAddress = pMain->Sensor.DataNumber - 7199;		// 가장 오래된 데이터 주소 계산
		if(StartAddress < ZeroAddress + 1000)							// 지금 가져오는 데이터가 마지막 구간
			{
			if(ZeroAddress % 7200 < 1000)
				{
				ChipAddress = ZeroAddress;				// 
				}
			else
				{
				ChipAddress = StartAddress - ZeroAddress;				// 
				}
			
			for(int i=ChipAddress ; i ; i--)
				{
				Data->Sensor[ChipAddress + i].PCBTemp;
				Data->Sensor[ChipAddress + i].PCBHumi;
				}
				
			}
		}
*/
}

void EEPROM_GetRawDataForBle(stMain *pMain)
{
	uint16 GetIndex = 0;
	EEPROM_Datastructure BleTempBlock = {0};
	
	EEPROM_GetDataLength(&GetIndex);
	GetIndex -= 1;
	pMain->BLE.AdvPacket.Index = GetIndex;
	for(int i=0 ; i<3 ; i++)
		{
		EEPROM_ReadData((uint8*)&BleTempBlock, sizeof(EEPROM_Datastructure) * ((GetIndex) % MaxEEPROMAddress),sizeof(EEPROM_Datastructure));
		pMain->BLE.AdvPacket.Data[i].TimeStamp = BleTempBlock.TimeStamp_UTC;
		pMain->BLE.AdvPacket.Data[i].Temp = BleTempBlock.Sensor.ProbeTemp;
		pMain->BLE.AdvPacket.Data[i].Humi = BleTempBlock.Sensor.PCBTemp;
		NRF_LOG_INFO("[%10d] index : %d timestamp : %10d Probe : %4d PCB : %3d",getTickCount(),GetIndex,pMain->BLE.AdvPacket.Data[i].TimeStamp,pMain->BLE.AdvPacket.Data[i].Temp,pMain->BLE.AdvPacket.Data[i].Humi);
		if(!GetIndex) return;
		GetIndex--;
		}
}

void EEPROM_WriteOffset(sint16 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffd8,2);
}

void EEPROM_GetOffset(sint16 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffd8,2);
}

void EEPROM_WriteCalibDataNumber(uint8 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffC1,1);
}


void EEPROM_GetCalibDataNumber(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffC1,1);
}

void EEPROM_WriteTempDataForCalib(sint16 Data)
{
	uint8 Number=0;
	EEPROM_GetCalibDataNumber(&Number);
	EEPROM_WriteData((uint8*)&Data,0xffC2 + Number*2,2);
	Number++;
	if(Number>9)Number=0;
	EEPROM_WriteCalibDataNumber(Number);
}

void EEPROM_GetTempDataForCalib(sint16 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffC2,20);
}

void EEPROM_WriteProbeCorrectCount(uint8 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffC1,1);
}

void EEPROM_GetProbeCorrectCount(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffC1,1);
}

void EEPROM_WriteProbeCorrectFailCount(uint8 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffC0,1);
}

void EEPROM_GetProbeCorrectFailCount(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffC0,1);
}

void EEPROM_WriteCalibFailCount(uint8 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffBF,1);
}

void EEPROM_GetCalibFailCount(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffBF,1);
}

void EEPROM_WriteSubCount(uint8 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffBE,1);
}

void EEPROM_GetSubCount(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffBE,1);
}

void EEPROM_WriteSubStatus(uint8 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffBD,1);
}

void EEPROM_GetSubStatus(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffBD,1);
}

void EEPROM_WriteEPDRefresh_Temperature(uint8 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffBC,1);
}

void EEPROM_GetEPDRefresh_Temperature(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffBC,1);
}


void EEPROM_GetEPDRefresh_QR(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffBB,1);
}

void EEPROM_ClearEPDRefresh_QR(void)
{
	uint8 RefreshCount=0;
	EEPROM_WriteData((uint8*)&RefreshCount,0xffBB,1);
}

void EEPROM_AddEPDRefresh_QR(void)
{
	uint8 RefreshCount=0;
	EEPROM_GetEPDRefresh_QR(&RefreshCount);
	RefreshCount++;
	EEPROM_WriteData((uint8*)&RefreshCount,0xffBB,1);
}



void EEPROM_GetTempSensorErrorCount(uint16 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffB9,2);
}

void EEPROM_WriteTempSensorErrorCount(uint16 *Data)
{
	EEPROM_WriteData((uint8*)Data,0xffB9,2);
}

void EEPROM_GetTempSensorI2cFailCount(uint16 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffB7,2);
}

void EEPROM_WriteTempSensorI2cFailCount(uint16 *Data)
{
	EEPROM_WriteData((uint8*)Data,0xffB7,2);
}

void EEPROM_GetLastDataNumber(uint16 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffB5,2);
}

void EEPROM_WriteLastDataNumber(uint16 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffB5,2);
}


void EEPROM_GetOffsetEnable(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffAB,1);
	if(*Data == 0xff) *Data = CalibView_OffsetApply;			// calibview 기본값
}

void EEPROM_WriteOffsetEnable(uint8 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffAB,1);
}

void EEPROM_GetRecordModeFlag(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xffAA,1);
	if(*Data == 0xff) *Data = 0;			// calibview 기본값
}

void EEPROM_WriteRecordModeFlag(uint8 Data)
{
	EEPROM_WriteData((uint8*)&Data,0xffAA,1);
}


void EEPROM_GetDisplayInterval(uint32 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xFFA6,4);
	if(*Data == 0xffffffff)*Data = 0;
	//*Data=600;
}

void EEPROM_WriteDisplayInterval(uint32 *Data)
{
	EEPROM_WriteData((uint8*)Data,0xFFA6,4);
}

void EEPROM_WriteProbeOffsetData(CalibrationData stData)
{
	EEPROM_Datastructure CheckData;
	memset(&CheckData,0x00,sizeof(CheckData));
	
	for(int i=0;i<10;i++)
		{
		EEPROM_WriteData((uint8*)&stData,0xff9D,sizeof(EEPROM_Datastructure));
		EEPROM_GetOffsetData((CalibrationData*)&CheckData);
		if(!memcmp(&CheckData,&stData,sizeof(EEPROM_Datastructure)))
			{
			break;
			}
		}
	
}

void EEPROM_GetProbeOffsetData(CalibrationData *stData)
{
	EEPROM_ReadData((uint8*)stData,0xff9D,sizeof(CalibrationData));
	if(stData->Index == 0xffff)
		{
		memset(stData,0x00,sizeof(CalibrationData));
		}
}

void EEPROM_WriteBLEMode(uint8 Data)
{
	EEPROM_WriteData(&Data,0xFF9C,1);
}

void EEPROM_GetBLEMode(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xFF9C,1);
	//*Data=600;
}


void EEPROM_WriteSerialNumber(uint8 *Data)
{
	EEPROM_WriteData(Data,0xff91,8);
}

void EEPROM_GetSerialNumber(uint8 *Data)
{
	EEPROM_ReadData(Data,0xff91,8);
}



void EEPROM_WriteOffsetData(CalibrationData stData)
{
	EEPROM_Datastructure CheckData;
	memset(&CheckData,0x00,sizeof(CheckData));
	
	for(int i=0;i<10;i++)
		{
		EEPROM_WriteData((uint8*)&stData,0xff85,sizeof(EEPROM_Datastructure));
		EEPROM_GetOffsetData((CalibrationData*)&CheckData);
		if(!memcmp(&CheckData,&stData,sizeof(EEPROM_Datastructure)))
			{
			break;
			}
		}
	
}

void EEPROM_GetOffsetData(CalibrationData *stData)
{
	EEPROM_ReadData((uint8*)stData,0xff85,sizeof(EEPROM_Datastructure));
	if(stData->Index == 0xffff)
		{
		memset(stData,0x00,sizeof(EEPROM_Datastructure));
		}
}


void EEPROM_WriteBLEPower(uint8 Data)
{
	EEPROM_WriteData(&Data,0xFF84,1);
}

void EEPROM_GetBLEPower(uint8 *Data)
{
	EEPROM_ReadData((uint8*)Data,0xFF84,1);
	//*Data=600;
}

uint8 TWI_EEPROMTestCode(void)
{
uint8 writetmp=0,readtmp=0;
uint16 Data=0,oldData=0;;
uint16 readaddr;

for(int i=0;i<5;i++)
  {
  writetmp = i;
  EEPROM_WriteData(&writetmp,9 * 7200 + 5 - i,1);
  HAL_Delay(10);
  EEPROM_ReadData(&readtmp,9 * 7200 + 5 - i,1);
  if(writetmp != readtmp)
    {
    return 0xff;
    }
  }

  return 1;
}

			/*
		for(int i = 0;i<7300;i++)
			{
			Main.Sensor.PCBTemp = (i * 10) % 4000;
			Main.Sensor.PCBHumi = i % 100;
			EEPROM_WriteSerialData(&Main);
		wdt_feed();
			}
			*/
			
void EEPROM_EraseAll(void)
{
	uint8 dummy[64];
	uint8 SerialNumberTemp[10];
	CalibrationData OffsetDataTemp;
	
	memset(dummy,0xff,64);
	memset(SerialNumberTemp,0xff,10);
	memset(&OffsetDataTemp,0xff,sizeof(OffsetDataTemp));
	
	
	EEPROM_GetProbeOffsetData(&OffsetDataTemp);
	EEPROM_GetSerialNumber(SerialNumberTemp);
	for(int i=0 ; i<EEPROM_MAX_SIZE_BYTE ; i+=EEPROM_PAGE_SIZE)
		{
		EEPROM_WriteData(dummy,i,EEPROM_PAGE_SIZE);
		wdt_feed();
		}
	
	EEPROM_WriteProbeOffsetData(OffsetDataTemp);
	EEPROM_WriteSerialNumber(SerialNumberTemp);
	EEPROM_WriteDataLength(0);
}

void EEPROM_DataDump(void)
{
	EEPROM_Datastructure DataTemp = {0};
		for(int i=0 ; i<MaxEEPROMAddress ; i++)
			{
			wdt_feed();
			EEPROM_ReadData((uint8*)&DataTemp, i * sizeof(DataTemp), sizeof(DataTemp));
			#ifdef BELL_ENC
			NRF_LOG_INFO("[%10d] ,%d, %d, %d, %d ",getTickCount(),DataTemp.TimeStamp_UTC,DataTemp.Index,DataTemp.Sensor.PCBTemp,DataTemp.Sensor.PCBHumi);
			#else
			NRF_LOG_INFO("[%10d] ,%d, %d, %d, %d ",getTickCount(),DataTemp.TimeStamp_UTC,DataTemp.Index,DataTemp.Sensor.ProbeTemp,DataTemp.Sensor.PCBTemp);
			#endif
			HAL_Delay(5);
			}	
}
	
	