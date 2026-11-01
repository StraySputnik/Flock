#pragma once

#include "common.hpp"
#include "platform/window.hpp"

namespace Flock::Graphics::Sokol::WGPU {
  struct FLK_API Context {
    void *instance     = nullptr;
    void *adapter      = nullptr;
    void *device       = nullptr;
    i32   color_format = 0;
  };

  Maybe<Context> create_ctx(const Window &window);
  void           release_ctx(Context &ctx);

  struct FLK_API Texture {
    void *texture = nullptr;
    void *view    = nullptr;
  };

  void release_texture(Texture &texture);

  enum PresentMode : u8 {
    FIFO        = 0x00000001,
    FIFORelaxed = 0x00000002,
    Immediate   = 0x00000003,
    Mailbox     = 0x00000004,
  };

  struct FLK_API SurfaceConfig {
    u32         width        = 0;
    u32         height       = 0;
    PresentMode present_mode = FIFO;
  };

  struct FLK_API Surface {
    void *ptr = nullptr;
  };

  Surface create_surface(const Context &ctx, const Window &window);
  void    configure_surface(const Context &ctx, const Surface &surface, SurfaceConfig config);
  void    present_surface(const Surface &surface);
  void    release_surface(Surface &surface);

  Maybe<i32>     get_surface_format(const Context &ctx, const Surface &surface);
  Maybe<Texture> get_surface_texture(const Surface &surface);
  Texture        create_msaa_target(const Context &ctx, u32 width, u32 height, usize sample_count);
}
