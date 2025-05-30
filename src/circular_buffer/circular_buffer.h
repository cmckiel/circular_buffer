#ifndef _CIRCULAR_BUFFER_H
#define _CIRCULAR_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

typedef struct {
    size_t buff_size;
    uint8_t buffer[MAX_BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
    uint32_t current_byte_count;
} circular_buffer_ctx;

/**
 * @brief Initializes an instance of circular buffer.
 *
 * @param ctx A blank handle for the buffer.
 * @param buff_size The size of the buffer to instantiate. Must be less than or equal to MAX_BUFFER_SIZE.
 * @return true if success, false if init failure.
*/
bool circular_buffer_init(circular_buffer_ctx *ctx, size_t buff_size);

/**
 * @brief Adds an item to the circular buffer.
 *
 * @param ctx A handle for the buffer.
 * @param data A piece of data to push.
 *
 * @return true on success.
*/
bool circular_buffer_push(circular_buffer_ctx *ctx, uint8_t data);

/**
 * @brief Removes an item from the circular buffer.
 *
 * @param ctx A handle for the buffer.
 * @param data A pointer to return popped data.
 *
 * @return true on success.
*/
bool circular_buffer_pop(circular_buffer_ctx *ctx, uint8_t *data);

/**
 * @brief Allows peeking at the next item without popping it.
 *
 * @param ctx A handle for the buffer.
 * @param data A pointer to return peeked data.
 *
 * @return true on success.
 */
bool circular_buffer_peek(circular_buffer_ctx *ctx, uint8_t *data);

#endif /* _CIRCULAR_BUFFER_H */
