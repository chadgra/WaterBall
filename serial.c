/**
 * @file
 * @defgroup WaterBall serial.c
 * @{
 * @ingroup WaterBall
 * @brief WaterBall serial communication module.
 */

#include <string.h>
#include "app_error.h"
#include "app_uart.h"
#include "bsp.h"
#include "serial.h"

static serial_state_t           m_serial_state;
static uint8_t                  m_serial_rx_buffer[SERIAL_RX_BUF_SIZE];
static uint32_t                 m_serial_rx_buffer_write_i;
static uint32_t                 m_serial_rx_buffer_read_i;
static uint32_t                 m_err_code;

/**
 * @brief Function for handling events raised by the uart driver.
 *
 * @param[in]  p_event      A pointer to the event occured.
 */
static void serial_evt_handler(app_uart_evt_t * p_event)
{
    switch (p_event->evt_type)
    {
        case APP_UART_TX_EMPTY:
        {
            break;
        }
        case APP_UART_COMMUNICATION_ERROR:
        {
            m_err_code = p_event->data.error_communication;
            break;
        }
        case APP_UART_FIFO_ERROR:
        {
            m_err_code = p_event->data.error_code;
            if (NRF_ERROR_NO_MEM != m_err_code)
            {
                m_serial_state = SERIAL_STATE_ERROR;
            }

            break;
        }
        default:
        {
            break;
        }
    }
}


void serial_init(void)
{
    m_serial_state = SERIAL_STATE_INIT;
    m_serial_rx_buffer_write_i = 0;
    m_serial_rx_buffer_read_i = 0;
}


void serial_tasks()
{
    switch (m_serial_state)
    {
        case SERIAL_STATE_INIT:
        {
            m_serial_state = SERIAL_STATE_CLOSED;
            break;
        }
        case SERIAL_STATE_CLOSED:
        {
            break;
        }
        case SERIAL_STATE_OPENED:
        {
            // Read from the uart in the main loop, so that we will continue to try even after the buffer is full.
            // If this is put in the interrupt handler and the buffer fills we won't be able to receive any more
            // uart data because RTS will be high preventing the peer from sending any data so we won't get any
            // more uart interrupts.
            serial_fifo_to_rx_buffer();
            break;
        }
        case SERIAL_STATE_ERROR:
        {
            APP_ERROR_HANDLER(m_err_code);
            break;
        }
        default:
        {
            break;
        }
    }
}


bool serial_try_open(void)
{
    if (SERIAL_STATE_OPENED == m_serial_state)
    {
        // We are already open so just make sure RTS is correct.
        m_serial_state = SERIAL_STATE_OPENED;
        return true;
    }

    static uint8_t     rx_buf[UART_RX_BUF_SIZE];
    static uint8_t     tx_buf[UART_TX_BUF_SIZE];
    app_uart_buffers_t buffers;
    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_ENABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200
    };

    buffers.rx_buf      = rx_buf;
    buffers.rx_buf_size = sizeof (rx_buf);
    buffers.tx_buf      = tx_buf;
    buffers.tx_buf_size = sizeof (tx_buf);
    APP_ERROR_CHECK(app_uart_init(&comm_params,
                                  &buffers,
                                  serial_evt_handler,
                                  APP_IRQ_PRIORITY_LOW));


    // We aren't using CTS so make sure it has a pulldown.
    nrf_gpio_cfg_sense_input(CTS_PIN_NUMBER, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    m_serial_state = SERIAL_STATE_OPENED;
    return true;
}


bool serial_try_close(void)
{
    if (SERIAL_STATE_CLOSED != m_serial_state)
    {
        m_serial_state = SERIAL_STATE_CLOSED;
    }

    return true;
}


static uint32_t serial_copy_rx_buffer(uint8_t * p_buffer, uint32_t size)
{
    uint32_t serial_rx_buffer_write_i = m_serial_rx_buffer_write_i;
    uint32_t serial_rx_buffer_read_i = m_serial_rx_buffer_read_i;

    if (IS_BUFFER_EMPTY(serial_rx_buffer_read_i, serial_rx_buffer_write_i, m_serial_rx_buffer) ||
        (0 == size))
    {
        return 0;
    }

    uint32_t return_size = BUFFER_FILLED_COUNT(serial_rx_buffer_read_i, serial_rx_buffer_write_i, m_serial_rx_buffer);

    return_size = MIN(return_size, size);

    // Copy first section of buffer - before rollover.
    uint32_t read_size = MIN(return_size, sizeof(m_serial_rx_buffer) - serial_rx_buffer_read_i);
    memcpy(p_buffer, &m_serial_rx_buffer[m_serial_rx_buffer_read_i], read_size);

    // Copy second section of buffer - after rollover, if any.
    p_buffer += read_size;
    read_size = return_size - read_size;
    memcpy(p_buffer, m_serial_rx_buffer, read_size);

    return return_size;
}


uint32_t serial_try_read_line(uint8_t * p_buffer, uint32_t size)
{
    uint32_t return_size = 0;
    uint32_t new_bytes;
    char * carriage_return = NULL;

    do
    {
        new_bytes = 0;
        return_size = serial_copy_rx_buffer(p_buffer, size);

        if (0 == return_size)
        {
            // There weren't any bytes at all, so just return.
            return 0;
        }

        // There are some bytes to check for a newline.
        return_size = 0;
        carriage_return = strpbrk((char *)p_buffer, "\r\n");

        if ((NULL == carriage_return) &&
            IS_BUFFER_FULL(m_serial_rx_buffer_read_i, m_serial_rx_buffer_write_i, m_serial_rx_buffer))
        {
            // No carriage return or new line, but the buffer was full, so remove the oldest byte and scan again.
            INCREMENT_BUFFER_INDEX(m_serial_rx_buffer_read_i, 1, m_serial_rx_buffer);
            new_bytes = serial_fifo_to_rx_buffer();
        }
    } while (0 < new_bytes);

    if (NULL != carriage_return)
    {
        // Add one to the size to make sure we return the newline;
        return_size = (carriage_return - (char *)&p_buffer[0]) + 1;
    }

    memset(&p_buffer[return_size], 0, sizeof(m_serial_rx_buffer) - return_size);
    if (0 < return_size)
    {
        INCREMENT_BUFFER_INDEX(m_serial_rx_buffer_read_i, return_size, m_serial_rx_buffer);
    }

    return return_size;
}


uint32_t serial_read_existing(uint8_t * p_buffer, uint32_t size)
{
    uint32_t return_size = serial_copy_rx_buffer(p_buffer, size);

    if (0 < return_size)
    {
        INCREMENT_BUFFER_INDEX(m_serial_rx_buffer_read_i, return_size, m_serial_rx_buffer);
    }

    return return_size;
}


void serial_write(uint8_t * p_buffer, uint32_t size)
{
    if (0 == size)
    {
        return;
    }

    serial_try_open();

    for (int i = 0; i < size; i++)
    {
        while(app_uart_put(p_buffer[i]) != NRF_SUCCESS);
    }
}


static uint32_t serial_fifo_to_rx_buffer(void)
{
    uint32_t err_code;
    uint32_t new_bytes = 0;

    do
    {
        if (IS_BUFFER_FULL(m_serial_rx_buffer_read_i, m_serial_rx_buffer_write_i, m_serial_rx_buffer))
        {
            // Don't read any bytes if the buffer is full. This is the most effective way to keep RTS high.
            break;
        }

        err_code = app_uart_get(&m_serial_rx_buffer[m_serial_rx_buffer_write_i]);
        if (NRF_SUCCESS == err_code)
        {
            new_bytes++;
            INCREMENT_BUFFER_INDEX(m_serial_rx_buffer_write_i, 1, m_serial_rx_buffer);
        }
    } while(NRF_SUCCESS == err_code);

    return new_bytes;
}

/** @} */
