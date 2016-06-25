#include "seven_segment.h"
#include "i2c.h"

void seven_segment_init(void)
{
    i2c_byte_write(TIME_ADDRESS, HT16K33_OSC_ON);   
    i2c_byte_write(TIME_ADDRESS, HT16K33_DISPLAYON);
    i2c_byte_write(TIME_ADDRESS, HT16K33_DIM + 15);
}


void seven_segment_tasks(void)
{
    seven_segment_set_digits(0, 4, 2, 7, COLON_TYPE_TOP_RIGHT);
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
    i2c_data_write(TIME_ADDRESS, tx_data, sizeof(tx_data));
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


static void seven_segment_set_colon(colon_type_t colon_type)
{
    // The colon is represented by bit 1 at address 0x04. There are three other
    // single LED "decimal points" on the display, which are at the following bit positions
    // bit2 = topo left, bit3=bottom left, bit4= top right
    uint8_t tx_data[] = { 0x04, colon_type };
    i2c_data_write(TIME_ADDRESS, tx_data, sizeof(tx_data));
}


void seven_segment_set_digits(uint8_t digit_0, uint8_t digit_1, uint8_t digit_2, uint8_t digit_3, colon_type_t colon_type)
{
    seven_segment_set_digit(0, digit_0);
    seven_segment_set_digit(1, digit_1);
    seven_segment_set_digit(2, digit_2);
    seven_segment_set_digit(3, digit_3);
    seven_segment_set_colon(colon_type);
}
