#include <iostream>

#include "mpi.h"
#include "../scan.h"

int main ( int argc , char** argv )
{
	MPI_Init(&argc,  &argv);
    int process_number, process_ID;

    constexpr const std::size_t SIZE{100000000};
    constexpr const bool display{false};

    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);

    // Initialise the values on the processor.
    std::size_t * values = new std::size_t[SIZE];
    for(std::size_t i{0}; i < SIZE; ++i) {
        values[i] = SIZE*process_ID + i;
    }

    distributed_prefix(values, SIZE, process_number, process_ID);
    
    // Output the values on each processor.
    if(display) {
        std::cout << "Hi! I'm processor n°" << process_ID << " and I have the values {";
        for(std::size_t i{0}; i < SIZE-1; ++i) {
            std::cout << values[i] << ",";
        }
        std::cout << values[SIZE-1] << "}" << std::endl;
    }
    
	MPI_Finalize();
	return 0;
}
