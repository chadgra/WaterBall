/*
 * @brief Seven Segment display module.
 */
 
#ifndef SEVEN_SEGMENT_HT16K33_H
#define SEVEN_SEGMENT_HT16K33_H

#include <stdint.h>
#include "app_twi.h"

#define HT16K33                     (0xE0>>1)                          // I2C bus address for Ht16K33 backpack
#define HT16K33_OSC_ON              0x21                               // turn device oscillator on
#define HT16K33_STANDBY             0x20                               // turn device oscillator off
#define HT16K33_DISPLAYON           0x81                               // turn on output pins
#define HT16K33_DISPLAYOFF          0x80                               // turn off output pins
#define HT16K33_BLINKON             0x85                               // blink rate 1 Hz (-2 for 2 Hz)
#define HT16K33_BLINKOFF            0x81                               // same as display on
#define HT16K33_DIM                 0xE0                               // add level (15=max) to byte

static const uint8_t number_table[] =
{
    0x3F,           // 0
    0x06,           // 1
    0x5B,           // 2
    0x4F,           // 3
    0x66,           // 4
    0x6D,           // 5
    0x7D,           // 6
    0x07,           // 7
    0x7F,           // 8
    0x6F,           // 9
    0x77,           // A
    0x7C,           // b
    0x39,           // C
    0x5E,           // d
    0x79,           // E
    0x71,           // F
    0x00,           //<blank>
};


// Initialize sevent segment module(HT16K33).
void seven_segment_init(void);

// Setting raw digit.
static void seven_segment_set_digit_raw(uint8_t digit, uint8_t data);

// Clearing out digits.
void seven_segment_blankdigit(uint8_t digit);

// Setting actual led's of the seven segment display
static void seven_segment_set_digit(uint8_t digit, uint8_t data);

// Setting colon.
static void seven_segment_set_colon(uint8_t data);

// Setting all seven segment all at once.
void seven_segment_set_digits(uint8_t digit_0, uint8_t digit_1, uint8_t digit_2, uint8_t digit_3, uint8_t colon);


#endif // SEVEN_SEGMENT_HT16K33_H
