#ifndef SWARMING_PROJECT_SORTED_RANGE_COUNT_DISTRIBUTED_H
#define SWARMING_PROJECT_SORTED_RANGE_COUNT_DISTRIBUTED_H

#include <functional>
#include <algorithm>

#include "mpi.h"
#include "definitions/constants.h"

#if SWARMING_DO_ALL_CHECKS == 1
#include <cassert>
#include "algorithms/is_sorted_distributed.h"
#endif

/**
 * Count the elements in @a distributed_container that are in [lhs, rhs].
 *
 * @tparam Container Type of the container.
 * @tparam StoredDataType Type of the elements stored in @a distributed_container.
 * @tparam Comp Comparator used. Should met the requirements of std::less<StoredDataType>.
 * @param distributed_container The *sorted* distributed container in which we will search for the elements.
 * @param lhs The lower bound (included) of the elements to count.
 * @param rhs The upper bound (included) of the elements to count.
 * @param comp The comparator used to sort the data.
 * @return The number of elements of @a distributed_container that are in [lhs, rhs].
 */
template <typename Container, typename StoredDataType = typename Container::value_type, typename Comp = std::less<StoredDataType>>
std::size_t sorted_range_count_distributed(Container const & distributed_container,
                                           StoredDataType const & lhs,
                                           StoredDataType const & rhs,
                                           Comp comp = Comp()) {

#if SWARMING_DO_ALL_CHECKS == 1
    assert(is_sorted_distributed(distributed_container, comp));
    // The provided values should also be sorted, i.e. lhs < rhs.
    assert(comp(lhs, rhs));
#endif

    // Compute first the local number of value_to_search.
    auto const lower_bound = std::lower_bound(distributed_container.begin(), distributed_container.end(), lhs, comp);
    auto const upper_bound = std::upper_bound(distributed_container.begin(), distributed_container.end(), rhs, comp);

    std::size_t const local_number_of_elements{static_cast<unsigned long>(std::distance(lower_bound, upper_bound)) };

    // Then do a distributed sum on this local number.
    std::size_t distributed_number_of_elements;
    MPI_Allreduce(&local_number_of_elements, &distributed_number_of_elements, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);

    // And all the processes return this number.
    return distributed_number_of_elements;
};


#endif //SWARMING_PROJECT_SORTED_RANGE_COUNT_DISTRIBUTED_H
