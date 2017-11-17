#ifndef SWARMING_PROJECT_TYPES_H
#define SWARMING_PROJECT_TYPES_H

#include <ostream>

#include "data_structures/MathArray.h"

namespace types {

    template <typename T, std::size_t S>
    using ArrayType    = MathArray<T, S>;

    using PositionType = float;
    using VelocityType = float;
    using ForceType    = float;
    using DistanceType = float;

    template <std::size_t Size> using Position = ArrayType<PositionType, Size>;
    template <std::size_t Size> using Velocity = ArrayType<VelocityType, Size>;
    template <std::size_t Size> using Force    = ArrayType<ForceType,    Size>;
    template <std::size_t Size> using Distance = ArrayType<DistanceType, Size>;


}

template <typename T, std::size_t S>
std::ostream & operator<<(std::ostream & os, const types::ArrayType<T,S> & array) {
    os << "(";
    for(std::size_t i{0}; i < S-1; ++i) {
        os << array[i] << ",";
    }
    return os << array[S-1] << ")";
};

#endif //SWARMING_PROJECT_TYPES_H
