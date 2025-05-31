#ifndef _CIRCULAR_BUFFER_H
#define _CIRCULAR_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef CIRCULAR_BUFFER_MAX_SIZE
#define CIRCULAR_BUFFER_MAX_SIZE 1024
#endif

typedef struct {
    size_t buff_size;          // User can set up to MAX_SIZE
    uint8_t buffer[CIRCULAR_BUFFER_MAX_SIZE];
    size_t head;               // Handles any reasonable buffer size
    size_t tail;
    size_t current_byte_count;
    uint32_t overflow_count;   // Accumulates over time
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

/**
 * @brief Use to check if there is anything in the buffer.
 *
 * @param ctx A handle for the buffer.
 *
 * @return true if the buffer is empty or if the ctx is NULL, false if there
 *          are items in the buffer.
 */
bool circular_buffer_is_empty(circular_buffer_ctx *ctx);

#endif /* _CIRCULAR_BUFFER_H */
