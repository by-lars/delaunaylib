cmake_minimum_required(VERSION 3.10)
project(delaunaylib_tests VERSION 1.0.0 DESCRIPTION "Delaunay Triangulation Libraray Unit Tests")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
        src/test_geometric_primitives.cpp
)

# GTEST
include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/530d5c8c84abd2a46f38583ee817743c9b3a42b4.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} GTest::gtest_main delaunaylib)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

