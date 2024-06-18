/******************** (C) COPYRIGHT 2010 GIANTPLUS ********************
* File Name          : main.c
* Author             : Jason
* Version            : V1.0.0
* Date               : 2011.01.20
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include <stdio.h>
#include "FAT.h"
#include "lcd.h"


const  u8 tab_1[]={
				0xf0,0x4e,0x80,0x1f,0xa0,0xfb,0x01,0xef,0x7f,0xf8,0xff,0xff,0xe3,0xff,0x00,0x07
					};	
const  u8 tab_2[]={
				0xf0,0x4e,0x80,0x1f,0xa0,0xfb,0x01,0xef,0x7f,0x0c,0x00,0x00,0x7f,0xf8,0x1c,0x07	
					};	
const  u8 tab_3[]={
				0xf0,0x4e,0x80,0x1f,0xa0,0xfb,0x01,0xef,0x7f,0xfe,0xff,0xff,0xe3,0xff,0xfc,0x07	
					};
const  u8 tab_4[]={
				0xf0,0x4e,0x80,0x1f,0xa0,0xfb,0x01,0xef,0xff,0x0f,0x00,0x00,0x7f,0xf8,0xff,0x07		 
					};
const  u8 tab_5[]={
				0xf0,0x1f,0x80,0x1f,0xe0,0xf6,0x61,0xff,0x7f,0xf8,0xff,0xff,0xe3,0xff,0x00,0x07		 
					};
const  u8 tab_6[]={
				0xf0,0x1f,0x80,0x1f,0xe0,0xf6,0x61,0xff,0x7f,0x0c,0x00,0x00,0x7f,0xf8,0x1c,0x07
					};
const  u8 tab_7[]={
				0xf0,0x1f,0x80,0x1f,0xe0,0xf6,0x61,0xff,0x7f,0xfe,0xff,0xff,0xe3,0xff,0xfc,0x07
					};
const  u8 tab_8[]={
				0xf0,0x1f,0x80,0x1f,0xe0,0xf6,0x61,0xff,0xff,0x0f,0x00,0x00,0x7f,0xf8,0xff,0x07
					};

unsigned  key_value = 0;
GPIO_InitTypeDef GPIO_InitStructure;
ErrorStatus HSEStartUpStatus;
static vu32 TimingDelay = 0;
extern BYTE FAT32_Enable; 
extern WORD SectorsPerClust;
u16 image_count = 0;
u16 image_all = 0;
extern UINT32 File_Size;
SD_Error Status = SD_OK;
SD_CardInfo SDCardInfo;
u32  sd_Capacity;
SD_Error SD_InitAndConfig(void);
void RCC_Configuration(void);
void InterruptConfig(void);
void NVIC_Configuration(void);
void Delay(vu32 nCount);
void Decrement_TimingDelay(void);
void SysTick_Config(void);
void USART_Configuration(void);
void USART1GPIO_Configuration(void); 
//void Disp_BMP(void);
void Temp_show(void);
void times_show(void);
void bit_test(void);

UINT8 image_all_file(void);
UINT32 Open_imageFile(void);
UINT32 Read_BMP24BIT_File(UINT32 clu,UINT32 file_size,UINT8 COLOR,UINT16 X_pixel,UINT16 Y_pixel);
//UINT8 FAT_Init_new(void);
extern UINT8 zf_font[];
unsigned char Color_Options;

//extern FAT_Init_Args *pInit_Args;
extern UINT8 FAT_Buffer[514];
extern UINT32 File_First_clu[10000];

//----------------�������ݲ���Ҫ�޸ģ���������readme.txt---------------

#include "PICC1.h"   //��оƬ�ڲ�Flash���Ϊ512KB���ɱ���С��ͼƬ����


#define KEYC6      GPIOC->IDR&GPIO_Pin_6
#define KEYC7      GPIOA->IDR&GPIO_Pin_7
#define KEYC8      GPIOA->IDR&GPIO_Pin_8
void DelayKEY (u32 k); //��ס��������ͣ
void Dly_1ms (u32 k);	
void KEYGPIO_Init(void);///*[��PA78���ó�����ģʽ] */
u16 GPIOB_DATA;


//----------------------------------------------------------------------------==
//						    main
/*******************************************************************************/
int main(void)
{ 
//	UINT16 i;
	volatile u16 n=1,key_value;
	#ifdef DEBUG
	debug();
	#endif
/* System Clocks Configuration */
	RCC_Configuration();	 
/* NVIC Configuration */
	NVIC_Configuration();   //�жϹ�����ʼ��   
	SysTick_Config();		    //��ʱ��	  

//-------------------------------------------------	 
	/* Initialize the LCD */
	STM32_Init();
	
	//W_COM(0xb0);W_COM(0x10);W_COM(0x00);W_DATA8(0Xff);
	//clear_screen();
	KEYGPIO_Init();
	clear_screen();	 

	
	
	if(!SD_Init())
	{	 
		if(!FAT_Init())
		{
			clear_screen();
			image_all_file();
			Open_imageFile();
			
		}
	}
	
	
	
	
	
	else
	{	
		while(1)
		{
			
		photo1();
	  DelayKEY(150);
		photo2();
	  DelayKEY(150);
		photo3();
	  DelayKEY(150);
//		cmcc(void);
//		time(void);
//	  lrc(void);
//		temp(void);		
		}
	}
	
	
	
	
 	if(full_color == 1)
	{
	W_COM(0x05);	//Set Graphic RAM Writing Direction
	W_DATA8(0x01);	//RGB	0x08 BGR

	W_COM(0x06);	//Set Row Scan Direction
	W_DATA8(0x00);

		Color_Options = full_color;
	}	  
	if(mono_color == 2)
	{
		W_COM(0xa1);    // Segment remap
		Color_Options = mono_color;
	}
	
	
	
	if(gray_scale == 3)
	{
		W_COM(0xA1);	//seg remap
		Color_Options = gray_scale;
	}
//	
	
	
	while(1)
	{	 
		
		n=1,key_value=0;
	  n=KEYC6;	Delay(2);
		
//	 W_COM(0xd3);	 //set offset
//   W_COM(0x08);
//	 W_COM(0xc8);
		
		
		if(n==0)
		{
	    	while(n==0)			
			{
				n=KEYC6;Delay(1);				
			}
			key_value++; 				
		}
		if((key_value%2)==1) 
        {				 	
			display_manual();
		}
		else if((key_value%2)==0)
		{  	
			display_auto();
		}	 		
	}     
}


 void display_auto()
 {
//----------------���½���SD����ʾ------------------------------------------ 

//  
//	if (FAT_Init())	//��ʼ��FAT�ļ�ϵͳ
//	{	
//	    Delay(60);
//	}	 	   
		if(image_count<image_all)
		{
			image_count++;
		} 
  	else 
		{ 
			image_count = 1 ;
		} 
		Read_BMP24BIT_File(File_First_clu[image_count],File_Size,Color_Options,XDP,YDP);

	  	//DelayKEY(50); 
		DelayKEY(150); 
}		


void display_manual()
{
	volatile u16 m=1,n,l=1;
	//----------------���½���SD����ʾ------------------------------------------ 
	//Status =SD_InitAndConfig();  /*����SD�ӿ�,��ʼ��SD��*/

LCDTest:	

		m=KEYC7; 
		if(m==0)
		{
			while(m==0)
			{
				m=KEYC7;Delay(1);
			}	
		   	if(image_count<image_all)
			{	
				image_count++;
			}
	  		else 
			{ 
				image_count = 1 ;
			}
			Read_BMP24BIT_File(File_First_clu[image_count],File_Size,Color_Options,XDP,YDP);
		}
		n=KEYC6;
		if(n==0)
		{
        	while(n==0)
			{
				n=KEYC6;Delay(1);				
			}
			key_value++;
			l++;	
		} 
		//DelayKEY(10);
		if((l%2)==1) 		
		   goto LCDTest;
}
	

u16 stopIndex;
void KEYGPIO_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStructure; 
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE); 
  
  /*[��KEYGPIO KEY7/8���ó�����ģʽ] */
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;    // 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 
  GPIO_Init(GPIOC, &GPIO_InitStructure); 

  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;    // 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

//	GPIOD->CRH &= 0XFFFFF0FF;
//	GPIOD->CRH |= 0X00000300;
//	GPIOD->CRH |= 1<<2; 
  
  stopIndex = 0; 
}

void DelayKEY (u32 k)
{ 
    volatile u16 m=1;
	volatile u32 j;
  
 key_1:
      for (j=0; j<k; j++)
         { 
		  	m=KEYC6;  Delay(2);
		 	if(m==0)
			{
            while(m==0)
		
			{
			m=KEYC6;Delay(1);				
			}
			key_value++; 		
			}

			if((key_value%2)==1)
			{
				m=KEYC7; Delay(1);
			 	if(m==0)
				{
	            while(m==0)
			
				{
				m=KEYC7;Delay(1);				
				}
				break; 		
				}
			}
			  
			if((key_value%2)==1) 			
         	goto key_1;
			 //if((key_value%2)==0)
        	 //break;

           }	
}  


/*******************************************************************************
* Function Name  : 			STM32_CONFIG


*******************************warning********************************************
			
			
			
			Please do not edit or delete the text below, thanks
							2011/12/26
							  
							
*******************************************************************************/



//=================================���³��������Ҫ�޸�==================================
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 	
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  /* Enable USART1 and GPIOA clock */
 // RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
//------------zp2000--------------------------------
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//�ú������������

  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

}

/*******************************************************************************
* Function Name  : InterruptConfig
* Description    : Configures the used IRQ Channels and sets their priority.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InterruptConfig(void)
{ 
  /* Deinitializes the NVIC */
  NVIC_DeInit();

  NVIC_Configuration();

  /* Configure the Priority Group to 2 bits */
  //  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		

  /* Configure the SysTick handler priority */
  //Ϊ��ʹ��SDIO�жϣ��������ж����ȼ����Ҹĵ���
  NVIC_SystemHandlerPriorityConfig(SystemHandler_SysTick, 1, 1);
}

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length (time base 10 ms).
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(u32 nCount)
{
  HAL_Delay(nCount);
}

/*******************************************************************************
* Function Name  : Decrement_TimingDelay
* Description    : Decrements the TimingDelay variable.
* Input          : None
* Output         : TimingDelay
* Return         : None
*******************************************************************************/
void Decrement_TimingDelay(void)

{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

/*******************************************************************************
* Function Name  : SysTick_Config
* Description    : Configure a SysTick Base time to 10 ms.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Config(void)
{
  /* Configure HCLK clock as SysTick clock source */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
 
  /* SysTick interrupt each 100 Hz with HCLK equal to 72MHz */
  SysTick_SetReload(720000);

  /* Enable the SysTick Interrupt */
  SysTick_ITConfig(DISABLE);//ENABLE DISABLE

  /* Enable the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Enable);
}
//----�趨�����ӿ�--------------


void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef  USART_ClockInitStructure;

/* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
/* Configure the USART1 synchronous paramters */
USART_ClockInit(USART1, &USART_ClockInitStructure);

USART_InitStructure.USART_BaudRate = 115200;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;
USART_InitStructure.USART_StopBits = USART_StopBits_1;
USART_InitStructure.USART_Parity = USART_Parity_No ;
USART_InitStructure.USART_HardwareFlowControl = 
USART_HardwareFlowControl_None;

USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
/* Configure USART1 basic and asynchronous paramters */
USART_Init(USART1, &USART_InitStructure);
    
  /* Enable USART1 */
  USART_Cmd(USART1, ENABLE);
  
}

/*******************************************************************************
* Function Name  : USART1GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
    
  /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/* Private functions ---------------------------------------------------------*/
SD_Error SD_InitAndConfig(void)
{
  Status = SD_Init();

  if (Status == SD_OK)
  {
    /*----------------- Read CSD/CID MSD registers ------------------*/
    Status = SD_GetCardInfo(&SDCardInfo);
  }
  
  if (Status == SD_OK)
  {
    /*----------------- Select Card --------------------------------*/
    Status = SD_SelectDeselect((u32) (SDCardInfo.RCA << 16));
  }
  
  if (Status == SD_OK)
  {
    /*----------------- Set BusWidth ------------------------------*/
    Status = SD_EnableWideBusOperation(SDIO_BusWide_4b);
  }
  
  /* Set Device Transfer Mode to INTERRUPT to DMA */
  if (Status == SD_OK)
  {  
    //Status = SD_SetDeviceMode(SD_DMA_MODE);//SD_DMA_MODE,SD_INTERRUPT_MODE
  }
  return Status;
}
//****************************************
//***********************temp********************


#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/

void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
