//
// Created by Hemingbear on 2021/3/5.
//

#include <cstddef>

#ifndef BETHSTL_ITERATOR_H
#define BETHSTL_ITERATOR_H

struct input_iterator_tag {
};
struct output_iterator_tag {
};
struct forward_iterator_tag : public input_iterator_tag {
};
struct bidirectional_iterator_tag : public forward_iterator_tag {
};
struct random_access_iterator_tag : public bidirectional_iterator_tag {
};


template<class Category, class T, class Distance = ptrdiff_t, class Pointer = T *, class Reference = T &>
struct iterator {
    typedef Category iterator_category;
    typedef T value_type;
    typedef Distance difference_type;
    typedef Pointer pointer;
    typedef Reference reference;
};

template<class Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type value_type;
    typedef typename Iterator::difference_type difference_type;
    typedef typename Iterator::pointer pointer;
    typedef typename Iterator::reference reference;
};

// native pointer partial version
template<class T>
struct iterator_traits<T *> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T *pointer;
    typedef T &reference;
};

//  pointer to const  partial version
template<class T>
struct iterator_traits<const T *> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T *pointer;
    typedef T &reference;
};

template<class Iterator>
inline typename iterator_traits<Iterator>::difference_type *distance_type(const Iterator &) {
    return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
}

template<class Iterator>
inline typename iterator_traits<Iterator>::value_type *value_type(const Iterator &) {
    return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
}

template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    for (; first != last; ++first) ++n;
    return n;
}

template<class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
    return last - first;
}

template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last) {
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    return __distance(first, last, category());
}

template<class InputIterator, class Distance>
inline void __advance(InputIterator &i, Distance n, input_iterator_tag) {
    while (n--) ++i;
}

template<class BidirectionalIterator, class Distance>
inline void __advance(BidirectionalIterator &i, Distance n, bidirectional_iterator_tag) {
    if (n >= 0) {
        while (n--) ++i;
    } else {
        while (n++) --i;
    }
}

template<class RandomIterator, class Distance>
inline void __advance(RandomIterator &i, Distance n, random_access_iterator_tag) {
    i += n;
}

template<class InputIterator, class Distance>
inline void advance(InputIterator &i, Distance n) {
    __distance(i, n, iterator_category(i));
}


#endif //BETHSTL_ITERATOR_H
