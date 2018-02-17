#include <iostream>
#include "data_structures/Octree.h"
#include "data_structures/Linear_Octree.h"
#include "data_structures/Boid.h"
#include "data_structures/MathArray.h"
#include "data_structures/Grid.h"
#include "visualization/GridVisualizer.h"
#include "mpi.h"
#include "mpi/MPI_sample_sort/sample_sort.h"
#include "algorithms/remove_duplicates.h"


//using types::Position;

#include "algorithms/morton_index.h"


template <typename T>
std::ostream & operator<<(std::ostream & os, const std::vector<T> & array) {
    os << "(";
    for(std::size_t i{0}; i < array.size()-1; ++i) {
        os << array[i] << ",";
    }
    return os << array[array.size()-1] << ")";
};

int main(int argc, char *argv[]) {

//    int process_ID, process_number;
//
//    MPI_Init(&argc,  &argv);
//    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
//    MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);
//    // Initialise the constants of the program.&ignored_request
//    constexpr const std::size_t DIMENSION{2};
//    constexpr const std::size_t NUMBER_OF_BOIDS{500};
//
//    std::list<Octree<DIMENSION>> l;
//
//    constexpr const std::size_t level{2};
//
//    for(std::size_t i{0}; i < (1<<(DIMENSION*level)); ++i) {
//        Coordinate<DIMENSION> anchor;
//        for(std::size_t j{0}; j < DIMENSION; ++j) anchor[j] = 0;
//        for(std::size_t j{0}; j < DIMENSION*level; ++j)
//            anchor[j%DIMENSION] |= ((i >> j) & 1) << (j/DIMENSION);
//        l.emplace_back(anchor, level);
//    }
//
//    std::cout << l.size() << std::endl;
//
//    std::vector<Octree<DIMENSION>> octrees(l.begin(), l.end());
//
//    std::cout << octrees << std::endl;
//    distributed_sort(octrees, process_number, process_ID);
//    std::cout << octrees << std::endl;


//    Linear_Octree<DIMENSION> LO(l);
//    std::cout << "[" << process_ID << "] I'm here!!!" << std::endl;
//    MPI_Barrier(MPI_COMM_WORLD);
//    distributed_sort(LO.m_octants, process_number, process_ID);
//
//    for(auto const &m_octant : LO.m_octants) {
//        std::cout << m_octant.m_anchor << "-------" << m_octant.m_depth << std::endl;
//    }

/*    std::cout << "Creating the grid... " << std::flush;
    Grid<std::uniform_real_distribution<float>, DIMENSION> grid(NUMBER_OF_BOIDS);
    std::cout << "Done!" << std::endl;

    GridVisualizer<std::uniform_real_distribution<float>, DIMENSION> visualizer(grid);
    visualizer.start();*/
	//MPI_Finalize();

    //std::cout << get_morton_index(1, 2, 3) << std::endl;

    return 0;

}
