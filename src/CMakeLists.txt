add_executable(CircularBuffer main.c)

add_subdirectory(circular_buffer)
target_link_libraries(CircularBuffer PRIVATE circular_buffer)
