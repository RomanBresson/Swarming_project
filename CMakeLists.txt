cmake_minimum_required(VERSION 3.2)
project(Swarming_project)


set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp src/data_structures/boids.h src/definitions/types.h)
add_executable(Swarming_project ${SOURCE_FILES})