/**
 * @file
 * @defgroup WaterBall game.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall game module.
 */

#include <string.h>

#include "app_button.h"
#include "app_error.h"
#include "bsp.h"
#include "clock.h"
#include "game.h"
#include "serial.h"
#include "service.h"
#include "service_client.h"
#include "service_server.h"
#include "seven_segment.h"

static game_state_t         m_game_state;
static uint32_t             m_my_score;
static uint32_t             m_their_score;
static uint32_t             m_game_time;
static uint32_t             m_game_init_ticks;
static uint32_t             m_game_start_ticks;


void game_event_handler(uint8_t pin_number, uint8_t button_action)
{
    if (APP_BUTTON_RELEASE == button_action)
    {
        return;
    }

    switch (pin_number)
    {
        case BUTTON_2:
            if (GAME_STATE_WAITING == m_game_state)
            {
                // Start the game if we are waiting.
                m_game_state = GAME_STATE_INITIALIZING_GAME;
            }
            else
            {
                // Otherwise stop the game.
                m_game_state = GAME_STATE_INIT;
            }
            
            break;
        case BUTTON_3:
            game_increment_my_score(1);
            break;
        default:
            break;
    }
}


void game_init(void)
{
    m_game_state = GAME_STATE_INIT;
}


void game_tasks(void)
{
    switch (m_game_state)
    {
        case GAME_STATE_INIT:
        {
            seven_segment_blank_digits(TIME_ADDRESS);
            seven_segment_blank_digits(SCORE_ADDRESS);
            m_game_state = GAME_STATE_WAITING;
            break;
        }
        case GAME_STATE_WAITING:
        {
            break;
        }
        case GAME_STATE_INITIALIZING_GAME:
        {
            m_game_init_ticks = clock_get_ticks();
            m_game_time = game_get_game_time();
            m_game_state = GAME_STATE_COUNTING_DOWN;
            break;
        }
        case GAME_STATE_WAITING_TO_COUNT_DOWN:
        {
            break;
        }
        case GAME_STATE_COUNTING_DOWN:
        {
            uint32_t count_down_ticks = CLOCK_MS_IN_TICKS(5000);
            uint32_t ms = clock_ms_until(m_game_init_ticks, count_down_ticks);
            if (clock_ticks_have_passed(m_game_init_ticks, count_down_ticks))
            {
                m_game_state = GAME_STATE_START;
            }

            game_print_start(ms);
            break;
        }
        case GAME_STATE_START:
        {
            m_game_start_ticks = clock_get_ticks();
            m_game_state = GAME_STATE_PLAYING;
            break;
        }
        case GAME_STATE_PLAYING:
        {
            // Time
            uint32_t play_ticks = CLOCK_MS_IN_TICKS(m_game_time);
            uint32_t ms = clock_ms_until(m_game_start_ticks, play_ticks);
            if (clock_ticks_have_passed(m_game_start_ticks, play_ticks))
            {
                m_game_state = GAME_STATE_TIME_UP;
            }

            game_print_time(ms, TIME_SCALE_AUTO);

            // Score
            uint32_t my_score = game_get_my_score();
            uint32_t their_score = game_get_their_score();
            game_print_score(my_score, their_score);

            break;
        }
        case GAME_STATE_TIME_UP:
        {
            break;
        }
        case GAME_STATE_MAX_SCORE:
        {
            break;
        }
        case GAME_STATE_ERROR:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}


uint32_t game_get_game_time(void)
{
    uint32_t game_time = 0;

    if (IS_SERVICE_SERVER)
    {
        game_time = service_server_get_game_time();
    }
    else if (IS_SERVICE_CLIENT)
    {
        game_time = service_client_get_game_time();
    }

    return game_time;
}


uint32_t game_get_my_score(void)
{
    return m_my_score;
}


void game_set_my_score(uint32_t score)
{
    m_my_score = score;
    if (IS_SERVICE_SERVER)
    {
        service_server_indicate_server_score(m_my_score);
    }
    else if (IS_SERVICE_CLIENT)
    {
        service_client_write_client_score(m_my_score);
    }
}


uint32_t game_increment_my_score(uint32_t points)
{
    game_set_my_score(m_my_score + points);
    return m_my_score;
}


uint32_t game_get_their_score(void)
{
    if (IS_SERVICE_SERVER)
    {
        m_their_score = service_server_get_client_score();
    }
    else if (IS_SERVICE_CLIENT)
    {
        m_their_score = service_client_get_server_score();
    }

    return m_their_score;
}


static void game_print_start(uint32_t ms)
{
    char count_string[] = "4321";
    int i = 5 - (ms / 1000);
    count_string[MIN(i, strlen(count_string))] = '\0';
    seven_segment_set_char_digits(TIME_ADDRESS, 0, count_string, COLON_TYPE_NONE);
    if (5 <= i)
    {
        seven_segment_set_char_digits(SCORE_ADDRESS, 0, "PLAY", COLON_TYPE_NONE);
    }
}


static void game_print_score(uint32_t my_score, uint32_t their_score)
{
    static char buffer[BUFFER_LEN] = { 0 };

    uint32_t size = snprintf(buffer, sizeof(buffer), "\t%02d %02d", my_score, their_score);
    serial_write((uint8_t *)buffer, size);

    seven_segment_set_numbers(SCORE_ADDRESS, my_score, their_score, COLON_TYPE_NONE);
}


static void game_print_time(uint32_t ms, time_scale_t time_scale)
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

    switch (time_scale)
    {
        case TIME_SCALE_MILLISECOND:
            left_time = total_seconds % 60;
            right_time = (ms % 1000) / 10;
            break;
        case TIME_SCALE_SECOND:
            left_time = total_seconds / 60;
            right_time = total_seconds % 60;
            break;
        default:
            break;
    }

    uint32_t size = snprintf(buffer, sizeof(buffer), "\r\t%02d:%02d", left_time, right_time);
    serial_write((uint8_t *)buffer, size);

    seven_segment_set_numbers(TIME_ADDRESS, left_time, right_time, COLON_TYPE_COLON);
}


/** @} */
