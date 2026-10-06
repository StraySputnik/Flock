#pragma once

#include "common.hpp"
#include "hash.hpp"
#include "vector.hpp"

namespace Flock {
    static constexpr usize MAP_INIT_VECTOR_LENGTH = 8;
    static constexpr f32   MAP_LOAD_FACTOR        = 0.5f;
    static constexpr usize MAP_INACTIVE_BUCKET    = INVALID - 1;

    template <typename K, typename V>
    class Map {
        struct Bucket {
            usize next_bucket_idx = MAP_INACTIVE_BUCKET;
            K     key             = {};
            V     value           = {};
        };

        Vector<Bucket> buckets_           = {};
        Vector<Bucket> collision_buckets_ = {};
        usize          elem_count_        = 0;

    public:
        static Map create() {
            Map map{};
            map.buckets_           = Vector<Bucket>::with_len(MAP_INIT_VECTOR_LENGTH);
            map.collision_buckets_ = Vector<Bucket>::with_cap(1);
            map.elem_count_        = 0;
            return map;
        }

        void free() {
            buckets_.free();
            collision_buckets_.free();
            elem_count_ = 0;
        }

        void rehash() {
            auto buckets = Vector<Bucket>::with_cap(elem_count_);
            for (const auto &bucket : buckets_) {
                if (bucket.next_bucket_idx == MAP_INACTIVE_BUCKET) {
                    continue;
                }

                buckets.push(bucket);
            }

            for (const auto &bucket : collision_buckets_) {
                if (bucket.next_bucket_idx == MAP_INACTIVE_BUCKET) {
                    continue;
                }

                buckets.push(bucket);
            }

            buckets_.empty_fill();
            collision_buckets_.resize(0);

            for (const auto &bucket : buckets) {
                insert_impl(bucket.key, bucket.value);
            }
        }

        void resize(usize len) {
            if (len <= buckets_.len()) {
                return;
            }

            buckets_.resize(len);
            rehash();
        }

        void insert(const K &key, const V &value) {
            ASSERT(!get(key), "Insert on an existing element");

            const f32 load_factor = static_cast<f32>(elem_count_) / static_cast<f32>(buckets_.len());
            if (load_factor > MAP_LOAD_FACTOR) {
                buckets_.grow();
                resize(buckets_.cap());
            }

            insert_impl(key, value);
            elem_count_++;
        }

        V *get(const K &key) {
            Hash  h   = hash(key);
            usize idx = h % buckets_.len();

            Bucket *bucket = buckets_.get(idx);
            ASSERT(bucket, "Out of bounds access");

            while (bucket && bucket->key != key) {
                bucket = collision_buckets_.get(bucket->next_bucket_idx);
            }

            if (bucket) {
                return &bucket->value;
            }

            return nullptr;
        }

        const V *get(const K &key) const {
            Hash  h   = hash(key);
            usize idx = h % buckets_.len();

            Bucket *bucket = buckets_.get(idx);
            ASSERT(bucket, "Out of bounds access");

            while (bucket && bucket->key != key) {
                bucket = collision_buckets_.get(bucket->next_bucket_idx);
            }

            if (bucket) {
                return &bucket->value;
            }

            return nullptr;
        }

        bool has(const K &key) {
            return get(key) != nullptr;
        }

        V &operator[](const K &key) {
            if (!has(key)) {
                insert(key, {});
            }

            return *get(key);
        }

        const V &operator[](const K &key) const {
            ASSERT(has(key), "Key not found");
            return *get(key);
        }

        void remove(const K &key) {
            Hash  h   = hash(key);
            usize idx = h % buckets_.len();

            Bucket *bucket = buckets_.get(idx);
            ASSERT(bucket, "Out of bounds access");

            while (bucket && bucket->key != key) {
                bucket = collision_buckets_.get(bucket->next_bucket_idx);
            }

            if (bucket) {
                bucket->next_bucket_idx = MAP_INACTIVE_BUCKET;
                bucket->key             = {};
                bucket->value           = {};
            }
        }

    private:
        void insert_impl(K key, V value) {
            Hash  h   = hash(key);
            usize idx = h % buckets_.len();

            auto *bucket = buckets_.get(idx);
            ASSERT(bucket, "Out of bounds access");

            if (bucket->next_bucket_idx != MAP_INACTIVE_BUCKET) {
                collision_buckets_.push(Bucket{.next_bucket_idx = INVALID, .key = key, .value = value});
                while (bucket->next_bucket_idx != INVALID && bucket->next_bucket_idx != MAP_INACTIVE_BUCKET) {
                    bucket = collision_buckets_.get(bucket->next_bucket_idx);
                }

                bucket->next_bucket_idx = collision_buckets_.len() - 1;
            } else {
                bucket->next_bucket_idx = INVALID;
                bucket->key             = key;
                bucket->value           = value;
            }
        }
    };
}
