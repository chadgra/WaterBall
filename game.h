/**
 * @file
 * @defgroup WaterBall game.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall game module.
 *
 * Main interface for the game.
 */

#ifndef GAME_H__
#define GAME_H__

#include <stdbool.h>
#include <stdint.h>
#include "ble.h"
#include "ble_types.h"

/**
 * @brief   service server module states.
 */
typedef enum
{
    GAME_STATE_INIT = 0,            /**< Initialize the service server module. */
    GAME_STATE_READY,               /**< The service server module is running normally. */
    GAME_STATE_ERROR                /**< Throw an error message if it occurred in the interrupt handler. */
} game_state_t;

/**
 * @brief   Function to initialize the game module.
 */
void game_init(void);

/**
 * @brief   Function to accomplish game module tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void game_tasks(void);

/**
 * @brief   Function to get my score.
 *
 * @retval      My current score.
 */
uint32_t game_get_my_score(void);

/**
 * @brief   Function to set my score.
 *
 * @param[in]   score       My new score.
 */
void game_set_my_score(uint32_t score);

/**
 * @brief   Function to set increment score.
 *
 * @param[in]   points      How many points to increment my score by.
 *
 * @retval      My new current score.
 */
uint32_t game_increment_my_score(uint32_t points);

/**
 * @brief   Function to get their score.
 *
 * @retval      Their current score.
 */
uint32_t game_get_their_score(void);

#endif //GAME_H__

/** @} */
