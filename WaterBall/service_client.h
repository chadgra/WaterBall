/**
 * @file
 * @defgroup WaterBall service_client.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall game client module.
 *
 * Create the game client which will connect to game servers.
 */

#ifndef SERVICE_CLIENT_H__
#define SERVICE_CLIENT_H__

#include <stdbool.h>
#include <stdint.h>

#include "ble.h"
#include "ble_types.h"

#define SERVICE_CLIENT_START_HANDLE             (0x0001)
#define SERVICE_INFO_ATTR_OFFSET                (2)

/**
 * @brief   service_client module states.
 */
typedef enum
{
    SERVICE_CLIENT_STATE_INIT,                  /**< Initialize the service_client module. */
    SERVICE_CLIENT_STATE_READY,                 /**< The service_client module is running normally. */
    SERVICE_CLIENT_STATE_CONNECTING,            /**< The client is trying to connect to a server. */
    SERVICE_CLIENT_STATE_CONNECTED,             /**< The client is connected to a server. */
    SERVICE_CLIENT_STATE_ERROR                  /**< Throw an error message if it occurred in the interrupt handler. */
} service_client_state_t;

/**
 * @brief   Function called on ble events.
 *
 * @param[in]   p_ble_evt       The event data.
 */
void service_client_on_ble_evt(ble_evt_t * p_ble_evt);

/**
 * @brief   Function to initialize the game client module.
 */
void service_client_init(void);

/**
 * @brief   Function to accomplish game client module tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void service_client_tasks(void);

/**
 * @brief   Function to check if the game is running in client mode.
 *
 * @retval  True if we are running in client mode.
 */
bool service_client_is_connected(void);

/**
 * @brief   Function to try and start a connection with a game server as the client.
 */
void service_client_try_connect(void);

/**
 * @brief   Function to get the previously indicated server score.
 *
 * @retval      The previously indicated server score.
 */
uint32_t service_client_get_server_score(void);

/**
 * @brief   Get the previously written current time.
 *
 * @retval      The current time that was previously written to the server.
 */
uint32_t service_client_get_current_time(void);

/**
 * @brief   Get the previously written game time.
 *
 * @retval      The game time that was previously written to the server.
 */
uint32_t service_client_get_game_time(void);

/**
 * @brief   Get the previously written vibration value.
 *
 * @retval      The vibration value that was previously written to the server.
 */
uint32_t service_client_get_vibration(void);

/**
 * @brief   Get the previously written hole value.
 *
 * @retval      The hole value that was previously written to the server.
 */
uint32_t service_client_get_hole(void);

/**
 * @brief   Get the previously written target score.
 *
 * @retval      The target score that was previously written to the server.
 */
uint32_t service_client_get_target_score(void);

/**
 * @brief   Function for writing to the client score handle of the server.
 *
 * @param[in]   score           The client score to write to the server.
 */
void service_client_write_client_score(uint32_t score);

/**
 * @brief   Function to facilitate writing values.
 *
 * @param[in]   write_op        The write operation, (REQUEST or COMMAND, etc).
 * @param[in]   handle          The handle of the characteristic to write to.
 * @param[in]   len             The length of the data (in bytes) to write.
 * @param[in]   p_value         A pointer to the data to write.
 */
static void service_client_write(uint8_t write_op, uint16_t handle, uint16_t len, void * p_value);

#endif //SERVICE_CLIENT_H__

/** @} */
