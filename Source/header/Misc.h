
#include "types.h"

#ifndef MISC_H_
#define MISC_H_

void BitShifter(uint8 *buffer,uint16 size, uint8 bitlen);
void BitDoubler(uint8 *Buf1, uint16 Location, uint16 Len);
void BitCopy(uint8 *Buf1, uint16 Location, uint8 *Buf2, uint16 BitNumber,uint16 Len);
uint8 QRLogger_HextoAscii(uint8 hex);

#endif