//
// Created by Hemingbear on 2021/3/14.
//

#ifndef BETHSTL_ALGORITHM_BASE_H
#define BETHSTL_ALGORITHM_BASE_H

#include "iterator.h"
#include "type_traits.h"
#include <cstring>
#include <utility>

template<class T>
inline const T &max(const T &a, const T &b) {
    return a < b ? b : a;
}

template<class T, class Compare>
inline const T &max(const T &a, const T &b, Compare comp) {
    return comp(a, b) ? b : a;
}

template<class T>
inline const T &min(const T &a, const T &b) {
    return a > b ? b : a;
}

template<class T, class Compare>
inline const T &min(const T &a, const T &b, Compare comp) {
    return comp(a, b) ? b : a;
}

template<class T>
inline void swap(T &a, T &b) {
    T temp = a;
    a = b;
    b = temp;
}

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


template<class ForwardIterator1, class ForwardIterator2>
inline void iter_swap(ForwardIterator1 iter1, ForwardIterator2 iter2) {
    typename iterator_traits<ForwardIterator1>::value_type temp;
    temp = *iter1;
    *iter1 = *iter2;
    *iter2 = temp;
}

template<class InputIterator1, class InputIterator2>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (*first1 < *first2) {
            return true;
        } else if (*first2 < *first1) {
            return false;
        }
    }
    return first1 == last1 && first2 != last2;
}

template<class InputIterator1, class InputIterator2, class Compare>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2,
                             Compare comp) {
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (comp(*first1, *first2)) {
            return true;
        } else if (comp(*first2, *first2)) {
            return false;
        }
    }
    return first1 == last1 && first2 != last2;
}

// special lexicographical_compare version for char*
inline bool
lexicographical_compare(const unsigned char *first1, const unsigned char *last1, const unsigned char *first2,
                        const unsigned char *last2) {
    const size_t len1 = last1 - first1;
    const size_t len2 = last2 - first2;

    int result = memcmp(first1, first2, min(len1, len2));
    return result == 0 ? len1 < len2 : result < 0;
}

template<class InputIterator1, class InputIterator2>
std::pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
    while (first1 != last1 && *first1 == *first2) {
        ++first1;
        ++first2;
    }
    return std::pair<InputIterator1, InputIterator2>(first1, first2);
}

template<class InputIterator1, class InputIterator2, class BinaryPredicate>
std::pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2,
         BinaryPredicate binary_pred) {
    while (first1 != last1 && binary_pred(*first1, *first2)) {
        ++first1;
        ++first2;
    }
    return std::pair<InputIterator1, InputIterator2>(first1, first2);
}

template<class InputIterator, class OutputIterator>
inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
    return copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
}

inline char *copy(const char *first, const char *last, char *result) {
    memmove(result, first, last - first);
    return result + (last - first);
}

inline wchar_t *copy(const wchar_t *first, const wchar_t *last, wchar_t *result) {
    memmove(result, first, (last - first) * sizeof(wchar_t));
    return result + (last - first);
}

template<class InputIterator, class OutputIterator>
struct copy_dispatch {
    OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
        return _copy(first, last, result, iterator_category(first));
    }
};

template<class T>
struct copy_dispatch<T *, T *> {
    T *operator()(T *first, T *last, T *result) {
        typedef typename __type_traits<T>::has_trivial_assignment_operator t;
        return _copy_t(first, last, result, t());
    }
};

template<class T>
struct copy_dispatch<const T *, T *> {
    T *operator()(T *first, T *last, T *result) {
        typedef typename __type_traits<T>::has_trivial_assignment_operator t;
        return _copy_t(first, last, result, t());
    }
};

template<class InputIterator, class OutputIterator>
inline OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag) {
    for (; first != last; ++result, ++first) {
        *result = *first;
    }
    return result;
}

template<class RandomAccessIterator, class OutputIterator>
inline OutputIterator
_copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, random_access_iterator_tag) {
    return _copy_d(first, last, result, distance_type(first));
}

template<class RandomAccessIterator, class OutputIterator, class Distance>
inline OutputIterator
_copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance *) {
    for (Distance n = last - first; n > 0; --n, ++result, ++first)
        *result = *first;
    return result;
}

template<class T>
inline T *_copy_t(const T *first, const T *last, T *result, __true_type) {
    memmove(result, first, sizeof(T) * (last - first));
    return result + (last - first);
}

template<class T>
inline T *_copy_t(const T *first, const T *last, T *result, __false_type) {
    return _copy_d(first, last, result, (ptrdiff_t *) 0);
}


// set related algorithm
template<class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, InputIterator2 last2,
                         OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 > *first2) {
            *result = *first2;
            ++first2;
        } else if (*first1 < *first2) {
            *result = *first1;
            ++first1;
        } else {
            //equal
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
    // copy all remain element to result
    return copy(first1, last1, copy(first2, last2, result));
}

template<class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                                InputIterator2 first2, InputIterator2 last2,
                                OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 > *first2) {
            ++first2;
        } else if (*first1 < *first2) {
            ++first1;
        } else {
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }
        return result;
    }
}

//set_difference(s1,s2) means s1 - s2
template<class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                              InputIterator2 first2, InputIterator2 last2,
                              OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            *result = *first1;
            ++result;
            ++first1;
        } else if (*first1 > *first2) {
            ++first2;
        } else {
            ++first1;
            ++first2;
        }
        return copy(first1, last1, result);
    }
}

//set_symmetric_difference(s1,s2) means (s1-s2) union (s2-s1)
template<class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                                        InputIterator2 first2, InputIterator2 last2,
                                        OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
            ++result;
        } else if (*first2 < *first1) {
            *result = *first2;
            ++first2;
            ++result;
        } else {
            ++first1;
            ++first2;
        }
        return copy(first1, last1, copy(first2, last2, result));
    }
}


#endif //BETHSTL_ALGORITHM_BASE_H
