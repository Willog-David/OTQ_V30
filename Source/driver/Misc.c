
#include "misc.h"
#include "string.h"


void BitShifter(uint8 *buffer,uint16 size, uint8 bitlen)
{
	for(int i=0;i<size-1;i++)
	{
		buffer[size-i]>>=bitlen;
		buffer[size-i]|=buffer[size-i-1]<<(8-bitlen);
	}
}


void BitDoubler(uint8 *Buf1, uint16 Location, uint16 Len)
{
	uint8 DoublerBuf[40];
	uint8 UpperByte,LowerByte;


	memset(DoublerBuf,0x00,40);
	for(int i=0;i<Len;i++)
	{
		DoublerBuf[i*2]=Buf1[Location+i];
	}
	for(int i=0;i<Len;i++)
	{
		UpperByte=0;
		LowerByte=0;
		for(int j=0;j<4;j++)
		{
			UpperByte>>=1;
			UpperByte|=(DoublerBuf[i*2]&(0x10<<j))<<(3-j);
			UpperByte>>=1;
			UpperByte|=(DoublerBuf[i*2]&(0x10<<j))<<(3-j);

			LowerByte>>=1;
			LowerByte|=(DoublerBuf[i*2]&(0x01<<j))<<(7-j);
			LowerByte>>=1;
			LowerByte|=(DoublerBuf[i*2]&(0x01<<j))<<(7-j);
		}
		DoublerBuf[i*2]=UpperByte;
		DoublerBuf[i*2+1]=LowerByte;
	}
	for(int i=0;i<Len*2;i++)
	{
		Buf1[Location+i]=DoublerBuf[i];
	}
}

void BitCopy(uint8 *Buf1, uint16 Location, uint8 *Buf2, uint16 BitNumber,uint16 Len)
{
	uint8 StartRemainder=BitNumber%8;
	uint8 tmp;

	for(int i=0;i<Len/8;i++)
	{
		tmp=((Buf2[BitNumber/8+i]<<StartRemainder)|(Buf2[BitNumber/8+i+1]>>(8-StartRemainder)));
		Buf1[Location/8+i]=((Buf2[BitNumber/8+i]<<StartRemainder)|(Buf2[BitNumber/8+i+1]>>(8-StartRemainder)));
	}
	if(Len%8)
	{
		if(StartRemainder<=3)
		{
			if(Len==49 | Len==57 | Len == 65 | Len == 81)
			{
				tmp=((Buf2[BitNumber/8+Len/8]<<StartRemainder)&0x80);
			}
			else
			{
				tmp=((Buf2[BitNumber/8+Len/8]<<StartRemainder)&0xf8);
			}

			//tmp|=(((Buf2[(BitNumber/8+Len/8)+1])>>(8-StartRemainder))&(0xff<<(8-StartRemainder)));

		}
		else
		{
			if(Len==49 | Len==57 | Len == 65 | Len == 81)
			{
				tmp=((Buf2[BitNumber/8+Len/8]<<StartRemainder)&0x80);
			}
			else
			{
				tmp=((Buf2[BitNumber/8+Len/8]<<StartRemainder)&(0xff<<(8-StartRemainder)));
				tmp|=(((Buf2[(BitNumber/8+Len/8)+1])>>(8-StartRemainder))&(0xf8));
			}
		}
		//Buf1[Location/8+Len/8]=((Buf2[BitNumber/8+Len/8]<<StartRemainder)|(Buf2[BitNumber/8+Len/8]>>(8-StartRemainder)))&0xff<<(8-BitRemainder);
		Buf1[Location/8+Len/8]=tmp;
	}
}



uint8 QRLogger_HextoAscii(uint8 hex)
{
	if(hex<10) return hex + 48;
	else return hex + 55;
}