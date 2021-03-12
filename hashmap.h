//
// Created by Hemingbear on 2021/3/13.
//

#ifndef BETHSTL_HASHMAP_H
#define BETHSTL_HASHMAP_H

#include "hashtable.h"

template<class T>
struct Identity : public std::unary_function<T, T> {
    const T &operator()(const T &x) const { return x; }
};

template<class Key, class T, class HashFunc = std::hash<Key>, class EqualKey = std::equal_to<Key>, class Alloc = STL_DEFAULT_ALLOCATOR>
class hash_map {
private:
    typedef hash_table<std::pair<const Key, T>, Key, HashFunc, EqualKey, Alloc> ht;
    ht rep;

public:
    typedef typename ht::key_type key_type;
    typedef T data_type;
    typedef T mapped_type;
    typedef typename ht::value_type value_type;
    typedef typename ht::hasher hasher;
    typedef typename ht::key_equal key_equal;

    typedef typename ht::size_type size_type;
    typedef typename ht::difference_type difference_type;
    typedef typename ht::pointer pointer;
    typedef typename ht::const_pointer const_pointer;
    typedef typename ht::reference reference;
    typedef typename ht::const_reference const_reference;

    typedef typename ht::iterator iterator;
    typedef typename ht::const_iterator const_iterator;

    typedef typename ht::allocator_type allocator_type;

    hasher hash_function() const { return rep.hash(); }

    key_equal key_eq() const { return rep.key_equal(); }

    hash_map() : rep(100, hasher(), key_equal(), allocator_type()) {}

    explicit hash_map(size_type n)
            : rep(n, hasher(), key_equal(), allocator_type()) {}

    hash_map(size_type n, const hasher &hf)
            : rep(n, hf, key_equal(), allocator_type()) {}

    hash_map(size_type n, const hasher &hf, const key_equal &eql,
             const allocator_type &a = allocator_type())
            : rep(n, hf, eql, a) {}

    size_type size() const { return rep.size(); }

    size_type max_size() const { return rep.max_size(); }

    bool empty() const { return rep.empty(); }

    void swap(hash_map &x) { rep.swap(x.rep); }

    iterator begin() { return rep.begin(); }

    iterator end() { return rep.end(); }

    const_iterator begin() const { return rep.begin(); }

    const_iterator end() const { return rep.end(); }

    std::pair<iterator, bool> insert(const value_type &value) {
        return rep.insert_unique(value);
    }

    template<class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        rep.insert_unique(first, last);
    }

    std::pair<iterator, bool> insert_noresize(const value_type &value) {
        return rep.insert_unique_noresize(value);
    }

    iterator find(const key_type &key) { return rep.find(key); }

    const_iterator find(const key_type &key) const { return rep.find(key); }

    T &operator[](const key_type &key) {
        return rep.find_or_insert(value_type(key, T())).second;
    }

    size_type count(const key_type &key) const { return rep.find(key); }

    std::pair<iterator, iterator> equal_range(const key_type &key) {
        return rep.equal_range(key);
    }

    std::pair<const_iterator, const_iterator> equal_range(const key_type &key) const {
        return rep.equal_range(key);
    }

    size_type erase(const key_type &key) { return rep.erase(key); }

    void erase(iterator it) { rep.erase(it); }

    void erase(iterator first, iterator last) { rep.erase(first, last); }

    void clear() { rep.clear(); }


};

#endif //BETHSTL_HASHMAP_H
