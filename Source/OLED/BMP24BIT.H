#ifndef __BMP24BIT_H__
#define __BMP24BIT_H__

#include "MYFILE.h"
#include "stm32f10x_sdio.h"


//unsigned char xdata FAT_Buffer[512];


//typedef struct tagBITMAPFILEHEADER{//bmfh
//	UINT16 bfType;         //判定是否为BMP文件
//	UINT32 bfSize;         //文件大小
//	UINT16 bfReserved1;    //保留字节
//	UINT16 bfReserced2;    //保留字节
//	UINT32 bfOffBits;      //字节偏移量  BMP文件开始字节
//}BITMAPFILEHEADER;

//typedef struct tagBITMAPINFOHEADER{//bmih
//	UINT32 biSize;           //说明BITMAPINFOHEADER结构所需要的字节数
//	UINT32 biWidth;          //说明图像的宽度，以像素为单位，图像宽度必须是4的整数倍
//	INT32  biHeight;         //说明图像的高度，以像素为单位，如果是如负数，则是正向；如果是正数，则是倒向；大多数BMP文件都是倒向的位图
//	UINT16 biPlanes;         //标示bmp图片的平面属，显然显示器只有一个平面，所以恒等于1
//	UINT16 biBitCount;       //说明比特数/像素，其值为1、4、8、16、24或32.
//	UINT32 biCompression;    //说明图像数据压缩的类型，BI_RGB:没有压缩
//	UINT32 biSizeImage;      //说明图像的大小以字节为单位，当用BI_RGB格式时，可设置为0
//	UINT32 biXPelsPerMeter;  //水平分辨率，用像素/米表示�
//	UINT32 biYPelsPerMeter;  //垂直分辨率，用像素/米表示
//	UINT32 biClrUsed;        //位图实际使用的彩色表中的颜色索引数(设置为0的话，则使用所有调色板)
//	UINT32 biClrImportant;   //对图像显示有重要影响的颜色索引的数目，如果是0，表示都重要
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
//	UINT16 bfType;         //判定是否为BMP文件
//	UINT32 bfSize;         //文件大小
//	UINT16 bfReserved1;    //保留字节
//	UINT16 bfReserced2;    //保留字节
//	UINT32 bfOffBits;      //字节偏移量  BMP文件开始字节
//};

//struct tagBITMAPINFOHEADER{//bmih
//	UINT32 biSize;           //说明BITMAPINFOHEADER结构所需要的字节数
//	UINT32 biWidth;          //说明图像的宽度，以像素为单位，图像宽度必须是4的整数倍
//	INT32  biHeight;         //说明图像的高度，以像素为单位，如果是如负数，则是正向；如果是正数，则是倒向；大多数BMP文件都是倒向的位图
//	UINT16 biPlanes;         //标示bmp图片的平面属，显然显示器只有一个平面，所以恒等于1
//	UINT16 biBitCount;       //说明比特数/像素，其值为1、4、8、16、24或32.  ***
//	UINT32 biCompression;    //说明图像数据压缩的类型，BI_RGB:没有压缩   ***
//	UINT32 biSizeImage;      //说明图像的大小以字节为单位，当用BI_RGB格式时，可设置为0  ***
//	UINT32 biXPelsPerMeter;  //水平分辨率，用像素/米表示�
//	UINT32 biYPelsPerMeter;  //垂直分辨率，用像素/米表示  ***
//	UINT32 biClrUsed;        //位图实际使用的彩色表中的颜色索引数(设置为0的话，则使用所有调色板)
//	UINT32 biClrImportant;   //对图像显示有重要影响的颜色索引的数目，如果是0，表示都重要
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




