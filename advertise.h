/**
 * @file
 *
 * @defgroup advertise module
 * @{
 * @ingroup application
 *
 * @brief advertise interface.
 */
 
#ifndef ADVERTISE_H__
#define ADVERTISE_H__

#include <stdbool.h>
#include <stdint.h>
#include "ble.h"

/**
 * @brief Advertise states.
 */
typedef enum
{
    ADVERTISE_STATE_INIT,               /**< Initialize Advertise state. */
    ADVERTISE_STATE_READY,              /**< Advertise state ready. */
    ADVERTISE_ADVERTISING,              /**< Advertising started. */
    ADVERTISE_STATE_ERROR               /**< Throw an error message if it occurred in the interrupt handler. */
} advertise_state_t;

/**
 * @brief Function to initialize the advertise module.
 */
void advertise_init(void);

/**
 * @brief Function to accomplish the advertise tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void advertise_tasks(void);

/**
 * @brief Function for handling the Advertising events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
void advertise_on_ble_evt(ble_evt_t const * p_adv_data_evt);

/**
 * @brief   Function to start advertising.
 *
 * @retval      True if advertising started correctly.
 */
bool advertise_advertise(void);

/**
 * @brief   Function to stop advertising.
 */
void advertise_cancel(void);

/**
 * @brief   Function to set the advertise data.
 */
static void advertise_set_data(void);

#endif
