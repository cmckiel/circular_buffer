function(AddValgrind target)
    find_program(VALGRIND_PATH valgrind REQUIRED)
    add_custom_target(valgrind
        COMMAND ${VALGRIND_PATH} --leak-check=full
                $<TARGET_FILE:${target}>
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endfunction()
