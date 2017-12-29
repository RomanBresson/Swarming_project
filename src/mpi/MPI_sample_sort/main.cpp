#include "mpi.h"
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
#include <cstdlib>
#include <thread>

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
    std::vector<T> selected_elements;
    selected_elements.reserve(number_of_elements);
    const double space_between_picked_elements{static_cast<double>(elements.size()) / static_cast<double>(number_of_elements+1)};
    for(std::size_t i{1}; i <= number_of_elements; ++i) {
        selected_elements.push_back(elements[ static_cast<std::size_t>(std::floor(i*space_between_picked_elements)) ]);
    }
    return selected_elements;
}

template <typename T>
static std::vector<T> merge_sorted_arrays_sequential(std::vector< std::vector<T> > const & arrays) {
    // Data structures initialisation.
    using QueueStoredType = std::pair<T, std::size_t>;
    std::priority_queue<QueueStoredType, std::vector<QueueStoredType>, std::greater<QueueStoredType>> prio_queue;
    std::vector<std::size_t> indexes(arrays.size(), 0);
    std::vector<T> result;
    std::size_t result_size{0};
    for(std::size_t i{0}; i < arrays.size(); ++i) {
        prio_queue.push( std::make_pair(arrays[i].front(), i) );
        result_size += arrays[i].size();
    }
    result.reserve(result_size);

    // Merge the arrays
    while(!prio_queue.empty()) {
        // Retrieve the lowest element
        std::size_t array_index;
        T value;
        std::tie( value, array_index ) = prio_queue.top();
        // Add the lowest element to the result array
        result.push_back(value);
        // And update our structures
        indexes[array_index]++;
        prio_queue.pop();
        if(indexes[array_index] < arrays[array_index].size())
            prio_queue.push( std::make_pair(arrays[array_index][indexes[array_index]], array_index) );
    }

    // Return the result
    return result;
}

template <typename T>
static void distributed_sort(std::vector<T> & array, int process_number, int process_ID) {

    if(process_number < 2) {
        std::cerr << "The distributed_sort procedure should only be called with 2 or more processors." << std::endl
                  << "For sequential computations, consider std::sort." << std::endl;
        return;
    }
    
    Timer timer(process_ID);
    // Each process sort sequentially its array.
    timer.tic("sequential sort");
    std::sort(array.begin(), array.end());
    timer.toc();

    // Then choose process_number-1 evenly-spaced elements and send them to the first process
    std::vector<T> elements_to_send = select_evenly_spaced(array, process_number-1);
    if(process_ID > 0)
        MPI_Send(elements_to_send.data(), elements_to_send.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
    // First we create the data structure that will store the splitters
    std::vector<T> selected_splitters(process_number);

    // Then if we are the first process, receive all the data and fill the splitter data structure
    if(process_ID == 0) {
        timer.tic("evenly-spaced elements");
        std::vector< std::vector<T> > received_data;
        received_data.reserve(process_number);
        received_data.emplace_back(std::move(elements_to_send));
        for(std::size_t p{1}; p < process_number; ++p) {
            received_data.emplace_back(process_number-1);
            MPI_Recv(received_data.back().data(), process_number-1, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        const std::vector<T> sorted_all_splitters = merge_sorted_arrays_sequential(received_data);
        selected_splitters = select_evenly_spaced(sorted_all_splitters, process_number);
        timer.toc();
    }
    // The work of the process n°0 was to fill the splitters, now we can broadcast.
    timer.tic("broadcast");
    MPI_Bcast(selected_splitters.data(), selected_splitters.size(), MPI_INT, /*root*/ 0, MPI_COMM_WORLD);
    timer.toc();

    
    // For the following code, it is easier to add a last splitter that will be +infinity (or a value bigger than every
    // other value possible in the array
    selected_splitters.back() = std::numeric_limits<T>::max();

    // And now each process sends its data to the process that should manage them.
    std::vector<std::size_t> index_delimitation{0};
    std::vector<std::size_t> sizes(selected_splitters.size()); // Important becase we don't want a reallocation (asynchronous communications).
    std::vector< std::pair<MPI_Request, MPI_Request> > send_requests;
    std::size_t bucket_index{0};
    timer.tic("sending buckets");
    for(std::size_t i{0}; i < array.size(); ++i) {
        // If we enter in a new bucket, than send the data and update the bucket index
        if(array[i] > selected_splitters[bucket_index]) {
            send_requests.emplace_back();
            sizes.emplace_back(i - index_delimitation.back());
            MPI_Isend(&(sizes.back()), 1, MPI_INT, bucket_index, /*tag*/ 0, MPI_COMM_WORLD, &(send_requests.back().first));
            MPI_Isend(array.data() + index_delimitation.back(), sizes.back(), MPI_INT, bucket_index, /*tag*/ 1, MPI_COMM_WORLD, &(send_requests.back().second));
            index_delimitation.emplace_back(i);
            bucket_index++;
        }
    }
    // Send the last bit of data
    const std::size_t buffer_size{array.size() - index_delimitation.back()};
    send_requests.emplace_back();
    MPI_Isend(&buffer_size, 1, MPI_INT, bucket_index, /*tag*/ 0, MPI_COMM_WORLD, &(send_requests.back().first));
    MPI_Isend(array.data() + index_delimitation.back(), buffer_size, MPI_INT, bucket_index, /*tag*/ 1, MPI_COMM_WORLD, &(send_requests.back().second));
    timer.toc();

    
    // Each process should receive its data from all the other process and sort it.
    timer.tic("receiving buckets");
    std::vector< std::vector<T> > final_data;
    // Reception
    for(std::size_t p{0}; p < process_number; ++p) {
        std::size_t size_to_receive;
        MPI_Recv(&size_to_receive, 1, MPI_INT, p, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        final_data.emplace_back(size_to_receive);
        MPI_Recv(final_data.back().data(), size_to_receive, MPI_INT, p, /*tag*/ 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    timer.toc();

    // Merging
    timer.tic("merging received data");
    const auto merged_array = merge_sorted_arrays_sequential(final_data);
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

    distributed_sort(values, process_number, process_ID);

    // Output the values on each processor.
    if(display) {
        // For a pretty print, we put a barrier here.
        // The barrier is NOT needed for a normal algorithm.
        for(std::size_t p{0}; p < process_number; ++p) {
            if(process_ID == p)
                std::cout << "Hi! I'm processor n°" << process_ID << " and I have the values " << values << std::endl;
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }
    
	MPI_Finalize();
	return 0;
}
