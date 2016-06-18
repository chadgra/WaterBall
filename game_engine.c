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
#include "game_engine.h"
#include "serial.h"

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
        case BUTTON_1:
            m_game_engine_state = GAME_ENGINE_STARTING;
            m_clock_start_ticks = clock_get_ticks();
            break;
        default:
            break;
    }
}


static void game_engine_print_score_board(uint32_t ms)
{
    static char buffer[BUFFER_LEN] = { 0 };
    uint32_t total_seconds = ms / 1000;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;

    if (60 > total_seconds)
    {
        minutes = seconds;
        seconds = (ms % 1000) / 10;
    }

    uint32_t size = snprintf(buffer, sizeof(buffer), "\r%02d:%02d", minutes, seconds);
    serial_write((uint8_t *)buffer, size);
}

/** @} */