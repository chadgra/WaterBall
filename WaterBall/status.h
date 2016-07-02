/**
 * @file
 * @defgroup WaterBall status.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall status.
 *
 * Set and clear and check the current module status.
 */

#ifndef STATUS_H__
#define STATUS_H__

#include <stdbool.h>
#include <stdint.h>
#include "ble_gap.h"

#define STATUS_CONNECTED                (0x01)
#define STATUS_ADVERTISING              (0x02)
#define STATUS_DISCOVERING              (0x04)
#define STATUS_CONNECTING               (0x08)

#define IS_CONNECTED                    (status_is_set(STATUS_CONNECTED))
#define IS_ADVERTISING                  (status_is_set(STATUS_ADVERTISING))
#define IS_DISCOVERING                  (status_is_set(STATUS_DISCOVERING))
#define IS_CONNECTING                   (status_is_set(STATUS_CONNECTING))
#define IS_IDLE                         (status_is_clear(STATUS_CONNECTED | STATUS_ADVERTISING | STATUS_DISCOVERING | STATUS_CONNECTING))

#define IS_INVALID_ROLE                 (BLE_GAP_ROLE_INVALID == status_get_role())
#define IS_PERIPHERAL                   (BLE_GAP_ROLE_PERIPH == status_get_role())
#define IS_CENTRAL                      (BLE_GAP_ROLE_CENTRAL == status_get_role())

/**
 * @brief Function to initialize the status module.
 */
void status_init(void);

/**
 * @brief Function to accomplish the status tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void status_tasks(void);

/**
 * @brief Set the current status.
 *
 * @param[in]   status  The status to set.
 */
void status_set(uint32_t status);

/**
 * @brief Clear the current status.
 *
 * @param[in]   status  The status to clear.
 */
void status_clear(uint32_t status);

/**
 * @brief Check if a status is currently set.
 *
 * @param[in]   status  The status(es) to check.
 */
bool status_is_set(uint32_t status);

/**
 * @brief Check if a status is currently clear.
 *
 * @param[in]   status  The status(es) to check.
 */
bool status_is_clear(uint32_t status);

/**
 * @brief   Set the role of the device - invalid, peripheral, or central.
 *
 * @param[in]   role    The current role.
 */
void status_set_role(uint8_t role);

/**
 * @brief   Get the role of the device - invalid, peripheral, or central.
 *
 * @retval              The current role
 */
uint8_t status_get_role(void);

#endif //STATUS_H__

/** @} */
