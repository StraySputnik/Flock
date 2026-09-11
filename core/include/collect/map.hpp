#ifndef FLOCK_MAP_HPP
#define FLOCK_MAP_HPP

#include "common.hpp"
#include "list.hpp"

namespace flock {
    template <typename K, typename V>
    class map {
        struct kv_pair {
            K key;
            V val;
        };

        struct bucket {
            list<kv_pair> pairs;
            usize         hash;
        };

    public:
        void insert(const K &key, const V &value) {
        }
    };
}

#endif //FLOCK_MAP_HPP
