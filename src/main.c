#include <stdio.h>
#include "circular_buffer.h"

int main(int argc, char **argv)
{
    // Fun test drive for the circular buffer.
    // Define two messages, one large enough to fit entirely in the buffer,
    // the other too large.
    char regular_message[] = "Regular message.\n";
    char message_with_overwrite[] = "Message with overwrite.\n";

    // Init the buffer to the size of a regular message, plus some extra.
    circular_buffer_ctx ctx;
    size_t buff_size = sizeof(regular_message) + 5;
    if (circular_buffer_init(&ctx, buff_size) == false)
    {
        fprintf(stderr, "Buffer init failed!\n");
        return -1;
    }

    // Push the regular message into the buffer one byte at a time.
    for (size_t i = 0; i < sizeof(regular_message); i++)
    {
        if (circular_buffer_push_with_overwrite(&ctx, regular_message[i]) == false)
        {
            fprintf(stderr, "Buffer push failed!\n");
            return -1;
        }
    }

    // Print out the regular message by popping and putting each byte.
    uint8_t c;
    while (circular_buffer_pop(&ctx, &c))
    {
        putchar(c);
    }

    // Reinit the buffer to start from scratch.
    if (circular_buffer_init(&ctx, buff_size) == false)
    {
        fprintf(stderr, "Buffer init failed!\n");
        return -1;
    }

    // Put the message into the buffer, and expect overwriting to occur.
    for (size_t i = 0; i < sizeof(message_with_overwrite); i++)
    {
        if (circular_buffer_push_with_overwrite(&ctx, message_with_overwrite[i]) == false)
        {
            fprintf(stderr, "Buffer push failed!\n");
            return -1;
        }
    }

    // Print out the message one byte at a time to demonstrate it was overwritten.
    while (circular_buffer_pop(&ctx, &c))
    {
        putchar(c);
    }

    return 0;
}
