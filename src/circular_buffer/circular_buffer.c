#include "circular_buffer.h"

bool circular_buffer_init(circular_buffer_ctx *ctx)
{
    bool res = false;

    if (ctx)
    {
        ctx->is_empty = true;
        ctx->head = 0;
        res = true;
    }

    return res;
}

bool circular_buffer_push(circular_buffer_ctx *ctx, uint8_t data)
{
    bool res = false;

    if (ctx)
    {
        ctx->is_empty = false;
        ctx->data[ctx->head] = data;
        ctx->head++;

        res = true;
    }

    return res;
}

bool circular_buffer_pop(circular_buffer_ctx *ctx, uint8_t *data)
{
    bool res = false;

    if (data && ctx && ctx->is_empty == false && ctx->head > 0)
    {
        ctx->head--;
        *data = ctx->data[ctx->head];
        res = true;
    }

    return res;
}
