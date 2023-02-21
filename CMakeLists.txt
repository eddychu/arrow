cmake_minimum_required(VERSION 3.10)

project(arrow)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# glob all source and header file
file(GLOB MY_SOURCE_FILES "src/*.cpp" "src/*.h")


add_executable(arrow ${MY_SOURCE_FILES})

target_compile_options(arrow PRIVATE -Wall -Wextra -Wpedantic)




