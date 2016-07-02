/**
 * @file
 * @defgroup WaterBall service.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall service module.
 */

#include "app_button.h"
#include "app_error.h"
#include "ble_srv_common.h"
#include "service.h"
#include "service_client.h"
#include "service_server.h"
#include "sdk_common.h"

static service_state_t         m_service_state;


void service_on_ble_evt(ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            break;
        }
        case BLE_GAP_EVT_DISCONNECTED:
        {
            break;
        }
    }

    service_client_on_ble_evt(p_ble_evt);
    service_server_on_ble_evt(p_ble_evt);
}


void service_init(void)
{
    service_client_init();
    service_server_init();
    m_service_state = SERVICE_STATE_INIT;
}


void service_tasks(void)
{
    switch (m_service_state)
    {
        case SERVICE_STATE_INIT:
        {
            m_service_state = SERVICE_STATE_READY;
            break;
        }
        case SERVICE_STATE_READY:
        {
            break;
        }
        case SERVICE_STATE_ERROR:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    service_client_tasks();
    service_server_tasks();
}


void service_try_connect(void)
{
    service_client_try_connect();
}


bool service_is_client(void)
{
    return service_client_is_connected();
}


bool service_is_server(void)
{
    return service_server_is_connected();
}

/** @} */
