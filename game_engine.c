/**
 * @file
 * @defgroup WaterBall game_engine.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall game engine.
 *
 * This file contains the source code for the game engine.
 */

#include <stdio.h>

#include "app_button.h"
#include "bsp.h"
#include "clock.h"
#include "game.h"
#include "game_engine.h"
#include "serial.h"
#include "seven_segment.h"

#define START_TIME              (CLOCK_MS_IN_TICKS(5000))
#define PLAY_TIME               (CLOCK_MS_IN_TICKS(80000))
#define BUFFER_LEN              (128)

static game_engine_state_t      m_game_engine_state;
static uint32_t                 m_clock_start_ticks;


void game_engine_init(void)
{
    m_game_engine_state = GAME_ENGINE_STATE_INIT;
}


void game_engine_tasks(void)
{
    uint32_t ms = 0;
    switch (m_game_engine_state)
    {
        case GAME_ENGINE_STATE_INIT:
        {
            m_game_engine_state = GAME_ENGINE_STATE_READY;
            break;
        }
        case GAME_ENGINE_STATE_READY:
        {
            break;
        }
        case GAME_ENGINE_STARTING:
        {
            ms = clock_ms_until(m_clock_start_ticks, START_TIME);
            if (clock_ticks_have_passed(m_clock_start_ticks, START_TIME))
            {
                m_game_engine_state = GAME_ENGINE_PLAYING;
                m_clock_start_ticks = clock_get_ticks();
            }

            break;
        }
        case GAME_ENGINE_PLAYING:
        {
            ms = clock_ms_until(m_clock_start_ticks, PLAY_TIME);
            if (clock_ticks_have_passed(m_clock_start_ticks, PLAY_TIME))
            {
                m_game_engine_state = GAME_ENGINE_ENDING;
            }

            break;
        }
        case GAME_ENGINE_ENDING:
        {
            break;
        }
        case GAME_ENGINE_STATE_ERROR:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    game_engine_print_score_board(ms);
}


void game_engine_event_handler(uint8_t pin_number, uint8_t button_action)
{
    if (APP_BUTTON_RELEASE == button_action)
    {
        return;
    }

    switch (pin_number)
    {
        case BUTTON_2:
            m_game_engine_state = GAME_ENGINE_STARTING;
            m_clock_start_ticks = clock_get_ticks();
            break;
        case BUTTON_3:
            game_increment_my_score(1);
            break;
        default:
            break;
    }
}


static void game_engine_print_score_board(uint32_t ms)
{
    static char buffer[BUFFER_LEN] = { 0 };
    uint32_t total_seconds = ms / 1000;
    uint32_t left_time = total_seconds / 60;
    uint32_t right_time = total_seconds % 60;

    if (60 > total_seconds)
    {
        left_time = right_time;
        right_time = (ms % 1000) / 10;
    }

    uint32_t my_score = game_get_my_score();
    uint32_t their_score = game_get_their_score();

    uint32_t size = snprintf(buffer, sizeof(buffer), "\r%02d:%02d\t%02d %02d", left_time, right_time, my_score, their_score);
    serial_write((uint8_t *)buffer, size);

    seven_segment_set_numbers(TIME_ADDRESS, left_time, right_time, COLON_TYPE_COLON);
    seven_segment_set_numbers(SCORE_ADDRESS, my_score, their_score, COLON_TYPE_NONE);
}

/** @} */
