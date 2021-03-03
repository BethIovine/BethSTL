//
// Created by Beth on 2021/3/3.
//

#ifndef BETHSTL_CONSTRUCT_H
#define BETHSTL_CONSTRUCT_H

#include "type_traits.h"

template<class T>
inline void construct(T *pointer) {
    new((void *) pointer)T(); // placement new
}

template<class T1, class T2>
inline void construct(T1 *pointer, T2 &value) {
    new((void *) pointer)T1(value);
}

template<class T>
inline void destroy(T *pointer) {
    pointer->~T();
}

template<class ForwardIterator, class T>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    destroyHelper(first, last, value_type(first));
}

template<class ForwardIterator, class T>
inline void destroyHelper(ForwardIterator first, ForwardIterator last, T *) {
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    destroyAux(first, last, trivial_destructor());
}

template<class ForwardIterator>
inline void destroyAux(ForwardIterator first,ForwardIterator last,__false_type){
    for(;first< last;++first){
        destroy(&(*first));
    }
}

template<class ForwardIterator>
inline void destroyAux(ForwardIterator first,ForwardIterator last,__true_type){}    //base type don't need operation


#endif //BETHSTL_CONSTRUCT_H
