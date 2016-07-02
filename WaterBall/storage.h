/**
 * @file
 * @defgroup WaterBall storage.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall persistant storage module.
 *
 * The storage driver is a wrapper around the SDKs pstorage module and provides
 * helper functions and macros to simplify the processes of initializing and
 * updating persisted values, whether they be basic types (uint32_t, bool, etc.)
 * or larger byte arrays.
 */

#ifndef STORAGE_H__
#define STORAGE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "app_util.h"

#define PERMANENT_BLOCK_SIZE            (STORAGE_ADDRESS_PERMANENT_MAX * sizeof(uint32_t))
#define NON_PERMANENT_BLOCK_SIZE        ((STORAGE_ADDRESS_MAX - STORAGE_ADDRESS_MIN) * sizeof(uint32_t))
#define BLOCK_SIZE                      (PERMANENT_BLOCK_SIZE + NON_PERMANENT_BLOCK_SIZE)
#define DEFAULT_MEMORY_VALUE            (PSTORAGE_FLASH_EMPTY_MASK)

/**
 * @brief   An address that needs to use multiple blocks can be declared in
 *          'storage_address_t' using this macro.
 *
 * @param[in]   ADDRESS     The name of the address that will be used to refer to it.
 * @param[in]   COUNT       The number of blocks that will be set aside to store this value.
 */
#define MULTI_BLOCK(ADDRESS, COUNT)     ADDRESS, ADDRESS##_rest = (ADDRESS + COUNT - 1)

/**
 * @brief   An address that needs to use more than 4 bytes can be declared in
 *          'storage_address_t' using this macro.
 *
 * @param[in]   ADDRESS     The name of the address that will be used to refer to it.
 * @param[in]   SIZE        The number of bytes that will be set aside to store this value.
 *                          This number should always be a multiple of four.
 */
#define MULTI_BYTE(ADDRESS, SIZE)       MULTI_BLOCK(ADDRESS, CEIL_DIV(SIZE, sizeof(uint32_t)))

/**
 * @brief   A macro to create local storage and the struct that describes it.
 *
 * @param[in]   ADDRESS         The address as defined in storage_address_t where this is permanently stored.
 * @param[in]   TYPE            The type of this storage could be a basic type (uint32_t) or a struct.
 * @param[in]   NAME            The name of this storage.
 * @param[in]   ...             The remaining arguments that will be used to initialize the default value.
 */
#define CREATE_STORAGE_VALUE(ADDRESS, TYPE, NAME, ...)                          \
static TYPE             NAME = { __VA_ARGS__ };                                 \
static storage_info_t   NAME##_info = { ADDRESS, sizeof(NAME), (uint8_t *)&NAME };

/**
 * @brief   A macro to create local storage and the struct that describes it for arrays.
 *
 * @param[in]   ADDRESS         The address as defined in storage_address_t where this is permanently stored.
 * @param[in]   TYPE            The type of this storage could be a basic type (uint32_t) or a struct.
 * @param[in]   NAME            The name of this storage.
 * @param[in]   COUNT           The number of entries in the array.
 * @param[in]   ...             The remaining arguments that will be used to initialize the default value.
 */
#define CREATE_ARRAY_STORAGE_VALUE(ADDRESS, TYPE, NAME, COUNT, ...)             \
static TYPE             NAME[COUNT] = { __VA_ARGS__ };                          \
static storage_info_t   NAME##_info = { ADDRESS, sizeof(NAME), (uint8_t *)NAME };

/**
 * @brief   A macro to simplify the initialization of 'storage_byte_array_info_t' types.
 *
 * @param[in]   INFO        The 'storage_byte_array_info_t' variable. Pointers to the
 *                          'storage_byte_array_info_t' variable should be dereferenced.
 */
#define INIT_STORAGE_VALUE(NAME)                                                \
    storage_init_value(NAME##_info.address, NAME##_info.size, NAME##_info.p_storage)

/**
 * @brief   A macro to simplify the initialization of 'storage_byte_array_info_t' types.
 *
 * @param[in]   INFO        The 'storage_byte_array_info_t' variable. Pointers to the
 *                          'storage_byte_array_info_t' variable should be dereferenced.
 */
#define TRY_UPDATE_STORAGE_VALUE(NAME)                                          \
    storage_update_value(NAME##_info.address, NAME##_info.size, NAME##_info.p_storage, false)

/**
 * @brief   A macro to simplify the initialization of 'storage_byte_array_info_t' types.
 *
 * @param[in]   INFO        The 'storage_byte_array_info_t' variable. Pointers to the
 *                          'storage_byte_array_info_t' variable should be dereferenced.
 */
#define UPDATE_STORAGE_VALUE(NAME)                                              \
    storage_update_value(NAME##_info.address, NAME##_info.size, NAME##_info.p_storage, true)

/**
 * @brief   Enum of all of the pstorage blocks. The last entry 'STORAGE_ADDRESS_MAX'
 *          will be used to initialize the block count in the pstorage module.
 */
typedef enum
{
    STORAGE_ADDRESS_PERMANENT_MIN = -1,                             /**< Permanent Address Minimum, the next value should be 0. */
    STORAGE_ADDRESS_PERMANENT_MAX,                                  /**< Everything before this will not be cleared with a factory reset. */
    STORAGE_ADDRESS_MIN = STORAGE_ADDRESS_PERMANENT_MAX,            /**< The min should always the the first entry of values that are cleared with a factory reset. */
    STORAGE_ADDRESS_FACTORY_RESET = STORAGE_ADDRESS_MIN,
    STORAGE_ADDRESS_CHECKSUM,
    STORAGE_ADDRESS_USED_FOR_SWAPPING,                              /**< We have this extra word. Sometimes we write it so that the swap will be skipped. */
    STORAGE_ADDRESS_MAX                                             /**< The max should always be the last entry that is cleared with a factory reset. */
} storage_address_t;

/**
 * @brief   Storage module states.
 */
typedef enum
{
    STORAGE_STATE_INIT,                 /**< The storage module is initializing. */
    STORAGE_STATE_READY,                /**< The storage module is ready for commands. */
    STORAGE_STATE_ERROR                 /**< The storage module has received an error. */
} storage_state_t;

/**
 * @brief   Structure to be used to put a value into storage.
 */
typedef struct
{
    storage_address_t   address;
    size_t              size;
    void *              p_storage;
} storage_info_t;

/**
 * @brief Function to initialize the storage module.
 */
void storage_init(void);

/**
 * @brief Function to accomplish the storage tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void storage_tasks(void);

/**
 * @brief   Function to initialize a stored value from the persistant storage.
 *          If no value has previously been stored the initialized value will be used.
 *
 * @param[in]   address     The 0-based address of where this value starts.
 * @param[in]   size        The size (in bytes) of this value.
 * @param[in]   p_local     A pointer to the memory where this value should be placed in the local module.
 */
void storage_init_value(storage_address_t address, uint32_t size, uint8_t * p_local);

/**
 * @brief   Function to request an update to a stored value in the persistant storage.
 *          A maximum of PSTORAGE_CMD_QUEUE_SIZE (default 10) updates can be queued.
 *
 * @param[in]   address     The 0-based address of where this value starts.
 * @param[in]   size        The size (in bytes) of this value.
 * @param[in]   p_local     A pointer to the memory where this value should be placed in the local module.
 * @param[in]   force       Force the update, even if the storage is locked.
 *
 * @retval                  Return an at_response_t enum. AT_RESPONSE_OK if the store succeeded,
 *                          AT_RESPONSE_CONFIGURATION_LOCKED otherwise.
 */
bool storage_update_value(storage_address_t address, uint32_t size, uint8_t * p_value, bool force);

/**
 * @brief   Function to request that all of the persistant memory gets cleared.
 */
void storage_clear_all(void);

/**
 * @brief   Function to see if the storage is in the ready state - meaning it isn't busy
 *          waiting for a task (store/update/clear) to complete.
 *
 * @retval      True if the storage module is in the "STORAGE_STATE_READY" state, false otherwise.
 */
bool storage_is_ready(void);

/**
 * @brief   Function to get the memory address in flash of a supplied address.
 *
 * @param[in]   address     The storage address (enum).
 *
 * @retval  A pointer in flash memory to the value.
 */
uint32_t * storage_memory_address(storage_address_t address);

/**
 * @brief   Store data to flash.
 *
 * param[in]    copy_to_swap    If the old flash page should be copied to flash before
 *                              the new page is written.
 */
static void storage_store_data(bool copy_to_swap);

/**
 * @brief   Load the swap page into the interal storage array.
 */
static void storage_load_swap(void);

/**
 * @brief   Calculate the checksum over the data.
 */
static uint32_t storage_calculate_checksum(void);

/**
 * @brief   Load the correct checksum of the data into the checksum storage location.
 */
static void storage_update_checksum(void);

/**
 * @brief   See if the stored checksum matches the data.
 */
static bool storage_is_correct_checksum(void);

#endif //STORAGE_H__

/** @} */
