/**
 * @file
 * @defgroup WaterBall game_server.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall game server module.
 */

#include "app_error.h"
#include "app_util_platform.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "game.h"
#include "game_server.h"
#include "sdk_common.h"

#define NUM_CHARACTERISTICS             (sizeof(m_characteristics) / sizeof(m_characteristics[0]))

static game_server_state_t  m_game_server_state;
static uint16_t             m_service_handle;
static uint16_t             m_conn_handle;
static game_info_t          m_info = { 0 };


static game_server_characteristic_t m_characteristics[] =
{
    { GAME_UUID(GAME_SERVER_SCORE_UUID),    game_server_read_server_score,  NULL,                           &m_info.server_score_handle,    &m_info.server_score_config_handle, PROPERTY_READ | PROPERTY_INDICATE },
    { GAME_UUID(GAME_CLIENT_SCORE_UUID),    NULL,                           game_server_write_client_score, &m_info.client_score_handle,    NULL,                               PROPERTY_WRITE },
    { GAME_UUID(GAME_TIME_UUID),            game_server_read_game_time,     NULL,                           &m_info.game_time_handle,       &m_info.game_time_config_handle,    PROPERTY_READ | PROPERTY_INDICATE }
};


void game_server_on_ble_evt(ble_evt_t * p_ble_evt)
{
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
            m_game_server_state = GAME_SERVER_STATE_READY;
            break;
        }
        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t * auth = &p_ble_evt->evt.gatts_evt.params.authorize_request;
            if (BLE_GATTS_AUTHORIZE_TYPE_READ == auth->type)
            {
                game_server_characteristic_t *  characteristic = game_server_get_characteristic(auth->request.read.uuid);
                if ((NULL == characteristic) || (NULL == characteristic->read_callback))
                {
                    break;
                }

                characteristic->read_callback(p_ble_evt);
            }

            if (BLE_GATTS_AUTHORIZE_TYPE_WRITE == auth->type)
            {
                game_server_characteristic_t *  characteristic = game_server_get_characteristic(auth->request.write.uuid);
                if ((NULL == characteristic) || (NULL == characteristic->write_callback))
                {
                    break;
                }

                characteristic->write_callback(p_ble_evt);
            }

            break;
        }
        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
        {
            APP_ERROR_CHECK(sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0));
            break;
        }
        case BLE_GATTS_EVT_HVC:
        {
            // A response was received from an indication.
            break;
        }
        case BLE_GATTS_EVT_TIMEOUT:
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


void game_server_init(void)
{
    ble_uuid_t    service_uuid;
    ble_uuid128_t base_uuid = GAME_BASE_UUID_128;

    service_uuid.uuid = GAME_BASE_UUID;
    APP_ERROR_CHECK(sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type));

    m_conn_handle = BLE_CONN_HANDLE_INVALID;

    // Add the service.
    APP_ERROR_CHECK(sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                             &service_uuid,
                                             &m_service_handle));

    game_server_characteristic_t * characteristic = &m_characteristics[0];
    for (int i = 0; i < NUM_CHARACTERISTICS; i++)
    {
        game_server_characteristic_add(characteristic++);
    }

    m_game_server_state = GAME_SERVER_STATE_INIT;
}


void game_server_tasks(void)
{
    static int cycle_count = 0;

    switch (m_game_server_state)
    {
        case GAME_SERVER_STATE_INIT:
        {
            m_game_server_state = GAME_SERVER_STATE_READY;
            break;
        }
        case GAME_SERVER_STATE_READY:
        {
            cycle_count = 0;
            break;
        }
        case GAME_SERVER_STATE_CONNECTING:
        {
            // We don't want to get stuck in this state forever.
            cycle_count++;
            if (MAX_CONNECTING_CYCLES < cycle_count)
            {
                m_game_server_state = GAME_SERVER_STATE_READY;
            }

            break;
        }
        case GAME_SERVER_STATE_CONNECTED:
        {
            break;
        }
        case GAME_SERVER_STATE_ERROR:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}


bool game_server_is_connected(void)
{
    return m_game_server_state == GAME_SERVER_STATE_CONNECTED;
}


static void game_server_hvx_send(uint8_t type, uint16_t handle, uint16_t len, void * p_value)
{
    if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return;
    }

    ble_gatts_hvx_params_t hvx_params;
    memset(&hvx_params, 0, sizeof(hvx_params));
    hvx_params.handle = handle;
    hvx_params.type   = type;
    hvx_params.p_len  = &len;
    hvx_params.p_data = p_value;

    uint32_t err_code = sd_ble_gatts_hvx(m_conn_handle, &hvx_params);
    if (NRF_ERROR_INVALID_STATE != err_code)
    {
        APP_ERROR_CHECK(err_code);
    }
}


void game_server_indicate_server_score(uint32_t score)
{
    game_server_hvx_send(BLE_GATT_HVX_INDICATION, m_info.server_score_handle, sizeof(score), &score);
}


void game_server_indicate_game_time(uint32_t ms_remaining)
{
    game_server_hvx_send(BLE_GATT_HVX_INDICATION, m_info.game_time_handle, sizeof(ms_remaining), &ms_remaining);
}


static void game_server_read_server_score(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_read_t * read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
    uint32_t score = 0;
    game_server_read_request_response(read->offset, sizeof(score), &score);
}


static void game_server_read_game_time(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_read_t * read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
    uint32_t time = 0;
    game_server_read_request_response(read->offset, sizeof(time), &time);
}


static void game_server_write_client_score(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * write = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.write;
    uint32_t score = *write->data;
    game_server_write_request_response(BLE_GATT_STATUS_SUCCESS);
}


static void game_server_write_request_response(uint16_t gatt_status)
{
    ble_gatts_rw_authorize_reply_params_t reply;
    memset(&reply, 0, sizeof(reply));

    reply.type                      = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
    reply.params.write.gatt_status  = gatt_status;
    reply.params.write.update       = true;

    APP_ERROR_CHECK(sd_ble_gatts_rw_authorize_reply(m_conn_handle, &reply));
}


static void game_server_read_request_response(uint16_t offset, uint16_t len, void * p_data)
{
    ble_gatts_rw_authorize_reply_params_t reply;
    memset(&reply, 0, sizeof(reply));

    reply.type                      = BLE_GATTS_AUTHORIZE_TYPE_READ;
    reply.params.read.update        = true;
    reply.params.read.gatt_status   = BLE_GATT_STATUS_SUCCESS;
    reply.params.read.offset        = offset;
    reply.params.read.p_data        = ((uint8_t *)p_data) + offset;
    reply.params.read.len           = len - offset;

    APP_ERROR_CHECK(sd_ble_gatts_rw_authorize_reply(m_conn_handle, &reply));
}


static void game_server_characteristic_add(game_server_characteristic_t * characteristic)
{
    // Add read/write properties to our characteristic.
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props        = *((ble_gatt_char_props_t *)&characteristic->properties);

    // Configure Client Characteristic Configuration Descriptor metadata and add to char_md structure.
    if ((char_md.char_props.notify) || (char_md.char_props.indicate))
    {
        ble_gatts_attr_md_t cccd_md;
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
        cccd_md.vloc              = BLE_GATTS_VLOC_STACK;
        char_md.p_cccd_md         = &cccd_md;
    }

    // Configure the attribute metadata.
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.vlen    = true;
    attr_md.rd_auth = true;
    attr_md.wr_auth = true;

    // Set read/write security levels to our characteristic.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    if (char_md.char_props.read)
    {
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    }

    // Configure the characteristic value attribute.
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid    = &characteristic->uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.max_len   = sizeof(uint32_t);

    APP_ERROR_CHECK(sd_ble_gatts_characteristic_add(m_service_handle,
                                                    &char_md,
                                                    &attr_char_value,
                                                    &characteristic->handles));

    // Put the handles into our info table if we have a pointer.
    if (NULL != characteristic->character_handle)
    {
        *characteristic->character_handle = characteristic->handles.value_handle;
    }

    if (NULL != characteristic->character_config_handle)
    {
        *characteristic->character_config_handle = characteristic->handles.cccd_handle;
    }
}


static game_server_characteristic_t * game_server_get_characteristic(ble_uuid_t uuid)
{
    game_server_characteristic_t *  characteristic = &m_characteristics[0];
    for (int i = 0; i < NUM_CHARACTERISTICS; i++)
    {
        if (characteristic->uuid.uuid == uuid.uuid)
        {
            return characteristic;
        }

        characteristic++;
    }

    return NULL;
}

/** @} */
