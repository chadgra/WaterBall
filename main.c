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
 
#include "advertise.h"
#include "ble_stack.h"
#include "bsp.h"
#include "buttons.h"
#include "clock.h"
#include "connect.h"
#include "discovery.h"
#include "service.h"
#include "game_engine.h"
#include "serial.h"
#include "status.h"
#include "seven_segment_ht16K33.h"

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    status_init();
    ble_stack_init();
    buttons_init();
    clock_init();
    game_engine_init();
    serial_init();
    service_init();
    advertise_init();
    discovery_init();
    connect_init();
    seven_segment_init();

    while (true)
    {
        status_tasks();
        ble_stack_tasks();
        buttons_tasks();
        clock_tasks();
        game_engine_tasks();
        serial_tasks();
        service_tasks();
        advertise_tasks();
        discovery_tasks();
        connect_tasks();
        //seven_segment_set_digits(1, 2, 3, 4, 5);
    }
}

/** @} */
