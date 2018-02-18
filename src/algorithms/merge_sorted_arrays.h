#ifndef SWARMING_PROJECT_MERGE_SORTED_ARRAYS_H
#define SWARMING_PROJECT_MERGE_SORTED_ARRAYS_H

#include <vector>
#include <algorithm>
#include <list>
#include <numeric>

#include "definitions/constants.h"

#if SWARMING_DO_ALL_CHECKS == 1
#include <iostream>
#endif

template <typename T, typename Comp = std::less<T>, typename ContainerOut = std::vector<T>>
static ContainerOut merge_sorted_arrays_sequential(std::vector< std::vector<T> > & arrays, Comp comp = Comp()) {

    // We compute the result in a std::vector because the std::merge_inplace and std::merge will benefit from the
    // data locality.
    std::vector<T> result;

#if SWARMING_DO_ALL_CHECKS == 1
    if(arrays.size() < 2) {
        std::cerr << "merge_sorted_arrays_sequential should be called with 2 or more arrays to merge." << std::endl;
        return ContainerOut(result.begin(), result.end());
    }
#endif

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
    return ContainerOut(result.begin(), result.end());
}


#endif //SWARMING_PROJECT_MERGE_SORTED_ARRAYS_H
