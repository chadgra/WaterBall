/**
 * @file
 * @defgroup WaterBall clock.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall clock module.
 */

#ifndef CLOCK_H__
#define CLOCK_H__

#include <stdbool.h>
#include <stdint.h>

#include "app_timer.h"
#include "ble_stack.h"

#define CLOCK_MS_IN_TICKS(MS)               (APP_TIMER_TICKS(MS, APP_TIMER_PRESCALER))
#define CLOCK_S_IN_TICKS(S)                 (CLOCK_MS_IN_TICKS(S * 1000))
#define CLOCK_TICKS_IN_MS(TICKS)            ((uint32_t)ROUNDED_DIV((uint64_t)(TICKS) * ((APP_TIMER_PRESCALER) + 1) * 1000, APP_TIMER_CLOCK_FREQ))

/**
 * @brief   Function to initialize the clock module.
 */
void clock_init(void);

/**
 * @brief   Function to accomplish the clock module tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void clock_tasks(void);

/**
 * @brief   Get the current value of the ticks clock.
 *
 * @retval  The ticks value right now.
 */
uint32_t clock_get_ticks(void);

/**
 * @brief   Test to see if 'ticks_target' ticks have passed since the 'start'.
 *
 * @param[in]   start           The time (also in ticks) to start measuring from - obtained from "clock_get_ticks".
 * @param[in]   ticks           The number of ticks that we are waiting to have passed.
 *
 * @retval      True if "ticks" have passed since the start.
 */
bool clock_ticks_have_passed(uint32_t start, uint32_t ticks);

/**
 * @brief   Get the number of milliseconds that still remain until ticks have passed counting from start.
 *
 * @param[in]   start           The time to start measuring from - obtained from "clock_get_ticks".
 * @param[in]   ticks           The number of ticks that we want to pass from the starting point.
 *
 * @retval      The number of milliseconds we still have to wait until ticks have passed since the start
 *              or 0 if ticks have already passed since start.
 */
uint32_t clock_ms_until(uint32_t start, uint32_t ticks);

/**
 * @brief   Get the number of milliseconds that have passed since a starting point.
 *
 * @param[in]   start           The time (in ticks) to start measuring from - obtained from "clock_get_ticks".
 *
 * @retval      The number of milliseconds since the start.
 */
uint32_t clock_ms_since(uint32_t start);

/**
 * @brief   Get the number of click ticks that have passed since a starting point.
 *
 * @param[in]   start           The time (in ticks) to start measuring from - obtained from "clock_get_ticks".
 *
 * @retval      The number of ticks since the start.
 */
static inline uint32_t clock_ticks_since(uint32_t start);

#endif //CLOCK_H__

/** @} */
