#include <iostream>
#include "data_structures/Octree.h"
#include "mpi.h"
#include "data_structures/Boid.h"
#include "data_structures/Grid.h"
#include "visualization/GridVisualizer.h"

using types::Position;

int main(int argc, char *argv[]) {

    // Initialise MPI.
    MPI_Init(&argc, &argv);
    int process_number, process_ID;
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);

    // Initialise the constants of the program.
    constexpr const std::size_t DIMENSION{2};
    constexpr const std::size_t NUMBER_OF_BOIDS{500};


    /*
    Position<DIMENSION> pos1({10.0,10.0,10.0});
    Position<DIMENSION> pos2({20.0,10.0,20.0});
    Position<DIMENSION> pos3({51.0,12.5,12.6});
    Velocity<DIMENSION> vel({0.0,0.0,0.0});
    Force<DIMENSION> forc({0.0,0.0,0.0});

    Boid<std::uniform_real_distribution<float>, DIMENSION> b1(pos1, vel, forc);
    Boid<std::uniform_real_distribution<float>, DIMENSION> b2(pos2, vel, forc);
    Boid<std::uniform_real_distribution<float>, DIMENSION> b3(pos3, vel, forc);

    Octree<DIMENSION> o1(b1);
    Octree<DIMENSION> o2(b2);
    Octree<DIMENSION> o3(b3);

    cout << o1.m_anchor << " ; " << o2.m_anchor <<" ; "<<o3.m_anchor<<std::endl;*/

    std::cout << "Creating the grid... " << std::flush;
    Grid<std::uniform_real_distribution<float>, DIMENSION> grid(NUMBER_OF_BOIDS);
    std::cout << "Done!" << std::endl;

    GridVisualizer<std::uniform_real_distribution<float>, DIMENSION> visualizer(grid);

    visualizer.start();

    for (std::size_t i{0}; i < 1000; ++i) {
        std::cout << "Update n°" << i << "... " << std::flush;
        grid.update_all_boids();
        std::cout << "Done!" << std::endl;
    }

    MPI_Finalize();

    return 0;

}
