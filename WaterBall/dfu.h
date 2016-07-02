/**
 * @file
 * @defgroup Appaloosa dfu.h
 * @{
 * @ingroup Appaloosa
 * @brief Appaloosa DFU header module.
 */
 
#ifndef DFU_H__
#define DFU_H__

#include <stdbool.h>
#include "device_manager.h"

#define APP_TIMER_PRESCALER              0                                          /**< Value of the RTC1 PRESCALER register. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY    APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER)/**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT     3                                          /**< Number of attempts before giving up the connection parameter negotiation. */

#define DFU_REV_MAJOR                    0x00                                       /** DFU Major revision number to be exposed. */
#define DFU_REV_MINOR                    0x01                                       /** DFU Minor revision number to be exposed. */
#define DFU_REVISION                     ((DFU_REV_MAJOR << 8) | DFU_REV_MINOR)     /** DFU Revision number to be exposed. Combined of major and minor versions. */

#define BOOTLOADER_WATCHDOG_RELOAD      120000                                      /** Set the reload value to 2 minutes so that we can perform an upgrade without the bootloader needing to feed the watchdog. */

/**
 * @brief   Function for preparing for system reset.
 *
 * @details This function implements @ref dfu_app_reset_prepare_t. It will be called by 
 *          @ref dfu_app_handler.c before entering the bootloader/DFU.
 *          This allows the current running application to shut down gracefully.
 */
static void dfu_reset_prepare(void);

/**
 * @brief   Function to initialize the dfu module.
 */
void dfu_init(void);

/**
 * @brief   Function to accomplish dfu module tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void dfu_tasks(void);

/**
 * @brief   Function called on ble events.
 *
 * @param[in]   p_ble_evt           The event data.
 */
void dfu_on_ble_evt(ble_evt_t * p_ble_evt);

#endif //DFU_H__

/** @} */
