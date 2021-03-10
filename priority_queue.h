//
// Created by Hemingbear on 2021/3/10.
//

#ifndef BETHSTL_PRIORITY_QUEUE_H
#define BETHSTL_PRIORITY_QUEUE_H

#include "vector.h"
#include "heap.h"

template<class T, class Sequence = vector<T>, class Compare = std::less<typename Sequence::value_type> >
class priority_queue {
public:
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;
protected:
    Sequence c;
    Compare comp;
public:
    priority_queue() : c() {}

    explicit priority_queue(const Compare &x) : c(), comp(x) {}

    template<class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare &x)
            :c(first, last), comp(x) { make_heap(c.begin(), c.end()); }

    template<class InputIterator>
    priority_queue(InputIterator first, InputIterator last):c(first, last) {
        make_heap(c.begin(), c.end());
    }

    bool empty() const { return c.empty(); }

    size_type size() const { return c.size(); }

    const_reference top() const { return c.front(); }

    void push(const value_type &value) {
        try {
            c.push_back(value);
            push_heap(c.begin(), c.end());
        } catch (...) {
            c.clear();
        }
    }

    void pop() {
        try {
            pop_heap(c.begin(), c.end());
            c.pop_back();
        } catch (...) {
            c.clear();
        }
    }
};

#endif //BETHSTL_PRIORITY_QUEUE_H
