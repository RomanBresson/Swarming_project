#ifndef SWARMING_PROJECT_LINEARISE_H
#define SWARMING_PROJECT_LINEARISE_H

#include <functional>

#include "algorithms/remove_duplicates.h"

template <typename Container, typename StoredDataType = typename Container::value_type>
Container linearise(Container const & container) {
    std::function<bool(const StoredDataType &, const StoredDataType &)> is_duplicate =
            [](StoredDataType const & lhs, StoredDataType const & rhs) { return lhs.is_ancestor(rhs); };
    return remove_duplicates(container, is_duplicate);
}


#endif //SWARMING_PROJECT_LINEARISE_H
