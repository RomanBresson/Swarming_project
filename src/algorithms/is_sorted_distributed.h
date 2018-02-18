#ifndef SWARMING_PROJECT_IS_SORTED_DISTRIBUTED_H
#define SWARMING_PROJECT_IS_SORTED_DISTRIBUTED_H

#include <algorithm>
#include <functional>

#include "mpi.h"

template <typename Container, typename StoredDataType = typename Container::value_type, typename Comp = std::less<StoredDataType>>
bool is_sorted_distributed(Container const & container, Comp comp = Comp()) {

    bool local_result, global_result;

    // Check first the sorted property locally on each processor.
    local_result = std::is_sorted(container.begin(), container.end(), comp);
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

    if (process_ID != 0) {
        StoredDataType tmp;
        MPI_Recv(&tmp, sizeof(StoredDataType), MPI_BYTE, process_ID - 1, /*tag*/ 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        if (comp(container.front(), tmp))
            local_result = false;
    }

    // Same as before, all the processors should agree:
    MPI_Allreduce(&local_result, &global_result, 1, MPI_CXX_BOOL, MPI_LAND, MPI_COMM_WORLD);
    return global_result;
}

#endif //SWARMING_PROJECT_IS_SORTED_DISTRIBUTED_H
