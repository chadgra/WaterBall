/**
 * @file
 * @defgroup WaterBall service.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall service module.
 *
 * Main interface for the service server and client.
 */

#ifndef SERVICE_H__
#define SERVICE_H__

#include <stdbool.h>
#include <stdint.h>
#include "ble.h"
#include "ble_types.h"

#define MAX_CONNECTING_CYCLES                       (8)
#define SERVICE_MAX_TX_BYTES                           (GATT_MTU_SIZE_DEFAULT - sizeof(uint8_t) - sizeof(uint16_t))    /**< The opcode and handle take up a few bytes of the MTU. */

#define SERVICE_BASE_UUID_128                          { 0x12, 0x9A, 0xF0, 0x11, 0xA1, 0x09, 0x2F, 0xF4, 0xE1, 0x00, 0x6A, 0x11, 0xBA, 0xE9, 0xA7, 0x44 }
#define SERVICE_BASE_UUID                              (0xE9BA)
#define SERVICE_SERVER_SCORE_UUID                      (0x15C0)
#define SERVICE_CLIENT_SCORE_UUID                      (0x25C0)
#define SERVICE_TIME_UUID                              (0x7100)

#define IS_SERVICE_CLIENT                              (service_is_client())
#define IS_SERVICE_SERVER                              (service_is_server())

#define SERVICE_UUID(UUID)                             { UUID, BLE_UUID_TYPE_BLE }

/**
 * @brief   service server module states.
 */
typedef enum
{
    SERVICE_STATE_INIT = 0,             /**< Initialize the service server module. */
    SERVICE_STATE_READY,               /**< The service server module is running normally. */
    SERVICE_STATE_ERROR                /**< Throw an error message if it occurred in the interrupt handler. */
} service_state_t;

/**
 * @brief   service info structure.
 */
typedef struct
{
    uint16_t    server_score_handle;
    uint16_t    server_score_config_handle;
    uint16_t    client_score_handle;
    uint16_t    game_time_handle;
    uint16_t    game_time_config_handle;
} service_info_t;

/**
 * @brief   Function called on ble events.
 *
 * @param[in]   p_ble_evt           The event data.
 */
void service_on_ble_evt(ble_evt_t * p_ble_evt);

/**
 * @brief   Handler for when the service button is pressed.
 *
 * @param[in]   pin_number          The pin number that was pressed.
 * @param[in]   button_action       The button action - PUSH/RELEASE.
 */
void service_button_event_handler(uint8_t pin_number, uint8_t button_action);

/**
 * @brief   Function to initialize the service module.
 */
void service_init(void);

/**
 * @brief   Function to accomplish service module tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void service_tasks(void);

uint32_t service_store_read_data(uint8_t * p_buffer, uint32_t size);

/**
 * @brief   Try to connect as a client, this function gives access to service_client_try_connect command.
 */
void service_try_connect(void);

/**
 * @brief   Is this device a service client, this function gives access to service_client_is_connected.
 *
 * @retval      True if the service_client module is in the connected state.
 */
bool service_is_client(void);

/**
 * @brief   Is this device a service server, this function gives access to service_server_is_connected.
 *
 * @retval      True if the service_server module is in the connected state.
 */
bool service_is_server(void);

#endif //SERVICE_H__

/** @} */
