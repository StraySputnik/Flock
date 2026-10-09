#pragma once

#include "allocator.hpp"
#include "common.hpp"
#include "collect/map.hpp"

namespace Flock {
    class Allocator;

    template <typename T>
    class OwnedRef {
        T *        ptr_       = nullptr;
        Allocator *allocator_ = nullptr;

    public:
        static OwnedRef from_value(const T &value) {
            OwnedRef ref{};
            ref.allocator_ = Allocator::get_global();
            ref.ptr_       = static_cast<T *>(alloc(ref.allocator_, sizeof(T), alignof(T)));
            ASSERT(ref.ptr_, "Allocation failed");
            *ref.ptr_ = value;
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

            free();

            ptr_             = other.ptr_;
            allocator_       = other.allocator_;
            other.ptr_       = nullptr;
            other.allocator_ = nullptr;

            return *this;
        }

        ~OwnedRef() {
            free();
        }

        void free() {
            if (!ptr_) {
                return;
            }

            ptr_->~T();
            allocator_->free(ptr_, sizeof(T));
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

        T &operator *() {
            ASSERT(ptr_, "Deref on invalid OwnedRef");
            return ptr_;
        }

        const T &operator*() const {
            ASSERT(ptr_, "Deref on invalid OwnedRef");
            return ptr_;
        }

        operator bool() const {
            return ptr_;
        }
    };

    // Not atomic
    template <typename T>
    class SharedRef {
        inline static Map<T *, usize> ref_counts_ = {};

        T *        ptr_       = nullptr;
        Allocator *allocator_ = nullptr;

    public:
        static SharedRef from_value(const T &value) {
            SharedRef ref{};
            ref.allocator_ = Allocator::get_global();
            ref.ptr_       = static_cast<T *>(alloc(ref.allocator_, sizeof(T), alignof(T)));
            ASSERT(ref.ptr_, "Allocation failed");
            *ref.ptr_ = value;

            ref_counts_[ref.ptr_]++;
            return ref;
        }

        static SharedRef from_ptr(T *ptr, Allocator *allocator = nullptr) {
            ASSERT(ptr, "Invalid pointer");
            SharedRef ref{};
            ref.allocator_ = allocator;
            ref.ptr_       = ptr;
            ref_counts_[ref.ptr_]++;
            return ref;
        }

        SharedRef() = default;

        SharedRef(const SharedRef &other) {
            ptr_       = other.ptr_;
            allocator_ = other.allocator_;

            ref_counts_[ptr_]++;
        };

        SharedRef(SharedRef &&other) noexcept {
            ptr_             = other.ptr_;
            allocator_       = other.allocator_;
            other.ptr_       = nullptr;
            other.allocator_ = nullptr;
        }

        SharedRef &operator=(const SharedRef &other) {
            if (this == &other) {
                return *this;
            }

            free();

            ptr_       = other.ptr_;
            allocator_ = other.allocator_;
            ref_counts_[ptr_]++;

            return *this;
        }

        SharedRef &operator=(SharedRef &&other) noexcept {
            if (this == &other) {
                return *this;
            }

            free();

            ptr_             = other.ptr_;
            allocator_       = other.allocator_;
            other.ptr_       = nullptr;
            other.allocator_ = nullptr;

            return *this;
        }

        ~SharedRef() {
            free();
        }

        void free() {
            if (!ptr_) {
                return;
            }

            ref_counts_[ptr_]--;
            if (ref_counts_[ptr_] == 0) {
                ptr_->~T();
                allocator_->free(ptr_, sizeof(T));
            }
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

        T &operator *() {
            ASSERT(ptr_, "Deref on invalid OwnedRef");
            return ptr_;
        }

        const T &operator*() const {
            ASSERT(ptr_, "Deref on invalid OwnedRef");
            return ptr_;
        }

        operator bool() const {
            return ptr_;
        }
    };
}
