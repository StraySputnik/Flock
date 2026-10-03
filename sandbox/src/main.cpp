#include "common.hpp"
#include "collect/map.hpp"
#include "collect/string.hpp"
#include "memory/allocator.hpp"

using namespace Flock;

i32 main() {
    Allocator allocator = allocator_create(AllocatorType::BumpAllocator, 64'000).value;
    push_allocator(&allocator);

    auto map = map_create<char, i32>();
    insert(&map, 'A', 1);
    insert(&map, 'B', 2);
    insert(&map, 'C', 3);
    insert(&map, 'D', 4);
    insert(&map, 'E', 5);
    insert(&map, 'F', 6);
    insert(&map, 'G', 7);
    insert(&map, 'H', 8);
    insert(&map, 'I', 9);
    insert(&map, 'J', 10);
    insert(&map, 'K', 11);
    insert(&map, 'L', 12);
    insert(&map, 'M', 13);
    insert(&map, 'N', 14);
    remove(&map, 'A');

    MAP_FOREACH(
        &map, c, const v,
        printf("%c: %i\n", *c, *v);
    );

    map_delete(&map);

    pop_allocator();
}
