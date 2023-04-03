cmake_minimum_required(VERSION 3.10)

project(arrow)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# glob all source and header file
file(GLOB MY_SOURCE_FILES "src/*.cpp" "src/*.h")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(arrow ${MY_SOURCE_FILES})

target_compile_options(arrow PRIVATE -Wall -Wextra -Wpedantic)


find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(arrow PUBLIC OpenMP::OpenMP_CXX)
endif()

# find and load assimp
find_package(assimp REQUIRED)
target_include_directories(arrow PUBLIC ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(arrow PUBLIC assimp::assimp)

