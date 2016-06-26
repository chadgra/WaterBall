#include "i2c.h"
#include "seven_segment.h"

static const uint8_t number_table[] =
{
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x77, // A
    0x7C, // b
    0x39, // C
    0x5E, // d
    0x79, // E
    0x71, // F
    0x00  //<blank>
};


static const uint16_t alphafont_table[] =
{
    0x00,
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, //  
    0x00, // !
    0x00, // "
    0x00, // #
    0x00, // $
    0x00, // %
    0x00, // &
    0x00, // '
    0x00, // (
    0x00, // )
    0x00, // *
    0x00, // +
    0x00, // ,
    0x00, // -
    0x00, // .
    0x00, // /
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x00, // :
    0x00, // ;
    0x00, // <
    0x00, // =
    0x00, // >
    0x00, // ?
    0x00, // @
    0xF7, // A
    0x7F, // B
    0x69, // C
    0x3F, // D
    0x79, // E
    0x71, // F
    0x7D, // G
    0xF6, // H
    0x06, // I
    0x1E, // J
    0x70, // K
    0x38, // L
    0x36, // M
    0x36, // N
    0x3F, // O
    0xF3, // P
    0x3F, // Q
    0xF3, // R
    0xED, // S
    0x01, // T
    0x3E, // U
    0x30, // V
    0x36, // W
    0x00, // X
    0x6E, // Y
    0x09, // Z
    0x00, // [
    0x00, // 
    0x00, // ]
    0x00, // ^
    0x00, // _
    0x00, // `
    0x58, // a
    0x78, // b
    0xD8, // c
    0x5E, // d
    0x58, // e
    0x71, // f
    0x8E, // g
    0x70, // h
    0x04, // i
    0x0E, // j
    0x00, // k
    0x06, // l
    0xD4, // m
    0x54, // n
    0x5C, // o
    0x70, // p
    0x86, // q
    0x50, // r
    0x88, // s
    0x78, // t
    0x1C, // u
    0x04, // v
    0x14, // w
    0xC0, // x
    0x0C, // y
    0x48, // z
    0x49, // {
    0x00, // |
    0x00, // }
    0x00, // ~
    0x00
};


void seven_segment_init(void)
{
    i2c_byte_write(TIME_ADDRESS, HT16K33_OSC_ON);   
    i2c_byte_write(TIME_ADDRESS, HT16K33_DISPLAYON);
    i2c_byte_write(TIME_ADDRESS, HT16K33_DIM + 15);
    seven_segment_blank_digits(TIME_ADDRESS);

    i2c_byte_write(SCORE_ADDRESS, HT16K33_OSC_ON);   
    i2c_byte_write(SCORE_ADDRESS, HT16K33_DISPLAYON);
    i2c_byte_write(SCORE_ADDRESS, HT16K33_DIM + 15);
    seven_segment_blank_digits(SCORE_ADDRESS);
}


void seven_segment_tasks(void)
{
}


static void seven_segment_set_digit_raw(uint8_t address, uint8_t digit, uint8_t data)
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
    i2c_data_write(address, tx_data, sizeof(tx_data));
}


void seven_segment_blank_digit(uint8_t address, uint8_t digit)
{
    seven_segment_set_digit_raw(address, digit, 0x00);
}

// Display data value (0-F) on specified digit (0-3) of LED display
static void seven_segment_set_digit(uint8_t address, uint8_t digit, uint8_t data)
{
    // only values <=16
    if (0x10 < data)
    {
        return;
    }

    // Show value on display.
    seven_segment_set_digit_raw(address, digit, number_table[data]);
}


static void seven_segment_set_char_digit(uint8_t address, uint8_t digit, char data)
{
    // only values <=16
    if (sizeof(alphafont_table) <= data)
    {
        return;
    }

    // Show value on display.
    seven_segment_set_digit_raw(address, digit, alphafont_table[data]);
}


static void seven_segment_set_colon(uint8_t address, colon_type_t colon_type)
{
    // The colon is represented by bit 1 at address 0x04. There are three other
    // single LED "decimal points" on the display, which are at the following bit positions
    // bit2 = topo left, bit3=bottom left, bit4= top right
    uint8_t tx_data[] = { 0x04, colon_type };
    i2c_data_write(address, tx_data, sizeof(tx_data));
}


void seven_segment_set_numbers(uint8_t address, uint8_t left_value, uint8_t right_value, colon_type_t colon_type)
{
    seven_segment_set_digits(address, left_value / 10, left_value % 10, right_value / 10, right_value % 10, colon_type);
}


void seven_segment_set_digits(uint8_t address, uint8_t digit_0, uint8_t digit_1, uint8_t digit_2, uint8_t digit_3, colon_type_t colon_type)
{
    seven_segment_set_digit(address, 0, digit_0);
    seven_segment_set_digit(address, 1, digit_1);
    seven_segment_set_digit(address, 2, digit_2);
    seven_segment_set_digit(address, 3, digit_3);
    seven_segment_set_colon(address, colon_type);
}


void seven_segment_set_char_digits(uint8_t address, uint8_t start_digit, char * string, colon_type_t colon_type)
{
    for (int i = start_digit, j = 0; (i < 4) && (string[j] != 0); i++, j++)
    {
        seven_segment_set_char_digit(address, i, string[j]);
    }

    seven_segment_set_colon(address, colon_type);
}


void seven_segment_blank_digits(uint8_t address)
{
    seven_segment_blank_digit(address, 0);
    seven_segment_blank_digit(address, 1);
    seven_segment_blank_digit(address, 2);
    seven_segment_blank_digit(address, 3);
    seven_segment_set_colon(address, COLON_TYPE_NONE);
}
