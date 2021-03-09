//
// Created by Hemingbear on 2021/3/9.
//

#ifndef BETHSTL_QUEUE_H
#define BETHSTL_QUEUE_H

#include "deque.h"

template<class T, class Sequence = deque<T> >
class queue {
public:
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;

protected:
    Sequence c;

public:
    bool empty() const {
        return c.empty();
    }

    size_type size() const {
        return c.size();
    }

    reference front() {
        return c.front();
    }

    const_reference front() const {
        return c.front();
    }

    reference back() {
        return c.back();
    }

    const_reference back() const {
        return c.back();
    }

    void push(const value_type &value) {
        c.push_back(value);
    }

    void pop() {
        c.pop_front();
    }
};

template<class T, class Sequence>
inline bool operator==(const queue<T, Sequence> &x, const queue<T, Sequence> &y) {
    return x.c == y.c;
}

template<class T, class Sequence>
inline bool operator<(const queue<T, Sequence> &x, const queue<T, Sequence> &y) {
    return x.c < y.c;
}

#endif //BETHSTL_QUEUE_H
