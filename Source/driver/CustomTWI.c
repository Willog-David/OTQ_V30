
#include "CustomTWI.h"

#define TWI_SCL_PIN     42
#define TWI_SDA_PIN     39

// SDA 핀 설정
void twi_sda_set(uint8_t value) {
    if (value) {
        nrf_gpio_cfg_input(TWI_SDA_PIN, NRF_GPIO_PIN_PULLUP);
    } else {
        nrf_gpio_cfg_output(TWI_SDA_PIN);
        //nrf_gpio_pin_clear(TWI_SDA_PIN);
    }
}
		
void twi_sda_setout(uint8_t value)
{
	if(value)
		{
	      nrf_gpio_cfg_output(TWI_SDA_PIN);
        nrf_gpio_pin_set(TWI_SDA_PIN);
		}
	else
		{
	      nrf_gpio_cfg_output(TWI_SDA_PIN);
        nrf_gpio_pin_clear(TWI_SDA_PIN);
		}
}

// SCL 핀 설정
void twi_scl_set(uint8_t value) {
    if (value) {
        nrf_gpio_cfg_output(TWI_SCL_PIN);
        nrf_gpio_pin_set(TWI_SCL_PIN);
    } else {
        nrf_gpio_cfg_output(TWI_SCL_PIN);
        nrf_gpio_pin_clear(TWI_SCL_PIN);
    }
}
		
// TWI ACK 확인
uint8_t twi_check_ack(void) {
    uint8_t ack;

    twi_sda_set(1);
    nrf_delay_us(10);
    twi_scl_set(1);
    nrf_delay_us(10);
    ack = nrf_gpio_pin_read(TWI_SDA_PIN);
    twi_scl_set(0);
		twi_sda_set(0);
    return !ack;
}
void gpio_init(void)
{
    nrf_gpio_cfg_output(TWI_SCL_PIN);
    nrf_gpio_cfg_output(TWI_SDA_PIN);

    nrf_gpio_pin_set(TWI_SCL_PIN);
    nrf_gpio_pin_set(TWI_SDA_PIN);
}
	
void twi_start(void)
{
    nrf_gpio_pin_set(TWI_SCL_PIN);
    nrf_gpio_pin_clear(TWI_SDA_PIN);
    nrf_delay_us(10);
    nrf_gpio_pin_clear(TWI_SCL_PIN);
}
	
void twi_stop(void)
{
    nrf_gpio_pin_clear(TWI_SCL_PIN);
    nrf_gpio_pin_clear(TWI_SDA_PIN);
    nrf_delay_us(10);
    nrf_gpio_pin_set(TWI_SCL_PIN);
    nrf_gpio_pin_set(TWI_SDA_PIN);
}
	
// TWI 바이트 쓰기
uint8_t twi_master_write_byte(uint8_t byte) {
    uint8_t i, ack;

		twi_sda_set(0);
    for (i = 0; i < 8; i++) {
        if (byte & 0x80) {
            twi_sda_setout(1);
        } else {
            twi_sda_setout(0);
        }
        byte <<= 1;
        nrf_delay_us(10);
        twi_scl_set(1);
        nrf_delay_us(10);
        twi_scl_set(0);
    }

    ack = twi_check_ack();

    return ack;
}

// TWI 바이트 읽기
uint8_t twi_master_read_byte(uint8_t ack) {
    uint8_t i, byte = 0;

    twi_sda_set(1);
    for (i = 0; i < 8; i++) {
        nrf_delay_us(10);
        twi_scl_set(1);
        nrf_delay_us(10);
        byte <<= 1;
        if (nrf_gpio_pin_read(TWI_SDA_PIN)) {
            byte |= 0x01;
        }
        twi_scl_set(0);
    }

    twi_sda_set(0);
    nrf_delay_us(10);
    twi_scl_set(1);
    nrf_delay_us(10);
    twi_scl_set(0);

    return byte;
}

uint16_t twi_master_read_word(uint8_t addr, uint16_t reg)
{
    uint16_t value;

    // 시작 조건 생성
    twi_master_start();

    // 슬레이브 주소 및 쓰기 비트 전송
    twi_write_byte((addr << 1) | 0x00);

    // 레지스터 주소 상위 8비트 전송
    twi_master_write_byte(reg >> 8);

    // 레지스터 주소 하위 8비트 전송
    twi_master_write_byte(reg & 0xFF);

    // 재시작 조건 생성
    twi_master_start();

    // 슬레이브 주소 및 읽기 비트 전송
    twi_write_byte((addr << 1) | 0x01);

    // 데이터 바이트 수신
    value = twi_master_read_byte(false) << 8;
    value |= twi_master_read_byte(true);

    // 정지 조건 생성
    twi_master_stop();

    return value;
}
	
void twi_master_write_bytes(uint8_t addr, uint16_t reg, uint8_t *data, uint8_t len)
{
	uint8 ack=0;
    // 시작 조건 생성
    twi_start();

    // 슬레이브 주소 및 쓰기 비트 전송
    ack = twi_master_write_byte((addr << 1) | 0x00);

    // 레지스터 주소 전송
    ack = twi_master_write_byte(reg >> 8 & 0xff);
		ack = twi_master_write_byte(reg & 0xff);

		NRF_LOG_INFO("%x %x",addr,reg);
    // 데이터 바이트 전송
    for (int i = 0; i < len; i++) {
        ack = twi_master_write_byte(data[i]);
				NRF_LOG_INFO("%x ",data[i]);
    }

    // 정지 조건 생성
    twi_stop();
}
	
void twi_master_read_bytes(uint8_t addr, uint16_t reg, uint8_t *data, uint8_t len)
{
	uint8_t ack=0;
    // 시작 조건 생성
    twi_start();

    // 슬레이브 주소 및 쓰기 비트 전송
    ack = twi_master_write_byte((addr << 1) | 0x00);

    // 레지스터 주소 전송
    ack = twi_master_write_byte(reg>>8 & 0xff);
		ack = twi_master_write_byte(reg & 0xff);

    // 재시작 조건 생성
		twi_stop();
    twi_start();

    // 슬레이브 주소 및 읽기 비트 전송
    ack = twi_master_write_byte((addr << 1) | 0x01);

    // 데이터 바이트 수신
    for (int i = 0; i < len-1; i++) {
        data[i] = twi_master_read_byte(1);
    }
			data[len-1] = twi_master_read_byte(0);

    // 정지 조건 생성
    twi_stop();
}