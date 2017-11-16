#ifndef SWARMING_PROJECT_MATHARRAY_H
#define SWARMING_PROJECT_MATHARRAY_H

#include <array>
#include <cmath>
#include <type_traits>
#include <limits>
#include <initializer_list>
#include <utility>

/**
 * An overload of std::array that implements usefull mathematical operations.
 * @tparam T type of the stored elements.
 * @tparam S number of elements stored.
 */
template <typename T, std::size_t S>
class MathArray : public std::array<T, S> {

    // Ensure that the mathematical operations are well-defined.
    static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value,
                  "The elements stored in a MathArray should be either integral or floating point values.");

public:

    /**
     * Default constructor.
     */
    MathArray() = default;

    /**
     * Construct the array and fill it with the given value.
     * @param value used to fill the created array.
     */
    explicit MathArray(T value) {
        for(std::size_t i{0}; i < S; ++i) {
            (*this)[i] = value;
        }
    }

    // Constructor implementation following the SO answer below.
    // https://stackoverflow.com/questions/24280521/stdarray-constructor-inheritance#24281360
    explicit MathArray(std::array<T, S>   array) : std::array<T, S>(std::forward<std::array<T,S>>(array)) { }
    explicit MathArray(std::array<T, S> & array) : std::array<T, S>(std::forward<std::array<T,S>>(array)) { }

    /**
     * Compute the p-norm of the array.
     * @param p parameter of the norm. p=2.0 is the euclidian norm and is the default behaviour.
     * @return  p-norm of the array.
     */
    double norm(double p = 2.0) const {
        double norm{0.0};
        for(std::size_t i{0}; i < S; ++i) {
            norm += std::pow((*this)[i], p);
        }
        return std::pow(norm, 1.0/p);
    }

};





#endif //SWARMING_PROJECT_MATHARRAY_H
