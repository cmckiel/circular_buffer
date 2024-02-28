#ifndef _CIRCULAR_BUFFER_H
#define _CIRCULAR_BUFFER_H

#include <stdbool.h>

#define CIRCULAR_BUFFER_DATA_ITEM_SIZE 64

typedef struct {
    int data[CIRCULAR_BUFFER_DATA_ITEM_SIZE];
    int size;
} data_item_t;

bool circular_buffer_init(data_item_t *data);

#endif /* _CIRCULAR_BUFFER_H */
