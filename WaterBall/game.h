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

#define BUFFER_LEN              (128)

/**
 * @brief   service server module states.
 */
typedef enum
{
    GAME_STATE_INIT = 0,                /**< Initialize the service server module. */
    GAME_STATE_WAITING,                 /**< The service server module is just waiting for a game to start. */
    GAME_STATE_INITIALIZING_GAME,       /**< A game has been requested to start to make sure we have all the correct value. */
    GAME_STATE_WAITING_TO_COUNT_DOWN,   /**< We may have to wait before we can count down to the start. */
    GAME_STATE_COUNTING_DOWN,           /**< Count down until the start of the game. */
    GAME_STATE_START,                   /**< Display that we can now start. */
    GAME_STATE_PLAYING,                 /**< We are now playing the game. */
    GAME_STATE_TIME_UP,                 /**< Time is up, decide who gets wet. */
    GAME_STATE_MAX_SCORE,               /**< Some one reached the max score, so figure out who gets wet. */
    GAME_STATE_ERROR                    /**< Throw an error message if it occurred in the interrupt handler. */
} game_state_t;

typedef enum
{
    TIME_SCALE_AUTO,                    /**< Use whatever time scale makes sense. */
    TIME_SCALE_MILLISECOND,             /**< Milliseconds will be in the lowest order place. */
    TIME_SCALE_SECOND                   /**< Seconds will be in the lowest order place. */
} time_scale_t;

/**
 * @brief   Event handler that runs when a button is pressed.
 *
 * @param[in]   pin_number      The number of the pin that was pressed.
 * @param[in]   button_action   The action (press or release) of the button.
 */
void game_event_handler(uint8_t pin_number, uint8_t button_action);

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
 * @brief   Function to get the game time.
 *
 * @retval      The game time.
 */
uint32_t game_get_game_time(void);

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

static void game_print_start(uint32_t ms);

static void game_print_score(uint32_t my_score, uint32_t their_score);

/**
 * @brief   Function to print the time on the seven segment display.
 *
 * @param[in]   ms          The number of milliseconds into the game.
 * @param[in]   time_scale  The 
 */
static void game_print_time(uint32_t ms, time_scale_t time_scale);

#endif //GAME_H__

/** @} */
