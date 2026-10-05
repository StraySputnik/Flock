#include "common.hpp"
#include "collect/string.hpp"
#include "ecs/registry.hpp"
#include "memory/allocator.hpp"

using namespace Flock;

i32 main() {
    BumpAllocator allocator = BumpAllocator::create(65'536).move();
    allocator.set_global();

    auto        registry = ECS::Registry::create<String>();
    ECS::Entity e       = registry.spawn();

    registry.add(e, String::from("Hello, World!"));
    registry.get<String>(e)->push('\0');

    printf("%s\n", registry.get<String>(e)->first());
}
