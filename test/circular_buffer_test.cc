#include <gtest/gtest.h>
#include <stdbool.h>

extern "C" {
#include "circular_buffer.h"
}

class CircularBufferTest : public ::testing::Test {
protected:
    circular_buffer_ctx ctx;
    size_t buff_size = 256;

    void SetUp() override {
        ASSERT_TRUE(circular_buffer_init(&ctx, buff_size));
    }
};

/****************** SECTION: Initialization ************************/

TEST(CircularBufferInitTest, InitHandlesNULLCtx)
{
    size_t buff_size = 256;
    ASSERT_FALSE(circular_buffer_init(NULL, buff_size));
}

TEST(CircularBufferInitTest, InitDoesNotAllowBuffSizeOfZero)
{
    size_t buff_size = 0;
    circular_buffer_ctx ctx;
    ASSERT_FALSE(circular_buffer_init(&ctx, buff_size));
}

TEST(CircularBufferInitTest, InitDoesNotAllowBuffSizeGreaterThanMax)
{
    circular_buffer_ctx ctx;
    size_t buff_size = CIRCULAR_BUFFER_MAX_SIZE + 1; // size greater than the max
    ASSERT_FALSE(circular_buffer_init(&ctx, buff_size));
}

TEST(CircularBufferInitTest, InitDoesAllowBuffSizeLessThanMax)
{
    circular_buffer_ctx ctx;
    size_t buff_size = CIRCULAR_BUFFER_MAX_SIZE - 1; // size less than the max
    ASSERT_TRUE(circular_buffer_init(&ctx, buff_size));
}

TEST(CircularBufferInitTest, InitDoesAllowBuffSizeEqualToMax)
{
    circular_buffer_ctx ctx;
    size_t buff_size = CIRCULAR_BUFFER_MAX_SIZE; // size equal to the max
    ASSERT_TRUE(circular_buffer_init(&ctx, buff_size));
}

/****************** SECTION: NULL Inputs ************************/

TEST_F(CircularBufferTest, PopHandlesNullCtx)
{
    uint8_t data_in = 0;
    ASSERT_FALSE(circular_buffer_pop(NULL, &data_in));
}

TEST_F(CircularBufferTest, PopHandlesNullData)
{
    ASSERT_FALSE(circular_buffer_pop(&ctx, NULL));
}

TEST_F(CircularBufferTest, PushHandlesNullCtx)
{
    uint8_t data_in = 0;
    ASSERT_FALSE(circular_buffer_push(NULL, data_in));
}

TEST_F(CircularBufferTest, PeekHandlesNullCtx)
{
    uint8_t data_in = 0;
    ASSERT_FALSE(circular_buffer_peek(NULL, &data_in));
}

TEST_F(CircularBufferTest, PeekHandlesNullData)
{
    ASSERT_FALSE(circular_buffer_peek(&ctx, NULL));
}

TEST_F(CircularBufferTest, IsEmptyHandlesNullCtx)
{
    ASSERT_TRUE(circular_buffer_is_empty(NULL)); // Report the buffer is "empty" for NULL ctx's.
}

TEST_F(CircularBufferTest, GetOverflowCountHandlesNullCtx)
{
    uint32_t overflow_count = 0;
    ASSERT_FALSE(circular_buffer_get_overflow_count(NULL, &overflow_count));
}

TEST_F(CircularBufferTest, GetOverflowCountHandlesNullOverflowCount)
{
    ASSERT_FALSE(circular_buffer_get_overflow_count(&ctx, NULL));
}

/****************** SECTION: Basic Usage ************************/

TEST_F(CircularBufferTest, PushData)
{
    uint8_t data_in = 4;
    ASSERT_TRUE(circular_buffer_push(&ctx, data_in));
}

TEST_F(CircularBufferTest, PushPopData)
{
    uint8_t data_in = 0x49, data_out = 0;
    ASSERT_TRUE(circular_buffer_push(&ctx, data_in));
    ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out));
    EXPECT_EQ(data_in, data_out);
}

TEST_F(CircularBufferTest, PushPopDataNTimes)
{
    uint8_t data_in[buff_size] = { 0 };
    uint8_t data_out[buff_size] = { 0 };

    // Push data in.
    for (size_t i = 0; i < buff_size; i++)
    {
        data_in[i] = i % 256;
        ASSERT_TRUE(circular_buffer_push(&ctx, data_in[i]));
    }

    // FIFO
    // Pop data out.
    for (size_t i = 0; i < buff_size; i++)
    {
        ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out[i]));
    }

    // Verify what was pushed was popped.
    for (int i = 0; i < buff_size; i++)
    {
        EXPECT_EQ(data_in[i], data_out[i]);
    }
}

TEST_F(CircularBufferTest, PeekData)
{
    // Basic peeking.
    uint8_t data_in = 0x32, data_out = 0;
    ASSERT_TRUE(circular_buffer_push(&ctx, data_in));
    ASSERT_TRUE(circular_buffer_peek(&ctx, &data_out));
    ASSERT_EQ(data_in, data_out);

    // Ensure that peek didn't pop the data.
    data_out = 0;
    ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out));
    ASSERT_EQ(data_in, data_out);
}

TEST_F(CircularBufferTest, PeekDataNTimes)
{
    for (size_t i = 0; i < buff_size; i++)
    {
        ASSERT_TRUE(circular_buffer_push(&ctx, i % 256));
    }

    for (size_t i = 0; i < buff_size; i++)
    {
        // Peek without popping.
        uint8_t data_out = 0;
        ASSERT_TRUE(circular_buffer_peek(&ctx, &data_out));
        ASSERT_EQ(data_out, i % 256);

        // Pop and make sure data is still there.
        data_out = 0;
        ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out));
        ASSERT_EQ(data_out, i % 256);
    }
}

TEST_F(CircularBufferTest, IsEmptyReturnsFalseForNonEmptyBuffer)
{
    uint8_t data_in = 24;
    ASSERT_TRUE(circular_buffer_push(&ctx, data_in)); // Non empty buffer.
    ASSERT_FALSE(circular_buffer_is_empty(&ctx));
}

TEST_F(CircularBufferTest, IsEmptyReturnsTrueForFreshBuffer)
{
    ASSERT_TRUE(circular_buffer_is_empty(&ctx));
}

TEST_F(CircularBufferTest, IsEmptyReturnsTrueForEmptyBuffer)
{
    uint8_t data_in = 24, data_out = 0;
    ASSERT_TRUE(circular_buffer_push(&ctx, data_in));
    ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out));
    ASSERT_TRUE(circular_buffer_is_empty(&ctx));
}

TEST_F(CircularBufferTest, GetOverflowCountRetrievesCorrectCount)
{
    uint32_t overflow_count = 0;
    uint32_t amount_to_overflow = 4;
    uint32_t second_amount_to_overflow = 8;

    for (size_t i = 0; i < (buff_size + amount_to_overflow); i++)
    {
        ASSERT_TRUE(circular_buffer_push(&ctx, i % 256));
    }

    // Verify the overflow amounts.
    ASSERT_TRUE(circular_buffer_get_overflow_count(&ctx, &overflow_count));
    EXPECT_EQ(overflow_count, amount_to_overflow);

    // Write more values, should overflow by a total of (amount_to_overflow + second_amount_to_overflow) now.
    for (size_t i = 0; i < second_amount_to_overflow; i++)
    {
        ASSERT_TRUE(circular_buffer_push(&ctx, i % 256));
    }

    // Verify the overflow increased as expected.
    overflow_count = 0;
    ASSERT_TRUE(circular_buffer_get_overflow_count(&ctx, &overflow_count));
    EXPECT_EQ(overflow_count, (amount_to_overflow + second_amount_to_overflow));
}

TEST_F(CircularBufferTest, ClearOverflowCount)
{
    uint32_t overflow_count = 0;
    uint32_t amount_to_overflow = 15;

    for (size_t i = 0; i < (buff_size + amount_to_overflow); i++)
    {
        ASSERT_TRUE(circular_buffer_push(&ctx, i % 256));
    }

    // Confirm some non-zero overflow has occurred.
    ASSERT_TRUE(circular_buffer_get_overflow_count(&ctx, &overflow_count));
    EXPECT_EQ(overflow_count, amount_to_overflow);

    // Rest the overflow counter.
    ASSERT_TRUE(circular_buffer_clear_overflow_count(&ctx));

    // Verify reset.
    ASSERT_TRUE(circular_buffer_get_overflow_count(&ctx, &overflow_count));
    EXPECT_EQ(overflow_count, 0);
}

/****************** SECTION: More Comprehensive Usage ************************/

TEST_F(CircularBufferTest, OverwritesOldestValueIfFullOnPush)
{
    uint8_t data_out = 0;
    uint32_t overflow_count = 0;

    // Fill buffer with 1's.
    for (size_t i = 0; i < buff_size; i++)
    {
        ASSERT_TRUE(circular_buffer_push(&ctx, 1));
    }

    // Confirm no overwrites have taken place yet.
    ASSERT_TRUE(circular_buffer_get_overflow_count(&ctx, &overflow_count));
    EXPECT_EQ(overflow_count, 0);

    // Write one more 2, which overwrites the first 1.
    ASSERT_TRUE(circular_buffer_push(&ctx, 2));

    // Confirm that exactly one overwrite took place.
    ASSERT_TRUE(circular_buffer_get_overflow_count(&ctx, &overflow_count));
    EXPECT_EQ(overflow_count, 1);

    // Pop all of the 1's. There should be (buff_size - 1) 1's, since
    // the first was overwritten with a 2.
    for (size_t i = 0; i < (buff_size - 1); i++)
    {
        data_out = 0;
        ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out));
        EXPECT_EQ(data_out, 1);
    }

    // All of the 1's should be gone, next pop should be the most recently pushed 2.
    data_out = 0;
    ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out));
    EXPECT_EQ(data_out, 2);

    // Buffer should now be empty. Another pop would be false.
    ASSERT_FALSE(circular_buffer_pop(&ctx, &data_out));
}

TEST_F(CircularBufferTest, HeadWrapsAroundBeforeTail)
{
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
        while (circular_buffer_pop(&ctx, &data_out) && infinite_loop_protection <= CIRCULAR_BUFFER_MAX_SIZE)
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
            if (infinite_loop_protection >= CIRCULAR_BUFFER_MAX_SIZE)
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

/****************** SECTION: Fault Handling and Edge Cases ************************/

TEST_F(CircularBufferTest, PopFailsForFreshBuffer)
{
    uint8_t data_out = 0;
    ASSERT_FALSE(circular_buffer_pop(&ctx, &data_out));
}

TEST_F(CircularBufferTest, PopFailsForEmptyBuffer)
{
    uint8_t data_out = 0;
    ASSERT_TRUE(circular_buffer_push(&ctx, 1));
    // First call should be okay, since there is one element in the buffer.
    ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out));
    // Second call should NOT be okay, since the last item was removed.
    ASSERT_FALSE(circular_buffer_pop(&ctx, &data_out));
}

TEST_F(CircularBufferTest, PeekFailsForFreshBuffer)
{
    uint8_t data_out = 0;
    ASSERT_FALSE(circular_buffer_peek(&ctx, &data_out));
}

TEST_F(CircularBufferTest, PeekFailsForEmptyBuffer)
{
    uint8_t data_out = 0;
    ASSERT_TRUE(circular_buffer_push(&ctx, 1));
    // First call should be okay, since there is one element in the buffer.
    ASSERT_TRUE(circular_buffer_peek(&ctx, &data_out));
    // Second call should NOT be okay, since the last item was removed.
    ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out));
    ASSERT_FALSE(circular_buffer_peek(&ctx, &data_out));
}

TEST_F(CircularBufferTest, PopProtectsAgainstCorruptCtx)
{
    uint8_t data_out = 0;
    circular_buffer_ctx corrupt_ctx = ctx;
    corrupt_ctx.tail = CIRCULAR_BUFFER_MAX_SIZE; // out of bounds index
    ASSERT_FALSE(circular_buffer_pop(&corrupt_ctx, &data_out));
}

TEST_F(CircularBufferTest, PushProtectsAgainstCorruptCtx)
{
    uint8_t data_in = 0;
    circular_buffer_ctx corrupt_ctx = ctx;
    corrupt_ctx.head = CIRCULAR_BUFFER_MAX_SIZE; // out of bounds index
    ASSERT_FALSE(circular_buffer_push(&corrupt_ctx, data_in));
}

TEST_F(CircularBufferTest, PeekProtectsAgainstCorruptCtx)
{
    uint8_t data_out = 0;
    circular_buffer_ctx corrupt_ctx = ctx;
    corrupt_ctx.tail = CIRCULAR_BUFFER_MAX_SIZE; // out of bounds index
    ASSERT_FALSE(circular_buffer_peek(&corrupt_ctx, &data_out));
}

// @todo Rework this test to use it's current buff size.
// TEST_F(CircularBufferTest, PushPopDataNTimes)
// {
//     circular_buffer_ctx ctx;
//     size_t buff_size = CIRCULAR_BUFFER_MAX_SIZE;
//     uint8_t data[CIRCULAR_BUFFER_MAX_SIZE] = { 0 };

//     circular_buffer_init(&ctx, buff_size);

//     for (int i = 0; i < CIRCULAR_BUFFER_MAX_SIZE; i++)
//     {
//         uint8_t data_in = i % 256;
//         circular_buffer_push(&ctx, data_in);
//         data[i] = data_in; // remember what we pushed.
//     }

//     // FIFO
//     for (int i = 0; i < CIRCULAR_BUFFER_MAX_SIZE; i++)
//     {
//         uint8_t data_out = 0;
//         circular_buffer_pop(&ctx, &data_out);
//         ASSERT_EQ(data[i], data_out);
//     }
// }
