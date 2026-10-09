#include "app.hpp"
#include "common.hpp"
#include "math/matrix.hpp"
#include "math/vector.hpp"
#include "math/quaternion.hpp"

using namespace Flock;

i32 main() {
    //BumpAllocator allocator = BumpAllocator::create(65'536).move();
    //allocator.set_global();
    //
    //App::create().run();

    auto vec  = Vector3f::xyz(2, 1, 1);
    auto quat = Quaternion::euler(180, 0, 0) * Quaternion::euler(0, 180, 0);

    vec *= quat;
    ASSERT(Vector3f::distance(vec, Vector3f::xyz(-2, -1, 1)) < Math::EPSILON_32, "");
}
