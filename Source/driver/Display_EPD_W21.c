#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "TriColorLED.h"
#include "string.h"
#include "OTQV2_WDT.h"

#define EPD_InitWaitTime 100
#define EPD_UpdateWaitTime 60000

/////////////////////////////////////partial screen update LUT///////////////////////////////////////////

#if 0
const unsigned char lut_vcom1_quick[] ={
0x66	,0x1F	,0x1F	,0x01	,0x01	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00
	,0x00	,0x00,					};

const unsigned char lut_ww1_quick[] ={
0xAA	,0x1E	,0x01	,0x00	,0x00	,0x03,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,};

const unsigned char lut_bw1_quick[] ={
0xAA	,0x1E	,0x01	,0x00	,0x00	,0x03,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,};

const unsigned char lut_wb1_quick[] ={
0x00	,0x1E	,0x01	,0x00	,0x00	,0x03,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,};

const unsigned char lut_bb1_quick[] ={
0x00	,0x1E	,0x01	,0x00	,0x00	,0x03,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,};

#else

unsigned char lut_vcom1_quick[] ={
0x00	,12	,0x01	,0x00	,0x00	,0x01,
0x00	,24	,0x01	,0x00	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00
	,0x00	,0x00,					};
unsigned char lut_ww1_quick[] ={
0x40	,12	,0x01	,0x00	,0x00	,0x01,
0x80	,24	,0x01	,0x00	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,};
unsigned char lut_bw1_quick[] ={
0x40	,12	,0x01	,0x00	,0x00	,0x01,
0x80	,24	,0x01	,0x00	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	};
unsigned char lut_wb1_quick[] ={
0x80	,12	,0x01	,0x00	,0x00	,0x01,
0x40	,24	,0x01	,0x00	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	};
unsigned char lut_bb1_quick[] ={
0x80	,12	,0x01	,0x00	,0x00	,0x01,
0x40	,24	,0x01	,0x00	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	};

#endif



void delay_xms(unsigned int xms)
{
	HAL_Delay(xms);
	/*unsigned int i;
	while(xms--)
	{
		i=12000;
		while(i--);
	}*/
}

void EPD_SetLUTList(LUTList *LUT,uint8 type,uint8 Timing1, uint8 Timing2, uint8 Timing3, uint8 Timing4 , uint8 Cycle)
{
LUT->type = type;
LUT->timing1 = Timing1;
LUT->timing2 = Timing2;
LUT->timing3 = Timing3;
LUT->timing4 = Timing4;
LUT->cycle = Cycle;
}

void EPD_SetRefreshLUT(LUTTiming *SetLUT)
{
	memcpy(lut_vcom1_quick,SetLUT->VCOM,sizeof(LUTList)*5);
	memcpy(lut_ww1_quick,SetLUT->WW,sizeof(LUTList)*5);
	memcpy(lut_bw1_quick,SetLUT->BW,sizeof(LUTList)*5);
	memcpy(lut_wb1_quick,SetLUT->WB,sizeof(LUTList)*5);
	memcpy(lut_bb1_quick,SetLUT->BB,sizeof(LUTList)*5);
}


void EPD_W21_Init(void)
{


        EPD_W21_RST_0;  // Module reset   
	delay_xms(10);//At least 10ms delay 
	EPD_W21_RST_1;
	delay_xms(10); //At least 10ms delay 

	lcd_chkstatus(EPD_InitWaitTime);   
	EPD_W21_WriteCMD(0x12);  //SWRESET
	lcd_chkstatus(EPD_InitWaitTime);   	
}

void EPD_QRDisplay(unsigned char *Image)
{
    unsigned int Width, Height,i,j;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

    EPD_W21_WriteCMD(0x10);
    for (j = 0; j < Height; j++) {
        for ( i = 0; i < Width; i++) {
            EPD_W21_WriteDATA(0x00);
        }
    }

    EPD_W21_WriteCMD(0x13);
    for ( j = 0; j < Height/2; j++)
    	{
    	for(i=0;i<23;i++)
    		{
           	EPD_W21_WriteDATA(~Image[i+j*23]);
    		}
    	for(i=0;i<2;i++)
    		{
           	EPD_W21_WriteDATA(0xff);
    		}

    	for(i=0;i<23;i++)
    		{
           	EPD_W21_WriteDATA(~Image[i+j*23]);
    		}
    	for(i=0;i<2;i++)
    		{
           	EPD_W21_WriteDATA(0xff);
    		}
    }
		EPD_W21_WriteCMD(0x12);			//DISPLAY REFRESH
		delay_xms(1);	    //!!!The delay here is necessary, 200uS at least!!!
		lcd_chkstatus(60000);
}

uint8 tmp;


void EPD_Update(stMain *pMain)
{   
  //EPD_W21_WriteCMD(0x22); //Display Update Control
  //EPD_W21_WriteDATA(0xF7);   
  //EPD_W21_WriteCMD(0x20); //Activate Display Update Sequence
  EPD_CallUpdate(pMain);
	EPD_sleep(pMain);

}

uint8 EPD_Update_Fast(stMain *pMain)
{   
  //EPD_W21_WriteCMD(0x22); //Display Update Control
  //EPD_W21_WriteDATA(0xC7);   
  //EPD_W21_WriteCMD(0x20); //Activate Display Update Sequence
	delay_xms(200);	    		//!!!The delay here is necessary, 200uS at least!!!
	EPD_CallUpdate(pMain);
	EPD_sleep(pMain);
}

uint8 EntryTemp=0;

void EPD_ScreenSide(ScreenSide Side)
{
uint8 XSA=0, XEA=0, Xadd=0;
uint16 YSA=0, YEA=0, Yadd=0;
uint8 RAMSide=0;


switch(Side)
  {
    case ScreenSide_Portrait_Up:
    XSA=0x00;
    XEA=0x15;
    Xadd=0x00;
    YSA=0x0000;
    YEA=0x0108;
    Yadd=0x0000;
    RAMSide=0x03;
    break;

    case ScreenSide_Portrait_Down:
    XSA=0x15;
    XEA=0x00;
    Xadd=0x15;
    YSA=0x0108;
    YEA=0x0000;
    Yadd=0x0107;
    RAMSide=0x00;
    break;

    case ScreenSide_Landscape_Up:
    XSA=0x15;
    XEA=0x00;
    Xadd=0x15;
    YSA=0x0000;
    YEA=0x0108;
    Yadd=0x0000;
    RAMSide=0x07;
    break;

    case ScreenSide_Landscape_Down:
    XSA=0x00;
    XEA=0x15;
    Xadd=0x00;
    YSA=0x0000;
    YEA=0x0108;
    Yadd=0x0000;
    RAMSide=0x03;
    break;

    default:
    break;
  }
  EPD_W21_WriteCMD(0x44);  //set RAM X
  EPD_W21_WriteDATA(XSA);  // XSA > 0x00
  EPD_W21_WriteDATA(XEA);  // XEA > 0x16

  EPD_W21_WriteCMD(0x45);  //set RAM X
  EPD_W21_WriteDATA(YSA & 0xff);  // YSA > 0x00
  EPD_W21_WriteDATA(YSA>>8 & 0xff);  // YSA > 0x00
  EPD_W21_WriteDATA(YEA & 0xff);  // YEA > 0x108
  EPD_W21_WriteDATA(YEA>>8 & 0xff);  // YEA > 0x108

  EPD_W21_WriteCMD(0x4E);  //set RAM X address
  EPD_W21_WriteDATA(Xadd);  // address > 0x15

  EPD_W21_WriteCMD(0x4F);  //set RAM Y address
  EPD_W21_WriteDATA(Yadd & 0xff);  // 
  EPD_W21_WriteDATA(Yadd>>8 & 0xff);  // address > 0x108

  EPD_W21_WriteCMD(0x11);  //data entry mode setting
  EPD_W21_WriteDATA(RAMSide);
}

void EPD_WhiteScreen_ALL(stMain *pMain,const unsigned char *datas)
{
   unsigned int i;	
   EPD_GPIO_Init();
   
	EPD_W21_RST_0;            // Module reset   
	delay_xms(10);            //At least 10ms delay 
	EPD_W21_RST_1;
	delay_xms(10);            //At least 10ms delay 
  
	EPD_W21_WriteCMD(0x12);   //SWRESET
	lcd_chkstatus(EPD_InitWaitTime);          // busy check


 	
  EPD_W21_WriteCMD(0x18);         //Read built-in temperature sensor
	EPD_W21_WriteDATA(0x80);	
	  	
	EPD_W21_WriteCMD(0x22);   // Load temperature value
	EPD_W21_WriteDATA(0xB1);		
  EPD_W21_WriteCMD(0x20);	
  lcd_chkstatus(EPD_InitWaitTime);   

	EPD_W21_WriteCMD(0x1A);   // Write to temperature register
	EPD_W21_WriteDATA(0x64);		
  EPD_W21_WriteDATA(0x00);	
				  	
	EPD_W21_WriteCMD(0x22);   // Load temperature value
	EPD_W21_WriteDATA(0x91);		
  EPD_W21_WriteCMD(0x20);	
	lcd_chkstatus(EPD_InitWaitTime);    

	EPD_W21_WriteCMD(0x01);  //driver output control
	EPD_W21_WriteDATA(0x07);  // A : 108
        EPD_W21_WriteDATA(0x01);  // A : 108
        EPD_W21_WriteDATA(0x00);  // B : GD > 0 , SM > 0 , TB > 0


        EPD_W21_WriteCMD(0x21);   // display update control 1
	EPD_W21_WriteDATA(0x00);  // Red RAM normal. BW ram inverse
        EPD_W21_WriteDATA(0x00);

        EPD_W21_WriteCMD(0x0F);  //gate scan start position
	EPD_W21_WriteDATA(0x00);  // 00
        EPD_W21_WriteDATA(0x00);  // 00

//EPD_ScreenSide(ScreenSide_Portrait_Down);
EPD_ScreenSide(ScreenSide_Portrait_Up);

  EPD_W21_WriteCMD(0x20);	
	lcd_chkstatus(EPD_InitWaitTime);    

  EPD_W21_WriteCMD(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<5808;i++)
   {               
     EPD_W21_WriteDATA(~*datas);
			datas++;
   }
		EPD_Update(pMain);	 
   //EPD_Update_Fast(pMain);	 
}

uint8 EPD_CallUpdate(stMain *pMain)
{
uint8 ret = 0;
if(pMain->Sensor.PCBTemp > 2000)			// 5secs wait time for over 20'C degree
	{
  EPD_W21_WriteCMD(0x22); //Display Update Control
  EPD_W21_WriteDATA(0xC7);   
  EPD_W21_WriteCMD(0x20); //Activate Display Update Sequence
	delay_xms(200);	    		//!!!The delay here is necessary, 200uS at least!!!
		if(lcd_chkstatus(5000)==0xff)
			{
			TriColorLED_Blink(LEDColor_Red,4,100);
			}	
	}
	else if(pMain->Sensor.PCBTemp > 1000)			// 10secs wait time for over 10'C degree
	{
  EPD_W21_WriteCMD(0x22); //Display Update Control
  EPD_W21_WriteDATA(0xF7);   
  EPD_W21_WriteCMD(0x20); //Activate Display Update Sequence
	delay_xms(200);	    		//!!!The delay here is necessary, 200uS at least!!!
		if(lcd_chkstatus(10000)==0xff)
			{
			TriColorLED_Blink(LEDColor_Red,4,100);
			}	
	}
	else if(pMain->Sensor.PCBTemp > 0)		// 20secs wait time for over 0'C degree
	{
  EPD_W21_WriteCMD(0x22); //Display Update Control
  EPD_W21_WriteDATA(0xF7);   
  EPD_W21_WriteCMD(0x20); //Activate Display Update Sequence
	delay_xms(200);	    		//!!!The delay here is necessary, 200uS at least!!!
		if(lcd_chkstatus(20000)==0xff)
			{
			TriColorLED_Blink(LEDColor_Red,4,100);
			}
	}
	/*						// disabled. recording skip issue
	else if(pMain->Sensor.PCBTemp > -1000)	// 60secs wait time for over 10'C degree
	{
  EPD_W21_WriteCMD(0x22); //Display Update Control
  EPD_W21_WriteDATA(0xC7);   
  EPD_W21_WriteCMD(0x20); //Activate Display Update Sequence
	delay_xms(200);	    		//!!!The delay here is necessary, 200uS at least!!!
		if(lcd_chkstatus(30000) == 0xff)
			{
			TriColorLED_Blink(LEDColor_Red,4,100);
			}
	}
	*/
	else
	{
		//EPD_W21_WriteCMD(0x12);		//�하 6�하�선 EPD 갱신 금�
		//delay_xms(200);	    		//!!!The delay here is necessary, 200uS at least!!!
		TriColorLED_Blink(LEDColor_Yellow,4,100);
	}
}
 
unsigned char EPD_Display(stMain *pMain,unsigned char *Image, char PartialMode,char WaitEnable)
{
    unsigned int Width, Height,i,j;
    uint8_t ret=0;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

    for(int i=0;i<10;i++)
    {
        EPD_GPIO_Init();
        EPD_WakeUp(PartialMode);

    	EPD_W21_WriteCMD(0x70);
    	tmp=EPD_W21_ReadDATA();
    	if(tmp)
    		{
    		break;
    		}

    	if (i==9)return 0xff;
        break;
    }

    EPD_W21_WriteCMD(0x91);
	EPD_W21_WriteCMD(0x90);		//resolution setting
	EPD_W21_WriteDATA (0);   //x-start
	EPD_W21_WriteDATA ((152-1) | 0x07);	 //x-end

	EPD_W21_WriteDATA (0/256);
	EPD_W21_WriteDATA (0%256);   //y-start

	EPD_W21_WriteDATA (152/256);
	EPD_W21_WriteDATA (152%256-1);  //y-end
	EPD_W21_WriteDATA (0x00);

    EPD_W21_WriteCMD(0x10);
    for (j = 0; j < Height; j++) {
        for ( i = 0; i < Width; i++) {
            EPD_W21_WriteDATA(0x00);
        }
    }

    EPD_W21_WriteCMD(0x13);
    for ( j = 0; j < Height; j++) {
        for ( i = 0; i < Width; i++) {
           EPD_W21_WriteDATA(~Image[i + j * Width]);
        	//EPD_W21_WriteDATA(0x00);
        }
    }

/*
		RCC->CFGR &= ~(0x07);
		RCC->CFGR |= 0x03;
		RCC->CR &= ~(1<<24);		// MCU clock reduce
*/
		EPD_CallUpdate(pMain);
		EPD_sleep(pMain);				// MOVE to MCU sleep code
		return ret;

}
//UC8151C
void EPD_init(void)
{
	//EPD_W21_Init(); //Electronic paper IC reset

	EPD_W21_WriteCMD(0x04);
	lcd_chkstatus(100);//waiting for the electronic paper IC to release the idle signal

	EPD_W21_WriteCMD(0x00);			//panel setting
	EPD_W21_WriteDATA(0x17);		//LUT from OTP��KW-BF   KWR-AF	BWROTP 0f	BWOTP 1f

	EPD_W21_WriteCMD(0X50);			//VCOM AND DATA INTERVAL SETTING
	EPD_W21_WriteDATA(0x97);		//WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
}
void lut1(void)
{
	unsigned int count;
	EPD_W21_WriteCMD(0x20);
	for(count=0;count<44;count++)	     
		{EPD_W21_WriteDATA(lut_vcom1_quick[count]);}

	EPD_W21_WriteCMD(0x21);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_ww1_quick[count]);}
	
	EPD_W21_WriteCMD(0x22);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_bw1_quick[count]);}

	EPD_W21_WriteCMD(0x23);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_wb1_quick[count]);}

	EPD_W21_WriteCMD(0x24);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_bb1_quick[count]);}
}
void EPD_WakeUp(char PartialMode)
{
	  EPD_W21_Init();
          /*
		EPD_W21_WriteCMD(0x01);			//POWER SETTING 
		EPD_W21_WriteDATA (0x03);	          
		EPD_W21_WriteDATA (0x20);
		EPD_W21_WriteDATA (0x26);
		EPD_W21_WriteDATA (0x26);
		EPD_W21_WriteDATA (0x03);

		EPD_W21_WriteCMD(0x06);         //boost soft start
		EPD_W21_WriteDATA (0x03);		//A
		EPD_W21_WriteDATA (0x00);		//B
		EPD_W21_WriteDATA (0x26);		//C       

		EPD_W21_WriteCMD(0x04);  
		lcd_chkstatus();

		EPD_W21_WriteCMD(0x00);			//panel setting
		EPD_W21_WriteDATA(0xbf);		//LUT from OTP£¬128x296
                
			//EPD_W21_WriteCMD(0x00);			//panel setting
	//EPD_W21_WriteDATA(0x1f);		//LUT from OTP��KW-BF   KWR-AF	BWROTP 0f	BWOTP 1f
	//EPD_W21_WriteDATA(0x13);		//LUT from OTP��KW-BF   KWR-AF	BWROTP 0f	BWOTP 1f, invert display
		EPD_W21_WriteCMD(0x30);			
        EPD_W21_WriteDATA (0x3C);      	// 3A 100HZ   29 150Hz 39 200HZ	31 171HZ


		EPD_W21_WriteCMD(0x61);			//resolution setting
		EPD_W21_WriteDATA (0x98);   //152     	 
		EPD_W21_WriteDATA (0x00);		//152
		EPD_W21_WriteDATA (0x98);
		
	  EPD_W21_WriteCMD(0x82);			//vcom_DC setting  	
    EPD_W21_WriteDATA (0x12);	

	EPD_W21_WriteCMD(0x00);			//panel setting

	if(PartialMode)
	{
#ifdef Display_ButtonUpSide
		EPD_W21_WriteDATA(0x9f);		//LUT from OTP��KW-BF   KWR-AF	BWROTP 0f	BWOTP 1f, invert display
#else
		EPD_W21_WriteDATA(0x13);		//LUT from OTP��KW-BF   KWR-AF	BWROTP 0f	BWOTP 1f
#endif
	}
	else
	{
#ifdef Display_ButtonUpSide
		EPD_W21_WriteDATA(0xbf);		//LUT from OTP��KW-BF   KWR-AF	BWROTP 0f	BWOTP 1f, invert display

#else
		EPD_W21_WriteDATA(0x33);		//LUT from OTP��KW-BF   KWR-AF	BWROTP 0f	BWOTP 1f
#endif
	}

	
		EPD_W21_WriteCMD(0X50);
		EPD_W21_WriteDATA(0x97);		
		lut1();	
                */
}


void EPD_sleep(stMain *pMain)
{
	/*
EPD_W21_WriteCMD(0X50);  //VCOM AND DATA INTERVAL SETTING
EPD_W21_WriteDATA(0xf7); //WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

EPD_W21_WriteCMD(0X02);  	//power off
lcd_chkstatus(100);          //waiting for the electronic paper IC to release the idle signal
EPD_W21_WriteCMD(0X07);  	//deep sleep
EPD_W21_WriteDATA(0xA5);
  	*/
pMain->Status.AlreadySleepFlag = 0x77;
EPD_W21_WriteCMD(0x10); //enter deep sleep
EPD_W21_WriteDATA(0x01); 
delay_xms(100);

}


unsigned char lcd_chkstatus(uint32 TimeOutMs)
{
	unsigned char busy;
	unsigned int WaitLimit=TimeOutMs;
	unsigned long oldSysTick=0;
	unsigned long WaitCount=0;
	oldSysTick = HAL_GetTick();
	wdt_feed();
	//OTQV2_HF_Stop();
	//NRF_LOG_INFO("[%10d] EPD busy wait start. timeout : %d",getTickCount(),TimeOutMs);
	do
	{
		busy = isEPD_W21_BUSY;
		HAL_Delay(1);
		WaitCount++;
		if(!(WaitCount % 1000))		// 1초에 1벌치초기
			{
			wdt_feed();
			}
		if(oldSysTick + WaitLimit < HAL_GetTick())
		{
			NRF_LOG_INFO("[%10d] EPD busy time out!!! timeout :  %d",getTickCount(),TimeOutMs);
			return 0xff;
		}
	}
	while(busy);   
	//NRF_LOG_INFO("[%10d] EPD busy wait end",getTickCount());
	//OTQV2_HF_Start();
	//delay_xms(200);
	return 0;
}


// LUT �리코드 - david


void SetLUT(void)				// �도 갱신 LUT
{
	LUTTiming LUT;
	uint8 FirstTiming = 12 ,SecondTiming = 18;
	memset(&LUT,0x00,sizeof(LUT));
	EPD_SetLUTList(&LUT.VCOM[0],0xC0,FirstTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.VCOM[1],0xC0,SecondTiming,1,0x00,0x00,0x01);

	EPD_SetLUTList(&LUT.WW[0],0x40,FirstTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.WW[1],0x80,SecondTiming,1,0x00,0x00,0x01);

	EPD_SetLUTList(&LUT.BW[0],0x40,FirstTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.BW[1],0x80,SecondTiming,1,0x00,0x00,0x01);

	EPD_SetLUTList(&LUT.BB[0],0x80,FirstTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.BB[1],0x40,SecondTiming,1,0x00,0x00,0x01);

	EPD_SetLUTList(&LUT.WB[0],0x80,FirstTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.WB[1],0x40,SecondTiming,1,0x00,0x00,0x01);

	EPD_SetRefreshLUT(&LUT);
}

void SetLUTSecond(void)
{
	LUTTiming LUT;
	uint8 FirstTiming = 12 ,SecondTiming = 24;

	memset(&LUT,0x00,sizeof(LUT));
	EPD_SetLUTList(&LUT.VCOM[0],0xC0,FirstTiming,12,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.VCOM[1],0xC0,SecondTiming,12,0x00,0x00,0x01);

	EPD_SetLUTList(&LUT.WW[0],0x40,FirstTiming,12,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.WW[1],0x80,SecondTiming,12,0x00,0x00,0x01);

	EPD_SetLUTList(&LUT.BW[0],0x40,FirstTiming,12,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.BW[1],0x80,SecondTiming,12,0x00,0x00,0x01);

	EPD_SetLUTList(&LUT.BB[0],0x80,FirstTiming,12,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.BB[1],0x40,SecondTiming,12,0x00,0x00,0x01);

	EPD_SetLUTList(&LUT.WB[0],0x80,FirstTiming,12,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.WB[1],0x40,SecondTiming,12,0x00,0x00,0x01);

	EPD_SetRefreshLUT(&LUT);
}

void SetLongLUT(void)
{
	LUTTiming LUT;
	uint8 FirstTiming = 12 ,SecondTiming = 18 , ThirdTiming = 24 , ForthTiming = 24, FifthTiming = 24;
	memset(&LUT,0x00,sizeof(LUT));
	EPD_SetLUTList(&LUT.VCOM[0],0xC0,FirstTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.VCOM[1],0xC0,SecondTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.VCOM[2],0xC0,ThirdTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.VCOM[3],0xC0,ForthTiming,12,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.VCOM[4],0xC0,FifthTiming,12,0x00,0x00,0x01);
	//EPD_SetLUTList(&LUT.VCOM[5],0xC0,ForthTiming,12,0x00,0x00,0x01);

	EPD_SetLUTList(&LUT.WW[0],0x40,FirstTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.WW[1],0x80,SecondTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.WW[2],0x40,ThirdTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.WW[3],0x80,ForthTiming,12,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.WW[4],0x80,FifthTiming,12,0x00,0x00,0x01);
	//EPD_SetLUTList(&LUT.WW[5],0x80,ForthTiming,0x01,0x00,0x00,0x01);

	EPD_SetLUTList(&LUT.BW[0],0x40,FirstTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.BW[1],0x80,SecondTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.BW[2],0x40,ThirdTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.BW[3],0x80,ForthTiming,12,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.BW[4],0x80,FifthTiming,12,0x00,0x00,0x01);
	//EPD_SetLUTList(&LUT.BW[5],0x80,ForthTiming,0x01,0x00,0x00,0x01);

	EPD_SetLUTList(&LUT.BB[0],0x40,FirstTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.BB[1],0x80,SecondTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.BB[2],0x40,ThirdTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.BB[3],0x80,ForthTiming,12,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.BB[4],0x40,FifthTiming,12,0x00,0x00,0x01);
	//EPD_SetLUTList(&LUT.BB[5],0x40,ForthTiming,0x01,0x00,0x00,0x01);

	EPD_SetLUTList(&LUT.WB[0],0x40,FirstTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.WB[1],0x80,SecondTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.WB[2],0x40,ThirdTiming,1,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.WB[3],0x80,ForthTiming,12,0x00,0x00,0x01);
	EPD_SetLUTList(&LUT.WB[4],0x40,FifthTiming,12,0x00,0x00,0x01);
	//EPD_SetLUTList(&LUT.WB[5],0x40,ForthTiming,0x01,0x00,0x00,0x01);

	EPD_SetRefreshLUT(&LUT);
}


// LUT �리코드 - david





/***********************************************************
						end file
***********************************************************/

