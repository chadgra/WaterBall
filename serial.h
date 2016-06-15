/**
 * @file
 * @defgroup WaterBall serial.h
 * @{
 * @ingroup WaterBall
 * @brief WaterBall serial communication module.
 *
 * The basic philosophy is that the serial_tasks will be called from the main loop
 * and see if there are any bytes available from the SDKs uart module. If there are
 * read them and put them into a receive buffer.
 *
 * The receive buffer is made available to be read from. When it is successfully read
 * from any reamining bytes are moved to the head of the buffer.
 */

#ifndef SERIAL_H__
#define SERIAL_H__

#include <stdbool.h>
#include <stdint.h>

#define UART_TX_BUF_SIZE         256                  /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE         256                  /**< UART RX buffer size. */
#define SERIAL_RX_BUF_SIZE       256
#define SERIAL_FLOW_CONTROL_BUF  (SERIAL_RX_BUF_SIZE - sizeof("\r"))

#define SERIAL_TX_EMPTY_TIMEOUT  (CLOCK_MS_IN_TICKS(2))

/**
 * @brief Increment the index into the buffer by the given count, rollover when we reach the max size.
 */
#define INCREMENT_BUFFER_INDEX(INDEX, COUNT, BUFFER)                    \
        (INDEX = (INDEX + COUNT) % sizeof(BUFFER))

/**
 * @brief Determine the number of bytes that contain data in a buffer.
 */
#define BUFFER_FILLED_COUNT(READ_I, WRITE_I, BUFFER)                    \
        ((WRITE_I + sizeof(BUFFER) - READ_I) % sizeof(BUFFER))

/**
 * @brief True if the buffer is full.
 */
#define IS_BUFFER_FULL(READ_I, WRITE_I, BUFFER)                         \
        (BUFFER_FILLED_COUNT(READ_I, WRITE_I, BUFFER) == (sizeof(BUFFER) - 1))

/**
 * @brief True if the buffer is empty.
 */
#define IS_BUFFER_EMPTY(READ_I, WRITE_I, BUFFER)                        \
        (READ_I == WRITE_I)

/**
 * @brief serial module states.
 */
typedef enum
{
    SERIAL_STATE_INIT,              /**< Initialize the serial module. */
    SERIAL_STATE_CLOSED,            /**< The serial module is closed. */
    SERIAL_STATE_OPENED,            /**< The serial module is opened and running normally. */
    SERIAL_STATE_ERROR              /**< Throw an error message if it occurred in the interrupt handler. */
} serial_state_t;

/**
 * @brief Function to initialize the serial port.
 */
void serial_init(void);

/**
 * @brief Function to accomplish serial port tasks.
 *
 * @details This should be called repeatedly from the main loop.
 */
void serial_tasks(void);

/**
 * @brief Try and open the serial port, if it is not already open.
 *
 * @retval  True if the serial port was opened (always return true).
 */
bool serial_try_open(void);

/**
 * @brief Try and close the serial port, if it is not already closed.
 *
 * @retval  True if the serial port was able to close.
 */
bool serial_try_close(void);

/**
 * @brief Function to try and return a line received over the serial port.
 *
 * @param[out]  p_buffer        A pointer to the buffer where data will be copied.
 * @param[in]   size            The maximum size of the supplied buffer.
 *
 * @retval      The number of bytes copied into the buffer.
 */
uint32_t serial_try_read_line(uint8_t * p_buffer, uint32_t size);

/**
 * @brief Function to return all of the data received over the serial port.
 *
 * @param[out]  p_buffer        A pointer to the buffer where data will be copied.
 * @param[in]   size            The maximum size of the supplied buffer.
 *
 * @retval      The number of bytes copied into the buffer.
 */
uint32_t serial_read_existing(uint8_t * p_buffer, uint32_t size);

/**
 * @brief Function to try and write a given number of bytes over the serial port.
 *
 * @param[in]   p_buffer        A pointer to the buffer that contains the data to transmit.
 * @param[in]   size            The number of bytes to send in the buffer.
 */
void serial_write(uint8_t * p_buffer, uint32_t length);

/**
 * @brief   Function to read bytes from the uart fifo and put them into our receive buffer.
 *
 * @retval      The number of bytes moved from the fifo to the receive buffer.
 */
static uint32_t serial_fifo_to_rx_buffer(void);

#endif //SERIAL_H__

/** @} */
