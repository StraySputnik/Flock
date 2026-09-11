#include "memory/allocator.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace flock::memory {
    // empty namespace to enforce internal linkage
    namespace {
        struct allocator_node {
            allocator *     alloc;
            allocator_node *next;
        };
    }

    static allocator_node *head = nullptr;

    void *system_allocate(usize size) {
        return malloc(size);
    }

    void *system_reallocate(void *ptr, usize dest_size) {
        return realloc(ptr, dest_size);
    }

    void system_deallocate(void *ptr) {
        free(ptr);
    }

    alloc_result allocate(allocator *allocator, usize size) {
        if (allocator) {
            return allocator->allocate(size);
        }

        void *ptr = system_allocate(size);
        if (!ptr) {
            return error::ALLOCATION_FAILED;
        }

        return (byte *)ptr;
    }

    alloc_result reallocate(allocator *allocator, byte *ptr, usize src_size, usize dest_size) {
        if (allocator) {
            return allocator->reallocate(ptr, src_size, dest_size);
        }

        void *new_ptr = system_reallocate(ptr, dest_size);
        if (!new_ptr) {
            return error::ALLOCATION_FAILED;
        }

        return (byte *)new_ptr;
    }

    void deallocate(allocator *allocator, byte *ptr, usize size) {
        if (allocator) {
            allocator->deallocate(ptr, size);
            return;
        }

        system_deallocate(ptr);
    }

    void push_allocator(allocator *allocator) {
        if (!head) {
            head        = (allocator_node *)system_allocate(sizeof(allocator_node));
            head->alloc = allocator;
            head->next  = nullptr;
        } else {
            auto *node  = (allocator_node *)system_allocate(sizeof(allocator_node));
            node->alloc = allocator;
            node->next  = head;

            head = node;
        }
    }

    allocator *get_allocator() {
        if (!head) {
            return nullptr;
        }

        return head->alloc;
    }

    void pop_allocator() {
        allocator_node *node = head;
        head                 = node->next;
        system_deallocate(node);
    }

    result<arena_allocator, error> arena_allocator::create(usize size) {
        arena_allocator allocator;

        allocator.allocator_ = get_allocator();

        alloc_result res = memory::allocate(allocator.allocator_, size);
        if (!res) {
            return res.get_err();
        }

        allocator.region_ = res.get();
        allocator.size_   = size;
        allocator.index_  = 0;

        push_allocator(&allocator);

        return allocator;
    }

    arena_allocator::arena_allocator(arena_allocator &&other) noexcept {
        allocator_ = other.allocator_;
        region_    = other.region_;
        size_      = other.size_;
        index_     = other.index_;

        other.allocator_ = nullptr;
        other.region_    = nullptr;
        other.size_      = 0;
        other.index_     = 0;

        if (get_allocator() == &other) {
            pop_allocator();
            push_allocator(this);
        }
    }

    arena_allocator &arena_allocator::operator=(arena_allocator &&other) noexcept {
        if (this == &other) {
            return *this;
        }

        memory::deallocate(allocator_, region_, size_);

        allocator_ = other.allocator_;
        region_    = other.region_;
        size_      = other.size_;
        index_     = other.index_;

        other.allocator_ = nullptr;
        other.region_    = nullptr;
        other.size_      = 0;
        other.index_     = 0;

        if (get_allocator() == &other) {
            pop_allocator();
            push_allocator(this);
        }

        return *this;
    }

    arena_allocator::~arena_allocator() {
        if (get_allocator() == this) {
            pop_allocator();
        }

        memory::deallocate(allocator_, region_, size_);
    }

    void arena_allocator::clear() {
        index_ = 0;
    }

    alloc_result arena_allocator::allocate(usize size) {
        return allocate(size, alignof(usize));
    }

    alloc_result arena_allocator::allocate(usize size, usize alignment) {
        usize offset = alignment - (usize)(region_ + index_) % alignment;
        offset       = offset == alignment ? 0 : offset;

        if (size_ < index_ + offset + size) {
            return error::MEMORY_OVERFLOW;
        }

        byte *ptr = region_ + index_ + offset;
        index_    += offset + size;

        return ptr;
    }

    alloc_result arena_allocator::reallocate(byte *ptr, usize src_size, usize dest_size) {
        alloc_result result = allocate(dest_size);
        if (!result) {
            return result;
        }

        byte *      dest_ptr = result.get();
        const usize min_size = src_size < dest_size ? src_size : dest_size;
        memcpy(dest_ptr, ptr, min_size);

        return dest_ptr;
    }

    void arena_allocator::deallocate(byte *ptr, usize size) {
        // No-op
    }
}
