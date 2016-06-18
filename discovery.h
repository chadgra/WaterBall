/**
 * @file
 * @defgroup WaterBall discovery.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall discovery module.
 *
 */

#ifndef DISCOVERY_H__
#define DISCOVERY_H__

#include <stdbool.h>
#include "ble.h"
#include "ble_gap.h"

#define DISCOVERY_ADV_REPORT_MAX          10
#define DISCOVERY_ADDRESS_MAX             10
#define CONVERT_TO_SEC                    1000

/**
 * @brief Discovery states.
 */
typedef enum
{
    DISCOVERY_STATE_INIT,               /**< Initialize Discovery state. */
    DISCOVERY_STATE_READY,              /**< Discovery state ready. */
    DISCOVERY_DISCOVERING,              /**< Discovery has been started. */
    DISCOVERY_STATE_ERROR               /**< Throw an error message if it occurred in the interrupt handler. */
} discovery_state_t;

/**
 * @brief Function to initialize the discovery module.
 */
void discovery_init(void);

/**
 * @brief Function to accomplish the discovery tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void discovery_tasks(void);

/**
 * @brief Function for handling the Scanning events.
 *
 * @param[in] p_scan_evt  Bluetooth stack event.
 */
void discovery_on_ble_evt(ble_evt_t const * p_scan_evt);

/**
 * @brief Function for checking address and data coming in while discovering.
 *
 * @param[in] check_address  Address to be checked.
 * @param[in] data           Data to be checked.
 * @param[in] length         Length of the data.
 */
bool check_adv_report_func(ble_gap_addr_t check_address, uint8_t const data[], uint8_t length);

/**
 * @brief   Function to start discovering(or scanning) devices.
 *
 * @retval      True if discovery was started.
 */
bool discovery_discovery(void);

/**
 * @brief   Function to cancel an ongoing discovery.
 */
void discovery_cancel(void);

#endif //DISCOVERY_H__

/** @} */
