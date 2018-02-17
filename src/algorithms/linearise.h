#ifndef SWARMING_PROJECT_LINEARISE_H
#define SWARMING_PROJECT_LINEARISE_H

#include "algorithms/remove_duplicates.h"

template <typename Container, typename StoredData>
Container<StoredData> linearise(Container<StoredData> & container) {
    return remove_duplicates(container,
                             [](StoredData const & lhs, StoredData const & rhs) { return lhs.is_ancestor(rhs); });
}


#endif //SWARMING_PROJECT_LINEARISE_H
