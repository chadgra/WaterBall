#include "app_util_platform.h"
#include "i2c.h"
#include "nrf_drv_twi.h"

static const nrf_drv_twi_t m_twi_master = NRF_DRV_TWI_INSTANCE(MASTER_TWI_INST);

#define NUM_TRIES                       50

void i2c_init(void)
{
    const nrf_drv_twi_config_t config =
    {
       .scl                = TWI_SCL_M,
       .sda                = TWI_SDA_M,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH
    };

    APP_ERROR_CHECK(nrf_drv_twi_init(&m_twi_master, &config, NULL, NULL));
    nrf_drv_twi_enable(&m_twi_master);
}


void i2c_tasks(void)
{
}


void i2c_byte_write(uint8_t address, uint8_t data)
{
    int i = NUM_TRIES;
    uint32_t result;
    do
    {
        result = nrf_drv_twi_tx(&m_twi_master, address, &data, sizeof(data), false);
        i--;
    } while ((NRF_SUCCESS != result) && (0 < i));
}


void i2c_data_write(uint8_t address, uint8_t * p_data, uint32_t size)
{
    int i = NUM_TRIES;
    uint32_t result;
    do
    {
        result = nrf_drv_twi_tx(&m_twi_master, address, p_data, size, false);
        i--;
    } while ((NRF_SUCCESS != result) && (0 < i));
}
