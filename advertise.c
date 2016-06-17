/**
 * @file
 * @defgroup WaterBall advertise.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall ADVERTISE module.
 */

#include "advertise.h"
#include "app_error.h"
#include "ble_advdata.h"
#include "ble_stack.h"
#include "game.h"
#include "status.h"


static advertise_state_t    m_advertise_state;

void advertise_init(void)
{
    m_advertise_state = ADVERTISE_STATE_INIT;
}


void advertise_tasks(void)
{
    switch (m_advertise_state)
    {
        case ADVERTISE_STATE_INIT:
            // Start advertising if Button 1 is held down on startup.
//            if (m_start_on_idle && IS_IDLE && !m_pause_idle_start)
            {
                advertise_advertise();
            }

            m_advertise_state = ADVERTISE_STATE_READY;
            break;
        case ADVERTISE_STATE_READY:
            break;
        case ADVERTISE_STATE_ERROR:
            break;
        default:
            break;
    }
}


void advertise_on_ble_evt(ble_evt_t const * p_adv_data_evt)
{
    switch(p_adv_data_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            if (IS_ADVERTISING)
            {
                advertise_cancel();
            }

            break;
        }
        case BLE_GAP_EVT_DISCONNECTED:
        {
            break;
        }
        case BLE_GAP_EVT_TIMEOUT:
        {
            if (BLE_GAP_TIMEOUT_SRC_ADVERTISING == p_adv_data_evt->evt.gap_evt.params.timeout.src)
            {
                status_clear(STATUS_ADVERTISING);
            }

            break;
        }
    }
}


bool advertise_advertise(void)
{
    if (IS_CONNECTING)
    {
        return false;
    }

    //If advertising before, stop it.
    if (IS_ADVERTISING)
    {
        sd_ble_gap_adv_stop();
    }

    advertise_set_data();

    ble_gap_adv_params_t adv_settings = { 0 };
    adv_settings.type = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_settings.fp = BLE_GAP_ADV_FP_ANY;
    adv_settings.interval = APP_ADV_INTERVAL;
    adv_settings.timeout = APP_ADV_TIMEOUT_IN_SECONDS;

    APP_ERROR_CHECK(sd_ble_gap_adv_start(&adv_settings));
    status_set(STATUS_ADVERTISING);
    return true;
}


void advertise_cancel(void)
{
    if (!IS_ADVERTISING)
    {
        return;
    }

    status_clear(STATUS_ADVERTISING);
    sd_ble_gap_adv_stop();
}


static void advertise_set_data(void)
{
    // Default advertising data if none has been explicitly set.
    ble_advdata_t advdata;
    memset(&advdata, 0, sizeof(ble_advdata_t));

    // Flags.
    advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    // ServicesCompleteListUuid128.
    ble_uuid_t uuid;
    uuid.uuid = GAME_BASE_UUID;
    uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN;
    advdata.uuids_complete.uuid_cnt = 1;
    advdata.uuids_complete.p_uuids = &uuid;

    APP_ERROR_CHECK(ble_advdata_set(&advdata, NULL));
}
