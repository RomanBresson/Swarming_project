#ifndef SWARMING_PROJECT_MORTON_INDEX_H
#define SWARMING_PROJECT_MORTON_INDEX_H

#include <type_traits>
#include <array>
#include "definitions/constants.h"

#if SWARMING_DO_ALL_CHECKS == 1
#include <cassert>
#endif

template <typename UIntTypeIn, std::size_t D, typename DepthType, typename UIntTypeOut = unsigned long long>
UIntTypeOut get_morton_index(std::array<UIntTypeIn, D> const & anchor, DepthType depth) {
    static_assert(std::is_integral<UIntTypeIn>::value, "The given input type is not integral.");
    static_assert(std::is_integral<DepthType>::value, "The given input type is not integral.");
    static_assert(D * constants::Dmax + 5 <= 8*sizeof(UIntTypeOut), "The output value may overflow.");

#if SWARMING_DO_ALL_CHECKS == 1
    assert(depth >= 0);
    for(std::size_t i{0}; i < D; ++i) assert(anchor[i] >= 0);
#endif

    UIntTypeOut morton_enc{depth & 0x1F};
    unsigned int bit_position{5};
    for (unsigned int dimension_bit_position{0}; dimension_bit_position < constants::Dmax; ++dimension_bit_position){
        for (std::size_t dimension{0}; dimension < D; ++dimension){
            morton_enc |= ( (anchor[dimension]>>dimension_bit_position) & 1ULL) << bit_position;
            ++bit_position;
        }
    }
    return morton_enc;
};

#endif //SWARMING_PROJECT_MORTON_INDEX_H
