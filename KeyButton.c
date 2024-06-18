
#include "KeyButton.h"
#include "EPD_Page.h"

void KeyExec_Factory(stMain *pMain,KeyButton Button)
{
	switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_Right:
			EPDPage_Factory_Calibration(pMain);
			pMain->Device.RunningStatus = RunStat_SelfCalib1_Idle;
			break;

		case KeyButton_RightLong:
			break;
		}
}

void KeyExec_FactoryIdle(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_Right:
			EPDPage_Factory_Calibration(pMain);
			pMain->Device.RunningStatus = RunStat_SelfCalib1_Idle;
			break;

		case KeyButton_RightLong:
			break;
		}
}

void KeyExec_Logging_Idle(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_Right:
			break;

		case KeyButton_RightLong:
			break;
		}
}

void KeyExec_Logging(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
		  pMain->Page.CurrentPage = 0;
      DisplayRefresh(pMain);
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_Right:
			if(pMain->Page.CurrentPage<10) {pMain->Page.CurrentPage++;}; // 페이지 변경
      DisplayRefresh(pMain);
			break;

		case KeyButton_RightLong:
			break;
		}
}

void KeyExec_SelfTest(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_Right:
			break;

		case KeyButton_RightLong:
			break;
		}
}

void KeyExec_SelfTest_Idle(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_Right:
			break;

		case KeyButton_RightLong:
			break;
		}
}

void KeyExec_SelfCalib(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_Right:
			break;

		case KeyButton_RightLong:
			break;
		}
}

void KeyExec_SelfCalib_Idle(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_Right:
			EPDPage_Factory_Calibration_Result(pMain);
			pMain->Device.RunningStatus = RunStat_Correction;

			break;

		case KeyButton_RightLong:
			break;
		}
}

void KeyExec_Correction(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_Right:
			EPDPage_Service_Standby(pMain);
			pMain->Device.RunningStatus = RunStat_Logging;
			break;

		case KeyButton_RightLong:
			break;
		}
}

void KeyExec_Correction_Idle(stMain *pMain,KeyButton Button)
{
		switch(Button)
		{
		case KeyButton_Left:
			break;

		case KeyButton_LeftLong:
			break;

		case KeyButton_Right:
			break;

		case KeyButton_RightLong:
			break;
		}
}
