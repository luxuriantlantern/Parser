cmake_minimum_required(VERSION 3.10)

# 工程名和语言
project(Parser C)

# 设定 C 标准（根据需要可改成 11/17 等）
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 源文件
add_executable(parser_gen
    parser_gen.c
)

add_executable(test
    parser_gen.c
    test.c
)

# 头文件所在目录（当前源码根目录）
target_include_directories(parser_gen PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
