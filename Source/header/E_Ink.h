/*
 * E_Ink.h
 *
 *  Created on: Sep 15, 2020
 *      Author: Administrator
 */

#ifndef SRC_DRIVER_E_INK_H_
#define SRC_DRIVER_E_INK_H_

#include "types.h"
#include "fonts.h"

extern const uint8_t Font12_Table[];
extern const uint8_t Icon12_Table[];
void EINK_WriteFontVertical(uint8 *buf,uint8 x, uint8 y, uint8 Character);
void EINK_WriteFont(uint8 *buf,uint8 x, uint8 y, uint8 Character);
void EINK_WriteLargeFont(uint8 *buf,uint8 x, uint8 y, uint8 Character);
void EINK_WriteIcon(uint8 *buf,uint8 x, uint8 y, uint8 Character);
void EINK_WriteString(uint8 *buf,uint8 x, uint8 y, uint8* Character);
void EINK_DrawingImage(uint8 *buf,uint8 x, uint8 y, uint8* image,uint16 ImageWidth,uint16 ImageHeight);
void EINK_WriteFont16(uint8 *buf,uint8 x, uint8 y, uint8 Character);
void EINK_WriteString16(uint8 *buf,uint8 x, uint8 y, uint8* Character);

#endif /* SRC_DRIVER_E_INK_H_ */
