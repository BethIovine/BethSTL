//
// Created by Hemingbear on 2021/3/11.
//



#ifndef BETHSTL_HASHTABLE_H
#define BETHSTL_HASHTABLE_H

#include "iterator.h"
#include "alloc.h"
#include "iterator.h"
#include "vector.h"
#include <algorithm>

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc = STL_DEFAULT_ALLOCATOR>
class hash_table;

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
struct hashtable_iterator;

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
struct hashtable_const_iterator;


template<class Value>
struct hashtable_node {
    hashtable_node *next;
    Value val;
};

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
struct hashtable_iterator {
    typedef hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> HashTable;
    typedef hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> iterator;
    typedef hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> const_iterator;
    typedef hashtable_node<Value> Node;

    typedef forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value &reference;
    typedef Value *pointer;

    Node *cur;
    HashTable *ht;

    hashtable_iterator(Node *n, HashTable *table) : cur(n), ht(table) {}

    hashtable_iterator() {}

    reference operator*() const { return cur->val; }

    pointer operator->() const { return &(operator*()); }

    iterator &operator++();

    iterator operator++(int);
};


template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
typename hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator &
hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::operator++() {
    const Node *old = cur;
    cur = cur->next;
    if (!cur) {
        size_type bucket = ht->bucket_num(old->val);
        while (!cur && ++bucket < ht->buckets.size())
            cur = ht->buckets[bucket];
    }
    return *this;
}

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
typename hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator
hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::operator++(int) {
    iterator temp = *this;
    ++*this;
    return temp;
}


static const int _stl_num_primes = 28;
static const unsigned long _stl_prime_list[_stl_num_primes] =
        {
                53ul, 97ul, 193ul, 389ul, 769ul,
                1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
                49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
                1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
                50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
                1610612741ul, 3221225473ul, 4294967291ul
        };

inline unsigned long __stl_next_prime(unsigned long __n) {
    const unsigned long *__first = _stl_prime_list;
    const unsigned long *__last = _stl_prime_list + (int) _stl_num_primes;
    const unsigned long *pos = std::lower_bound(__first, __last, __n);
    return pos == __last ? *(__last - 1) : *pos;
}

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
class hash_table {
public:
    typedef HashFunc hasher;
    typedef EqualKey key_equal;
    typedef size_t size_type;
    typedef Key key_type;
    typedef Value value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;

private:
    //three function object
    hasher hash;
    key_equal equals;
    ExtractKey get_key;

    typedef hashtable_node<Value> Node;
    typedef simpleAlloc<Node, Alloc> node_allocator;

    vector<Node *, Alloc> buckets;
    size_type num_elements;

    Node *get_node() { return node_allocator::allocate(1); }

    void put_node(Node *p) { node_allocator::deallocate(p, 1); }

public:
    size_type bucket_count() const { return buckets.size(); }

    size_type max_bucket_count() const { return _stl_prime_list[_stl_num_primes - 1]; }

    size_type elems_in_bucket(size_type bucket) const {
        size_type result = 0;
        for (Node *cur = buckets[bucket]; cur; cur = cur->next) {
            result += 1;
        }
        return result;
    }

    typedef hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> iterator;
    typedef hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> const_iterator;

    typedef Alloc allocator_type;

    allocator_type get_allocator() const { return allocator_type(); }

    friend struct
            hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    friend struct
            hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;

    hash_table(size_type n, const HashFunc &hf, const EqualKey &eqk, const ExtractKey &exk)
            : hash(hf), equals(eqk), get_key(exk), buckets(), num_elements(0) {
        initialize_buckets(num_elements);
    }

    hash_table(size_type n, const HashFunc &hf, const EqualKey &eqk)
            : hash(hf), equals(eqk), buckets(), num_elements(0) {
        initialize_buckets(num_elements);
    }

    hash_table(const hash_table &x)
            : hash(x.hash), equals(x.equals), get_key(x.get_key), buckets(x.buckets), num_elements(0) {
        copy_from(x);
    }

    hash_table &operator=(const hash_table &x) {
        if (&x != this) {
            clear();
            hash = x.hash;
            equals = x.equals;
            get_key = x.get_key;
            copy_from(x);
        }
        return *this;
    }

    ~hash_table() {
        clear();
    }

    size_type size() const { return num_elements; }

    size_type max_size() const { return size_type(-1); }

    bool empty() const { return 0 == size(); }

    void swap(const hash_table &x) {
        std::swap(hash, x.hash);
        std::swap(equals, x.equals);
        std::swap(get_key, x.get_key);
        buckets.swap(x.buckets);
        std::swap(num_elements, x.num_elements);
    }

    iterator begin() {
        for (size_type i = 0; i < buckets.size(); ++i) {
            if (buckets[i]) {
                return iterator(buckets[i], this);
            }
            return end();
        }
    }

    const_iterator begin() const {
        for (size_type i = 0; i < buckets.size(); ++i) {
            if (buckets[i]) {
                return iterator(buckets[i], this);
            }
            return end();
        }
    }

    iterator end() {
        return iterator(nullptr, this);
    }

    const_iterator end() const {
        return iterator(nullptr, this);
    }

    iterator insert_equal(const value_type &value) {
        resize(num_elements + 1);
        return insert_equal_noresize(value);
    }

    std::pair<iterator, bool> insert_unique(const value_type &value) {
        resize(num_elements + 1);
        return insert_unique_noresize(value);
    }

    iterator insert_equal_noresize(const value_type &value);

    std::pair<iterator, bool> insert_unique_noresize(const value_type &value);

    void insert_unique(const value_type *x, const value_type *y) {
        size_type n = y - x;
        resize(num_elements + n);
        for (; n > 0; --n, ++x)
            insert_unique_noresize(*x);
    }

    void insert_equal(const value_type *x, const value_type *y) {
        size_type n = y - x;
        resize(num_elements + n);
        for (; n > 0; --n, ++x)
            insert_equal_noresize(*x);
    }

    void insert_unique(const_iterator x, const_iterator y) {
        size_type n = 0;
        distance(x, y, n);
        resize(num_elements + n);
        for (; n > 0; --n, ++x) {
            insert_unique_noresize(*x);
        }
    }

    void insert_equal(const_iterator x, const_iterator y) {
        size_type n = 0;
        distance(x, y, n);
        resize(num_elements + n);
        for (; n > 0; --n, ++x) {
            insert_equal_noresize(*x);
        }
    }

    reference find_or_insert(const value_type &value);

    iterator find(const key_type &key) {
        size_type n = bkt_num_key(key);
        Node *first;
        for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next) {}
        return iterator(first, this);
    }

    const_iterator find(const key_type &key) const {
        size_type n = bkt_num_key(key);
        Node *first;
        for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next) {}
        return iterator(first, this);
    }

    size_type count(const key_type &key) const {
        const size_type n = bkt_num_key(key);
        size_type result = 0;
        for (const Node *cur = buckets[n]; cur; cur = cur->next) {
            if (equals(get_key(cur->val), key))
                ++result;
        }
        return result;
    }

    std::pair<iterator, iterator> equal_range(const key_type &key);

    std::pair<const_iterator, const_iterator> equal_range(const key_type &key) const;

    size_type erase(const key_type &pos);

    void erase(const iterator &pos);

    void erase(iterator first, iterator last);

    void erase(const const_iterator &pos);

    void erase(const_iterator first, const_iterator last);

    void resize(size_type num_elements_need);

    void clear();


private:
    size_type next_size(size_type n) const {
        return __stl_next_prime(n);
    }

    void initialize_buckets(size_type n) {
        const size_type bucketsNum = next_size(n);
        buckets.reserve(bucketsNum);
        buckets.insert(buckets.end(), bucketsNum, (Node *) nullptr);
        num_elements = 0;
    }

    size_type bkt_num_key(const key_type &key) const {
        return bkt_num_key(key, buckets.size());
    }

    size_type bkt_num_val(const value_type &value) const {
        return bkt_num_key(get_key(value));
    }

    size_type bkt_num_key(const key_type &key, size_t n) const {
        return hash(key) % n;
    }

    size_type bkt_num_val(const value_type &value, size_t n) const {
        return bkt_num_key(get_key(value), n);
    }

    Node *new_node(const value_type &value) {
        Node *node = get_node();
        node->next = 0;
        try {
            construct(&node->val, value);
            return node;
        } catch (...) {
            put_node(node);
        }
    }

    void delete_node(Node *node) {
        destroy(&node->val);
        put_node(node);
    }

    void erase_bucket(const size_type n, Node *first, Node *last);

    void erase_bucket(const size_type n, Node *last);

    void copy_from(const hash_table &x);

};

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
std::pair<typename hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator, bool>
hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::insert_unique_noresize(const value_type &value) {
    size_type bucket_index = bkt_num_val(value);
    Node *first = buckets[bucket_index];
    for (Node *cur = first; cur; cur = cur->next) {
        if (EqualKey(ExtractKey(value), ExtractKey(cur->val)))
            return std::pair<iterator, bool>(iterator(cur, this), false);
    }
    Node *newNode = new_node(value);
    newNode->next = first;
    buckets[bucket_index] = newNode;
    num_elements++;
    return std::pair<iterator, bool>(iterator(newNode, this), true);
}

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
typename hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator
hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::insert_equal_noresize(const value_type &value) {
    size_type bucket_index = bkt_num_val(value);
    Node *first = buckets[bucket_index];
    for (Node *cur = first; cur; cur = cur->next) {
        if (EqualKey(ExtractKey(value), ExtractKey(cur->val))) {
            Node *newNode = new_node(value);
            newNode->next = cur->next;
            cur->next = newNode;
            num_elements++;
            return iterator(newNode, this);
        }
    }
    Node *newNode = new_node(value);
    newNode->next = first;
    buckets[bucket_index] = newNode;
    num_elements++;
    return iterator(newNode, this);
}

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
typename hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::reference
hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::find_or_insert(const value_type &value) {
    resize(num_elements + 1);

    size_type bucket_index = bkt_num_val(value);
    Node *first = buckets[bucket_index];

    for (Node *cur = first; cur; cur = cur->next) {
        if (EqualKey(ExtractKey(value), ExtractKey(cur->val)))
            return cur->val;
    }

    Node *newNode = new_node(value);
    newNode->next = first;
    buckets[bucket_index] = newNode;
    num_elements++;
    return newNode->val;
}

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
std::pair<typename hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator,
        typename hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator>
hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::equal_range(const key_type &key) {
    typedef std::pair<iterator, iterator> _Pii;
    const size_type bucket_index = bkt_num_key(key);

    for (Node *first = buckets[bucket_index]; first; first = first->next) {
        if (EqualKey(ExtractKey(first->val), key)) {
            for (Node *cur = first->next; cur; cur = cur->next) {
                if (!EqualKey(ExtractKey(cur->val), key))
                    return _Pii(iterator(first, this), iterator(cur, this));
            }
            for (size_type n = bucket_index + 1; n < buckets.size(); ++n) {
                if (buckets[n]) return _Pii(iterator(first, this), iterator(buckets[n], this));
            }

            return _Pii(iterator(first, this), end());
        }
    }
    return _Pii(end(), end());
}

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
std::pair<typename hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::const_iterator,
        typename hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::const_iterator>
hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::equal_range(const key_type &key) const {
    typedef std::pair<const_iterator, const_iterator> _Pii;
    const size_type bucket_index = bkt_num_key(key);

    for (Node *first = buckets[bucket_index]; first; first = first->next) {
        if (EqualKey(ExtractKey(first->val), key)) {
            for (Node *cur = first->next; cur; cur = cur->next) {
                if (!EqualKey(ExtractKey(cur->val), key))
                    return _Pii(const_iterator(first, this), const_iterator(cur, this));
            }
            for (size_type n = bucket_index + 1; n < buckets.size(); ++n) {
                if (buckets[n]) return _Pii(const_iterator(first, this), const_iterator(buckets[n], this));
            }

            return _Pii(const_iterator(first, this), end());
        }
    }
    return _Pii(end(), end());
}


template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
typename hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::size_type
hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase(const key_type &key) {
    const size_type bucket_index = bkt_num_key(key);
    Node *first = buckets[bucket_index];
    size_type eraseNum = 0;

    if (first) {
        Node *cur = first;
        Node *next = cur->next;
        while (next) {
            if (EqualKey(key, ExtractKey(next->val))) {
                cur->next = next->next;
                delete_node(next);
                next = cur->next;
                eraseNum++;
                num_elements--;
            } else {
                cur = next;
                next = next->next;
            }
        }

        if (EqualKey(key, ExtractKey(first->val))) {
            buckets[bucket_index] = first->next;
            delete_node(first);
            ++eraseNum;
            --num_elements;
        }
    }

    return eraseNum;
}

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
void hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase(const iterator &pos) {
    Node *cur = pos.cur;
    if (cur) {
        const size_type bucket_index = bkt_num_val(cur->val);
        Node *first = buckets[bucket_index];
        if (cur == first) {
            buckets[bucket_index] = cur->next;
            delete_node(cur);
            --num_elements;
        } else {
            Node *next = first->next;
            while (next) {
                if (next == cur) {
                    first->next = next->next;
                    delete_node(next);
                    --num_elements;
                    break;
                } else {
                    cur = next;
                    next = cur->next;
                }
            }
        }
    }
}


template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
void hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase(iterator first, iterator last) {
    size_type f_bucket_index = first.cur ? bkt_num_val(first.cur->val) : buckets.size();
    size_type l_bucket_index = last.cur ? bkt_num_val(last.cur->val) : buckets.size();

    if (first.cur == last.cur) return;
    else if (f_bucket_index = l_bucket_index) {
        erase_bucket(f_bucket_index, first.cur, last.cur);
    } else {
        erase_bucket(f_bucket_index, first.cur, nullptr);
        for (size_type n = f_bucket_index + 1; n < l_bucket_index; ++n) {
            erase_bucket(n, nullptr);
        }
        if (l_bucket_index != buckets.size())
            erase_bucket(l_bucket_index, last.cur);
    }
}

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
inline void
hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase(const_iterator first, const_iterator last) {
    erase(iterator(const_cast<Node *>(first.cur), const_cast<hash_table *>(first.ht)),
          iterator(const_cast<Node *>(last.cur), const_cast<hash_table *>(last.ht)));
}

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
void hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase(const const_iterator &pos) {
    erase(iterator(const_cast<Node *>(pos.cur), const_cast<hash_table *>(pos.ht));
}

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
void hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::resize(size_type num_elements_need) {
    const size_type old_size = buckets.size();
    if (num_elements_need <= old_size) return;

    size_type new_size = __stl_next_prime(num_elements_need);
    if (new_size <= old_size) return;
    vector<Node *, Alloc> new_buckets(new_size, (Node *) nullptr);
    try {
        for (size_type i = 0; i < buckets.size(); ++i) {
            Node *first = buckets[i];
            while (first) {
                size_type new_bucket_index = bkt_num_val(first->val, new_size);
                buckets[i] = first->next;
                first->next = new_buckets[new_bucket_index];
                new_buckets[new_bucket_index] = first;
                first = buckets[i];
            }
        }
        buckets.swap(new_buckets);
    } catch (...) {
        for (int i = 0; i < new_buckets.size(); ++i) {
            while (new_buckets[i]) {
                Node *next = new_buckets[i]->next;
                delete_node(new_buckets[i]);
                new_buckets[i] = next;
            }
        }
        throw;
    }
}


template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
void hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase_bucket(const size_type n, Node *first,
                                                                                 Node *last) {
    Node *cur = buckets[n];
    if (cur == first) {
        erase_bucket(n, last);
    } else {
        Node *next;
        for (next = cur->next; next != first; cur = next, next = cur->next);
        while (next != last) {
            cur->next = next->next;
            delete_node(next);
            next = cur->next;
            --num_elements;
        }
    }
}

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
void hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase_bucket(const size_type n, Node *last) {
    Node *cur = buckets[n];
    while (cur != last) {
        Node *next = cur->next;
        delete_node(cur);
        cur = next;
        buckets[n] = cur;
        --num_elements;
    }
}

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
void hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::clear() {
    for (size_type i = 0; i < buckets.size(); ++i) {
        Node *cur = buckets[i];
        while (cur) {
            Node *next = cur->next;
            delete_node(cur);
            cur = next;
        }
        buckets[i] = nullptr;
    }
    num_elements = 0;
}

template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc>
void hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::copy_from(
        const hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> &x) {
    buckets.clear();
    buckets.reserve(x.buckets.size());
    buckets.insert(buckets.end(), x.buckets.size(), (Node *) nullptr);
    try {
        for (size_type i = 0; i < x.buckets.size(); ++i) {
            const Node *cur = x.buckets[i];
            if (cur) {
                Node *copy = new_node(cur->val);
                buckets[i] = copy;

                for (Node *next = cur->next; next; cur = next, next = cur->next) {
                    copy->next = new_node(next->val);
                    copy = copy->next;
                }

            }
        }
        num_elements = x.num_elements;
    } catch (...) {
        clear();
    }
}


#endif //BETHSTL_HASHTABLE_H
