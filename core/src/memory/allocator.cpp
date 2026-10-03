#include "memory/allocator.hpp"

#include <cstring>

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

    Maybe<Allocator> allocator_create(AllocatorType type, usize size) {
        void *allocation = sys_alloc(size);
        if (!allocation) {
            return {};
        }

        Allocator allocator = {};
        allocator.type      = type;
        allocator.ptr       = (byte *)allocation;
        allocator.size      = size;

        switch (type) {
        case AllocatorType::BumpAllocator:
            allocator.bump = {};
            break;
        default:
            PANIC();
        }

        return maybe(allocator);
    }

    void allocator_delete(Allocator *allocator) {
        sys_free(allocator->ptr);

        switch (allocator->type) {
        case AllocatorType::BumpAllocator:
            allocator->bump = {};
            break;
        default:
            PANIC();
        }
    }

    static void *bump_alloc(Allocator *allocator, usize size, usize align) {
        auto        allocator_ptr    = allocator->ptr;
        const usize allocator_size   = allocator->size;
        const usize allocator_offset = allocator->bump.offset;

        usize allocation_offset = align - (usize)(allocator_ptr + allocator_offset) % align;
        allocation_offset       = allocation_offset == align ? 0 : allocation_offset;

        if (allocator_size < allocator_offset + allocation_offset + size) {
            return nullptr;
        }

        void *ptr              = allocator_ptr + allocator_offset + allocation_offset;
        allocator->bump.offset += allocation_offset + size;

        return ptr;
    }

    static void *bump_realloc(Allocator *allocator, const void *src, usize src_size, usize dest_size, usize align) {
        void *dest = bump_alloc(allocator, dest_size, align);
        if (!dest) {
            return nullptr;
        }

        const usize min_size = src_size < dest_size ? src_size : dest_size;
        memcpy(dest, src, min_size);

        return dest;
    }

    static void bump_free(Allocator *allocator, void *allocation, usize size) {
        // No-op
    }

    void *alloc(Allocator *allocator, usize size, usize align) {
        if (!allocator) {
            return sys_alloc(size);
        }

        switch (allocator->type) {
        case AllocatorType::BumpAllocator:
            return bump_alloc(allocator, size, align);
            break;
        default:
            PANIC();
        }
    }

    void *realloc(Allocator *allocator, void *src, usize src_size, usize dest_size, usize align) {
        if (!allocator) {
            return sys_realloc(src, dest_size);
        }

        switch (allocator->type) {
        case AllocatorType::BumpAllocator:
            return bump_realloc(allocator, src, src_size, dest_size, align);
            break;
        default:
            PANIC();
        }
    }

    void free(Allocator *allocator, void *allocation, usize size) {
        if (!allocator) {
            return sys_free(allocation);
        }

        switch (allocator->type) {
        case AllocatorType::BumpAllocator:
            return bump_free(allocator, allocation, size);
            break;
        default:
            PANIC();
        }
    }

    namespace {
        struct AllocatorNode {
            Allocator *    allocator = nullptr;
            AllocatorNode *next      = nullptr;
        };
    }

    static AllocatorNode *base = nullptr;

    void push_allocator(Allocator *allocator) {
        void *ptr = sys_alloc(sizeof(AllocatorNode));
        ASSERT(ptr, "Allocation failed");

        auto *node      = (AllocatorNode *)ptr;
        node->next      = base;
        node->allocator = allocator;
        base            = node;
    }

    void pop_allocator() {
        if (!base) {
            return;
        }

        AllocatorNode *next = base->next;
        sys_free(base);
        base = next;
    }

    Allocator *get_allocator() {
        if (!base) {
            return nullptr;
        }

        return base->allocator;
    }
}
