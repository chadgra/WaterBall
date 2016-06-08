/**
 * @file
 * @defgroup WaterBall game_client.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall game client module.
 *
 * Create the game client which will connect to game servers.
 */

#ifndef GAME_CLIENT_H__
#define GAME_CLIENT_H__

#include <stdbool.h>
#include <stdint.h>

#include "ble.h"
#include "ble_types.h"

#define GAME_CLIENT_START_HANDLE            (0x0001)

/**
 * @brief   game_client module states.
 */
typedef enum
{
    GAME_CLIENT_STATE_INIT,                 /**< Initialize the game_client module. */
    GAME_CLIENT_STATE_READY,                /**< The game_client module is running normally. */
    GAME_CLIENT_STATE_CONNECTING,           /**< The client is trying to connect to a server. */
    GAME_CLIENT_STATE_CONNECTED,            /**< The client is connected to a server. */
    GAME_CLIENT_STATE_ERROR                 /**< Throw an error message if it occurred in the interrupt handler. */
} game_client_state_t;

/**
 * @brief   Function called on ble events.
 *
 * @param[in]   p_ble_evt       The event data.
 */
void game_client_on_ble_evt(ble_evt_t * p_ble_evt);

/**
 * @brief   Function to initialize the game client module.
 */
void game_client_init(void);

/**
 * @brief   Function to accomplish game client module tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void game_client_tasks(void);

/**
 * @brief   Function to check if the game is running in client mode.
 *
 * @retval  True if we are running in client mode.
 */
bool game_client_is_connected(void);

/**
 * @brief   Function to try and start a connection with a game server as the client.
 */
void game_client_try_connect(void);

/**
 * @brief   Function to facilitate writing values.
 *
 * @param[in]   write_op        The write operation, (REQUEST or COMMAND, etc).
 * @param[in]   handle          The handle of the characteristic to write to.
 * @param[in]   len             The length of the data (in bytes) to write.
 * @param[in]   p_value         A pointer to the data to write.
 */
static void game_client_write(uint8_t write_op, uint16_t handle, uint16_t len, void * p_value);

#endif //GAME_CLIENT_H__

/** @} */
