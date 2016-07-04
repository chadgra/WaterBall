/**
 * @file
 * @defgroup WaterBall watchdog.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall watchdog module.
 */

#include "app_error.h"
#include "app_timer.h"
#include "app_util_platform.h"
#include "ble_stack.h"
#include "nrf_drv_wdt.h"
#include "watchdog.h"


static watchdog_state_t         m_watchdog_state;
static nrf_drv_wdt_channel_id   m_channel_id;


static void watchdog_event_handler(void)
{
    // The watchdog timer fired, we were probably in an infinite loop.
    APP_ERROR_HANDLER(NRF_ERROR_BUSY);
}


void watchdog_init(void)
{
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    APP_ERROR_CHECK(nrf_drv_wdt_init(&config, watchdog_event_handler));
    APP_ERROR_CHECK(nrf_drv_wdt_channel_alloc(&m_channel_id));
    m_watchdog_state = WATCHDOG_STATE_INIT; 
}


void watchdog_tasks(void)
{
    switch (m_watchdog_state)
    {
        case WATCHDOG_STATE_INIT:
        {
            nrf_drv_wdt_enable();
            m_watchdog_state = WATCHDOG_STATE_RUN;
            break;
        }
        case WATCHDOG_STATE_RUN:
        {
            nrf_drv_wdt_channel_feed(m_channel_id);
            break;
        }
        case WATCHDOG_STATE_ERROR:
        {
            break;
        }
    }
}

/** @} */
