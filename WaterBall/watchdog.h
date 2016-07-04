/**
 * @file
 * @defgroup WaterBall watchdog.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall watchdog module.
 */

#ifndef WATCHDOG_H__
#define WATCHDOG_H__

/**
 * @brief watchdog module states.
 */
typedef enum
{
    WATCHDOG_STATE_INIT,    /**< Initialize the watchdog module. */
    WATCHDOG_STATE_RUN,     /**< The watchdog module is running normally. */
    WATCHDOG_STATE_ERROR    /**< Throw an error message if it occurred in the interrupt handler. */
} watchdog_state_t;

/**
 * @brief The watchdog event handler - throw an application error.
 */
static void watchdog_event_handler(void);

/**
 * @brief Function to initialize the watchdog.
 */
void watchdog_init(void);

/**
 * @brief Function to accomplish watchdog tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void watchdog_tasks(void);

#endif //WATCHDOG_H__

/** @} */
