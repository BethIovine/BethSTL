//
// Created by Hemingbear on 2021/3/8.
//

#ifndef BETHSTL_DEQUE_H
#define BETHSTL_DEQUE_H

#include <cstdio>
#include "iterator.h"
#include "alloc.h"
#include "uninitialized.h"
#include <cmath>

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


template<class T, class Alloc = STL_DEFAULT_ALLOCATOR, size_t BufSize = 0>
class deque {
public:
    typedef Deque_iterator<T, T &, T *> iterator;
    typedef Deque_iterator<T, const T &, const T *> const_iterator;
    typedef size_t size_type;
    typedef T value_type;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef T **Map_pointer;
    typedef ptrdiff_t difference_type;

protected:
    typedef simpleAlloc<T, Alloc> _Node_alloc_type;
    typedef simpleAlloc<T *, Alloc> _Map_alloc_type;

    T *allocate_node() {
        return _Node_alloc_type::allocate(deque_buf_size(BufSize, sizeof(T)));
    }

    void deallocate_node(T *pointer) {
        _Node_alloc_type::deallocate(pointer, deque_buf_size(BufSize, sizeof(T)));
    }

    T **allocate_map(size_t n) {
        return _Map_alloc_type::allocate(n);
    }

    void deallocate_map(T **pointer, size_t n) {
        _Map_alloc_type::deallocate(pointer, n);
    }


    void initialize_map(size_type size);

    void create_nodes(T **start, T **finish);

    void destroy_nodes(T **start, T **finish);

    enum {
        initial_map_size = 8
    };

    T **map;
    size_t map_size;
    iterator start;
    iterator finish;

    iterator begin() { return start; }

    iterator end() { return finish; }

    reference operator[](size_type n) {
        return start[n];    //invoke deque iterator operator []
    }

    reference front() { return *start; }

    reference back() {
        iterator temp = finish;
        --temp;
        return *temp;
    }

    size_type size() const {
        return finish - start;
    }

    size_type maxSize() const {
        return size_type(-1);
    }

    bool empty() const { return start == finish; }

public:
    deque() : map(nullptr), map_size(0), start(nullptr), finish(nullptr) {};


    deque(size_type n, const value_type &value) : map(nullptr), map_size(0), start(nullptr), finish(nullptr) {
        fill_initialize(n, value);
    }

    deque(size_type n) : map(nullptr), map_size(0), start(nullptr), finish(nullptr) {
        create_map_and_nodes(n);
    }

    void push_back(const value_type &value) {
        if (finish.cur != finish.last - 1) {
            construct(finish.cur, value);
            ++finish.cur;
        } else {
            push_back_aux(value);
        }
    }

    void push_back_aux(const value_type &value) {
        value_type copy = value;
        resize_map_at_back();
        *(finish.map_node + 1) = allocate_node();
        try {
            construct(finish.cur, copy);
        } catch (...) {
            deallocate_node(*(finish.map_node + 1));
        }

        finish.set_node(finish.map_node + 1);
        finish.cur = finish.first;
    }

    void push_front(const value_type &value) {
        if (start.cur != start.first) {
            construct(start.cur, value);
            start.cur--;
        } else {
            push_front_aux(value);
        }
    }

    void push_front_aux(const value_type &value) {
        value_type copy = value;
        resize_map_at_front();
        *(start.map_node - 1) = allocate_node();
        try {
            construct(start.cur, copy);
        } catch (...) {
            deallocate_node(*(start.map_node - 1));
        }

        start.set_node(start.map_node - 1);
        start.cur = start.last - 1;
    }

    void pop_back() {
        if (finish.cur != finish.first) {
            // cur is empty,move one step backward and deconstruct
            finish.cur--;
            destroy(finish.cur);
        } else {
            pop_back_aux();
        }
    }

    void pop_back_aux() {
        deallocate_node(finish.first);
        finish.set_node(finish.map_node - 1);
        finish.cur = finish.last - 1;
        destroy(finish.cur);
    }

    void pop_front() {
        if (start.cur != start.last - 1) {
            start.cur++;
            destroy(start.cur);
        } else {
            pop_front_aux();
        }
    }

    void pop_front_aux() {
        destroy(start.cur);
        deallocate_node(start.first);
        start.set_node(start.map_node + 1);
        start.cur = start.first;
    }

    iterator erase(iterator pos) {
        iterator next = pos;
        ++next;
        difference_type index = pos - start;
        if (index < (size() / 2)) {
            // move pre-pos elements;
            std::copy_backward(start, pos, next);
            pop_front();
        } else {
            std::copy(next, finish, pos);
            pop_back();
        }
        return start + index;
    }

    iterator erase(iterator first, iterator last) {
        if (first == start && last == finish) {
            clear();
            return finish;
        } else {
            difference_type eraseNum = last - first;
            difference_type preNum = first - start;
            if (preNum < (size() - eraseNum) / 2) {
                std::copy_backward(start, first, last);
                iterator new_start = start + eraseNum;
                destroy(start, new_start);
                for (Map_pointer cur = start.map_node; cur < new_start.map_node; ++cur) {
                    deallocate_node(*cur);
                }
                start = new_start;
            } else {
                std::copy(last, finish, first);
                iterator new_finish = finish - eraseNum;
                destroy(new_finish, finish);
                for (Map_pointer cur = new_finish.map_node + 1; cur < finish.map_node; ++cur) {
                    deallocate_node(*cur);
                }
                finish = new_finish;
            }
            return start + preNum;
        }
    }

    void clear();


protected:
    void fill_initialize(size_type n, const value_type &value);

    void create_map_and_nodes(size_type n);

    void resize_map_at_back(size_type node_to_add = 1) {
        if (map_size - (finish.map_node - map) < node_to_add + 1) {
            realloc_map(node_to_add, false);
        }
    }

    void resize_map_at_front(size_type node_to_add = 1) {
        if (node_to_add > start.map_node - map) {
            realloc_map(node_to_add, true);
        }
    }

    void realloc_map(size_type node_to_add, bool addFromFront);

};

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type n) {
    size_type bufferSize = deque_buf_size(0, BufSize);
    size_type map_node_num = n / bufferSize + 1;
    size_type map_size = fmax(map_node_num + 2, (size_type) initial_map_size);

    map = allocate_map(map_size);
    Map_pointer nStart = map + (map_size - map_node_num) / 2;
    Map_pointer nFinish = nStart + map_node_num - 1;
    Map_pointer cur = nStart;
    try {
        for (; cur <= nFinish; ++cur) {
            *cur = allocate_node();
        }
    } catch (...) {
        for (; cur >= nStart; --cur)
            deallocate_node(*cur);
        deallocate_map(map, map_size);
        throw;
    }
    start.set_node(nStart);
    finish.set_node(nFinish);
    start.cur = start.first;
    finish.cur = finish.first + n % bufferSize;
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type &value) {
    size_type bufferSize = deque_buf_size(0, BufSize);
    create_map_and_nodes(n);
    Map_pointer cur;

    try {
        for (cur = start.map_node; cur < finish.map_node; ++cur) {
            uninitialized_fill(*cur, *cur + bufferSize, value);
        }
        uninitialized_fill(finish.first, finish.cur, value);
    } catch (...) {
        for (cur = start.map_node; cur <= finish.map_node; ++cur)
            deallocate_node(*cur);
        deallocate_map(map, map_size);
        throw;
    }
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::realloc_map(size_type node_to_add, bool addFromFront) {
    size_type old_nodes_num = finish.map_node - start.map_node + 1;
    size_type new_nodes_num = old_nodes_num + node_to_add;

    Map_pointer newNstart;
    if (map_size > 2 * new_nodes_num) {
        //do not need reallocate space
        newNstart = map + (map_size - new_nodes_num) / 2 + (addFromFront ? node_to_add : 0);
        if (newNstart < start.map_node) {
            std::copy(start.map_node, finish.map_node + 1, newNstart);
        } else {
            std::copy_backward(start.map_node, finish.map_node + 1,
                               newNstart + old_nodes_num);   // copy_backward 3th param is result space end
        }
    } else {
        size_type new_map_size = map_size + fmax(map_size, node_to_add) + 2;
        Map_pointer new_map = allocate_map(new_map_size);
        newNstart = new_map + (new_map_size - new_nodes_num) / 2 + (addFromFront ? node_to_add : 0);
        std::copy(start.map_node, finish.map_node, newNstart);
        deallocate_map(map, map_size);
        map = new_map;
        map_size = new_map_size;
    }

    start.set_node(newNstart);
    finish.set_node(newNstart + old_nodes_num - 1);
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::clear() {
    size_type bufferSize = deque_buf_size(0, BufSize);
    // need to save one map_node as buffer
    for (Map_pointer node = start.map_node + 1; node < finish.node_map; ++node) {
        destroy(*node, *node + bufferSize);
        deallocate_node(*node);
    }


    if (start.map_node != finish.map_node) {
        // at least have headBuffer and tailBuffer,remove tailBuffer
        destroy(start.cur, start.last);
        destroy(finish.start, finish.cur);
        deallocate_node(finish.first);
    } else {
        destroy(start.cur, finish.cur);
    }

    finish = start;
}

#endif //BETHSTL_DEQUE_H
