#pragma once

#include "common.hpp"
#include "collect/vector.hpp"

namespace Flock::ECS {
    using EntityID = usize;

    template <typename T>
    struct Group {
        Vector<usize>    sparse     = {};
        Vector<EntityID> dense      = {};
        Vector<T>        components = {};
    };

    template <typename T>
    Group<T> group_create(usize cap = 0) {
        return Group<T>{
            .sparse     = vector_with_cap<usize>(cap),
            .dense      = vector_with_cap<EntityID>(cap),
            .components = vector_with_cap<T>(cap),
        };
    }

    template <typename T>
    void group_delete(Group<T> *group) {
        vector_delete(&group->sparse);
        vector_delete(&group->dense);
        vector_delete(&group->components);
    }

    template <typename T>
    void insert(Group<T> *group, EntityID id, T element) {
        if (id >= len(&group->sparse)) {
            resize(&group->sparse, id + 1, INVALID_64);
        }

        ASSERT(*get(&group->sparse, id) == INVALID_64, "Insert on an existing element");

        *get(&group->sparse, id) = len(&group->dense);
        push(&group->dense, id);
        push(&group->components, element);
    }

    template <typename T>
    T *get(Group<T> *group, EntityID id) {
        if (id >= len(&group->sparse) || *get(&group->sparse, id) == INVALID_64) {
            return nullptr;
        }

        return get(&group->components, *get(&group->sparse, id));
    }

    template <typename T>
    void remove(Group<T> *group, EntityID id) {
        if (id >= len(&group->sparse) || *get(&group->sparse, id) == INVALID_64) {
            return;
        }

        usize dense_idx = *get(&group->sparse, id);
        if (dense_idx != len(&group->dense) - 1) {
            *get(&group->sparse, *last(&group->dense)) = dense_idx;
        }

        swap_remove(&group->dense, dense_idx);
        swap_remove(&group->components, dense_idx);

        *get(&group->sparse, id) = INVALID_64;
    }

    template <typename T>
    void for_each(Group<T> *group, void (*func)(T *)) {
        for_each(&group->components, func);
    }

    template <typename T>
    void for_each(const Group<T> *group, void (*func)(const T *)) {
        for_each(&group->components, func);
    }

    template <typename T>
    void for_each_i(Group<T> *group, void (*func)(EntityID, T *)) {
        for (usize idx = 0; idx < len(&group->components); idx++) {
            auto v  = get(&group->components, idx);
            auto id = *get(&group->dense, idx);
            func(id, v);
        }
    }

    template <typename T>
    void for_each_i(const Group<T> *group, void (*func)(EntityID, const T *)) {
        for (usize idx = 0; idx < len(&group->components); idx++) {
            auto v  = get(&group->components, idx);
            auto id = *get(&group->dense, idx);
            func(id, v);
        }
    }

    template <typename T>
    void for_each(Group<T> *group, void *ctx, void (*func)(T *, void *)) {
        for_each(&group->components, ctx, func);
    }

    template <typename T>
    void for_each(const Group<T> *group, void *ctx, void (*func)(const T *, void *)) {
        for_each(&group->components, ctx, func);
    }

    template <typename T>
    void for_each_i(Group<T> *group, void *ctx, void (*func)(EntityID, T *, void *)) {
        for (usize idx = 0; idx < len(&group->components); idx++) {
            auto v  = get(&group->components, idx);
            auto id = *get(&group->dense, idx);
            func(id, v, ctx);
        }
    }

    template <typename T>
    void for_each_i(const Group<T> *group, void *ctx, void (*func)(EntityID, const T *, void *)) {
        for (usize idx = 0; idx < len(&group->components); idx++) {
            auto v  = get(&group->components, idx);
            auto id = *get(&group->dense, idx);
            func(id, v, ctx);
        }
    }
}
