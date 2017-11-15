cmake_minimum_required(VERSION 2.8)
project(Swarming_project)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(src)

set(SOURCE_FILES
        main.cpp
        src/data_structures/Boid.h
        src/data_structures/Boid.cpp
        src/definitions/types.h)
add_executable(Swarming_project ${SOURCE_FILES})
