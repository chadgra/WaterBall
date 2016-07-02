/**
 * @file
 * @defgroup WaterBall connect.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall connect module.
 *
 * Connect/Disconnect to a device.
 */

#ifndef CONNECT_H__
#define CONNECT_H__

#include <stdbool.h>
#include <stdint.h>
#include "ble.h"
#include "ble_gap.h"

#define CONNECTION_TIMEOUT_MAX                  (0xF000)
#define MS_IN_SEC                               (1000)
#define INTERVAL_TICKS_TO_MS(TICKS)             (TICKS * 5 / 4)     /**< Interval ticks are each 1.25 ms. This is how to convert without using floats. */
#define SUPERVISION_TIMEOUT_MS_TO_TICKS(MS)     (MS / 10)           /**< Supervision ticks are each 10 ms. */

/**
 * @brief   connect module states.
 */
typedef enum
{
    CONNECT_STATE_INIT,     /**< Initialize the connect module. */
    CONNECT_STATE_READY,    /**< The connect module is running normally. */
    CONNECT_STATE_ERROR     /**< Throw an error message if it occurred in the interrupt handler. */
} connect_state_t;

/**
 * @brief   Function to initialize the connect module.
 */
void connect_init(void);

/**
 * @brief   Function to accomplish connect module tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void connect_tasks(void);

/**
 * @brief   Handle ble events.
 *
 * @param[in]   p_ble_evt       Event data.
 */
void connect_on_ble_evt(ble_evt_t const * p_ble_evt);

/**
 * @brief   Function to make a connection.
 *
 * @param[in]   p_address       The address of the device to connect to.
 *
 * @retval      True if the soft device connect command succeeded.
 */
bool connect_connect(ble_gap_addr_t * p_address);

/**
 * @brief   Function to disconnect.
 */
void connect_disconnect(void);

/**
 * @brief   Function to cancel a connection attempt.
 */
void connect_cancel(void);

/**
 * @brief   Function for default connection parameters.
 *
 */
void default_conn_params(void);

/**
 * @brief   Get the handle of any current connections.
 *
 * @retval      The handle of the current connection.
 */
uint16_t connect_get_handle(void);

#endif //CONNECT_H__

/** @} */
