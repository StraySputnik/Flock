#pragma once

#include "common.hpp"
#include "wgpu.hpp"
#include "graphics/renderer.hpp"

namespace Flock::Graphics::Sokol {
  class FLK_API Renderer : public Flock::Renderer {
    inline static WGPU::Context s_ctx_            = {};
    inline static usize         s_renderer_count_ = 0;

    Window *      window_         = nullptr;
    RenderConfig  config_         = {};
    WGPU::Surface surface_        = {};
    WGPU::Texture msaa_target_    = {};
    WGPU::Texture depth_target_   = {};
    u32           surface_width_  = 0;
    u32           surface_height_ = 0;

  public:
    static Maybe<Renderer> create(Window &window, const RenderConfig &config = {});

    Renderer() = default;

    Renderer(const Renderer &other) = delete;
    Renderer(Renderer &&other) noexcept;

    Renderer &operator=(const Renderer &other) = delete;
    Renderer &operator=(Renderer &&other) noexcept;

    ~Renderer() override;

    void release();

    [[nodiscard]] bool is_valid() const;

    void configure(const RenderConfig &config) override;
    void render(const RenderScene &scene) override;
  };
}
