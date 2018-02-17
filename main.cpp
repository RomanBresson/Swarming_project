#include <iostream>
#include <vector>
#include <list>
#include <functional>

#include "mpi.h"
#include "algorithms/partition.h"


template <typename T>
std::ostream & operator<<(std::ostream & os, const std::vector<T> & array) {
    os << "(";
    for(std::size_t i{0}; i < array.size()-1; ++i) {
        os << array[i] << ",";
    }
    return os << array[array.size()-1] << ")";
};

int main(int argc, char *argv[]) {

    int process_ID, process_number;

    MPI_Init(&argc,  &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);

    std::list<int> l{1,2,3,4,4};
    if(process_ID == 0) {
        std::list<int> tmp{5,5,5,6,6,7,9,10,11,11,11,11,11,11,11,11,11,11,11};
        l.insert(l.end(), tmp.begin(), tmp.end());
    }

    std::function<unsigned long long(int const &)> weight = [](int const & elt){ return 1; };

    partition(l, weight);
    std::vector<int> v(l.begin(), l.end());

    std::cout << "[" << process_ID << "] " << v << std::endl;

    MPI_Finalize();
    return 0;

}
