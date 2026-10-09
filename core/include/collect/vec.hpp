#pragma once

#include "common.hpp"
#include "memory/allocator.hpp"

namespace Flock {
    static constexpr usize VECTOR_INIT_LENGTH = 8;
    static constexpr f32   VECTOR_GROW_FACTOR = 1.4f;

    template <typename T>
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

        static Vec with_cap(usize cap) {
            Vec vector        = {};
            vector.allocator_ = Allocator::get_global();
            vector.len_       = 0;
            vector.cap_       = cap;

            vector.ptr_ = static_cast<T *>(alloc(vector.allocator_, cap * sizeof(T), alignof(T)));
            ASSERT(vector.ptr_, "Allocation failed");
            return vector;
        }

        static Vec with_len(usize len, T fill = {}) {
            Vec vector        = {};
            vector.allocator_ = Allocator::get_global();
            vector.len_       = len;
            vector.cap_       = len;

            vector.ptr_ = static_cast<T *>(alloc(vector.allocator_, len * sizeof(T), alignof(T)));
            ASSERT(vector.ptr_, "Allocation failed");

            for (usize i = 0; i < len; i++) {
                new(vector.ptr_ + i) T(fill);
            }

            return vector;
        }

        Vec() = default;

        Vec(const Vec &other) {
            allocator_ = Allocator::get_global();
            len_       = other.len_;
            cap_       = other.cap_;

            ptr_ = static_cast<T *>(alloc(allocator_, cap_ * sizeof(T), alignof(T)));
            ASSERT(ptr_, "Allocation failed");

            for (usize i = 0; i < len_; i++) {
                ptr_[i] = other[i];
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

        Vec &operator=(const Vec &other) {
            if (this == &other) {
                return *this;
            }

            free();

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

            free();

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
            free();
        }

        void free() {
            if (!ptr_) {
                return;
            }

            for (usize i = 0; i < len_; i++) {
                ptr_[i].~T();
            }

            Flock::free(allocator_, ptr_, cap_ * sizeof(T));
        }

        void grow() {
            usize new_cap = cap_ < VECTOR_INIT_LENGTH ? VECTOR_INIT_LENGTH : cap_;
            new_cap       *= VECTOR_GROW_FACTOR;

            ptr_ = static_cast<T *>(realloc(allocator_, ptr_, cap_ * sizeof(T), new_cap * sizeof(T), alignof(T)));
            cap_ = new_cap;

            ASSERT(ptr_, "Allocation failed");
        }

        void reserve(usize cap) {
            if (cap <= cap_) {
                return;
            }

            usize new_cap = cap_ < VECTOR_INIT_LENGTH ? VECTOR_INIT_LENGTH : cap_;
            while (new_cap < cap) {
                new_cap *= VECTOR_GROW_FACTOR;
            }

            ptr_ = static_cast<T *>(realloc(allocator_, ptr_, cap_ * sizeof(T), new_cap * sizeof(T), alignof(T)));
            cap_ = new_cap;

            ASSERT(ptr_, "Allocation failed");
        }

        void shrink_to(usize cap) {
            if (cap >= cap_) {
                return;
            }

            if (cap < len_) {
                for (usize i = cap; i < len_; i++) {
                    ptr_[i].~T();
                }
            }

            ptr_ = static_cast<T *>(realloc(allocator_, ptr_, cap_ * sizeof(T), cap * sizeof(T), alignof(T)));
            cap_ = cap;
        }

        void shrink_to_fit() {
            shrink_to(len_);
        }

        Vec &resize(usize len) {
            if (len > len_) {
                if (len > cap_) {
                    reserve(len);
                }

                for (usize i = len_; i < len; i++) {
                    new(ptr_ + i) T();
                }
            } else if (len < len_) {
                for (usize i = len; i < len_; i++) {
                    ptr_[i].~T();
                }
            }

            len_ = len;
            return *this;
        }

        Vec &resize(usize len, const T &fill) {
            if (len > len_) {
                if (len > cap_) {
                    reserve(len);
                }

                for (usize i = len_; i < len; i++) {
                    new(ptr_ + i) T(fill);
                }
            } else if (len < len_) {
                for (usize i = len; i < len_; i++) {
                    ptr_[i].~T();
                }
            }

            len_ = len;
            return *this;
        }

        Vec &resize(usize len, T &&fill) {
            if (len > len_) {
                if (len > cap_) {
                    reserve(len);
                }

                for (usize i = len_; i < len; i++) {
                    new(ptr_ + i) T(std::move(fill));
                }
            } else if (len < len_) {
                for (usize i = len; i < len_; i++) {
                    ptr_[i].~T();
                }
            }

            len_ = len;
            return *this;
        }

        void fill(T value) {
            for (usize i = 0; i < len_; i++) {
                ptr_[i] = value;
            }
        }

        void empty_fill() {
            for (usize i = 0; i < len_; i++) {
                ptr_[i] = {};
            }
        }

        usize len() const {
            return len_;
        }

        usize cap() const {
            return cap_;
        }

        Allocator *allocator() const {
            return allocator_;
        }

        bool is_empty() const {
            return len_ == 0;
        }

        T *get(usize idx) {
            if (idx >= len_) {
                return nullptr;
            }

            return ptr_ + idx;
        }

        T &operator[](usize idx) {
            ASSERT(idx < len_, "Out of bounds access");
            return ptr_[idx];
        }

        T *first() {
            return get(0);
        }

        T *last() {
            return get(len_ - 1);
        }

        const T *get(usize idx) const {
            if (idx >= len_) {
                return nullptr;
            }

            return ptr_ + idx;
        }

        const T &operator[](usize idx) const {
            ASSERT(idx < len_, "Out of bounds access");
            return ptr_[idx];
        }

        const T *first() const {
            return get(0);
        }

        const T *last() const {
            return get(len_ - 1);
        }

        Vec &push(const T &element = {}) {
            return resize(len_ + 1, element);
        }

        Vec &push(T &&element) {
            return resize(len_ + 1, std::move(element));
        }

        void append(const Vec vector) {
            const usize len = vector.len_;
            reserve(len_ + len);
            for (usize i = 0; i < len; i++) {
                push(vector[i]);
            }
        }

        T pop() {
            ASSERT(len_ > 0, "Pop on empty vector");
            T val = std::move(*last());
            resize(len_ - 1);
            return val;
        }

        T swap_remove(usize idx) {
            ASSERT(len_ > 0, "Swap remove on empty vector");
            ASSERT(idx < len_, "Out of bounds access");

            if (idx == len_ - 1) {
                return pop();
            }

            T val     = std::move(ptr_[idx]);
            ptr_[idx] = std::move(*last());
            len_--;

            return val;
        }

        T *begin() {
            return first();
        }

        T *end() {
            return last();
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

        const T *begin() const {
            return first();
        }

        const T *end() const {
            return last();
        }
    };
}
