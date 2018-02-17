#include <iostream>
#include <random>

#include "mpi.h"
#include "../sample_sort.h"


template <typename T>
static std::ostream & operator<<(std::ostream& os, std::vector<T> const & vec) {
    os << vec.size() << "{";
    for(std::size_t i{0}; i < vec.size()-1; ++i) {
        os << vec[i] << ",";
    }
    os << vec.back() << "}" << std::endl;
    return os;
}

int main ( int argc , char** argv )
{

    if(argc < 2) {
        std::cerr << "You should provide the length of the arrays." << std::endl;
        return 1;
    }

	MPI_Init(&argc,  &argv);

    int process_number, process_ID;
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);

    const std::size_t SIZE{std::strtoull(argv[1], nullptr, 10)};
    constexpr const std::size_t MAX_INT{100};
    constexpr const bool display{true};

    // Initialise the values on the processor.
    std::vector<int> values;
    std::random_device d;
    std::default_random_engine generator(d());
    std::uniform_int_distribution<> distribution(0, MAX_INT);
    values.reserve(SIZE);
    for(std::size_t i{0}; i < SIZE; ++i) {
        values.push_back(distribution(generator));
    }

    distributed_sort(values, process_number, process_ID);

    // Output the values 
    // Here, the first process gather all the values and output them.
    if(display) {
        MPI_Barrier(MPI_COMM_WORLD);
        if(process_ID == 0) {
            std::cout << "Hi! The processor n°" << process_ID << " have " << values.size() << " values in [" << values.front() << "," << values.back() << "]." << std::endl;
            for(std::size_t p{1}; p < process_number; ++p) {
                std::size_t size_to_receive;
                MPI_Recv(&size_to_receive, 1, MPI_UNSIGNED_LONG_LONG, p, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // Resize the vector that will receive the data.
                values.resize(size_to_receive);
                // Then receive the values
                MPI_Recv(values.data(), size_to_receive, MPI_INT, p, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                std::cout << "Hi! The processor n°" << p << " have " << values.size() << " values in [" << values.front() << "," << values.back() << "]." << std::endl;
            }
        }
        else {
            const std::size_t size_to_send{values.size()};
            MPI_Send(&size_to_send, 1, MPI_UNSIGNED_LONG_LONG, 0, /*tag*/ 0, MPI_COMM_WORLD);
            MPI_Send(values.data(), size_to_send, MPI_INT, 0, /*tag*/ 0, MPI_COMM_WORLD);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    
	MPI_Finalize();

	return 0;
}
