cmake_minimum_required(VERSION 3.16)
project(High_Performance_Limit_Order_Book CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

# Compiler-specific options
if (MSVC)
    # /W4 = warnings, /O2 only in Release
    add_compile_options(/W4)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
else()
    add_compile_options(-Wall -Wextra)
endif()

add_executable(orderbook
    src/main.cpp
    src/Book.cpp
    src/Limit.cpp
)
