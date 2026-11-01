#include "graphics/sokol/sokol_renderer.hpp"

#include <sokol_gfx.h>

#include "webgpu/webgpu.h"

namespace Flock::Graphics {
  static sg_pixel_format wgpu_to_sg_format(const i32 format) {
    switch (format) {
      case WGPUTextureFormat_BGRA8Unorm: return SG_PIXELFORMAT_BGRA8;
      case WGPUTextureFormat_RGBA8Unorm: return SG_PIXELFORMAT_RGBA8;
      default: return SG_PIXELFORMAT_NONE;
    }
  }

  static void sokol_renderer_log(
    const char *tag,              // always "sg"
    uint32_t    log_level,        // 0=panic, 1=error, 2=warning, 3=info
    uint32_t    log_item_id,      // SG_LOGITEM_*
    const char *message_or_null,  // a message string, may be nullptr in release mode
    uint32_t    line_nr,          // line number in sokol_gfx.h
    const char *filename_or_null, // source filename, may be nullptr in release mode
    void *      user_data
  ) {
    if (message_or_null == nullptr) {
      return;
    }

    Debug::Logger &logger = Debug::Logger::get_or_create("Sokol", {});

    switch (log_level) {
      case 0:
        logger.log_ftl("%s", message_or_null);
        PANIC();
        break;
      case 1:
        logger.log_err("%s", message_or_null);
        break;
      case 2:
        logger.log_wrn("%s", message_or_null);
        break;
      case 3:
        logger.log_inf("%s", message_or_null);
        break;
      default:
        break;
    }
  }

  Maybe<Sokol::Renderer> Sokol::Renderer::create(Window &window, const RenderConfig &config) {
    if (s_renderer_count_ == 0) {
      // Init globals
      s_ctx_ = WGPU::create_ctx(window).get();

      const sg_desc desc = {
        .logger      = sg_logger{.func = sokol_renderer_log},
        .environment = {
          .defaults = {
            .color_format = wgpu_to_sg_format(s_ctx_.color_format),
            .depth_format = SG_PIXELFORMAT_NONE,
            .sample_count = config.sample_count,
          },
          .wgpu = {.device = s_ctx_.device},
        }
      };

      sg_setup(&desc);
    }

    const WGPU::Surface surface = create_surface(s_ctx_, window);

    Renderer renderer{};
    renderer.config       = config;
    renderer.window_      = &window;
    renderer.surface_     = surface;
    renderer.msaa_target_ = config.sample_count > 1
                              ? WGPU::create_msaa_target(s_ctx_, window.width(), window.height(), config.sample_count)
                              : WGPU::Texture{};
    renderer.surface_width_  = window.width();
    renderer.surface_height_ = window.height();

    s_renderer_count_++;
    return renderer;
  }

  Sokol::Renderer::Renderer(Renderer &&other) noexcept {
    config          = other.config;
    window_         = other.window_;
    surface_        = other.surface_;
    msaa_target_    = other.msaa_target_;
    surface_width_  = other.surface_width_;
    surface_height_ = other.surface_height_;

    other.config          = {};
    other.window_         = nullptr;
    other.surface_        = {};
    other.msaa_target_    = {};
    other.surface_width_  = 0;
    other.surface_height_ = 0;
  }

  Sokol::Renderer &Sokol::Renderer::operator=(Renderer &&other) noexcept {
    if (this == &other) {
      return *this;
    }

    release();

    config          = other.config;
    window_         = other.window_;
    surface_        = other.surface_;
    msaa_target_    = other.msaa_target_;
    surface_width_  = other.surface_width_;
    surface_height_ = other.surface_height_;

    other.config          = {};
    other.window_         = nullptr;
    other.surface_        = {};
    other.msaa_target_    = {};
    other.surface_width_  = 0;
    other.surface_height_ = 0;

    return *this;
  }

  Sokol::Renderer::~Renderer() {
    release();
  }

  void Sokol::Renderer::release() {
    if (!is_valid()) {
      return;
    }

    WGPU::release_texture(msaa_target_);
    WGPU::release_surface(surface_);

    s_renderer_count_--;
    if (s_renderer_count_ == 0) {
      // Release globals
      sg_shutdown();
      WGPU::release_ctx(s_ctx_);
    }
  }

  bool Sokol::Renderer::is_valid() const {
    return window_;
  }

  GPUMesh Sokol::Renderer::upload_mesh(const Mesh &mesh) {
    const sg_buffer_desc vert_desc{
      .data = {.ptr = mesh.vertices.ptr(), .size = mesh.vertices.len() * sizeof(Vector3f)},
    };

    const sg_buffer_desc norm_desc{
      .data = {.ptr = mesh.normals.ptr(), .size = mesh.normals.len() * sizeof(Vector3f)},
    };

    const sg_buffer_desc texcoord_desc{
      .data = {.ptr = mesh.tex_coords.ptr(), .size = mesh.tex_coords.len() * sizeof(Vector2f)},
    };

    const sg_buffer_desc idx_desc{
      .data = {.ptr = mesh.indices.ptr(), .size = mesh.indices.len() * sizeof(u32)},
    };

    sg_buffer vert_buf     = sg_make_buffer(&vert_desc);
    sg_buffer norm_buf     = sg_make_buffer(&norm_desc);
    sg_buffer texcoord_buf = sg_make_buffer(&texcoord_desc);
    sg_buffer idx_buf      = sg_make_buffer(&idx_desc);

    return GPUMesh{
      .vert_buf_id     = vert_buf.id,
      .norm_buf_id     = norm_buf.id,
      .texcoord_buf_id = texcoord_buf.id,
      .idx_buf_id      = idx_buf.id,
    };
  }

  GPUImage2D Sokol::Renderer::upload_texture(const Image &image) {
    const sg_image_desc img_desc{
      .type        = SG_IMAGETYPE_2D,
      .width       = static_cast<i32>(image.width),
      .height      = static_cast<i32>(image.height),
      .num_mipmaps = 1,
      .data        = {sg_range{.ptr = image.pixels.ptr(), .size = image.pixels.len() * sizeof(Color4u8)}},
    };

    sg_image img = sg_make_image(&img_desc);

    return GPUImage2D{
      .id = img.id,
    };
  }

  void Sokol::Renderer::render(const RenderScene &scene) {
    if (window_->width() == 0 || window_->height() == 0) {
      return;
    }

    if (surface_width_ != window_->width() || surface_height_ != window_->height()) {
      WGPU::configure_surface(s_ctx_, surface_, {.width = window_->width(), .height = window_->height()});
      surface_width_ = window_->width(), surface_height_ = window_->height();
    }

    const bool msaa         = config.sample_count > 1;
    auto       [r, g, b, a] = config.clear_color.normalized();

    sg_pass pass{};
    pass.action.colors[0] = {
      .load_action  = SG_LOADACTION_CLEAR,
      .store_action = msaa ? SG_STOREACTION_DONTCARE : SG_STOREACTION_STORE,
      .clear_value  = {.r = r, .g = g, .b = b, .a = a},
    };

    Maybe<WGPU::Texture> surface_tex = WGPU::get_surface_texture(surface_);
    if (!surface_tex || !surface_tex->view) {
      return;
    }

    pass.swapchain.width             = window_->width();
    pass.swapchain.height            = window_->height();
    pass.swapchain.sample_count      = config.sample_count;
    pass.swapchain.color_format      = wgpu_to_sg_format(s_ctx_.color_format);
    pass.swapchain.depth_format      = SG_PIXELFORMAT_NONE;
    pass.swapchain.wgpu.render_view  = msaa ? msaa_target_.view : surface_tex->view;
    pass.swapchain.wgpu.resolve_view = msaa ? surface_tex->view : nullptr;

    sg_begin_pass(&pass);

    // TODO: Render scene

    sg_end_pass();
    sg_commit();

    WGPU::present_surface(surface_);
    WGPU::release_texture(*surface_tex);
  }
}
