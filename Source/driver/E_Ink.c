/*
 * E_Ink.c
 *
 *  Created on: Sep 15, 2020
 *      Author: Administrator
 */

#include "E_Ink.h"
#include "string.h"

#ifdef EPD_DISPLAY
#define EPD_WIDTH 176/8
#define EPD_HEIGHT 264
#else
#define EPD_WIDTH 128/8
#define EPD_HEIGHT 160
#endif

void EINK_WriteFontVertical(uint8 *buf,uint8 x, uint8 y, uint8 Character)
{
	uint16 Pointer;
	uint8 PointerRemainder;
	uint8 UpperFontTmp,LowerFontTmp;
	volatile uint16 TextBuf[12];

	Pointer=y*EPD_WIDTH;
	Pointer+=x/8;
	PointerRemainder=x%8;

	for(int i=0;i<8;i++)
	{
		for(int j=0;j<12;j++)
		{
			UpperFontTmp=(Font12_Table[(Character-0x20)*12]&((0x80>>i))<<i);
			UpperFontTmp|=((Font12_Table[(Character-0x20)*12+1]&(0x80>>i))<<i)>>1;
			UpperFontTmp|=((Font12_Table[(Character-0x20)*12+2]&(0x80>>i))<<i)>>2;
			UpperFontTmp|=((Font12_Table[(Character-0x20)*12+3]&(0x80>>i))<<i)>>3;
			UpperFontTmp|=((Font12_Table[(Character-0x20)*12+4]&(0x80>>i))<<i)>>4;
			UpperFontTmp|=((Font12_Table[(Character-0x20)*12+5]&(0x80>>i))<<i)>>5;
			UpperFontTmp|=((Font12_Table[(Character-0x20)*12+6]&(0x80>>i))<<i)>>6;
			UpperFontTmp|=((Font12_Table[(Character-0x20)*12+7]&(0x80>>i))<<i)>>7;

			LowerFontTmp=((Font12_Table[(Character-0x20)*12+8]&(0x80>>i))<<i);
			LowerFontTmp|=((Font12_Table[(Character-0x20)*12+9]&(0x80>>i))<<i)>>1;
			LowerFontTmp|=((Font12_Table[(Character-0x20)*12+10]&(0x80>>i))<<i)>>2;
			LowerFontTmp|=((Font12_Table[(Character-0x20)*12+11]&(0x80>>i))<<i)>>3;
		}

		TextBuf[i]=UpperFontTmp<<8|LowerFontTmp;
		if(PointerRemainder<=4)		// 2바이트 픽셀 수정
		{
			buf[Pointer+(7-i)*EPD_WIDTH]&=0xf0<<4-PointerRemainder;
			buf[Pointer+(7-i)*EPD_WIDTH]|=UpperFontTmp>>PointerRemainder;
			buf[Pointer+(7-i)*EPD_WIDTH+1]&=0x0f>>PointerRemainder;
			buf[Pointer+(7-i)*EPD_WIDTH+1]|=UpperFontTmp<<8-PointerRemainder|LowerFontTmp>>(PointerRemainder);
		}
		else 						// 3바이트 픽셀 수정
		{

		}
	}


}

void EINK_WriteIcon(uint8 *buf,uint8 x, uint8 y, uint8 Character)
{
	uint16 pointer;
	uint8 remainder=x%8;

	pointer=y*EPD_WIDTH;
	pointer+=x/8;
	if(remainder)
	{
		for(int i=0;i<11;i++)		// for qrcode
		//for(int i=0;i<12;i++)
		{
			//buf[pointer+i*EPD_WIDTH]=0x0a;
			buf[pointer+i*EPD_WIDTH]|=(Icon12_Table[(Character)*12+i])>>remainder;
			buf[pointer+i*EPD_WIDTH+1]|=(Icon12_Table[(Character)*12+i])<<8-remainder;
		}
	}
	else
	{
		for(int i=0;i<12;i++)
		{
			//buf[pointer+i*EPD_WIDTH]=0x0a;
			if(Icon12_Table[(Character)*12+i])
				{
				buf[pointer+i*EPD_WIDTH]=Icon12_Table[(Character)*12+i];
				}
		}
	}
}

void EINK_WriteFont(uint8 *buf,uint8 x, uint8 y, uint8 Character)
{
	uint16 pointer;
	uint8 remainder=x%8;

	pointer=y*EPD_WIDTH;
	pointer+=x/8;
	if(remainder)
	{
		for(int i=0;i<11;i++)		// for qrcode
		//for(int i=0;i<12;i++)
		{
			//buf[pointer+i*EPD_WIDTH]=0x0a;
			buf[pointer+i*EPD_WIDTH]|=(Font12_Table[(Character-0x20)*12+i])>>remainder;
			buf[pointer+i*EPD_WIDTH+1]|=(Font12_Table[(Character-0x20)*12+i])<<8-remainder;
		}
	}
	else
	{
		for(int i=0;i<11;i++)		// for qrcode
		//for(int i=0;i<12;i++)
		{
			//buf[pointer+i*EPD_WIDTH]=0x0a;
			buf[pointer+i*EPD_WIDTH]=Font12_Table[(Character-0x20)*12+i];
		}
	}
}

void EINK_WriteFont16(uint8 *buf,uint8 x, uint8 y, uint8 Character)
{
	uint16 pointer;
	uint8 remainder=x%8;

	pointer=y*EPD_WIDTH;
	pointer+=x/8;
	if(remainder)
	{
		for(int i=0;i<16;i++)		// for qrcode
		//for(int i=0;i<12;i++)
		{
			//buf[pointer+i*EPD_WIDTH]=0x0a;
			buf[pointer+i*EPD_WIDTH]|=(Font16_Table[(Character-0x20)*32+(i*2)])>>remainder;
			buf[pointer+i*EPD_WIDTH+1]|=(Font16_Table[(Character-0x20)*32+(i*2)])<<(8-remainder);
			buf[pointer+i*EPD_WIDTH+1]|=(Font16_Table[(Character-0x20)*32+(i*2)+1])>>remainder;
			buf[pointer+i*EPD_WIDTH+2]|=(Font16_Table[(Character-0x20)*32+(i*2)+1])<<(8-remainder);
		}
	}
	else
	{
		for(int i=0;i<16;i++)		// for qrcode
		//for(int i=0;i<12;i++)
		{
			//buf[pointer+i*EPD_WIDTH]=0x0a;
			buf[pointer+i*EPD_WIDTH]=Font16_Table[(Character-0x20)*32+(i*2)];
			buf[pointer+i*EPD_WIDTH+1]=Font16_Table[(Character-0x20)*32+(i*2)+1];
		}
	}
}

void EINK_WriteLargeFont(uint8 *buf,uint8 x, uint8 y, uint8 Character)
{
	uint16 pointer;
	uint8 remainder=x%8;

	pointer=y*EPD_WIDTH;
	pointer+=x/8;
	if(remainder)
	{
		for(int i=0;i<24;i++)		// for qrcode
		//for(int i=0;i<12;i++)
		{
			//buf[pointer+i*EPD_WIDTH]=0x0a;
			buf[pointer+i*EPD_WIDTH]|=(font16x24[(Character)][i] >> 8 & 0xff)   >> remainder;
			buf[pointer+i*EPD_WIDTH+1]|=(font16x24[(Character)][i] >> 8 & 0xff) << 8-remainder;
			buf[pointer+i*EPD_WIDTH+1]|=(font16x24[(Character)][i] & 0xff )     >> remainder;
			buf[pointer+i*EPD_WIDTH+2]|=(font16x24[(Character)][i] & 0xff )     << 8-remainder;
		}
	}
	else
	{
		for(int i=0;i<24;i++)		// for qrcode
		//for(int i=0;i<12;i++)
		{
			//buf[pointer+i*EPD_WIDTH]=0x0a;
			buf[pointer+i*EPD_WIDTH]=font16x24[(Character)][i] >>8 & 0xff;
			buf[pointer+i*EPD_WIDTH+1]=font16x24[(Character)][i] & 0xff;
		}
	}
}

void EINK_WriteString(uint8 *buf,uint8 x, uint8 y, uint8* Character)
{
  uint16 Len = strlen(Character);
  for(int i=0;i<Len;i++)
  {
    EINK_WriteFont(buf,x+i*7,y,Character[i]);
  }
}

void EINK_WriteString16(uint8 *buf,uint8 x, uint8 y, uint8* Character)
{
  uint16 Len = strlen(Character);
  for(int i=0;i<Len;i++)
  {
    EINK_WriteFont16(buf,x+i*10,y,Character[i]);
  }
}

void EINK_DrawingImage(uint8 *buf,uint8 x, uint8 y, uint8* image,uint16 ImageWidth,uint16 ImageHeight)
{
	uint16 pointer;
	uint8 remainder=x%8;
	pointer=(y)*EPD_WIDTH;
	pointer+=x/8;
        uint16 length = ImageWidth * ImageHeight;

#if 0
	do
	{
		for(int i=0;i<11;i++)		// for qrcode
		//for(int i=0;i<12;i++)
		{
			//buf[pointer+i*EPD_WIDTH]=0x0a;
			buf[pointer+i*EPD_WIDTH]=Font12_Table[(*Character-0x20)*12+i];
		}
		pointer++;
		Character++;
	}while(*Character);

#else
	for(int j=0;j<ImageHeight;j++)
	{
		if(remainder)
		{
			for(int i=0 ; i<ImageWidth;i++)		// for qrcode
			//for(int i=0;i<12;i++)
			{
				//buf[pointer+i*EPD_WIDTH]=0x0a;
				buf[pointer+i] |= ((image[j * ImageWidth +  i])) >> (remainder);
				buf[pointer+i+1] |= ((image[j * ImageWidth + i])) << (8-remainder);
			}
		}
		else
		{
			for(int i=0;i<ImageWidth-2;i++)		// for qrcode
			//for(int i=0;i<12;i++)
			{
				//buf[pointer+i*EPD_WIDTH]=0x0a;
				buf[pointer+(i)]=~image[(ImageHeight-j)*ImageWidth+i];
			}
                        buf[pointer+(ImageWidth-2)]=~image[(ImageHeight-j)*ImageWidth+ImageWidth-2]&0xE0;
		}
		pointer+=EPD_WIDTH;
	}

			for(int i=0 ; i<ImageWidth;i++)		// for qrcode
			//for(int i=0;i<12;i++)
			{
				//buf[pointer+i*EPD_WIDTH]=0x0a;
				//buf[pointer+i] |= ((image[ImageHeight * ImageWidth +  i])) >> (remainder);
				//buf[pointer+i+1] |= ((image[ImageHeight * ImageWidth + i])) << (8-remainder);
			}
                
#endif
}