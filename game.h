/**
 * @file
 * @defgroup WaterBall game.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall game module.
 *
 * Main interface for the game server and client.
 */

#ifndef GAME_H__
#define GAME_H__

#include <stdbool.h>
#include <stdint.h>
#include "ble.h"
#include "ble_types.h"

#define MAX_CONNECTING_CYCLES                       (8)
#define GAME_MAX_TX_BYTES                           (GATT_MTU_SIZE_DEFAULT - sizeof(uint8_t) - sizeof(uint16_t))    /**< The opcode and handle take up a few bytes of the MTU. */

#define GAME_BASE_UUID_128                          { 0x11, 0x9A, 0xF0, 0x11, 0xA1, 0x09, 0x2F, 0xF4, 0xE1, 0x00, 0x6A, 0x11, 0xBA, 0xE9, 0xA7, 0x44 }
#define GAME_BASE_UUID                              (0xE9BA)
#define GAME_SERVER_SCORE_UUID                      (0x15C0)
#define GAME_CLIENT_SCORE_UUID                      (0x25C0)
#define GAME_TIME_UUID                              (0x7100)

#define IS_GAME_CLIENT                              (game_is_client())
#define IS_GAME_SERVER                              (game_is_server())

#define GAME_UUID(UUID)                             { UUID, BLE_UUID_TYPE_BLE }

/**
 * @brief   game server module states.
 */
typedef enum
{
    GAME_STATE_INIT = 0,             /**< Initialize the game server module. */
    GAME_STATE_READY,               /**< The game server module is running normally. */
    GAME_STATE_ERROR                /**< Throw an error message if it occurred in the interrupt handler. */
} game_state_t;

/**
 * @brief   game info structure.
 */
typedef struct
{
    uint16_t    server_score_handle;
    uint16_t    server_score_config_handle;
    uint16_t    client_score_handle;
    uint16_t    game_time_handle;
    uint16_t    game_time_config_handle;
} game_info_t;

/**
 * @brief   Function called on ble events.
 *
 * @param[in]   p_ble_evt           The event data.
 */
void game_on_ble_evt(ble_evt_t * p_ble_evt);

/**
 * @brief   Handler for when the game button is pressed.
 *
 * @param[in]   pin_number          The pin number that was pressed.
 * @param[in]   button_action       The button action - PUSH/RELEASE.
 */
void game_button_event_handler(uint8_t pin_number, uint8_t button_action);

/**
 * @brief   Function to initialize the game module.
 */
void game_init(void);

/**
 * @brief   Function to accomplish game module tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void game_tasks(void);

uint32_t game_store_read_data(uint8_t * p_buffer, uint32_t size);

/**
 * @brief   Try to connect as a client, this function gives access to game_client_try_connect command.
 */
void game_try_connect(void);

/**
 * @brief   Is this device a game client, this function gives access to game_client_is_connected.
 *
 * @retval      True if the game_client module is in the connected state.
 */
bool game_is_client(void);

/**
 * @brief   Is this device a game server, this function gives access to game_server_is_connected.
 *
 * @retval      True if the game_server module is in the connected state.
 */
bool game_is_server(void);

#endif //GAME_H__

/** @} */
