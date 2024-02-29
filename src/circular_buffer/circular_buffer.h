#ifndef _CIRCULAR_BUFFER_H
#define _CIRCULAR_BUFFER_H

#include <stdbool.h>

typedef struct {
    bool is_empty;
} circular_buffer_ctx;

/**
 * @brief Initializes an instance of circular buffer.
 *
 * @param ctx A blank handle for the buffer.
 * @return true if success, false if init failure.
*/
bool circular_buffer_init(circular_buffer_ctx *ctx);

/**
 * @brief Adds an item to the circular buffer.
 *
 * @param ctx A handle for the buffer.
 * @param data A piece of data to push.
 *
 * @return true on success.
*/
bool circular_buffer_push(circular_buffer_ctx *ctx, int data);

/**
 * @brief Removes an item from the circular buffer.
 *
 * @param ctx A handle for the buffer.
 * @param data A pointer to return popped data.
 *
 * @return true on success.
*/
bool circular_buffer_pop(circular_buffer_ctx *ctx, int *data);

#endif /* _CIRCULAR_BUFFER_H */
