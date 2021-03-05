//
// Created by Hemingbear on 2021/3/5.
//

#ifndef BETHSTL_UNINITIALIZED_H
#define BETHSTL_UNINITIALIZED_H

#include "type_traits.h"
#include <algorithm>
#include "construct.h"

template<class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &value) {
    return __uninitialized_fill_n(first, n, value, value_type(first));
}

template<class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &value, T1 *) {
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, value, is_POD());  // plain old data need ctor and dtor
}

template<class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &value, __true_type) {
    return fill_n(first, n, value);
}

template<class ForwardIterator, class Size, class T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &value, __false_type) {
    ForwardIterator cur = first;
    for (int i = 0; i < n; ++i, ++cur) {
        construct(&*cur, value);
    }
    return cur;
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
    return __uninitialized_copy(first, last, result, value_type(result));
}

template<class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T *) {
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type) {
    return copy(first, last, result);
}

template<class InputIterator, class ForwardIterator>
ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type) {
    ForwardIterator cur = result;
    for (; first != last; ++first, ++cur) {
        construct(&*cur, *first);
    }
    return cur;
}

//for char* and wchar_t* , write a special version to maximize performance
inline char *uninitialized_copy(const char *first, const char *last, char *result) {
    memmove(result, first, last - first);
    return result + (last - first);
}

inline wchar_t *uninitialized_copy(const wchar_t *first, const wchar_t *last, wchar_t *result) {
    memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}

template<class ForwardIterator, class T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &value) {
    __uninitialized_fill(first, last, value, value_type(first));
}

template<class ForwardIterator, class T, class T1>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &value, T1 *) {
    typedef typename __type_traits<T1>::is_POD is_POD;
    __uninitialized_fill_aux(first, last, value, is_POD());
}

template<class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &value, __true_type) {
    fill(first, last, value);
}

template<class ForwardIterator, class T>
void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &value, __false_type) {
    ForwardIterator cur = first;
    for (; cur != last; ++cur) {
        construct(&*cur, value);
    }
}


#endif //BETHSTL_UNINITIALIZED_H