#pragma once

#include "common.hpp"
#include "memory/allocator.hpp"

namespace Flock {
    static constexpr usize VECTOR_INIT_LENGTH = 8;
    static constexpr f32   VECTOR_GROW_FACTOR = 1.4f;

    template <typename T, Deleter<T> deleter = nullptr>
    struct Vector {
        T *        ptr       = nullptr;
        Allocator *allocator = nullptr;
        usize      len       = 0;
        usize      cap       = 0;
    };

    template <typename T, Deleter<T> deleter = nullptr>
    Vector<T, deleter> vector_create() {
        Vector<T, deleter> vector = {
            .ptr       = (T *)alloc(get_allocator(), VECTOR_INIT_LENGTH * sizeof(T), alignof(T)),
            .allocator = get_allocator(),
            .len       = 0,
            .cap       = VECTOR_INIT_LENGTH,
        };

        ASSERT(vector.ptr, "Allocation failed");
        return vector;
    }

    template <typename T, Deleter<T> deleter = nullptr>
    Vector<T, deleter> vector_with_cap(usize cap) {
        Vector<T, deleter> vector = {
            .ptr       = (T *)alloc(get_allocator(), cap * sizeof(T), alignof(T)),
            .allocator = get_allocator(),
            .len       = 0,
            .cap       = cap,
        };

        ASSERT(vector.ptr, "Allocation failed");
        return vector;
    }

    template <typename T, Deleter<T> deleter = nullptr>
    Vector<T, deleter> vector_with_len(usize len) {
        Vector<T, deleter> vector = {
            .ptr       = (T *)alloc(get_allocator(), len * sizeof(T), alignof(T)),
            .allocator = get_allocator(),
            .len       = 0,
            .cap       = len,
        };

        ASSERT(vector.ptr, "Allocation failed");

        for (usize i = 0; i < len; i++) {
            vector.ptr[i] = {};
        }

        vector.len = len;
        return vector;
    }

    template <typename T, Deleter<T> deleter>
    void vector_delete(Vector<T, deleter> *vector) {
        if constexpr (deleter != nullptr) {
            for (usize i = 0; i < vector->len; i++) {
                deleter(&vector->ptr[i]);
            }
        }

        free(vector->allocator, vector->ptr, vector->cap * sizeof(T));
        vector->ptr       = nullptr;
        vector->allocator = nullptr;
        vector->cap       = 0;
        vector->len       = 0;
    }

    template <typename T, Deleter<T> deleter>
    void grow(Vector<T, deleter> *vector) {
        usize new_cap = vector->cap < VECTOR_INIT_LENGTH ? VECTOR_INIT_LENGTH : vector->cap;
        new_cap       *= VECTOR_GROW_FACTOR;

        vector->ptr = (T *)realloc(vector->allocator, vector->ptr, vector->cap * sizeof(T), new_cap * sizeof(T),
                                   alignof(T));
        vector->cap = new_cap;

        ASSERT(vector->ptr, "Allocation failed");
    }

    template <typename T, Deleter<T> deleter>
    void reserve(Vector<T, deleter> *vector, usize cap) {
        if (cap <= vector->cap) {
            return;
        }

        usize new_cap = vector->cap < VECTOR_INIT_LENGTH ? VECTOR_INIT_LENGTH : vector->cap;
        while (new_cap < cap) {
            new_cap *= VECTOR_GROW_FACTOR;
        }

        vector->ptr = (T *)realloc(vector->allocator, vector->ptr, vector->cap * sizeof(T), new_cap * sizeof(T),
                                   alignof(T));
        vector->cap = new_cap;

        ASSERT(vector->ptr, "Allocation failed");
    }

    template <typename T, Deleter<T> deleter>
    void shrink_to(Vector<T, deleter> *vector, usize cap) {
        if (cap >= vector->cap) {
            return;
        }

        if constexpr (deleter != nullptr) {
            if (cap < vector->len) {
                for (usize i = cap; i < vector->len; i++) {
                    deleter(&vector->ptr[i]);
                }
            }
        }

        vector->ptr = (T *)realloc(vector->allocator, vector->ptr, vector->cap * sizeof(T), cap * sizeof(T),
                                   alignof(T));
        vector->cap = cap;
    }

    template <typename T, Deleter<T> deleter>
    void shrink_to_fit(Vector<T, deleter> *vector) {
        shrink_to(vector, vector->len);
    }

    template <typename T, Deleter<T> deleter>
    void resize(Vector<T, deleter> *vector, usize len) {
        if (len > vector->len) {
            if (len > vector->cap) {
                reserve(vector, len);
            }

            for (usize i = vector->len; i < len; i++) {
                vector->ptr[i] = {};
            }
        } else if (len < vector->len) {
            if constexpr (deleter != nullptr) {
                for (usize i = len; i < vector->len; i++) {
                    deleter(&vector->ptr[i]);
                }
            }
        }

        vector->len = len;
    }

    template <typename T, Deleter<T> deleter>
    void fill(Vector<T, deleter> *vector, T value) {
        for (usize i = 0; i < vector->len; i++) {
            vector->ptr[i] = value;
        }
    }

    template <typename T, Deleter<T> deleter>
    void zero_fill(Vector<T, deleter> *vector) {
        for (usize i = 0; i < vector->len; i++) {
            vector->ptr[i] = {};
        }
    }

    template <typename T, Deleter<T> deleter>
    usize len(const Vector<T, deleter> *vector) {
        return vector->len;
    }

    template <typename T, Deleter<T> deleter>
    usize cap(const Vector<T, deleter> *vector) {
        return vector->cap;
    }

    template <typename T, Deleter<T> deleter>
    Allocator *allocator(const Vector<T, deleter> *vector) {
        return vector->allocator;
    }

    template <typename T, Deleter<T> deleter>
    T *get(Vector<T, deleter> *vector, usize idx) {
        if (idx >= vector->len) {
            return nullptr;
        }

        return &vector->ptr[idx];
    }

    template <typename T, Deleter<T> deleter>
    T *first(Vector<T, deleter> *vector) {
        return get(vector, 0);
    }

    template <typename T, Deleter<T> deleter>
    T *last(Vector<T, deleter> *vector) {
        return get(vector, vector->len - 1);
    }

    template <typename T, Deleter<T> deleter>
    const T *get(const Vector<T, deleter> *vector, usize idx) {
        if (idx >= vector->len) {
            return nullptr;
        }

        return &vector->ptr[idx];
    }

    template <typename T, Deleter<T> deleter>
    const T *first(const Vector<T, deleter> *vector) {
        return get(vector, 0);
    }

    template <typename T, Deleter<T> deleter>
    const T *last(const Vector<T, deleter> *vector) {
        return get(vector, vector->len - 1);
    }

    template <typename T, Deleter<T> deleter>
    void push(Vector<T, deleter> *vector, T element = {}) {
        resize(vector, vector->len + 1);
        *last(vector) = element;
    }

    template <typename T, Deleter<T> deleter>
    void pop(Vector<T, deleter> *vector) {
        ASSERT(vector->len > 0, "Pop on empty vector");
        resize(vector, vector->len - 1);
    }
}

#define VECTOR_FOREACH(vec, v, func) for (usize i = 0; i < len((vec)); i++) { auto v = get((vec), i); func }