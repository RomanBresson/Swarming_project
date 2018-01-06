#include <iostream>

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
    Position<DIMENSION> bottom_left({0.0, 0.0});
    Position<DIMENSION> top_right({100.0, 100.0});

    std::cout << "Creating the grid... " << std::flush;
    Grid<std::uniform_real_distribution<float>, DIMENSION> grid(bottom_left, top_right, NUMBER_OF_BOIDS);
    std::cout << "Done!" << std::endl;

//    GridVisualizer<std::uniform_real_distribution<float>, DIMENSION> visualizer(grid);
//
//    visualizer.start();

    for (std::size_t i{0}; i < 1000; ++i) {
        std::cout << "Update n°" << i << "... " << std::flush;
        grid.update_all_boids();
        std::cout << "Done!" << std::endl;
    }

    MPI_Finalize();

    return 0;

}