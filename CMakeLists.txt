cmake_minimum_required(VERSION 3.10)

# 工程名和语言
project(Parser C CXX)

# 设定 C 标准和 C++ 标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# parser_gen 可执行文件，只使用 C 源码
add_executable(parser_gen
    parser_gen.c
)

# test 可执行文件，使用 parser_gen.c 与 test.c
add_executable(test
    parser_gen.c
    test.c
)

# 独立的 main 可执行文件，只依赖 main.cpp，本身不依赖其他现有源码
add_executable(main
    main.cpp
)

# 头文件所在目录（当前源码根目录）
target_include_directories(parser_gen PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
