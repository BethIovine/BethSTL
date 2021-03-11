//
// Created by Hemingbear on 2021/3/11.
//


#ifndef BETHSTL_SET_H
#define BETHSTL_SET_H

#include <algorithm>
#include "alloc.h"
#include "tree.h"

template<class T>
struct Identity : public std::unary_function<T, T> {
    const T &operator()(const T &x) const { return x; }
};

template<class Key, class Compare = std::less<Key>, class Alloc = STL_DEFAULT_ALLOCATOR>
class set {
public:
    typedef Key key_type;
    typedef Key value_type;
    typedef Compare key_compare;
    typedef Compare value_compare;
protected:
    typedef rb_tree<key_type, value_type, Identity<value_type>, Alloc> rep_type;
    rep_type t;     //use rb_tree represent set
public:
    typedef typename rep_type::const_pointer pointer;
    typedef typename rep_type::const_pointer const_pointer;
    typedef typename rep_type::const_reference reference;
    typedef typename rep_type::const_reference const_reference;
    typedef typename rep_type::const_iterator iterator;
    typedef typename rep_type::const_iterator const_iterator;
    typedef typename rep_type::size_type size_type;
    typedef typename rep_type::difference_type difference_type;
    typedef typename rep_type::allocator_type allocator_type;

    set() : t(Compare()) {}

    explicit set(const Compare &comp) : t(comp) {}

    template<class InputIterator>
    set(InputIterator first, InputIterator last):t(Compare()) {
        t.insert_unique(first, last);
    }

    template<class InputIterator>
    set(InputIterator first, InputIterator last, const Compare &comp):t(comp) {
        t.insert_unique(first, last);
    }

    set(const set<Key, Compare, Alloc> &x) : t(x.t) {}

    set<Key, Compare, Alloc> &operator=(const set<Key, Compare, Alloc> &x) {
        t = x.t;
        return *this;
    }

    key_compare key_comp() const { return t.key_compare(); }

    value_compare value_comp() const { return t.key_compare(); }

    iterator begin() const { return t.begin(); }

    iterator end() const { return t.end(); }

    bool empty() const { return t.empty(); }

    size_type size() const { return t.size(); }

    size_type max_size() const { return t.max_size(); }

    void swap(set<Key, Compare, Alloc> &x) { t.swap(x.t); }

    typedef std::pair<iterator, bool> pair_iterator_bool;

    std::pair<iterator, bool> insert(const value_type &value) {
        std::pair<typename rep_type::iterator, bool> p = t.insert_unique(value);
        return std::pair<iterator, bool>(p.first, p.second);
    }

    std::pair<iterator, bool> insert(iterator pos, const value_type &value) {
        typedef typename rep_type::iterator rep_iterator;
        return t.insert_unique((rep_iterator &) pos, value);
    }

    template<class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        t.insert_unique(first, last);
    }

    void erase(iterator pos) {
        typedef typename rep_type::iterator rep_iterator;
        t.erase((rep_iterator &) pos);
    }

    size_type erase(const key_type &keyValue) {
        return t.erase(keyValue);
    }

    void erase(iterator first, iterator last) {
        typedef typename rep_type::iterator rep_iterator;
        t.erase((rep_iterator &) first, (rep_iterator &) last);
    }

    void clear() {
        t.clear();
    }

    iterator find(const key_type &x) const { return t.find(x); }

    size_type count(const key_type &x) const { return t.count(x); }

    iterator lower_bound(const key_type &x) const {
        return t.lower_bound(x);
    }

    std::pair<iterator, iterator> equal_range(const key_type &x) const {
        return t.equal_range(x);
    }

};

template<class Key, class Compare, class Alloc>
inline bool operator==(const set<Key, Compare, Alloc> &x, const set<Key, Compare, Alloc> &y) {
    return x.t == y.t;
}

template<class Key, class Compare, class Alloc>
inline bool operator<(const set<Key, Compare, Alloc> &x, const set<Key, Compare, Alloc> &y) {
    return x.t < y.t;
}

#endif //BETHSTL_SET_H
