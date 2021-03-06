/**
 * @file
 * @defgroup WaterBall buttons.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall buttons module.
 */

#include "app_error.h"
#include "buttons.h"
#include "game.h"

static buttons_state_t  m_buttons_state;
static app_button_cfg_t m_buttons[] =
{
    { BUTTON_1, ACTIVE_STATE, PULL_CONFIGURATION, button_event_handler },
    { BUTTON_2, ACTIVE_STATE, PULL_CONFIGURATION, game_event_handler },
    { BUTTON_3, ACTIVE_STATE, PULL_CONFIGURATION, game_event_handler },
    { BUTTON_4, ACTIVE_STATE, PULL_CONFIGURATION, game_event_handler }
};


static void button_event_handler(uint8_t pin_number, uint8_t button_action)
{
    if (APP_BUTTON_RELEASE == button_action)
    {
        return;
    }

    switch (pin_number)
    {
        case BUTTON_2:
            LEDS_INVERT(BSP_LED_1_MASK);
            break;
        case BUTTON_3:
            LEDS_INVERT(BSP_LED_2_MASK);
            break;
        default:
            break;
    }
}


void buttons_init(void)
{
    APP_ERROR_CHECK(app_button_init(m_buttons, NUM_BUTTONS, BUTTON_DETECTION_DELAY));
    APP_ERROR_CHECK(app_button_enable());
    m_buttons_state = BUTTONS_STATE_INIT;
}


void buttons_tasks(void)
{
    switch (m_buttons_state)
    {
        case BUTTONS_STATE_INIT:
        {
            m_buttons_state = BUTTONS_STATE_READY;
            break;
        }
        case BUTTONS_STATE_READY:
        {
            break;
        }
        case BUTTONS_STATE_ERROR:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}


bool buttons_is_pushed(uint32_t pin_no)
{
    bool result;
    uint32_t index = 0;

    for (; index < NUM_BUTTONS; index++)
    {
        if (pin_no == m_buttons[index].pin_no)
        {
            break;
        }
    }

    if (NUM_BUTTONS <= index)
    {
        return false;
    }

    APP_ERROR_CHECK(app_button_is_pushed(index, &result));
    return result;
}

/** @} */
