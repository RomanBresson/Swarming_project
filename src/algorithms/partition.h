#ifndef SWARMING_PROJECT_PARTITION_H
#define SWARMING_PROJECT_PARTITION_H

#include <functional>
#include <algorithm>

#include "mpi.h"
#include "mpi/scan.h"

#include "definitions/constants.h"
#include "algorithms/merge_sorted_arrays.h"

#if SWARMING_DO_ALL_CHECKS == 1
#include <cassert>
#endif

template <typename Container, typename StoredDataType = typename Container::value_type, typename UIntType = unsigned long long>
void partition(Container & container, std::function<UIntType(StoredDataType const &)> weight) {

#if SWARMING_DO_ALL_CHECKS == 1
    assert(std::is_sorted(container.begin(), container.end()));
#endif

    int process_ID, process_number;
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);

    // Compute the distributed list of weights.
    auto const S = distributed_scan(container, weight);

    // The last process broadcast the total number of octants to all the other process.
    // Here we don't need to filter the process that will set the total_weight variable because the broadcast just
    // after will override the incorrect values on all the process except the last one (which has the only correct value).
    unsigned long long total_weight{S.back()};
    MPI_Bcast(&total_weight, 1, MPI_UNSIGNED_LONG_LONG, process_number-1, MPI_COMM_WORLD);

    // Compute useful constants that will help us to equally split the data across the processors.
    float const mean_weight{static_cast<float>(total_weight) / process_number};
    unsigned long long const k{total_weight % process_number};

    std::vector<StoredDataType> data_to_send;
    std::vector<std::size_t> sizes;
    data_to_send.reserve(container.size());
    sizes.reserve(static_cast<std::size_t>(process_number));

    MPI_Request ignored_request;

    for(int p{0}; p < process_number; ++p) {
        auto container_it = container.begin();
        auto const size_before = data_to_send.size();

        for(std::size_t element_index{0}; element_index < container.size(); ++element_index, ++container_it) {
            // If we should send this element to process p
            if( (p < k &&
                 p*(mean_weight+1) <= S[element_index] &&
                 S[element_index] < (p+1)*(mean_weight+1))
                ||
                (p >= k &&
                 p*mean_weight + k <= S[element_index] &&
                 S[element_index] < (p+1)*mean_weight + k) ) {

                // We store this element for sending it after
                data_to_send.push_back(*container_it);
            }
        }
        if(p == 0) sizes[p] = data_to_send.size();
        else       sizes[p] = data_to_send.size() - sizes[p-1];
        // Asynchronously send the data. The data (and the sizes) will not move in memory because:
        //   1. We reserved enough space for the vectors, so it won't be reallocated.
        //   2. The vectors were declared in the main scope, so they won't be destroyed until the end of the function.
        MPI_Isend(&sizes[p], 1, MPI_UNSIGNED_LONG_LONG, p, /*tag*/0, MPI_COMM_WORLD, &ignored_request);
        MPI_Isend(data_to_send.data() + size_before, sizes[p] * sizeof(StoredDataType),
                  MPI_BYTE, p, /*tag*/ 1, MPI_COMM_WORLD, &ignored_request);
    }

    // Now we receive the data from all the process
    std::vector< std::vector<StoredDataType> > received_data;
    for(int p{0}; p < process_number; ++p) {
        // First ask for the number of elements we will receive
        std::size_t number_of_elements;
        MPI_Recv(&number_of_elements, 1, MPI_UNSIGNED_LONG_LONG, p, /*tag*/0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Prepare the memory place where we will receive the data
        received_data.emplace_back(number_of_elements);
        MPI_Recv(received_data.back().data(), number_of_elements * sizeof(StoredDataType),
                 MPI_BYTE, p, /*tag*/ 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    container = merge_sorted_arrays_sequential<StoredDataType, std::less<StoredDataType>, Container>(received_data);
};

#endif //SWARMING_PROJECT_PARTITION_H