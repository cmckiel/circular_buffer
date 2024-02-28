#include "circular_buffer.h"

bool circular_buffer_init(data_item_t *data)
{
    for (int i = 0; i < data->size; i++)
    {
        data->data[i] = 0;
    }

    return true;
}
