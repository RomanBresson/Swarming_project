cmake_minimum_required(VERSION 2.8)
project(Swarming_project)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-std=gnu++11")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_LINKER)
include_directories(src)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

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
        src/definitions/constants.h
		src/algorithms/sample_sort.h
        src/algorithms/remove_duplicates.h
        src/algorithms/morton_index.h
        #src/algorithms/balance_subtree.h
        src/algorithms/linearise.h
        src/algorithms/complete_region.h
        src/algorithms/complete_octree.h
        src/algorithms/partition.h
		src/algorithms/distributed_scan.h
        src/algorithms/merge_sorted_arrays.h
        src/algorithms/points2octree.h
        src/algorithms/block_partition.h
        src/algorithms/is_sorted_distributed.h
        src/algorithms/sorted_search_distributed.h
		src/algorithms/sorted_count_distributed.h
		src/algorithms/sorted_range_count_distributed.h)

add_executable(Swarming_project ${SOURCE_FILES})
target_link_libraries(Swarming_project ${VTK_LIBRARIES} ${MPI_LIBRARIES})


if (MPI_COMPILE_FLAGS)
    set_target_properties(Swarming_project PROPERTIES
            COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif ()

if (MPI_LINK_FLAGS)
    set_target_properties(Swarming_project PROPERTIES
            LINK_FLAGS "${MPI_LINK_FLAGS}")
endif ()
