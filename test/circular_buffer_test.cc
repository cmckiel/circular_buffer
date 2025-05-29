#include <gtest/gtest.h>
#include <stdbool.h>

extern "C" {
#include "circular_buffer.h"
}

TEST(CircularBufferTest, DoesInitialize)
{
    size_t buff_size = 256;
    circular_buffer_ctx ctx;
    EXPECT_EQ(true, circular_buffer_init(&ctx, buff_size));
}

TEST(CircularBufferTest, PopHandlesNULLCtx)
{
    uint8_t data = 0;
    ASSERT_EQ(false, circular_buffer_pop(NULL, &data));
}

TEST(CircularBufferTest, PopHandlesNULLData)
{
    circular_buffer_ctx ctx;
    ASSERT_EQ(false, circular_buffer_pop(&ctx, NULL));
}

TEST(CircularBufferTest, PushHandlesNULLCtx)
{
    uint8_t data = 0;
    ASSERT_EQ(false, circular_buffer_push(NULL, data));
}

TEST(CircularBufferTest, InitHandlesNULLCtx)
{
    size_t buff_size = 256;
    ASSERT_EQ(false, circular_buffer_init(NULL, buff_size));
}

TEST(CircularBufferTest, DoesNotAllowBuffSizeInitGreaterThanMax)
{
    size_t buff_size = MAX_BUFFER_SIZE + 1; // size greater than the max
    circular_buffer_ctx ctx;

    ASSERT_EQ(false, circular_buffer_init(&ctx, buff_size));
}

TEST(CircularBufferTest, DoesAllowBuffSizeInitLessThanMax)
{
    size_t buff_size = MAX_BUFFER_SIZE - 1; // size less than the max
    circular_buffer_ctx ctx;

    ASSERT_EQ(true, circular_buffer_init(&ctx, buff_size));
}

TEST(CircularBufferTest, DoesAllowBuffSizeInitEqualToMax)
{
    size_t buff_size = MAX_BUFFER_SIZE; // size equal to the max
    circular_buffer_ctx ctx;

    ASSERT_EQ(true, circular_buffer_init(&ctx, buff_size));
}

TEST(CircularBufferTest, DoesPushData)
{
    size_t buff_size = 256;
    circular_buffer_ctx ctx;
    circular_buffer_init(&ctx, buff_size);

    EXPECT_EQ(true, circular_buffer_push(&ctx, 4));
}

TEST(CircularBufferTest, DoesPopData)
{
    uint8_t data = 0;
    size_t buff_size = 256;
    circular_buffer_ctx ctx;
    circular_buffer_init(&ctx, buff_size);

    // If buffer is empty (just initialized) pop should return false.
    EXPECT_EQ(false, circular_buffer_pop(&ctx, &data));

    // Test that pop() can retrieve data.
    data = 0;
    circular_buffer_push(&ctx, 4);
    EXPECT_EQ(true, circular_buffer_pop(&ctx, &data));
    EXPECT_EQ(4, data);
}

TEST(CircularBufferTest, DoesPopWhatItPushes)
{
    uint8_t data_in = 36;
    uint8_t data_out = 0;
    size_t buff_size = 256;
    circular_buffer_ctx ctx;
    circular_buffer_init(&ctx, buff_size);

    circular_buffer_push(&ctx, data_in);

    circular_buffer_pop(&ctx, &data_out);

    ASSERT_EQ(data_in, data_out);
}

// @todo
// subsequent pops don't yield same element

TEST(CirularBufferTest, DoesPopWhatItPushesNTimes)
{
    circular_buffer_ctx ctx;
    size_t buff_size = MAX_BUFFER_SIZE;
    uint8_t data[MAX_BUFFER_SIZE] = { 0 };

    circular_buffer_init(&ctx, buff_size);

    for (int i = 0; i < MAX_BUFFER_SIZE; i++)
    {
        uint8_t data_in = i % 256;
        circular_buffer_push(&ctx, data_in);
        data[i] = data_in; // remember what we pushed.
    }

    // FIFO
    for (int i = 0; i < MAX_BUFFER_SIZE; i++)
    {
        uint8_t data_out = 0;
        circular_buffer_pop(&ctx, &data_out);
        ASSERT_EQ(data[i], data_out);
    }
}

TEST(CircularBufferTest, DoesNotAccessOutOfBoundsOnEmptyPop)
{
    uint8_t data_out = 0;
    // Only modifiy ctx to setup a test, never in production.
    circular_buffer_ctx ctx = {
        .buff_size = 256,
        .buffer = { 0 },
        .head = 0,
        .tail = MAX_BUFFER_SIZE, // out of bounds index
        .current_byte_count = 0,
    };

    ASSERT_EQ(false, circular_buffer_pop(&ctx, &data_out));
}

TEST(CircularBufferTest, DoesNotAccessOutOfBoundsOnFullPush)
{
    uint8_t data_in = 0;
    // Only modifiy ctx to setup a test, never in production.
    circular_buffer_ctx ctx = {
        .buff_size = 256,
        .buffer = { 0 },
        .head = MAX_BUFFER_SIZE, // out of bounds index
        .tail = 0,
        .current_byte_count = 0,
    };

    ASSERT_EQ(false, circular_buffer_push(&ctx, data_in));
}

TEST(CircularBufferTest, OverwritesOldestValueIfFullOnPush)
{

}
