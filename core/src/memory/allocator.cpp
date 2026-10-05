#include "memory/allocator.hpp"

#include <cstring>
#include <utility>

namespace Flock {
    void *sys_alloc(usize size) {
        return malloc(size);
    }

    void *sys_realloc(void *allocation, usize size) {
        return ::realloc(allocation, size);
    }

    void sys_free(void *allocation) {
        ::free(allocation);
    }

    namespace {
        struct AllocatorNode {
            Allocator *    allocator = nullptr;
            AllocatorNode *next      = nullptr;
        };
    }

    static AllocatorNode *base = nullptr;

    void Allocator::push_global(Allocator *allocator) {
        void *ptr = sys_alloc(sizeof(AllocatorNode));
        ASSERT(ptr, "Allocation failed");

        auto *node      = (AllocatorNode *)ptr;
        node->next      = base;
        node->allocator = allocator;
        base            = node;
    }

    void Allocator::pop_global() {
        if (!base) {
            return;
        }

        AllocatorNode *next = base->next;
        sys_free(base);
        base = next;
    }

    Allocator *Allocator::get_global() {
        if (!base) {
            return nullptr;
        }

        return base->allocator;
    }

    void *alloc(Allocator *allocator, usize size, usize align) {
        if (!allocator) {
            return sys_alloc(size);
        }

        return allocator->alloc(size, align);
    }

    void *realloc(Allocator *allocator, void *src, usize src_size, usize dest_size, usize align) {
        if (!allocator) {
            return sys_realloc(src, dest_size);
        }

        return allocator->realloc(src, src_size, dest_size, align);
    }

    void free(Allocator *allocator, void *ptr, usize size) {
        if (!allocator) {
            return sys_free(ptr);
        }

        return allocator->free(ptr, size);
    }

    Maybe<BumpAllocator> BumpAllocator::create(usize size) {
        void *allocation = sys_alloc(size);
        if (!allocation) {
            return {};
        }

        BumpAllocator allocator = {};
        allocator.ptr_          = (byte *)allocation;
        allocator.size_         = size;
        allocator.offset_       = 0;

        return allocator;
    }

    BumpAllocator::BumpAllocator(BumpAllocator &&other) noexcept {
        ptr_    = other.ptr_;
        size_   = other.size_;
        offset_ = other.offset_;

        other.ptr_    = nullptr;
        other.size_   = 0;
        other.offset_ = 0;
    }

    BumpAllocator &BumpAllocator::operator=(BumpAllocator &&other) noexcept {
        if (this == &other) {
            return *this;
        }

        clear();

        ptr_    = other.ptr_;
        size_   = other.size_;
        offset_ = other.offset_;

        other.ptr_    = nullptr;
        other.size_   = 0;
        other.offset_ = 0;

        return *this;
    }

    BumpAllocator::~BumpAllocator() {
        clear();
    }

    void *BumpAllocator::alloc(usize size, usize align) {
        ASSERT(ptr_, "Operation on an uninitialized object");

        usize allocation_offset = align - (usize)(ptr_ + offset_) % align;
        allocation_offset       = allocation_offset == align ? 0 : allocation_offset;

        if (size_ < offset_ + allocation_offset + size) {
            return nullptr;
        }

        void *ptr = ptr_ + offset_ + allocation_offset;
        offset_   += allocation_offset + size;

        return ptr;
    }

    void *BumpAllocator::realloc(void *src, usize src_size, usize dest_size, usize align) {
        ASSERT(ptr_, "Operation on an uninitialized object");

        void *dest = alloc(dest_size, align);
        if (!dest) {
            return nullptr;
        }

        const usize min_size = src_size < dest_size ? src_size : dest_size;
        memcpy(dest, src, min_size);

        return dest;
    }

    void BumpAllocator::free(void *ptr, usize size) {
        ASSERT(ptr_, "Operation on an uninitialized object");
        // No-op
    }

    void BumpAllocator::clear() {
        if (ptr_) {
            sys_free(ptr_);
        }

        if (get_global() == this) {
            pop_global();
        }

        ptr_    = nullptr;
        size_   = 0;
        offset_ = 0;
    }

    void BumpAllocator::set_global() {
        push_global(this);
    }
}
