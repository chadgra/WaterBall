/**
 * @file
 * @defgroup WaterBall discovery.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall discovery module.
 */

#include "app_util.h"
#include "buttons.h"
#include "connect.h"
#include "discovery.h"
#include "game.h"
#include "status.h"
#include "string.h"

static discovery_state_t        m_discovery_state;

void discovery_init(void)
{
    m_discovery_state = DISCOVERY_STATE_INIT;
}


void discovery_tasks(void)
{
    switch (m_discovery_state)
    {
        case DISCOVERY_STATE_INIT:
            m_discovery_state = DISCOVERY_STATE_READY;
            if (!buttons_is_pushed(BUTTON_1))
            {
                m_discovery_state = DISCOVERY_STATE_DISCOVERING;
            }

            break;
        case DISCOVERY_STATE_READY:
            break;
        case DISCOVERY_STATE_DISCOVERING:
            if (IS_IDLE)
            {
                discovery_discovery();
            }

            break;
        case DISCOVERY_STATE_ERROR:
            break;        
        default:
            break;
    }
}


void discovery_on_ble_evt(ble_evt_t const * p_scan_evt)
{
    switch(p_scan_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            break;
        case BLE_GAP_EVT_ADV_REPORT:
        {
            char game_service_uuid[] = GAME_BASE_UUID_128;
            ble_gap_evt_adv_report_t * adv_report = (ble_gap_evt_adv_report_t *)&p_scan_evt->evt.gap_evt.params.adv_report;
            char * p_service_start = memchr(adv_report->data, game_service_uuid[0], adv_report->dlen);
            if (NULL != p_service_start)
            {
                int result = memcmp(p_service_start, game_service_uuid, sizeof(game_service_uuid));
                if (0 == result)
                {
                    // This device has the service, try to connect.
                    connect_connect(&(adv_report->peer_addr));
                }
            }

            break;
        }
        case BLE_GAP_EVT_TIMEOUT:
        {
            if (BLE_GAP_TIMEOUT_SRC_SCAN == p_scan_evt->evt.gap_evt.params.timeout.src)
            {
                status_clear(STATUS_DISCOVERING);
            }

            break;
        }
    }

}


bool discovery_discovery(void)
{
    if (IS_DISCOVERING || IS_CONNECTING)
    {
        return true;
    }

    ble_gap_scan_params_t scan_settings = { 0 };
    scan_settings.active = true;
    scan_settings.selective = false;
    scan_settings.interval = MSEC_TO_UNITS(10, UNIT_0_625_MS);
    scan_settings.window = MSEC_TO_UNITS(10, UNIT_0_625_MS);
    scan_settings.timeout = BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED;

    uint32_t err_code = sd_ble_gap_scan_start(&scan_settings);
    status_set(STATUS_DISCOVERING);

    return true;
}


void discovery_cancel(void)
{
        if (!IS_DISCOVERING)
        {
            return;
        }

        status_clear(STATUS_DISCOVERING);
        sd_ble_gap_scan_stop();
}

/** @} */
