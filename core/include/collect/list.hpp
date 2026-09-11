#ifndef FLOCK_LIST_HPP
#define FLOCK_LIST_HPP

#include "common.hpp"
#include "memory/allocator.hpp"

namespace flock {
    template <typename T>
    class list {
    public:
        struct element {
            T        value;
            element *next;
            element *prev;
        };

    private:
        element *          head_      = nullptr;
        memory::allocator *allocator_ = nullptr;
        usize              len_       = 0;

    public:
        static list create() {
            list out;
            out.allocator_ = memory::get_allocator();
            out.head_      = nullptr;
            out.len_       = 0;

            return out;
        }

        template <typename... Args>
            requires (std::same_as<Args, T> && ...)
        static list with_elements(Args... args) {
            list out;
            out.allocator_ = memory::get_allocator();
            out.head_      = nullptr;
            out.len_       = 0;

            (out.push_last(args), ...);
            return out;
        }

        list(const list &other) {
            allocator_ = memory::get_allocator();
            head_      = nullptr;
            len_       = 0;

            element *elem = other.head_;
            for (usize i = 0; i < other.len_; i++) {
                push_last(elem->value);
                elem = elem->next;
            }
        }

        list(list &&other) noexcept {
            allocator_ = other.allocator_;
            head_      = other.head_;
            len_       = other.len_;

            other.allocator_ = nullptr;
            other.head_      = nullptr;
            other.len_       = 0;
        }

        list &operator=(const list &other) {
            if (this == &other) {
                return *this;
            }

            clear();

            allocator_ = memory::get_allocator();
            head_      = nullptr;
            len_       = 0;

            element *elem = other.head_;
            for (usize i = 0; i < other.len_; i++) {
                push_last(elem->value);
                elem = elem->next;
            }

            return *this;
        }

        list &operator=(list &&other) noexcept {
            if (this == &other) {
                return *this;
            }

            clear();

            allocator_ = other.allocator_;
            head_      = other.head_;
            len_       = other.len_;

            other.allocator_ = nullptr;
            other.head_      = nullptr;
            other.len_       = 0;

            return *this;
        }

        ~list() {
            clear();
        }

        usize len() const {
            return len_;
        }

        bool is_empty() const {
            return len_ == 0;
        }

        const T *first() const {
            return get(0);
        }

        T *first() {
            return get(0);
        }

        const T *last() const {
            return get(len_ - 1);
        }

        T *last() {
            return get(len_ - 1);
        }

        const T *get(usize index) const {
            if (!head_) {
                return nullptr;
            }

            element *elem = head_;
            usize    i    = 0;
            while (i < index) {
                elem = elem->next;

                if (!elem) {
                    return nullptr;
                }

                i++;
            }

            return &elem->value;
        }

        T *get(usize index) {
            if (!head_) {
                return nullptr;
            }

            element *elem = head_;
            usize    i    = 0;
            while (i < index) {
                elem = elem->next;

                if (!elem) {
                    return nullptr;
                }

                i++;
            }

            return &elem->value;
        }

        const T &operator[](usize index) const {
            FLK_ASSERT(index < len_, "Out of bounds index");
            return *get(index);
        }

        T &operator[](usize index) {
            FLK_ASSERT(index < len_, "Out of bounds index");
            return *get(index);
        }

        void resize(usize new_len, const T &value = {}) {
            while (new_len > 0) {
                push_last(value);
                new_len--;
            }
        }

        void push_last(const T &value = {}) {
            if (len_ == 0) {
                head_ = (element *)allocator_->allocate(sizeof(element), alignof(element)).get();
                new(&head_->value) T(value);
                head_->prev = nullptr;
                head_->next = nullptr;
            } else {
                element *elem = last();
                elem->next    = (element *)allocator_->allocate(sizeof(element), alignof(element)).get();
                new(&elem->next->value) T(value);
                elem->next->prev = elem;
                elem->next->next = nullptr;
            }

            len_++;
        }

        void push_last(T &&value) {
            if (len_ == 0) {
                head_ = (element *)allocator_->allocate(sizeof(element), alignof(element)).get();
                new(&head_->value) T(std::move(value));
                head_->prev = nullptr;
                head_->next = nullptr;
            } else {
                element *elem = last();
                elem->next    = (element *)allocator_->allocate(sizeof(element), alignof(element)).get();
                new(&elem->next->value) T(std::move(value));
                elem->next->prev = elem;
                elem->next->next = nullptr;
            }

            len_++;
        }

        void push_first(const T &value = {}) {
            element *new_head = (element *)allocator_->allocate(sizeof(element), alignof(element)).get();
            new(&new_head->value) T(value);
            new_head->prev = nullptr;
            new_head->next = head_;

            if (head_) {
                head_->prev = new_head;
                head_       = new_head;
            }

            len_++;
        }

        void push_first(T &&value) {
            element *new_head = (element *)allocator_->allocate(sizeof(element), alignof(element)).get();
            new(&new_head->value) T(std::move(value));
            new_head->prev = nullptr;
            new_head->next = head_;

            if (head_) {
                head_->prev = new_head;
                head_       = new_head;
            }

            len_++;
        }

        T pop_last() {
            element *elem = get(len_ - 2);
            T        val  = elem->next->value;
            elem->next->value.~T();
            allocator_->deallocate(elem->next, sizeof(element));
            elem->next = nullptr;

            len_--;
            return val;
        }

        T pop_first() {
            T val = head_->value;
            head_->value.~T();
            element *new_head = head_->next;
            allocator_->deallocate(head_, sizeof(element));

            head_ = new_head;
            len_--;
            return val;
        }

        void insert(usize index, const T &value = {}) {
            FLK_ASSERT(index <= len_, "Out of bounds index");
            if (index == len_) {
                return push_last(value);
            }

            element *prev = get(index - 1);
            element *next = prev->next;
            element *elem = (element *)allocator_->allocate(sizeof(element), alignof(element)).get();
            new(&elem->value) T(value);
            elem->next = next;
            elem->prev = prev;

            prev->next = elem;
            next->prev = elem;
        }

        void insert(usize index, T &&value) {
            FLK_ASSERT(index <= len_, "Out of bounds index");
            if (index == len_) {
                return push_last(std::move(value));
            }

            element *prev = get(index - 1);
            element *next = prev->next;
            element *elem = (element *)allocator_->allocate(sizeof(element), alignof(element)).get();
            new(&elem->value) T(std::move(value));
            elem->next = next;
            elem->prev = prev;

            prev->next = elem;
            next->prev = elem;
        }

        T remove(usize index) {
            FLK_ASSERT(index <= len_, "Out of bounds index");
            if (index == len_) {
                return pop_last();
            }

            element *prev = get(index - 1);
            element *elem = prev->next;
            element *next = elem->next;

            T value = elem->value;
            elem->value.~T();
            allocator_->deallocate(elem, sizeof(element));

            prev->next = next;
            next->prev = prev;

            return value;
        }

        void removen(usize index, usize num) {
            FLK_ASSERT(index + num - 1 <= len_, "Out of bounds index");
            while (num > 0) {
                remove(index);
                num--;
            }
        }

        void append(const list &other) {
            const element *elem = other.head_;
            while (elem) {
                push_last(elem->value);
                elem = elem->next;
            }
        }

        void clear() {
            while (len_ > 0) {
                pop_last();
            }
        }

    private:
        list() = default;
    };
}

#endif //FLOCK_LIST_HPP
