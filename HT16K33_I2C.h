/*
 * @brief Seven Segment display module.
 */
 
#ifndef HT16K33_I2C_H
#define HT16K33_I2C_H

#include <stdint.h>
#include "app_error.h"

#define TWI_SCL_M                3   //!< Master SCL pin
#define TWI_SDA_M                4   //!< Master SDA pin

#define MASTER_TWI_INST          0   //!< MASTERTWI interface 

ret_code_t HT16K33_I2C_byte_write(uint8_t slave_addr, uint8_t  * data);

ret_code_t HT16K33_I2C_register_write(uint8_t slave_addr, uint8_t * data, uint32_t bytes);

#endif // HT16K33_I2C_H
