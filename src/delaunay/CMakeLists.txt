cmake_minimum_required(VERSION 3.10)
project(delaunaylib VERSION 1.0.0 DESCRIPTION "Delaunay Triangulation Libraray")

set(SOURCES 
        src/delaunay.cpp
        src/quad_edge.cpp
        src/point.cpp
        include/delaunay/types.hpp
        include/delaunay/types.hpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE src)

add_subdirectory(tests)