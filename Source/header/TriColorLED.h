
#ifndef TRICOLORLED_H_
#define TRICOLORLED_H_

#include "struct.h"

typedef enum
{
  LedOnOff_On = 1,
  LedOnOff_Off = 2
}LEDOnOff;

typedef enum
{
	LEDColor_Red = 0x01,
	LEDColor_Green = 0x02,
	LEDColor_Blue = 0x04,
	LEDColor_Purple = 0x05,
	LEDColor_Yellow = 0x03,
	LEDColor_Cyan = 0x06,
	LEDColor_White = 0x07,
}LEDColor;

void TriColorLED_Init(void);
void TriColorLED_ActiveRed(LEDOnOff LEDControl);
void TriColorLED_ActiveGreen(LEDOnOff LEDControl);
void TriColorLED_ActiveBlue(LEDOnOff LEDControl);
void TriColorLED_SetOff(void);
void TriColorLED_SetRed(void);
void TriColorLED_SetGreen(void);
void TriColorLED_SetBlue(void);
void TriColorLED_SetPurple(void);
void TriColorLED_SetYellow(void);
void TriColorLED_SetCyan(void);
void TriColorLED_SetWhite(void);
void TriColorLED_Blink(LEDColor BlinkColor,uint16 BlinkCount,uint16 BlinkInterval);
#endif