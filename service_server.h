/**
 * @file
 * @defgroup WaterBall service_server.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall game server module.
 *
 * Create the game server service.
 */

#ifndef SERVICE_SERVER_H__
#define SERVICE_SERVER_H__

#include <stdbool.h>
#include <stdint.h>
#include "ble.h"
#include "ble_types.h"

#define PROPERTY_BROADCAST                          (0x01)
#define PROPERTY_READ                               (0x02)
#define PROPERTY_WRITE_WO_RESPONSE                  (0x04)
#define PROPERTY_WRITE                              (0x08)
#define PROPERTY_NOTIFY                             (0x10)
#define PROPERTY_INDICATE                           (0x20)
#define PROPERTY_AUTH_SIGNED_WRITE                  (0x40)

/**
 * @brief   game server module states.
 */
typedef enum
{
    SERVICE_SERVER_STATE_INIT,         /**< Initialize the game server module. */
    SERVICE_SERVER_STATE_READY,        /**< The game server module is running normally. */
    SERVICE_SERVER_STATE_CONNECTING,   /**< The game server is being connected to. */
    SERVICE_SERVER_STATE_CONNECTED,    /**< The game server has been connect to - the mode set. */
    SERVICE_SERVER_STATE_ERROR         /**< Throw an error message if it occurred in the interrupt handler. */
} service_server_state_t;

/**
 * @brief   game server callback for ble events.
 */
typedef void (* service_server_ble_evt_callback_t)(ble_evt_t * p_ble_evt);

/**
 * @brief   game server characteristics.
 */
typedef struct
{
    ble_uuid_t                          uuid;
    uint32_t                            max_length;
    service_server_ble_evt_callback_t   read_callback;
    service_server_ble_evt_callback_t   write_callback;
    __packed uint16_t *                 character_handle;
    uint8_t                             properties;
    ble_gatts_char_handles_t            handles;
} service_server_characteristic_t;

/**
 * @brief   Function called on ble events.
 *
 * @param[in]   p_ble_evt       The event data.
 */
void service_server_on_ble_evt(ble_evt_t * p_ble_evt);

/**
 * @brief   Function to initialize the game server module.
 */
void service_server_init(void);

/**
 * @brief   Function to accomplish game server module tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void service_server_tasks(void);

/**
 * @brief   Function to check if the game is running in server mode.
 *
 * @retval  True if we are running in server mode.
 */
bool service_server_is_connected(void);

/**
 * @brief   Send a HVX struct for a indication or notification.
 *
 * @param[in]   type            A BLE_GATT_HVX_TYPES.
 * @param[in]   uuid            The UUID of the characteristic to send, this will be used to retrieve the handle.
 * @param[in]   len             The length of the message to send.
 * @param[in]   p_value         A pointer to the data to send.
 */
static void service_server_hvx_send(uint8_t type, uint16_t uuid, uint16_t len, void * p_value);

/**
 * @brief   Get the previously written client score.
 *
 * @retval      The client score that was previously written to the server.
 */
uint32_t service_server_get_client_score(void);

/**
 * @brief   Create a server score indication.
 *
 * @param[in]   score           The server score to indicate.
 */
void service_server_indicate_server_score(uint32_t score);

/**
 * @brief   Create a remaining game time indication.
 *
 * @param[in]   ms_remaining    The remaining time (in ms) to indicate.
 */
void service_server_indicate_game_time(uint32_t ms_remaining);

/**
 * @brief   Handle a read of the service info.
 *
 * @param[in]   p_ble_evt       The event data.
 */
static void service_server_read_info(ble_evt_t * p_ble_evt);

/**
 * @brief   Handle a read of the server score.
 *
 * @param[in]   p_ble_evt       The event data.
 */
static void service_server_read_server_score(ble_evt_t * p_ble_evt);

/**
 * @brief   Handle a read of the client score.
 *
 * @param[in]   p_ble_evt       The event data.
 */
static void service_server_read_client_score(ble_evt_t * p_ble_evt);

/**
 * @brief   Handle a write of the client score.
 *
 * @param[in]   p_ble_evt       The event data.
 */
static void service_server_write_client_score(ble_evt_t * p_ble_evt);

/**
 * @brief   Handle a a read of the remaining game time.
 *
 * @param[in]   p_ble_evt       The event data.
 */
static void service_server_read_game_time(ble_evt_t * p_ble_evt);

/**
 * @brief   Function to respond to a write request.
 *
 * @param[in]   gatt_status     The status of the response (success/failure).
 */
static void service_server_write_request_response(uint16_t gatt_status);

/**
 * @brief   Function to respond to a read request.
 *
 * @param[in]   offset          The offset of the data, received from the red request.
 * @param[in]   len             The length of the data (before the offset).
 * @param[in]   p_data          A pointer to the beginning of the data (without the offset applied).
 */
static void service_server_read_request_response(uint16_t offset, uint16_t len, void * p_data);

/**
 * @brief   Function to add a characteristic to a service.
 *
 * @param[in]   characteristic  A pointer to a struct that contains information about the characteristic.
 */
static void service_server_characteristic_add(service_server_characteristic_t * characteristic);

/**
 * @brief   Given a uuid return the characteristic that it belongs to.
 *
 * @param[in]   uuid            The uuid to find the characteristic of.
 *
 * @retval                      A pointer to the characteristic.
 */
static service_server_characteristic_t * service_server_get_characteristic(ble_uuid_t uuid);

#endif //SERVICE_SERVER_H__

/** @} */
