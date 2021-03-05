//
// Created by Beth on 2021/3/3.
//

#ifndef BETHSTL_ALLOC_H
#define BETHSTL_ALLOC_H

#include <cstdio>
#include <cstdlib>
#include <new>

// define first level memory allocator
class firstLevelAlloc {
private:
    static void *oom_malloc_handler(size_t);

    static void *oom_realloc_handler(void *pointer, size_t);

    static void (*oom_malloc_selfHandler)();

public:
    static void *allocate(size_t n) {
        void *result = malloc(n);
        if (nullptr == result) result = oom_malloc_handler(n);

        return result;
    }

    static void deallocate(void *pointer, size_t) {
        free(pointer);
    }

    static void *reallocate(void *pointer, size_t old_size, size_t new_size) {
        void *result = realloc(pointer, new_size);
        if (nullptr == result) oom_realloc_handler(pointer, new_size);

        return result;
    }

    static void (*set_oom_malloc_selfHandler(void(*f)()))() {
        void (*old_handler)() = oom_malloc_selfHandler;
        oom_malloc_selfHandler = f;

        return (old_handler);
    }
};

// initial client oom handler to nullptr
void (*firstLevelAlloc::oom_malloc_selfHandler)() = nullptr;

// when oom occur,invoke client oom handler constantly until free enough memory to malloc or realloc.
void *firstLevelAlloc::oom_malloc_handler(size_t n) {
    void (*my_oom_handler)();
    void *result;

    for (;;) {
        my_oom_handler = oom_malloc_selfHandler;
        if (nullptr == my_oom_handler) throw std::bad_alloc();
        (*my_oom_handler)();
        result = malloc(n);
        if (result) return result;
    }
}

void *firstLevelAlloc::oom_realloc_handler(void *pointer, size_t n) {
    void (*my_oom_handler)();
    void *result;

    for (;;) {
        my_oom_handler = oom_malloc_selfHandler;
        if (nullptr == my_oom_handler) {
            fprintf(stderr, "out of memory\n");
            exit(1);
        }
        (*my_oom_handler)();
        result = realloc(pointer, n);
        if (result) return result;
    }
}

typedef firstLevelAlloc malloc_alloc;


class secondLevelAlloc {
public:
    static void *allocate(size_t n);

    static void deallocate(void *pointer, size_t n);

    static void *reallocate(void *pointer, size_t oldSize, size_t newSize);

private:
    enum {
        ALIGN = 8
    };
    enum {
        MAX_BYTES = 128
    };
    enum {
        LIST_SIZE = 16
    };

    static size_t chuckRoundUp(size_t n) {
        return ((n + (size_t) ALIGN - 1) & ~((size_t) ALIGN - 1));
    }

    union obj {
        union obj *next_link;
        char client_data[1];
    };

    static obj *volatile freeList[LIST_SIZE];

    static size_t freeListIndex(size_t bytes) {
        return ((bytes + ALIGN - 1) / ALIGN - 1);
    }

    static void *refill(size_t n);

    static char *chunk_alloc(size_t size, int &nobjs);

    static char *start_free;
    static char *end_free;
    static size_t heap_size;
};

char *secondLevelAlloc::start_free = nullptr;
char *secondLevelAlloc::end_free = nullptr;
size_t secondLevelAlloc::heap_size = 0;

secondLevelAlloc::obj *volatile secondLevelAlloc::freeList[LIST_SIZE] =
        {nullptr, nullptr, nullptr, nullptr,
         nullptr, nullptr, nullptr, nullptr,
         nullptr, nullptr, nullptr, nullptr,
         nullptr, nullptr, nullptr, nullptr};

void *secondLevelAlloc::allocate(size_t n) {
    if (n > (size_t) MAX_BYTES) {
        return (malloc_alloc::allocate(n));
    }

    obj *volatile *myFreeList;
    obj *result;
    myFreeList = freeList + freeListIndex(n);
    result = *myFreeList;
    if (nullptr == result) {
        // not find useful freelist unit in freelist array
        void *r = refill(chuckRoundUp(n));
        return r;
    }

    *myFreeList = result->next_link;
    return (result);
}

void secondLevelAlloc::deallocate(void *pointer, size_t n) {
    if (n > (size_t) MAX_BYTES) {
        malloc_alloc::deallocate(pointer, n);
        return;
    }

    obj *temp = (obj *) pointer;
    obj *volatile *myFreeList;
    myFreeList = freeList + freeListIndex(n);
    temp->next_link = *myFreeList;
    *myFreeList = temp;
}

void *secondLevelAlloc::refill(size_t n) {
    int nobj = 20;
    char *chuck = chunk_alloc(n, nobj);
    if (1 == nobj) return chuck;

    obj *volatile *myFreeList = freeList + freeListIndex(n);
    obj *result = (obj *) chuck;
    obj *pre, *next;

    //myFreeList is empty, connect to new list directly.
    *myFreeList = next = (obj *) (chuck + n);
    for (int i = 2; i < nobj; i++) {
        pre = next;
        next = (obj *) (chuck + n * i);
        pre->next_link = next;
    }
    next->next_link = nullptr;

    return result;
}

char *secondLevelAlloc::chunk_alloc(size_t size, int &nobjs) {
    char *result = nullptr;
    size_t totalBytes = size * nobjs;
    size_t leftBytes = end_free - start_free;

    if (leftBytes >= totalBytes) {
        result = start_free;
        start_free += totalBytes;
        return result;
    } else if (leftBytes >= size) {
        nobjs = leftBytes / size;
        result = start_free;
        start_free += (nobjs * size);
        return result;
    } else {
        //get extra space to lower time cost
        size_t bytesToGet = 2 * totalBytes + chuckRoundUp(heap_size >> 4);
        if (leftBytes > 0) {
            obj *volatile *myFreeList = freeList + freeListIndex(leftBytes);
            ((obj *) start_free)->next_link = *myFreeList;
            *myFreeList = (obj *) start_free;
        }

        start_free = (char *) malloc(bytesToGet);
        if (nullptr == start_free) {
            for (int i = size; i <= MAX_BYTES; i += ALIGN) {
                obj *volatile *myFreeList = freeList + freeListIndex(i);
                if (nullptr != *myFreeList) {
                    obj *temp = *myFreeList;
                    *myFreeList = temp->next_link;
                    start_free = (char *) temp;
                    end_free = start_free + i;
                    //invoke self to modify nobjs number
                    return chunk_alloc(size, nobjs);
                }
            }
            // can't find one useful freeListNode,invoke firstLevelAlloc to try
            // firstLevelAlloc can invoke oom_handler to free space
            // if oom still occur after oom_handler invoked ,firstLevelAlloc will throw error
            end_free = 0;
            start_free = (char *) malloc_alloc::allocate(bytesToGet);
        }

        heap_size += bytesToGet;
        end_free = start_free + bytesToGet;
        return (chunk_alloc(size, nobjs));
    }
}

typedef secondLevelAlloc freeList_alloc;


template<class T, class Alloc>
class simpleAlloc {
public:
    static T *allocate(size_t n) {
        return 0 == n ? 0 : (T *) Alloc::allocate(n * sizeof(T));
    }

    static T *allocate() {
        return (T *) Alloc::allocate(sizeof(T));
    }

    static void deallocate(T *pointer, size_t n) {
        if (0 != n) {
            Alloc::deallocate(pointer, n);
        }
    }

    static void deallocate(T *pointer) {
        Alloc::deallocate(pointer, sizeof(T));
    }
};


#endif //BETHSTL_ALLOC_H
