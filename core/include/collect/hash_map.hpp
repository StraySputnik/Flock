#pragma once

#include "common.hpp"
#include "hash.hpp"
#include "pair.hpp"
#include "vec.hpp"

namespace Flock {
  static constexpr usize MAP_INIT_VECTOR_LENGTH = 8;
  static constexpr f32   MAP_LOAD_FACTOR        = 0.5f;
  static constexpr usize MAP_INACTIVE_BUCKET    = INVALID - 1;

  template<typename K, typename V>
  class HashMap {
    struct Bucket {
      usize      next_bucket_idx = MAP_INACTIVE_BUCKET;
      Pair<K, V> pair;
    };

    Vec<Bucket> buckets_           = {};
    Vec<Bucket> collision_buckets_ = {};
    usize       elem_count_        = 0;

  public:
    static HashMap create() {
      HashMap map{};
      map.buckets_           = Vec<Bucket>::with_capacity(MAP_INIT_VECTOR_LENGTH);
      map.collision_buckets_ = Vec<Bucket>::with_capacity(1);
      map.elem_count_        = 0;
      map.buckets_.resize(map.buckets_.capacity());
      return map;
    }

    void release() {
      buckets_.release();
      collision_buckets_.release();
      elem_count_ = 0;
    }

    void rehash() {
      if (buckets_.is_empty()) {
        return;
      }

      auto buckets = Vec<Bucket>::with_capacity(elem_count_);
      for (const auto &bucket: buckets_) {
        if (bucket.next_bucket_idx == MAP_INACTIVE_BUCKET) {
          continue;
        }

        buckets.push(bucket);
      }

      for (const auto &bucket: collision_buckets_) {
        if (bucket.next_bucket_idx == MAP_INACTIVE_BUCKET) {
          continue;
        }

        buckets.push(bucket);
      }

      buckets_.fill({});
      collision_buckets_.resize(0);

      for (const auto &bucket: buckets) {
        insert_impl(bucket.pair.first, bucket.pair.second);
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

      if (buckets_.is_empty()) {
        buckets_.resize(MAP_INIT_VECTOR_LENGTH);
      }

      const f32 load_factor = static_cast<f32>(elem_count_) / static_cast<f32>(buckets_.len());
      if (load_factor > MAP_LOAD_FACTOR) {
        buckets_.reserve_exact(buckets_.len() * 0.5);
        resize(buckets_.capacity());
      }

      insert_impl(key, value);
      elem_count_++;
    }

    V *get(const K &key) {
      if (buckets_.is_empty()) {
        return nullptr;
      }

      Hash  h   = hash(key);
      usize idx = h % buckets_.len();

      Bucket *bucket = buckets_.get(idx);
      ASSERT(bucket, "Out of bounds access");

      while (bucket && bucket->pair.first != key) {
        bucket = collision_buckets_.get(bucket->next_bucket_idx);
      }

      if (bucket) {
        return &bucket->pair.second;
      }

      return nullptr;
    }

    const V *get(const K &key) const {
      if (buckets_.is_empty()) {
        return nullptr;
      }

      Hash  h   = hash(key);
      usize idx = h % buckets_.len();

      Bucket *bucket = buckets_.get(idx);
      ASSERT(bucket, "Out of bounds access");

      while (bucket && bucket->pair.first != key) {
        bucket = collision_buckets_.get(bucket->next_bucket_idx);
      }

      if (bucket) {
        return &bucket->pair.second;
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
      if (buckets_.is_empty()) {
        return;
      }

      Hash  h   = hash(key);
      usize idx = h % buckets_.len();

      Bucket *bucket = buckets_.get(idx);
      ASSERT(bucket, "Out of bounds access");

      while (bucket && bucket->pair.first != key) {
        bucket = collision_buckets_.get(bucket->next_bucket_idx);
      }

      if (!bucket) {
        return;
      }

      if (bucket->next_bucket_idx != INVALID && bucket->next_bucket_idx != MAP_INACTIVE_BUCKET) {
        *bucket = std::move(collision_buckets_[bucket->next_bucket_idx]);
      } else {
        bucket->next_bucket_idx = MAP_INACTIVE_BUCKET;
        bucket->pair.first      = {};
        bucket->pair.second     = {};
      }
    }

    class Iter {
      HashMap &map_;
      Bucket * ptr_;

      Bucket *next_bucket() {
        auto ptr = ptr_;
        ++ptr;
        while (ptr->next_bucket_idx == MAP_INACTIVE_BUCKET) {
          ++ptr;

          if (ptr == map_.buckets_.end()) {
            ptr = map_.collision_buckets_.begin();
          }

          if (ptr == map_.collision_buckets_.end()) {
            return nullptr;
          }
        }

        return ptr;
      }

    public:
      explicit Iter(HashMap &map, Bucket *bucket) : map_(map), ptr_(bucket) {
        if (bucket && bucket->next_bucket_idx == MAP_INACTIVE_BUCKET) {
          ptr_ = next_bucket();
        }
      }

      Pair<K, V> &operator*() const {
        return ptr_->pair;
      }

      Pair<K, V> *operator->() const {
        return &ptr_->pair;
      }

      Iter &operator++() {
        ptr_ = next_bucket();
        return *this;
      }

      Iter operator++(i32) {
        auto tmp = *this;
        ptr_     = next_bucket();
        return tmp;
      }

      friend bool operator==(const Iter &a, const Iter &b) {
        return a.ptr_ == b.ptr_;
      }
    };

    class ConstIter {
      const HashMap &map_;
      const Bucket * ptr_;

      const Bucket *next_bucket() {
        auto ptr = ptr_;
        ++ptr;
        while (ptr->next_bucket_idx == MAP_INACTIVE_BUCKET) {
          ++ptr;

          if (ptr == map_.buckets_.end()) {
            ptr = map_.collision_buckets_.begin();
          }

          if (ptr == map_.collision_buckets_.end()) {
            return nullptr;
          }
        }

        return ptr;
      }

    public:
      explicit ConstIter(const HashMap &map, const Bucket *bucket) : map_(map), ptr_(bucket) {
        if (bucket->next_bucket_idx == MAP_INACTIVE_BUCKET) {
          ptr_ = next_bucket();
        }
      }

      const Pair<K, V> &operator*() const {
        return ptr_->pair;
      }

      const Pair<K, V> *operator->() const {
        return &ptr_->pair;
      }

      ConstIter &operator++() {
        ptr_ = next_bucket();
        return *this;
      }

      ConstIter operator++(i32) {
        auto tmp = *this;
        ptr_     = next_bucket();
        return tmp;
      }

      friend bool operator==(const ConstIter &a, const ConstIter &b) {
        return a.ptr_ == b.ptr_;
      }
    };

    Iter begin() {
      return Iter(*this, buckets_.begin());
    }

    Iter end() {
      return Iter(*this, collision_buckets_.end());
    }

    ConstIter begin() const {
      return ConstIter(*this, buckets_.begin());
    }

    ConstIter end() const {
      return ConstIter(*this, collision_buckets_.end());
    }

  private:
    void insert_impl(K key, V value) {
      Hash  h   = hash(key);
      usize idx = h % buckets_.len();

      auto *bucket = buckets_.get(idx);
      ASSERT(bucket, "Out of bounds access");

      if (bucket->next_bucket_idx != MAP_INACTIVE_BUCKET) {
        collision_buckets_.push(Bucket{.next_bucket_idx = INVALID, .pair = {key, value}});
        while (bucket->next_bucket_idx != INVALID && bucket->next_bucket_idx != MAP_INACTIVE_BUCKET) {
          bucket = collision_buckets_.get(bucket->next_bucket_idx);
        }

        bucket->next_bucket_idx = collision_buckets_.len() - 1;
      } else {
        bucket->next_bucket_idx = INVALID;
        bucket->pair.first      = key;
        bucket->pair.second     = value;
      }
    }
  };
}
