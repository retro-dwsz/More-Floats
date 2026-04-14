cmake_minimum_required(VERSION 3.2)
project(Half_Float)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMMON_FLAGS
        -std=c++23
        -O0
        #        -mrdseed
        #        -mrdrnd
        #        -march=native
        -m64
        -fansi-escape-codes
        -fcolor-diagnostics
        -fdiagnostics-show-option
        -fcxx-exceptions
        -fexceptions
        -fcxx-exceptions
        -Wall
        -flto
        #        -v
        -Wno-deprecated-declarations
)

target_compile_options(Half_Float PRIVATE
        ${COMMON_FLAGS}
)

target_link_libraries(Half_Float PRIVATE fmt)

set_target_properties(Half_Float PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_executable(Half_Float main.cpp)
