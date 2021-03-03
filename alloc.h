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
        if (nullptr == my_oom_handler) throw std::bad_alloc;
        (*my_oom_handler)();
        result = realloc(pointer, n);
        if (result) return result;
    }
}

typedef firstLevelAlloc malloc_alloc;

#endif //BETHSTL_ALLOC_H
