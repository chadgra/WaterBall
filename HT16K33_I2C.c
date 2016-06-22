#include "HT16K33_I2C.h"
#include "nrf_drv_twi.h"

static const nrf_drv_twi_t * p_twi_master;

ret_code_t HT16K33_I2C_byte_write(uint8_t slave_addr, uint8_t  * data)
{
    ret_code_t ret_code;
    
    ret_code = nrf_drv_twi_tx(p_twi_master, slave_addr, data, sizeof(uint8_t), false);
    
    return ret_code;
}


ret_code_t HT16K33_I2C_register_write(uint8_t slave_addr, uint8_t * data, uint32_t size)
{
    ret_code_t ret_code;
    
    ret_code = nrf_drv_twi_tx(p_twi_master, slave_addr, data, size, false);
    
    return ret_code;
}
