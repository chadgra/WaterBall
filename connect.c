/**
 * @file
 * @defgroup WaterBall connect.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall connect module.
 */

#include <string.h>
#include "advertise.h"
#include "ble_hci.h"
#include "clock.h"
#include "connect.h"
#include "discovery.h"
#include "service.h"
#include "status.h"

static connect_state_t      m_connect_state;
static uint16_t             m_conn_handle = BLE_CONN_HANDLE_INVALID;

void connect_init(void)
{
    m_connect_state = CONNECT_STATE_INIT;
}


void connect_tasks(void)
{
    switch (m_connect_state)
    {
        case CONNECT_STATE_INIT:
            default_conn_params();
            m_connect_state = CONNECT_STATE_READY;
            break;
        case CONNECT_STATE_READY:
            break;
        case CONNECT_STATE_ERROR:
            break;
        default:
            break;
    }
}


void connect_on_ble_evt(ble_evt_t const * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            const ble_gap_evt_connected_t * connected = &p_ble_evt->evt.gap_evt.params.connected;
            status_set_role(connected->role);
            status_clear(STATUS_CONNECTING);
            status_set(STATUS_CONNECTED);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

            if (IS_CENTRAL)
            {
                // If this is the central then try to connect as the BRSP client.
                service_try_connect();
            }

            break;
        }
        case BLE_GAP_EVT_DISCONNECTED:
        {
            // Clear connection handle.
            status_clear(STATUS_CONNECTED);
            status_set_role(BLE_GAP_ROLE_INVALID);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;
        }
        case BLE_GAP_EVT_TIMEOUT:
        {
            if (BLE_GAP_TIMEOUT_SRC_CONN == p_ble_evt->evt.gap_evt.params.timeout.src)
            {
                status_clear(STATUS_CONNECTING);
            }

            break;
        }
        default:
        {
            break;
        }
    }
}


bool connect_connect(ble_gap_addr_t * p_address)
{
    ble_gap_scan_params_t   scan_params;
    ble_gap_conn_params_t   conn_params;

    if (IS_CONNECTED)
    {
        // Can't connect if we are already connected.
        return true;
    }

    scan_params.selective           = false;
    scan_params.p_whitelist         = NULL;
    scan_params.active              = false;
    scan_params.interval            = MSEC_TO_UNITS(10, UNIT_0_625_MS);
    scan_params.window              = MSEC_TO_UNITS(10, UNIT_0_625_MS);
    scan_params.timeout             = BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED;

    conn_params.min_conn_interval   = MSEC_TO_UNITS(10, UNIT_1_25_MS);
    conn_params.max_conn_interval   = MSEC_TO_UNITS(20, UNIT_1_25_MS);
    conn_params.slave_latency       = 0;
    conn_params.conn_sup_timeout    = MSEC_TO_UNITS(400, UNIT_10_MS);

    // Cancel all ongoing commands.
    advertise_cancel();
    discovery_cancel();
    connect_cancel();

    APP_ERROR_CHECK(sd_ble_gap_connect(p_address, &scan_params, &conn_params));
    status_set(STATUS_CONNECTING);
    return true;
}


void connect_disconnect(void)
{
    if (!IS_CONNECTED)
    {
        return;
    }

    uint32_t err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    switch (err_code)
    {
        case NRF_ERROR_INVALID_STATE:
            return;
        default:
            APP_ERROR_CHECK(err_code);
            break;
    }

    return;
}


void connect_cancel(void)
{
    if (!IS_CONNECTING)
    {
        return;
    }

    status_clear(STATUS_CONNECTING);
    APP_ERROR_CHECK(sd_ble_gap_connect_cancel());
}


void default_conn_params(void)
{
    ble_gap_conn_params_t  conn_params;
    conn_params.min_conn_interval   = MSEC_TO_UNITS(10, UNIT_1_25_MS);
    conn_params.max_conn_interval   = MSEC_TO_UNITS(20, UNIT_1_25_MS);
    conn_params.slave_latency       = 0;
    conn_params.conn_sup_timeout    = MSEC_TO_UNITS(400, UNIT_10_MS);
    APP_ERROR_CHECK(sd_ble_gap_ppcp_set(&conn_params));
}


uint16_t connect_get_handle(void)
{
    return m_conn_handle;
}

/** @} */
