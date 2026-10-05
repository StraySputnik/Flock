#pragma once

#include "common.hpp"
#include "maybe.hpp"

namespace Flock {
    void *sys_alloc(usize size);
    void *sys_realloc(void *allocation, usize size);
    void  sys_free(void *allocation);

    class Allocator {
    public:
        static void       push_global(Allocator *allocator);
        static void       pop_global();
        static Allocator *get_global();

        virtual ~Allocator() = default;

        virtual void *alloc(usize size, usize align) = 0;
        virtual void *realloc(void *src, usize src_size, usize dest_size, usize align) = 0;
        virtual void  free(void *ptr, usize size) = 0;
        virtual void  clear() = 0;
    };

    void *alloc(Allocator *allocator, usize size, usize align = alignof(usize));
    void *realloc(Allocator *allocator, void *src, usize src_size, usize dest_size, usize align = alignof(usize));
    void  free(Allocator *allocator, void *ptr, usize size);

    class BumpAllocator : public Allocator {
        byte *ptr_    = nullptr;
        usize size_   = 0;
        usize offset_ = 0;

    public:
        static Maybe<BumpAllocator> create(usize size);

        BumpAllocator() = default;

        BumpAllocator(const BumpAllocator &other) = delete;
        BumpAllocator(BumpAllocator &&other) noexcept;

        BumpAllocator &operator=(const BumpAllocator &other) = delete;
        BumpAllocator &operator=(BumpAllocator &&other) noexcept;

        ~BumpAllocator() override;

        void *alloc(usize size, usize align = alignof(usize)) override;
        void *realloc(void *src, usize src_size, usize dest_size, usize align = alignof(usize)) override;
        void  free(void *ptr, usize size) override;
        void  clear() override;

        void set_global();
    };
}
