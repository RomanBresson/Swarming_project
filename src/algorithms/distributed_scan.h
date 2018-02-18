#ifndef SWARMING_PROJECT_SCAN_H
#define SWARMING_PROJECT_SCAN_H

#include <vector>
#include <functional>
#include <type_traits>
#include <algorithm>
#include "mpi.h"
#include "omp.h"

#include "definitions/constants.h"

#if SWARMING_DO_ALL_CHECKS == 1
#include <cassert>
#endif

template <typename Container, typename StoredDataType = typename Container::value_type, typename IntTypeOut = unsigned long long>
std::vector<IntTypeOut> local_scan(Container const & container,
                                   std::function<IntTypeOut(StoredDataType const &)> weight) {
    std::vector<IntTypeOut> result(container.size());
    auto container_it = container.begin();

    // Start with the first element because this is a special case.
    if(container.size() > 0) {
        result[0] = weight(*container_it);
        ++container_it;
    }
    // And then do a sequential scan on the local data.
    for(std::size_t i{1}; i < container.size(); ++i) {
        result[i] = result[i-1] + weight(*container_it);
        ++container_it;
    }

    return result;
};

template <typename Container, typename StoredDataType = typename Container::value_type, typename IntTypeOut = unsigned long long>
std::vector<IntTypeOut> distributed_scan(Container const & container,
                                         std::function<IntTypeOut(StoredDataType const &)> weight) {

    // Because of the difficulty to adapt the MPI_[type] at compile-time/runtime.
    static_assert(std::is_same<IntTypeOut, unsigned long long>::value, "Template parameter IntTypeOut should be unsigned "
            "long long. Other types are not currently implemented.");

    int process_ID, process_number;
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);

    // First we perform a local scan on each processor.
    std::vector<IntTypeOut> result = local_scan(container, weight);

    // Then we do a distributed scan on the last local element of each processor.
    IntTypeOut prefix;

    MPI_Scan(&result.back(), &prefix, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);

    // And now each processor has the distributed scan result for its last element.
    // It will send this result to the next processor, receive the result from the previous processor,
    // and update its values according to the received result.
    MPI_Request request;
    if(process_ID < process_number-1)
        MPI_Isend(&prefix, 1, MPI_UNSIGNED_LONG_LONG, process_ID+1, /*tag*/ 0, MPI_COMM_WORLD, &request);
    if(process_ID > 0) {
        IntTypeOut previous_prefix;
        MPI_Recv(&previous_prefix, 1, MPI_UNSIGNED_LONG_LONG, process_ID - 1, /*tag*/ 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        std::transform(result.begin(), result.end(), result.begin(),
                       [previous_prefix](IntTypeOut integer) { return previous_prefix + integer; });
    }

    // We should wait for the first process to ensure it does not exit this scope before the sending operation is done.
    if(process_ID == 0)
        MPI_Wait(&request, MPI_STATUS_IGNORE);

#if SWARMING_DO_ALL_CHECKS
    assert(result.back() == prefix);
#endif

    return result;
};

#endif //SWARMING_PROJECT_SCAN_H
