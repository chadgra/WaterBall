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
#include "dev_man.h"
#include "dfu.h"
#include "discovery.h"
#include "game.h"
#include "i2c.h"
#include "ir_led.h"
#include "serial.h"
#include "service.h"
#include "seven_segment.h"
#include "status.h"
#include "storage.h"
#include "watchdog.h"

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    ble_stack_init();
    watchdog_init();
    dev_man_init();                 /**< Run before storage_init, since it also uses pstorage and will initialize it. */
    storage_init();
    status_init();
    buttons_init();
    clock_init();
    serial_init();
    service_init();
    dfu_init();                     /**< Initialize after the service, or else it won't work. */
    advertise_init();
    discovery_init();
    connect_init();
    i2c_init();
    seven_segment_init();
    ir_led_init();
    game_init();


    while (true)
    {
        ble_stack_tasks();
        watchdog_tasks();
        dev_man_tasks();
        storage_tasks();
        status_tasks();
        buttons_tasks();
        clock_tasks();
        serial_tasks();
        service_tasks();
        dfu_tasks();
        advertise_tasks();
        discovery_tasks();
        connect_tasks();
        i2c_tasks();
        seven_segment_tasks();
        ir_led_tasks();
        game_tasks();
    }
}

/** @} */
