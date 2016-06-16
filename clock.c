/**
 * @file
 * @defgroup Appaloosa clock.h
 * @{
 * @ingroup Appaloosa
 * @brief Appaloosa clock module.
 */

#include "ble_stack.h"
#include "clock.h"
#include "nrf_drv_clock.h"

APP_TIMER_DEF(m_loop_timer_id);

static void clock_handler(void * p_context)
{
}


void clock_init(void)
{
    // Initialize the low frequency clock.
    APP_ERROR_CHECK(nrf_drv_clock_init());
    nrf_drv_clock_lfclk_request(NULL);

    // Create timers.
    APP_ERROR_CHECK(app_timer_create(&m_loop_timer_id, APP_TIMER_MODE_REPEATED, clock_handler));
    APP_ERROR_CHECK(app_timer_start(m_loop_timer_id, CLOCK_S_IN_TICKS(2), NULL));
}


void clock_tasks(void)
{
}


uint32_t clock_get_ticks(void)
{
    uint32_t ticks;
    APP_ERROR_CHECK(app_timer_cnt_get(&ticks));
    return ticks;
}


bool clock_ticks_have_passed(uint32_t start, uint32_t ticks)
{
    return clock_ticks_since(start) >= ticks;
}


bool clock_ms_have_passed(uint32_t start, uint32_t ms)
{
    return clock_ms_since(start) >= ms;
}


uint32_t clock_ms_until(uint32_t start, uint32_t ticks)
{
    uint32_t passed = clock_ticks_since(start);
    return passed >= ticks ? 0 : CLOCK_TICKS_IN_MS(ticks - passed);
}


uint32_t clock_ms_since(uint32_t start)
{
    return CLOCK_TICKS_IN_MS(clock_ticks_since(start));
}


static inline uint32_t clock_ticks_since(uint32_t start)
{
    uint32_t stop;
    uint32_t ticks_diff;
    APP_ERROR_CHECK(app_timer_cnt_get(&stop));
    APP_ERROR_CHECK(app_timer_cnt_diff_compute(stop, start, &ticks_diff));
    return ticks_diff;
}

/** @} */
