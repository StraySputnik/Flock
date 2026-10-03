#pragma once

#include "common.hpp"
#include "maybe.hpp"

namespace Flock {
    void *sys_alloc(usize size);
    void *sys_realloc(void *allocation, usize size);
    void  sys_free(void *allocation);

    enum class AllocatorType : u8 {
        BumpAllocator,
    };

    struct BumpAllocatorData {
        usize offset = 0;
    };

    struct Allocator {
        AllocatorType type = {};
        byte *        ptr  = nullptr;
        usize         size = 0;

        union {
            BumpAllocatorData bump;
        };
    };

    Maybe<Allocator> allocator_create(AllocatorType type, usize size);
    void             allocator_delete(Allocator *allocator);

    void *alloc(Allocator *allocator, usize size, usize align = alignof(usize));
    void *realloc(Allocator *allocator, void *src, usize src_size, usize dest_size, usize align = alignof(usize));
    void  free(Allocator *allocator, void *allocation, usize size);

    void       push_allocator(Allocator *allocator);
    void       pop_allocator();
    Allocator *get_allocator();
}
