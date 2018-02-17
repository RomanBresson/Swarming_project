#ifndef SWARMING_PROJECT_LINEARISE_H
#define SWARMING_PROJECT_LINEARISE_H

#include "algorithms/remove_duplicates.h"

template <typename Container, typename StoredDataType = typename Container::value_type>
Container<StoredDataType> linearise(Container const & container) {
    return remove_duplicates(container,
                             [](StoredDataType const & lhs, StoredDataType const & rhs) { return lhs.is_ancestor(rhs); });
}


#endif //SWARMING_PROJECT_LINEARISE_H
