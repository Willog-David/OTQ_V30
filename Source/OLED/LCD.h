/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : lcd.h
* Author             : MCD Application Team
* Version            : V1.1.2
* Date               : 09/22/2008
* Description        : 
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H	  
/* Includes ------------------------------------------------------------------*/
#include "struct.h"
#include "OTQV30_Init.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
 #define uint uint16 
//-------------*******--LCD端口定义-------*********** ---------------
//因为DEMO板与测试版不同，故针对DEMO板，"#define STM32DEMO" ,否则把下面的语句注释掉即可

#define	LCDPin_RS	(1<<3)  //数据/命令选择端口	 PA1
#define	LCDPin_CS	(1<<6)  //LCD片选端口  	 PA6     
#define	LCDPin_RST	(1<<1 )  //硬复位	PA5
#define	LCDPin_RD	(1<<4)  //时钟端口	PA4      
#define	LCDPin_WR	(1<<5)  //数据端口	PA3	
#define LCDPin_SCLK (1<<0)	//SPI时钟
#define LCDPin_SDIN (1<<1)	//SPI数据输入

//#define LCD_SCLK_set() 	(GPIOB->ODR |= 0x01)
//#define LCD_SCLK_claer() 	(GPIOB->ODR &= 0xfffe)	 //(GPIO_TypeDef *)0x40010C00

#define LCD_SCLK_set() 	(GPIOB->ODR = 0x40010C01)
#define LCD_SCLK_claer() (GPIOB->ODR = 0x40010C00)	 //(GPIO_TypeDef *)0x40010C00

#define LCD_SDIN_set() 	(GPIOB->ODR |= 0x02) 
#define LCD_SDIN_claer() 	(GPIOB->ODR &= 0xfffd)

/******未选择哪种显示方适时设置为0******/
#define full_color 0  //1
#define mono_color 0 //2
#define gray_scale 3  //3

#define Interface_4_wire_SPI //Interface:Interface_8080/Interface_4_wire_SPI

#define YDP 160
#define XDP 128

//#define STM32DEMO
//
//#ifdef STM32DEMO
//  #define	SSD1963Pin_CS	  (1<<2)   //SSD1963片选端口  	         
//  #define	LCDPin_RST	(1<<0)   //硬复位		 
//#else  
//  #define	SSD1963Pin_CS	  (1<<0)   //SSD1963片选端口  	        
//  #define	LCDPin_RST	(1<<2)   //硬复位	 
//#endif 
//
//#define	LCDPin_RS	(1<<3)//A0
//#define	LCDPin_CS	(1<<1)//CS       
//#define	LCDPin_RST	(1<<0)//RST  	
//#define	LCDPin_RD	(1<<4)//SDA    
//#define	LCDPin_WR	(1<<5)//SCL 	
//#define	S1963_CS	(1<<2)


#define EPD_W21_MOSI_0	nrf_gpio_pin_write(OTQV2_GPIO_EPD_DA, 0)
#define EPD_W21_MOSI_1	nrf_gpio_pin_write(OTQV2_GPIO_EPD_DA, 1)

#define EPD_W21_CLK_0	nrf_gpio_pin_write(OTQV2_GPIO_EPD_CLK, 0)
#define EPD_W21_CLK_1	nrf_gpio_pin_write(OTQV2_GPIO_EPD_CLK, 1)

#define EPD_W21_CS_0	nrf_gpio_pin_write(OTQV2_GPIO_EPD_CS, 0)
#define EPD_W21_CS_1	nrf_gpio_pin_write(OTQV2_GPIO_EPD_CS, 1)

#define EPD_W21_DC_0	nrf_gpio_pin_write(OTQV2_GPIO_EPD_DC, 0)
#define EPD_W21_DC_1	nrf_gpio_pin_write(OTQV2_GPIO_EPD_DC, 1)

#define EPD_W21_RST_0	nrf_gpio_pin_write(OTQV2_GPIO_EPD_RST, 0)
#define EPD_W21_RST_1	nrf_gpio_pin_write(OTQV2_GPIO_EPD_RST, 1)

#define isEPD_W21_BUSY nrf_gpio_pin_read(OTQV2_GPIO_EPD_BUSY)
#define isEPD_W21_Read nrf_gpio_pin_read(OTQV2_GPIO_EPD_BUSY) // for solomen solutions

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
//*************************For GPM1190A0  USE************

void pc1(unsigned char X,unsigned char Y,unsigned char No,unsigned char *p);
void LCD_Clear(void);
void Display_image(uint8 X,uint8 Y,uint16 X_seg,uint8 *buff);
void All_Color(uint8 rr,uint8 gg,uint8 bb);
void All_Color_16(uint16 color);
void W_DATA(uint16 sdata);
void W_COM16(uint16 cmd);
void SSD1963INITIALSPHX8363(void);
void LCD_WriteRAM(uint16 RGB_Code);
void  showpic_a(const uint16 *bitmap);
uint16  LCD_ReadRAM(void);
uint16 DATAIN(void);
void Flicker_v(void);
void Flicker_v2(void);
void GPIOSETIINPUT_PB(void);
void GPIO_OUTPUT_PB(void);
void MLCM_Initial(void);
void MLCM_Initial_16(void);
void LCD_CtrlWrite_ILI9325C(uint16 cmd1, uint16 datax);
void LCD_WriteBMP(uint16 Xpos, uint16 Ypos, uint16 Height, uint16 Width, uint8 *bitmap);
void LCD_WriteArea(uint16 add_sx, uint16 add_ex, uint16 add_sy, uint16 add_ey, uint8 corr,uint8 corg,uint8 corb);
void W_COM(uint8 cmd);
void W_DATA8(uint8 sdata);
void W_DATA81(uint8 sdata);
/*----- Medium layer function -----*/
void LCD_WriteReg(uint8 LCD_Reg, uint16 LCD_RegValue);
uint16  LCD_ReadReg(uint8 LCD_Reg);

void display_auto(void);
void display_manual(void);
void vvvadaa(void);

void cmcc(void);
//void time(void);
void lrc(void);
void temp(void);
void all_screen(void);
void photo3(void);
void photo3(void);
void photo3(void);

void clear_screen(void);
void Clear_Screen(void);

void OLED_ConsoleWrite(unsigned char *str,unsigned int Count);
void Write_816_characters(unsigned char No);

void LCD_writeBMP(const uint8 *p);


void LCD_PowerOn(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
//void LCD_SetCursor(uint16 Xpos, uint16 Ypos);
//void LCD_SetPoint(uint16 x,uint16 y,uint16 point);
/*----- Low layer function -----*/
void LCD_CtrlLinesConfig(void);
//void LCD_FSMCConfig(void);
void SSD1963Data_8(uint8 sdata);
void SSD1963Command_8(uint8 cmd);

//---------------以下五个函数非常重要，务必保证正确存在----------------------

void LCD_SetDisplayWindow(uint16 add_sx, uint16 add_ex, uint16 add_sy, uint16 add_ey);
void LCD_DisplayStringLine(uint16 addx,uint16 addy, unsigned char *p );
void LCD_PutChar(uint16 adx,uint16 ady,const uint16  *p);
void LCD_WriteRAM_Prepare(void);   
void LCD_DrawPoint(uint32 point);

/* LCD Registers */
#define R0             0x00
#define R1             0x01
#define R2             0x02
#define R3             0x03
#define R4             0x04
#define R5             0x05
#define R6             0x06
#define R7             0x07
#define R8             0x08
#define R9             0x09
#define R10            0x0A
#define R12            0x0C
#define R13            0x0D
#define R14            0x0E
#define R15            0x0F
#define R16            0x10
#define R17            0x11
#define R18            0x12
#define R19            0x13
#define R20            0x14
#define R21            0x15
#define R22            0x16
#define R23            0x17
#define R24            0x18
#define R25            0x19
#define R26            0x1A
#define R27            0x1B
#define R28            0x1C
#define R29            0x1D
#define R30            0x1E
#define R31            0x1F
#define R32            0x20
#define R33            0x21
#define R34            0x22
#define R36            0x24
#define R37            0x25
#define R40            0x28
#define R41            0x29
#define R43            0x2B
#define R45            0x2D
#define R48            0x30
#define R49            0x31
#define R50            0x32
#define R51            0x33
#define R52            0x34
#define R53            0x35
#define R54            0x36
#define R55            0x37
#define R56            0x38
#define R57            0x39
#define R59            0x3B
#define R60            0x3C
#define R61            0x3D
#define R62            0x3E
#define R63            0x3F
#define R64            0x40
#define R65            0x41
#define R66            0x42
#define R67            0x43
#define R68            0x44
#define R69            0x45
#define R70            0x46
#define R71            0x47
#define R72            0x48
#define R73            0x49
#define R74            0x4A
#define R75            0x4B
#define R76            0x4C
#define R77            0x4D
#define R78            0x4E
#define R79            0x4F
#define R80            0x50
#define R81            0x51
#define R82            0x52
#define R83            0x53
#define R96            0x60
#define R97            0x61
#define R106           0x6A
#define R118           0x76
#define R128           0x80
#define R129           0x81
#define R130           0x82
#define R131           0x83
#define R132           0x84
#define R133           0x85
#define R134           0x86
#define R135           0x87
#define R136           0x88
#define R137           0x89
#define R139           0x8B
#define R140           0x8C
#define R141           0x8D
#define R143           0x8F
#define R144           0x90
#define R145           0x91
#define R146           0x92
#define R147           0x93
#define R148           0x94
#define R149           0x95
#define R150           0x96
#define R151           0x97
#define R152           0x98
#define R153           0x99
#define R154           0x9A
#define R157           0x9D
#define R192           0xC0
#define R193           0xC1
#define R229           0xE5


/* LCD color */
#define   Black         0x0000      		/* 黑色：    0,   0,   0 */
#define   BLUE          0x001F                	/* 蓝色：    0,   0, 255 */
#define   GREEN         0x07E0                	/* 绿色：    0, 255,   0 */
#define   CYAN          0x07FF                 	/* 青色：    0, 255, 255 */
#define   RED           0xF800                	/* 红色：  255,   0,   0 */
#define   MAGENTA       0xF81F                	/* 品红：  255,   0, 255 */
#define   YELLOW        0xFFE0                	/* 黄色：  255, 255, 0   */
#define   White         0xFFFF      		/* 白色：  255, 255, 255 */
#define   NAVY          0x000F      		/* 深蓝色：  0,   0, 128 */
#define   DGREEN        0x03E0               	/* 深绿色：  0, 128,   0 */
#define   DCYAN         0x03EF                	/* 深青色：  0, 128, 128 */
#define   MAROON        0x7800                	/* 深红色：128,   0,   0 */
#define   PURPLE        0x780F                	/* 紫色：  128,   0, 128 */
#define   OLIVE         0x7BE0             	/* 橄榄绿：128, 128,   0 */
#define   LGRAY         0xC618            	/* 灰白色：192, 192, 192 */
#define   DGRAY         0x7BEF                	/* 深灰色：128, 128, 128 */

#define Line0          0
#define Line1          24
#define Line2          48
#define Line3          72
#define Line4          96
#define Line5          120
#define Line6          144
#define Line7          168
#define Line8          192
#define Line9          216

#endif /* __LCD_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
