#include <iostream>
#include "data_structures/Octree.h"
#include "data_structures/Linear_Octree.h"
#include "data_structures/Boid.h"
#include "data_structures/MathArray.h"
#include "data_structures/Grid.h"
#include "visualization/GridVisualizer.h"
#include "mpi.h"
#include "mpi/MPI_sample_sort/sample_sort.h"

using types::Position;

int main(int argc, char *argv[]) {

    int process_ID, process_number;

    MPI_Init(&argc,  &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);
    // Initialise the constants of the program.&ignored_request
    constexpr const std::size_t DIMENSION{2};
    constexpr const std::size_t NUMBER_OF_BOIDS{500};

    std::list<Octree<DIMENSION>> l;

    for(std::size_t i{0}; i < 1000; ++i) {
        Coordinate<DIMENSION> anchor;
        for(std::size_t d{0}; d < DIMENSION; ++d)
            anchor[d] = static_cast<unsigned long>(((process_ID + 53 * d) * (958 + process_ID*57) / (d+567)) % (477*(d+7)));
        l.emplace_back(anchor, (process_ID + 87) * (process_ID + 54) / (process_ID+3) % (process_ID + 2) + 1);
    }

    Linear_Octree<DIMENSION> LO(l);
    MPI_Barrier(MPI_COMM_WORLD);
    distributed_sort(LO.m_octants, process_number, process_ID);

    for(auto const &m_octant : LO.m_octants) {
        std::cout << m_octant.m_anchor << "-------" << m_octant.m_depth << std::endl;
    }

/*    std::cout << "Creating the grid... " << std::flush;
    Grid<std::uniform_real_distribution<float>, DIMENSION> grid(NUMBER_OF_BOIDS);
    std::cout << "Done!" << std::endl;

    GridVisualizer<std::uniform_real_distribution<float>, DIMENSION> visualizer(grid);
    visualizer.start();*/
	MPI_Finalize();
    return 0;

}
