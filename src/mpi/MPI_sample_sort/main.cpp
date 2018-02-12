#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include <vector>
#include <type_traits>
#include <utility>
#include <tuple>
#include <queue>
#include <functional>
#include <chrono>
#include <string>
#include <numeric>
#include <iterator>
#include <list>

#include "mpi.h"

class Timer {

public:

    Timer(int processor_ID) : m_processor_ID{processor_ID}
    { }
    
    void tic(std::string name) {
        m_last_tic_name = name;
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


template <typename T>
static std::ostream & operator<<(std::ostream& os, std::vector<T> const & vec) {
    os << vec.size() << "{";
    for(std::size_t i{0}; i < vec.size()-1; ++i) {
        os << vec[i] << ",";
    }
    os << vec.back() << "}" << std::endl;
    return os;
}

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
static std::vector<T> merge_sorted_arrays_sequential(std::vector< std::vector<T> > & arrays, Comp comp) {
    std::vector<T> result;

    if(arrays.size() < 2) {
        std::cerr << "merge_sorted_arrays_sequential should be called with 2 or more arrays to merge." << std::endl;
        return result;
    }

    // Sort the vector of values by increasing lengths for a better efficiency.
    // We can try to remove the 2 following lines after and analyse the performance.
    auto comparator = [] (std::vector<T> const & lhs, std::vector<T> const & rhs) { return lhs.size() < rhs.size(); };
    std::sort(arrays.begin(), arrays.end(), comparator);

    // Computing the total number of elements in order to save some re-allocations
    auto add_size = [] (std::size_t const & previous_size, std::vector<T> const & elem) { return previous_size + elem.size(); };
    std::size_t const number_of_elements{ std::accumulate(arrays.begin(), arrays.end(), static_cast<std::size_t>(0), add_size) };
    
    // Resize the resulting vector to be sure that all the values can fit in it.
    result.resize(number_of_elements);

    // First step, use the std::merge to combine the merge and the copy of the data.
    // We also keep track of the iterator that separate 2 non-sorted lists in separators.
    // separators stores iterators pointing to the beginning of a non-sorted sequence.
    std::list< typename std::vector<T>::iterator > separators{result.begin()};
    std::size_t next_free_position{0};
    for(std::size_t i{0}; i < arrays.size()/2; ++i) {
        separators.emplace_back( std::merge(arrays[2*i].begin()  , arrays[2*i].end(),   /*input  1*/
                                            arrays[2*i+1].begin(), arrays[2*i+1].end(), /*input  2*/
                                            result.begin() + next_free_position,        /*output 1*/
											comp) );
        next_free_position += arrays[2*i].size() + arrays[2*i+1].size();
    }

    if(arrays.size()%2)
        separators.emplace_back( std::copy(arrays.back().begin(), arrays.back().end(), separators.back()) );

    // Now result contain all the values, we just need to merge all the lists not merged in the previous step.
    // In the separators list, we also have the begin() and end() iterators, that is why our stop condition is
    // that the separators list should contain only 2 elements: the begin() and the end()
    while(separators.size() != 2) {
        // We will iterate over each separators, and each time merge two blocks
        // by merging first the smallest blocks.
        auto left_iterator   = separators.begin();
        auto middle_iterator = std::next(left_iterator);
        while(*middle_iterator != result.end() && separators.size() != 2) {
            auto right_iterator = std::next(middle_iterator);
            std::inplace_merge(*left_iterator, *middle_iterator, *right_iterator, comp);
            separators.erase(middle_iterator);
            left_iterator   = right_iterator;
            middle_iterator = std::next(right_iterator);
        }
    }

    // Return the result
    return result;
}

template <typename T, typename Comp>
static std::vector<T> select_splitters(std::vector<T> & array, int process_ID, int process_number, Comp comp) {
    Timer timer(process_ID);
    // Each process sort sequentially its array.
    timer.tic("sequential sort");
    std::sort(array.begin(), array.end(), comp);
    timer.toc();

    // Then choose process_number-1 evenly-spaced elements and send them to the first process
    std::vector<T> elements_to_send = select_evenly_spaced(array, process_number-1);
    if(process_ID > 0)
	    MPI_Send(elements_to_send.data(), elements_to_send.size() * sizeof(T), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    // First we create the data structure that will store the splitters
    std::vector<T> selected_splitters(process_number-1);

    // Then if we are the first process, receive all the data and fill the splitter data structure
    if(process_ID == 0) {
        timer.tic("evenly-spaced elements");
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
        timer.toc();
    }
    // The work of the process n°0 was to fill the splitters, now we can broadcast.
    timer.tic("broadcast");
    MPI_Bcast(selected_splitters.data(), selected_splitters.size() * sizeof(T), MPI_BYTE, /*root*/ 0, MPI_COMM_WORLD);
    timer.toc();
    
    return selected_splitters;
}

template <typename T, typename Comp = std::less<T>>
static void distributed_sort(std::vector<T> & array, int process_number, int process_ID, Comp comp = Comp()) {

    if(process_number < 2) {
        std::cerr << "The distributed_sort procedure should only be called with 2 or more processors." << std::endl
                  << "For sequential computations, consider std::sort." << std::endl;
        return;
    }
    
    Timer timer(process_ID);

    // Select the splitters.
    std::vector<T> selected_splitters = select_splitters(array, process_ID, process_number);
    
    // For the following code, it is easier to add a last splitter that will be +infinity (or a value bigger than every
    // other value possible in the array
    selected_splitters.push_back(std::numeric_limits<T>::max());

    // And now each process sends its data to the process that should manage them.
    std::vector<std::size_t> index_delimitation{0};            // Stores the indexes where we splitted the buckets.
    std::vector<std::size_t> sizes(selected_splitters.size()); // Stored the number of elements of each bucket.
                                                               // Important becase we don't want a reallocation (asynchronous communications).
    MPI_Request              ignored_request;                  // We perform synchronous MPI_Recv so we don't need to wait for the asynchronous request.
    std::size_t              bucket_index{0};                  // Index of the current bucket (i.e. index of the processor that should handle it).
    timer.tic("sending buckets");
    for(std::size_t i{0}; i < array.size(); ++i) {
        // If we enter in a new bucket, than send the data and update the bucket index
        if(array[i] > selected_splitters[bucket_index]) {
            sizes.emplace_back(i - index_delimitation.back());
            MPI_Isend(&(sizes.back()), 1, MPI_INT, bucket_index, /*tag*/ 0, MPI_COMM_WORLD, &ignored_request);
            MPI_Isend(array.data() + index_delimitation.back(), sizes.back(), MPI_INT, bucket_index, /*tag*/ 1, MPI_COMM_WORLD, &ignored_request);
            index_delimitation.emplace_back(i);
            ++bucket_index;
        }
    }
    // The last bucket has not been sent in the loop, so we need to send it now.
    const std::size_t buffer_size{array.size() - index_delimitation.back()};
    MPI_Isend(&buffer_size, 1, MPI_INT, bucket_index, /*tag*/ 0, MPI_COMM_WORLD, &ignored_request);
    MPI_Isend(array.data() + index_delimitation.back(), buffer_size, MPI_INT, bucket_index, /*tag*/ 1, MPI_COMM_WORLD, &ignored_request);
    timer.toc();

    
    // Each process should receive its data from all the other process.
    timer.tic("receiving buckets");
    std::vector< std::vector<T> > final_data;
    for(std::size_t p{0}; p < process_number; ++p) {
        std::size_t size_to_receive;
        MPI_Recv(&size_to_receive, 1, MPI_INT, p, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        final_data.emplace_back(size_to_receive);
        MPI_Recv(final_data.back().data(), size_to_receive, MPI_INT, p, /*tag*/ 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    timer.toc();

    // Finally, the received data is sorted so we can merge it efficiently.
    timer.tic("merging received data");
    array = merge_sorted_arrays_sequential(final_data);
    timer.toc();
}

int main ( int argc , char** argv )
{

    if(argc < 2) {
        std::cerr << "You should provide the length of the arrays." << std::endl;
        return 1;
    }

	MPI_Init(&argc,  &argv);

    int process_number, process_ID;
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);

    const std::size_t SIZE{std::strtoull(argv[1], nullptr, 10)};
    constexpr const std::size_t MAX_INT{100};
    constexpr const bool display{true};

    // Initialise the values on the processor.
    std::vector<int> values;
    std::random_device d;
    std::default_random_engine generator(d());
    std::uniform_int_distribution<> distribution(0, MAX_INT);
    values.reserve(SIZE);
    for(std::size_t i{0}; i < SIZE; ++i) {
        values.push_back(distribution(generator));
    }

    Timer timer(process_ID);
    timer.tic("TOTAL TIME");
    distributed_sort(values, process_number, process_ID);
    timer.toc();

    // Output the values 
    // Here, the first process gather all the values and output them.
    if(display) {
        MPI_Barrier(MPI_COMM_WORLD);
        if(process_ID == 0) {
            std::cout << "Hi! The processor n°" << process_ID << " have " << values.size() << " values in [" << values.front() << "," << values.back() << "]." << std::endl;
            for(std::size_t p{1}; p < process_number; ++p) {
                std::size_t size_to_receive;
                MPI_Recv(&size_to_receive, 1, MPI_UNSIGNED_LONG_LONG, p, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // Resize the vector that will receive the data.
                values.resize(size_to_receive);
                // Then receive the values
                MPI_Recv(values.data(), size_to_receive, MPI_INT, p, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                std::cout << "Hi! The processor n°" << p << " have " << values.size() << " values in [" << values.front() << "," << values.back() << "]." << std::endl;
            }
        }
        else {
            const std::size_t size_to_send{values.size()};
            MPI_Send(&size_to_send, 1, MPI_UNSIGNED_LONG_LONG, 0, /*tag*/ 0, MPI_COMM_WORLD);
            MPI_Send(values.data(), size_to_send, MPI_INT, 0, /*tag*/ 0, MPI_COMM_WORLD);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    
	MPI_Finalize();

	return 0;
}
