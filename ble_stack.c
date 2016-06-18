/**
 * @file
 * @defgroup WaterBall ble_stack.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall BLE module.
 */

#include <stdint.h>

#include "advertise.h"
#include "app_timer.h"
#include "ble_gap.h"
#include "ble_srv_common.h"
#include "ble_stack.h"
#include "bsp.h"
#include "connect.h"
#include "discovery.h"
#include "service.h"
#include "softdevice_handler.h"

static ble_stack_state_t                       m_ble_stack_state;


static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    service_on_ble_evt(p_ble_evt);
    connect_on_ble_evt(p_ble_evt);
    advertise_on_ble_evt(p_ble_evt);
    discovery_on_ble_evt(p_ble_evt);
}


static void sys_evt_dispatch(uint32_t sys_evt)
{
}


void ble_stack_init(void)
{
    uint32_t err_code;

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, NULL);

    // Initialize the SoftDevice handler module.
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    // Enable BLE stack.
    ble_enable_params_t ble_enable_params;
    APP_ERROR_CHECK(softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                         PERIPHERAL_LINK_COUNT,
                                                         &ble_enable_params));

    ble_enable_params.gatts_enable_params.service_changed = 0;

    // Change the number of uuids from the minimum (1) to the default (10);
    ble_enable_params.common_enable_params.vs_uuid_count = BLE_UUID_VS_COUNT_DEFAULT;

    // Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);

    // Enable BLE stack.
    APP_ERROR_CHECK(softdevice_enable(&ble_enable_params));

    // Register with the SoftDevice handler module for BLE events.
    APP_ERROR_CHECK(softdevice_ble_evt_handler_set(ble_evt_dispatch));

    // Register with the SoftDevice handler module for BLE events.
    APP_ERROR_CHECK(softdevice_sys_evt_handler_set(sys_evt_dispatch));

    m_ble_stack_state = BLE_STACK_STATE_INIT;
}


void ble_stack_tasks(void)
{
    switch (m_ble_stack_state)
    {
        case BLE_STACK_STATE_INIT:
        {
            m_ble_stack_state = BLE_STACK_STATE_READY;
            break;
        }
        case BLE_STACK_STATE_READY:
        {
            break;
        }
        case BLE_STACK_STATE_ERROR:
        {
            break;
        }
    }
}


/** @} */
