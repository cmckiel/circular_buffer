#include <gtest/gtest.h>
#include <stdbool.h>

extern "C" {
#include "circular_buffer.h"
}

TEST(CircularBufferTest, DoesInitialize)
{
    circular_buffer_ctx ctx;
    EXPECT_EQ(true, circular_buffer_init(&ctx));
}

TEST(CircularBufferTest, DoesPushData)
{
    circular_buffer_ctx ctx;
    circular_buffer_init(&ctx);

    EXPECT_EQ(true, circular_buffer_push(&ctx, 4));
}

TEST(CircularBufferTest, DoesPopData)
{
    int data = 0;
    circular_buffer_ctx ctx;
    circular_buffer_init(&ctx);

    // If buffer is empty (just initialized) pop should return false.
    EXPECT_EQ(false, circular_buffer_pop(&ctx, &data));

    // Test that pop() can retrieve data.
    data = 0;
    circular_buffer_push(&ctx, 4);
    EXPECT_EQ(true, circular_buffer_pop(&ctx, &data));
    EXPECT_EQ(4, data);
}
