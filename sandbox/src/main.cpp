#include "app.hpp"
#include "common.hpp"
#include "memory/allocator.hpp"

using namespace Flock;

i32 main() {
    BumpAllocator allocator = BumpAllocator::create(65'536).move();
    allocator.set_global();
    App::create().run();
}
