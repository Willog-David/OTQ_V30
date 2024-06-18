
#ifndef OTQV2_INIT_H__
#define OTQV2_INIT_H__

#include "EEPROM.h"


#include "struct.h"
#include "app_error.h"
#include "nrf_sdh.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh_ble.h"
#include "ble_lbs.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"
#include "boards.h"
#include "app_timer.h"
#include "app_button.h"
#include "struct.h"

#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh_ble.h"
#include "ble_lbs.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"

void OTQV2_SystemSleep(stMain *pMain);
void OTQV2_GPIO_Init(void);
void OTQV2_SystemWakeUp(void);
void OTQV2_SAADC_Init(void);
void BLEActive_Handler(stMain *pMain);
void UpdateBleAdvertisePacket(stMain *pMain);

extern nrf_drv_twi_t  m_twi;
//extern nrf_drv_twi_t m_twi;
void buttons_init(void);
void advertising_start(stMain *pMain);
void advertising_stop(void);



// BSP define
//#define ADVERTISING_LED                 BSP_BOARD_LED_0                         /**< Is on when device is advertising. */
//#define CONNECTED_LED                   BSP_BOARD_LED_1                         /**< Is on when device has connected. */
//#define LEDBUTTON_LED                   BSP_BOAR+D_LED_2                         /**< LED to be toggled with the help of the LED Button Service. */
//#define LEDBUTTON_BUTTON                BSP_BUTTON_0                            /**< Button that will trigger the notification event with the LED Button Service */

// BSP define done


// BLE define

#define DEVICE_NAME                     "OTQ V2"                         /**< Name of device. Will be included in the advertising data. */

#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                       /**< A tag identifying the SoftDevice BLE configuration. */

//#define APP_ADV_INTERVAL                64                                      /**< The advertising interval (in units of 0.625 ms; this value corresponds to 40 ms). */
#define APP_ADV_INTERVAL                160      // 100mS interval                                      
//#define APP_ADV_INTERVAL                1600      // 1000mS interval                                      
#define APP_ADV_DURATION                BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED   /**< The advertising time-out (in units of seconds). When set to 0, we will never time out. */


#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(50, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory time-out (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(20000)                  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000)                   /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                       /**< Number of attempts before giving up the connection parameter negotiation. */

#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50)                     /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define DEAD_BEEF                       0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */


BLE_LBS_DEF(m_lbs);                                                             /**< LED Button Service instance. */
NRF_BLE_GATT_DEF(m_gatt);                                                       /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                         /**< Context for the Queued Write module.*/

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN   
static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;                   /**< Advertising handle used to identify an advertising set. */
static uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];                    /**< Buffer for storing an encoded advertising set. */
static uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX];         /**< Buffer for storing an encoded scan data. */


// BLe define done

//-- GPIO define
#define OTQV2_GPIO_ADC_PROBE_EN (0+2)
#define OTQV2_GPIO_ADC_PROBE NRF_SAADC_INPUT_AIN2 // P0.4

#define OTQV2_GPIO_ADC_BATT_EN (0+19)
#define OTQV2_GPIO_ADC_BATT NRF_SAADC_INPUT_AIN1 // P0.3

#define OTQV2_GPIO_EPD_BUSY     EPD_GPIO_BUSY
#define OTQV2_GPIO_EPD_CLK      EPD_GPIO_CLK
#define OTQV2_GPIO_EPD_DA       EPD_GPIO_DA
#define OTQV2_GPIO_EPD_CS       EPD_GPIO_CS
#define OTQV2_GPIO_EPD_RST      EPD_GPIO_RST
#define OTQV2_GPIO_EPD_DC       EPD_GPIO_DC

#define EPD_GPIO_BUSY (0+16)    // P0.2
#define EPD_GPIO_CLK  (0+30)    // P0.3
#define EPD_GPIO_DA   (0+31)  // P1.12
#define EPD_GPIO_CS   (0+28)  // P1.13
#define EPD_GPIO_RST  (0+27)  // P1.14
#define EPD_GPIO_DC   (0+29)  // P1.15

#define OTQV2_GPIO_TWI_SENSOR_SCL (0+6)
#define OTQV2_GPIO_TWI_SENSOR_SDA (0+7)

#define OTQV2_GPIO_LED_R (0+8)
#define OTQV2_GPIO_LED_G (0+11)
#define OTQV2_GPIO_LED_B (0+12)

#define OTQV2_GPIO_SDI_SCLK (0+13)
#define OTQV2_GPIO_SDI_MISO (0+14)
#define OTQV2_GPIO_SDI_MOSI (0+15)

#define OTQV2_GPIO_INT (0+17)		// include GPIO expander, ACC, RTC

#define OTQV2_GPIO_EXP_RST (0+18)		// GPIO expander rst

#define OTQV2_GPIO_KEY_RIGHT (0+20)
#define OTQV2_GPIO_KEY_LEFT (0+22)

#define OTQV2_GPIO_OLED_VPP_EN (0+23)		// OLED VPP enable
#define OTQV2_GPIO_LCD_PWR_EN (0+24)		// LCD & EPD power enable
#define OTQV2_GPIO_LCD_RST EPD_GPIO_RST


#define OTQV2_GPIO_SENSOR_POWER (0+21) // P0.21

#define OTQV2_GPIO_NRF_UART_TX (0+25)
#define OTQV2_GPIO_NRF_UART_RX (0+26)




//-- GPIO define done


void OTQV2_SAADC_UnInit(void);
void OTQV2_SAADC_Init_Battery(void);
void OTQV2_SAADC_Init_Probe(void);
void Board_Init(void);
void BLE_Init(void);
void Init_TWI(void);
void OTQV2_Init(stMain *pMain);
void OTQV2_MACProcess(stMain *pMain);

//-- BSP define

#define BSP_INDICATIONS_LIST {                \
        "BSP_INDICATE_IDLE",                  \
        "BSP_INDICATE_SCANNING",              \
        "BSP_INDICATE_ADVERTISING",           \
        "BSP_INDICATE_ADVERTISING_WHITELIST", \
        "BSP_INDICATE_ADVERTISING_SLOW",      \
        "BSP_INDICATE_ADVERTISING_DIRECTED",  \
        "BSP_INDICATE_BONDING",               \
        "BSP_INDICATE_CONNECTED",             \
        "BSP_INDICATE_SENT_OK",               \
        "BSP_INDICATE_SEND_ERROR",            \
        "BSP_INDICATE_RCV_OK",                \
        "BSP_INDICATE_RCV_ERROR",             \
        "BSP_INDICATE_FATAL_ERROR",           \
        "BSP_INDICATE_ALERT_0",               \
        "BSP_INDICATE_ALERT_1",               \
        "BSP_INDICATE_ALERT_2",               \
        "BSP_INDICATE_ALERT_3",               \
        "BSP_INDICATE_ALERT_OFF",             \
        "BSP_INDICATE_USER_STATE_OFF",        \
        "BSP_INDICATE_USER_STATE_0",          \
        "BSP_INDICATE_USER_STATE_1",          \
        "BSP_INDICATE_USER_STATE_2",          \
        "BSP_INDICATE_USER_STATE_3",          \
        "BSP_INDICATE_USER_STATE_ON"          \
} /**< See @ref examples_bsp_states for a list of how these states are indicated for the PCA10028/PCA10040 board and the PCA10031 dongle.*/


/**@brief BSP indication states.
 *
 * @details See @ref examples_bsp_states for a list of how these states are indicated for the PCA10028/PCA10040 board and the PCA10031 dongle.
 */
typedef enum
{
    BSP_INDICATE_FIRST = 0,
    BSP_INDICATE_IDLE  = BSP_INDICATE_FIRST, /**< See \ref BSP_INDICATE_IDLE.*/
    BSP_INDICATE_SCANNING,                   /**< See \ref BSP_INDICATE_SCANNING.*/
    BSP_INDICATE_ADVERTISING,                /**< See \ref BSP_INDICATE_ADVERTISING.*/
    BSP_INDICATE_ADVERTISING_WHITELIST,      /**< See \ref BSP_INDICATE_ADVERTISING_WHITELIST.*/
    BSP_INDICATE_ADVERTISING_SLOW,           /**< See \ref BSP_INDICATE_ADVERTISING_SLOW.*/
    BSP_INDICATE_ADVERTISING_DIRECTED,       /**< See \ref BSP_INDICATE_ADVERTISING_DIRECTED.*/
    BSP_INDICATE_BONDING,                    /**< See \ref BSP_INDICATE_BONDING.*/
    BSP_INDICATE_CONNECTED,                  /**< See \ref BSP_INDICATE_CONNECTED.*/
    BSP_INDICATE_SENT_OK,                    /**< See \ref BSP_INDICATE_SENT_OK.*/
    BSP_INDICATE_SEND_ERROR,                 /**< See \ref BSP_INDICATE_SEND_ERROR.*/
    BSP_INDICATE_RCV_OK,                     /**< See \ref BSP_INDICATE_RCV_OK.*/
    BSP_INDICATE_RCV_ERROR,                  /**< See \ref BSP_INDICATE_RCV_ERROR.*/
    BSP_INDICATE_FATAL_ERROR,                /**< See \ref BSP_INDICATE_FATAL_ERROR.*/
    BSP_INDICATE_ALERT_0,                    /**< See \ref BSP_INDICATE_ALERT_0.*/
    BSP_INDICATE_ALERT_1,                    /**< See \ref BSP_INDICATE_ALERT_1.*/
    BSP_INDICATE_ALERT_2,                    /**< See \ref BSP_INDICATE_ALERT_2.*/
    BSP_INDICATE_ALERT_3,                    /**< See \ref BSP_INDICATE_ALERT_3.*/
    BSP_INDICATE_ALERT_OFF,                  /**< See \ref BSP_INDICATE_ALERT_OFF.*/
    BSP_INDICATE_USER_STATE_OFF,             /**< See \ref BSP_INDICATE_USER_STATE_OFF.*/
    BSP_INDICATE_USER_STATE_0,               /**< See \ref BSP_INDICATE_USER_STATE_0.*/
    BSP_INDICATE_USER_STATE_1,               /**< See \ref BSP_INDICATE_USER_STATE_1.*/
    BSP_INDICATE_USER_STATE_2,               /**< See \ref BSP_INDICATE_USER_STATE_2.*/
    BSP_INDICATE_USER_STATE_3,               /**< See \ref BSP_INDICATE_USER_STATE_3.*/
    BSP_INDICATE_USER_STATE_ON,              /**< See \ref BSP_INDICATE_USER_STATE_ON.*/
    BSP_INDICATE_LAST = BSP_INDICATE_USER_STATE_ON
} bsp_indication_t;

/**@brief BSP events.
 *
 * @note Events from BSP_EVENT_KEY_0 to BSP_EVENT_KEY_LAST are by default assigned to buttons.
 */
typedef enum
{
    BSP_EVENT_NOTHING = 0,                  /**< Assign this event to an action to prevent the action from generating an event (disable the action). */
    BSP_EVENT_DEFAULT,                      /**< Assign this event to an action to assign the default event to the action. */
    BSP_EVENT_CLEAR_BONDING_DATA,           /**< Persistent bonding data should be erased. */
    BSP_EVENT_CLEAR_ALERT,                  /**< An alert should be cleared. */
    BSP_EVENT_DISCONNECT,                   /**< A link should be disconnected. */
    BSP_EVENT_ADVERTISING_START,            /**< The device should start advertising. */
    BSP_EVENT_ADVERTISING_STOP,             /**< The device should stop advertising. */
    BSP_EVENT_WHITELIST_OFF,                /**< The device should remove its advertising whitelist. */
    BSP_EVENT_BOND,                         /**< The device should bond to the currently connected peer. */
    BSP_EVENT_RESET,                        /**< The device should reset. */
    BSP_EVENT_SLEEP,                        /**< The device should enter sleep mode. */
    BSP_EVENT_WAKEUP,                       /**< The device should wake up from sleep mode. */
    BSP_EVENT_SYSOFF,                       /**< The device should enter system off mode (without wakeup). */
    BSP_EVENT_DFU,                          /**< The device should enter DFU mode. */
    BSP_EVENT_KEY_0,                        /**< Default event of the push action of BSP_BUTTON_0 (only if this button is present). */
    BSP_EVENT_KEY_1,                        /**< Default event of the push action of BSP_BUTTON_1 (only if this button is present). */
    BSP_EVENT_KEY_2,                        /**< Default event of the push action of BSP_BUTTON_2 (only if this button is present). */
    BSP_EVENT_KEY_3,                        /**< Default event of the push action of BSP_BUTTON_3 (only if this button is present). */
    BSP_EVENT_KEY_4,                        /**< Default event of the push action of BSP_BUTTON_4 (only if this button is present). */
    BSP_EVENT_KEY_5,                        /**< Default event of the push action of BSP_BUTTON_5 (only if this button is present). */
    BSP_EVENT_KEY_6,                        /**< Default event of the push action of BSP_BUTTON_6 (only if this button is present). */
    BSP_EVENT_KEY_7,                        /**< Default event of the push action of BSP_BUTTON_7 (only if this button is present). */
    BSP_EVENT_KEY_LAST = BSP_EVENT_KEY_7,
} bsp_event_t;
//-- BSP define end


#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256 

#endif