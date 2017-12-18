#include "omp.h"
#include "mpi.h"
#include <iostream>

template <typename T>
void local_prefix(T* data, std::size_t size) {
    for(std::size_t i{1}; i < size; ++i)
        data[i] += data[i-1];
}

template <typename T>
void distributed_prefix(T* array, std::size_t length, int process_number, int process_ID) {

    T prefix_local{T()}, prefix_previous{T()};

    // Each processor is making a local prefix
    local_prefix(array, length);
    // We compute the distributed prefix on the last element of each vector
    MPI_Scan(array+length-1, &prefix_local, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    // and send the values to the next processor
    if(process_ID < process_number-1)
        MPI_Send(&prefix_local, 1, MPI_DOUBLE, process_ID+1, 0, MPI_COMM_WORLD);
    // and retrieve the value sent by the previous processor
    if(process_ID > 0)
        MPI_Recv(&prefix_previous, 1, MPI_DOUBLE, process_ID-1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // Finally we are adding to all the local array the prefix of the previous processor
#pragma omp parallel for
    for(std::size_t i = 0; i < length; ++i)
        array[i] += prefix_previous;
}

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
