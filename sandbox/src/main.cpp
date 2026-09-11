#include <cstdio>

#include "collect/list.hpp"
#include "memory/allocator.hpp"
#include "collect/string.hpp"

using namespace flock;
using namespace flock::memory;

i32 main() {
    arena_allocator alloc = std::move(arena_allocator::create(1024).get());

    list<i32> list = ::list<i32>::create();
    list.push_last(1);
    list.push_last(2);
    list.push_last(3);

    for (usize i = 0; i < list.len(); i++) {
        printf("%i\n", list[i]);
    }
}
