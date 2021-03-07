//
// Created by Hemingbear on 2021/3/6.
//

#ifndef BETHSTL_LIST_H
#define BETHSTL_LIST_H

#include <cstdio>
#include <cstddef>
#include "iterator.h"
#include "alloc.h"
#include "construct.h"


struct list_node_base {
    list_node_base *next;
    list_node_base *prev;
};


template<class T>
struct list_node {
    list_node *next;
    list_node *prev;
    T data;
};


template<class T, class Ref, class Ptr>
struct list_iterator {
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef bidirectional_iterator_tag iterator_category;

    typedef list_iterator<T, T &, T *> iterator;
    typedef list_iterator<T, const T &, const T *> const_iterator;
    typedef list_iterator<T, Ref, Ptr> _self;

    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef list_node<T> Node;
    list_node<T> *node;

    list_iterator(list_node<T> *x) : node(x) {}

    list_iterator(const list_node<T> *x) : node(x) {}

    list_iterator() {}


    void incr() { node = node->next; }

    void decr() { node = node->prev; }

    bool operator==(const list_iterator &x) const {
        return node == x.node;
    }

    bool operator!=(const list_iterator &x) const {
        return node != x.node;
    }

    reference operator*() const {
        return ((Node *) node)->data;
    }

    pointer operator->() const {
        return &(operator*());
    }

    _self &operator++() {
        this->incr();
        return *this;
    }

    _self operator++(int) {
        _self temp = *this;
        this->incr();
        return temp;
    }

    _self &operator--() {
        this->decr();
        return *this;
    }

    _self operator--(int) {
        _self temp = *this;
        this->decr();
        return temp;
    }
};

template<class T, class Alloc = STL_DEFAULT_ALLOCATOR>
class list {
public :
    typedef T value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef list_node<T> Node;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef Alloc allocator_type;

    typedef list_iterator<T, T &, T *> iterator;
    typedef list_iterator<T, const T &, const T *> const_iterator;

protected:

    Node *getNode() {
        return Alloc::allocate(1);
    }

    void *removeNode(list_node<T> *node) {
        Alloc::deallocate(node, 1);
    }

    Node *createNode(const T &value) {
        Node *p = getNode();
        try {
            construct(&(p->data));
        } catch (...) {
            removeNode(p);
            throw;
        }
        return p;
    }


    list_node<T> *endNode;

public:
    list() {
        endNode = getNode();
        endNode->next = endNode;
        endNode->prev = endNode;
    }

    list(size_type n, const T &value) {
        list();
        insert(begin(), n, value);
    }

    explicit list(size_type n) {
        list();
        insert(begin(), n, T());
    }

    list(const_iterator first, const_iterator last) {
        list();
        insert(begin(), first, last);
    }

    ~list() {
        clear();
        removeNode(endNode);
    }

    iterator begin() { return (Node *) (endNode->next); }

    const_iterator begin() const { return (Node *) (endNode->next); }

    iterator end() { return endNode; }

    const_iterator end() const { return endNode; }

    bool empty() const { return endNode->next == endNode; }

    size_type size() const {
        size_type result = 0;
        list_iterator<T, T &, T *> cur = begin();
        while (cur != end()) {
            cur++;
            result++;
        }
        return result;
    }

    size_type max_size() const {
        return size_type(-1);
    }

    reference front() { return *begin(); }

    const_reference front() const { return *begin(); }

    reference back() { return *(--end()); }

    const_reference back() const { return *(--end()); }

    void swap(list<T> &x) {
        std::swap(endNode, x.endNode);
    }

    iterator insert(iterator pos, const T &value) {
        Node *temp = createNode(value);
        temp->next = pos.node;
        temp->prev = pos.node->prev;
        pos.node->prev->next = temp;
        pos.node->prev = temp;
        return temp;
    }

    iterator insert(iterator pos) {
        return insert(pos, T());
    }

    void insert(iterator pos, const T *first, const T *last);

    void insert(iterator pos, const_iterator first, const_iterator last);

    void insert(iterator pos, size_type n, const T &value);

    void push_front(const T &value) { insert(begin(), value); }

    void push_front() { insert(begin()); }

    void push_back(const T &value) { insert(end(), value); }

    void push_back() { insert(end()); }

    iterator erase(iterator pos) {
        list_node<T> *p = pos.node->prev;
        list_node<T> *n = pos.node->next;
        p->next = n;
        n->prev = p;
        destroy(pos.node->data);
        removeNode(pos.node);
        return iterator((Node *) n);
    }

    iterator erase(iterator first, iterator last);

    void clear();

    void resize(size_type newSize, const T &value);

    void resize(size_type newSize);

    void pop_front() { erase(begin()); }

    void pop_back() {
        iterator temp = end();
        erase(--temp);
    }

protected:
    // move [first,last) nodes from old position to new position(before pos)
    void transfer(iterator pos, iterator first, iterator last) {
        if (pos == last) return;
        // firstly, change ->prev->next  and ->next->prev
        // then , change ->prev and ->next
        last.node->prev->next = pos.node;
        first.node->prev->next = last.node;
        pos.node->prev->next = first.node;

        list_node<T> *temp = pos.node->prev;
        pos.node->prev = last.node->prev;
        last.node->prev = first.node.prev;
        first.node->prev = temp;
    }

public:
    void splice(iterator pos, list &x) {
        if (!x.empty()) {
            transfer(pos, x.begin(), x.end());
        }
    }

    void splice(iterator pos, list &, iterator i) {
        iterator j = i;
        ++j;
        if (pos == i || pos == j) return;
        this->transfer(pos, i, j);
    }

    void remove(const T &value);

    void unique();

    void merge(list &x);

    void reverse();

    void sort();

    list &operator=(const list<T, Alloc> &x);

    void fill_assign(size_type n, const T &value);
};

template<class T, class Alloc>
inline bool operator==(const list<T, Alloc> &x, const list<T, Alloc> &y) {
    typedef typename list<T, Alloc>::const_iterator const_iterator;
    const_iterator cur1 = x.begin();
    const_iterator cur2 = y.begin();
    const_iterator end1 = x.end();
    const_iterator end2 = y.end();

    while (cur1 != end1 && cur2 != end2 && *cur1 == *cur2) {
        ++cur1;
        ++cur2;
    }

    return cur1 == end1 && cur2 == end2;
}

template<class T, class Alloc>
void list<T, Alloc>::insert(iterator pos, const T *first, const T *last) {
    for (; first != last; ++first) {
        insert(pos, *first);
    }
}

template<class T, class Alloc>
void list<T, Alloc>::insert(iterator pos, const_iterator first, const_iterator last) {
    for (; first != last; ++first) {
        insert(pos, *first);
    }
}

template<class T, class Alloc>
void list<T, Alloc>::insert(iterator pos, size_type n, const T &value) {
    for (int i = 0; i < n; ++i) {
        insert(pos, value);
    }
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator first, iterator last) {
    while (first != last) {
        erase(first++);
        return last;
    }
}

template<class T, class Alloc>
void list<T, Alloc>::resize(size_type newSize, const T &value) {
    size_type len = 0;
    iterator cur = begin();
    for (; cur != end() && len < newSize; ++len, ++cur);
    if (len == newSize) {
        erase(cur, end());
    } else {
        insert(end(), newSize - len, value);
    }
}

template<class T, class Alloc>
list<T, Alloc> &list<T, Alloc>::operator=(const list<T, Alloc> &x) {
    if (this != x) {
        iterator cur1 = begin();
        iterator last1 = end();
        const_iterator cur2 = x.begin();
        const_iterator last2 = x.end();

        while (cur1 != last1 && cur2 != last2) {
            *cur1++ = *cur2++;
        }
        if (cur2 == last2) {
            erase(cur1, last1);
        } else {
            insert(cur1, cur2, last2);
        }
    }
    return *this;
}

template<class T, class Alloc>
void list<T, Alloc>::fill_assign(size_type n, const T &value) {
    iterator cur = begin();
    for (; cur != end() && n > 0; --n) {
        *cur++ = value;
    }
    if (n > 0) {
        insert(end(), n, value);
    } else {
        erase(cur, end());
    }
}

template<class T, class Alloc>
void list<T, Alloc>::remove(const T &value) {
    iterator cur = begin();
    iterator last = end();
    while (cur != last) {
        iterator temp = cur;
        ++temp;
        if (value == *cur) {
            erase(cur);
        }
        cur = temp;
    }
}

template<class T, class Alloc>
void list<T, Alloc>::unique() {
    iterator cur = begin();
    iterator last = end();
    if (cur == last) return;
    iterator next = cur;
    while (++next != last) {
        if (*cur == *next) {
            erase(next);
        } else {
            cur = next;
            next = cur;
        }
    }
}

template<class T, class Alloc>
void list<T, Alloc>::merge(list<T, Alloc> &x) {
    iterator cur1 = begin();
    iterator last1 = end();
    iterator cur2 = x.begin();
    iterator last2 = x.end();

    while (cur1 != last1 && cur2 != last2) {
        if (*cur2 < *cur1) {
            iterator temp = cur2;
            transfer(cur1, cur2, ++temp);
            cur2 = temp;
        } else {
            ++cur1;
        }
    }
    if (cur2 != last2) transfer(last1, cur2, last2);
}

template<class T, class Alloc>
void list<T, Alloc>::reverse() {
    iterator cur = begin();
    iterator last = end();
    if (cur == last || cur.node->next == last.node) return;
    ++cur;
    while (cur != last) {
        iterator temp = cur;
        transfer(begin(), cur, ++temp);
        cur = temp;
    }
}

// list can't use std::sort, use its own sort version.
// 1.carry get 1 element from list, and transfer to counter[0].
// 2.carry get 1 element from list and merge with counter[0],then transfer to count[1] (2 elements).
// 3.repeat counter[0] and merge count[1],then transfer to count[2] (4 elements)
// 4. go on and on and on...
// 5.util traverse all element from list, then merge count[0] to count[fill-1] ,swap back to list.
template<class T, class Alloc>
void list<T, Alloc>::sort() {
    if (endNode->next == endNode || endNode->next->next == endNode) return;
    list<T, Alloc> carry;
    list<T, Alloc> counter[64];  // most hold 2^64 elements;
    int fill = 0;
    while (!empty()) {
        carry.splice(carry.begin(), this, begin());   // get one element from list to carry
        int i = 0;
        while (i < fill && !counter[i].empty()) {
            counter[i].merge(carry);    // move to counter[i],counter[i] can hold 2^i elements;
            carry.swap(counter[i + 1]);
        }
        carry.swap(counter[i]);
        if (i == fill) ++fill;
    }

    for (int i = 1; i < fill; ++i)
        counter[i].merge(counter[i - 1]);     //merge all data
    swap(counter[fill - 1]);
}

#endif //BETHSTL_LIST_H
