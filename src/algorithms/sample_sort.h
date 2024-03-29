#ifndef SWARMING_PROJECT_SAMPLE_SORT_H
#define SWARMING_PROJECT_SAMPLE_SORT_H

#include <random>
#include <algorithm>
#include <cmath>
#include <vector>
#include <type_traits>
#include <utility>
#include <tuple>
#include <queue>
#include <functional>
#include <numeric>
#include <iterator>
#include <list>

#include "mpi.h"

#include "definitions/constants.h"
#include "algorithms/merge_sorted_arrays.h"

#if SWARMING_SORT_USE_TIMER == 1

#include <iostream>
#include <chrono>
#include <string>
class Timer {
public:
    explicit Timer(int processor_ID) : m_processor_ID{processor_ID}
    { }

    void tic(std::string name) {
        m_last_tic_name = std::move(name);
        m_last_tic = std::chrono::high_resolution_clock::now();
    }

    void toc() const {
        const auto end_time = std::chrono::high_resolution_clock::now();
        std::cout << "(" << m_processor_ID << ") " << m_last_tic_name << ": "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - m_last_tic).count()
                  << " ms" << std::endl;
    }

private:
    int m_processor_ID;
    std::chrono::high_resolution_clock::time_point m_last_tic;
    std::string m_last_tic_name;
};
#define SWARMING_SORT_CONSTRUCT_TIMER(PROCESS_ID) Timer timer(PROCESS_ID);
#define SWARMING_SORT_TIMER_TIC(TIC_STRING)       timer.tic(TIC_STRING);
#define SWARMING_SORT_TIMER_TOC                   timer.toc();

#else
#define SWARMING_SORT_CONSTRUCT_TIMER(PROCESS_ID)
#define SWARMING_SORT_TIMER_TIC(TIC_STRING)
#define SWARMING_SORT_TIMER_TOC
#endif


template <typename T>
static std::vector<T> select_evenly_spaced(std::vector<T> const & elements, std::size_t number_of_elements) {
    // Select number_of_elements in the vector elements.
    // The selected elements are "evenly spaced" in elements. By "evenly spaced" we mean that there exist an integer
    // N such that for a selected element elements[i], elements[i+N] or elements[i+N+1] is selected (depending on
    // integer rounding).

    // First create the array of the selected elements and reserve enough space to avoid reallocation.
    std::vector<T> selected_elements;
    selected_elements.reserve(number_of_elements);
    // Then compute the space between 2 elements.
    const double space_between_picked_elements{static_cast<double>(elements.size()) / static_cast<double>(number_of_elements+1)};
    // Select the elements by computing an offset depending on the number of elements already selected and the
    // space between 2 elements, and then rounding the result.
    for(std::size_t i{1}; i <= number_of_elements; ++i)
        selected_elements.push_back(elements[ static_cast<std::size_t>(std::floor(i*space_between_picked_elements)) ]);

    return selected_elements;
}

template <typename T, typename Comp>
static std::vector<T> select_splitters(std::vector<T> & array, int process_ID, int process_number, Comp comp) {

    SWARMING_SORT_CONSTRUCT_TIMER(process_ID)
    // Each process sort sequentially its array.
    SWARMING_SORT_TIMER_TIC("sequential sort")
    std::sort(array.begin(), array.end(), comp);
    SWARMING_SORT_TIMER_TOC

    // Then choose process_number-1 evenly-spaced elements and send them to the first process
    std::vector<T> elements_to_send = select_evenly_spaced(array, process_number-1);
    if(process_ID > 0)
        MPI_Send(elements_to_send.data(), elements_to_send.size() * sizeof(T), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    // First we create the data structure that will store the splitters
    std::vector<T> selected_splitters(process_number-1);

    // Then if we are the first process, receive all the data and fill the splitter data structure
    if(process_ID == 0) {
        SWARMING_SORT_TIMER_TIC("evenly-spaced elements")
        // Create the vector that will store the splitters selected by all the processes and
        // reserve enough space, even if here reallocation is cheap.
        std::vector< std::vector<T> > received_data;
        received_data.reserve(process_number);
        // Move the splitters selected by the current process (ID 0).
        received_data.emplace_back(std::move(elements_to_send));
        // Then for each process, append to the list of vectors of splitters a vector
        // of process_number-1 already allocated elements and receive the data from
        // other processes
        for(std::size_t p{1}; p < process_number; ++p) {
            received_data.emplace_back(process_number-1);
            MPI_Recv(received_data.back().data(), (process_number-1) * sizeof(T), MPI_BYTE, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        // The selected splitters are SORTED, so we can merge them efficiently
        const std::vector<T> sorted_all_splitters = merge_sorted_arrays_sequential(received_data, comp);
        // And finally we select our final splitters
        selected_splitters = select_evenly_spaced(sorted_all_splitters, process_number-1);
        SWARMING_SORT_TIMER_TOC
    }
    // The work of the process n°0 was to fill the splitters, now we can broadcast.
    SWARMING_SORT_TIMER_TIC("broadcast")
    MPI_Bcast(selected_splitters.data(), selected_splitters.size() * sizeof(T), MPI_BYTE, /*root*/ 0, MPI_COMM_WORLD);
    SWARMING_SORT_TIMER_TOC

    return selected_splitters;
}

template <typename T, typename Comp = std::less<T>>
static void sample_sort_inplace(std::vector<T> & array, Comp comp = Comp()) {

    int process_ID, process_number;

    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);

    if(process_number < 2) {
        std::cerr << "The distributed_sort procedure should only be called with 2 or more processors." << std::endl
                  << "For sequential computations, consider std::sort." << std::endl;
        return;
    }


    SWARMING_SORT_CONSTRUCT_TIMER(process_ID)

    // Select the splitters.
    std::vector<T> selected_splitters = select_splitters(array, process_ID, process_number, comp);

    // For the following code, it is easier to add a last splitter that will be +infinity (or a value bigger than every
    // other value possible in the array
    selected_splitters.push_back(std::numeric_limits<T>::max());

    // And now each process sends its data to the process that should manage them.
    std::vector<std::size_t> index_delimitation{0};            // Stores the indexes where we splitted the buckets.
    std::vector<std::size_t> sizes(selected_splitters.size()); // Stored the number of elements of each bucket.
    // Important because we don't want a reallocation (asynchronous communications).
    MPI_Request              ignored_request;                  // We perform synchronous MPI_Recv so we don't need to wait for the asynchronous request.
    std::size_t              bucket_index{0};                  // Index of the current bucket (i.e. index of the processor that should handle it).
    SWARMING_SORT_TIMER_TIC("sending buckets")
    for(std::size_t i{0}; i < array.size(); ++i) {
        // If we enter in a new bucket, than send the data and update the bucket index
        if(array[i] > selected_splitters[bucket_index]) {
            sizes.emplace_back(i - index_delimitation.back());
            MPI_Isend(&(sizes.back()), 1, MPI_UNSIGNED_LONG_LONG, bucket_index, /*tag*/ 0, MPI_COMM_WORLD, &ignored_request);
            MPI_Isend(array.data() + index_delimitation.back(), sizes.back() * sizeof(T), MPI_BYTE, bucket_index, /*tag*/ 1, MPI_COMM_WORLD, &ignored_request);
            index_delimitation.emplace_back(i);
            ++bucket_index;
        }
    }
    // The last bucket has not been sent in the loop, so we need to send it now.
    const std::size_t buffer_size{array.size() - index_delimitation.back()};
    MPI_Isend(&buffer_size, 1, MPI_UNSIGNED_LONG_LONG, bucket_index, /*tag*/ 0, MPI_COMM_WORLD, &ignored_request);
    MPI_Isend(array.data() + index_delimitation.back(), buffer_size * sizeof(T), MPI_BYTE, bucket_index, /*tag*/ 1, MPI_COMM_WORLD, &ignored_request);
    SWARMING_SORT_TIMER_TOC


    // Each process should receive its data from all the other process.
    SWARMING_SORT_TIMER_TIC("receiving buckets")
    std::vector< std::vector<T> > final_data;
    for(std::size_t p{0}; p < process_number; ++p) {
        std::size_t size_to_receive;
        MPI_Recv(&size_to_receive, 1, MPI_UNSIGNED_LONG_LONG, p, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        final_data.emplace_back(size_to_receive);
        MPI_Recv(final_data.back().data(), size_to_receive * sizeof(T), MPI_BYTE, p, /*tag*/ 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    SWARMING_SORT_TIMER_TOC

    //MPI_Barrier(MPI_COMM_WORLD);
    // Finally, the received data is sorted so we can merge it efficiently.
    SWARMING_SORT_TIMER_TIC("merging received data")
    array = merge_sorted_arrays_sequential(final_data, comp);
    SWARMING_SORT_TIMER_TOC
}

#undef SWARMING_SORT_CONSTRUCT_TIMER
#undef SWARMING_SORT_TIMER_TIC
#undef SWARMING_SORT_TIMER_TOC

#endif //SWARMING_PROJECT_SAMPLE_SORT_H
