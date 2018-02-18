#ifndef SWARMING_PROJECT_SORTED_SEARCH_DISTRIBUTED_H
#define SWARMING_PROJECT_SORTED_SEARCH_DISTRIBUTED_H

#include <functional>
#include <algorithm>

#include "mpi.h"


template <typename Container, typename StoredDataType = typename Container::value_type, typename Comp = std::less<StoredDataType>>
bool sorted_search_distributed(Container const & distributed_container,
                               StoredDataType const & value_to_search,
                               Comp comp = Comp()) {

    bool local_found, global_found;

    // Check first the sorted property locally on each processor.
    local_found = std::binary_search(distributed_container.begin(), distributed_container.end(), value_to_search, comp);
    // All the processors should agree, so if one processor found the value, then all the processors should return true.
    MPI_Allreduce(&local_found, &global_found, 1, MPI_CXX_BOOL, MPI_LOR, MPI_COMM_WORLD);
    return global_found;
};

#endif //SWARMING_PROJECT_SORTED_SEARCH_DISTRIBUTED_H
