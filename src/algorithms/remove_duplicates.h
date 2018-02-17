#ifndef SWARMING_PROJECT_REMOVE_DUPLICATES_H
#define SWARMING_PROJECT_REMOVE_DUPLICATES_H

#include <iterator>
#include <functional>
#include "definitions/constants.h"

#if SWARMING_DO_ALL_CHECKS == 1
#include <algorithm>
#include <cassert>
#endif

/**
 * In this file we implement the algorithm used to remove duplicates from a sorted list
 * of octants.
 */

template <typename Container, typename StoredData = typename Container::value_type>
Container remove_duplicates(Container const & container,
                            std::function<bool(StoredData const &, StoredData const &)> is_duplicate = std::equal_to<StoredData const &>()) {

#if SWARMING_DO_ALL_CHECKS == 1
    assert(std::is_sorted(container.begin(), container.end()));
#endif

    Container container_without_duplicates;
    auto end        = container.end();
    auto before_end = std::prev(end);

    for(auto octant_it = container.begin();
        octant_it != before_end && octant_it != end;
        ++octant_it) {

        // First push this octant inside the no-duplicate structure
        container_without_duplicates.push_back(*octant_it);
        // Then check if the following octant is a duplicate and if it is, then
        // avoid its processing by moving the iterator one step forward.
        auto next_it = std::next(octant_it);
        while(is_duplicate(*octant_it, *next_it)) {
            ++octant_it;
            ++next_it;
        }
    }
    return container_without_duplicates;
};

#endif //SWARMING_PROJECT_REMOVE_DUPLICATES_H
