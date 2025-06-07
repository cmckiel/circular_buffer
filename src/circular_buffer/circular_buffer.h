/**
 * @file circular_buffer.h
 * @brief A circular byte buffer for embedded hardware drivers in constrained environments
 * where correctness and safety are greater concerns than flexibility.
 *
 * @note Not thread-safe. This is intentional to allow the user to decide how best to implement
 * thread-safety, whether that is using the standard library, an RTOS, or interrupt disabling.
 * Make sure to wrap the calls with the proper protections.
 */
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
 * Will overwrite the oldest data in buffer if full on push.
 *
 * @param ctx A handle for the buffer.
 * @param data A piece of data to push.
 *
 * @return true on success.
*/
bool circular_buffer_push_with_overwrite(circular_buffer_ctx *ctx, uint8_t data);

/**
 * @brief Adds an item to the circular buffer.
 * Never overwrites data in buffer. Fails if buffer is full.
 *
 * @param ctx A handle for the buffer.
 * @param data A piece of data to push.
 *
 * @return true on success.
*/
bool circular_buffer_push_no_overwrite(circular_buffer_ctx *ctx, uint8_t data);

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
bool circular_buffer_peek(const circular_buffer_ctx *ctx, uint8_t *data);

/**
 * @brief Use to check if there is anything in the buffer.
 *
 * @param ctx A handle for the buffer.
 *
 * @return true if the buffer is empty or if the ctx is NULL, false if there
 *          are items in the buffer.
 */
bool circular_buffer_is_empty(const circular_buffer_ctx *ctx);

/**
 * @brief Retrieve the number of bytes that have been overwritten due to overflow.
 *
 * @param ctx A handle for the buffer.
 * @param overflow_count A pointer to a place where the retrieved overflow count should be stored.
 *
 * @return true if the data was successfully retrieved. false otherwise.
 */
bool circular_buffer_get_overflow_count(const circular_buffer_ctx *ctx, uint32_t *overflow_count);

/**
 * @brief Reset the overflow count for this buffer.
 *
 * @param ctx A handle for the buffer.
 *
 * @return true if the reset was successful. false otherwise.
 */
bool circular_buffer_clear_overflow_count(circular_buffer_ctx *ctx);

#endif /* _CIRCULAR_BUFFER_H */
