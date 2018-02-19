#ifndef SWARMING_PROJECT_COMPLETE_OCTREE_H
#define SWARMING_PROJECT_COMPLETE_OCTREE_H

#include <list>
#include <functional>

#include "mpi.h"
#include "data_structures/Octree.h"
#include "data_structures/Linear_Octree.h"
#include "algorithms/remove_duplicates.h"
#include "algorithms/linearise.h"
#include "algorithms/partition.h"
#include "definitions/constants.h"
#include "algorithms/is_sorted_distributed.h"

#if SWARMING_DO_ALL_CHECKS == 1
#include <cassert>
#endif

template <typename T>
std::ostream& operator<<(std::ostream & os, std::list<T> const & vec) {
    os << "(";
    auto const before_end = std::prev(vec.end());
    for(auto it = vec.begin(); it != before_end; ++it)
        os << *it << ",";
    return os << *before_end << ")";
}

template <std::size_t Dimension>
std::vector<Octree<Dimension>> complete_octree(std::list<Octree<Dimension>> partial_list)
{
    std::vector< Octree<Dimension> > completed_octree;

    int process_number, process_ID;
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);


#if SWARMING_DO_ALL_CHECKS == 1
    assert(is_sorted_distributed(partial_list));
#endif

    partial_list = remove_duplicates(partial_list);
    partial_list = linearise(partial_list);

    std::function<unsigned long long(Octree<Dimension> const &)> weight = [](Octree<Dimension> const & octree){ return 1ULL; };
    partition(partial_list, weight);

    Octree<Dimension> root;
    for(std::size_t d{0}; d < Dimension; ++d)
        root.m_anchor[d] = 0.0;
    root.m_depth = 0;

    // TODO: Problem here!
    if (process_ID == 0) {
        partial_list.push_front(root.get_dfd()
                                        .get_closest_ancestor(partial_list.front())
                                        .get_children().front());
    }
    else if (process_ID == process_number-1) {
        partial_list.push_back(partial_list.back()
                                       .get_closest_ancestor(root.get_dld())
                                       .get_children().back());
    }

    MPI_Request request;

    // Sending the first octree of the local list to the previous processor.
    if (process_ID > 0)
        MPI_Isend(&partial_list.front(), sizeof(Octree<Dimension>), MPI_BYTE, process_ID-1, 0, MPI_COMM_WORLD, &request);

    // Receiving the first octree of the next processor
    if (process_ID < process_number-1) {
        Octree<Dimension> recv;
        MPI_Recv(&recv, sizeof(Octree<Dimension>), MPI_BYTE, process_ID+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        partial_list.push_back(recv);
    }

    auto const before_end = std::prev(partial_list.end());
    for(auto it = partial_list.begin(); it != before_end; ++it) {
        auto completed_region = complete_region(*it, *std::next(it));
        completed_octree.push_back(*it);
        completed_octree.insert(completed_octree.end(), completed_region.begin(), completed_region.end());
    }

    if(process_ID == process_number-1)
        completed_octree.push_back(partial_list.back());

    // We need to ensure that all no process will exit the method before its sending operation is complete.
    MPI_Barrier(MPI_COMM_WORLD);

#if SWARMING_DO_ALL_CHECKS == 1
    assert(is_sorted_distributed(completed_octree));
#endif

    return completed_octree;
}


#endif //SWARMING_PROJECT_COMPLETE_OCTREE_H
