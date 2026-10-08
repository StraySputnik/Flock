#include "app.hpp"
#include "common.hpp"
#include "math/matrix.hpp"
#include "math/vector.hpp"
#include "memory/allocator.hpp"

using namespace Flock;

i32 main() {
    BumpAllocator allocator = BumpAllocator::create(65'536).move();
    allocator.set_global();
    Vector3f vec = {2, 1, 0};
    Matrix4f mat = Matrix4f::rotation(180, 0, 0);
    vec          *= mat;
    ASSERT((vec == Vector3f{2, -1, 0}), "");
    App::create().run();
}
