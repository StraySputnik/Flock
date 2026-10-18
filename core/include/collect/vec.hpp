#pragma once

#include <type_traits>

#include "common.hpp"
#include "memory/allocator.hpp"

namespace Flock {
  static constexpr usize VECTOR_INIT_LENGTH = 8;
  static constexpr f32   VECTOR_GROW_FACTOR = 1.4f;

  template<typename T>
  class Vec {
    T *        ptr_       = nullptr;
    Allocator *allocator_ = nullptr;
    usize      len_       = 0;
    usize      cap_       = 0;

  public:
    static Vec create() {
      Vec vector        = {};
      vector.allocator_ = Allocator::get_global();
      vector.len_       = 0;
      vector.cap_       = VECTOR_INIT_LENGTH;

      vector.ptr_ = static_cast<T *>(alloc(vector.allocator_, VECTOR_INIT_LENGTH * sizeof(T), alignof(T)));
      ASSERT(vector.ptr_, "Allocation failed");
      return vector;
    }

    static Vec with_capacity(usize cap) {
      Vec vector        = {};
      vector.allocator_ = Allocator::get_global();
      vector.len_       = 0;
      vector.cap_       = cap;

      vector.ptr_ = static_cast<T *>(alloc(vector.allocator_, cap * sizeof(T), alignof(T)));
      ASSERT(vector.ptr_, "Allocation failed");
      return vector;
    }

    Vec() = default;

    Vec(const Vec &other) requires std::is_copy_constructible_v<T> {
      allocator_ = Allocator::get_global();
      len_       = other.len_;
      cap_       = other.cap_;

      ptr_ = static_cast<T *>(alloc(allocator_, cap_ * sizeof(T), alignof(T)));
      ASSERT(ptr_, "Allocation failed");

      for (usize i = 0; i < len_; i++) {
        new(ptr_ + i) T(other[i]);
      }
    }

    Vec(Vec &&other) noexcept {
      allocator_ = other.allocator_;
      len_       = other.len_;
      cap_       = other.cap_;
      ptr_       = other.ptr_;

      other.allocator_ = nullptr;
      other.len_       = 0;
      other.cap_       = 0;
      other.ptr_       = nullptr;
    }

    Vec &operator=(const Vec &other) requires std::is_copy_assignable_v<T> {
      if (this == &other) {
        return *this;
      }

      release();

      allocator_ = Allocator::get_global();
      len_       = other.len_;
      cap_       = other.cap_;

      ptr_ = static_cast<T *>(alloc(allocator_, cap_ * sizeof(T), alignof(T)));
      ASSERT(ptr_, "Allocation failed");

      for (usize i = 0; i < len_; i++) {
        ptr_[i] = other[i];
      }

      return *this;
    }

    Vec &operator=(Vec &&other) noexcept {
      if (this == &other) {
        return *this;
      }

      release();

      allocator_ = other.allocator_;
      len_       = other.len_;
      cap_       = other.cap_;
      ptr_       = other.ptr_;

      other.allocator_ = nullptr;
      other.len_       = 0;
      other.cap_       = 0;
      other.ptr_       = nullptr;

      return *this;
    }

    ~Vec() {
      release();
    }

    void release() {
      if (!ptr_) {
        return;
      }

      if (len_ > cap_) {
        UNREACHABLE(); // For a gcc warning
      }

      for (usize i = 0; i < len_; i++) {
        ptr_[i].~T();
      }

      ptr_ = nullptr;
      len_ = 0;
      cap_ = 0;

      Flock::release(allocator_, ptr_, cap_ * sizeof(T));
    }

    void reserve(const usize additional) {
      usize new_cap = cap_;
      if (new_cap < VECTOR_INIT_LENGTH) {
        new_cap = VECTOR_INIT_LENGTH;
      }

      while (new_cap < len_ + additional) {
        new_cap *= VECTOR_GROW_FACTOR;
      }

      ptr_ = static_cast<T *>(realloc(allocator_, ptr_, cap_ * sizeof(T), new_cap * sizeof(T), alignof(T)));
      cap_ = new_cap;

      ASSERT(ptr_, "Allocation failed");
    }

    void reserve_exact(const usize additional) {
      if (cap_ >= len_ + additional) {
        return;
      }

      const usize new_cap = len_ + additional;
      ptr_ = static_cast<T *>(allocator_->realloc(ptr_, cap_ * sizeof(T), new_cap * sizeof(T), alignof(T)));
      cap_ = new_cap;

      ASSERT(ptr_, "Allocation failed");
    }

    void shrink_to(const usize min_capacity) {
      if (min_capacity >= cap_) {
        return;
      }

      const usize new_cap = min_capacity < len_ ? len_ : min_capacity;

      ptr_ = static_cast<T *>(allocator_->realloc(ptr_, cap_ * sizeof(T), new_cap * sizeof(T), alignof(T)));
      cap_ = min_capacity;
    }

    void shrink_to_fit() {
      shrink_to(len_);
    }

    [[nodiscard]] usize len() const {
      return len_;
    }

    [[nodiscard]] usize capacity() const {
      return cap_;
    }

    [[nodiscard]] Allocator *allocator() const {
      return allocator_;
    }

    [[nodiscard]] bool is_empty() const {
      return len_ == 0;
    }

    [[nodiscard]] const T *ptr() const {
      return ptr_;
    }

    [[nodiscard]] T *ptr() {
      return ptr_;
    }

    T *first() {
      return ptr_;
    }

    const T *first() const {
      return ptr_;
    }

    T *last() {
      if (!ptr_) {
        return nullptr;
      }

      return ptr_ + len_ - 1;
    }

    const T *last() const {
      if (!ptr_) {
        return nullptr;
      }

      return ptr_ + len_ - 1;
    }

    T *get(usize idx) {
      if (idx >= len_) {
        return nullptr;
      }

      return ptr_ + idx;
    }

    const T *get(usize idx) const {
      if (idx >= len_) {
        return nullptr;
      }

      return ptr_ + idx;
    }

    T &operator[](usize idx) {
      ASSERT(idx < len_, "Out of bounds access");
      return ptr_[idx];
    }

    const T &operator[](usize idx) const {
      ASSERT(idx < len_, "Out of bounds access");
      return ptr_[idx];
    }

    bool contains(const T &value) const {
      for (usize i = 0; i < len_; i++) {
        if (ptr_[i] == value) {
          return true;
        }
      }

      return false;
    }

    void push(const T &value) {
      reserve(1);
      new(ptr_ + len_) T(value);
      len_++;
    }

    void push(T &&value) {
      reserve(1);
      new(ptr_ + len_) T(std::move(value));
      len_++;
    }

    void insert(usize idx, const T &value) {
      ASSERT(idx <= len_, "Out of bounds access");
      if (idx == len_) {
        push(value);
        return;
      }

      reserve(1);
      for (usize i = len_; i > idx; i--) {
        ptr_[i] = std::move(ptr_[i - 1]);
      }

      ptr_[idx] = value;
      len_++;
    }

    void insert(usize idx, T &&value) {
      ASSERT(idx <= len_, "Out of bounds access");
      if (idx == len_) {
        push(std::move(value));
        return;
      }

      reserve(1);
      for (usize i = len_; i > idx; i--) {
        ptr_[i] = std::move(ptr_[i - 1]);
      }

      ptr_[idx] = std::move(value);
      len_++;
    }

    Maybe<T> pop() {
      if (len_ == 0) {
        return {};
      }

      T value = std::move(ptr_[len_ - 1]);
      len_--;
      return value;
    }

    T remove(usize idx) {
      ASSERT(idx < len_, "Out of bounds access");
      if (idx == len_ - 1) {
        return pop().get();
      }

      T value = std::move(ptr_[idx]);
      for (usize i = idx; i < len_ - 1; i++) {
        ptr_[i] = std::move(ptr_[i + 1]);
      }

      len_--;
      return value;
    }

    T swap_remove(usize idx) {
      ASSERT(len_ > 0 && idx < len_, "Out of bounds access");

      if (idx == len_ - 1) {
        return pop().get();
      }

      T val     = std::move(ptr_[idx]);
      ptr_[idx] = std::move(ptr_[len_ - 1]);
      len_--;

      return val;
    }

    void grow(const usize len, const T &value) {
      if (len <= len_) {
        return;
      }

      reserve(len - len_);
      for (usize i = len_; i < len; i++) {
        new(ptr_ + i) T(value);
      }

      len_ = len;
    }

    void grow(const usize len, T &&value) {
      if (len <= len_) {
        return;
      }

      reserve(len - len_);
      for (usize i = len_; i < len; i++) {
        new(ptr_ + i) T(std::move(value));
      }

      len_ = len;
    }

    void truncate(const usize len) {
      if (len >= len_) {
        return;
      }

      for (usize i = len; i < len_; i++) {
        ptr_[i].~T();
      }

      len_ = len;
    }

    void clear() {
      for (usize i = 0; i < len_; i++) {
        ptr_[i].~T();
      }

      len_ = 0;
    }

    void append(const Vec vector) {
      const usize len = vector.len_;
      reserve(len_ + len);
      for (usize i = 0; i < len; i++) {
        push(vector[i]);
      }
    }

    Vec split_off(const usize at) {
      Vec vec = with_capacity(len_ - at);
      for (usize i = at; i < len_; i++) {
        vec.push(std::move(ptr_[i]));
      }

      truncate(at);
      return vec;
    }

    Vec &resize(const usize new_len, const T &value = {}) {
      if (new_len > len_) {
        grow(new_len, value);
      } else if (new_len < len_) {
        truncate(new_len);
      }

      len_ = new_len;
      return *this;
    }

    Vec &resize(const usize new_len, T &&value) {
      if (new_len > len_) {
        grow(new_len, std::move(value));
      } else if (new_len < len_) {
        truncate(new_len);
      }

      len_ = new_len;
      return *this;
    }

    void swap(usize a, usize b) {
      ASSERT(a < len_ && b < len_, "Out of bounds access");
      auto temp = std::move(ptr_[a]);
      ptr_[a]   = std::move(ptr_[b]);
      ptr_[b]   = std::move(temp);
    }

    void reverse() {
      for (usize i = 0; i < len_ / 2; i++) {
        swap(i, len_ - i - 1);
      }
    }

    void sort() {
      // TODO: Implement sorting
      PANIC();
    }

    void fill(const T &value) {
      for (usize i = 0; i < len_; i++) {
        ptr_[i] = value;
      }
    }

    Maybe<usize> rfindn(const T &value, usize n) const {
      for (usize i = len_ - 1; i != static_cast<usize>(0) - 1; i--) {
        if (ptr_[i] == value && n != 0) {
          n--;
        }

        if (ptr_[i] == value && n == 0) {
          return i;
        }
      }

      return {};
    }

    Maybe<usize> findn(const T &value, usize n) const {
      for (usize i = 0; i < len_; i++) {
        if (ptr_[i] == value && n != 0) {
          n--;
        }

        if (ptr_[i] == value && n == 0) {
          return i;
        }
      }

      return {};
    }

    Maybe<usize> find(const T &value) const {
      return findn(value, 0);
    }

    Maybe<usize> rfind(const T &value) const {
      return rfindn(value, 0);
    }

    void replacen(const T &value, const T &replace, usize n) {
      while (n != 0) {
        auto i = find(value);
        if (!i) {
          break;
        }

        ptr_[i.get()] = replace;
        n--;
      }
    }

    void rreplacen(const T &value, const T &replace, usize n) {
      while (n != 0) {
        auto i = find(value);
        if (!i) {
          break;
        }

        ptr_[i.get()] = replace;
        n--;
      }
    }

    void replace(const T &value, const T &replace) {
      return replacen(value, replace, 1);
    }

    void rreplace(const T &value, const T &replace) {
      return rreplacen(value, replace, 1);
    }

    void replace_all(const T &value, const T &replace) {
      auto  i = findn(value, 0);
      usize n = 1;
      while (!i) {
        ptr_[i.get()] = replace;
        i             = findn(value, n);
        n++;
      }
    }

    bool operator==(const Vec &other) const {
      if (len_ != other.len_) {
        return false;
      }

      for (usize i = 0; i < len_; i++) {
        if (ptr_[i] != other[i]) {
          return false;
        }
      }

      return true;
    }

    bool operator!=(const Vec &other) const {
      return !(*this == other);
    }

    T *begin() {
      return ptr_;
    }

    T *end() {
      return ptr_ + len_;
    }

    const T *begin() const {
      return ptr_;
    }

    const T *end() const {
      return ptr_ + len_;
    }
  };
}
