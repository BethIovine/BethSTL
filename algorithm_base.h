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

template<class ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last) {
    if (first == last) return last;
    ForwardIterator next = first;
    while (++next != last) {
        if (*first == *next) return first;
        first = next;
    }
    return last;
}

template<class ForwardIterator, class BinaryPredicate>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last, BinaryPredicate binary_op) {
    if (first == last) return last;
    ForwardIterator next = first;
    while (++next != last) {
        if (BinaryPredicate(*first, *next)) return first;
        first = next;
    }
    return last;
}

template<class InputIterator, class T>
typename iterator_traits<InputIterator>::difference_type
count(InputIterator first, InputIterator last, const T &value) {
    typename iterator_traits<InputIterator>::difference_type num = 0;
    for (; first != last; ++first) {
        if (*first == value) ++num;
    }
    return num;
}

// count_if means count number if meet the conditions
template<class InputIterator, class Predicate>
typename iterator_traits<InputIterator>::difference_type
count_if(InputIterator first, InputIterator last, Predicate pred) {
    typename iterator_traits<InputIterator>::difference_type num = 0;
    for (; first != last; ++first) {
        if (pred(*first)) ++num;
    }
    return num;
}

template<class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T &value) {
    for (; first != last; ++first) if (*first == value) return first;
    return last;
}

template<class InputIterator, class Predicate>
InputIterator find_if(InputIterator first, InputIterator last, Predicate pred) {
    for (; first != last; ++first) if (pred(*first)) return first;
    return last;
}

template<class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                                 ForwardIterator2 first2, ForwardIterator2 last2) {
    typedef typename iterator_traits<ForwardIterator1>::iterator_category category1;
    typedef typename iterator_traits<ForwardIterator2>::iterator_category category2;
    return find_end_aux(first1, last1, first2, last2, category1(), category2());
}

template<class ForwardIterator1, class ForwardIterator2>
ForwardIterator1 find_end_aux(ForwardIterator1 first1, ForwardIterator1 last1,
                              ForwardIterator2 first2, ForwardIterator2 last2,
                              forward_iterator_tag, forward_iterator_tag) {
    if (first2 == last2) return last1;

    ForwardIterator1 result = last1;
    while (true) {
        ForwardIterator1 newResult = search(first1, last1, first2, last2);
        if (newResult == last1) {
            // not found
            return result;
        } else {
            result = newResult;
            first1 = newResult;
            ++first1;
        }
    }
}

//template<class BidirectionalIterator1, class BidirectionalIterator2>
//BidirectionalIterator1 find_end_aux(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
//                              BidirectionalIterator2 first2, BidirectionalIterator2 last2,
//                              bidirectional_iterator_tag, bidirectional_iterator_tag){
//
//}


template<class InputIterator, class ForwardIterator>
InputIterator find_first_of(InputIterator first1, InputIterator last1,
                            ForwardIterator first2, ForwardIterator last2) {
    for (; first1 != last1; ++first1) {
        for (ForwardIterator iter = first2; iter != last2; ++iter) {
            if (*first1 == *iter) return first1;
        }
    }
    return last1;
}

template<class InputIterator, class ForwardIterator, class BinaryPredicate>
InputIterator find_first_of(InputIterator first1, InputIterator last1,
                            ForwardIterator first2, InputIterator last2,
                            BinaryPredicate binary_op) {
    for (; first1 != last1; ++first1) {
        for (ForwardIterator iter = first2; iter != last2; ++iter) {
            if (binary_op(*first1, *iter)) return first1;
        }
    }
    return last1;
}

template<class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function func) {
    for (; first != last; ++first) {
        func(*first);
    }
    return func;
}

template<class OutputIterator, class Size, class Generator>
OutputIterator generate_n(OutputIterator first, Size n, Generator gen) {
    for (int i = 0; i < n; ++i, ++first) {
        *first = gen();
    }
    return first;
}

template<class ForwardIterator, class Generator>
void generate(ForwardIterator first, ForwardIterator last, Generator gen) {
    for (; first != last; ++first) {
        *first = gen();
    }
}

template<class InputIterator1, class InputIterator2>
bool includes(InputIterator1 first1, InputIterator1 last1,
              InputIterator2 first2, InputIterator2 last2) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            ++first1;
        } else if (*first1 > *first2) {
            return false;
        } else {
            ++first1;
            ++first2;
        }
    }
    return first2 == last2;
}

template<class InputIterator1, class InputIterator2, class Compare>
bool includes(InputIterator1 first1, InputIterator1 last1,
              InputIterator2 first2, InputIterator2 last2,
              Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            ++first1;
        } else if (comp(*first2, *first1)) {
            return false;
        } else {
            ++first1;
            ++first2;
        }
    }
    return first2 == last2;
}

#endif //BETHSTL_ALGORITHM_BASE_H
