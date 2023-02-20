cmake_minimum_required(VERSION 3.10)

project(arrow)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(arrow main.cpp)

target_compile_options(arrow PRIVATE -Wall -Wextra -Wpedantic)




