#include <gtest/gtest.h>
#include <stdbool.h>
#include <time.h>

extern "C" {
#include "circular_buffer.h"
}

// Helper for random test data.
uint8_t random_uint8() {
    return (uint8_t)(rand() % 256);
}

class CircularBufferTest : public ::testing::Test {
private:
    static bool seed_is_set;
    int best_seed_ever = 42;
protected:
    size_t buff_size = 256;
    circular_buffer_ctx ctx;

    void SetUp() override {
        ASSERT_TRUE(circular_buffer_init(&ctx, buff_size));

        // Set up the seed only once per full testing run.
        if (!seed_is_set) {
            srand(best_seed_ever);
            seed_is_set = true;
        }
    }
};

bool CircularBufferTest::seed_is_set = false;

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
    ASSERT_FALSE(circular_buffer_push_with_overwrite(NULL, data_in));
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
    uint8_t data_in = random_uint8();
    ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in));
}

TEST_F(CircularBufferTest, PushPopData)
{
    uint8_t data_in = random_uint8(), data_out = 0;
    ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in));
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
        data_in[i] = random_uint8();
        ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in[i]));
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
    uint8_t data_in = random_uint8(), data_out = 0;
    ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in));
    ASSERT_TRUE(circular_buffer_peek(&ctx, &data_out));
    ASSERT_EQ(data_in, data_out);

    // Ensure that peek didn't pop the data.
    data_out = 0;
    ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out));
    ASSERT_EQ(data_in, data_out);
}

TEST_F(CircularBufferTest, PeekDataNTimes)
{
    uint8_t data_in[buff_size] = { 0 };

    for (size_t i = 0; i < buff_size; i++)
    {
        data_in[i] = random_uint8();
        ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in[i]));
    }

    for (size_t i = 0; i < buff_size; i++)
    {
        // Peek without popping.
        uint8_t data_peeked = 0;
        ASSERT_TRUE(circular_buffer_peek(&ctx, &data_peeked));
        ASSERT_EQ(data_peeked, data_in[i]);

        // Pop and make sure data is still there.
        uint8_t data_out = 0;
        ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out));
        ASSERT_EQ(data_out, data_peeked);
    }
}

TEST_F(CircularBufferTest, IsEmptyReturnsFalseForNonEmptyBuffer)
{
    uint8_t data_in = random_uint8();
    ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in)); // Non empty buffer.
    ASSERT_FALSE(circular_buffer_is_empty(&ctx));
}

TEST_F(CircularBufferTest, IsEmptyReturnsTrueForFreshBuffer)
{
    ASSERT_TRUE(circular_buffer_is_empty(&ctx));
}

TEST_F(CircularBufferTest, IsEmptyReturnsTrueForEmptyBuffer)
{
    uint8_t data_in = random_uint8(), data_out = 0;
    ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in));
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
        ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, random_uint8()));
    }

    // Verify the overflow amounts.
    ASSERT_TRUE(circular_buffer_get_overflow_count(&ctx, &overflow_count));
    EXPECT_EQ(overflow_count, amount_to_overflow);

    // Write more values, should overflow by a total of (amount_to_overflow + second_amount_to_overflow) now.
    for (size_t i = 0; i < second_amount_to_overflow; i++)
    {
        ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, random_uint8()));
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
        ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, random_uint8()));
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
        ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, 1));
    }

    // Confirm no overwrites have taken place yet.
    ASSERT_TRUE(circular_buffer_get_overflow_count(&ctx, &overflow_count));
    EXPECT_EQ(overflow_count, 0);

    // Write one more 2, which overwrites the first 1.
    ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, 2));

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
    // Memory to store everything that went in and out of the buffer for verification.
    size_t current_data_in_loc = 0;
    size_t current_data_out_loc = 0;
    uint8_t data_in[buff_size * 2] = { 0 };
    uint8_t data_out[buff_size * 2] = { 0 };

    // Size divisions based off of buff_size.
    size_t healthy_amount_of_data = (3 * buff_size) / 4;     // 75-ish% of buff_size
    size_t appreciable_amount_of_data = (1 * buff_size) / 3; // 33-ish% of buff_size
    size_t sizeable_amount_of_data = (1 * buff_size) / 4;    // 25-ish% of buff_size

    // Add a healthy amount of initial data to the buffer,
    // but don't fill it.
    for (size_t i = 0; i < healthy_amount_of_data && current_data_in_loc < sizeof(data_in); i++)
    {
        data_in[current_data_in_loc] = random_uint8();
        ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in[current_data_in_loc]));
        current_data_in_loc++;
    }

    // Pop off a sizeable amount of data.
    for (size_t i = 0; i < sizeable_amount_of_data &&
        i < buff_size && current_data_out_loc < sizeof(data_out); i++)
    {
        ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out[current_data_out_loc]));
        current_data_out_loc++;
    }

    // Adding an appreciable amount of data will cause head to wrap around but without
    // overwritting any data.
    for (size_t i = 0; i < appreciable_amount_of_data && current_data_in_loc < sizeof(data_in); i++)
    {
        data_in[current_data_in_loc] = random_uint8();
        ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in[current_data_in_loc]));
        current_data_in_loc++;
    }

    // Buffer should look something like this:
    // [117, 182, empty, 251, 203, 14, 132, 1]
    //        ^           ^
    //        head        tail

    // Pop the rest of everything out of the buffer.
    size_t i = 0;
    while (current_data_out_loc < sizeof(data_out) &&
           circular_buffer_pop(&ctx, &data_out[current_data_out_loc]) &&
           i < buff_size)
    {
        current_data_out_loc++;
        i++;
    }
    // All the data should be out now.
    ASSERT_TRUE(circular_buffer_is_empty(&ctx));

    // Verify everything that was put in was taken out in the correct order.
    // Only works if no data was overwritten and lost.
    for (size_t i = 0; i < sizeof(data_in) && i < sizeof(data_out); i++)
    {
        EXPECT_EQ(data_in[i], data_out[i]);
    }
}

TEST_F(CircularBufferTest, StressTest)
{
    // For keeping track of total in, total out.
    uint8_t data_in = 0, data_out = 0;
    size_t total_data_in_count = 0;
    size_t total_data_out_count = 0;

    // The number of overflows will be compared to the difference of (in_count - out_count).
    uint32_t overflow_count = 0;

    // Size divisions based off of buff_size.
    size_t healthy_amount_of_data = (3 * buff_size) / 4;     // 75-ish% of buff_size
    size_t appreciable_amount_of_data = (1 * buff_size) / 3; // 33-ish% of buff_size
    size_t sizeable_amount_of_data = (1 * buff_size) / 4;    // 25-ish% of buff_size
    size_t mega_amount_of_data = (132 * buff_size);          // 13,100% of buff_size

    // Add six healthy servings of initial data to the buffer,
    for (size_t i = 0; i < (6 * healthy_amount_of_data); i++)
    {
        data_in = random_uint8();
        ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in));
        total_data_in_count++;
    }

    // Pop off two sizeable amounts of data.
    for (size_t i = 0; i < (2 * sizeable_amount_of_data) && i < buff_size; i++)
    {
        ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out));
        total_data_out_count++;
    }

    // Add an appreciable amount of data.
    for (size_t i = 0; i < appreciable_amount_of_data; i++)
    {
        data_in = random_uint8();
        ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in));
        total_data_in_count++;
    }

    // Pop the rest of everything out of the buffer.
    size_t i = 0;
    while (circular_buffer_pop(&ctx, &data_out) && i < buff_size)
    {
        total_data_out_count++;
        i++;
    }
    // All the data should be out now.
    ASSERT_TRUE(circular_buffer_is_empty(&ctx));

    // Add an appreciable amount of data.
    for (size_t i = 0; i < appreciable_amount_of_data; i++)
    {
        data_in = random_uint8();
        ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in));
        total_data_in_count++;
    }

    // Add a mega amount of data.
    for (size_t i = 0; i < mega_amount_of_data; i++)
    {
        data_in = random_uint8();
        ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in));
        total_data_in_count++;
    }

    // Pop the rest of everything out of the buffer.
    i = 0;
    while (circular_buffer_pop(&ctx, &data_out) && i < buff_size)
    {
        total_data_out_count++;
        i++;
    }
    // All the data should be out now.
    ASSERT_TRUE(circular_buffer_is_empty(&ctx));

    // Retrieve the bytes lost and compare it to the difference of in vs out counts.
    ASSERT_TRUE(circular_buffer_get_overflow_count(&ctx, &overflow_count));
    ASSERT_EQ(overflow_count, (total_data_in_count - total_data_out_count));
}

TEST_F(CircularBufferTest, IsEmptyConsistency)
{
    // Push a piece of data into buffer.
    uint8_t data_in = 90, data_out = 0; // Random data
    ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, data_in));

    // Two repeating calls should yield same result.
    ASSERT_FALSE(circular_buffer_is_empty(&ctx));
    ASSERT_FALSE(circular_buffer_is_empty(&ctx));

    // Remove the data.
    ASSERT_TRUE(circular_buffer_pop(&ctx, &data_out));

    // Two repeating calls should yield same result.
    ASSERT_TRUE(circular_buffer_is_empty(&ctx));
    ASSERT_TRUE(circular_buffer_is_empty(&ctx));
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
    ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, 1));
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
    ASSERT_TRUE(circular_buffer_push_with_overwrite(&ctx, 1));
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
    ASSERT_FALSE(circular_buffer_push_with_overwrite(&corrupt_ctx, data_in));
}

TEST_F(CircularBufferTest, PeekProtectsAgainstCorruptCtx)
{
    uint8_t data_out = 0;
    circular_buffer_ctx corrupt_ctx = ctx;
    corrupt_ctx.tail = CIRCULAR_BUFFER_MAX_SIZE; // out of bounds index
    ASSERT_FALSE(circular_buffer_peek(&corrupt_ctx, &data_out));
}
