cmake_minimum_required(VERSION 3.10)
project(delaunaylib VERSION 1.0.0 DESCRIPTION "Delaunay Triangulation Libraray")

set(SOURCES 
    src/delaunay.cpp
    )


add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)