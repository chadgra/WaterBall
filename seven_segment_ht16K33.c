#include "seven_segment_ht16K33.h"
#include "app_twi.h"
#include "HT16k33_I2C.h"
#include "app_util_platform.h"
#include "nrf_drv_twi.h"

/**
 * @brief TWI master instance
 *
 * Instance of TWI master driver 
 *
 */
static const nrf_drv_twi_t m_twi_master = NRF_DRV_TWI_INSTANCE(MASTER_TWI_INST);

void seven_segment_init(void)
{
    ret_code_t ret;
    const nrf_drv_twi_config_t config =
    {
       .scl                = TWI_SCL_M,
       .sda                = TWI_SDA_M,
       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH
    };

    do
    {
        ret = nrf_drv_twi_init(&m_twi_master, &config, NULL, NULL);
        if(NRF_SUCCESS != ret)
        {
            break;
        }
        nrf_drv_twi_enable(&m_twi_master);
    }while(0);

    HT16K33_I2C_byte_write(HT16K33, (uint8_t *)HT16K33_OSC_ON);   
    HT16K33_I2C_byte_write(HT16K33, (uint8_t *)HT16K33_DISPLAYON);
    HT16K33_I2C_byte_write(HT16K33, (uint8_t *)(HT16K33_DIM + 15));
}


static void seven_segment_set_digit_raw(uint8_t digit, uint8_t data)
{
    // Digits (L-to-R) are 0,1,2,3
    // Send segment-data to specified digit (0-3) on led display
    if (4 < digit)
    {
        // Only digits 0-3
        return;
    }
    
    // Skip over colon at position 2
    if (1 < digit)
    {
        digit++;
    }
    
    // Multiply by 2
    digit<<=1;
    
    uint8_t tx_data[2] = {digit, data};
    HT16K33_I2C_register_write(HT16K33, tx_data, sizeof(tx_data));
}


void seven_segment_blankdigit(uint8_t digit)
{
    seven_segment_set_digit_raw(digit, 0x00);
}

// Display data value (0-F) on specified digit (0-3) of LED display
static void seven_segment_set_digit(uint8_t digit, uint8_t data)
{
    // only values <=16
    if (0x10 < data)
    {
        return;
    }
    
    // Show value on display.
    seven_segment_set_digit_raw(digit, number_table[data]);  
}


static void seven_segment_set_colon(uint8_t data)
{
    // The colon is represented by bit 1 at address 0x04. There are three other
    // single LED "decimal points" on the display, which are at thge following bit positions
    // bit2 = topo left, bit3=bottom left, bit4= top right
    uint8_t tx_data[] = {0x04, data<<1};
    
    HT16K33_I2C_register_write(HT16K33, tx_data, sizeof(tx_data));
}


void seven_segment_set_digits(uint8_t digit_0, uint8_t digit_1, uint8_t digit_2, uint8_t digit_3, uint8_t colon)
{
    seven_segment_set_digit(0, digit_0);
    seven_segment_set_digit(1, digit_1);
    seven_segment_set_digit(2, digit_2);
    seven_segment_set_digit(3, digit_3);
    seven_segment_set_colon(colon);
}
