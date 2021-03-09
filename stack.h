//
// Created by Hemingbear on 2021/3/9.
//

#ifndef BETHSTL_STACK_H
#define BETHSTL_STACK_H

#include "deque.h"

template<class T, class Sequence = deque<T> >
class stack {
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

    size_type size() {
        return c.size();
    }

    reference top() {
        return c.back();
    }

    const_reference top() const {
        return c.back();
    }

    void push(const value_type &value) {
        c.push_back(value);
    }

    void pop() {
        c.pop_back();
    }
};

template<class T, class Sequence>
inline bool operator==(const stack<T, Sequence> &x, const stack<T, Sequence> &y) {
    return x.c == y.c;
}

template<class T, class Sequence>
inline bool operator<(const stack<T, Sequence> &x, const stack<T, Sequence> &y) {
    return x.c < y.c;
}


#endif //BETHSTL_STACK_H
