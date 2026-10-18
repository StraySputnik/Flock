#pragma once

#include "allocator.hpp"
#include "common.hpp"
#include "collect/hash_map.hpp"

namespace Flock {
  class Allocator;

  template<typename T>
  class OwnedRef {
    T *        ptr_       = nullptr;
    Allocator *allocator_ = nullptr;

  public:
    template<typename U>
    static OwnedRef from_value(const U &value) {
      static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U>, "U must be the same as or derive from T");

      OwnedRef ref{};
      ref.allocator_ = Allocator::get_global();
      void *mem      = alloc(ref.allocator_, sizeof(U), alignof(U));
      ASSERT(mem, "Allocation failed");
      ref.ptr_ = new(mem) U(value);
      return ref;
    }

    static OwnedRef from_value(T &&value) {
      OwnedRef ref{};
      ref.allocator_ = Allocator::get_global();
      void *mem      = alloc(ref.allocator_, sizeof(T), alignof(T));
      ASSERT(mem, "Allocation failed");
      ref.ptr_ = new(mem) T(value);

      return ref;
    }

    static OwnedRef from_ptr(T *ptr, Allocator *allocator = nullptr) {
      ASSERT(ptr, "Invalid pointer");
      OwnedRef ref{};
      ref.allocator_ = allocator;
      ref.ptr_       = ptr;
      return ref;
    }

    OwnedRef() = default;

    OwnedRef(const OwnedRef &) = delete;

    OwnedRef(OwnedRef &&other) noexcept {
      ptr_             = other.ptr_;
      allocator_       = other.allocator_;
      other.ptr_       = nullptr;
      other.allocator_ = nullptr;
    }

    OwnedRef &operator=(const OwnedRef &) = delete;

    OwnedRef &operator=(OwnedRef &&other) noexcept {
      if (this == &other) {
        return *this;
      }

      release();

      ptr_             = other.ptr_;
      allocator_       = other.allocator_;
      other.ptr_       = nullptr;
      other.allocator_ = nullptr;

      return *this;
    }

    ~OwnedRef() {
      release();
    }

    void release() {
      if (!ptr_) {
        return;
      }

      ptr_->~T();
      allocator_->release(ptr_, sizeof(T));
      ptr_ = nullptr;
    }

    T *get() {
      return ptr_;
    }

    const T *get() const {
      return ptr_;
    }

    T *operator ->() {
      ASSERT(ptr_, "Deref on invalid OwnedRef");
      return ptr_;
    }

    const T *operator ->() const {
      ASSERT(ptr_, "Deref on invalid OwnedRef");
      return ptr_;
    }

    T *operator *() {
      ASSERT(ptr_, "Deref on invalid OwnedRef");
      return ptr_;
    }

    const T *operator*() const {
      ASSERT(ptr_, "Deref on invalid OwnedRef");
      return ptr_;
    }

    operator bool() const {
      return ptr_;
    }
  };

  // Not atomic
  template<typename T>
  class SharedRef {
    inline static HashMap<T *, usize> ref_counts_ = {};

    T *        ptr_       = nullptr;
    Allocator *allocator_ = nullptr;

  public:
    template<typename U>
    static SharedRef from_value(const U &value) {
      static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U>, "U must be the same as or derive from T");

      SharedRef ref{};
      ref.allocator_ = Allocator::get_global();
      void *mem      = alloc(ref.allocator_, sizeof(U), alignof(U));
      ASSERT(mem, "Allocation failed");
      ref.ptr_ = new(mem) U(value);

      printf("ALLOC\n");
      return ref;
    }

    static SharedRef from_value(T &&value) {
      SharedRef ref{};
      ref.allocator_ = Allocator::get_global();
      void *mem      = alloc(ref.allocator_, sizeof(T), alignof(T));
      ASSERT(mem, "Allocation failed");
      ref.ptr_ = new(mem) T(value);

      printf("ALLOC\n");
      return ref;
    }

    static SharedRef from_ptr(T *ptr, Allocator *allocator = nullptr) {
      ASSERT(ptr, "Invalid pointer");
      SharedRef ref{};
      ref.allocator_ = allocator;
      ref.ptr_       = ptr;
      ++ref_counts_[ref.ptr_];
      return ref;
    }

    SharedRef() = default;

    SharedRef(const SharedRef &other) {
      ptr_       = other.ptr_;
      allocator_ = other.allocator_;

      ++ref_counts_[ptr_];
      printf("COPY, COUNT: %zu\n", ref_counts_[ptr_]);
    };

    SharedRef(SharedRef &&other) noexcept {
      ptr_             = other.ptr_;
      allocator_       = other.allocator_;
      other.ptr_       = nullptr;
      other.allocator_ = nullptr;

      printf("MOVE\n");
    }

    SharedRef &operator=(const SharedRef &other) {
      if (this == &other) {
        return *this;
      }

      release();

      ptr_       = other.ptr_;
      allocator_ = other.allocator_;
      ++ref_counts_[ptr_];

      return *this;
    }

    SharedRef &operator=(SharedRef &&other) noexcept {
      if (this == &other) {
        return *this;
      }

      release();

      ptr_             = other.ptr_;
      allocator_       = other.allocator_;
      other.ptr_       = nullptr;
      other.allocator_ = nullptr;

      return *this;
    }

    ~SharedRef() {
      release();
    }

    void release() {
      if (!ptr_) {
        return;
      }

      --ref_counts_[ptr_];
      if (ref_counts_[ptr_] == 0) {
        ptr_->~T();
        allocator_->release(ptr_, sizeof(T));
      }
    }

    T *get() {
      return ptr_;
    }

    const T *get() const {
      return ptr_;
    }

    T *operator ->() {
      ASSERT(ptr_, "Deref on invalid SharedRef");
      return ptr_;
    }

    const T *operator ->() const {
      ASSERT(ptr_, "Deref on invalid SharedRef");
      return ptr_;
    }

    T *operator *() {
      ASSERT(ptr_, "Deref on invalid SharedRef");
      return ptr_;
    }

    const T *operator*() const {
      ASSERT(ptr_, "Deref on invalid SharedRef");
      return ptr_;
    }

    operator bool() const {
      return ptr_;
    }
  };
}
