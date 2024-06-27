
#include "max31865.h"
#include "nrfx_spi.h"
#include "nrf_drv_spi.h"
#include "PCA9537.h"
#include "OTQV30_Init.h"

#include "string.h"

#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer*/


#define TEST_STRING "Nordic"
static uint8_t       m_tx_buf[10];           /**< TX buffer. */
static uint8_t       m_rx_buf[10];    /**< RX buffer. */
static const uint8_t m_length = sizeof(m_tx_buf);        /**< Transfer length. */

void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
	#if 0
	spi_xfer_done = true;
	NRF_LOG_INFO("Transfer completed.");
	NRF_LOG_INFO(" Received:");
	NRF_LOG_HEXDUMP_INFO(m_rx_buf, 10);
	#endif
}

void MAX31865_Init(stMain *pMain)
{
		nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = SPI_SS_PIN;
    spi_config.miso_pin = SPI_MISO_PIN;
    spi_config.mosi_pin = SPI_MOSI_PIN;
    spi_config.sck_pin  = SPI_SCK_PIN;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config,		spi_event_handler, NULL));

		        memset(m_rx_buf, 0, m_length);
        spi_xfer_done = false;

 nrf_gpio_pin_write(OTQV2_GPIO_ADC_PROBE_EN, 1);

				PCA9537_SetPort_RTDPWR_HIGH();
			PCA9537_SetPort_RTDCS_LOW();
				m_tx_buf[0] = 0x00;
        APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, m_rx_buf, 10));
				HAL_Delay(10);
			PCA9537_SetPort_RTDCS_HIGH();

			PCA9537_SetPort_RTDCS_LOW();
				m_tx_buf[0] = 0x80;
				m_tx_buf[1] = 0x60;
        APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 2, m_rx_buf, m_length));
			PCA9537_SetPort_RTDCS_HIGH();
nrf_gpio_pin_write(OTQV2_GPIO_ADC_PROBE_EN, 0);
}
// PT100 센서의 특성
#define R0 100.0  // 0도씨에서의 저항 값 (옴)
#define ALPHA 0.00385  // PT100의 온도 계수
#define ZERO_DEGREE_VALUE 8192  // 0도씨에서의 값

double HexToTemp(uint16 *Hex)
{
    // 16진수 문자열을 정수로 변환
    int adc_value = *Hex;
    
    // 저항 값 계산
    double resistance = R0 * ((double)adc_value / ZERO_DEGREE_VALUE);
    
    // 온도 계산
    double temperature = (resistance - R0) / (ALPHA * R0);
		
		return temperature;
}

	uint8 GPIOtmp = 0;
	double ProbeTemp = 0;
void MAX31865_Test(stMain *pMain)
{
uint16 HexTmp = 0;
memset(m_tx_buf,0x00,6);
				memset(m_rx_buf, 0, m_length);
 nrf_gpio_pin_write(OTQV2_GPIO_ADC_PROBE_EN, 1);
				PCA9537_SetPort_RTDPWR_HIGH();
			PCA9537_SetPort_RTDCS_LOW();
				m_tx_buf[0] = 0x80;
				m_tx_buf[1] = 0xE0;
        APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 2, m_rx_buf, m_length));
			PCA9537_SetPort_RTDCS_HIGH();
			PCA9537_SetPort_RTDCS_LOW();
				m_tx_buf[0] = 0x00;
        APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 2, m_rx_buf, 10));
				HAL_Delay(10);
			PCA9537_SetPort_RTDCS_HIGH();
			HexTmp = m_rx_buf[2] <<8 | m_rx_buf[3];
			HexTmp >>= 1;		// last bit = fault flag
 nrf_gpio_pin_write(OTQV2_GPIO_ADC_PROBE_EN, 0);
 
 ProbeTemp = HexToTemp(&HexTmp);
 pMain->Sensor.Probe.ProbeTemp = ProbeTemp*100;
 pMain->Sensor.Probe.fProbeTemp = ProbeTemp;
 		NRF_LOG_INFO("[%10d] probe temp : "NRF_LOG_FLOAT_MARKER,getTickCount(),NRF_LOG_FLOAT(ProbeTemp));

}