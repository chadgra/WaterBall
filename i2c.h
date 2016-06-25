/*
 * @brief i2c module.
 */
 
#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "app_error.h"

#define TWI_SCL_M                   3                               //!< Master SCL pin
#define TWI_SDA_M                   4                               //!< Master SDA pin

#define MASTER_TWI_INST             0                               //!< MASTERTWI interface 

void i2c_init(void);

void i2c_tasks(void);

void i2c_byte_write(uint8_t address, uint8_t data);

void i2c_data_write(uint8_t address, uint8_t * p_data, uint32_t size);

#endif // I2C_H
