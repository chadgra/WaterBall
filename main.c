/**
 * @file
 * @defgroup WaterBall main.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall application main file.
 *
 * It is intended that this file will have a main function that initializes all
 * of the supporting modules, and then an infinite loop that calls of the supporting
 * modules allowing them to perform work.
 */
 
 #include "ble_stack.h"
 #include "bsp.h"
 #include "buttons.h"
 #include "clock.h"
 #include "game_engine.h"
 #include "serial.h"

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
    ble_stack_init();
    buttons_init();
    clock_init();
    game_engine_init();
    serial_init();

    while (true)
    {
        ble_stack_tasks();
        buttons_tasks();
        clock_tasks();
        game_engine_tasks();
        serial_tasks();
    }
}

/** @} */
