/*
 * @brief Seven Segment display module.
 */
 
#ifndef SEVEN_SEGMENT_H
#define SEVEN_SEGMENT_H

#include <stdint.h>
#include "app_twi.h"

#define HT16K33_BASE_ADDRESS        0x70                            // I2C bus base address for Ht16K33 backpack
#define TIME_ADDRESS                (HT16K33_BASE_ADDRESS + 0)
#define SCORE_ADDRESS               (HT16K33_BASE_ADDRESS + 1)

#define HT16K33_OSC_ON              0x21                            // turn device oscillator on
#define HT16K33_STANDBY             0x20                            // turn device oscillator off
#define HT16K33_DISPLAYON           0x81                            // turn on output pins
#define HT16K33_DISPLAYOFF          0x80                            // turn off output pins
#define HT16K33_BLINKON             0x85                            // blink rate 1 Hz (-2 for 2 Hz)
#define HT16K33_BLINKOFF            0x81                            // same as display on
#define HT16K33_DIM                 0xE0                            // add level (15=max) to byte

// The bit numbers of each segment.
//  0000
// 5    1
// 5    1
// 5    1
// 5    1
//  6666
// 4    2
// 4    2
// 4    2
// 4    2
//  3333

typedef enum
{
    COLON_TYPE_NONE = 0,
    COLON_TYPE_COLON = 2,
    COLON_TYPE_TOP_LEFT = 4,
    COLON_TYPE_BOTTOM_LEFT = 8,
    COLON_TYPE_TOP_RIGHT = 16
} colon_type_t;

// Initialize seven segment module(HT16K33).
void seven_segment_init(void);

// Tasks for the seven segment module.
void seven_segment_tasks(void);

// Setting raw digit.
static void seven_segment_set_digit_raw(uint8_t address, uint8_t digit, uint8_t data);

// Clearing out digits.
void seven_segment_blank_digit(uint8_t address, uint8_t digit);

// Setting actual led's of the seven segment display with a number.
static void seven_segment_set_digit(uint8_t address, uint8_t digit, uint8_t data);

// Setting actual led's of the seven segment display - given a character.
static void seven_segment_set_char_digit(uint8_t address, uint8_t digit, char data);

// Setting colon.
static void seven_segment_set_colon(uint8_t address, colon_type_t colon_type);

// Setting the left and right 2 digit numbers.
void seven_segment_set_numbers(uint8_t address, uint8_t left_value, uint8_t right_value, colon_type_t colon_type);

// Setting all seven segment all at once.
void seven_segment_set_digits(uint8_t address, uint8_t digit_0, uint8_t digit_1, uint8_t digit_2, uint8_t digit_3, colon_type_t colon_type);

// Setting the digits with characters - at least the best representation we can make of them.
void seven_segment_set_char_digits(uint8_t address, uint8_t start_digit, char * string, colon_type_t colon_type);

// Blanking all digits.
void seven_segment_blank_digits(uint8_t address);

#endif // SEVEN_SEGMENT_H
