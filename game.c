/**
 * @file
 * @defgroup WaterBall game.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall game module.
 */

#include "app_button.h"
#include "app_error.h"
#include "game.h"
#include "service.h"
#include "service_client.h"
#include "service_server.h"

static game_state_t         m_game_state;
static uint32_t             m_my_score;
static uint32_t             m_their_score;


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
            m_game_state = GAME_STATE_READY;
            break;
        }
        case GAME_STATE_READY:
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

/** @} */
