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

    /**
     * Soustraction logic for the MathArray class.
     * @tparam T2   type of the stored elements of the array to substract.
     * @param array the array to substract.
     * @return      the updated current instance.
     */
    template <typename T2>
    MathArray<T, S> operator-=(const MathArray<T2, S> & array) {
        for(std::size_t i{0}; i < S; ++i) {
            (*this)[i] -= array[i];
        }
        return *this;
    };

    /**
     * Soustraction logic for the MathArray class.
     * @tparam T2   type of the stored elements of the array to substract.
     * @param array the array to substract.
     * @return      the updated current instance.
     */
    template <typename T2>
    MathArray<T, S> operator-=(const T2 & element) {
        for(std::size_t i{0}; i < S; ++i) {
            (*this)[i] -= element;
        }
        return *this;
    };


    /**
     * Addition logic for the MathArray class.
     * @tparam T2   type of the stored elements of the array to add.
     * @param array the array to add.
     * @return      the updated current instance.
     */
    template <typename T2>
    MathArray<T, S> operator+=(const MathArray<T2, S> & array) {
        for(std::size_t i{0}; i < S; ++i) {
            (*this)[i] += array[i];
        }
        return *this;
    };

    /**
     * Addition logic for the MathArray class.
     * @tparam T2   type of the stored elements of the array to add.
     * @param array the array to add.
     * @return      the updated current instance.
     */
    template <typename T2>
    MathArray<T, S> operator+=(const T2 & element) {
        for(std::size_t i{0}; i < S; ++i) {
            (*this)[i] += element;
        }
        return *this;
    };

    /**
     * Multiplication logic for the MathArray class.
     * @tparam T2   type of the stored elements of the array to multiply.
     * @param array the array to multiply.
     * @return      the updated current instance.
     */
    template <typename T2>
    MathArray<T, S> operator*=(const MathArray<T2, S> & array) {
        for(std::size_t i{0}; i < S; ++i) {
            (*this)[i] *= array[i];
        }
        return *this;
    };

    /**
     * Multiplication logic for the MathArray class.
     * @tparam T2   type of the stored elements of the array to multiply.
     * @param array the array to multiply.
     * @return      the updated current instance.
     */
    template <typename T2>
    MathArray<T, S> operator*=(const T2 & element) {
        for(std::size_t i{0}; i < S; ++i) {
            (*this)[i] *= element;
        }
        return *this;
    };


    /**
     * Division logic for the MathArray class.
     * @tparam T2   type of the stored elements of the array to divide.
     * @param array the array to divide.
     * @return      the updated current instance.
     */
    template <typename T2>
    MathArray<T, S> operator/=(const MathArray<T2, S> & array) {
        for(std::size_t i{0}; i < S; ++i) {
            (*this)[i] /= array[i];
        }
        return *this;
    };

    /**
     * Division logic for the MathArray class.
     * @tparam T2   type of the stored elements of the array to division.
     * @param array the array to division.
     * @return      the updated current instance.
     */
    template <typename T2>
    MathArray<T, S> operator/=(const T2 & element) {
        for(std::size_t i{0}; i < S; ++i) {
            (*this)[i] /= element;
        }
        return *this;
    };
};


//#include "MathArrayOperators.tpp"


/*
 * Addition
 */
template <typename T, typename U, std::size_t S>
MathArray<T,S> operator+(const MathArray<T,S> & lhs, const MathArray<U,S> & rhs) {
    MathArray<T,S> copy(lhs);
    copy += rhs;
    return copy;
};

template <typename T, typename U, std::size_t S>
MathArray<T,S> operator+(MathArray<T,S> && lhs, MathArray<U,S> && rhs) {
    MathArray<T,S> copy(std::forward<MathArray<T,S>>(lhs));
    copy += rhs;
    return copy;
};

template <typename T, typename U, std::size_t S>
MathArray<T,S> operator+(const MathArray<T,S> & lhs, const U & rhs) {
    MathArray<T,S> copy(lhs);
    copy += rhs;
    return copy;
};

template <typename T, typename U, std::size_t S>
MathArray<U,S> operator+(T lhs, const MathArray<U,S> & rhs) {
    MathArray<U,S> copy(rhs);
    copy += lhs;
    return copy;
};


/*
 * Substraction
 */
template <typename T, typename U, std::size_t S>
MathArray<T,S> operator-(const MathArray<T,S> & lhs, const MathArray<U,S> & rhs) {
    MathArray<T,S> copy(lhs);
    copy -= rhs;
    return copy;
};

template <typename T, typename U, std::size_t S>
MathArray<T,S> operator-(MathArray<T,S> && lhs, MathArray<U,S> && rhs) {
    MathArray<T,S> copy(std::forward<MathArray<T,S>>(lhs));
    copy -= rhs;
    return copy;
};


template <typename T, typename U, std::size_t S>
MathArray<T,S> operator-(const MathArray<T,S> & lhs, const U & rhs) {
    MathArray<T,S> copy(lhs);
    copy -= rhs;
    return copy;
};

template <typename T, typename U, std::size_t S>
MathArray<U,S> operator-(T lhs, const MathArray<U,S> & rhs) {
    MathArray<U,S> copy(rhs);
    copy -= lhs;
    return copy;
};


/*
 * Multiplication
 */
template <typename T, typename U, std::size_t S>
MathArray<T,S> operator*(const MathArray<T,S> & lhs, const MathArray<U,S> & rhs) {
    MathArray<T,S> copy(lhs);
    copy *= rhs;
    return copy;
};

template <typename T, typename U, std::size_t S>
MathArray<T,S> operator*(MathArray<T,S> && lhs, MathArray<U,S> && rhs) {
    MathArray<T,S> copy(std::forward<MathArray<T,S>>(lhs));
    copy *= rhs;
    return copy;
};

template <typename T, typename U, std::size_t S>
MathArray<T,S> operator*(const MathArray<T,S> & lhs, const U & rhs) {
    MathArray<T,S> copy(lhs);
    copy *= rhs;
    return copy;
};

template <typename T, typename U, std::size_t S>
MathArray<U,S> operator*(T lhs, const MathArray<U,S> & rhs) {
    MathArray<U,S> copy(rhs);
    copy *= lhs;
    return copy;
};


/*
 * Division
 */
template <typename T, typename U, std::size_t S>
MathArray<T,S> operator/(const MathArray<T,S> & lhs, const MathArray<U,S> & rhs) {
    MathArray<T,S> copy(lhs);
    copy /= rhs;
    return copy;
};

template <typename T, typename U, std::size_t S>
MathArray<T,S> operator/(MathArray<T,S> && lhs, MathArray<U,S> && rhs) {
    MathArray<T,S> copy(std::forward<MathArray<T,S>>(lhs));
    copy /= rhs;
    return copy;
};

template <typename T, typename U, std::size_t S>
MathArray<T,S> operator/(const MathArray<T,S> & lhs, const U & rhs) {
    MathArray<T,S> copy(lhs);
    copy /= rhs;
    return copy;
};

template <typename T, typename U, std::size_t S>
MathArray<U,S> operator/(T lhs, const MathArray<U,S> & rhs) {
    MathArray<U,S> copy(rhs);
    copy /= lhs;
    return copy;
};



#endif //SWARMING_PROJECT_MATHARRAY_H
