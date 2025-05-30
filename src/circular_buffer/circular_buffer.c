#include "circular_buffer.h"

// Defensive check: head should always stay within buffer bounds,
// but we verify to guard against potential corruption/misuse.
static bool ctx_is_valid(const circular_buffer_ctx *ctx) {
    return ctx &&
           ctx->buff_size <= MAX_BUFFER_SIZE &&
           ctx->head < ctx->buff_size &&
           ctx->tail < ctx->buff_size &&
           ctx->current_byte_count <= ctx->buff_size;
}

bool circular_buffer_init(circular_buffer_ctx *ctx, size_t buff_size)
{
    bool res = false;

    if (ctx && buff_size <= MAX_BUFFER_SIZE)
    {
        ctx->buff_size = buff_size;
        ctx->current_byte_count = 0;
        ctx->head = 0;
        ctx->tail = 0;
        res = true;
    }

    return res;
}

bool circular_buffer_push(circular_buffer_ctx *ctx, uint8_t data)
{
    bool res = false;

    if (ctx_is_valid(ctx))
    {
        // Buffer is full if true, overwrite mode.
        if (ctx->current_byte_count == ctx->buff_size)
        {
            ctx->tail = (ctx->tail + 1) % ctx->buff_size;
            ctx->current_byte_count--;
        }

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
