#include "circular_buffer.h"

bool circular_buffer_init(circular_buffer_ctx *ctx, size_t buff_size)
{
    bool res = false;

    if (ctx && buff_size <= MAX_BUFFER_SIZE)
    {
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

    if (ctx && ctx->head < MAX_BUFFER_SIZE)
    {
        ctx->current_byte_count += 1;
        ctx->buffer[ctx->head] = data;
        ctx->head++;

        res = true;
    }

    return res;
}

bool circular_buffer_pop(circular_buffer_ctx *ctx, uint8_t *data)
{
    bool res = false;

    if (data && ctx && ctx->current_byte_count > 0 && ctx->tail < MAX_BUFFER_SIZE)
    {
        *data = ctx->buffer[ctx->tail];
        ctx->tail++;
        res = true;
    }

    return res;
}
