#pragma once

#include "common.hpp"
#include "graphics.hpp"
#include "math/color.hpp"

namespace Flock::Graphics {
  struct FLK_API RenderConfig {
    Color4u8 clear_color  = Color4u8::rgba(20, 40, 40, 255);
    u8       sample_count = 4;
  };

  struct FLK_API RenderObject {
    Matrix4f  transform = Matrix4f::identity();
    Mesh *    mesh      = nullptr;
    Material *material  = nullptr;

    [[nodiscard]] bool is_valid() const {
      return mesh;
    }
  };

  struct FLK_API RenderScene {
    Vec<RenderObject> objects    = {};
    Matrix4f          view       = Matrix4f::identity();
    Matrix4f          projection = Matrix4f::identity();
  };

  class FLK_API Renderer {
  public:
    virtual ~Renderer() = default;

    virtual void configure(const RenderConfig &config) = 0;
    virtual void render(const RenderScene &scene) = 0;
  };
}

namespace Flock {
  using namespace Graphics;
}
