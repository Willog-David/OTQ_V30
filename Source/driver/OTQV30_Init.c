

#include "OTQV2_Init.h"
#include "WillogService.h"
#include "OTQV2_nrfRTC.h"
#include "TriColorLED.h"
#include "drv_rtc.h"
#include "OTQV2_WDT.h"
#include "OTQV2_Timer.h"
#include "OTQV2_BLEService.h"
#include "KeyButton.h"
#include "EPD_Page.h"
#include "EEPROM.h"

#include "nrfx_systick.h"

extern stMain Main;

// -- BSP define
//#include "bsp.h"

// -- NRF uart define
#include "nrf_uart.h"
#include "ble_gap.h"

//-- DFU define

#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_bootloader_app_start.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"
#include "ble_dfu.h"
#include "nrf_bootloader_info.h"
#include "nrf_bootloader.h"


// -- RTC define

#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"

BLE_OTQV2_DEF(m_OTQV2_service); //OTQV2 Service.
APP_TIMER_DEF(m_update_timer);

static uint32_t data_index = 0;
static uint16_t DataLength = 0;
static bool update_enable = false;
static uint16_t update_interval = 0;

#define DATA_UPDATE_INTERVAL 100

#define COMPARE_COUNTERTIME  (3UL)                                        /**< Get Compare event COMPARE_TIME seconds after the counter starts from 0. */

/** @brief Function starting the internal LFCLK XTAL oscillator.
 */
static void lfclk_config(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    //APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);
}

static void hfclk_config(void)
{
nrf_drv_clock_hfclk_request(NULL);
}



//-- PWM define
#include "nrf_drv_pwm.h"

static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
static nrf_drv_pwm_t m_pwm1 = NRF_DRV_PWM_INSTANCE(1);
static nrf_drv_pwm_t m_pwm2 = NRF_DRV_PWM_INSTANCE(2);

uint16 PWM_Value;
static uint16_t const              m_demo1_top  = 15000;
static uint16_t               m_demo1_step = 100;
static uint8_t                     m_demo1_phase;
static nrf_pwm_values_individual_t m_demo1_seq_values;
static nrf_pwm_sequence_t const    m_demo1_seq =
{
    .values.p_individual = &m_demo1_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_demo1_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

// This is for tracking PWM instances being used, so we can unintialize only
// the relevant ones when switching from one demo to another.
#define USED_PWM(idx) (1UL << idx)
static uint8_t m_used = 0;

static void demo1_handler(nrf_drv_pwm_evt_type_t event_type)
{
    if (event_type == NRF_DRV_PWM_EVT_FINISHED)
    {
        uint8_t channel    = m_demo1_phase >> 1;
        bool    down       = m_demo1_phase & 1;
        bool    next_phase = false;
        bool SubPhase = false;

        uint16_t * p_channels = (uint16_t *)&m_demo1_seq_values;
        static uint16_t value = 0;
        if (down)
        {
            value -= m_demo1_step;
            if (value <= m_demo1_step)
            {
                next_phase = true;
            }
        }
        else
        {
            value += m_demo1_step;
            if (value >= m_demo1_top)
            {
                next_phase = true;
            }
        }
        if(!SubPhase)
        {
        if(0 < value && value < 5000)
        {
          p_channels[0] = value*2;
          p_channels[1] = 0;
          p_channels[2] = 0;
        }
        else if(5000 < value && value < 10000)
        {
          p_channels[0] = 10000;
          p_channels[1] = (value - 5000) *2;
          p_channels[2] = 0;
        }
        else if(10000 < value && value < 15000)
        {
          p_channels[0] = 10000;
          p_channels[1] = 10000;
          p_channels[2] = (value - 10000) *2;

        }
        
        SubPhase = true;
        }
        else
        {
        p_channels[0] = value;
        p_channels[1] = (3333 + value) % 10000 ;
        p_channels[2] = (6666 + value)  % 10000;
        SubPhase = false;
        }

        if (next_phase)
        {
            if (++m_demo1_phase >= 2 * NRF_PWM_CHANNEL_COUNT)
            {
                m_demo1_phase = 0;
            }
        }
    }
}
static void demo1(void)
{
    NRF_LOG_INFO("Demo 1");

    /*
     * This demo plays back a sequence with different values for individual
     * channels (LED 1 - LED 4). Only four values are used (one per channel).
     * Every time the values are loaded into the compare registers, they are
     * updated in the provided event handler. The values are updated in such
     * a way that increase and decrease of the light intensity can be observed
     * continuously on succeeding channels (one second per channel).
     */

    nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            BSP_LED_0 | NRF_DRV_PWM_PIN_INVERTED, // channel 0
            BSP_LED_1 | NRF_DRV_PWM_PIN_INVERTED, // channel 1
            //BSP_LED_3 | NRF_DRV_PWM_PIN_INVERTED, // channel 2
            BSP_LED_2 | NRF_DRV_PWM_PIN_INVERTED  // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_1MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = m_demo1_top,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, demo1_handler));
    m_used |= USED_PWM(0);

    m_demo1_seq_values.channel_0 = 0;
    m_demo1_seq_values.channel_1 = 0;
    m_demo1_seq_values.channel_2 = 0;
    m_demo1_seq_values.channel_3 = 0;
    m_demo1_phase                = 0;

    (void)nrf_drv_pwm_simple_playback(&m_pwm0, &m_demo1_seq, 1,
                                      NRF_DRV_PWM_FLAG_LOOP);
}

//-- PWM define done


//-- SAADC define

#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"

#define SAMPLES_IN_BUFFER 30

uint8 GetSampleInBuf(void)
{
	uint8 ret;
	ret = SAMPLES_IN_BUFFER;
	return ret;
}

static uint8 SAADC_InitFlag = 0;
static SAADC_Channel_Mode SAADC_Channel = SAADC_Channel_Battery;			// 1 = battery, 2 = probe


volatile uint8_t state = 1;

static const nrf_drv_timer_t m_timer = NRF_DRV_TIMER_INSTANCE(3);
nrf_saadc_value_t     m_buffer_pool[2][SAMPLES_IN_BUFFER];
static nrf_ppi_channel_t     m_ppi_channel_bat = NRF_PPI_CHANNEL1;
static nrf_ppi_channel_t     m_ppi_channel_probe = NRF_PPI_CHANNEL2;


//-- SAADC define done


void SAADC_timer_handler(nrf_timer_event_t event_type, void * p_context)
{

}


void saadc_sampling_event_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
    err_code = nrf_drv_timer_init(&m_timer, &timer_cfg, SAADC_timer_handler);
    APP_ERROR_CHECK(err_code);

    /* setup m_timer for compare event every 400ms */
    uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, 5);
    nrf_drv_timer_extended_compare(&m_timer,
                                   NRF_TIMER_CC_CHANNEL1,
                                   ticks,
                                   NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK,
                                   false);
    nrf_drv_timer_enable(&m_timer);

    uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer,
                                                                                NRF_TIMER_CC_CHANNEL1);
    uint32_t saadc_sample_task_addr   = nrf_drv_saadc_sample_task_get();

    /* setup ppi channel so that timer compare event is triggering sample task in SAADC */
    err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel_bat);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_assign(m_ppi_channel_bat,
                                          timer_compare_event_addr,
                                          saadc_sample_task_addr);
    APP_ERROR_CHECK(err_code);
}


void saadc_sampling_event_enable(void)
{
    ret_code_t err_code = nrf_drv_ppi_channel_enable(m_ppi_channel_bat);

    APP_ERROR_CHECK(err_code);
}


void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        ret_code_t err_code;

        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);
				if(SAADC_Channel == SAADC_Channel_Battery)
					{
					for(int j=0;j<SAMPLES_IN_BUFFER;j++)
						{
						Main.Device.BatVoltBuffer[j] = p_event->data.done.p_buffer[j];
						}
					}
				else if(SAADC_Channel == SAADC_Channel_Probe)
					{
					for(int j=0;j<SAMPLES_IN_BUFFER;j++)
						{
						Main.Sensor.Probe.ProbeBuffer[j] = p_event->data.done.p_buffer[j];
						}
					}

    }
}


void saadc_init_Battery(void)
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config_bat =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(OTQV2_GPIO_ADC_BATT);
        channel_config_bat.reference = NRF_SAADC_REFERENCE_INTERNAL; //internal voltage reference (0.6v) use
        channel_config_bat.gain = SAADC_CH_CONFIG_GAIN_Gain1_6; //1:6 gain. reference = 3.6v


    err_code = nrf_drv_saadc_init(NULL, saadc_callback);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(2, &channel_config_bat);
    APP_ERROR_CHECK(err_code);

    //err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
    //APP_ERROR_CHECK(err_code);


}
	

void saadc_init_Probe(void)
{
    ret_code_t err_code;

    nrf_saadc_channel_config_t channel_config_probe =
			NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(OTQV2_GPIO_ADC_PROBE);

    err_code = nrf_drv_saadc_init(NULL, saadc_callback);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(3, &channel_config_probe);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);

    //nrf_saadc_channel_init(1,&channel_config_probe);
    
    //err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1], SAMPLES_IN_BUFFER);
    //APP_ERROR_CHECK(err_code);
}
	

void OTQV2_SAADC_UnInit(void)
{

	if(SAADC_InitFlag)
		{
		nrfx_saadc_uninit();
		nrf_drv_ppi_uninit();
		nrfx_timer_uninit(&m_timer);
		NRF_SAADC->INTENCLR = (SAADC_INTENCLR_END_Clear << SAADC_INTENCLR_END_Pos);
                NVIC_ClearPendingIRQ(SAADC_IRQn);
		}
	SAADC_InitFlag = 0;
        
}

void OTQV2_SAADC_Init_Battery(void)
{
	SAADC_Channel = SAADC_Channel_Battery;
	if(!SAADC_InitFlag)
		{
    saadc_init_Battery();
    saadc_sampling_event_init();
    saadc_sampling_event_enable();
		}
	SAADC_InitFlag = 1;
}

void OTQV2_SAADC_Init_Probe(void)
{
	SAADC_Channel = SAADC_Channel_Probe;
	if(!SAADC_InitFlag)
		{
    saadc_init_Probe();
    saadc_sampling_event_init();
    saadc_sampling_event_enable();
		}
	SAADC_InitFlag = 1;
}


//---------------------TWI define -----------------------
#include "nrf_drv_twi.h"

//#define TWI_INSTANCE_ID             0
#define MAX_PENDING_TRANSACTIONS    5
nrf_drv_twi_t  m_twi  = NRF_DRV_TWI_INSTANCE(1);
//nrf_drv_twi_t  m_twi_sensor  = NRF_DRV_TWI_INSTANCE(0);

static volatile bool  m_xfer_done     = false;
static volatile bool  sensor_rd_done  = false;

static volatile bool  m_xfer_done_sensor     = false;
static volatile bool  sensor_rd_done_sensor  = false;

uint8 twibuffer[100];
uint8 m_sample;

ret_code_t twi_err_code;

void twi_errorcode_drive(ret_code_t err_code)
{
switch(err_code)
	{
	case NRF_SUCCESS:
		break;
		
	case NRF_TWIM_ERROR_DATA_NACK:
		TriColorLED_Blink(LEDColor_Red,10,100);
		break;
		
	case NRF_TWIM_ERROR_ADDRESS_NACK:
		TriColorLED_Blink(LEDColor_Cyan,4,100);
		break;
			
	default:
		TriColorLED_Blink(LEDColor_Purple,4,100);
		break;
	}
	
}


ret_code_t twi_write(nrf_drv_twi_t *twi_handler,uint8 TWIAddr,uint16 RegAddr,uint8 RegAddrLen, uint8 *data, uint16 len)
{
		uint16 TWI_TimeoutMs = 1000;
  // 2023.07.27 kyyun : Test
  uint8_t reg[256] = {0};
  //uint8_t reg[256] ;
    uint8_t AddrLen=0;
     if(RegAddrLen == 2)
    {
      //reg[0] = RegAddr & 0xff;
      //reg[1] = RegAddr>>8 & 0xff;        // swap
      reg[0] = RegAddr>>8 & 0xff;       
      reg[1] = RegAddr & 0xff;          // no swap
      AddrLen=2;
      memcpy(&reg[2],data,len);
    }
    else
    {
      reg[0] = RegAddr & 0xff;
      memcpy(&reg[1],data,len);
      AddrLen=1;
    }
    twi_err_code = nrf_drv_twi_tx(twi_handler, TWIAddr, reg, len + AddrLen, false);
		//NRF_LOG_INFO("[%10d] TWI handler : 0x%08x",getTickCount(), twi_handler);
    APP_ERROR_CHECK(twi_err_code);
  // 2023.07.27 kyyun : Test
  //free(reg);
    while (m_xfer_done == false)
			{
// 2023.07.27 kyyun : Test
#if 0
			HAL_Delay(1);
			TWI_TimeoutMs--;
			if(!TWI_TimeoutMs) 
#endif
				{
				TriColorLED_Blink(LEDColor_Cyan,4,100);
      // 2023.07.28 kyyun : Test
      //return NRFX_ERROR_DRV_TWI_ERR_ANACK;
				}
			}
		twi_errorcode_drive(twi_err_code);
}

#define TWIDelayUs 70
uint32 TWIDelay_1 = 300;
uint32 TWIDelay_2 = 500000;
uint32 TWITimeOutUs = 500000;			// 500mS
uint8 SuspendTemp = 0;


uint8 TWI_WaitSuspend(nrf_drv_twi_t *twi_handler)
{
	/*
uint32 *pSuspend = 0x40004148;
uint32 SuspendTemp2;
NRF_TWIM_Type *p_twim = twi_handler->u.twim.p_twim;
SuspendTemp = p_twim->EVENTS_SUSPENDED;	
SuspendTemp2 = *pSuspend;
	for(int i = 0; i < TWITimeOutUs ; i ++)
		{
		//nrf_delay_us(1);
		//if((!p_twim->SHORTS & TWIM_SHORTS_LASTTX_SUSPEND_Msk) && (!p_twim->SHORTS & TWIM_SHORTS_LASTRX_SUSPEND_Msk))
		if(!nrfx_twim_is_busy(p_twim))
		if(!nrf_twim_event_check(p_twim, NRF_TWIM_EVENT_SUSPENDED))
			{
			NRF_LOG_INFO("[%10d] Suspended end at %d",getTickCount(),i);
			return 0;					// TWI data transfer complete
			}
		}
	NRF_LOG_INFO("[%10d] Suspended time out : %d",getTickCount(),TWITimeOutUs);
	return 1;			// timeout
	*/					// suspend 플래그가 작동하지 않음
	return 0;
}
	
	
ret_code_t twi_read(nrf_drv_twi_t *twi_handler,uint8 TWIaddr,uint16 RegAddr,uint8 RegAddrLen, uint8 *data, uint16 len)
{
		uint32 TimeOut = TWITimeOutUs;
    m_xfer_done = false;
    uint8 RegAddr_8b;
    uint8 RegAddr_16b[2] = {0,0};
    if(RegAddrLen == 2)
    {
    RegAddr_16b[0] = RegAddr >>8 & 0xff;
    RegAddr_16b[1] = RegAddr & 0xff;
    //RegAddr_16b = (RegAddr >>8 & 0xff) | (RegAddr <<8 & 0xff00);        // swap little endian
    //RegAddr_16b = RegAddr;        // no swap
    twi_err_code = nrf_drv_twi_tx(twi_handler, TWIaddr, RegAddr_16b, 2, true);
		nrf_delay_us(TWIDelayUs);
    }
    else
    {
    RegAddr_8b = RegAddr;
    twi_err_code = nrf_drv_twi_tx(twi_handler, TWIaddr, &RegAddr_8b, 1, true);
		nrf_delay_us(TWIDelayUs);
    }

		do{
			nrf_delay_us(1);
			TimeOut--;
			if (!TimeOut){
					// TWI MODULE_ADDRESS error
					m_xfer_done = true;
					sensor_rd_done = true;
					return twi_err_code;
			}
	}while (m_xfer_done == false);
	
    m_xfer_done = false;
    twi_err_code = nrf_drv_twi_rx(twi_handler, TWIaddr, data, len);
			do{
				nrf_delay_us(1);
				TimeOut--;
				if (!TimeOut){
						// TWI MODULE_ADDRESS error
						m_xfer_done = true;
						sensor_rd_done = true;
						return twi_err_code;
				}
		}while (m_xfer_done == false);
		twi_errorcode_drive(twi_err_code);
    return twi_err_code;
}



EEPROM_Context EEPROM_TestContext;
uint8 Sensor_TestContext;

void twi_data_handler(uint8_t *d_samples)
{
EEPROM_Context *p = (EEPROM_Context*)d_samples;
  //NRF_LOG_INFO("index: %d   value : %d\n", p->index,p->Value);
}


static void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{

    switch (p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            //if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
            //{
            //    twi_data_handler(p_context);
            //}
            m_xfer_done = true;
            break;
        default:
            break;
    }
    
}

static uint8 TWIInitFlag = 0;

void DeInit_TWI(void)
{
	ret_code_t err_code;
	NRF_LOG_INFO("[%10d] TWI disabled. Was : %d",getTickCount() , TWIInitFlag);
	if(TWIInitFlag)
		{
		const nrf_drv_twi_config_t twi_config = {
       .scl                = 42,
       .sda                = 39,
       .frequency          = NRF_DRV_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = true
    };
    nrf_drv_twi_uninit(&m_twi);
*(volatile uint32_t *)0x40004FFC = 0;						// twi를 완전히 종료하기 위한 코드
*(volatile uint32_t *)0x40004FFC;								// 해당 선언을 하지 않으면 
*(volatile uint32_t *)0x40004FFC = 1;						// 슬립모드 에서 소모전류가 증가함
		err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);
		nrf_drv_twi_uninit(&m_twi);
		}
	TWIInitFlag = 0;
}
	
void Init_TWI(void)
{
    ret_code_t err_code;
NRF_LOG_INFO("[%10d] TWI Enable. Was : %d",getTickCount() , TWIInitFlag);
if(!TWIInitFlag)
	{
	    const nrf_drv_twi_config_t twi_config = {
       .scl                = 7,
       .sda                = 6,
       .frequency          = NRF_DRV_TWI_FREQ_400K,
			 //.frequency          = 0xCD00000,				// 800khz test value
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = true
    };

    //err_code = nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, NULL);
    err_code = nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, &EEPROM_TestContext);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
		//gpio_init();

	}
TWIInitFlag = 1;
}

//---------------------TWI define done-----------------------
//-- nus define
BLE_NUS_DEF(m_nus,1);
//-- nus define done



//---------------------BSP define -----------------------


/**@brief Function for handling events from the button handler module.
 *
 * @param[in] pin_no        The pin that the event applies to.
 * @param[in] button_action The button action (press/release).
 */
static uint16_t   m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;            /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
static ble_uuid_t m_adv_uuids[]          =                                          /**< Universally unique service identifier. */
{
    {BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}
};

/**@brief Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by the application.
 */
static void leds_init(void)
{
    bsp_board_init(BSP_INIT_LEDS);
}

/**@brief Function for initializing the button handler module.
 */
void buttons_init(void)
{
    ret_code_t err_code;

    //The array must be static because a pointer to it will be saved in the button handler module.
    static app_button_cfg_t buttons[2] =
    {
        {BSP_BUTTON_0, false, BUTTON_PULL, NULL},
        {BSP_BUTTON_1, false, BUTTON_PULL, NULL}
    };

    err_code = app_button_init(buttons, ARRAY_SIZE(buttons),
                               BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);
}


void Board_Init(void)
{
 
    //leds_init();
    buttons_init();

}

//---------------------BSP define done-----------------------


//-- dfu define
#include "ble_advertising.h"
BLE_ADVERTISING_DEF(m_advertising);  

static void dfu_observer(nrf_dfu_evt_type_t evt_type)
{
    switch (evt_type)
    {
        case NRF_DFU_EVT_DFU_FAILED:
            break;

				case NRF_DFU_EVT_DFU_ABORTED:
            break;

				case NRF_DFU_EVT_DFU_INITIALIZED:
						TriColorLED_SetBlue();
            break;
        case NRF_DFU_EVT_TRANSPORT_ACTIVATED:
            break;
        case NRF_DFU_EVT_DFU_STARTED:
						TriColorLED_SetGreen();
						break;
        default:
            break;
    }
}



void BLDFU_Init(void)
{
    uint32_t ret_val;

    // Must happen before flash protection is applied, since it edits a protected page.
    nrf_bootloader_mbr_addrs_populate();

    // Protect MBR and bootloader code from being overwritten.
    ret_val = nrf_bootloader_flash_protect(0, MBR_SIZE);
    APP_ERROR_CHECK(ret_val);
    ret_val = nrf_bootloader_flash_protect(BOOTLOADER_START_ADDR, BOOTLOADER_SIZE);
    APP_ERROR_CHECK(ret_val);

    //(void) NRF_LOG_INIT(nrf_bootloader_dfu_timer_counter_get);
    //NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("Inside main");

    ret_val = nrf_bootloader_init(dfu_observer);
    APP_ERROR_CHECK(ret_val);

    NRF_LOG_FLUSH();

    NRF_LOG_ERROR("After main, should never be reached.");
    NRF_LOG_FLUSH();

    APP_ERROR_CHECK_BOOL(false);
}

static void advertising_config_get(ble_adv_modes_config_t * p_config)
{
    memset(p_config, 0, sizeof(ble_adv_modes_config_t));

    p_config->ble_adv_fast_enabled  = true;
    p_config->ble_adv_fast_interval = APP_ADV_INTERVAL;
    p_config->ble_adv_fast_timeout  = APP_ADV_DURATION;
}

static void disconnect(uint16_t conn_handle, void * p_context)
{
    UNUSED_PARAMETER(p_context);

    ret_code_t err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_WARNING("Failed to disconnect connection. Connection handle: %d Error: %d", conn_handle, err_code);
    }
    else
    {
        NRF_LOG_DEBUG("Disconnected connection handle %d", conn_handle);
    }
}


static void ble_dfu_buttonless_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
  switch(event)
  {
    case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
      NRF_LOG_INFO("Device is preparing to enter bootloader mode\r\n");

      //OTQ_EnterBootLoader();

			wdt_feed();
     // Prevent device from advertising on disconnect.
      ble_adv_modes_config_t config;
      advertising_config_get(&config);
      config.ble_adv_on_disconnect_disabled = true;
      ble_advertising_modes_config_set(&m_advertising, &config);

      // Disconnect all other bonded devices that currently are connected.
      // This is required to receive a service changed indication
      // on bootup after a successful (or aborted) Device Firmware Update.
      uint32_t conn_count = ble_conn_state_for_each_connected(disconnect, NULL);
      //BLDFU_Init();

      NRF_LOG_INFO("Disconnected %d links.", conn_count);
      break;

    case BLE_DFU_EVT_BOOTLOADER_ENTER:
      NRF_LOG_INFO("Device will enter bootloader mode\r\n"); 
      OTQ_EnterBootLoader();
      break;

    case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
      NRF_LOG_ERROR("Device failed to enter bootloader mode\r\n");
      break;

    default:
      NRF_LOG_INFO("Unknown event from ble_dfu.\r\n");
      break;
  }
}

static bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
  switch(event)
  {
    case NRF_PWR_MGMT_EVT_PREPARE_DFU:
      NRF_LOG_INFO("Power management wants to reset to DFU mode\r\n");
      break;
    
    default:
      return true;
  }
  NRF_LOG_INFO("Power management allowed to reset to DFU mode\r\n");
  return true;
}

NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);

//-- dfu define done


//---------------------BLE define -----------------------



/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Struct that contains pointers to the encoded advertising data. */
static ble_gap_adv_data_t m_adv_data =
{
    .adv_data =
    {
        .p_data = m_enc_advdata,
        .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX
    },
    .scan_rsp_data =
    {
        .p_data = m_enc_scan_response_data,
        .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX

    }
};


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(stMain *pMain)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;
		
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)pMain->BLE.DeviceName,
                                          strlen(pMain->BLE.DeviceName));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
        m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        NRF_LOG_INFO("Data len is set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
    }
    NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
                  p_gatt->att_mtu_desired_central,
                  p_gatt->att_mtu_desired_periph);
}

/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
	ret_code_t  err_code;
		err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
    APP_ERROR_CHECK(err_code);

		err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);
		
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            //err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            //APP_ERROR_CHECK(err_code);
            break;
        case BLE_ADV_EVT_IDLE:
            //sleep_mode_enter();
            break;
        default:
            break;
    }
}

/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
 

 
ble_advdata_manuf_data_t adv_manuf_data;
uint8_array_t            adv_manuf_data_array;
uint8_t                  adv_manuf_data_data[24] = {
			1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,24 };




 
void advertising_init(void)
{
    ret_code_t    err_code;
	#if 1
    ble_advdata_t advdata;
    ble_advdata_t srdata;
    ble_uuid_t adv_uuids[] = {{0x1200, 3}};

		// 제조사 데이터 설정
    adv_manuf_data.company_identifier = 0xAFC4;
    adv_manuf_data.data.p_data = adv_manuf_data_data;
    adv_manuf_data.data.size = sizeof(adv_manuf_data_data);
			
    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    // 광고 데이터 초기화
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type          = BLE_ADVDATA_NO_NAME; // 이름을 광고 데이터에서 제거
    advdata.include_appearance = false;
    advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
		advdata.p_manuf_specific_data = &adv_manuf_data;

    // 스캔 응답 데이터 초기화
    memset(&srdata, 0, sizeof(srdata));
    srdata.name_type               = BLE_ADVDATA_FULL_NAME; // 이름을 스캔 응답 데이터로 설정
    srdata.include_appearance      = false;
    srdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    srdata.uuids_complete.p_uuids  = adv_uuids;

    err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    APP_ERROR_CHECK(err_code);

    err_code = ble_advdata_encode(&srdata, m_adv_data.scan_rsp_data.p_data, &m_adv_data.scan_rsp_data.len);
    APP_ERROR_CHECK(err_code);

    ble_gap_adv_params_t adv_params;

    // Set advertising parameters.
    memset(&adv_params, 0, sizeof(adv_params));

		#if 1			// 1 = BLE long range
		adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;
    //adv_params.secondary_phy   = BLE_GAP_PHY_CODED;
    //adv_params.properties.type = BLE_GAP_ADV_TYPE_EXTENDED_CONNECTABLE_NONSCANNABLE_UNDIRECTED;
		adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
		#else
    adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;
    adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
		#endif

    adv_params.duration        = APP_ADV_DURATION;
    adv_params.p_peer_addr     = NULL;
    adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    adv_params.interval        = APP_ADV_INTERVAL;

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &adv_params);
    APP_ERROR_CHECK(err_code);
		
		#else
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_NO_NAME;
    init.advdata.include_appearance      = false;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    
    advertising_config_get(&init.config);
		init.config.ble_adv_fast_interval=MSEC_TO_UNITS(500, UNIT_0_625_MS);
    init.evt_handler = on_adv_evt;
		adv_manuf_data_array.p_data = midascon_adv_manuf_data_data;
    adv_manuf_data_array.size = sizeof(midascon_adv_manuf_data_data);

		adv_manuf_data.company_identifier = 0x83c4; 
    adv_manuf_data.data = adv_manuf_data_array;
		init.advdata.p_manuf_specific_data=&adv_manuf_data;
		init.srdata.name_type             = BLE_ADVDATA_FULL_NAME;
    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
	#endif
}


static void update_manufacturer_data(uint8_t *new_data, uint8_t new_data_length)
{
    if (new_data_length > 24)
    {
        NRF_LOG_ERROR("New manufacturer data is too long!");
        return;
    }

    memcpy(adv_manuf_data_data, new_data, new_data_length);
    adv_manuf_data.data.p_data = adv_manuf_data_data;
    adv_manuf_data.data.size = new_data_length;

    advertising_init();
}

	void UpdateBleAdvertisePacket(stMain *pMain)
	{
	EEPROM_RawDatastructure EEP_Temp = {0};
	pMain->BLE.AdvPacket.Battery = Batt_GetVoltage_Percent(pMain);
	EEPROM_GetRawDataForBle(pMain);
	//memcpy(adv_manuf_data_data , &pMain->BLE.AdvPacket , 24);
	advertising_stop();
  update_manufacturer_data(&pMain->BLE.AdvPacket,24);
	advertising_start(pMain);
	NRF_LOG_INFO("[%10d] BLE advertise packet update",getTickCount());
	}
/**@brief Function for initializing services that will be used by the application.
 */

 
/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@snippet [Handling the data received over BLE] */
#include "app_uart.h"

static void nus_data_handler(ble_nus_evt_t * p_evt)
{

    if (p_evt->type == BLE_NUS_EVT_RX_DATA)
    {
        uint32_t err_code;

        NRF_LOG_DEBUG("Received data from BLE NUS. Writing data on UART.");
        NRF_LOG_HEXDUMP_DEBUG(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);

        for (uint32_t i = 0; i < p_evt->params.rx_data.length; i++)
        {
            do
            {
                //err_code = app_uart_put(p_evt->params.rx_data.p_data[i]);
                err_code = NRF_SUCCESS;
                if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_BUSY))
                {
                    NRF_LOG_ERROR("Failed receiving NUS message. Error 0x%x. ", err_code);
                    APP_ERROR_CHECK(err_code);
                }
            } while (err_code == NRF_ERROR_BUSY);
        }
        if (p_evt->params.rx_data.p_data[p_evt->params.rx_data.length - 1] == '\r')
        {
            while (app_uart_put('\n') == NRF_ERROR_BUSY);
        }
    }

}
			
/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                err_code = ble_advertising_restart_without_whitelist(&m_advertising);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break;

        default:
            break;
    }
}			

/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' '\n' (hex 0x0A) or if the string has reached the maximum data length.
 */
/**@snippet [Handling the data received over UART] */
void uart_event_handle(app_uart_evt_t * p_event)
{
    static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
    static uint8_t index = 0;
    uint32_t       err_code;

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            index++;

            if ((data_array[index - 1] == '\n') ||
                (data_array[index - 1] == '\r') ||
                (index >= m_ble_nus_max_data_len))
            {
                if (index > 1)
                {
                    NRF_LOG_DEBUG("Ready to send data over BLE NUS");
                    NRF_LOG_HEXDUMP_DEBUG(data_array, index);

                    do
                    {
                        uint16_t length = (uint16_t)index;
                        err_code = ble_nus_data_send(&m_nus, data_array, &length, m_conn_handle);
                        if ((err_code != NRF_ERROR_INVALID_STATE) &&
                            (err_code != NRF_ERROR_RESOURCES) &&
                            (err_code != NRF_ERROR_NOT_FOUND))
                        {
                            APP_ERROR_CHECK(err_code);
                        }
                    } while (err_code == NRF_ERROR_RESOURCES);
                }

                index = 0;
            }
            break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}

/**@snippet [Handling the data received over UART] */


/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
void uart_init(void)
{
    uint32_t                     err_code;
    app_uart_comm_params_t const comm_params =
    {
        .rx_pin_no    = RX_PIN_NUMBER,
        .tx_pin_no    = TX_PIN_NUMBER,
        .rts_pin_no   = RTS_PIN_NUMBER,
        .cts_pin_no   = CTS_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity   = false,
#if defined (UART_PRESENT)
        .baud_rate    = NRF_UART_BAUDRATE_115200
#else
        .baud_rate    = NRF_UARTE_BAUDRATE_115200
#endif
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}
				
/**@snippet [Handling the data received over BLE] */

//ble_nus_data_handler_t nus_data_handler;


static void services_init(void)
{
    ret_code_t         err_code;
    ble_lbs_init_t     init     = {0};
    ble_nus_init_t nus_init;
    nrf_ble_qwr_init_t qwr_init = {0};
    ble_OTQV2_init_t   OTQV2_init = {0};
    ble_OTQV2_init_t   OTQV2_Set_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // initialize NUS
    memset(&nus_init, 0, sizeof(nus_init));
    nus_init.data_handler = nus_data_handler;
    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&OTQV2_init.custom_value_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&OTQV2_init.custom_value_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&OTQV2_init.custom_value_char_attr_md.write_perm);

		
    err_code = ble_OTQV2_service_init(&m_OTQV2_service, &OTQV2_init);
    if(err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("OTQV2 Service Initializing is failed = %d", err_code);
        return;
    }

    // initialize buttonless DFU
    ble_dfu_buttonless_init_t dfus_init = 
    {
      .evt_handler = ble_dfu_buttonless_evt_handler
    };
    err_code = ble_dfu_buttonless_init(&dfus_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module that
 *          are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply
 *       setting the disconnect_on_fail config parameter, but instead we use the event
 *       handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for starting advertising.
 */
 static uint8 ADV_RunFlag = 0;
void advertising_start(stMain *pMain)
{
    ret_code_t           err_code;
		NRF_LOG_INFO("Advertise start request. ADC flag %d",ADV_RunFlag);
		if(!ADV_RunFlag)
			{
			//err_code = ble_advertising_start(&m_advertising,BLE_ADV_MODE_FAST);
			//APP_ERROR_CHECK(err_code);
			
			err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV,NULL,pMain->BLE.Power);
			APP_ERROR_CHECK(err_code);

			//err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
			
			err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
			APP_ERROR_CHECK(err_code);
			Main.BLE.AdvFlag = 1;
			}
		ADV_RunFlag = 1;

}

void advertising_stop(void)
{
    ret_code_t           err_code;

		if(ADV_RunFlag)
			{
			err_code = sd_ble_gap_adv_stop(m_adv_handle);
			APP_ERROR_CHECK(err_code);
			
			}
		NRF_LOG_INFO("Advertise stop request. ADC flag %d",ADV_RunFlag);
		Main.BLE.AdvFlag = 0;
		ADV_RunFlag = 0;

}
	
	
/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
 uint32 LastBLEEvent = 0;
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code;
		static RunStat oldRunStat;

		LastBLEEvent = p_ble_evt->header.evt_id;
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected");
						TriColorLED_ActiveBlue(LedOnOff_On);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
						oldRunStat = Main.Device.RunningStatus;			// last status backup
						BLEConnectTimer_Start(BLE_AutoDisConnectionTimerSec * 1000);							// ble keep connection limit timer
						ADV_RunFlag = 0;
						if(Main.Device.RunningStatus == RunStat_SelfTest2)
							{
							Main.Device.RunningStatus = RunStat_SelfTest2_Connected;
							}
						else
							{
							Main.Device.RunningStatus = RunStat_Logging_Connected;
							}
						
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected");
						BLEConnectTimer_Stop();
						TriColorLED_ActiveBlue(LedOnOff_Off);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
						//Main.Device.RunningStatus = RunStat_Logging;
						Main.Device.RunningStatus = oldRunStat;								// last status return
						ADV_RunFlag = 0;
						OTQV2_SystemWakeUp();
						if(Main.DeviceConfig.BLEActive == DeviceConfig_BLEActive_Always)
							{
							advertising_start(&Main);	// for intsain test
							}
						OTQV2_SystemSleep(&Main);
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
                                                   BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
                                                   NULL,
                                                   NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                //.rx_phys = BLE_GAP_PHY_AUTO,
                //.tx_phys = BLE_GAP_PHY_AUTO,
                .rx_phys = BLE_GAP_PHY_1MBPS,
                .tx_phys = BLE_GAP_PHY_1MBPS,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

			
        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
						Main.Device.RunningStatus = oldRunStat;								// last status return
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
						Main.Device.RunningStatus = oldRunStat;								// last status return
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
						Main.Device.RunningStatus = oldRunStat;								// last status return
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


//---------------------BLE define done-----------------------


//-- GPIO define

static uint8 HFActiveFlag=0;

void OTQV2_HF_Stop(void)
{
	if(HFActiveFlag)
		{
		nrf_drv_clock_hfclk_release();
		}
	HFActiveFlag = 0;
}

void OTQV2_HF_Start(void)
{
	if(!HFActiveFlag)
		{
		hfclk_config();
		}
	HFActiveFlag = 1;
}


void OTQV2_SystemSleep(stMain *pMain)
{
	NRF_P0->DETECTMODE = GPIO_DETECTMODE_DETECTMODE_LDETECT << GPIO_DETECTMODE_DETECTMODE_Pos;

#if 1					// 1 = sensor power enable. for prevent on/off inrush current
	nrf_gpio_pin_write(OTQV2_GPIO_SENSOR_POWER, 1);			// on off switch

	nrf_gpio_pin_write(OTQV2_GPIO_ADC_PROBE_EN, 1);			// p channel FET
	nrf_gpio_pin_write(OTQV2_GPIO_ADC_BATT_EN, 0);			// n channel FET
	
	nrf_gpio_pin_write(OTQV2_GPIO_EPD_CLK, 1);					// EPD GPIO disable
	nrf_gpio_pin_write(OTQV2_GPIO_EPD_DA, 1);						// EPD GPIO disable
	nrf_gpio_pin_write(OTQV2_GPIO_EPD_CS, 1);						// EPD GPIO disable
	nrf_gpio_pin_write(OTQV2_GPIO_EPD_RST, 1);					// EPD GPIO disable
	nrf_gpio_pin_write(OTQV2_GPIO_EPD_DC, 1);						// EPD GPIO disable

	nrf_gpio_pin_write(OTQV2_GPIO_LED_R, 1);						// R LED off
	nrf_gpio_pin_write(OTQV2_GPIO_LED_G, 1);						// G LED off
	nrf_gpio_pin_write(OTQV2_GPIO_LED_B, 1);						// B LED off
#else
	nrf_gpio_pin_write(OTQV2_GPIO_SENSOR_POWER, 0);			// on off switch

	nrf_gpio_pin_write(OTQV2_GPIO_ADC_OPAMP_SHDN, 0);		// 0 = op amp disable
	nrf_gpio_pin_write(OTQV2_GPIO_ADC_PROBE_EN, 0);			// p channel FET
	nrf_gpio_pin_write(OTQV2_GPIO_ADC_BATT_EN, 0);			// n channel FET
	nrf_gpio_pin_write(OTQV2_GPIO_TWI_SHTC3_RST, 0);		// SHTC3 sensor reset
	nrf_gpio_pin_write(OTQV2_GPIO_EPD_CLK, 0);					// EPD GPIO disable
	nrf_gpio_pin_write(OTQV2_GPIO_EPD_DA, 0);						// EPD GPIO disable
	nrf_gpio_pin_write(OTQV2_GPIO_EPD_CS, 0);						// EPD GPIO disable
	nrf_gpio_pin_write(OTQV2_GPIO_EPD_RST, 0);					// EPD GPIO disable
	nrf_gpio_pin_write(OTQV2_GPIO_EPD_DC, 0);						// EPD GPIO disable
	nrf_gpio_pin_write(OTQV2_GPIO_TWI_MEM_WP, 1);				// EEPROM write protection enable
	nrf_gpio_pin_write(OTQV2_GPIO_NRF2_PWR, 0);					// on off switch
	nrf_gpio_pin_write(OTQV2_GPIO_LED_R, 1);						// R LED off
	nrf_gpio_pin_write(OTQV2_GPIO_LED_G, 1);						// G LED off
	nrf_gpio_pin_write(OTQV2_GPIO_LED_B, 1);						// B LED off
#endif

	//deInit_TWI();
	OTQV2_SAADC_UnInit();
	if(m_conn_handle == BLE_CONN_HANDLE_INVALID)
		{
		DeInit_TWI();
		}
/*
	nrf_gpio_cfg_default(42);
	nrf_gpio_cfg_default(39);
	nrf_gpio_cfg_default(24);
	nrf_gpio_cfg_default(22);
	*/
	OTQV2_HF_Stop();
}

void OTQV2_GPIO_Init(void)
{

nrf_gpio_cfg_output(OTQV2_GPIO_SENSOR_POWER);    
nrf_gpio_pin_write(OTQV2_GPIO_SENSOR_POWER, 1);

nrf_gpio_cfg_output(OTQV2_GPIO_OLED_VPP_EN);
nrf_gpio_cfg_output(OTQV2_GPIO_LCD_PWR_EN);

nrf_gpio_pin_write(OTQV2_GPIO_LCD_PWR_EN, 0);
HAL_Delay(1);
nrf_gpio_pin_write(OTQV2_GPIO_OLED_VPP_EN, 0);
HAL_Delay(1);


nrf_gpio_pin_write(OTQV2_GPIO_LCD_PWR_EN, 1);
HAL_Delay(1);
nrf_gpio_pin_write(OTQV2_GPIO_OLED_VPP_EN, 1);

nrf_gpio_cfg_output(OTQV2_GPIO_LED_R);    
nrf_gpio_cfg_output(OTQV2_GPIO_LED_G);
nrf_gpio_cfg_output(OTQV2_GPIO_LED_B);

  nrf_gpio_cfg_input(OTQV2_GPIO_EPD_BUSY, NRF_GPIO_PIN_NOPULL);
  nrf_gpio_cfg_output(OTQV2_GPIO_EPD_CLK);
  nrf_gpio_cfg_output(OTQV2_GPIO_EPD_DA);
  nrf_gpio_cfg_output(OTQV2_GPIO_EPD_CS);
  nrf_gpio_cfg_output(OTQV2_GPIO_EPD_RST);
  nrf_gpio_cfg_output(OTQV2_GPIO_EPD_DC);

 nrf_gpio_cfg_output(OTQV2_GPIO_ADC_PROBE_EN);
 nrf_gpio_pin_write(OTQV2_GPIO_ADC_PROBE_EN, 0);			

 nrf_gpio_cfg_output(OTQV2_GPIO_ADC_BATT_EN);
 nrf_gpio_pin_write(OTQV2_GPIO_ADC_BATT_EN, 1);
/*
 nrf_gpio_cfg_input(6, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(7, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(8, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(12, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(13, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(14, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(15, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(16, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(20, NRF_GPIO_PIN_PULLDOWN);
 
 
 
 nrf_gpio_cfg_input(29, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(32, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(33, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(34, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(35, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(36, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(37, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(38, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(39, NRF_GPIO_PIN_NOPULL);
 nrf_gpio_cfg_input(40, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(41, NRF_GPIO_PIN_PULLDOWN);
 nrf_gpio_cfg_input(42, NRF_GPIO_PIN_NOPULL);
*/
  //nrf_gpio_cfg_input(OTQV2_GPIO_KEY_RIGHT, NRF_GPIO_PIN_NOPULL);
  //nrf_gpio_cfg_input(OTQV2_GPIO_KEY_LEFT, NRF_GPIO_PIN_NOPULL);
HAL_Delay(10);

}

//-- GPIO define done

void OTQV2_SystemWakeUp(void)
{
	OTQV2_GPIO_Init();
	OTQV2_HF_Start();
	Init_TWI();
	//NVIC_DisableIRQ(SysTick_IRQn);
}

void OTQV2_BLE_Init(stMain *pMain)
{
    ble_stack_init();
    gap_params_init(pMain);
    gatt_init();
		OTQV2_MACProcess(&Main);
    services_init();
    advertising_init();
    conn_params_init();
		OTQV2_BLE_TxPowerApply();

}

/*
void OTQV2_RTC_Init(void)
{
//lfclk_config();
//rtc_config();
//rtc_init();
}*/

extern const unsigned char stVersionString[11];


void OTQV2_MainStructure_Init(stMain *pMain)
{

	#ifdef BELL_ENC
	pMain->DeviceConfig.Interval = DeviceConfig_Interval_10Min;
	#else
	pMain->DeviceConfig.Interval = DeviceConfig_Interval_15Min;
	#endif
	pMain->DeviceConfig.DisplayInterval = DeviceConfig_DisplayInterval_FU;
	pMain->DeviceConfig.DeviceConfigPage = DeviceConfig_Page_Main;
	pMain->DeviceConfig.BLEActive = DeviceConfig_BLEActive_NFC;
	//pMain->DeviceConfig.BLEActive = DeviceConfig_BLEActive_Interval;
	//pMain->DeviceConfig.BLEActive = DeviceConfig_BLEActive_Always;
	pMain->DeviceConfig.BLEPower = DeviceConfig_BLEPwr_4dBm;
	pMain->Device.RunningStatus = RunStat_Booting;
	pMain->Calib.CalibrationWaitTimeMin = 5;
	pMain->Calib.CorrectionWaitTimeMin = 420;			// correction data collection time . 420 = 7Hours
	//pMain->Calib.CorrectionWaitTimeMin = 5 * 300;			// correction data collection time . 1500 = 25Hours
	//pMain->Calib.CorrectionWaitTimeMin = 10;			// correction data collection time . 10min
	pMain->Calib.CalibView = CalibView_OffsetApply;
	memcpy(pMain->DevInfo.FWVer , stVersionString , 11);
	//memcpy(pMain->BLE.MAC, (uint8_t *)0x10001080, 6);


}


uint8 DeviceSoftName[30] = {"V2-"};

void MCU_Init(stMain *pMain)
{
sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
nrf_gpio_cfg_sense_set(OTQV2_GPIO_KEY_LEFT,GPIO_PIN_CNF_SENSE_Low);       // button wake up setting
nrf_gpio_cfg_sense_set(OTQV2_GPIO_KEY_RIGHT,GPIO_PIN_CNF_SENSE_Low);      // button wake up setting

}

void OTQV2_UICRtoSerial(stMain *pMain, sint64 UICRTemp)
{
	uint64 SerialUICRDataMajor = (UICRTemp) / 100000;
	uint64 SerialUICRDataMinor = (UICRTemp) % 100000;
	
	if(SizeOfSerial == 8)
			{
			pMain->DevInfo.Serial[0] = SerialUICRDataMajor / 100 % 10 + 0x30;
			pMain->DevInfo.Serial[1] = SerialUICRDataMajor / 10 % 10 + 0x30;
			pMain->DevInfo.Serial[2] = SerialUICRDataMajor / 1 % 10 + 0x30;
		
			pMain->DevInfo.Serial[3] = SerialUICRDataMinor / 10000 % 10 + 0x30;
			pMain->DevInfo.Serial[4] = SerialUICRDataMinor / 1000 % 10 + 0x30;
			pMain->DevInfo.Serial[5] = SerialUICRDataMinor / 100 % 10 + 0x30;
			pMain->DevInfo.Serial[6] = SerialUICRDataMinor / 10 % 10 + 0x30;
			pMain->DevInfo.Serial[7] = SerialUICRDataMinor / 1 % 10 + 0x30;
			
			UICRTemp %= 100000000;
			}
		else
			{
			pMain->DevInfo.Serial[0] = SerialUICRDataMajor / 10000 % 10 + 0x30;
			pMain->DevInfo.Serial[1] = SerialUICRDataMajor / 1000 % 10 + 0x30;
			pMain->DevInfo.Serial[2] = SerialUICRDataMajor / 100 % 10 + 0x30;
			pMain->DevInfo.Serial[3] = SerialUICRDataMajor / 10 % 10 + 0x30;
			pMain->DevInfo.Serial[4] = SerialUICRDataMajor / 1 % 10 + 0x30;
		
			pMain->DevInfo.Serial[5] = SerialUICRDataMinor / 10000 % 10 + 0x30;
			pMain->DevInfo.Serial[6] = SerialUICRDataMinor / 1000 % 10 + 0x30;
			pMain->DevInfo.Serial[7] = SerialUICRDataMinor / 100 % 10 + 0x30;
			pMain->DevInfo.Serial[8] = SerialUICRDataMinor / 10 % 10 + 0x30;
			pMain->DevInfo.Serial[9] = SerialUICRDataMinor / 1 % 10 + 0x30;
			}

		
#if BLEDataSyntaxVersion == 11
		pMain->BLE.MAC[5] = 0x00;
		pMain->BLE.MAC[4] = 0xA1;

		pMain->BLE.MAC[3] = (SerialUICRDataMajor >> 8) & 0xff;
		pMain->BLE.MAC[2] = (SerialUICRDataMajor) & 0xff;
		
		pMain->BLE.MAC[1] = (SerialUICRDataMinor >> 8) & 0xff;
		pMain->BLE.MAC[0] = (SerialUICRDataMinor) & 0xff;
#elif BLEDataSyntaxVersion == 12
		pMain->BLE.MAC[5] = 0x01;
		pMain->BLE.MAC[4] = 0xA1;

		pMain->BLE.MAC[3] = (UICRTemp >> 24) & 0xff;
		pMain->BLE.MAC[2] = (UICRTemp >> 16) & 0xff;
		pMain->BLE.MAC[1] = (UICRTemp >> 8) & 0xff;
		pMain->BLE.MAC[0] = (UICRTemp) & 0xff;
#endif
		pMain->BLE.Serial = SerialUICRDataMajor<<16 | SerialUICRDataMinor;
}

void OTQV2_AsciitoSerial(stMain *pMain)
{
	uint32 SerialData = 
			(pMain->DevInfo.Serial[0] - 0x30) * 10000000 + 
			(pMain->DevInfo.Serial[1] - 0x30) * 1000000 + 
			(pMain->DevInfo.Serial[2] - 0x30) * 100000 + 
			(pMain->DevInfo.Serial[3] - 0x30) * 10000 + 
			(pMain->DevInfo.Serial[4] - 0x30) * 1000 + 
			(pMain->DevInfo.Serial[5] - 0x30) * 100 + 
			(pMain->DevInfo.Serial[6] - 0x30) * 10 + 
			(pMain->DevInfo.Serial[7] - 0x30) * 1;

		pMain->BLE.Serial = SerialData;

#if BLEDataSyntaxVersion == 12
		pMain->BLE.MAC[5] = 0x01;
#elif BLEDataSyntaxVersion == 11
		pMain->BLE.MAC[5] = 0x00;
#endif
		pMain->BLE.MAC[4] = 0xA1;
		
		pMain->BLE.MAC[3] = (SerialData >> 24) & 0xff;
		pMain->BLE.MAC[2] = (SerialData >> 16) & 0xff;
		pMain->BLE.MAC[1] = (SerialData >> 8) & 0xff;
		pMain->BLE.MAC[0] = (SerialData) & 0xff;
}
	
void OTQV2_WriteSerialToUicr(stMain *pMain, uint64 UICRTemp)
{
	uint32 Backup_NRFFW[13];
	uint32 Backup_NRFHW[12];
	uint32 Backup_UICR[32];
	uint32 Backup_PSELRESET[2];
	uint32_t uicr_address = 0x10001014;
	
	uint32 serial_buffer_H = 0 , serial_buffer_L = 0;;

	serial_buffer_H = (UICRTemp >> 32) & 0xffffffff;
	serial_buffer_L = (UICRTemp) & 0xffffffff;
	
	uint8_t sd_enabled = nrf_sdh_is_enabled();		// 소프트 디바이스 활성화 체크
	
	  if(sd_enabled)
			{
			nrf_sdh_disable_request();								// UICR 편집 전 소프트디바이스 비활성화
			}
    
		for(int i=0 ; i<13; i++)
			{
			Backup_NRFFW[i] = NRF_UICR->NRFFW[i];
			}

		for(int i=0 ; i<12; i++)
			{
			Backup_NRFHW[i] = NRF_UICR->NRFHW[i];
			}
	
		for(int i=0 ; i<2; i++)
			{
			Backup_PSELRESET[i] = NRF_UICR->PSELRESET[i];
			}

		for(int i=0 ; i<32 ; i++)
			{
			Backup_UICR[i] = NRF_UICR->CUSTOMER[i];
			}
		
		//__disable_irq();
		//CRITICAL_REGION_ENTER();
		// Read and buffer UICR register content prior to erase
		// Enable Erase mode
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos; //0x02; 
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
		
		// Erase the UICR registers
		NRF_NVMC->ERASEUICR = NVMC_ERASEUICR_ERASEUICR_Erase << NVMC_ERASEUICR_ERASEUICR_Pos; //0x00000001;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
	 
		// Enable WRITE mode
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos; //0x01;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
			
		NRF_LOG_INFO(" NVMC_ERASEUICR_ERASEUICR_Erase ");		

		Backup_UICR[0] = serial_buffer_L;
		Backup_UICR[1] = serial_buffer_H;
		
			for(int i=0 ; i<32 ; i++)
				{
				uicr_address = 0x10001084 + i*4;
				if(Backup_UICR[i] != 0xffffffff)
					{
					NRF_UICR->CUSTOMER[i] = Backup_UICR[i];
					}
				// Wait untill the NVMC peripheral has finished writting to the UICR register
				while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
				}

		for(int i=0 ; i<13; i++)
			{
			NRF_UICR->NRFFW[i] = Backup_NRFFW[i];
				// Wait untill the NVMC peripheral has finished writting to the UICR register
				while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
			}

		for(int i=0 ; i<12; i++)
			{
			NRF_UICR->NRFHW[i] = Backup_NRFHW[i];
				// Wait untill the NVMC peripheral has finished writting to the UICR register
				while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
			}
	
		for(int i=0 ; i<2; i++)
			{
			NRF_UICR->PSELRESET[i] = Backup_PSELRESET[i];
				// Wait untill the NVMC peripheral has finished writting to the UICR register
				while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
			}
		

		//CRITICAL_REGION_EXIT();
		//__enable_irq();
		if(sd_enabled)
			{
			NVIC_SystemReset();													// 재부팅.  소프트 디바이스를 재 활성화 하는것보다 리셋 하는것이 효율적임
			}
    
}

void OTQV2_BackupSerialNumber(stMain *pMain)
{
	uint64_t *pSerialUICRData = (uint32_t *) 0x10001080;
	uint64_t *pSerialUICRDataMinor = (uint32_t *) 0x10001084;

	
	sint64 SerialUICRDataTemp = *pSerialUICRData;
	

	if(SerialUICRDataTemp != -1)			// uicr valid check
		{
		EEPROM_WriteSerialNumber(&SerialUICRDataTemp);
		}
}
	
void OTQV2_BringSerialNumber_UICR(stMain *pMain)
{
	uint64_t *pSerialUICRData = (uint32_t *) 0x10001080;
	uint64_t *pSerialUICRDataMinor = (uint32_t *) 0x10001084;

	
	sint64 SerialUICRDataTemp = *pSerialUICRData;
	

	if(SerialUICRDataTemp != -1 && SerialUICRDataTemp < 0x200000000)			// uicr valid check
		{
		OTQV2_UICRtoSerial(pMain,SerialUICRDataTemp);
		}
	else
		{
		EEPROM_GetSerialNumber(&SerialUICRDataTemp);
		if(SerialUICRDataTemp != -1 && SerialUICRDataTemp < 0x200000000)			// uicr valid check
			{
			OTQV2_UICRtoSerial(pMain,SerialUICRDataTemp);
			OTQV2_WriteSerialToUicr(pMain,SerialUICRDataTemp);
			}
		else
			{
			pMain->DevInfo.Serial[0] = NRF_FICR->DEVICEID[1] / 10000000 % 10 + 0x30;
			pMain->DevInfo.Serial[1] = NRF_FICR->DEVICEID[1] / 1000000 % 10 + 0x30;
			pMain->DevInfo.Serial[2] = NRF_FICR->DEVICEID[1] / 100000 % 10 + 0x30;
			pMain->DevInfo.Serial[3] = NRF_FICR->DEVICEID[1] / 10000 % 10 + 0x30;
			pMain->DevInfo.Serial[4] = NRF_FICR->DEVICEID[1] / 1000 % 10 + 0x30;
			pMain->DevInfo.Serial[5] = NRF_FICR->DEVICEID[1] / 100 % 10 + 0x30;
			pMain->DevInfo.Serial[6] = NRF_FICR->DEVICEID[1] / 10 % 10 + 0x30;
			pMain->DevInfo.Serial[7] = NRF_FICR->DEVICEID[1] / 1 % 10 + 0x30;
			pMain->DevInfo.Serial[8] = 0x00;
			pMain->DevInfo.Serial[9] = 0x00;
			}
		}

if(pMain->DevInfo.Serial[3] > 0x40)
	{
	pMain->DevInfo.Serial[3] -= 5;
	memcpy(pMain->BLE.DeviceName,DeviceSoftName,sizeof(DeviceSoftName));
	memcpy(&pMain->BLE.DeviceName[strlen(Main.BLE.DeviceName)],Main.DevInfo.Serial,strlen(Main.DevInfo.Serial));
	Main.BLE.DeviceName[strlen(Main.BLE.DeviceName)] = '-';
	memcpy(&pMain->BLE.DeviceName[strlen(Main.BLE.DeviceName)],stVersionString,strlen(stVersionString));
	pMain->DevInfo.Serial[3] += 5;
	}
else
	{
	memcpy(pMain->BLE.DeviceName,DeviceSoftName,sizeof(DeviceSoftName));
	memcpy(&pMain->BLE.DeviceName[strlen(Main.BLE.DeviceName)],Main.DevInfo.Serial,strlen(Main.DevInfo.Serial));
	Main.BLE.DeviceName[strlen(Main.BLE.DeviceName)] = '-';
	memcpy(&pMain->BLE.DeviceName[strlen(Main.BLE.DeviceName)],stVersionString,strlen(stVersionString));
	}
if(SerialUICRDataTemp == 0xffffffffffffffff)
	{
	Main.DevInfo.RawSerial = NRF_FICR->DEVICEID[1] % 100000000;
	}
else
	{
	Main.DevInfo.RawSerial = SerialUICRDataTemp;
	}
}



void OTQV2_TWIInit(stMain *pMain)
{
    Init_TWI();                           // twi peripheral init
    Init_EEPROM(pMain,&m_twi);            // eeprom twi handler init
    SHTC3_TWIInit(pMain,&m_twi);   // temperature sensor handler init
    //Init_MC3479(pMain,&m_twi_sensor);     // accelerometer handler init & register init
}

void OTQV2_GetMac(stMain *pMain)
{
ble_gap_addr_t stGetMac;
memset(&stGetMac,0x00,sizeof(stGetMac));
sd_ble_gap_addr_get(&stGetMac);
memcpy(pMain->BLE.MAC,stGetMac.addr,6);
}
	
void OTQV2_WriteMac(stMain *pMain)
{
uint32 err_ret;
ble_gap_addr_t stSetMac;
memset(&stSetMac,0x00,sizeof(stSetMac));
sd_ble_gap_addr_get(&stSetMac);

stSetMac.addr_type = BLE_GAP_ADDR_TYPE_PUBLIC;

memcpy(stSetMac.addr,pMain->BLE.MAC,6);

err_ret = sd_ble_gap_addr_set(&stSetMac);

APP_ERROR_CHECK(err_ret);

}

void OTQV2_MACProcess(stMain *pMain)
{
uint32 SerialTemp = 0;

if(pMain->BLE.Serial)
	{
	OTQV2_WriteMac(pMain);
	}
else
	{
	#if 1						// 시리얼번호 32비트 선언시
	OTQV2_AsciitoSerial(pMain);
	OTQV2_WriteMac(pMain);
	#else
	OTQV2_GetMac(pMain);				// 24.01.08 UUID기반 시리얼도 맥으로 지정
	SerialTemp =			pMain->DevInfo.Serial[0] - 0x30 * 10000000 + 
										pMain->DevInfo.Serial[1] - 0x30 * 1000000 + 
										pMain->DevInfo.Serial[2] - 0x30 * 100000 +
										pMain->DevInfo.Serial[3] - 0x30 * 10000+
										pMain->DevInfo.Serial[4] - 0x30 * 1000+
										pMain->DevInfo.Serial[5] - 0x30 * 100+
										pMain->DevInfo.Serial[6] - 0x30 * 10+
										pMain->DevInfo.Serial[7] - 0x30 * 1;
	
	pMain->BLE.MAC[2] = (SerialTemp >> 24) & 0xff;
	pMain->BLE.MAC[3] = (SerialTemp >> 16) & 0xff;
	pMain->BLE.MAC[4] = (SerialTemp >> 8) & 0xff;
	pMain->BLE.MAC[5] = (SerialTemp) & 0xff;
	
	OTQV2_WriteMac(pMain);
	#endif
	}
}


void OTQV2_Init_RNG(stMain *pMain)
{
uint32_t err_code;
err_code = nrf_drv_rng_init(NULL);
APP_ERROR_CHECK(err_code);
}

ret_code_t update_timer_init(void);

void OTQV2_Init(stMain *pMain)
{
		MCU_Init(pMain);
		timers_init();
		Board_Init();            
		OTQV2_MainStructure_Init(pMain);
		nrfx_systick_init();
    SysTick_Config(SystemCoreClock / 1000UL);  //1000UL  ms 
    //NVIC_EnableIRQ(SysTick_IRQn);              //systick    ͷ Ʈ Ȱ  
    power_management_init();
		OTQV2_TWIInit(pMain);
		OTQV2_BringSerialNumber_UICR(pMain);
		ble_OTQV2_InitBleHandler(&m_OTQV2_service);
		OTQV2_BLE_Init(pMain);
    OTQV2_HF_Start();
    OTQV2_GPIO_Init();
		ButtonPushTimer_Init();
		ButtonReleaseTimer_Init();
		OTQV2_BackupSerialNumber(pMain);
		update_timer_init();
		OTQV2_NFC_Init(pMain);
		OTQV2_Init_RNG(pMain);

		OTQV2_SAADC_Init_Probe();
		HAL_Delay(500);
		OTQV2_SAADC_UnInit();
    OTQV2_SAADC_Init_Battery();
	while(0)
		{
		pMain->Device.BatVolt = Batt_GetVoltage_int(pMain);
		HAL_Delay(300);
		}
}


	

void OTQV2_BLE_Disconnect(void)
{
NRF_LOG_INFO("BLE Disconnect");
ADV_RunFlag = 0;
ret_code_t err_code;
err_code = sd_ble_gap_disconnect(m_conn_handle,
																 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
//APP_ERROR_CHECK(err_code);
}


void start_update_timer(void)
{
    ret_code_t err_code;
    OTQV2_SystemWakeUp();

if(Main.Device.RunningStatus == RunStat_SelfTest2_Connected)
	{
		Main.BLE.CallStartPoint = 0;
		Main.BLE.CallLastPoint = 2;
	}		
    //data_index = 0;
			data_index = Main.BLE.CallLastPoint;
			DataLength = Main.BLE.CallLastPoint;
			if(!DataLength)
				{
				EEPROM_GetDataLength(&DataLength);
				data_index = DataLength;
				}
    NRF_LOG_INFO("start address : %d    data length : %d", data_index,DataLength);        
    err_code = app_timer_start(m_update_timer, APP_TIMER_TICKS(DATA_UPDATE_INTERVAL), NULL);
    APP_ERROR_CHECK(err_code);    

}

void stop_update_timer(void)
{
    ret_code_t err_code;
    err_code = app_timer_stop(m_update_timer);
    APP_ERROR_CHECK(err_code);    

}

#if 1		// david test code for ble data transfer rate
#define StructureNumberInPacket 10
EEPROM_Datastructure TempData[StructureNumberInPacket];

void BLE_SetAddress(stMain *pMain)
{
#if 1					// ascii to hex

	uint8 StartAddress[10],LastAddress[10];
	uint8 DividePoint = 0;
	/*
	if(pMain->BLE.ReceiveData[0] == 0x1234
		{
				OTQV2_WriteSerialToUicr(pMain,5234000123);
		}
	*/				// 시리얼번호 재설정 기능 구현
	//if(pMain->BLE.ReceiveData[0] > 0x2f)			// ascii receive
	if(0)			// ascii disabled
		{
		memset(StartAddress,0x00,10);
		memset(LastAddress,0x00,10);

		for(int i=0;i<100;i++)
		{
			if(pMain->BLE.ReceiveData[i] == ',')
			{
			DividePoint = i;
			}
		}
		memcpy(StartAddress,pMain->BLE.ReceiveData,DividePoint);
		memcpy(LastAddress,&pMain->BLE.ReceiveData[DividePoint+1],strlen(pMain->BLE.ReceiveData)-DividePoint-1);

		pMain->BLE.CallStartPoint = 0;
		pMain->BLE.CallLastPoint = 0;

		for(int i=0;i<strlen(StartAddress);i++)
			{
			pMain->BLE.CallStartPoint *= 10;
			pMain->BLE.CallStartPoint += StartAddress[i] - 0x30;
			}

		for(int i=0;i<strlen(LastAddress);i++)
			{
			pMain->BLE.CallLastPoint *= 10;
			pMain->BLE.CallLastPoint += LastAddress[i] - 0x30;
			}
		}
	else																			// hex receive
		{
		pMain->BLE.CallStartPoint = 0;
		pMain->BLE.CallLastPoint = 0;
		pMain->BLE.CallStartPoint = pMain->BLE.ReceiveData[0] << 8 | pMain->BLE.ReceiveData[1];
		pMain->BLE.CallLastPoint = (pMain->BLE.ReceiveData[2] << 8 | pMain->BLE.ReceiveData[3])+1;
		}
if(pMain->BLE.CallLastPoint > pMain->Sensor.DataNumber)		// 데이터 요청 마지막 주소가 최대값 이상인 경우
	{
	pMain->BLE.CallLastPoint = pMain->Sensor.DataNumber;
	}
if(pMain->BLE.CallStartPoint > pMain->Sensor.DataNumber)	// 데이터 요청  첫 주소가 최대값 이상인 경우
	{
	pMain->BLE.CallStartPoint = pMain->Sensor.DataNumber;
	}
if(pMain->BLE.CallStartPoint < pMain->BLE.CallStartPoint - MaxEEPROMAddress)	// 요청 범위가 최대범위 이상일 경우
	{
	pMain->BLE.CallStartPoint = pMain->BLE.CallStartPoint - MaxEEPROMAddress;		// 최대범위 기준으로 변경
	}
if(pMain->BLE.CallStartPoint == 0)				// 0번 데이터 사용 금지
	{
	pMain->BLE.CallStartPoint = 1;
	}

	pMain->BLE.SendStartPoint = 0xffff;
	pMain->BLE.SendLastPoint = 0;
	pMain->BLE.SendBlockCount = 0;
#else
pMain->BLE.CallStartPoint = 0;
pMain->BLE.CallLastPoint = 0;
	pMain->BLE.CallStartPoint = pMain->BLE.ReceiveData[0] << 8 | pMain->BLE.ReceiveData[1];
	pMain->BLE.CallLastPoint = pMain->BLE.ReceiveData[2] << 8 | pMain->BLE.ReceiveData[3];
#endif
}

#if 0			// 1 = increase index , 0 = decrease index
void BLEActive_Handler(stMain *pMain)
{
    ret_code_t err_code;
		uint8 ChipDataLength = DataLength % StructureNumberInPacket;
    if(data_index <= DataLength-StructureNumberInPacket) 
    {
        memset(&TempData, 1, sizeof(EEPROM_Datastructure)*StructureNumberInPacket);
        EEPROM_ReadData((uint8 *) &TempData[0], (data_index % MaxEEPROMAddress )*sizeof(EEPROM_Datastructure) , sizeof(EEPROM_Datastructure)*StructureNumberInPacket);
				HAL_Delay(2);
        //NRF_LOG_INFO("[%10d] time : %ld, index : %d, PCBTemp : %d %d %d",getTickCount(), TempData[0].TimeStamp_UTC, TempData[0].Index, TempData[0].Sensor.PCBTemp , TempData[1].Sensor.PCBTemp , TempData[2].Sensor.PCBTemp);
        if(TempData[0].Index == 0xFFFF)
        {
            //NRF_LOG_ERROR("Invalid Data....tranfer stoped");
            data_index = 0;
            return;
        }

        if(!ble_OTQV2_value_update(&m_OTQV2_service, (uint8 *)&TempData[0], sizeof(EEPROM_Datastructure)*StructureNumberInPacket))
					{
					data_index +=StructureNumberInPacket;
					//data_index = DataLength;
					}
        
        err_code = app_timer_start(m_update_timer, APP_TIMER_TICKS(DATA_UPDATE_INTERVAL), NULL);
        APP_ERROR_CHECK(err_code);
    }
    else
    {
				if(ChipDataLength)
					{
					memset(TempData,0x00,sizeof(TempData));
					HAL_Delay(1);
					EEPROM_ReadData((uint8 *) &TempData[0], (data_index % MaxEEPROMAddress )*sizeof(EEPROM_Datastructure) , sizeof(EEPROM_Datastructure)*ChipDataLength);
					HAL_Delay(2);
					if(!ble_OTQV2_value_update(&m_OTQV2_service, (uint8 *)&TempData[0], sizeof(EEPROM_Datastructure)*StructureNumberInPacket))
						{
						data_index +=ChipDataLength;
						//data_index = DataLength;
						}
					HAL_Delay(1);
					}

        NRF_LOG_INFO("%d Data Transfer is completed", data_index);
        data_index = 0;
        update_enable = false;
				OTQV2_BLE_Disconnect();
    }
}
#else

uint8 BLE_PacketValidation(EEPROM_Datastructure *Data, uint16 StartIndex)
{
	for(int i=0 ; i<StructureNumberInPacket ; i++)
		{
		if(Data[i].Index >= StartIndex + StructureNumberInPacket || Data[i].Index < StartIndex)
			{
			if(Data[i].Index)
				{
				NRF_LOG_INFO("[%10d] packet validation fail : index %d",getTickCount() , StartIndex + i);
				return 0;
				}
			}
		}
	NRF_LOG_INFO("[%10d] packet validation success: index %d",getTickCount() , StartIndex);
	return 1;
}

void BLEActive_Handler_Setting(stMain *pMain)
{
stBleSettingWriteData *WriteData = (stBleSettingWriteData*)pMain->BLE.ReceiveData;
OTQV2_SystemWakeUp();

NRF_LOG_INFO("[%10d] Setting Handler Detected",getTickCount());
	if(pMain->DeviceConfig.Interval != WriteData->Interval)
		{
		NRF_LOG_INFO("[%10d] Record interval changed!! %d to %d",getTickCount() , pMain->DeviceConfig.Interval , WriteData->Interval);
		pMain->DeviceConfig.Interval = WriteData->Interval;
		OTQV2_ApplyInterval(pMain);
		EEPROM_WriteInterval(&pMain->Sensor.SensorIntervalSec);
		EEPROM_WriteDataLength(0);
		}
	pMain->DeviceConfig.DisplayInterval = WriteData->DisplayInterval;
	pMain->Flags.CallBleSleepFlag = WriteData->SleepFlag;
	
	
	OTQV2_ApplyDisplayInterval(pMain);
	EEPROM_WriteDisplayInterval(&pMain->Device.DisplayIntervalSec);
	OTQV2_BLE_ApplySettingData(pMain);
	#ifdef DEBUG
	EPDPage_BLE_SettingApply(pMain);
	if(pMain->Flags.CallBleSleepFlag)
		{
		#ifdef BLESleepEnable
		BLEIntervalTimer_Sleep(3600 * 6);		// 6hour ble sleep
		#endif
		}
	#else
	EPDPage_BLE_ReadyToService(pMain);
	EPDPage_Service_Standby(pMain);
	if(pMain->Flags.CallBleSleepFlag)
		{
		BLEIntervalTimer_Sleep(3600 * 6);		// 6hour ble sleep
		}
	#endif
		
	Main.Key.IntervalProcess_CallFlag = 1;		// record data after index clear
OTQV2_SystemSleep(&Main);
DeInit_TWI();
}

void BLEActive_Handler_DataSend(stMain *pMain)
{
    ret_code_t err_code;
		uint32 ret = 0;
		//uint8 ChipDataLength = DataLength % StructureNumberInPacket;
		EEPROM_Datastructure TempDataBuffer[StructureNumberInPacket];
		static uint8 ChipDataLength = 0;
		static uint8 FinalDataFlag = 0;
		
		pMain->BLE.SendBlockCount++;
		
		memset(TempDataBuffer,0x00,sizeof(EEPROM_Datastructure) * StructureNumberInPacket);
		if((pMain->BLE.CallLastPoint - pMain->BLE.CallStartPoint) < StructureNumberInPacket)  		// 요구데이터가 최소단위 이하일때
			{
			FinalDataFlag = 1;
			ChipDataLength = pMain->BLE.CallLastPoint - pMain->BLE.CallStartPoint;
			if(data_index > StructureNumberInPacket) data_index -= StructureNumberInPacket;
			pMain->BLE.SendStartTick = getTickCount();
			}
		else if((data_index > StructureNumberInPacket) && (data_index == DataLength))	// 첫데이터 이고 최소단위 이상
			{
			data_index -= StructureNumberInPacket;
			pMain->BLE.SendStartTick = getTickCount();
			}
		else if(data_index == DataLength) 
			{
			FinalDataFlag = 1;		// 첫 데이터이고 최소단위 동일할때
			ChipDataLength = DataLength;
			pMain->BLE.SendStartTick = getTickCount();
			}
		if(!FinalDataFlag)
    {
				if((data_index % MaxEEPROMAddress ) > MaxEEPROMAddress - StructureNumberInPacket)
					{
					for(int i=0 ; i<StructureNumberInPacket ; i++)
						{
						EEPROM_ReadData((uint8 *) &TempData[i], ((data_index + i) % MaxEEPROMAddress )*sizeof(EEPROM_Datastructure) , sizeof(EEPROM_Datastructure));
						if(TempData[i].Index == 0xffff) TempData[i].Index = 0;
						}
					}
				else
					{
					EEPROM_ReadData((uint8 *) &TempData[0], (data_index % MaxEEPROMAddress )*sizeof(EEPROM_Datastructure) , sizeof(EEPROM_Datastructure)*StructureNumberInPacket);
					//EEPROM_ReadData((uint8 *) &TempData[0], ((data_index + StructureNumberInPacket - ChipDataLength) % MaxEEPROMAddress )*sizeof(EEPROM_Datastructure) , sizeof(EEPROM_Datastructure)*(ChipDataLength));
					}
				HAL_Delay(2);
				//memcpy(TempDataBuffer,TempData,sizeof(EEPROM_Datastructure) * StructureNumberInPacket);
				for(int i=0;i<StructureNumberInPacket;i++)			// data reverse
					{
					memcpy(&TempDataBuffer[i],&TempData[StructureNumberInPacket - 1 - i],sizeof(EEPROM_Datastructure));
					}
        //NRF_LOG_INFO("[%10d] time : %ld, index : %d, PCBTemp : %d %d %d",getTickCount(), TempData[0].TimeStamp_UTC, TempData[0].Index, TempData[0].Sensor.PCBTemp , TempData[1].Sensor.PCBTemp , TempData[2].Sensor.PCBTemp);
        if(TempData[0].Index == 0xFFFF)
					{
            //NRF_LOG_ERROR("Invalid Data....tranfer stoped");
            data_index = 0;
						OTQV2_BLE_Disconnect();
            return;
					}
				if((DataLength > 7200) && (data_index < DataLength - 7200))
					{
            data_index = 0;
						OTQV2_BLE_Disconnect();
            return;
					}

		    if(data_index <= pMain->BLE.CallStartPoint + StructureNumberInPacket)
					{
					ChipDataLength = data_index - pMain->BLE.CallStartPoint;
					FinalDataFlag = 1;
					}
				if(!BLE_PacketValidation(TempDataBuffer , data_index))
					{
					OTQV2_BLE_Disconnect();
					data_index = 0;
					return;
					}
        else if(!(ble_OTQV2_value_update(&m_OTQV2_service, (uint8 *)&TempDataBuffer[0], sizeof(EEPROM_Datastructure)*StructureNumberInPacket)))
					{
					for(int i=0 ; i<StructureNumberInPacket; i++)
						{
						if(pMain->BLE.SendStartPoint > TempDataBuffer[i].Index) pMain->BLE.SendStartPoint = TempDataBuffer[i].Index;
						if(pMain->BLE.SendLastPoint < TempDataBuffer[i].Index && TempDataBuffer[i].Index >= 1) pMain->BLE.SendLastPoint = TempDataBuffer[i].Index;
						}
					if(data_index >= StructureNumberInPacket)
						{
						data_index -= StructureNumberInPacket;
						}
					if(!pMain->BLE.SendBlockCount%1000)		// delay for prevent BLE packet vanishing
						{
						HAL_Delay(100);
						}
					//data_index = DataLength;
					}

        err_code = app_timer_start(m_update_timer, APP_TIMER_TICKS(DATA_UPDATE_INTERVAL), NULL);
        APP_ERROR_CHECK(err_code);
    }
    else
    {
				if(ChipDataLength)
					{
					memset(TempData,0x00,sizeof(TempData));
					HAL_Delay(1);
					if(data_index > StructureNumberInPacket)
						{
						if((data_index % MaxEEPROMAddress ) > MaxEEPROMAddress - StructureNumberInPacket)
							{
							for(int i=0 ; i<ChipDataLength ; i++)
								{
								EEPROM_ReadData((uint8 *) &TempData[i], ((data_index + (StructureNumberInPacket - ChipDataLength) + i) % MaxEEPROMAddress )*sizeof(EEPROM_Datastructure) , sizeof(EEPROM_Datastructure));
								}
							}
						else
							{
							EEPROM_ReadData((uint8 *) &TempData[0], ((data_index + StructureNumberInPacket - ChipDataLength) % MaxEEPROMAddress )*sizeof(EEPROM_Datastructure) , sizeof(EEPROM_Datastructure)*(ChipDataLength));
							}
						}
					else if ((data_index < pMain->BLE.CallStartPoint))
						{
						if((data_index % MaxEEPROMAddress ) > MaxEEPROMAddress - StructureNumberInPacket)
							{
							for(int i=0 ; i<ChipDataLength ; i++)
								{
								EEPROM_ReadData((uint8 *) &TempData[i], ((data_index + (StructureNumberInPacket - ChipDataLength) + i) % MaxEEPROMAddress )*sizeof(EEPROM_Datastructure) , sizeof(EEPROM_Datastructure));
								}
							}
						else
							{
							EEPROM_ReadData((uint8 *) &TempData[0], ((data_index + StructureNumberInPacket - ChipDataLength) % MaxEEPROMAddress )*sizeof(EEPROM_Datastructure) , sizeof(EEPROM_Datastructure)*(ChipDataLength));
							}
						}
					else
						{
						if((data_index % MaxEEPROMAddress ) > MaxEEPROMAddress - StructureNumberInPacket)
							{
							for(int i=0 ; i<ChipDataLength ; i++)
								{
								EEPROM_ReadData((uint8 *) &TempData[i], ((data_index + (StructureNumberInPacket - ChipDataLength) + i) % MaxEEPROMAddress )*sizeof(EEPROM_Datastructure) , sizeof(EEPROM_Datastructure));
								}
							}
						else if (data_index >= ChipDataLength)
							{
							EEPROM_ReadData((uint8 *) &TempData[0], ((data_index - ChipDataLength) % MaxEEPROMAddress )*sizeof(EEPROM_Datastructure) , sizeof(EEPROM_Datastructure)*(ChipDataLength));
							data_index = data_index - ChipDataLength;
							}
						else								
							{
							EEPROM_ReadData((uint8 *) &TempData[0], ((data_index + StructureNumberInPacket - ChipDataLength) % MaxEEPROMAddress )*sizeof(EEPROM_Datastructure) , sizeof(EEPROM_Datastructure)*(ChipDataLength));
							}
						}
					HAL_Delay(2);
					//memcpy(TempDataBuffer,TempData,sizeof(EEPROM_Datastructure) * StructureNumberInPacket);
					for(int i=0;i<ChipDataLength;i++)			// data reverse
						{
						memcpy(&TempDataBuffer[i],&TempData[StructureNumberInPacket - 1 - i - (StructureNumberInPacket - ChipDataLength)],sizeof(EEPROM_Datastructure));
						}
					if(!BLE_PacketValidation(TempDataBuffer , data_index))
						{
						NRF_LOG_INFO("%d Data Transfer fail", data_index);
						data_index = 0;
						update_enable = false;
						FinalDataFlag = 0;
						pMain->BLE.SendLastTick = getTickCount();
						OTQV2_BLE_Disconnect();
						OTQV2_SystemWakeUp();
						EPDPage_BLE_DataSendResult(&Main);
						OTQV2_SystemSleep(&Main);
						return;
						}
					else if(!ble_OTQV2_value_update(&m_OTQV2_service, (uint8 *)&TempDataBuffer[0], sizeof(EEPROM_Datastructure)*StructureNumberInPacket))
						{
						data_index -=ChipDataLength;
						for(int i=0 ; i<ChipDataLength; i++) 
							{
							if(pMain->BLE.SendStartPoint > TempDataBuffer[i].Index) pMain->BLE.SendStartPoint = TempDataBuffer[i].Index;
							if(pMain->BLE.SendLastPoint < TempDataBuffer[i].Index && TempDataBuffer[i].Index >= 1) pMain->BLE.SendLastPoint = TempDataBuffer[i].Index;
							}

						//data_index = DataLength;
						}
					HAL_Delay(1);
					}

        NRF_LOG_INFO("%d Data Transfer is completed", data_index);
        data_index = 0;
        update_enable = false;
				FinalDataFlag = 0;
				pMain->BLE.SendLastTick = getTickCount();
				//OTQV2_BLE_Disconnect();
				OTQV2_SystemWakeUp();
				EPDPage_BLE_DataSendResult(&Main);
				OTQV2_SystemSleep(&Main);
    }
}
#endif


void BLEActive_Handler(stMain *pMain)
{
NRF_LOG_INFO("[%10d] BLE Handler Detected. Handler : %x",getTickCount(),Main.Flags.BLEGattHandler);
	switch(Main.Flags.BLEGattHandler)
		{
		case OTQV2_SERVICE_CHAR_UUID:
		BLEActive_Handler_DataSend(&Main);
			break;

		case OTQV2_SERVICE_CHAR_WRITE_UUID:
		BLEActive_Handler_Setting(&Main);
			break;
		}
}



static void update_timer_handler(void * p_context)
{
	Main.Flags.BLEActiveFlag = 1;
}
#else
static void update_timer_handler(void * p_context)
{
    ret_code_t err_code;
    if(data_index < DataLength) 
    {
        EEPROM_Datastructure TempData;
        memset(&TempData, 0, sizeof(EEPROM_Datastructure));
        EEPROM_ReadData((uint8 *) &TempData, (data_index % MaxEEPROMAddress )*sizeof(TempData) , sizeof(TempData));
        //NRF_LOG_INFO("time : %ld, index : %d, PCBTemp : %d", TempData.TimeStamp_UTC, TempData.Index, TempData.Sensor.PCBTemp);
        if(TempData.Index == 0xFFFF)
        {
            //NRF_LOG_ERROR("Invalid Data....tranfer stoped");
            data_index = 0;
            return;
        }

        ble_OTQV2_value_update(&m_OTQV2_service, (void *)&TempData, sizeof(EEPROM_Datastructure));
        data_index ++;
        err_code = app_timer_start(m_update_timer, APP_TIMER_TICKS(DATA_UPDATE_INTERVAL), NULL);
        APP_ERROR_CHECK(err_code);
    }
    else
    {
        NRF_LOG_INFO("%d Data Transfer is completed", data_index);
        data_index = 0;
        update_enable = false;
    }
}
#endif


ret_code_t update_timer_init(void)
{
    ret_code_t err_code;
    err_code = app_timer_create(&m_update_timer, APP_TIMER_MODE_SINGLE_SHOT, update_timer_handler);
    APP_ERROR_CHECK(err_code);    
    return err_code;
}

	
void OTQV2_SerialQR_EnableOffset(stMain *pMain)
{
	pMain->Flags.SerialQR_OffsetViewFlag = 0;
}

void OTQV2_SerialQR_DisableOffset(stMain *pMain)
{
	pMain->Flags.SerialQR_OffsetViewFlag = 1;
}
