cmake_minimum_required(VERSION 3.22)
project(CircularBuffer)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(NOT CMAKE_CROSSCOMPILING)
    # GoogleTest requires at least C++14
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    #For Windows: Prevent overriding the parent project's compiler/linker settings
    #set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    add_subdirectory(test)
    include(CTest)
    enable_testing()
endif()

add_subdirectory(src)
