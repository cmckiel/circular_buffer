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

TEST(CircularBufferTest, InitHandlesNULLCtx)
{
    size_t buff_size = 256;
    ASSERT_EQ(false, circular_buffer_init(NULL, buff_size));
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

TEST(CircularBufferTest, PeekReturnsFalseForNULLCtx)
{
    uint8_t data = 0;
    ASSERT_EQ(false, circular_buffer_peek(NULL, &data));
}

TEST(CircularBufferTest, PeekReturnsFalseForNULLData)
{
    circular_buffer_ctx ctx;
    ASSERT_EQ(false, circular_buffer_peek(&ctx, NULL));
}

TEST(CircularBufferTest, IsEmptyReturnsTrueForNULLCtx)
{
    ASSERT_EQ(true, circular_buffer_is_empty(NULL));
}

TEST(CircularBufferTest, DoesNotAllowBuffSizeInitGreaterThanMax)
{
    size_t buff_size = MAX_BUFFER_SIZE + 1; // size greater than the max
    circular_buffer_ctx ctx;

    ASSERT_EQ(false, circular_buffer_init(&ctx, buff_size));
}

TEST(CircularBufferTest, DoesNotAllowBuffSizeOfZeroDuringInit)
{
    size_t buff_size = 0;
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

TEST(CircularBufferTest, DoesPopWhatItPushesNTimes)
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
    size_t buff_size = MAX_BUFFER_SIZE;
    circular_buffer_ctx ctx;

    circular_buffer_init(&ctx, buff_size);

    // Fill buffer with 1's.
    for (size_t i = 0; i < MAX_BUFFER_SIZE; i++)
    {
        circular_buffer_push(&ctx, 1);
    }

    // Write one more 2, which overwrites the first 1.
    circular_buffer_push(&ctx, 2);

    uint8_t data_out = 0;

    // Pop all of the 1's. There should be (MAX_BUFFER_SIZE - 1) 1's, since
    // the first was overwritten with a 2.
    for (size_t i = 0; i < (MAX_BUFFER_SIZE - 1); i++)
    {
        data_out = 0;
        circular_buffer_pop(&ctx, &data_out);

        ASSERT_EQ(1, data_out);
    }

    // All of the 1's should be gone, next pop should be the most recently pushed 2.
    data_out = 0;
    circular_buffer_pop(&ctx, &data_out);

    ASSERT_EQ(2, data_out);
}

TEST(CircularBufferTest, PreventsPoppingEmptyBuffer)
{
    uint8_t data_out = 0;
    size_t buff_size = MAX_BUFFER_SIZE;
    circular_buffer_ctx ctx;

    circular_buffer_init(&ctx, buff_size);

    circular_buffer_push(&ctx, 1);

    // First call should be okay, since there is one element in the buffer.
    ASSERT_EQ(true, circular_buffer_pop(&ctx, &data_out));

    // Second call should NOT be okay, since the last item was removed.
    ASSERT_EQ(false, circular_buffer_pop(&ctx, &data_out));
}

TEST(CircularBufferTest, OnlyAllowsUpToBuffSizeBytesToBeWritten)
{
    size_t buff_size = 256;
    circular_buffer_ctx ctx;

    circular_buffer_init(&ctx, buff_size);

    // Fill buffer with 1's.
    for (size_t i = 0; i < buff_size; i++)
    {
        circular_buffer_push(&ctx, 1);
    }

    // Write one more 2, which should overwrites the first 1.
    circular_buffer_push(&ctx, 2);

    uint8_t data_out = 0;

    // Pop all of the 1's. There should be (buff_size - 1) 1's, since
    // the first was overwritten with a 2.
    for (size_t i = 0; i < (buff_size - 1); i++)
    {
        data_out = 0;
        circular_buffer_pop(&ctx, &data_out);

        ASSERT_EQ(1, data_out);
    }

    // All of the 1's should be gone, next pop should be the most recently pushed 2.
    data_out = 0;
    circular_buffer_pop(&ctx, &data_out);

    ASSERT_EQ(2, data_out);
}

TEST(CircularBufferTest, HeadWrapsAroundBeforeTail)
{
    size_t buff_size = 8;
    circular_buffer_ctx ctx;

    circular_buffer_init(&ctx, buff_size);

    // Push a couple times.
    for (size_t i = 0; i < 5; i++)
    {
        uint8_t data_in = 253; // random data
        ASSERT_EQ(true, circular_buffer_push(&ctx, data_in));
    }

    // Pop a couple times less.
    for (size_t i = 0; i < 3; i++)
    {
        uint8_t data_out = 0;
        ASSERT_EQ(true, circular_buffer_pop(&ctx, &data_out));
        ASSERT_EQ(253, data_out);
    }

    // These pushes will wrap around.
    for (size_t i = 0; i < 5; i++)
    {
        uint8_t data_in = 112; // random data
        ASSERT_EQ(true, circular_buffer_push(&ctx, data_in));
    }

    // Buffer should look like this:
    // [112, 112, empty, 253, 253, 112, 112, 112]
    //        ^           ^
    //        head        tail

    const size_t EXPECTED_DATA_SIZE = 7;
    uint8_t expected_data[EXPECTED_DATA_SIZE] = { 253, 253, 112, 112, 112, 112, 112 };

    for (size_t i = 0; i < EXPECTED_DATA_SIZE; i++)
    {
        uint8_t data_out = 0;
        ASSERT_EQ(true, circular_buffer_pop(&ctx, &data_out));
        ASSERT_EQ(expected_data[i], data_out);
    }

    // Next pop should be empty.
    uint8_t data_out = 0;
    ASSERT_EQ(false, circular_buffer_pop(&ctx, &data_out));
}

TEST(CircularBufferTest, AllowsPeekingWithoutRemoval)
{
    size_t buff_size = 8;
    circular_buffer_ctx ctx;

    ASSERT_EQ(true, circular_buffer_init(&ctx, buff_size));

    for (uint8_t i = 0; i < 4; i++)
    {
        ASSERT_EQ(true, circular_buffer_push(&ctx, i + 1));
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        // Peek without popping.
        uint8_t data_out = 0;
        ASSERT_EQ(true, circular_buffer_peek(&ctx, &data_out));
        ASSERT_EQ(i + 1, data_out);

        // Pop and make sure data is still there.
        data_out = 0;
        ASSERT_EQ(true, circular_buffer_pop(&ctx, &data_out));
        ASSERT_EQ(i + 1, data_out);
    }
}

TEST(CircularBufferTest, PeekReturnsFalseOnEmptyBuffer)
{
    uint8_t data = 0;
    size_t buff_size = 8;
    circular_buffer_ctx ctx;

    ASSERT_EQ(true, circular_buffer_init(&ctx, buff_size));
    ASSERT_EQ(false, circular_buffer_peek(&ctx, &data));
}

TEST(CircularBufferTest, ReturnsFalseForNonEmptyBuffer)
{
    // Setup
    uint8_t data = 24;
    size_t buff_size = 16;
    circular_buffer_ctx ctx;
    ASSERT_EQ(true, circular_buffer_init(&ctx, buff_size));

    // Non empty buffer.
    ASSERT_EQ(true, circular_buffer_push(&ctx, data));

    ASSERT_EQ(false, circular_buffer_is_empty(&ctx));
}

TEST(CircularBufferTest, ReturnsTrueForEmptyBuffer)
{
    // Setup
    uint8_t data = 24;
    size_t buff_size = 16;
    circular_buffer_ctx ctx;
    ASSERT_EQ(true, circular_buffer_init(&ctx, buff_size));

    // No push and empty buffer.

    ASSERT_EQ(true, circular_buffer_is_empty(&ctx));
}

// @todo find a more reasonable pattern in the data to test.
TEST(CircularBufferTest, SupportsNormalUse)
{
    // Setup
    uint8_t data = 24;
    size_t buff_size = 256;
    circular_buffer_ctx ctx;
    ASSERT_EQ(true, circular_buffer_init(&ctx, buff_size));

    // Write a bunch of values.
    for (uint8_t i = 0; i < 200; i++)
    {
        ASSERT_EQ(true, circular_buffer_push(&ctx, i));
    }

    // Read some.
    for (uint8_t i = 0; i < 65; i++)
    {
        uint8_t data_out = 0;
        ASSERT_EQ(true, circular_buffer_pop(&ctx, &data_out));
        ASSERT_EQ(i, data_out);
    }

    // Write a bunch more values.
    for (uint8_t i = 0; i < 175; i++)
    {
        ASSERT_EQ(true, circular_buffer_push(&ctx, i));
    }

    // Check if it is empty and pop all the data.
    ASSERT_EQ(false, circular_buffer_is_empty(&ctx));
    if (!circular_buffer_is_empty(&ctx))
    {
        uint8_t data_out = 0;
        size_t infinite_loop_protection = 0;
        size_t expected_data_out = 119;  // This number is based on math from the above loops. Sorry.
        while (circular_buffer_pop(&ctx, &data_out) && infinite_loop_protection <= MAX_BUFFER_SIZE)
        {
            // Check that we're getting the data we expect.
            ASSERT_EQ(expected_data_out, data_out);

            // This is a crazy way to test this, I admit.
            expected_data_out++;
            if (expected_data_out == 200)
            {
                expected_data_out = 0;
            }

            infinite_loop_protection++;
            if (infinite_loop_protection >= MAX_BUFFER_SIZE)
            {
                // Infinite loop detected. This means pop() didn't return false as it should.
                ASSERT_EQ(false, true);
            }
        }
    }

    // All the data should be out now.
    ASSERT_EQ(true, circular_buffer_is_empty(&ctx));
    // A second call should do say the same thing.
    ASSERT_EQ(true, circular_buffer_is_empty(&ctx));
}
