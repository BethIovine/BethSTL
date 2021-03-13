//
// Created by Hemingbear on 2021/3/14.
//

#ifndef BETHSTL_NUMERIC_H
#define BETHSTL_NUMERIC_H

#include "iterator.h"

template<class InputIterator, class T>
T accumulate(InputIterator first, InputIterator last, T init) {
    for (; first != last; ++first) {
        init += *first;
    }

    return init;
}


template<class InputIterator, class T, class BinaryOperator>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOperator binary_op) {
    for (; first != last; ++first) {
        init = binary_op(init, *first);
    }

    return init;
}

template<class InputIterator, class OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result) {
    if (first == last) return result;
    *result = *first;
    typename iterator_traits<InputIterator>::value_type value = *first;
    while (++first != last) {
        *++result = *first - value;
        value = *first;
    }

    return ++result;
}

template<class InputIterator, class OutputIterator, class BinaryOperator>
OutputIterator
adjacent_difference(InputIterator first, InputIterator last, OutputIterator result, BinaryOperator binary_op) {
    if (first == last) return result;

    *result = *first;
    typename iterator_traits<InputIterator>::value_type value = *first;
    while (++first != last) {
        *++result = binary_op(*first, value);
        value = *first;
    }

    return ++result;
}

template<class InputIterator1, class InputIterator2, class T>
T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init) {
    for (; first1 != last1; ++first1, ++first2) {
        init = init + ((*first1) * (*first2));
    }
    return init;
}

template<class InputIterator1, class InputIterator2, class T, class BinaryOperator1, class BinaryOperator2>
T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init,
                BinaryOperator1 binary_op1, BinaryOperator2 binary_op2) {
    for (; first1 != last1; ++first1, ++first2) {
        init = binary_op1(init, binary_op2(*first1, *first2));
    }

    return init;
}

template<class InputIterator, class OutputIterator>
OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result) {
    if (first == last) return result;

    *result = *first;
    typename iterator_traits<InputIterator>::value_type value = *first;
    while (++first != last) {
        value += *first;
        *++result = value;
    }

    return ++result;
}

template<class InputIterator, class OutputIterator, class BinaryOperator>
OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result, BinaryOperator binary_op) {
    if (first == last) return result;

    *result = *first;
    typename iterator_traits<InputIterator>::value_type value = *first;
    while (++first != last) {
        value = binary_op(value, *first);
        *++result = value;
    }

    return ++result;
}

template<class T, class Integer>
inline T power(T x, Integer n) {
    return power(x, n, std::multiplies<T>());
}

template<class T, class Integer, class MonoidOperator>
T power(T x, Integer n, MonoidOperator op) {
    if (n == 0)
        return identity_element(op);
    else {
        while ((n & 1) == 0) {
            // if exponent binary end with 0, can binary compute util end with 1
            n >> 1;
            x = op(x, x);
        }
        T result = x;
        n >> 1;
        while (n != 0) {
            x = op(x, x);
            if ((n & 1) != 0)
                result = op(result, x);
            n >> 1;
        }
        return result;
    }
}

template<class ForwardIterator, class T>
void itoa(ForwardIterator first, ForwardIterator last, T value) {
    while (first != last) {
        *first++ = value++;
    }
}

#endif //BETHSTL_NUMERIC_H
