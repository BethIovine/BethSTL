//
// Created by Hemingbear on 2021/3/10.
//

#ifndef BETHSTL_HEAP_H
#define BETHSTL_HEAP_H

// heap operation mostly base on vector container.

template<class RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
    push_heap_aux(first, last, size_type(first), value_type(first));
}

template<class RandomAccessIterator, class Distance, class T>
inline void push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance *, T *) {
    _push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
}

template<class RandomAccessIterator, class Distance, class T>
void _push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value) {
    Distance parentIndex = (holeIndex - 1) / 2;
    while (parentIndex >= topIndex && *(first + parentIndex) < value) {
        *(first + holeIndex) = *(first + parentIndex);
        holeIndex = parentIndex;
        parentIndex = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;
}

template<class RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
    pop_heap_aux(first, last, value_type(first));
}

template<class RandomAccessIterator, class T>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, T *) {
    _pop_heap(first, last - 1, last - 1, T(*(last - 1)), size_type(first));
}

template<class RandomAccessIterator, class T, class Distance>
inline void
_pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value, Distance *) {
    *result = *first; // set back value to back() , so back() is wanted value;
    _adjust_heap(first, Distance(0), Distance(last - first), value);
}

template<class RandomAccessIterator, class Distance, class T>
void _adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value) {
    Distance topIndex = holeIndex;
    Distance secondChild = holeIndex * 2 + 2;
    while (secondChild < len) {
        if (*(first + secondChild - 1) > *(first + secondChild)) --secondChild;
        *(first + holeIndex) = *(first + secondChild);
        holeIndex = secondChild;
        secondChild = holeIndex * 2 + 2;
    }

    // holeIndex has no secondChild, compare with first chi
    if (secondChild == len) {
        *(first + holeIndex) = *(first + (secondChild - 1));
        holeIndex = secondChild - 1;
    }
    // not remove last biggest node, need do one more vector::pop_back()
}

template<class RandomAccessIterator>
inline void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
    while (last - first > 1) {
        pop_heap(first, last--);
    }
}

template<class RandomAccessIterator>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
    make_heap_aux(first, last, value_type(first), size_type(first));
}

template<class RandomAccessIterator, class T, class Distance>
void make_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T *, Distance *) {
    if (last - first < 2) return;

    Distance len = last - first;
    Distance parent = (len - 2) / 2;    // find first sorted parent node, leaf node don't need adjust
    while (true) {
        _adjust_heap(first, parent, len, T(*(first + parent)));
        parent--;
    }
}

#endif //BETHSTL_HEAP_H
