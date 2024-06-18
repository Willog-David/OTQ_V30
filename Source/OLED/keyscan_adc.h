/******************** (C) COPYRIGHT 2010 GIANTPLUS ********************
* File Name          : keyscan_adc.h
* Author             : YIMING
* Version            : V1.0.0
* Date               : 2011.12.15
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __keyscan_adc_H
#define __keyscan_adc_H	  
/* Includes ------------------------------------------------------------------*/

#include "struct.h"

#define ADC_LIVES 8

extern void Keyscan_init(void);
//-----?ªð 0??«ö¤U¡A1--8?---//
uint8 Keyscan(void);
extern void Delay(uint32 nCount);
void ADC_Configuration(void);

uint8 Tempscan(void);
void temp_config(void);
#endif /* __keyscan_adc_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
