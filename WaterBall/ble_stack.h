/**
 * @file
 * @defgroup WaterBall ble_stack.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall BLE module.
 *
 * The BLE module handles interactions with the SoftDevice and is the gateway to
 * all bluetooth funcitonality.
 */

#ifndef BLE_STACK_H__
#define BLE_STACK_H__

#include <stdint.h>
#include "ble.h"

#define APP_TIMER_PRESCALER                     0       /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE                 2       /**< Size of timer operation queues. */

#define CENTRAL_LINK_COUNT                      1       /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT                   1       /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

/**
 * @brief   BLE stack module states.
 */
typedef enum
{
    BLE_STACK_STATE_INIT,               /**< The ble stack module is initializing. */
    BLE_STACK_STATE_READY,              /**< The ble stack module is ready for commands. */
    BLE_STACK_STATE_ERROR               /**< The ble stack module has received an error. */
} ble_stack_state_t;

/**
 * @brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt);

/**
 * @brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in] sys_evt  System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt);

/**
 * @brief Function to initialize the ble stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(void);

/**
 * @brief Function to accomplish at engine tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void ble_stack_tasks(void);

#endif //BLE_STACK_H__

/** @} */
