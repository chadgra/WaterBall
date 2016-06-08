/**
 * @file
 * @defgroup WaterBall game.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall game module.
 */

#include "app_button.h"
#include "app_error.h"
#include "ble_srv_common.h"
#include "game.h"
#include "game_client.h"
#include "game_server.h"
#include "sdk_common.h"

static game_state_t         m_game_state;


void game_on_ble_evt(ble_evt_t * p_ble_evt)
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

    game_client_on_ble_evt(p_ble_evt);
    game_server_on_ble_evt(p_ble_evt);
}


void game_button_event_handler(uint8_t pin_number, uint8_t button_action)
{
    if ((APP_BUTTON_PUSH == button_action) && (6 == pin_number))
    {
    }
}


void game_init(void)
{
    game_client_init();
    game_server_init();
    m_game_state = GAME_STATE_INIT;
}


void game_tasks(void)
{
    switch (m_game_state)
    {
        case GAME_STATE_INIT:
        {
            m_game_state = GAME_STATE_READY;
            break;
        }
        case GAME_STATE_READY:
        {
            break;
        }
        case GAME_STATE_ERROR:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    game_client_tasks();
    game_server_tasks();
}


uint32_t game_store_read_data(uint8_t * p_buffer, uint32_t size)
{
    return 0;
}


void game_try_connect(void)
{
    game_client_try_connect();
}


bool game_is_client(void)
{
    return game_client_is_connected();
}


bool game_is_server(void)
{
    return game_server_is_connected();
}

/** @} */
