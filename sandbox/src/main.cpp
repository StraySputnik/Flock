#include <cstdio>

#include "memory/allocator.hpp"
#include "collect/string.hpp"

using namespace flock;
using namespace flock::memory;

i32 main() {
    arena_allocator alloc = std::move(arena_allocator::create(1024).get());

    string str = string::from("Hello, World!");

    str.insert_str(5, string::from("pe"));

    str.remove(5);
    str.remove(5);
    str.remove(5);

    str.pop();
    str.make_lowercase();

    str.replace(string::from("hello"), string::from("hi"));
    str.replace(string::from("world"), string::from("mom"));

    for (usize i = 0; i < str.len(); i++) {
        printf("%c", str[i]);
    }

    printf("\n");
}
