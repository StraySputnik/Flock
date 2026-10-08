#pragma once

#include "common.hpp"
#include "collect/vec.hpp"

namespace Flock::ECS {
    using EntityID                                = u32;
    static constexpr usize GROUP_DEFAULT_CAPACITY = 32;

    template <typename T>
    class Group {
        Vec<usize>    sparse     = {};
        Vec<EntityID> dense      = {};
        Vec<T>        components = {};

    public:
        static Group create(usize cap = GROUP_DEFAULT_CAPACITY) {
            Group group{};
            group.sparse     = Vec<usize>::with_cap(cap);
            group.dense      = Vec<EntityID>::with_cap(cap);
            group.components = Vec<T>::with_cap(cap);
            return group;
        }

        void insert(EntityID id, T element) {
            if (id >= sparse.len()) {
                sparse.resize(id + 1, INVALID);
            }

            ASSERT(sparse[id] == INVALID, "Insert on an existing element");

            sparse[id] = dense.len();
            dense.push(id);
            components.push(element);
        }

        T *get(EntityID id) {
            if (id >= sparse.len() || sparse[id] == INVALID) {
                return nullptr;
            }

            return components.get(sparse[id]);
        }

        const T *get(EntityID id) const {
            if (id >= sparse.len() || sparse[id] == INVALID) {
                return nullptr;
            }

            return components.get(sparse[id]);
        }

        void remove(EntityID id) {
            if (id >= sparse.len() || sparse[id] == INVALID) {
                return;
            }

            usize dense_idx = sparse[id];
            if (dense_idx != dense.len() - 1) {
                sparse[*dense.last()] = dense_idx;
            }

            dense.swap_remove(dense_idx);
            components.swap_remove(dense_idx);

            sparse[id] = INVALID;
        }
    };
}
