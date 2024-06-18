
#include "struct.h"


void gpio_init(void);
void twi_start(void);
void twi_stop(void);
bool twi_write_byte(uint8_t byte);
void twi_master_write_bytes(uint8_t addr, uint16_t reg, uint8_t *data, uint8_t len);
void twi_master_read_bytes(uint8_t addr, uint16_t reg, uint8_t *data, uint8_t len);
