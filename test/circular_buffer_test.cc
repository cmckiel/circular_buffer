#include <gtest/gtest.h>
#include <stdbool.h>

extern "C" {
#include "circular_buffer.h"
}

TEST(CircularBufferTest, DoesInitializeToZero)
{
    data_item_t data = { .data = {0}, .size = CIRCULAR_BUFFER_DATA_ITEM_SIZE };
    for (int i = 0; i < data.size; i++)
    {
        data.data[i] = 1;
    }

    // returns true
    EXPECT_EQ(true, circular_buffer_init(&data));

    // data initialized to zero
    for (int i = 0; i < data.size; i++)
    {
        EXPECT_EQ(0, data.data[i]);
    }
}
