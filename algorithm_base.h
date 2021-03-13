//
// Created by Hemingbear on 2021/3/14.
//

#ifndef BETHSTL_ALGORITHM_BASE_H
#define BETHSTL_ALGORITHM_BASE_H

template<class InputIterator1, class InputIterator2>
inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
    for (; first1 != last1; ++first1, ++first2) {
        if (*first1 != *first2) return false;
    }
    return true;
}

template<class InputIterator1, class InputIterator2, class BinaryEqualOperator>
inline bool
equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryEqualOperator binary_equal_op) {
    for (; first1 != last1; ++first1, ++first2) {
        if (!binary_equal_op(*first1, *first2)) return false;
    }
    return true;
}

template<class ForwardIterator, class T>
void fill(ForwardIterator first, ForwardIterator last, const T &value) {
    for (; first != last; ++first) {
        *first = value;
    }
}


template<class OutputIterator, class Size, class T>
OutputIterator fill_n(OutputIterator first, Size n, const T &value) {
    for (; n > 0; n--, first++) {
        *first = value;
    }
    return first;
}


#endif //BETHSTL_ALGORITHM_BASE_H
