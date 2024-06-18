/*************************************************************/
/*                   FAT操作函数库                           */
/*  环境WinAVR 20060421                                      */
/*  作者：Bozai（章其波）                                    */
/*  E-mail：sudazqb@163.com                                  */
/*  2007年2月13日                                            */
/*************************************************************/


#ifndef __FAT_H__
#define __FAT_H__
#include<stdlib.h>
#include "MYFILE.h"


#define CHAR	char
#define BYTE	unsigned char
#define WORD	unsigned short
#define DWORD	unsigned int

#define NFATITEMBYTES   (4)    //FAT表项所占用的字节数
#define NITEMSINFATSEC  (128)  //FAT表一个扇区中包含的表项数


#define FAT_BUFFERED 1

#define MAX_LONG_NAME_SIZE 132	/* 26*n+2   n=5 */
#define FAT_DEBUG 0		/* FAT debug flag */

#define FIX_DIRECTORY 0		/* 1 means use fix directory, 0 for any directory */


#if FIX_DIRECTORY==0

	#define  RECORD_SIZE 256	
	#define  RECORD_ADDR_START 0x0000	/* eeprom start address */
	#define  RECORD_ADDR_END  RECORD_SIZE*4	/* eeprom end address */
	
#endif

//extern BYTE (* SD_ReadBlock)(DWORD,BYTE *);
//extern BYTE (* SD_WriteBlock)(DWORD,BYTE *);
//extern DWORD (* FAT_ReadCapacity)(void);

//声明外部器件读写函数

extern BYTE CH375_ReadOneSector(DWORD LBA,BYTE *buffer);
extern BYTE CH375_WriteOneSector(DWORD LBA,BYTE *buffer);
extern BYTE SD_ReadSingleBlock(DWORD sector, BYTE* buffer);
extern BYTE SD_WriteSingleBlock(DWORD sector, BYTE* buffer);
extern BYTE IDE_ReadOneSector(DWORD LBA,BYTE *buffer);
extern BYTE IDE_WriteOneSector(DWORD LBA,BYTE *buffer);

extern DWORD SD_GetCapacity(void);



#define MSDOSFSROOT     0               // cluster 0 means the root dir
#define CLUST_FREE      0               // cluster 0 also means a free cluster
#define MSDOSFSFREE     CLUST_FREE
#define CLUST_FIRST     2             	// first legal cluster number
#define CLUST_RSRVD     0xfff6      	// reserved cluster range
#define CLUST_BAD       0xfff7     	// a cluster with a defect
#define CLUST_EOFS      0xfff8     	// start of eof cluster range
#define CLUST_EOFE      0xffff      	// end of eof cluster range



#pragma pack(push,1)
typedef struct
{			
	BYTE	prIsActive;		// 0x80 indicates active partition
	BYTE	prStartHead;		// starting head for partition
	WORD	prStartCylSect;		// starting cylinder and sector
	BYTE	prPartType;		// partition type (see above)
	BYTE	prEndHead;		// ending head for this partition
	WORD	prEndCylSect;		// ending cylinder and sector
	DWORD	prStartLBA;		// first LBA sector for this partition
	DWORD	prSize;			// size of this partition (bytes or sectors ?)
} partrecord;
#pragma pack(pop)

        
#pragma pack(push,1)
typedef struct
{
	CHAR	psPartCode[512-64-2];	// pad so struct is 512b
	BYTE	psPart[64];		// four partition records (64 bytes)
	BYTE	psBootSectSig0;		// two signature bytes (2 bytes)
	BYTE	psBootSectSig1;
#define BOOTSIG0        0x55
#define BOOTSIG1        0xaa
} partsector;
#pragma pack(pop)


#pragma pack(push,1)
typedef struct
{
	CHAR	exDriveNumber;		// drive number (0x80)//0x00 for floopy disk 0x80 for hard disk
	CHAR	exReserved1;		// reserved should always set 0
	CHAR	exBootSignature;	// ext. boot signature (0x29)
#define EXBOOTSIG       0x29
	CHAR	exVolumeID[4];		// volume ID number
	CHAR	exVolumeLabel[11];	// volume label "NO NAME"
	CHAR	exFileSysType[8];	// fs type (FAT12 or FAT)
} extboot;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
	BYTE	bsJump[3];		// jump inst E9xxxx or EBxx90
	CHAR	bsOemName[8];		// OEM name and version
	CHAR	bsBPB[25];		// BIOS parameter block
	CHAR	bsExt[26];		// Bootsector Extension
	CHAR	bsBootCode[448];	// pad so structure is 512b
	BYTE	bsBootSectSig0;		// boot sector signature byte 0x55 
	BYTE	bsBootSectSig1;		// boot sector signature byte 0xAA
#define BOOTSIG0        0x55
#define BOOTSIG1        0xaa
} bootsector50;
#pragma pack(pop)


#pragma pack(push,1)
typedef struct
{
        WORD	bpbBytesPerSec; 	// bytes per sector	//512 1024 2048 or 4096
        BYTE	bpbSecPerClust; 	// sectors per cluster	// power of 2
        WORD	bpbResSectors;  	// number of reserved sectors	//1 is recommend
        BYTE	bpbFATs;        	// number of FATs	// 2 is recommend
        WORD	bpbRootDirEnts; 	// number of root directory entries
        WORD	bpbSectors;     	// total number of sectors
        BYTE	bpbMedia;       	// media descriptor	//0xf8 match the fat[0]
        WORD	bpbFATsecs;     	// number of sectors per FAT
        WORD	bpbSecPerTrack; 	// sectors per track
        WORD	bpbHeads;       	// number of heads
        DWORD	bpbHiddenSecs;  	// # of hidden sectors
        DWORD	bpbHugeSectors; 	// # of sectors if bpbSectors == 0
} bpb50;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
	BYTE	bsJump[3];		// jump inst E9xxxx or EBxx90
	CHAR	bsOemName[8];		// OEM name and version
	CHAR	bsBPB[53];		// BIOS parameter block
	CHAR	bsExt[26];		// Bootsector Extension
	CHAR	bsBootCode[418];	// pad so structure is 512b
	BYTE	bsBootSectSig2;		// boot sector signature byte 0x00 
	BYTE	bsBootSectSig3;		// boot sector signature byte 0x00
	BYTE	bsBootSectSig0;		// boot sector signature byte 0x55 
	BYTE	bsBootSectSig1;		// boot sector signature byte 0xAA
#define BOOTSIG0        0x55
#define BOOTSIG1        0xaa
#define BOOTSIG2        0x00
#define BOOTSIG3        0x00
} bootsector710 ;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
	WORD	bpbBytesPerSec;		// bytes per sector
	BYTE	bpbSecPerClust;		// sectors per cluster
	WORD	bpbResSectors;		// number of reserved sectors
	BYTE	bpbFATs;		// number of FATs
	WORD	bpbRootDirEnts;		// number of root directory entries
	WORD	bpbSectors;		// total number of sectors
	BYTE	bpbMedia;		// media descriptor
	WORD	bpbFATsecs;		// number of sectors per FAT
	WORD	bpbSecPerTrack;		// sectors per track
	WORD	bpbHeads;		// number of heads
	DWORD	bpbHiddenSecs;		// # of hidden sectors
// 3.3 compat ends here
	DWORD	bpbHugeSectors;		// # of sectors if bpbSectors == 0
// 5.0 compat ends here
	DWORD     bpbBigFATsecs;	// like bpbFATsecs for FAT32
	WORD      bpbExtFlags;		// extended flags:
#define FATNUM    0xf			// mask for numbering active FAT
#define FATMIRROR 0x80			// FAT is mirrored (like it always was)
	WORD      bpbFSVers;		// filesystem version
#define FSVERS    0			// currently only 0 is understood
	DWORD     bpbRootClust;		// start cluster for root directory
	WORD      bpbFSInfo;		// filesystem info structure sector
	WORD      bpbBackup;		// backup boot sector
	// There is a 12 byte filler here, but we ignore it
} bpb710;
#pragma pack(pop)


// Structure of a dos directory entry.	 FAT32的目录项定义 http://blog.csdn.net/kv110/archive/2009/09/26/4596875.aspx
#pragma pack(push,1)
typedef struct
{
	BYTE		deName[8];      // filename, blank filled
#define SLOT_EMPTY      0x00            // slot has never been used
#define SLOT_E5         0x05            // the real value is 0xE5
#define SLOT_DELETED    0xE5            // file in this slot deleted
#define SLOT_DIR		0x2E	// a directorymmm
	BYTE		deExtension[3]; // extension, blank filled
	BYTE		deAttributes;   // file attributes	文件属性（最高两位保留未用，0至5位分别是b0只读位、b1隐藏位、b2系统位、b3卷标位、b4子目录位、b5归档位 00读写）
#define ATTR_NORMAL     0x00            // normal file
#define ATTR_READONLY   0x01            // file is readonly
#define ATTR_HIDDEN     0x02            // file is hidden
#define ATTR_SYSTEM     0x04            // file is a system file
#define ATTR_VOLUME     0x08            // entry is a volume label
#define ATTR_LONG_FILENAME	0x0F	// this is a long filename entry			    
#define ATTR_DIRECTORY  0x10            // entry is a directory name
#define ATTR_ARCHIVE    0x20            // file is new or modified
	BYTE        deLowerCase;    	// NT VFAT lower case flags  (set to zero) --仅长文件名目录项用，用来存储其对应的短文件名目录项的文件名字节校验和等?
#define LCASE_BASE      0x08            // filename base in lower case
#define LCASE_EXT       0x10            // filename extension in lower case
	BYTE        deCHundredth;   	// hundredth of seconds in CTime   24位二进制的文件建立时间，其中的高5位为小时，次6位为分钟
	BYTE        deCTime[2];     	// create time
	BYTE        deCDate[2];     	// create date 16位二进制的文件建立日期，其中的高7位为相对于1980年的年份值，次4位为月份，后5位为月内日期
	BYTE        deADate[2];     	// access date	16位二进制的文件最新访问日期，定义同上
	WORD        deHighClust; 	// high bytes of cluster number	起始簇号的高16位
	BYTE        deMTime[2];     	// last update time	   16位二进制的文件最新修改时间，其中的高5位为小时，次6位为分钟，后5位的二倍为秒数
	BYTE        deMDate[2];     	// last update date	 16位二进制的文件建立日期，其中的高7位为相对于1980年的年份值，次4位为月份，后5位为月内日期
	WORD        deStartCluster; 	// starting cluster of file	起始簇号的低16位
	DWORD       deFileSize;  	// size of file in bytes32位的文件字节长度（对于子目录，此项为0）
} direntry;
#pragma pack(pop)

// number of directory entries in one sector
#define DIRENTRIES_PER_SECTOR	0x10	//when the bpbBytesPerSec=512 

// Structure of a Win95 long name directory entry

#pragma pack(push,1)
typedef struct
{
	BYTE		weCnt;		// 
#define WIN_LAST        0x40
#define WIN_CNT         0x3f
	BYTE		wePart1[10];
	BYTE		weAttributes;
#define ATTR_WIN95      0x0f
	BYTE		weReserved1;
	BYTE		weChksum;
	BYTE		wePart2[12];
	WORD       	weReserved2;
	BYTE		wePart3[4];
} winentry;
#pragma pack(pop)

#define WIN_ENTRY_CHARS	13      	// Number of chars per winentry

// Maximum filename length in Win95
// Note: Must be < sizeof(dirent.d_name)
#define WIN_MAXLEN      255

// This is the format of the contents of the deTime field in the direntry
// structure.
// We don't use bitfields because we don't know how compilers for
// arbitrary machines will lay them out.
#define DT_2SECONDS_MASK        0x1F    // seconds divided by 2
#define DT_2SECONDS_SHIFT       0
#define DT_MINUTES_MASK         0x7E0   // minutes
#define DT_MINUTES_SHIFT        5
#define DT_HOURS_MASK           0xF800  // hours
#define DT_HOURS_SHIFT          11

// This is the format of the contents of the deDate field in the direntry
// structure.
#define DD_DAY_MASK		0x1F	// day of month
#define DD_DAY_SHIFT		0
#define DD_MONTH_MASK		0x1E0	// month
#define DD_MONTH_SHIFT		5
#define DD_YEAR_MASK		0xFE00	// year - 1980
#define DD_YEAR_SHIFT		9



// Stuctures
#pragma pack(push,1)
typedef struct
{
	DWORD StartCluster;		//< file starting cluster for last file accessed
	DWORD Size;			//< file size for last file accessed
	BYTE Attr;			//< file attr for last file accessed
	//unsigned short CreateTime;	//< file creation time for last file accessed
	//unsigned short CreateDate;	//< file creation date for last file accessed
	DWORD Sector;			//<file record place
	WORD Offset;			//<file record offset
} FileInfoStruct;
#pragma pack(pop)


//MP3后添加的函数

#if FIX_DIRECTORY
	extern BYTE Search(BYTE *dir, direntry *MusicInfo,WORD *Count,BYTE *type);//查找音乐文件
#else
	extern BYTE SearchInit(void);
	extern BYTE Search(WORD *music_record_addr, direntry *MusicInfo,WORD *Count,BYTE *type);//查找音乐文件
	extern BYTE SearchLrc(BYTE *dir,BYTE * longnamebuffer,direntry *LrcInfo,WORD music_record_addr);//serch lrc file
#endif

extern BYTE FAT_LoadPartCluster(DWORD cluster,BYTE part,BYTE * buffer);//加载文件


//DPT:分区表记录结构如下
struct DPT_Item
{
 UINT8 Active;         //0x80表示此分区有效
 UINT8 StartHead;      //分区的开始磁头
 UINT8 StartSect;      //开始扇区
 UINT8 StartCyl;       //开始柱面
 UINT8 PartType;       //分区类型
 UINT8 EndHead;        //分区的结束头
 UINT8 EndSect;        //结束扇区
 UINT8 EndCyl;         //结束柱面
 UINT8 StartLBA[4];    //分区的第一个扇区
 UINT8 Size[4];        //分区的大小，总扇区数
};

//MBR扇区（绝对0扇区）定义如下
struct MBR
{
 UINT8 PartCode[446]; //MBR的引导程序
 struct DPT_Item Part[4]; //4个分区记录
 UINT8 BootSectSig0;  //55
 UINT8 BootSectSig1;  //AA
};

//typedef struct 
//{
// UINT32 BPB_Sector_No;   //DBR(BPB)所在扇区号*****
//
// UINT32 Total_SizeKB;    //磁盘的总容量，单位为KB  ******
// UINT32 BytesPerSector;	 //每个扇区的字节数
// UINT32 FATsectors;      //FAT表所占扇区数
// UINT32 SectorsPerClust1; //每簇的扇区数
// UINT32 FirstFATSector;	 //第一个FAT表所在扇区
// UINT32 FirstDirSector;	 //第一个目录所在扇区
//// UINT32 FirstDirCluster;	 //第一个目录所在的簇
//	
// UINT32 FSINFO_Sec;      //FSINFO扇区所在的扇区
// UINT32 Free_nCluster;   //空闲簇的个数
// UINT32 Next_Free_Cluster; //下一空簇  *****
//}FAT_Init_Args;

struct FAT_Item  //znFAT中对FAT表项的结构定义
{
 UINT8 Item[NFATITEMBYTES]; //FAT32中FAT表项占用4个字节，即32位
};
	
struct FAT_Sec	//znFAT中对FAT表扇区结构的定义
{
 struct FAT_Item items[NITEMSINFATSEC]; //FAT扇区包含128个FAT表项
	                                //FAT扇区的结构就是有128个FAT表项类型的数组
};


struct FDI
{
 UINT8 Name[8];         // 文件名，不足部分以空格补充
 UINT8 Extension[3]; 	// 扩展名，不足部分以空格补充
 UINT8 Attributes;   	// 文件属性
 UINT8 LowerCase;    	// 0
 UINT8 CTime10ms;   	// 创建时间的10毫秒位
 UINT8 CTime[2];     	// 创建时间
 UINT8 CDate[2];     	// 创建日期
 UINT8 ADate[2];     	// 访问日期
 UINT8 HighClust[2];    // 开始簇的高字
 UINT8 MTime[2];     	// 最近的修改时间
 UINT8 MDate[2];     	// 最近的修改日期
 UINT8 LowClust[2]; 	// 开始簇的低字
 UINT8 FileSize[4];     // 文件大小
};

struct LFN_FDI //长名的文件目录项结构定义
{
 UINT8 AttrByte[1]; //属性字节
 UINT8 Name1[10];   //第一部分长名
 UINT8 LFNSign[1];  //长名项标志
 UINT8 Resv[1];     //保留
 UINT8 ChkVal[1];   //检验值，与SFN的绑定校验
 UINT8 Name2[12];   //第二部分长名
 UINT8 StartClu[2]; //取0
 UINT8 Name3[4];    //第三部分长名
};

struct FDIesInSEC
{
 struct FDI FDIes[16]; //扇区中的文件目录项数组
};

typedef struct 
{
 UINT8 BS_jmpBoot[3];     //跳转指令            offset: 0
 UINT8 BS_OEMName[8];     //OEM名称             offset: 3

 UINT8 BPB_BytesPerSec[2];//每扇区字节数        offset:11
 UINT8 BPB_SecPerClus;    //每簇扇区数          offset:13
 UINT8 BPB_RsvdSecCnt[2]; //保留扇区数目        offset:14
 UINT8 BPB_NumFATs;       //此卷中FAT表数       offset:16
 UINT8 BPB_RootEntCnt[2]; //FAT32为0            offset:17
 UINT8 BPB_TotSec16[2];   //FAT32为0            offset:19
 UINT8 BPB_Media;         //存储介质            offset:21
 UINT8 BPB_FATSz16[2];    //FAT32为0            offset:22
 UINT8 BPB_SecPerTrk[2];  //磁道扇区数          offset:24
 UINT8 BPB_NumHeads[2];   //磁头数              offset:26
 UINT8 BPB_HiddSec[4];    //FAT区前隐扇区数     offset:28
 UINT8 BPB_TotSec32[4];   //该卷总扇区数        offset:32
 UINT8 BPB_FATSz32[4];    //一个FAT表扇区数     offset:36
 UINT8 BPB_ExtFlags[2];   //FAT32特有           offset:40
 UINT8 BPB_FSVer[2];      //FAT32特有           offset:42
 UINT8 BPB_RootClus[4];   //根目录簇号          offset:44
 UINT8 FSInfo[2];         //保留扇区FSINFO扇区数offset:48
 UINT8 BPB_BkBootSec[2];  //通常为6             offset:50
 UINT8 BPB_Reserved[12];  //扩展用              offset:52
 UINT8 BS_DrvNum;         //                    offset:64
 UINT8 BS_Reserved1;      //                    offset:65
 UINT8 BS_BootSig;        //                    offset:66
 UINT8 BS_VolID[4];       //                    offset:67
 UINT8 BS_FilSysType[11]; //	                offset:71
 UINT8 BS_FilSysType1[8]; //"FAT32    "         offset:82
}DBR;


struct FSInfo //znFAT中对文件系统信息结构的定义
{
 UINT8 Head[4]; //"RRaA"
 UINT8 Resv1[480];
 UINT8 Sign[4]; //"rrAa"
 UINT8 Free_Cluster[4]; //剩余空簇数
 UINT8 Next_Free_Cluster[4]; //下一空簇参考值
 UINT8 Resv2[14]; 
 UINT8 Tail[2]; //"55 AA"
};


#define MBR_SECTOR (0) //MBRY扇区
#define DBR_MARK {0XEB,0X58,0X90} //DBR的标志码
#define IS_FAT32_TYPE(FST) (('F'==(FST[0])) && ('A'==(FST[1])) && ('T'==(FST[2])) && ('3'==(FST[3])) && ('2'==(FST[4]))) //检验文件系统是否FAT32 
#define FSTYPE_NOT_FAT32          (2)

//FAT原有的函数
extern BYTE FAT_Init(void);//初始化
//extern BYTE FAT_LoadCluster(DWORD cluster,BYTE * buffer);//读一个扇区
extern DWORD FAT_NextCluster(DWORD cluster);//查找下一簇号
//extern DWORD FAT_FindFreeCluster(void);//查找空簇
extern BYTE FAT_DisDir(BYTE *dir);//显示目录内容
extern WORD FAT_FindItem(DWORD cluster, BYTE *name, FileInfoStruct *  FileInfo);//查找文件
extern DWORD FAT_Open(BYTE * dir);//打开文件
extern DWORD FAT_OpenDir(BYTE * dir);//打开目录
extern BYTE FAT_Read(DWORD pointer, DWORD size);//读文件，size=0代表整个文件
extern BYTE FAT_Rename(BYTE *dir,BYTE *newname);//重命名
extern BYTE FAT_ModifyFAT(DWORD cluster,DWORD val);//修改FAT项
extern BYTE FAT_Delete(BYTE *dir);//删除文件
extern BYTE FAT_DelItem( FileInfoStruct  *FileInfo);//删除记录项
extern WORD FAT_FindFreeItem(DWORD cluster, FileInfoStruct  *FileInfo);//查找空余项
extern BYTE FAT_MkDir(BYTE * dir);//建目录
extern BYTE FAT_Write(DWORD cluster,BYTE *data,DWORD size);//写文件
extern DWORD FAT_Create(BYTE * dir,DWORD size);//建文件，必须指定文件大小
extern BYTE FAT_RmDir(BYTE * dir);//删除目录
extern WORD FAT_Close(DWORD * p);//关闭文件

extern void CopyDirentruyItem( direntry  *Desti,direntry *Source);
extern BYTE Search_man( direntry  *MusicInfo,WORD *Count,BYTE *type);

#endif
