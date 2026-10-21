#include "app.hpp"
#include "common.hpp"
#include "asset/asset.hpp"
#include "debug/log.hpp"

using namespace Flock;

i32 main() {
  Debug::Logger::create("Flock", {});
  BumpAllocator allocator = BumpAllocator::create(4096).move();
  {
    AssetLoader loader = AssetLoader::create("../../assets/").move();

    loader.set_allocator(AssetType::Mesh, &allocator);
    AssetHandle<Mesh> mesh     = AssetHandle<Mesh>::from_path("mesh.obj");
    Vec<Vector3f>     vertices = loader.resolve(mesh)->vertices;
  }

  allocator.clear();
}
