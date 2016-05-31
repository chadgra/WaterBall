/**
 * @file
 * @defgroup WterBall main.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall application main file.
 *
 * It is intended that this file will have a main function that initializes all
 * of the supporting modules, and then an infinite loop that calls of the supporting
 * modules allowing them to perform work.
 */
 
 #include "bsp.h"
 
 /**
 * @brief Function for main application entry.
 */
int main(void)
{
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);

    while (true)
    {
    }
}

/** @} */
