/* ******************************************************
 *                External overloads
 *******************************************************/

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
    MathArray<T,S> copy(rhs);
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
    MathArray<T,S> copy(rhs);
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
    MathArray<T,S> copy(rhs);
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
    MathArray<T,S> copy(rhs);
    copy /= lhs;
    return copy;
};

