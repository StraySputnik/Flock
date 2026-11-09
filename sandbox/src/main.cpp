#include <app.hpp>
#include <asset/asset.hpp>

using namespace Flock;

i32 main() {
  AssetLoader loader = AssetLoader::create("../../assets").move();
  Material *  mat    = loader.resolve(AssetHandle<Material>::from_path("mat.mat"));

  App app = App::create();
  app.run();
}
