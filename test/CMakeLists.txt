enable_testing()

add_executable(
    CircularBufferTest
    circular_buffer_test.cc
)
target_link_libraries(
    CircularBufferTest
    circular_buffer
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(CircularBufferTest)
