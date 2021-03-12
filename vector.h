//
// Created by Hemingbear on 2021/3/6.
//

#ifndef BETHSTL_VECTOR_H
#define BETHSTL_VECTOR_H

#include "alloc.h"
#include "construct.h"
#include "uninitialized.h"
#include <iostream>
#include <algorithm>

template<class T, class Alloc = STL_DEFAULT_ALLOCATOR>
class vector {
public:
    typedef T value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef value_type *iterator;
    typedef const value_type *const_iterator;

protected:
    typedef simpleAlloc<T, Alloc> data_allocator;
    iterator start;
    iterator finish;
    iterator end_of_storage;

    void insert_aux(iterator pos, const T &x);

    void insert_aux(iterator pos);

    void insert_aux(iterator pos, size_type n, const T &x);

    void deallocate() {
        if (start) data_allocator::deallocate(start, end_of_storage - start);
    }

    void fill_initialize(size_type n, const T &value) {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

public:
    iterator begin() { return start; }

    iterator end() { return finish; }

    size_t size() const { return (size_t) (end() - begin()); }

    size_t capacity() const { return (size_t) (end_of_storage - begin()); }

    bool empty() const { return end() == begin(); }

    reference operator[](size_t n) { return *(begin() + n); }

    const_reference operator[](size_t n) const { return *(begin() + n); }

    vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) {}

    vector(size_t n, const T &value) { fill_initialize(n, value); }

    vector(int n, const T &value) { fill_initialize(n, value); }

    vector(long n, const T &value) { fill_initialize(n, value); }

    explicit vector(size_t n) { fill_initialize(n, T()); }

    ~vector() {
        destroy(start, finish);
        deallocate();
    }

    reference front() { return *begin(); }

    reference back() { return *(end() - 1); }

    void push_back(const T &value) {
        if (finish != end_of_storage) {
            construct(finish, value);
            ++finish;
        } else {
            insert_aux(end(), value);
        }
    }

    void pop_back() {
        --finish;
        destroy(finish);
    }

    iterator erase(iterator pos) {
        if (pos != (end() - 1)) {
            std::copy(pos + 1, finish, pos);
        }
        --finish;
        destroy(finish);
        return pos;
    }

    iterator erase(iterator first, iterator last) {
        iterator head = std::copy(last, finish, first);
        destroy(head, finish);
        finish = finish - (last - first);
        return first;
    }

    void resize(size_type newSize, const T &value) {
        if (newSize > size()) {
            erase(begin() + newSize, end());
        } else {
            insert(end(), newSize - size(), value);
        }
    }

    void resize(size_type newSize) {
        return resize(newSize, T());
    }

    void reserve(size_type __n) {
        if (capacity() < __n) {
            const size_type __old_size = size();
            iterator __tmp = allocate_and_copy(__n, start, finish);
            destroy(start, finish);
            deallocate(start, end_of_storage - start);
            start = __tmp;
            finish = __tmp + __old_size;
            end_of_storage = start + __n;
        }
    }

    void clear() { erase(begin(), end()); }

    iterator insert(iterator pos, const T &value) {
        size_type n = pos - begin();
        if (finish != end_of_storage && pos == end()) {
            construct(finish, value);
            ++finish;
        } else {
            insert_aux(pos, value);
        }

        return (begin() + n);
    }

    iterator insert(iterator pos) {
        size_type n = pos - begin();
        if (finish != end_of_storage && pos == end()) {
            construct(pos);
            ++finish;
        } else {
            insert_aux(pos, T());
        }

        return (begin() + n);
    }

protected:
    iterator allocate_and_fill(size_type n, const T &value) {
        iterator result = data_allocator::allocate(n);
        uninitialized_fill_n(begin(), n, value);
        return result;
    }
};

template<class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator pos, const T &value) {
    if (finish != end_of_storage) {
        construct(finish, *(finish - 1));
        finish++;
        std::copy_backward(pos, finish - 2, finish - 1);
        T copy = value;
        *pos = copy;
    } else {
        const size_type oldSize = size();
        const size_type newSize = oldSize == 0 ? 1 : 2 * oldSize;
        iterator newStart = data_allocator::allocate(newSize);
        iterator newFinish = newStart;
        try {
            newFinish = uninitialized_copy(start, pos, newStart);
            construct(newFinish, value);
            ++newFinish;
            newFinish = uninitialized_copy(pos, finish, newFinish);
        } catch (...) {
            // roll back all operation
            destroy(newStart, newFinish);
            data_allocator::deallocate(newStart, newSize);
            throw;
        }

        destroy(start, finish);
        deallocate();

        start = newStart;
        finish = newFinish;
        end_of_storage = start + newSize;
    }
}

template<class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator pos) {
    insert(pos, T());
}

template<class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator pos, size_t n, const T &value) {
    if (n <= 0) return;
    T copy = value;
    if ((size_t) (end_of_storage - finish) >= n) {
        const size_type elemAfterPos = finish - pos;
        iterator oldFinish = finish;
        // how to move element depend on which function is best match
        // If this space haven't been initialized, invoke uninitialized_copy(fill) rather than std::copy(fill)
        // and invoke copy rather than fill when there still have old value to use.
        if (elemAfterPos > n) {
            uninitialized_copy(finish - n, finish, finish);
            finish += n;
            std::copy_backward(pos, oldFinish - n, oldFinish);
            std::fill(pos, pos + n, copy);
        } else {
            uninitialized_fill_n(finish, n - elemAfterPos, copy);
            finish += n - elemAfterPos;
            uninitialized_copy(pos, oldFinish, finish);
            finish += elemAfterPos;
            std::fill(pos, oldFinish, copy);
        }
    } else {
        const size_type oldSize = size();
        //const size_type newSize = oldSize == 0 ? 1 : 2 * oldSize;
        // set new size to old size + max(n,old size)
        const size_type newSize = oldSize + std::max(n, oldSize);
        iterator newStart = data_allocator::allocate(newSize);
        iterator newFinish = newStart;
        try {
            newFinish = uninitialized_copy(start, pos, newStart);
            newFinish = uninitialized_fill_n(newFinish, n, copy);
            newFinish = uninitialized_copy(pos, finish, newFinish);
        } catch (...) {
            destroy(newStart, newFinish);
            data_allocator::deallocate(newStart, newSize);
            throw;
        }

        destroy(start, finish);
        deallocate();
        start = newStart;
        finish = newFinish;
        end_of_storage = newStart + newSize;
    }
}


#endif //BETHSTL_VECTOR_H
