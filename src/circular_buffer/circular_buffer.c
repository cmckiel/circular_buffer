#include "circular_buffer.h"

// Defensive check: head should always stay within buffer bounds,
// but we verify to guard against potential corruption/misuse.
static bool ctx_is_valid(const circular_buffer_ctx *ctx) {
    return ctx &&
           ctx->buff_size <= CIRCULAR_BUFFER_MAX_SIZE &&
           ctx->buff_size > 0 &&
           ctx->head < ctx->buff_size &&
           ctx->tail < ctx->buff_size &&
           ctx->current_byte_count <= ctx->buff_size;
}

bool circular_buffer_init(circular_buffer_ctx *ctx, size_t buff_size)
{
    bool res = false;

    if (ctx && 0 < buff_size && buff_size <= CIRCULAR_BUFFER_MAX_SIZE)
    {
        ctx->buff_size = buff_size;
        ctx->head = 0;
        ctx->tail = 0;
        ctx->current_byte_count = 0;
        ctx->overflow_count = 0;
        res = true;
    }

    return res;
}

bool circular_buffer_push_with_overwrite(circular_buffer_ctx *ctx, uint8_t data)
{
    bool res = false;

    if (ctx_is_valid(ctx))
    {
        // Buffer is full if true, overwrite mode.
        if (ctx->current_byte_count == ctx->buff_size)
        {
            ctx->tail = (ctx->tail + 1) % ctx->buff_size;
            ctx->current_byte_count--;
            ctx->overflow_count++;
        }

        ctx->buffer[ctx->head] = data;
        ctx->head = (ctx->head + 1) % ctx->buff_size;
        ctx->current_byte_count++;

        res = true;
    }

    return res;
}

bool circular_buffer_push_no_overwrite(circular_buffer_ctx *ctx, uint8_t data)
{
    bool res = false;

    if (ctx_is_valid(ctx) && ctx->current_byte_count < ctx->buff_size)
    {
        ctx->buffer[ctx->head] = data;
        ctx->head = (ctx->head + 1) % ctx->buff_size;
        ctx->current_byte_count++;

        res = true;
    }

    return res;
}

bool circular_buffer_pop(circular_buffer_ctx *ctx, uint8_t *data)
{
    bool res = false;

    if (data && ctx_is_valid(ctx) && ctx->current_byte_count > 0)
    {
        *data = ctx->buffer[ctx->tail];
        ctx->tail = (ctx->tail + 1) % ctx->buff_size;
        ctx->current_byte_count -= 1;
        res = true;
    }

    return res;
}

bool circular_buffer_peek(const circular_buffer_ctx *ctx, uint8_t *data)
{
    bool res = false;

    if (data && ctx_is_valid(ctx) && ctx->current_byte_count > 0)
    {
        *data = ctx->buffer[ctx->tail];
        res = true;
    }

    return res;
}

bool circular_buffer_is_empty(const circular_buffer_ctx *ctx)
{
    bool res = true; // Consider a NULL ctx to be an empty buffer.

    if (ctx_is_valid(ctx) && ctx->current_byte_count > 0)
    {
        res = false;
    }

    return res;
}

bool circular_buffer_get_overflow_count(const circular_buffer_ctx *ctx, uint32_t *overflow_count)
{
    bool res = false;

    if (overflow_count && ctx_is_valid(ctx))
    {
        *overflow_count = ctx->overflow_count;
        res = true;
    }

    return res;
}

bool circular_buffer_clear_overflow_count(circular_buffer_ctx *ctx)
{
    bool res = false;

    if (ctx_is_valid(ctx))
    {
        ctx->overflow_count = 0;
        res = true;
    }

    return res;
}
