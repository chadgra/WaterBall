/**
 * @file
 * @defgroup WaterBall ble_stack.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall BLE module.
 */

#include <stdint.h>
#include <string.h>

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
#include "version.h"

#define NUM_CHARACTERISTICS             (sizeof(m_characteristics) / sizeof(m_characteristics[0]))

static ble_stack_state_t                       m_ble_stack_state;
static uint16_t                                m_device_information_service_handle;
static ble_device_information_characteristic_t m_characteristics[] =
{
    { BLE_UUID_FIRMWARE_REVISION_STRING_CHAR,   sizeof(FIRMWARE_VERSION),   FIRMWARE_VERSION }
};


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

    // Add the device information service.
    ble_device_information_service_init();

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


static void ble_device_information_service_init(void)
{
    ble_uuid_t service_uuid = { BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE };

    // Add the service.
    APP_ERROR_CHECK(sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                             &service_uuid,
                                             &m_device_information_service_handle));

    ble_device_information_characteristic_t * characteristic = &m_characteristics[0];
    for (int i = 0; i < NUM_CHARACTERISTICS; i++)
    {
        ble_device_information_characteristic_add(characteristic++);
    }
}


static void ble_device_information_characteristic_add(ble_device_information_characteristic_t * characteristic)
{
    // Add read/write properties to our characteristic.
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = true;

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

    // Set read/write security levels to our characteristic.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    if (char_md.char_props.read)
    {
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    }

    // Configure the characteristic value attribute.
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          service_uuid = { characteristic->uuid, BLE_UUID_TYPE_BLE };
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid    = &service_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.max_len   = characteristic->length;
    attr_char_value.init_len  = characteristic->length;
    attr_char_value.p_value   = (uint8_t *)characteristic->read_value;

    ble_gatts_char_handles_t handles;
    APP_ERROR_CHECK(sd_ble_gatts_characteristic_add(m_device_information_service_handle,
                                                    &char_md,
                                                    &attr_char_value,
                                                    &handles));
}


/** @} */
