cmake_minimum_required(VERSION 2.8)
project(Swarming_project)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-std=gnu++11")

include_directories(src)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(SOURCE_FILES
        # Entry point
        main.cpp
        # Data structures
        src/data_structures/Boid.h
        src/data_structures/Grid.h
		src/data_structures/Octree.h
        src/data_structures/Linear_Octree.h
		src/data_structures/MathArray.h
        # Definitions
        src/definitions/types.h
        src/definitions/graphical_constants.h
        # Visualization part
        src/visualization/GridVisualizer.h
        src/visualization/vtkTimerCallback.h
        src/definitions/constants.h)

add_executable(Swarming_project ${SOURCE_FILES})
target_link_libraries(Swarming_project ${VTK_LIBRARIES})
