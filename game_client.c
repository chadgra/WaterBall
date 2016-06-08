/**
 * @file
 * @defgroup WaterBall game_client.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall game client module.
 */

#include "app_error.h"
#include "app_util_platform.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "game.h"
#include "game_client.h"
#include "sdk_common.h"

static game_client_state_t  m_game_client_state;
static ble_uuid_t           m_service_uuid = { GAME_BASE_UUID, BLE_UUID_TYPE_VENDOR_BEGIN };
static uint16_t             m_service_handle;
static uint16_t             m_conn_handle;
static game_info_t          m_info = { 0 };


void game_client_on_ble_evt(ble_evt_t * p_ble_evt)
{
    ble_gattc_evt_t * p_ble_gattc_evt = &p_ble_evt->evt.gattc_evt;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
        }
        case BLE_GAP_EVT_DISCONNECTED:
        {
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            m_game_client_state = GAME_CLIENT_STATE_READY;
            break;
        }
        case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
        {
            if (BLE_GATT_STATUS_SUCCESS == p_ble_gattc_evt->gatt_status)
            {
                // If we found it then save the handle and search again - to make sure we got the last one.
                ble_gattc_evt_prim_srvc_disc_rsp_t * p_prim_srvc_disc_rsp = &p_ble_gattc_evt->params.prim_srvc_disc_rsp;
                uint16_t count = p_prim_srvc_disc_rsp->count;
                if (0 == count)
                {
                    return;
                }

                m_service_handle = p_prim_srvc_disc_rsp->services[count - 1].handle_range.start_handle;
                sd_ble_gattc_primary_services_discover(m_conn_handle, m_service_handle + 1, &m_service_uuid);
            }
            else if (BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND == p_ble_gattc_evt->gatt_status)
            {
                // If we didn't find the service, but we found one previously then we are connected.
                if (BLE_CONN_HANDLE_INVALID != m_service_handle)
                {
                }
                else
                {
                    // We never found the service so just go back to the GAME_CLIENT_STATE_READY state - connection failed.
                    m_game_client_state = GAME_CLIENT_STATE_READY;
                }
            }

            break;
        }
        case BLE_GATTC_EVT_READ_RSP:
        {
            // Enable server score and game time indicate.
            uint16_t write_value = BLE_GATT_HVX_INDICATION;
            game_client_write(BLE_GATT_OP_WRITE_CMD, m_info.server_score_config_handle, sizeof(write_value), &write_value);
            game_client_write(BLE_GATT_OP_WRITE_CMD, m_info.game_time_config_handle, sizeof(write_value), &write_value);

            break;
        }
        case BLE_GATTC_EVT_WRITE_RSP:
        {
//            ble_gattc_evt_write_rsp_t * p_write_rsp = &p_ble_gattc_evt->params.write_rsp;
            break;
        }
        case BLE_GATTC_EVT_HVX:
        {
            ble_gattc_evt_hvx_t * p_hvx = &p_ble_gattc_evt->params.hvx;
            if (p_hvx->handle == m_info.server_score_handle)
            {
                game_store_read_data(p_hvx->data, p_hvx->len);
            }
            else if (p_hvx->handle == m_info.game_time_handle)
            {
                game_store_read_data(p_hvx->data, p_hvx->len);
            }

            if (BLE_GATT_HVX_INDICATION == p_hvx->type)
            {
                // Send confirmation that we received the indicate message.
                sd_ble_gattc_hv_confirm(m_conn_handle, p_hvx->handle);
            }

            break;
        }
        case BLE_GATTC_EVT_TIMEOUT:
        {
            // Something has timed out - Bluetooth Spec 4.1, Volume 3, Part F, Chapter 2 states
            // that we must now break the connection.
            APP_ERROR_CHECK(sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION));
            break;
        }
        default:
        {
            break;
        }
    }
}


void game_client_init(void)
{
    m_conn_handle = BLE_CONN_HANDLE_INVALID;
    m_game_client_state = GAME_CLIENT_STATE_INIT;
}


void game_client_tasks(void)
{
    switch (m_game_client_state)
    {
        case GAME_CLIENT_STATE_INIT:
        {
            m_game_client_state = GAME_CLIENT_STATE_READY;
            break;
        }
        case GAME_CLIENT_STATE_READY:
        {
            break;
        }
        case GAME_CLIENT_STATE_CONNECTING:
        {
            break;
        }
        case GAME_CLIENT_STATE_CONNECTED:
        {
            break;
        }
        case GAME_CLIENT_STATE_ERROR:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}


bool game_client_is_connected(void)
{
    return m_game_client_state == GAME_CLIENT_STATE_CONNECTED;
}


void game_client_try_connect(void)
{
    if (BLE_CONN_HANDLE_INVALID == m_conn_handle)
    {
        return;
    }

    APP_ERROR_CHECK(sd_ble_gattc_primary_services_discover(m_conn_handle, GAME_CLIENT_START_HANDLE, &m_service_uuid));
    m_game_client_state = GAME_CLIENT_STATE_CONNECTING;
}


static void game_client_write(uint8_t write_op, uint16_t handle, uint16_t len, void * p_value)
{
    if (0 == len)
    {
        return;
    }

    ble_gattc_write_params_t write_params;
    memset(&write_params, 0, sizeof(write_params));

    write_params.write_op   = write_op;
    write_params.flags      = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE;
    write_params.handle     = handle;
    write_params.len        = len;
    write_params.p_value    = (uint8_t *)p_value;

    // Keep trying to send the message if it is busy.
    uint32_t err_code = NRF_SUCCESS;
    do
    {
        err_code = sd_ble_gattc_write(m_conn_handle, &write_params);
    } while (NRF_ERROR_BUSY == err_code);
    APP_ERROR_CHECK(err_code);
}

/** @} */
