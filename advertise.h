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

#define AD_TIMEOUT_MAX                      0x7800
#define NO_TIMEOUT                          0
#define MAX_INTERVAL                        0x4000
#define MIN_UNCONNECTED_INTERVAL            0x0020
#define MIN_CONNECTED_INTERVAL              0x00A0
#define DEFAULT_UNCONNECTED_INTERVAL        0x00A0
#define DEFAULT_CONNECTED_INTERVAL          0x0800

#define ADV_DATA_CHARACTERS                 64
#define ADV_DATA_BYTES                      31
#define MAX_ADVERTISING_DATA_LENGTH         31

#define APP_ADV_INTERVAL                     MSEC_TO_UNITS(25, UNIT_0_625_MS)                        /**< The advertising interval (25 ms.). */
#define APP_ADV_TIMEOUT_IN_SECONDS           BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED                   /**< The advertising timeout in units of seconds. This is set to @ref BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED so that the advertisement is done as long as there there is a call to @ref dfu_transport_close function.*/

/**
 * @brief Advertise states.
 */
typedef enum
{
    ADVERTISE_STATE_INIT,               /**< Initialize Advertise state. */
    ADVERTISE_STATE_READY,              /**< Advertise state ready. */
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
