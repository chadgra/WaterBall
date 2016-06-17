/**
 * @file
 * @defgroup WaterBall status.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall status.
 */

#include "bsp.h"
#include "status.h"

static uint32_t m_status = 0;
static uint8_t  m_role = BLE_GAP_ROLE_INVALID;

void status_init(void)
{
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
}


void status_tasks(void)
{
    if (IS_ADVERTISING)
    {
        LEDS_ON(BSP_LED_0_MASK);
    }
    else
    {
        LEDS_OFF(BSP_LED_0_MASK);
    }

    if (IS_CONNECTED)
    {
        LEDS_ON(BSP_LED_1_MASK);
    }
    else
    {
        LEDS_OFF(BSP_LED_1_MASK);
    }
}


void status_set(uint32_t status)
{
    m_status |= status;
}


void status_clear(uint32_t status)
{
    m_status &= ~status;
}


bool status_is_set(uint32_t status)
{
    return (bool)(m_status & status);
}


bool status_is_clear(uint32_t status)
{
    return !status_is_set(status);
}


void status_set_role(uint8_t role)
{
    m_role = role;
}


uint8_t status_get_role(void)
{
    return m_role;
}

/** @} */
