/**
 * @file
 *
 * @defgroup ir_led module
 * @{
 * @ingroup application
 *
 * @brief ir_led module interface.
 */
 
#ifndef IR_LED_H__
#define IR_LED_H__

#include <stdint.h>
#include "app_pwm.h"
#include "ble.h"

/**
 * @brief gpio module states.
 */
typedef enum
{
    IR_LED_STATE_INIT,                     /**< The led module is initializing. */
    IR_LED_STATE_READY,                    /**< The led module is ready. */
    IR_LED_STATE_ERROR                     /**< The led module has received an error. */
} ir_led_state_t;

/**
 * @brief Function to initialize the ir_led module.
 */
void ir_led_init(void);

/**
 * @brief Function to accomplish the ir_led tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void ir_led_tasks(void);

/**
 * @brief   Set the pwm.
 *
 * @param[in]   physical_pin    The pin the pwm will be on.
 * @param[in]   duty_cycle      The duty cycle to set.
 * @param[in]   period          The period to set.
 */
static void ir_led_set_pwm(uint32_t physical_pin, uint32_t duty_cycle, uint32_t period);

#endif
