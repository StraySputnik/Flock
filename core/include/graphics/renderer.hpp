#pragma once

#include "common.hpp"
#include "maybe.hpp"
#include "asset/asset.hpp"

namespace Flock::Graphics {
  using RendererID = u32;

  static constexpr RendererID INVALID_RENDERER_ID = INVALID_32;

  struct FLK_API GPUMesh {
    RendererID vert_buf_id     = INVALID_RENDERER_ID;
    RendererID norm_buf_id     = INVALID_RENDERER_ID;
    RendererID texcoord_buf_id = INVALID_RENDERER_ID;
    RendererID idx_buf_id      = INVALID_RENDERER_ID;
  };

  struct FLK_API GPUImage2D {
    RendererID id = INVALID_RENDERER_ID;
  };

  using GPUImage = GPUImage2D;

  struct FLK_API RenderObject {
    GPUMesh           mesh;
    Maybe<GPUImage2D> texture;
  };

  struct FLK_API RenderScene {
    Vec<RenderObject> objects = {};
  };

  struct FLK_API RenderConfig {
    Color4u8 clear_color  = Color4u8::rgba(20, 40, 40, 255);
    u8       sample_count = 1;
  };

  class FLK_API Renderer {
  public:
    virtual ~Renderer() = default;

    virtual GPUMesh    upload_mesh(const Mesh &mesh) = 0;
    virtual GPUImage2D upload_texture(const Image &image) = 0;

    virtual void render(const RenderScene &scene) = 0;
  };
}

namespace Flock {
  using namespace Graphics;
}
