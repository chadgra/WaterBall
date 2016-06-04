/**
 * @file
 * @defgroup WaterBall buttons.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall buttons module.
 *
 * Setup buttons.
 */

#ifndef BUTTONS_H__
#define BUTTONS_H__

#include "app_button.h"
#include "app_timer.h"
#include "nrf_soc.h"

#define ACTIVE_STATE                APP_BUTTON_ACTIVE_LOW
#define PULL_CONFIGURATION          NRF_GPIO_PIN_PULLUP
#define BUTTON_DETECTION_DELAY      APP_TIMER_MIN_TIMEOUT_TICKS //APP_TIMER_TICKS(1, APP_TIMER_PRESCALER)

/**
 * @brief The number of mapping pins in mapping structure.
 */
#define NUM_BUTTONS                                         \
        sizeof(m_buttons) / sizeof(app_button_cfg_t)

/**
 * @brief gpio module states.
 */
typedef enum
{
    BUTTONS_STATE_INIT,             /**< The buttons module is initializing. */
    BUTTONS_STATE_READY,            /**< The buttons module is ready. */
    BUTTONS_STATE_ERROR             /**< The buttons module has received an error. */
} buttons_state_t;

/**
 * @brief   Event handler that runs when a button is pressed.
 *
 * @param[in]   pin_number      The number of the pin that was pressed.
 * @param[in]   button_action   The action (press or release) of the button.
 */
static void button_event_handler(uint8_t pin_number, uint8_t button_action);

/**
 * @brief   Function to initialize the sleep module.
 */
void buttons_init(void);

/**
 * @brief   Function to accomplish the sleep module tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void buttons_tasks(void);

/**
 * @brief   Function fo find the state of the button.
 *
 * @param[in]   index       The index of the button in the button structure.
 *
 * @retval      True if pushed.
 */
bool buttons_is_pushed(uint32_t index);

#endif //BUTTONS_H__

/** @} */
