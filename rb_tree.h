//
// Created by Hemingbear on 2021/3/10.
//

#ifndef BETHSTL_RB_TREE_H
#define BETHSTL_RB_TREE_H

#include "iterator.h"
#include "alloc.h"
#include "construct.h"

typedef bool _rb_tree_color_type;
const _rb_tree_color_type _rb_tree_red = false;
const _rb_tree_color_type _rb_tree_black = true;

struct _rb_tree_node_base {
    typedef _rb_tree_color_type color_type;
    typedef _rb_tree_node_base *base_ptr;

    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    static base_ptr minimum(base_ptr x) {
        while (x->left != nullptr) x = x->left;
        return x;
    }

    static base_ptr maximum(base_ptr x) {
        while (x->right != nullptr) x = x->right;
        return x;
    }
};

template<class T>
struct _rb_tree_node : public _rb_tree_node_base {
    typedef _rb_tree_node<T> *link_type;
    T value_field;
};

struct _rb_tree_iterator_base {
    typedef _rb_tree_node_base::base_ptr base_ptr;
    typedef bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;

    base_ptr node;

    void increment() {
        if (node->right != nullptr) {
            node = node->right;
            while (node->left != nullptr) node = node->left;
        } else {
            base_ptr y = node->parent;
            // node is rightChild,constantly go up until not a right node
            while (node == y->right) {
                node = y;
                y = y->parent;
            }
            if (node->right != y) node = y;
        }
    }

    void decrement() {
        if (node->color == _rb_tree_red && node->parent->parent == node) {
            node = node->right;
        } else if (node->left != 0) {
            base_ptr y = node->left;
            while (y->right != 0)
                y = y->right;
            node = y;
        } else {
            base_ptr y = node->parent;
            while (node == y->left) {
                node = y;
                y = y->parent;
            }
            node = y;
        }
    }
};

template<class T, class Ref, class Ptr>
struct _rb_tree_iterator : public _rb_tree_iterator_base {
    typedef T value_type;
    typedef Ref reference;
    typedef Ptr pointer;
    typedef _rb_tree_iterator<T, T &, T *> iterator;
    typedef _rb_tree_iterator<T, const T &, const T *> const_iterator;
    typedef _rb_tree_iterator<T, Ref, Ptr> _self;
    typedef _rb_tree_node<T> *link_type;

    _rb_tree_iterator() {}

    _rb_tree_iterator(link_type x) { node = x; }

    _rb_tree_iterator(const iterator &x) { node = x.node; }

    reference operator*() const {
        return link_type(node)->value_field;
    }

    pointer operator->() const {
        return &(operator*());
    }

    _self &operator++() {
        increment();
        return *this;
    }

    _self &operator++(int) {
        _self temp = *this;
        increment();
        return temp;
    }

    _self &operator--() {
        decrement();
        return *this;
    }

    _self &operator--(int) {
        _self temp = *this;
        decrement();
        return temp;
    }
};

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc =STL_DEFAULT_ALLOCATOR>
class rb_tree {
protected:
    typedef void *void_pointer;
    typedef _rb_tree_node_base *base_ptr;
    typedef _rb_tree_node<Value> rb_tree_node;
    typedef simpleAlloc<rb_tree_node, Alloc> rb_tree_node_allocator;
    typedef _rb_tree_color_type color_type;
public:
    typedef Key key_type;
    typedef Value value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef rb_tree_node *link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
protected:
    link_type get_node() { return rb_tree_node_allocator::allocate(); }

    void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

    link_type create_node(const value_type &value) {
        link_type temp = get_node();
        try {
            construct(&temp->value_field, value);
        } catch (...) {
            put_node(temp);
        }
    }

    link_type clone_node(link_type x) {
        link_type temp = create_node(x->value_field);
        temp->color = x->color;
        temp->left = nullptr;
        temp->right = nullptr;
        return temp;
    }

    void destroy_node(link_type p) {
        destroy(&p->value_field);
        put_node(p);
    }

protected:
    size_type node_count;
    link_type header;
    Compare key_compare;

    link_type &root() const { return (link_type &) header->parent; }

    link_type &leftmost() const { return (link_type &) header->left; }

    link_type &rightmost() const { return (link_type &) header->right; }


    static link_type &left(link_type x) {
        return (link_type &) (x->left);
    }

    static link_type &right(link_type x) {
        return (link_type &) (x->right);
    }

    static link_type &parent(link_type x) {
        return (link_type &) (x->parent);
    }

    static reference value(link_type x) {
        return x->value_field;
    }

    // const return value forbid immediately change
    static const Key &key(link_type x) {
        return (KeyOfValue) (value(x));
    }

    static color_type &color(link_type x) {
        return (color_type &) (x->color);
    }

    static link_type &left(base_ptr x) {
        return (link_type &) (x->left);
    }

    static link_type &right(base_ptr x) {
        return (link_type &) (x->right);
    }

    static link_type &parent(base_ptr x) {
        return (link_type &) (x->parent);
    }

    static reference value(base_ptr x) {
        return ((link_type) x)->value_field;
    }

    // const return value forbid immediately change
    static const Key &key(base_ptr x) {
        return (KeyOfValue) (value(link_type(x)));
    }

    static color_type &color(base_ptr x) {
        return (color_type &) (link_type(x)->color);
    }

    static link_type minimum(link_type x) {
        return (link_type) _rb_tree_node_base::minimum(x);
    }

    static link_type maximum(link_type x) {
        return (link_type) _rb_tree_node_base::maximum(x);
    }

public:
    typedef _rb_tree_iterator<value_type, reference, pointer> iterator;
private:
    iterator _insert(base_ptr x, base_ptr y, const value_type &value);

    link_type _copy(link_type x, link_type p);

    void _erase(link_type x);


    void init() {
        header = get_node();
        color(header) = _rb_tree_red;

        root() = nullptr;
        leftmost() = header;
        rightmost() = header;
    }

public:
    rb_tree(const Compare &comp = Compare()) : node_count(0), key_compare(comp) { init(); }

    ~rb_tree() {
        clear();
        put_node(header);
    }

    void clear();

    rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &
    operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &x);

    Compare key_comp() const { return key_compare; }

    iterator begin() { return leftmost(); }

    iterator end() { return header; }

    bool empty() const { return node_count; }

    size_type size() const { return node_count; }

    size_type most_size() const { return size_type(-1); }


public:
    std::pair<iterator, bool> insert_unique(const value_type &x);

    iterator insert_equal(const value_type &x);

};

void _rb_tree_rotate_left(_rb_tree_node_base *pBase, _rb_tree_node_base *&pBase1);

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const value_type &v) {
    link_type y = header;
    link_type x = root();
    while (x != nullptr) {
        y = x;
        x = key_compare((KeyOfValue) (v), key(x)) ? left(x) : right(x);
    }
    return _insert(x, y, v);
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const value_type &v) {
    link_type y = header();
    link_type x = root();
    bool comp = true;
    while (x != nullptr) {
        y = x;
        comp = key_compare((KeyOfValue) (v), key(x));
        x = comp ? left(x) : right(x);
    }

    iterator j = iterator(y);
    if (comp)
        if (j == begin())
            return std::pair<iterator, bool>(_insert(x, y, v), true);
        else
            --j;

    if (key_compare(key(j.node), KeyOfValue()(v)))
        return std::pair<iterator, bool>(_insert(x, y, v), true);

    return std::pair<iterator, bool>(j, false);

}


template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_insert(base_ptr x_, base_ptr y_, const value_type &value) {
    link_type x = (link_type) x_;
    link_type y = (link_type) y_;
    link_type z;
    if (y == header || x != 0 || key_compare(KeyOfValue()(value), key(y))) {
        z = create_node(value);
        left(y) = z;
        if (y == header) {
            root() = z;
            rightmost() = z;
        } else if (y == leftmost()) {
            leftmost() = z;
        }
    } else {
        z = create_node(value);
        right(y) = z;
        if (y == rightmost())
            rightmost() = z;
    }
    parent(z) = y;
    left(z) = nullptr;
    right(z) = nullptr;
    _rb_tree_rebalance(z, header->parent);
    ++node_count;
    return iterator(z);
}

inline void _rb_tree_rotate_left(_rb_tree_node_base *x, _rb_tree_node_base *&root) {
    _rb_tree_node_base *y = x->right;
    x->right = y->left;
    if (y->left != 0) {
        y->left->parent = x;
    }
    y->parent = x->parent;

    if (x == root) {
        root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

inline void _rb_tree_rotate_right(_rb_tree_node_base *x, _rb_tree_node_base *&root) {
    _rb_tree_node_base *y = x->left;
    x->left = y->right;
    if (y->right != 0)
        y->right->parent = x;
    y->parent = x->parent;

    if (x == root)
        root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

inline void _rb_tree_rebalance(_rb_tree_node_base *x, _rb_tree_node_base *&root) {
    x->color = _rb_tree_red;
    while (x != root && x->parent->color == _rb_tree_red) {
        if (x->parent == x->parent->parent->left) {
            _rb_tree_node_base *y = x->parent->parent->right;
            if (y && y->color == _rb_tree_red) {
                x->parent->color = _rb_tree_black;
                y->color = _rb_tree_red;
                x->parent->parent->color = _rb_tree_red;
                x = x->parent->parent;
            } else {
                if (x == x->parent->right) {
                    x = x->parent;
                    _rb_tree_rotate_left(x, root);
                }
                x->parent->color = _rb_tree_red;
                x->parent->parent->color = _rb_tree_red;
                _rb_tree_rotate_right(x->parent->parent, root);
            }
        } else {
            _rb_tree_node_base *y = x->parent->parent->left;
            if (y && y->color == _rb_tree_red) {
                x->parent->color = _rb_tree_black;
                y->color = _rb_tree_black;
                x->parent->parent->color = _rb_tree_red;
                x = x->parent->parent;
            } else {
                if (x == x->parent->left) {
                    x = x->parent;
                    _rb_tree_rotate_right(x, root);
                }
                x->parent->color = _rb_tree_black;
                x->parent->parent->color = _rb_tree_red;
                _rb_tree_rotate_left(x->parent->parent, root);
            }
        }
    }
    root->color = _rb_tree_black;
}


#endif //BETHSTL_RB_TREE_H