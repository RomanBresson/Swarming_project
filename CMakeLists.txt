cmake_minimum_required(VERSION 3.2)
project(Swarming_project)


set(CMAKE_CXX_STANDARD 11)

include_directories(src)

set(SOURCE_FILES
        main.cpp
        src/data_structures/Boid.h
        src/definitions/types.h)
add_executable(Swarming_project ${SOURCE_FILES})