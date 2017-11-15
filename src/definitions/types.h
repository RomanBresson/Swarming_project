#ifndef SWARMING_PROJECT_TYPES_H
#define SWARMING_PROJECT_TYPES_H

#include <array>
#include <ostream>

namespace types {

    template <typename T, std::size_t S>
    using ArrayType    = std::array<T, S>;

    using PositionType = float;
    using VelocityType = float;
    using ForceType    = float;

    template <std::size_t Size> using Position = ArrayType<PositionType, Size>;
    template <std::size_t Size> using Velocity = ArrayType<VelocityType, Size>;
    template <std::size_t Size> using Force    = ArrayType<ForceType,    Size>;

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
