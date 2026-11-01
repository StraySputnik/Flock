#include "graphics/sokol/wgpu.hpp"

#include <webgpu/webgpu.h>

#include "debug/log.hpp"

namespace {
  void on_adapter(
    const WGPURequestAdapterStatus status,
    const WGPUAdapter              adapter,
    const WGPUStringView           message,
    void *                         userdata1,
    void * /*userdata2*/
  ) {
    auto *res = static_cast<Flock::Maybe<WGPUAdapter> *>(userdata1);
    if (status == WGPURequestAdapterStatus_Success) {
      *res = adapter;
    } else {
      LOG_FTL("Adapter request failed: %.*s\n", message.length, message.data);
      PANIC();
    }
  }

  void on_device(
    const WGPURequestDeviceStatus status,
    const WGPUDevice              device,
    const WGPUStringView          message,
    void *                        userdata1,
    void * /*userdata2*/
  ) {
    auto *res = static_cast<Flock::Maybe<WGPUDevice> *>(userdata1);
    if (status == WGPURequestDeviceStatus_Success) {
      *res = device;
    } else {
      LOG_FTL("Device request failed: %.*s\n", message.length, message.data);
      PANIC();
    }
  }

  void on_error(
    WGPUDevice const * /*device*/,
    const WGPUErrorType  type,
    const WGPUStringView message,
    void *,
    void *
  ) {
    LOG_FTL("WebGPU error (%d): %.*s\n", type, message.length, message.data);
    PANIC();
  }
}

namespace Flock::Graphics::Sokol::WGPU {
  static i32 s_allowed_formats[2] = {
    WGPUTextureFormat_BGRA8Unorm,
    WGPUTextureFormat_RGBA8Unorm
  };

  Maybe<Context> create_ctx(const Window &window) {
    Context    ctx{};
    void *     instance = wgpuCreateInstance(nullptr);
    const auto surface  = static_cast<WGPUSurface>(window.create_wgpu_surface(instance));

    Maybe<WGPUAdapter>        ma;
    WGPURequestAdapterOptions adapter_opts = {};
    adapter_opts.compatibleSurface         = surface;

    WGPURequestAdapterCallbackInfo adapter_cb = {};
    adapter_cb.mode                           = WGPUCallbackMode_AllowProcessEvents;
    adapter_cb.callback                       = on_adapter;
    adapter_cb.userdata1                      = &ma;
    wgpuInstanceRequestAdapter(static_cast<WGPUInstance>(instance), &adapter_opts, adapter_cb);

    while (!ma) {
      wgpuInstanceProcessEvents(static_cast<WGPUInstance>(instance));
    }

    if (!ma.get()) {
      return {};
    }

    WGPUAdapter adapter = ma.get();

    Maybe<WGPUDevice>    md;
    WGPUDeviceDescriptor dev_desc                 = {};
    dev_desc.uncapturedErrorCallbackInfo.callback = on_error;
    WGPURequestDeviceCallbackInfo device_cb       = {};
    device_cb.mode                                = WGPUCallbackMode_AllowProcessEvents;
    device_cb.callback                            = on_device;
    device_cb.userdata1                           = &md;
    wgpuAdapterRequestDevice(adapter, &dev_desc, device_cb);

    while (!md) {
      wgpuInstanceProcessEvents(static_cast<WGPUInstance>(instance));
    }

    if (!md.get()) {
      return {};
    }

    WGPUDevice device = md.get();

    ctx.instance = instance;
    ctx.adapter  = adapter;
    ctx.device   = device;

    const auto surface_format = get_surface_format(ctx, Surface{.ptr = surface});
    if (!surface_format) {
      wgpuSurfaceRelease(surface);
      release_ctx(ctx);
      return {};
    }

    ctx.color_format = surface_format.get();

    wgpuSurfaceRelease(surface);
    return ctx;
  }

  void release_ctx(Context &ctx) {
    if (ctx.device) {
      wgpuDeviceRelease(static_cast<WGPUDevice>(ctx.device));
    }

    if (ctx.adapter) {
      wgpuAdapterRelease(static_cast<WGPUAdapter>(ctx.adapter));
    }

    if (ctx.instance) {
      wgpuInstanceRelease(static_cast<WGPUInstance>(ctx.instance));
    }

    ctx.device   = nullptr;
    ctx.adapter  = nullptr;
    ctx.instance = nullptr;
  }

  void release_texture(Texture &texture) {
    if (texture.view) {
      wgpuTextureViewRelease(static_cast<WGPUTextureView>(texture.view));
    }

    if (texture.texture) {
      wgpuTextureRelease(static_cast<WGPUTexture>(texture.texture));
    }

    texture.view    = nullptr;
    texture.texture = nullptr;
  }

  Surface create_surface(const Context &ctx, const Window &window) {
    const Surface surface{.ptr = window.create_wgpu_surface(ctx.instance)};
    configure_surface(ctx, surface, {.width = window.width(), .height = window.height()});
    return surface;
  }

  void configure_surface(const Context &ctx, const Surface &surface, const SurfaceConfig config) {
    if (config.width == 0 || config.height == 0) {
      return;
    }

    const WGPUSurfaceConfiguration cfg = {
      .device      = static_cast<WGPUDevice>(ctx.device),
      .format      = static_cast<WGPUTextureFormat>(ctx.color_format),
      .usage       = WGPUTextureUsage_RenderAttachment,
      .width       = config.width,
      .height      = config.height,
      .alphaMode   = WGPUCompositeAlphaMode_Auto,
      .presentMode = static_cast<WGPUPresentMode>(config.present_mode),
    };

    wgpuSurfaceConfigure(static_cast<WGPUSurface>(surface.ptr), &cfg);
  }

  void present_surface(const Surface &surface) {
    wgpuSurfacePresent(static_cast<WGPUSurface>(surface.ptr));
  }

  void release_surface(Surface &surface) {
    if (!surface.ptr) {
      return;
    }

    wgpuSurfaceUnconfigure(static_cast<WGPUSurface>(surface.ptr));
    wgpuSurfaceRelease(static_cast<WGPUSurface>(surface.ptr));

    surface.ptr = nullptr;
  }

  Maybe<i32> get_surface_format(const Context &ctx, const Surface &surface) {
    WGPUSurfaceCapabilities caps{};
    wgpuSurfaceGetCapabilities(static_cast<WGPUSurface>(surface.ptr), static_cast<WGPUAdapter>(ctx.adapter), &caps);

    Maybe<i32> result = {};
    for (usize i = 0; i < caps.formatCount; i++) {
      const auto fmt = caps.formats[i];
      for (const i32 allowed_fmt: s_allowed_formats) {
        if (fmt != allowed_fmt) {
          continue;
        }

        result = fmt;
        break;
      }

      if (result) {
        break;
      }
    }

    wgpuSurfaceCapabilitiesFreeMembers(caps);
    return result;
  }

  Maybe<Texture> get_surface_texture(const Surface &surface) {
    const auto surf = static_cast<WGPUSurface>(surface.ptr);

    WGPUSurfaceTexture st = {};
    wgpuSurfaceGetCurrentTexture(surf, &st);
    if (st.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
        st.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal) {
      if (st.texture) {
        wgpuTextureRelease(st.texture);
      }

      return {};
    }

    WGPUTextureView surface_view = wgpuTextureCreateView(st.texture, nullptr);
    return Texture{.texture = st.texture, .view = surface_view};
  }

  Texture create_msaa_target(const Context &ctx, const u32 width, const u32 height, const usize sample_count) {
    ASSERT(sample_count > 1, "Invalid sample count");

    WGPUTextureView       msaa_view = nullptr;
    WGPUTexture           msaa_tex  = nullptr;
    WGPUTextureDescriptor td        = {};

    td.usage         = WGPUTextureUsage_RenderAttachment;
    td.dimension     = WGPUTextureDimension_2D;
    td.size          = {.width = width, .height = height, .depthOrArrayLayers = 1};
    td.format        = static_cast<WGPUTextureFormat>(ctx.color_format);
    td.mipLevelCount = 1;
    td.sampleCount   = sample_count;
    msaa_tex         = wgpuDeviceCreateTexture(static_cast<WGPUDevice>(ctx.device), &td);
    msaa_view        = wgpuTextureCreateView(msaa_tex, nullptr);

    return Texture{.texture = msaa_tex, .view = msaa_view};
  }
}
