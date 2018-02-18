#ifndef SWARMING_PROJECT_IS_SORTED_DISTRIBUTED_H
#define SWARMING_PROJECT_IS_SORTED_DISTRIBUTED_H

#include <algorithm>

#include "mpi.h"

template <typename Container>
bool is_sorted_distributed(Container const & container) {

    using StoredDataType = typename Container::value_type;

    bool local_result{true}, global_result;

    // Check first the sorted property locally on each processor.
    if(! std::is_sorted(container.begin(), container.end()))
        local_result = false;
    // All the processors should agree, so if one processor is not sorted, then all the processors should return false.
    MPI_Allreduce(&local_result, &global_result, 1, MPI_CXX_BOOL, MPI_LAND, MPI_COMM_WORLD);
    if(!global_result) return false;


    // Then, we check that every array is globally sorted.
    int process_number, process_ID;
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);

    MPI_Request request;
    if (process_ID != process_number - 1)
        MPI_Isend(&container.back(), sizeof(StoredDataType), MPI_BYTE, process_ID + 1, /*tag*/ 0,
                  MPI_COMM_WORLD, &request);
    StoredDataType tmp;
    if (process_ID != 0)
        MPI_Recv(&tmp, sizeof(StoredDataType), MPI_BYTE, process_ID - 1, /*tag*/, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

    if (tmp > container.front())
        local_result = false;

    // Same as before, all the processors should agree:
    MPI_Allreduce(&local_result, &global_result, 1, MPI_CXX_BOOL, MPI_LAND, MPI_COMM_WORLD);
    if(!global_result) return false;

    // Wait for the processes because we don't want this "debug" section to interact with the algorithm at all.
    MPI_Barrier(MPI_COMM_WORLD);

    // Finally if everything is sorted, return true
    return true;
}

#endif //SWARMING_PROJECT_IS_SORTED_DISTRIBUTED_H
