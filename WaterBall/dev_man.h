/**
 * @file
 * @defgroup Appaloosa dev_man.h
 * @{
 * @ingroup Appaloosa
 * @brief Appaloosa device_manager module.
 *
 * Application specific setup of the SDKs device manager module.
 */

#ifndef DEV_MAN_H__
#define DEV_MAN_H__

#include <stdint.h>
#include "device_manager.h"

#define SEC_PARAM_BOND                   1                                          /**< Perform bonding. */
#define SEC_PARAM_MITM                   0                                          /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                   0                                          /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS               0                                          /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES        BLE_GAP_IO_CAPS_NONE                       /**< No I/O capabilities. */
#define SEC_PARAM_OOB                    0                                          /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE           7                                          /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE           16                                         /**< Maximum encryption key size. */

#define APP_SERVICE_HANDLE_START         0x000C                                     /**< Handle of first application specific service when service changed characteristic is present. */
#define BLE_HANDLE_MAX                   0xFFFF                                     /**< Max handle value in BLE. */

/**
 * @brief   Function for the Device Manager initialization.
 */
void dev_man_init(void);

/**
 * @brief   Function to accomplish dev_man module tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void dev_man_tasks(void);

/**
 * @brief   Function to get a pointer to the dm_application_instance_t struct.
 *
 * @retval  Pointer to the dm_application_instance_t struct.
 */
dm_application_instance_t * dev_man_get_app_instance(void);

#endif //DEV_MAN_H__

/** @} */
