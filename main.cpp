#include <iostream>
#include <vector>
#include <list>
#include <functional>

#include "mpi.h"
#include "algorithms/sorted_range_count_distributed.h"

template <typename T>
std::ostream & operator<<(std::ostream & os, const std::vector<T> & array) {
    os << "(";
    for(std::size_t i{0}; i < array.size()-1; ++i) {
        os << array[i] << ",";
    }
    return os << array[array.size()-1] << ")";
};

template <typename T>
std::ostream & operator<<(std::ostream & os, const std::list<T> & array) {
    os << "(";
    auto const before_end = std::prev(array.end());
    auto it = array.begin();
    for(; it != before_end; ++it) {
        os << *it << ",";
    }
    return os << *(++it) << ")";
};


int main(int argc, char *argv[]) {

    int process_ID, process_number;

    MPI_Init(&argc,  &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);

    std::list<int> l{1,2,3,4,4,5};
    if(process_ID == 1) {
        std::list<int> tmp{5,5,5,6,6,7,9,10,11,11,11,11,11,11,11,11,11,11,11};
        l.clear();
        l.insert(l.end(), tmp.begin(), tmp.end());
    }

    std::cout << "[" << process_ID << "] Number of elements in [3,4]: " << sorted_range_count_distributed(l, 3, 4) << std::endl;
    std::cout << "[" << process_ID << "] Number of elements in [5,11]: " << sorted_range_count_distributed(l, 5, 11) << std::endl;


    MPI_Finalize();
    return 0;

}
