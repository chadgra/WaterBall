/**
 * @file
 * @defgroup WaterBall service_server.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall game server module.
 */

#include "app_error.h"
#include "app_util_platform.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "service.h"
#include "service_server.h"
#include "sdk_common.h"

#define NUM_CHARACTERISTICS     (sizeof(m_characteristics) / sizeof(m_characteristics[0]))

static service_server_state_t   m_service_server_state;
static uint16_t                 m_service_handle;
static uint16_t                 m_conn_handle;
static service_info_t           m_info = { 0 };
static uint32_t                 m_server_score = 0;
static uint32_t                 m_client_score = 0;
static uint32_t                 m_current_time = 0;
static uint32_t                 m_game_state = 0;
static uint32_t                 m_game_time = 60000;
static uint32_t                 m_vibration = 1;
static uint32_t                 m_hole = UINT32_MAX;
static uint32_t                 m_target_score = 0;

static service_server_characteristic_t m_characteristics[] =
{
    { SERVICE_UUID(SERVICE_INFO_UUID),          sizeof(m_info),         service_server_read_info,           NULL,                               NULL,                           PROPERTY_READ },
    { SERVICE_UUID(SERVICE_SERVER_SCORE_UUID),  sizeof(m_server_score), service_server_read_server_score,   NULL,                               &m_info.server_score_handle,    PROPERTY_READ | PROPERTY_INDICATE },
    { SERVICE_UUID(SERVICE_CLIENT_SCORE_UUID),  sizeof(m_client_score), service_server_read_client_score,   service_server_write_client_score,  &m_info.client_score_handle,    PROPERTY_READ | PROPERTY_WRITE },
    { SERVICE_UUID(SERVICE_GAME_STATE_UUID),    sizeof(m_game_state),   service_server_read_game_state,     NULL,                               &m_info.game_state_handle,      PROPERTY_READ | PROPERTY_INDICATE },
    { SERVICE_UUID(SERVICE_CURRENT_TIME_UUID),  sizeof(m_current_time), service_server_read_current_time,   service_server_write_current_time,  &m_info.current_time_handle,    PROPERTY_READ | PROPERTY_WRITE | PROPERTY_INDICATE },
    { SERVICE_UUID(SERVICE_GAME_TIME_UUID),     sizeof(m_game_time),    service_server_read_game_time,      service_server_write_game_time,     &m_info.game_time_handle,       PROPERTY_READ | PROPERTY_WRITE },
    { SERVICE_UUID(SERVICE_VIBRATION_UUID),     sizeof(m_vibration),    service_server_read_vibration,      service_server_write_vibration,     &m_info.vibration_handle,       PROPERTY_READ | PROPERTY_WRITE },
    { SERVICE_UUID(SERVICE_HOLE_UUID),          sizeof(m_hole),         service_server_read_hole,           service_server_write_hole,          &m_info.hole_handle,            PROPERTY_READ | PROPERTY_WRITE },
    { SERVICE_UUID(SERVICE_TARGET_SCORE_UUID),  sizeof(m_target_score), service_server_read_target_score,   service_server_write_target_score,  &m_info.target_score_handle,    PROPERTY_READ | PROPERTY_WRITE },
};


void service_server_on_ble_evt(ble_evt_t * p_ble_evt)
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
            m_service_server_state = SERVICE_SERVER_STATE_READY;
            break;
        }
        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t * auth = &p_ble_evt->evt.gatts_evt.params.authorize_request;
            if (BLE_GATTS_AUTHORIZE_TYPE_READ == auth->type)
            {
                service_server_characteristic_t *  characteristic = service_server_get_characteristic(auth->request.read.uuid);
                if ((NULL == characteristic) || (NULL == characteristic->read_callback))
                {
                    break;
                }

                characteristic->read_callback(p_ble_evt);
            }

            if (BLE_GATTS_AUTHORIZE_TYPE_WRITE == auth->type)
            {
                service_server_characteristic_t *  characteristic = service_server_get_characteristic(auth->request.write.uuid);
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


void service_server_init(void)
{
    ble_uuid_t    service_uuid;
    ble_uuid128_t base_uuid = SERVICE_BASE_UUID_128;

    service_uuid.uuid = SERVICE_BASE_UUID;
    APP_ERROR_CHECK(sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type));

    m_conn_handle = BLE_CONN_HANDLE_INVALID;

    // Add the service.
    APP_ERROR_CHECK(sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                             &service_uuid,
                                             &m_service_handle));

    service_server_characteristic_t * characteristic = &m_characteristics[0];
    for (int i = 0; i < NUM_CHARACTERISTICS; i++)
    {
        service_server_characteristic_add(characteristic++);
    }

    m_service_server_state = SERVICE_SERVER_STATE_INIT;
}


void service_server_tasks(void)
{
    static int cycle_count = 0;

    switch (m_service_server_state)
    {
        case SERVICE_SERVER_STATE_INIT:
        {
            m_service_server_state = SERVICE_SERVER_STATE_READY;
            break;
        }
        case SERVICE_SERVER_STATE_READY:
        {
            cycle_count = 0;
            break;
        }
        case SERVICE_SERVER_STATE_CONNECTING:
        {
            // We don't want to get stuck in this state forever.
            cycle_count++;
            if (MAX_CONNECTING_CYCLES < cycle_count)
            {
                m_service_server_state = SERVICE_SERVER_STATE_READY;
            }

            break;
        }
        case SERVICE_SERVER_STATE_CONNECTED:
        {
            break;
        }
        case SERVICE_SERVER_STATE_ERROR:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}


bool service_server_is_connected(void)
{
    return m_service_server_state == SERVICE_SERVER_STATE_CONNECTED;
}


static void service_server_hvx_send(uint8_t type, uint16_t handle, uint16_t len, void * p_value)
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


uint32_t service_server_get_client_score(void)
{
    return m_client_score;
}


uint32_t service_server_get_current_time(void)
{
    return m_current_time;
}


uint32_t service_server_get_game_time(void)
{
    return m_game_time;
}


uint32_t service_server_get_vibration(void)
{
    return m_vibration;
}


uint32_t service_server_get_hole(void)
{
    return m_hole;
}


uint32_t service_server_get_target_score(void)
{
    return m_target_score;
}


void service_server_indicate_server_score(uint32_t score)
{
    m_server_score = score;
    service_server_hvx_send(BLE_GATT_HVX_INDICATION, m_info.server_score_handle, sizeof(m_server_score), &m_server_score);
}


void service_server_indicate_game_time(uint32_t ms_remaining)
{
    service_server_hvx_send(BLE_GATT_HVX_INDICATION, m_info.game_time_handle, sizeof(ms_remaining), &ms_remaining);
}


static void service_server_read_info(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_read_t * read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
    service_server_read_request_response(read->offset, sizeof(m_info), &m_info);
    m_service_server_state = SERVICE_SERVER_STATE_CONNECTED;
}


static void service_server_read_server_score(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_read_t * read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
    service_server_read_request_response(read->offset, sizeof(m_server_score), &m_server_score);
}


static void service_server_read_client_score(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_read_t * read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
    service_server_read_request_response(read->offset, sizeof(m_client_score), &m_client_score);
}


static void service_server_read_game_time(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_read_t * read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
    service_server_read_request_response(read->offset, sizeof(m_game_time), &m_game_time);
}


static void service_server_read_vibration(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_read_t * read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
    service_server_read_request_response(read->offset, sizeof(m_vibration), &m_vibration);
}


static void service_server_read_hole(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_read_t * read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
    service_server_read_request_response(read->offset, sizeof(m_hole), &m_hole);
}


static void service_server_read_current_time(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_read_t * read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
    service_server_read_request_response(read->offset, sizeof(m_current_time), &m_current_time);
}


static void service_server_read_target_score(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_read_t * read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
    service_server_read_request_response(read->offset, sizeof(m_target_score), &m_target_score);
}


static void service_server_read_game_state(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_read_t * read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
    service_server_read_request_response(read->offset, sizeof(m_game_state), &m_game_state);
}


static void service_server_write_client_score(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * write = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.write;
    m_client_score = *(uint32_t *)write->data;
    service_server_write_request_response(BLE_GATT_STATUS_SUCCESS);
}


static void service_server_write_game_time(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * write = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.write;
    m_game_time = *(uint32_t *)write->data;
    service_server_write_request_response(BLE_GATT_STATUS_SUCCESS);
}


static void service_server_write_vibration(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * write = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.write;
    m_vibration = *(uint32_t *)write->data;
    service_server_write_request_response(BLE_GATT_STATUS_SUCCESS);
}


static void service_server_write_hole(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * write = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.write;
    m_hole = *(uint32_t *)write->data;
    service_server_write_request_response(BLE_GATT_STATUS_SUCCESS);
}


static void service_server_write_current_time(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * write = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.write;
    m_current_time = *(uint32_t *)write->data;
    service_server_write_request_response(BLE_GATT_STATUS_SUCCESS);
}


static void service_server_write_target_score(ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * write = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.write;
    m_target_score = *(uint32_t *)write->data;
    service_server_write_request_response(BLE_GATT_STATUS_SUCCESS);
}


static void service_server_write_request_response(uint16_t gatt_status)
{
    ble_gatts_rw_authorize_reply_params_t reply;
    memset(&reply, 0, sizeof(reply));

    reply.type                      = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
    reply.params.write.gatt_status  = gatt_status;
    reply.params.write.update       = true;

    APP_ERROR_CHECK(sd_ble_gatts_rw_authorize_reply(m_conn_handle, &reply));
}


static void service_server_read_request_response(uint16_t offset, uint16_t len, void * p_data)
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


static void service_server_characteristic_add(service_server_characteristic_t * characteristic)
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
    attr_char_value.max_len   = characteristic->max_length;

    APP_ERROR_CHECK(sd_ble_gatts_characteristic_add(m_service_handle,
                                                    &char_md,
                                                    &attr_char_value,
                                                    &characteristic->handles));

    // Put the handles into our info table if we have a pointer.
    if (NULL != characteristic->character_handle)
    {
        *characteristic->character_handle = characteristic->handles.value_handle;
    }
}


static service_server_characteristic_t * service_server_get_characteristic(ble_uuid_t uuid)
{
    service_server_characteristic_t *  characteristic = &m_characteristics[0];
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
