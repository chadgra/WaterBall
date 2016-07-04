/**
 * @file
 * @defgroup WaterBall ir_led.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall infra-red led module.
 *
 * This file contains the source code for the IR LED module.
 */

#include <stdbool.h>
#include <stdint.h>
#include "app_error.h"
#include "bsp.h"
#include "ir_led.h"
#include "nrf.h"
#include "nrf_gpio.h"


APP_PWM_INSTANCE(pwm0, 1);                   // Create the instance "PWM0" using TIMER0.
static ir_led_state_t      m_ir_led_state;


void ir_led_init(void)
{
    m_ir_led_state = IR_LED_STATE_INIT;
}


void ir_led_tasks(void)
{
    switch (m_ir_led_state)
    {
        case IR_LED_STATE_INIT:
        {
            ir_led_set_pwm(0, 50, 26);
            m_ir_led_state = IR_LED_STATE_READY;
            break;
        }
        case IR_LED_STATE_READY:
        {
            break;
        }
        case IR_LED_STATE_ERROR:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}


static void ir_led_set_pwm(uint32_t physical_pin, uint32_t duty_cycle, uint32_t us_period)
{
    //Uniniatialize PWM if there is any initialized. 
    app_pwm_uninit(&pwm0);

    //Configuring PWM single channel.
    app_pwm_config_t pwm0_cfg = APP_PWM_DEFAULT_CONFIG_1CH(us_period, physical_pin);

    //Initializing PWM.
    APP_ERROR_CHECK(app_pwm_init(&pwm0, &pwm0_cfg, NULL));

    //Enable PWM.
    app_pwm_enable(&pwm0);

    //Set duty cycle.
    while(app_pwm_channel_duty_set(&pwm0, 0, duty_cycle) == NRF_ERROR_BUSY);
}

