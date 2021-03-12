//
// Created by Hemingbear on 2021/3/13.
//

#ifndef BETHSTL_HASHSET_H
#define BETHSTL_HASHSET_H

#include "hashtable.h"

template<class T>
struct Identity : public std::unary_function<T, T> {
    const T &operator()(const T &x) const { return x; }
};

template<class Value, class HashFunc = std::hash<Value>, class EqualKey = std::equal_to<Value>, class Alloc = STL_DEFAULT_ALLOCATOR>
class hash_set {
private:
    typedef hash_table<Value, Value, HashFunc, Identity<Value>, EqualKey, Alloc> ht;
    ht rep;
public:
    typedef typename ht::key_type key_type;
    typedef typename ht::value_type value_type;
    typedef typename ht::hasher hasher;
    typedef typename ht::key_equal key_equal;

    typedef typename ht::size_type size_type;
    typedef typename ht::difference_type difference_type;
    typedef typename ht::const_pointer pointer;
    typedef typename ht::const_pointer const_pointer;
    typedef typename ht::const_reference reference;
    typedef typename ht::const_reference const_reference;

    typedef typename ht::const_iterator iterator;
    typedef typename ht::const_iterator const_iterator;

    typedef typename ht::allocator_type allocator_type;

    hasher hash_function() const { return rep.hash(); }

    key_equal key_eq() const { return rep.key_equal(); }

    //default set to 100 size
    hash_set() : rep(100, hasher(), key_equal()) {}

    explicit hash_set(size_type n) : rep(n, hasher(), key_equal()) {}

    hash_set(size_type n, const hasher &hf) : rep(n, hf, key_equal()) {}

    hash_set(size_type n, const hasher &hf, const key_equal &kq) : rep(n, hf, kq) {}

    template<class InputIterator>
    hash_set(InputIterator first, InputIterator last)
            :rep(100, hasher(), key_equal()) {
        rep.insert_unique(first, last);
    }

    size_type size() const { return rep.size(); }

    size_type max_size() const { return rep.max_size(); }

    bool empty() const { return rep.empty(); }

    void swap(hash_set &x) { rep.swap(x.rep); }

    iterator begin() const { return rep.begin(); }

    iterator end() const { return rep.end(); }

    std::pair<iterator, bool> insert(const value_type &value) {
        std::pair<typename ht::iterator, bool> p = rep.insert_unique(value);
        return std::pair<iterator, bool>(p.first, p.second);
    }

    template<class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        rep.insert_unique(first, last);
    }

    std::pair<iterator, bool> insert_noresize(const value_type &value) {
        std::pair<typename ht::iterator, bool> p = rep.insert_unique_noresize(value);
        return std::pair<iterator, bool>(p.first, p.second);
    }

    iterator find(const key_type &key) const { return rep.find(key); }

    size_type count(const key_type &key) const { return rep.count(key); }

    std::pair<iterator, iterator> equal_range(const key_type &key) const {
        return rep.equal_range(key);
    }

    size_type erase(const key_type &key) { return rep.erase(key); }

    void erase(iterator it) { rep.erase(it); }

    void erase(iterator first, iterator last) {
        rep.erase(first, last);
    }

    void clear() { rep.clear(); }
};

template<class Value, class HashFunc, class EqualKey, class Alloc>
inline bool operator==(const hash_set<Value, HashFunc, EqualKey, Alloc> &x,
                       const hash_set<Value, HashFunc, EqualKey, Alloc> &y) {
    return x.rep = y.rep;
}


#endif //BETHSTL_HASHSET_H
