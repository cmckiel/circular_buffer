
# Circular Byte Buffer

A simple, robust, and production-quality circular byte buffer written in C, with extensive unit tests.

---

## Overview

This library provides a fixed-size circular (ring) buffer implementation for storing byte data (`uint8_t`). It supports multiple independent buffer instances, defensive integrity checking, and automatic overwrite when full.

Designed for embedded systems or performance-critical applications where heap allocation is undesired.

---

## Features

- Fixed-capacity circular buffer.
- Supports multiple independent buffer instances.
- Automatic overwrite of oldest data when buffer is full.
- Defensive runtime validation to prevent data corruption.
- Full unit test suite using Google Test.
- Pure C99 implementation (portable).
- Zero dynamic memory allocations.
- Peek API for non-destructive reads.

---

## API

### Types

```c
#define MAX_BUFFER_SIZE 1024

typedef struct {
    size_t buff_size;
    uint8_t buffer[MAX_BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
    uint32_t current_byte_count;
} circular_buffer_ctx;
```

The caller is responsible for:

- Allocating and maintaining `circular_buffer_ctx` instances.
- Not modifying `ctx` fields directly after initialization.

### Functions

```c
bool circular_buffer_init(circular_buffer_ctx *ctx, size_t buff_size);
bool circular_buffer_push(circular_buffer_ctx *ctx, uint8_t data);
bool circular_buffer_pop(circular_buffer_ctx *ctx, uint8_t *data);
bool circular_buffer_peek(circular_buffer_ctx *ctx, uint8_t *data);
```

- `circular_buffer_init()` initializes a buffer instance.
- `circular_buffer_push()` inserts data into the buffer.
- `circular_buffer_pop()` retrieves the oldest data from the buffer.
- `circular_buffer_peek()` allows looking at the next data without removing it.
- All functions return `true` on success, `false` on failure.

---

## Defensive Programming

The implementation includes internal safety checks to validate buffer state on every operation, defending against unexpected state corruption.

- Invalid arguments are safely rejected.
- Invalid buffer sizes (e.g. zero, too large) are rejected.
- Internal consistency of the buffer context is continuously validated.

---

## Tests

Unit tests are provided under `circular_buffer_test.cc` using [Google Test](https://github.com/google/googletest):

- Initialization tests
- Push/pop functional tests
- Overwrite behavior tests
- Peek functionality tests
- Edge case tests (empty, full, out-of-bounds, null arguments)
- Validation tests for invalid states

### Run Tests

```bash
mkdir build
cd build
cmake ..
make
./circular_buffer_tests
```

> (Assumes CMake project setup and Google Test installed)

---

## Building

You can build and integrate this library into any C99-compliant project.

### Files

- `circular_buffer.h`  (public API)
- `circular_buffer.c`  (implementation)
- `circular_buffer_test.cc`  (test suite)
- `main.c` (simple demonstration)

---

## Usage Example

```c
circular_buffer_ctx ctx;

if (circular_buffer_init(&ctx, 256)) {
    circular_buffer_push(&ctx, 0x42);

    uint8_t value = 0;
    if (circular_buffer_peek(&ctx, &value)) {
        printf("Peeked value: %u\n", value);
    }

    if (circular_buffer_pop(&ctx, &value)) {
        printf("Popped value: %u\n", value);
    }
}
```

---

## License

@todo

---

## Author

Cory McKiel

---

## Notes

- No dynamic memory usage.
- Suitable for embedded systems.
- Extensively tested, production-ready.
