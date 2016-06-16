/**
 * @file
 * @defgroup WaterBall game_engine.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall game engine.
 */

#ifndef GAME_ENGINE_H__
#define GAME_ENGINE_H__

#include "stdint.h"

/**
 * @brief game engine module states.
 */
typedef enum
{
    GAME_ENGINE_STATE_INIT,             /**< Initialize the game engine module. */
    GAME_ENGINE_STATE_READY,            /**< The game engine module is ready to start. */
    GAME_ENGINE_STARTING,
    GAME_ENGINE_PLAYING,
    GAME_ENGINE_ENDING,
    GAME_ENGINE_STATE_ERROR             /**< Throw an error message if it occurred in the interrupt handler. */
} game_engine_state_t;

/**
 * @brief Function to initialize the game engine.
 */
void game_engine_init(void);

/**
 * @brief Function to accomplish game engine tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void game_engine_tasks(void);

/**
 * @brief   Event handler that runs when a button is pressed.
 *
 * @param[in]   pin_number      The number of the pin that was pressed.
 * @param[in]   button_action   The action (press or release) of the button.
 */
void game_engine_event_handler(uint8_t pin_number, uint8_t button_action);

/**
 * @brief   Print the game information on the score board.
 *
 * @param[in]   ms              The number of milliseconds left in the game to print.
 */
static void game_engine_print_score_board(uint32_t ms);

#endif //GAME_ENGINE_H__

/** @} */
