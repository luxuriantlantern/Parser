cmake_minimum_required(VERSION 3.10)

project(Parser C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(test
    parser_gen.c
    test.c
)

target_include_directories(test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
