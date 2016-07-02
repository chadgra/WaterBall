/**
 * @file
 * @defgroup WaterBall storage.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall persistant storage module.
 */

#include <string.h>

#include "app_error.h"
#include "connect.h"
#include "discovery.h"
#include "nordic_common.h"
#include "pstorage.h"
#include "status.h"
#include "storage.h"


static bool                 m_auto_flash = true;
static bool                 m_is_locked = false;
static uint8_t              m_volatile_copy[BLOCK_SIZE] = { 0 };
static uint8_t              m_flashed_copy[BLOCK_SIZE] = { 0 };
static storage_state_t      m_storage_state;
static pstorage_handle_t    m_base_handle;
static uint32_t             m_event_result;


/**
 * @brief pstorage event handler.
 *
 * @details The persistent storage operation completion callback is used by the interface to report
 *          success or failure of a flash operation. Since data is not copied for a store operation, 
 *          a callback is an indication that the resident memory can now be reused or freed.
 *
 * @param[in] handle   Identifies the module and block for the callback that is received.
 * @param[in] op_code  Identifies the operation for the event that is notified.
 * @param[in] result   Identifies the result of a flash access operation. NRF_SUCCESS implies 
 *                     operation succeeded.
 *
 *                     @note Unmanaged (abnormal behaviour) error codes from the SoftDevice flash 
 *                     access API are forwarded as is and are expected to be handled by the 
 *                     application. For details refer to the implementation file and corresponding 
 *                     SoftDevice flash API documentation.
 *
 * @param[in] p_data   Identifies the application data pointer. For a store operation, this points 
 *                     to the resident source of application memory that the application can now 
 *                     free or reuse. When there is a clear operation, this is NULL since no 
 *                     application pointer is needed for this operation.
 * @param[in] data_len Length data the application provided for the operation. 
 */
static void storage_event_handler(pstorage_handle_t *   p_handle,
                                  uint8_t               op_code,
                                  uint32_t              result,
                                  uint8_t *             p_data,
                                  uint32_t              data_len)
{
    if (NRF_SUCCESS != result)
    {
        m_event_result = result;
        m_storage_state = STORAGE_STATE_ERROR;
    }
}


void storage_init(void)
{
    // Put all the values in a single block.
    // We can then access as little as a single 4 byte block.
    pstorage_module_param_t param =
    {
        storage_event_handler,
        BLOCK_SIZE,
        1
    };

    //APP_ERROR_CHECK(pstorage_init());
    APP_ERROR_CHECK(pstorage_register(&param, &m_base_handle));
    APP_ERROR_CHECK(pstorage_load(m_flashed_copy, &m_base_handle, BLOCK_SIZE, 0));

    // If the device is turned off in the middle of a pstorage update it is possible that our
    // flash page is empty (or corrupt), so see if the checksum is correct.
    if (!storage_is_correct_checksum())
    {
        // If it isn't correct then let's load from the pstorage swap - it likely contains the
        // last good data.
        storage_load_swap();
        if (!storage_is_correct_checksum())
        {
            // If the checksum still doesn't match then our data is truly lost - or we are starting
            // for the first time. Clear it out just in case there is corrupt data.
            memset(m_flashed_copy, 0xFF, BLOCK_SIZE);
        }
        else
        {
            // If our data is good then make sure it gets copied into our flash page.
            storage_store_data(false);
        }
    }

    memcpy(m_volatile_copy, m_flashed_copy, BLOCK_SIZE);
    m_storage_state = STORAGE_STATE_INIT;
}


void storage_tasks(void)
{
    switch (m_storage_state)
    {
        case STORAGE_STATE_INIT:
        {
            m_storage_state = STORAGE_STATE_READY;
            break;
        }
        case STORAGE_STATE_READY:
        {
            break;
        }
        case STORAGE_STATE_ERROR:
        {
            APP_ERROR_HANDLER(m_event_result);
            break;
        }
        default:
        {
            break;
        }
    }
}


void storage_init_value(storage_address_t address, uint32_t size, uint8_t * p_local)
{
    uint32_t    offset      = address * sizeof(uint32_t);
    uint8_t *   p_internal  = m_volatile_copy + offset;
    uint32_t *  p_uint32    = (uint32_t *)p_internal;
    uint32_t    max_size    = CEIL_DIV(size, sizeof(uint32_t)) * sizeof(uint32_t);

    // Make sure the size is not greater than our block size.
    APP_ERROR_CHECK_BOOL(BLOCK_SIZE >= (offset + max_size));

    // If we've never permanently stored this value then don't update the info object.
    bool hasModifications = false;
    for (int i = 0; i < (max_size / sizeof(uint32_t)); i++)
    {
        if (DEFAULT_MEMORY_VALUE != *p_uint32++)
        {
            hasModifications = true;
            break;
        }
    }

    // If there are modifications in the internal copy then copy it to the local.
    // If there aren't modifications then copy local to internal.
    uint8_t * p_source      = hasModifications ? p_internal : p_local;
    uint8_t * p_destination = hasModifications ? p_local : p_internal;
    memcpy(p_destination, p_source, size);
}


bool storage_update_value(storage_address_t address, uint32_t size, uint8_t * p_local, bool force)
{
    uint32_t    offset      = address * sizeof(uint32_t);
    uint8_t *   p_volatile  = m_volatile_copy + offset;
    uint8_t *   p_flashed   = m_flashed_copy + offset;
    uint32_t    max_size    = CEIL_DIV(size, sizeof(uint32_t)) * sizeof(uint32_t);

    // Make sure the size is not greater than our block size.
    APP_ERROR_CHECK_BOOL(BLOCK_SIZE >= (offset + max_size));

    // If this is locked and we aren't forcing it then exit with an error.
    if (m_is_locked && !force)
    {
        // If it was locked we need to restore the original value.
        memcpy(p_local, p_volatile, size);
        return false;
    }

    if (0 == memcmp(p_volatile, p_local, size))
    {
        // The values already match, so don't write them again.
        return true;
    }

    memcpy(p_volatile, p_local, size);

    // Make sure auto_flash is true, or that we are setting auto_flash.
    if (m_auto_flash ||
        force)
    {
        bool was_connecting = false;
        bool was_discovering = false;

        // Pause connecting and discovering since they often fail because the radio is so busy.
        if (IS_CONNECTING)
        {
            connect_cancel();
            was_connecting = true;
        }

        if (IS_DISCOVERING)
        {
            discovery_cancel();
            was_discovering = true;
        }

        memcpy(p_flashed, p_local, size);
        storage_store_data(true);

        if (was_connecting)
        {
            //connect_connect();
        }

        if (was_discovering)
        {
            discovery_discovery();
        }
    }

    return true;
}


void storage_clear_all(void)
{
    // Make sure to clear out the internal copy.
    memset(m_volatile_copy + PERMANENT_BLOCK_SIZE, 0xFF, NON_PERMANENT_BLOCK_SIZE);
    memset(m_flashed_copy + PERMANENT_BLOCK_SIZE, 0xFF, NON_PERMANENT_BLOCK_SIZE);
    storage_store_data(true);
}


bool storage_is_ready(void)
{
    // Check the count of remaining accesses. When it is zero we are ready.
    uint32_t count;
    APP_ERROR_CHECK(pstorage_access_status_get(&count));
    return (0 == count);
}


uint32_t * storage_memory_address(storage_address_t address)
{
    uint32_t offset = address * sizeof(uint32_t);
    return (uint32_t *)(m_base_handle.block_id + offset);
}


static void storage_store_data(bool copy_to_swap)
{
    // If you write all the bytes then pstorage SDK won't copy them to the swap.
    uint32_t size = copy_to_swap ? BLOCK_SIZE - sizeof(uint32_t) : BLOCK_SIZE;
    storage_update_checksum();
    APP_ERROR_CHECK(pstorage_update(&m_base_handle, m_flashed_copy, size, 0));
    while (!storage_is_ready());
}


static void storage_load_swap(void)
{
    memcpy(m_flashed_copy, (void *)PSTORAGE_SWAP_ADDR, BLOCK_SIZE);
}


static uint32_t storage_calculate_checksum(void)
{
    uint32_t checksum = 0;
    uint32_t * p_data = (uint32_t *)m_flashed_copy;

    // Only calculate the checksum over the "non-permanent" stored values. The main reason
    // for this is that the error message is "non-permanent", but because an error could happen
    // during an interrupt we can't use the pstorage module, we write it manually using
    // registers. As such we can't update the checksum (erasing and copying the flash to the
    // swap page would be too much to do right before reseting). If the checksum is incorrect
    // after the reset the flash page gets erased and we fall back to the swap page which
    // has old values, and doesn't have the error information.
    for (int i = STORAGE_ADDRESS_MIN; i < STORAGE_ADDRESS_CHECKSUM; i++)
    {
        checksum += p_data[i];
    }

    return checksum;
}


static void storage_update_checksum(void)
{
    uint32_t * p_data = (uint32_t *)m_flashed_copy;
    p_data[STORAGE_ADDRESS_CHECKSUM] = storage_calculate_checksum();
}


static bool storage_is_correct_checksum(void)
{
    uint32_t * p_data = (uint32_t *)m_flashed_copy;
    return p_data[STORAGE_ADDRESS_CHECKSUM] == storage_calculate_checksum();
}

/** @} */
