#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_

#include "struct.h"

#define EPD_WIDTH   152
#define EPD_HEIGHT  152


typedef enum
{
  ScreenSide_Portrait_Up = 1,
  ScreenSide_Portrait_Down ,
  ScreenSide_Landscape_Up ,
  ScreenSide_Landscape_Down ,
}ScreenSide;


typedef struct
{
	uint8 type;
	uint8 timing1;
	uint8 timing2;
	uint8 timing3;
	uint8 timing4;
	uint8 cycle;
}LUTList;

typedef struct
{
	LUTList VCOM[5];
	LUTList WW[5];
	LUTList BW[5];
	LUTList WB[5];
	LUTList BB[5];
}LUTTiming;


//EPD
void EPD_W21_Init(void);
void EPD_init(void);
void EPD_WakeUp(char PartialMode);
void PIC_display (const unsigned char* picData);
void EPD_sleep(stMain *pMain);
void EPD_refresh(void);
unsigned char lcd_chkstatus(uint32 TimeOutMs);
void PIC_display_Clear(void);
//Display canvas function
void SetLUTSecond(void);
unsigned char EPD_Display(stMain *pMain,unsigned char *Image, char PartialMode,char WaitEnable);
void EPD_WhiteScreen_ALL(stMain *pMain,const unsigned char *datas);
uint8 EPD_CallUpdate(stMain *pMain);

#endif
/***********************************************************
						end file
***********************************************************/


