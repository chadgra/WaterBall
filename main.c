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
#include "game_engine.h"
#include "i2c.h"
#include "serial.h"
#include "service.h"
#include "seven_segment.h"
#include "status.h"

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
    i2c_init();
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
        i2c_tasks();
        seven_segment_tasks();
    }
}

/** @} */
