#ifndef SWARMING_PROJECT_SORTED_COUNT_DISTRIBUTED_H
#define SWARMING_PROJECT_SORTED_COUNT_DISTRIBUTED_H

#include <functional>
#include <algorithm>

#include "mpi.h"


template <typename Container, typename StoredDataType = typename Container::value_type, typename Comp = std::less<StoredDataType>>
std::size_t sorted_count_distributed(Container const & distributed_container,
                                     StoredDataType const & value_to_search,
                                     Comp comp = Comp()) {

    // Compute first the local number of value_to_search.
    auto const bounds = std::equal_range(distributed_container.begin(), distributed_container.end(), value_to_search, comp);
    std::size_t const local_number_of_elements{static_cast<unsigned long>(std::distance(bounds.first, bounds.second)) };

    // Then do a distributed sum on this local number.
    std::size_t distributed_number_of_elements;
    MPI_Allreduce(&local_number_of_elements, &distributed_number_of_elements, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);

    // And all the processes return this number.
    return distributed_number_of_elements;
};

#endif //SWARMING_PROJECT_SORTED_COUNT_DISTRIBUTED_H
