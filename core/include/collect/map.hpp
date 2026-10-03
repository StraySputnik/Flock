#pragma once

#include "common.hpp"
#include "hash.hpp"
#include "vector.hpp"

namespace Flock {
    static constexpr usize MAP_INIT_VECTOR_LENGTH = 8;
    static constexpr f32   MAP_LOAD_FACTOR        = 0.5f;

    template <typename K, typename V, Deleter<K> k_deleter = nullptr, Deleter<V> v_deleter = nullptr>
    struct Map {
        struct Bucket {
            usize next_bucket_idx = INVALID_64;
            K     key             = {};
            V     value           = {};
            bool  active          = false;

            static void deleter(Bucket *bucket) {
                if constexpr (k_deleter != nullptr) k_deleter(&bucket->key);
                if constexpr (v_deleter != nullptr) v_deleter(&bucket->value);
            }
        };

        Vector<Bucket, Bucket::deleter> buckets           = {};
        Vector<Bucket, Bucket::deleter> collision_buckets = {};
        usize                           elem_count        = 0;
    };

    template <typename K, typename V, Deleter<K> k_deleter = nullptr, Deleter<V> v_deleter = nullptr>
    Map<K, V, k_deleter, v_deleter> map_create() {
        using Map = Map<K, V, k_deleter, v_deleter>;

        return Map{
            .buckets           = vector_with_len<typename Map::Bucket, Map::Bucket::deleter>(MAP_INIT_VECTOR_LENGTH),
            .collision_buckets = vector_with_cap<typename Map::Bucket, Map::Bucket::deleter>(1),
            .elem_count        = 0,
        };
    }

    template <typename K, typename V, Deleter<K> k_deleter, Deleter<V> v_deleter>
    void map_delete(Map<K, V, k_deleter, v_deleter> *map) {
        vector_delete(&map->buckets);
        vector_delete(&map->collision_buckets);
        map->elem_count = 0;
    }

    namespace Impl {
        template <typename K, typename V, Deleter<K> k_deleter, Deleter<V> v_deleter>
        void insert(Map<K, V, k_deleter, v_deleter> *map, K key, V value) {
            Hash  h   = hash(&key);
            usize idx = h % map->buckets.len;

            auto *bucket = get(&map->buckets, idx);
            ASSERT(bucket, "Out of bounds access");

            if (bucket->active) {
                push(&map->collision_buckets);
                last(&map->collision_buckets)->active = true;
                last(&map->collision_buckets)->key    = key;
                last(&map->collision_buckets)->value  = value;

                while (bucket->next_bucket_idx != INVALID_64) {
                    bucket = get(&map->collision_buckets, bucket->next_bucket_idx);
                }

                bucket->next_bucket_idx = map->collision_buckets.len - 1;
            } else {
                bucket->active          = true;
                bucket->next_bucket_idx = INVALID_64;
                bucket->key             = key;
                bucket->value           = value;
            }
        }
    }

    template <typename K, typename V, Deleter<K> k_deleter, Deleter<V> v_deleter>
    void rehash(Map<K, V, k_deleter, v_deleter> *map) {
        using Map    = Map<K, V, k_deleter, v_deleter>;
        auto buckets = vector_with_cap<typename Map::Bucket, Map::Bucket::deleter>(map->elem_count);

        for (usize i = 0; i < map->buckets.len; i++) {
            auto *bucket = get(&map->buckets, i);
            if (!bucket->active) {
                continue;
            }

            push(&buckets, *bucket);
        }

        for (usize i = 0; i < map->collision_buckets.len; i++) {
            auto *bucket = get(&map->collision_buckets, i);
            if (!bucket->active) {
                continue;
            }

            push(&buckets, *bucket);
        }

        zero_fill(&map->buckets);
        resize(&map->collision_buckets, 0);

        for (usize i = 0; i < buckets.len; i++) {
            Impl::insert(map, get(&buckets, i)->key, get(&buckets, i)->value);
        }

        vector_delete(&buckets);
    }

    template <typename K, typename V, Deleter<K> k_deleter, Deleter<V> v_deleter>
    void resize(Map<K, V, k_deleter, v_deleter> *map, usize len) {
        if (len <= map->buckets.len) {
            return;
        }

        resize(&map->buckets, len);
        rehash(map);
    }

    template <typename K, typename V, Deleter<K> k_deleter, Deleter<V> v_deleter>
    void insert(Map<K, V, k_deleter, v_deleter> *map, K key, V value) {
        if ((f32)map->elem_count / (f32)map->buckets.len > MAP_LOAD_FACTOR) {
            grow(&map->buckets);
            resize(map, map->buckets.cap);
        }

        Impl::insert(map, key, value);
        map->elem_count++;
    }

    template <typename K, typename V, Deleter<K> k_deleter, Deleter<V> v_deleter>
    V *get(Map<K, V, k_deleter, v_deleter> *map, K key) {
        using Bucket = Map<K, V, k_deleter, v_deleter>::Bucket;

        Hash  h   = hash(&key);
        usize idx = h % map->buckets.len;

        Bucket *bucket = get(&map->buckets, idx);
        ASSERT(bucket, "Out of bounds access");

        while (bucket && !equal(&bucket->key, &key) && bucket->next_bucket_idx != INVALID_64) {
            bucket = get(&map->collision_buckets, bucket->next_bucket_idx);
        }

        if (bucket) {
            return &bucket->value;
        }

        return nullptr;
    }
}
