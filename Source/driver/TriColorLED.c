
#include "TriColorLED.h"

void TriColorLED_Init(void)
{
  nrf_gpio_cfg_output(OTQV2_GPIO_LED_R);
  nrf_gpio_cfg_output(OTQV2_GPIO_LED_G);
  nrf_gpio_cfg_output(OTQV2_GPIO_LED_B);
  
}

void TriColorLED_ActiveRed(LEDOnOff LEDControl)
{
  if(LEDControl == LedOnOff_On)
  {
    nrf_gpio_pin_write(OTQV2_GPIO_LED_R, 0);
  }
  else
  {
    nrf_gpio_pin_write(OTQV2_GPIO_LED_R, 1);
  }
}

void TriColorLED_ActiveGreen(LEDOnOff LEDControl)
{
  if(LEDControl == LedOnOff_On)
  {
    nrf_gpio_pin_write(OTQV2_GPIO_LED_G, 0);
  }
  else
  {
    nrf_gpio_pin_write(OTQV2_GPIO_LED_G, 1);
  }
}

void TriColorLED_ActiveBlue(LEDOnOff LEDControl)
{
  if(LEDControl == LedOnOff_On)
  {
    nrf_gpio_pin_write(OTQV2_GPIO_LED_B, 0);
  }
  else
  {
    nrf_gpio_pin_write(OTQV2_GPIO_LED_B, 1);
  }
}

void TriColorLED_SetOff(void)
{
  TriColorLED_ActiveRed(LedOnOff_Off);
  TriColorLED_ActiveGreen(LedOnOff_Off);
  TriColorLED_ActiveBlue(LedOnOff_Off);
}

void TriColorLED_SetRed(void)
{
  TriColorLED_ActiveRed(LedOnOff_On);
  TriColorLED_ActiveGreen(LedOnOff_Off);
  TriColorLED_ActiveBlue(LedOnOff_Off);
}

void TriColorLED_SetGreen(void)
{
  TriColorLED_ActiveRed(LedOnOff_Off);
  TriColorLED_ActiveGreen(LedOnOff_On);
  TriColorLED_ActiveBlue(LedOnOff_Off);
}

void TriColorLED_SetBlue(void)
{
  TriColorLED_ActiveRed(LedOnOff_Off);
  TriColorLED_ActiveGreen(LedOnOff_Off);
  TriColorLED_ActiveBlue(LedOnOff_On);
}

void TriColorLED_SetPurple(void)
{
  TriColorLED_ActiveRed(LedOnOff_On);
  TriColorLED_ActiveGreen(LedOnOff_Off);
  TriColorLED_ActiveBlue(LedOnOff_On);
}

void TriColorLED_SetYellow(void)
{
  TriColorLED_ActiveRed(LedOnOff_On);
  TriColorLED_ActiveGreen(LedOnOff_On);
  TriColorLED_ActiveBlue(LedOnOff_Off);
}

void TriColorLED_SetCyan(void)
{
  TriColorLED_ActiveRed(LedOnOff_Off);
  TriColorLED_ActiveGreen(LedOnOff_On);
  TriColorLED_ActiveBlue(LedOnOff_On);
}

void TriColorLED_SetWhite(void)
{
  TriColorLED_ActiveRed(LedOnOff_On);
  TriColorLED_ActiveGreen(LedOnOff_On);
  TriColorLED_ActiveBlue(LedOnOff_On);
}

void TriColorLED_Blink(LEDColor BlinkColor,uint16 BlinkCount,uint16 BlinkInterval)
{
TriColorLED_SetOff();
for(int i=0 ; i<BlinkCount ; i++)
	{
	if(BlinkColor & LEDColor_Red)
		{
		TriColorLED_ActiveRed(LedOnOff_On);
		}

	if(BlinkColor & LEDColor_Green)
		{
		TriColorLED_ActiveGreen(LedOnOff_On);
		}

	if(BlinkColor & LEDColor_Blue)
		{
		TriColorLED_ActiveBlue(LedOnOff_On);
		}
	HAL_Delay(BlinkInterval);
	TriColorLED_SetOff();
	HAL_Delay(BlinkInterval);
	}
}