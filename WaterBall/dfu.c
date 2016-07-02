/**
 * @file
 * @defgroup WaterBall dfu.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall DFU module.
 */

#include "advertise.h"
#include "app_error.h"
#include "ble_dfu.h"
#include "ble_hci.h"
#include "bsp.h"
#include "connect.h"
#include "dev_man.h"
#include "dfu.h"
#include "dfu_app_handler.h"
#include "nrf_delay.h"
#include "status.h"
#include "storage.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT  1
STATIC_ASSERT(IS_SRVC_CHANGED_CHARACT_PRESENT);                                     /** When having DFU Service support in application the Service Changed Characteristic should always be present. */

static ble_dfu_t                         m_dfus;                                    /**< Structure used to identify the DFU service. */


static void dfu_reset_prepare(void)
{
    if (IS_CONNECTED)
    {
        // Disconnect from peer.
        connect_disconnect();
    }
    else
    {
        // If not connected, the device will be advertising. Hence stop the advertising.
        advertise_cancel();
    }

    while (!storage_is_ready());

    // Without this wait the update fails.
    nrf_delay_ms(500);
}


void dfu_init(void)
{
    ble_dfu_init_t   dfus_init;

    // Initialize the Device Firmware Update Service.
    memset(&dfus_init, 0, sizeof(dfus_init));

    dfus_init.evt_handler   = dfu_app_on_dfu_evt;
    dfus_init.error_handler = NULL;
    dfus_init.revision      = DFU_REVISION;

    uint32_t err_code = ble_dfu_init(&m_dfus, &dfus_init);
    APP_ERROR_CHECK(err_code);

    dfu_app_reset_prepare_set(dfu_reset_prepare);
    dfu_app_dm_appl_instance_set(*dev_man_get_app_instance());
}


void dfu_tasks(void)
{
}


void dfu_on_ble_evt(ble_evt_t * p_ble_evt)
{
    ble_dfu_on_ble_evt(&m_dfus, p_ble_evt);
}

/** @} */
