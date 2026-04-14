cmake_minimum_required(VERSION 3.25)
project(Half_Float)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMMON_FLAGS
        -std=c++23
        -O0
        #        -mrdseed
        #        -mrdrnd
        #        -march=native
        -m64
        -Wall
        -flto
        #        -v
        -Wno-deprecated-declarations
)

find_package(fmt REQUIRED)

add_executable(Half_Float main.cpp)

target_compile_options(Half_Float PRIVATE
        ${COMMON_FLAGS}
)

target_link_libraries(Half_Float PRIVATE fmt::fmt)

set_target_properties(Half_Float PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
