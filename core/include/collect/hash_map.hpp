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

    [[nodiscard]] usize capacity() const {
      return buckets_.capacity() + collision_buckets_.capacity();
    }

    [[nodiscard]] usize len() const {
      return elem_count_;
    }

    [[nodiscard]] bool is_empty() const {
      return len() == 0;
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

      const Bucket *bucket = buckets_.get(idx);
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
      HashMap *map_;
      usize    idx_;

      [[nodiscard]] usize total() const { return map_->buckets_.len() + map_->collision_buckets_.len(); }

      Bucket &at(usize i) const {
        const usize n = map_->buckets_.len();
        return i < n ? map_->buckets_[i] : map_->collision_buckets_[i - n];
      }

      void skip() {
        while (idx_ < total() && at(idx_).next_bucket_idx == MAP_INACTIVE_BUCKET) {
          ++idx_;
        }
      }

    public:
      Iter(HashMap &map, usize idx) : map_(&map), idx_(idx) { skip(); }

      Pair<K, V> &operator*() const { return at(idx_).pair; }
      Pair<K, V> *operator->() const { return &at(idx_).pair; }

      Iter &operator++() {
        ++idx_;
        skip();
        return *this;
      }

      Iter operator++(i32) {
        auto tmp = *this;
        ++*this;
        return tmp;
      }

      friend bool operator==(const Iter &a, const Iter &b) { return a.idx_ == b.idx_; }
    };

    class ConstIter {
      const HashMap *map_;
      usize          idx_;

      [[nodiscard]] usize total() const { return map_->buckets_.len() + map_->collision_buckets_.len(); }

      const Bucket &at(usize i) const {
        const usize n = map_->buckets_.len();
        return i < n ? map_->buckets_[i] : map_->collision_buckets_[i - n];
      }

      void skip() {
        while (idx_ < total() && at(idx_).next_bucket_idx == MAP_INACTIVE_BUCKET) {
          ++idx_;
        }
      }

    public:
      ConstIter(const HashMap &map, usize idx) : map_(&map), idx_(idx) { skip(); }

      const Pair<K, V> &operator*() const { return at(idx_).pair; }
      const Pair<K, V> *operator->() const { return &at(idx_).pair; }

      ConstIter &operator++() {
        ++idx_;
        skip();
        return *this;
      }

      ConstIter operator++(i32) {
        auto tmp = *this;
        ++*this;
        return tmp;
      }

      friend bool operator==(const ConstIter &a, const ConstIter &b) { return a.idx_ == b.idx_; }
    };

    Iter begin() {
      return Iter(*this, 0);
    }

    Iter end() {
      return Iter(*this, buckets_.len() + collision_buckets_.len());
    }

    ConstIter begin() const {
      return ConstIter(*this, 0);
    }

    ConstIter end() const {
      return ConstIter(*this, buckets_.len() + collision_buckets_.len());
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
