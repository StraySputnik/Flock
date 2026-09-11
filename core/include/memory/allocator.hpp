#ifndef FLOCK_ALLOCATOR_HPP
#define FLOCK_ALLOCATOR_HPP

#include "common.hpp"
#include "result.hpp"

namespace flock::memory {
    enum class error : u8 {
        NONE,
        ALLOCATION_FAILED,
        MEMORY_OVERFLOW,
    };

    using alloc_result = result<byte *, error>;

    void *system_allocate(usize size);
    void *system_reallocate(void *ptr, usize dest_size);
    void  system_deallocate(void *ptr);

    class FLK_API allocator {
    public:
        virtual ~allocator() = default;

        virtual void clear() = 0;

        virtual alloc_result allocate(usize size) = 0;
        virtual alloc_result allocate(usize size, usize alignment) = 0;
        virtual alloc_result reallocate(byte *ptr, usize src_size, usize dest_size) = 0;
        virtual void         deallocate(byte *ptr, usize size) = 0;
    };

    FLK_API alloc_result allocate(allocator *allocator, usize size);
    FLK_API alloc_result reallocate(allocator *allocator, byte *ptr, usize src_size, usize dest_size);
    FLK_API void         deallocate(allocator *allocator, byte *ptr, usize size);

    FLK_API void       push_allocator(allocator *allocator);
    FLK_API allocator *get_allocator();
    FLK_API void       pop_allocator();

    class FLK_API arena_allocator : public allocator {
        // TODO: Lazy initialization might be necessary.

        byte *     region_    = nullptr;
        usize      size_      = 0;
        usize      index_     = 0;
        allocator *allocator_ = nullptr;

        arena_allocator() = default;

    public:
        static result<arena_allocator, error> create(usize size);

        arena_allocator(const arena_allocator &other) = delete;
        arena_allocator(arena_allocator &&other) noexcept;

        arena_allocator &operator=(const arena_allocator &other) = delete;
        arena_allocator &operator=(arena_allocator &&other) noexcept;

        ~arena_allocator() override;

        void clear() override;

        alloc_result allocate(usize size) override;
        alloc_result allocate(usize size, usize alignment) override;
        alloc_result reallocate(byte *ptr, usize src_size, usize dest_size) override;
        void         deallocate(byte *ptr, usize size) override;
    };
}

#endif //FLOCK_ALLOCATOR_HPP
