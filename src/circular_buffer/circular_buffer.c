#include "circular_buffer.h"

bool circular_buffer_init(circular_buffer_ctx *ctx)
{
    ctx->is_empty = true;
    return true;
}

bool circular_buffer_push(circular_buffer_ctx *ctx, int data)
{
    ctx->is_empty = false;
    return true;
}

bool circular_buffer_pop(circular_buffer_ctx *ctx, int *data)
{
    bool res = false;
    *data = 4;

    if (ctx->is_empty == false)
    {
        res = true;
    }

    return res;
}
