
# Circular Byte Buffer

A simple, robust, and well-tested circular byte buffer written in C with embedded applications in mind.

---

## Overview

This library provides a fixed-size circular (ring) buffer implementation for storing byte data (`uint8_t`). It supports multiple independent buffer instances, defensive integrity checking, and automatic overwrite when full.

Designed for embedded systems or performance-critical applications where heap allocation is undesired.

---

## Features

- Fixed-capacity circular buffer.
- Zero dynamic memory allocations.
- Prioritizes safety and reliability over flexibility.
- Supports multiple independent buffer instances.
- Automatic overwrite of oldest data when buffer is full.
- Defensive runtime validation to prevent data corruption.
- Full unit test suite using Google Test.

---

## Thread Safety
> ⚠ This implementation is **not thread-safe** by design.

This allows you to apply appropriate protections depending on your platform:
- Critical section / interrupt disabling (bare-metal)
- Mutex (RTOS, multithreaded)
- Lock-free synchronization (advanced)

## API

### Types

```c
#ifndef CIRCULAR_BUFFER_MAX_SIZE
#define CIRCULAR_BUFFER_MAX_SIZE 1024
#endif

typedef struct {
    size_t buff_size;
    uint8_t buffer[CIRCULAR_BUFFER_MAX_SIZE];
    size_t head;
    size_t tail;
    size_t current_byte_count;
    uint32_t overflow_count;
} circular_buffer_ctx;
```

The caller is responsible for:

- Allocating and maintaining `circular_buffer_ctx` instances.
- Not modifying `ctx` fields directly after initialization.
- Ensuring thread-safety.

### Functions

```c
bool circular_buffer_init(circular_buffer_ctx *ctx, size_t buff_size);
bool circular_buffer_push(circular_buffer_ctx *ctx, uint8_t data);
bool circular_buffer_pop(circular_buffer_ctx *ctx, uint8_t *data);
bool circular_buffer_peek(circular_buffer_ctx *ctx, uint8_t *data);
bool circular_buffer_is_empty(circular_buffer_ctx *ctx);
bool circular_buffer_get_overflow_count(circular_buffer_ctx *ctx, uint32_t *overflow_count);
bool circular_buffer_clear_overflow_count(circular_buffer_ctx *ctx);
```

- `circular_buffer_init()` initializes a buffer instance.
- `circular_buffer_push()` inserts data into the buffer.
- `circular_buffer_pop()` retrieves the oldest data from the buffer.
- `circular_buffer_peek()` allows looking at the next data without removing it.
- `circular_buffer_is_empty()` quickly informs if there is data in the buffer.
- `circular_buffer_get_overflow_count()` retrieves the number of bytes lost to overflow.
- `circular_buffer_clear_overflow_count()` resets the overflow count.
- All functions return `true` on success, `false` on failure (except `is_empty()`).

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
cmake --build .
ctest .
```

> (Assumes CMake project setup and Google Test installed)

### Run Valgrind Memcheck

This Project includes a build target for run Valgrind Memcheck on the unit test suite to verify that no memory is leaked or accessed incorrectly.

To run:
```bash
mkdir build
cd build
cmake ..
cmake --build . -t valgrind
```
This runs all of the tests as well and verifies correct memory handling, producing a report at the end.

---

## Building

You can build and integrate this library into any C99-compliant project.

### Files

- `circular_buffer.h`  (public API)
- `circular_buffer.c`  (implementation)
- `circular_buffer_test.cc`  (test suite)
- `main.c` (simple demonstration)

### Dev container

A dev container was added for convenience. For example, when developing on Apple silicon valgrind is not available natively. The container solves that.
If you install the dev container extension on vscode and have docker installed, you can use the container. Otherwise, you'll have to set up your own dev environment
to include cmake, gcc, gtest, valgrind, etc.

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
- Extensively tested.
