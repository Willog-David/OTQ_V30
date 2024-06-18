#ifndef __BMP24BIT_H__
#define __BMP24BIT_H__

#include "MYFILE.h"
#include "stm32f10x_sdio.h"


//unsigned char xdata FAT_Buffer[512];


//typedef struct tagBITMAPFILEHEADER{//bmfh
//	UINT16 bfType;         //ÅĞ¶¨ÊÇ·ñÎªBMPÎÄ¼ş
//	UINT32 bfSize;         //ÎÄ¼ş´óĞ¡
//	UINT16 bfReserved1;    //±£Áô×Ö½Ú
//	UINT16 bfReserced2;    //±£Áô×Ö½Ú
//	UINT32 bfOffBits;      //×Ö½ÚÆ«ÒÆÁ¿  BMPÎÄ¼ş¿ªÊ¼×Ö½Ú
//}BITMAPFILEHEADER;

//typedef struct tagBITMAPINFOHEADER{//bmih
//	UINT32 biSize;           //ËµÃ÷BITMAPINFOHEADER½á¹¹ËùĞèÒªµÄ×Ö½ÚÊı
//	UINT32 biWidth;          //ËµÃ÷Í¼ÏñµÄ¿í¶È£¬ÒÔÏñËØÎªµ¥Î»£¬Í¼Ïñ¿í¶È±ØĞëÊÇ4µÄÕûÊı±¶
//	INT32  biHeight;         //ËµÃ÷Í¼ÏñµÄ¸ß¶È£¬ÒÔÏñËØÎªµ¥Î»£¬Èç¹ûÊÇÈç¸ºÊı£¬ÔòÊÇÕıÏò£»Èç¹ûÊÇÕıÊı£¬ÔòÊÇµ¹Ïò£»´ó¶àÊıBMPÎÄ¼ş¶¼ÊÇµ¹ÏòµÄÎ»Í¼
//	UINT16 biPlanes;         //±êÊ¾bmpÍ¼Æ¬µÄÆ½ÃæÊô£¬ÏÔÈ»ÏÔÊ¾Æ÷Ö»ÓĞÒ»¸öÆ½Ãæ£¬ËùÒÔºãµÈÓÚ1
//	UINT16 biBitCount;       //ËµÃ÷±ÈÌØÊı/ÏñËØ£¬ÆäÖµÎª1¡¢4¡¢8¡¢16¡¢24»ò32.
//	UINT32 biCompression;    //ËµÃ÷Í¼ÏñÊı¾İÑ¹ËõµÄÀàĞÍ£¬BI_RGB:Ã»ÓĞÑ¹Ëõ
//	UINT32 biSizeImage;      //ËµÃ÷Í¼ÏñµÄ´óĞ¡ÒÔ×Ö½ÚÎªµ¥Î»£¬µ±ÓÃBI_RGB¸ñÊ½Ê±£¬¿ÉÉèÖÃÎª0
//	UINT32 biXPelsPerMeter;  //Ë®Æ½·Ö±æÂÊ£¬ÓÃÏñËØ/Ã×±íÊ¾×
//	UINT32 biYPelsPerMeter;  //´¹Ö±·Ö±æÂÊ£¬ÓÃÏñËØ/Ã×±íÊ¾
//	UINT32 biClrUsed;        //Î»Í¼Êµ¼ÊÊ¹ÓÃµÄ²ÊÉ«±íÖĞµÄÑÕÉ«Ë÷ÒıÊı(ÉèÖÃÎª0µÄ»°£¬ÔòÊ¹ÓÃËùÓĞµ÷É«°å)
//	UINT32 biClrImportant;   //¶ÔÍ¼ÏñÏÔÊ¾ÓĞÖØÒªÓ°ÏìµÄÑÕÉ«Ë÷ÒıµÄÊıÄ¿£¬Èç¹ûÊÇ0£¬±íÊ¾¶¼ÖØÒª
//}BITMAPINFOHEADER;         //


//typedef struct tagPALETTEENTRY{//pe
//	UINT8 peRed;
//	UINT8 peGreen;
//	UINT8 peBlue;
//	UINT8 peFlags;
//}PALETTEENTRY;


//typedef struct tagBITMAP_FILE{
//	BITMAPFILEHEADER bitmapfileheader;
//	BITMAPINFOHEADER bitmapinfoheader;
//	PALETTEENTRY     palette[256];
//	UINT8            *buffer;
//}BITMAP_FILE;

//struct tagBITMAPFILEHEADER{//bmfh 
//	UINT32 dumy_1;
//	UINT32 dumy_2;
//	UINT16 bfType;         //ÅĞ¶¨ÊÇ·ñÎªBMPÎÄ¼ş
//	UINT32 bfSize;         //ÎÄ¼ş´óĞ¡
//	UINT16 bfReserved1;    //±£Áô×Ö½Ú
//	UINT16 bfReserced2;    //±£Áô×Ö½Ú
//	UINT32 bfOffBits;      //×Ö½ÚÆ«ÒÆÁ¿  BMPÎÄ¼ş¿ªÊ¼×Ö½Ú
//};

//struct tagBITMAPINFOHEADER{//bmih
//	UINT32 biSize;           //ËµÃ÷BITMAPINFOHEADER½á¹¹ËùĞèÒªµÄ×Ö½ÚÊı
//	UINT32 biWidth;          //ËµÃ÷Í¼ÏñµÄ¿í¶È£¬ÒÔÏñËØÎªµ¥Î»£¬Í¼Ïñ¿í¶È±ØĞëÊÇ4µÄÕûÊı±¶
//	INT32  biHeight;         //ËµÃ÷Í¼ÏñµÄ¸ß¶È£¬ÒÔÏñËØÎªµ¥Î»£¬Èç¹ûÊÇÈç¸ºÊı£¬ÔòÊÇÕıÏò£»Èç¹ûÊÇÕıÊı£¬ÔòÊÇµ¹Ïò£»´ó¶àÊıBMPÎÄ¼ş¶¼ÊÇµ¹ÏòµÄÎ»Í¼
//	UINT16 biPlanes;         //±êÊ¾bmpÍ¼Æ¬µÄÆ½ÃæÊô£¬ÏÔÈ»ÏÔÊ¾Æ÷Ö»ÓĞÒ»¸öÆ½Ãæ£¬ËùÒÔºãµÈÓÚ1
//	UINT16 biBitCount;       //ËµÃ÷±ÈÌØÊı/ÏñËØ£¬ÆäÖµÎª1¡¢4¡¢8¡¢16¡¢24»ò32.  ***
//	UINT32 biCompression;    //ËµÃ÷Í¼ÏñÊı¾İÑ¹ËõµÄÀàĞÍ£¬BI_RGB:Ã»ÓĞÑ¹Ëõ   ***
//	UINT32 biSizeImage;      //ËµÃ÷Í¼ÏñµÄ´óĞ¡ÒÔ×Ö½ÚÎªµ¥Î»£¬µ±ÓÃBI_RGB¸ñÊ½Ê±£¬¿ÉÉèÖÃÎª0  ***
//	UINT32 biXPelsPerMeter;  //Ë®Æ½·Ö±æÂÊ£¬ÓÃÏñËØ/Ã×±íÊ¾×
//	UINT32 biYPelsPerMeter;  //´¹Ö±·Ö±æÂÊ£¬ÓÃÏñËØ/Ã×±íÊ¾  ***
//	UINT32 biClrUsed;        //Î»Í¼Êµ¼ÊÊ¹ÓÃµÄ²ÊÉ«±íÖĞµÄÑÕÉ«Ë÷ÒıÊı(ÉèÖÃÎª0µÄ»°£¬ÔòÊ¹ÓÃËùÓĞµ÷É«°å)
//	UINT32 biClrImportant;   //¶ÔÍ¼ÏñÏÔÊ¾ÓĞÖØÒªÓ°ÏìµÄÑÕÉ«Ë÷ÒıµÄÊıÄ¿£¬Èç¹ûÊÇ0£¬±íÊ¾¶¼ÖØÒª
//};         //


//struct tagPALETTEENTRY{//pe
//	UINT8 peRed;
//	UINT8 peGreen;
//	UINT8 peBlue;
//	UINT8 peFlags;
//};


//struct tagBITMAP_FILE{
//	struct tagBITMAPFILEHEADER bitmapfileheader;
//	struct tagBITMAPINFOHEADER bitmapinfoheader;
//	struct tagPALETTEENTRY     palette[256];
//	UINT8            *buffer;
//};

UINT8 FAT_Init(void);
UINT8 image_all_file(void);
UINT32 Read_BMP24BIT_File(UINT32 clu,UINT32 file_size,UINT8 COLOR,UINT16 X_pixel,UINT16 Y_pixel);


#endif




