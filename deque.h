//
// Created by Hemingbear on 2021/3/8.
//

#ifndef BETHSTL_DEQUE_H
#define BETHSTL_DEQUE_H

#include <cstdio>
#include "iterator.h"
#include "alloc.h"

enum {
    default_alloc_buffer_size = 512
};

inline size_t deque_buf_size(size_t n, size_t size) {
    // n != 0 ,client directly set alloc n*T bytes to one buffer
    return (n != 0 ? n : (size < (size_t) default_alloc_buffer_size ? (size_t) default_alloc_buffer_size / size
                                                                    : size_t(1)));
}

template<class T, class Ref, class Ptr>
class Deque_iterator {
    typedef Deque_iterator<T, T &, T *> iterator;
    typedef Deque_iterator<T, const T &, const T *> const_iterator;

    static size_t bufferSize(size_t n) { return deque_buf_size(n, sizeof(T)); }

    static size_t bufferSize() { return deque_buf_size(0, sizeof(T)); }

    typedef T value_type;
    typedef random_access_iterator_tag iterator_category;
    typedef Ref reference;
    typedef Ptr pointer;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T **Map_pointer;

    typedef Deque_iterator _self;

    T *cur;
    T *first;
    T *last;
    Map_pointer map_node;

    Deque_iterator(T *nodePtr, Map_pointer mapPtr) : cur(nodePtr), first(*mapPtr), last(*mapPtr + bufferSize()),
                                                     map_node(mapPtr) {}

    Deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), map_node(nullptr) {}

    Deque_iterator(const iterator &x) : cur(x.cur), first(x.first), last(x.last), map_node(x.map_node) {}

    reference operator*() const { return *cur; };

    pointer operator->() const { return cur; }

    difference_type operator-(const _self &x) const {
        return difference_type(bufferSize()) * (map_node - map_node - 1) + (cur - first) + (x.last - x.cur);
    }

    _self operator++() {
        ++cur;
        if (cur == last) {
            set_node(map_node + 1);
            cur = first;
        }
    }

    _self operator++(int) {
        _self temp = *this;
        ++*this;
        return temp;
    }

    _self operator--() {
        if (cur == first) {
            set_node(map_node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    _self operator--(int) {
        _self temp = *this;
        --*this;
        return temp;
    }

    _self operator+=(difference_type n) {
        difference_type offset = n + (cur - first);
        difference_type bufSize = difference_type(bufferSize());
        if (offset >= 0 && offset < bufSize)
            cur += n;
        else {
            difference_type node_offset = offset > 0 ? offset / bufSize : -((-offset - 1) / bufSize) - 1;
            set_node(map_node + node_offset);
            cur = first + (offset - node_offset * bufSize);
        }
        return *this;
    }

    _self operator+(difference_type n) const {
        _self temp = *this;
        return temp += n;
    }

    _self operator-=(difference_type n) {
        return *this += -n;
    }

    _self operator-(difference_type n) const {
        _self temp = *this;
        return temp -= n;
    }

    reference operator[](difference_type n) const {
        return *(*this + n);
    }

    bool operator==(const _self &x) { return cur == x.cur; }

    bool operator!=(const _self &x) { return !(*this == x); }

    bool operator<(const _self &x) {
        return (map_node == x.map_node) ? cur < x.cur : map_node < x.map_node;
    }

    bool operator>(const _self &x) {
        return x < *this;
    }

    bool operator<=(const _self &x) const { return !(x < *this); }

    bool operator>=(const _self &x) const { return !(*this < x); }

    void set_node(Map_pointer new_node) {
        map_node = new_node;
        first = *new_node;
        last = first + difference_type(bufferSize());
    }

};


#endif //BETHSTL_DEQUE_H
