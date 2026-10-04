#include "common.hpp"
#include "ecs/group.hpp"
#include "memory/allocator.hpp"

using namespace Flock;

i32 main() {
    Allocator allocator = allocator_create(AllocatorType::BumpAllocator, 64'000).value;
    push_allocator(&allocator);

    auto group = ECS::group_create<char>();
    insert(&group, 1, 'A');
    insert(&group, 2, 'B');
    insert(&group, 3, 'C');
    insert(&group, 4, 'D');
    insert(&group, 5, 'E');
    insert(&group, 6, 'F');
    insert(&group, 7, 'G');
    insert(&group, 8, 'H');
    insert(&group, 9, 'I');
    insert(&group, 10, 'J');
    insert(&group, 11, 'K');
    insert(&group, 12, 'L');
    insert(&group, 13, 'M');
    insert(&group, 14, 'N');
    remove(&group, 1);

    for_each_i(
        &group, +[](const ECS::EntityID id, const char *c) {
            printf("%lu: %c\n", id, *c);
        }
    );

    group_delete(&group);

    pop_allocator();
}
