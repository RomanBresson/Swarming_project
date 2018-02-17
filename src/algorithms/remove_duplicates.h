#ifndef SWARMING_PROJECT_REMOVE_DUPLICATES_H
#define SWARMING_PROJECT_REMOVE_DUPLICATES_H

#include <iterator>
#include <functional>
#include "definitions/constants.h"
#include "mpi.h"

#if SWARMING_DO_ALL_CHECKS == 1
#include <algorithm>
#include <cassert>
#endif

/**
 * In this file we implement the algorithm used to remove duplicates from a sorted list
 * of octants.
 */

template <typename Container, typename StoredDataType = typename Container::value_type>
Container remove_duplicates(Container const & container,
                            std::function<bool(StoredDataType const &, StoredDataType const &)> is_duplicate = std::equal_to<StoredDataType const &>()) {

#if SWARMING_DO_ALL_CHECKS == 1
    assert(std::is_sorted(container.begin(), container.end()));
#endif

    Container container_without_duplicates;
    auto end        = container.end();
    auto before_end = std::prev(end);

    // First remove all the duplicates in the local data.
    for(auto octant_it = container.begin();
        octant_it != before_end && octant_it != end;
        ++octant_it) {

        // First push this octant inside the no-duplicate structure
        container_without_duplicates.push_back(*octant_it);
        // Then check if the following octant is a duplicate and if it is, then
        // avoid its processing by moving the iterator one step forward.
        auto next_it = std::next(octant_it);
        while(is_duplicate(*octant_it, *next_it)) {
            ++octant_it;
            ++next_it;
        }
    }

    // Then we may need to remove the last element because the next processor already store it.
    int process_ID, process_number;
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);

    MPI_Request request;
    StoredDataType next_element;

    if(process_ID != 0)
        // Send the first element to the previous process.
        MPI_Isend(container_without_duplicates.data(), sizeof(StoredDataType), MPI_BYTE, process_ID-1, /*tag*/ 0, MPI_COMM_WORLD, &request);
    if(process_ID != process_number-1)
        // Receive the first element of the next process.
        MPI_Recv(&next_element, sizeof(StoredDataType), MPI_BYTE, process_ID+1, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // And check if it is considered as a duplicate.
        if(is_duplicate(container_without_duplicates.back(), next_element))
            container_without_duplicates.pop_back();

    // We need to wait here because we don't want a process to exit before its sending operation was complete.
    MPI_Barrier(MPI_COMM_WORLD);

    return container_without_duplicates;
};

#endif //SWARMING_PROJECT_REMOVE_DUPLICATES_H
