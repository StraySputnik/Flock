#include "platform/sdl/sdl_window.hpp"

#include <SDL3/SDL.h>

/**
 * This is an extension of SDL3 for WebGPU, abstracting away the details of
 * OS-specific operations.
 *
 * This file is part of the "Learn WebGPU for C++" book.
 *   https://eliemichel.github.io/LearnWebGPU
 *
 * Most of this code comes from the wgpu-native triangle example:
 *   https://github.com/gfx-rs/wgpu-native/blob/master/examples/triangle/main.c
 *
 * MIT License
 * Copyright (c) 2022-2024 Elie Michel and the wgpu-native authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <webgpu/webgpu.h>

#include "../../../include/graphics/sokol/wgpu.hpp"

#if defined(SDL_PLATFORM_MACOS)
#  include <Cocoa/Cocoa.h>
#  include <Foundation/Foundation.h>
#  include <QuartzCore/CAMetalLayer.h>
#elif defined(SDL_PLATFORM_IOS)
#  include <UIKit/UIKit.h>
#  include <Foundation/Foundation.h>
#  include <QuartzCore/CAMetalLayer.h>
#  include <Metal/Metal.h>
#elif defined(SDL_PLATFORM_WIN32)
#  include <windows.h>
#endif

#include <SDL3/SDL.h>

WGPUSurface SDL_GetWGPUSurface(WGPUInstance instance, SDL_Window *window) {
  SDL_PropertiesID props = SDL_GetWindowProperties(window);

#if defined(SDL_PLATFORM_MACOS)
  {
    id        metal_layer = NULL;
    NSWindow *ns_window   = (__bridge
    NSWindow *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
    if (!ns_window) return NULL;
    [ns_window.contentView setWantsLayer : YES];
    metal_layer = [CAMetalLayer layer];
    [ns_window.contentView setLayer : metal_layer];

    WGPUSurfaceSourceMetalLayer fromMetalLayer;
    fromMetalLayer.chain.sType = WGPUSType_SurfaceSourceMetalLayer;
    fromMetalLayer.chain.next  = NULL;
    fromMetalLayer.layer       = metal_layer;

    WGPUSurfaceDescriptor surfaceDescriptor;
    surfaceDescriptor.nextInChain = &fromMetalLayer.chain;
    surfaceDescriptor.label       = (WGPUStringView){NULL, WGPU_STRLEN};

    return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
  }
#elif defined(SDL_PLATFORM_IOS)
  {
    UIWindow *ui_window = (__bridge
    UIWindow *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER, NULL);
    if (!uiwindow) return NULL;

    UIView *      ui_view     = ui_window.rootViewController.view;
    CAMetalLayer *metal_layer = [CAMetalLayer new];
    metal_layer.opaque        = true;
    metal_layer.frame         = ui_view.frame;
    metal_layer.drawableSize  = ui_view.frame.size;

    [ui_view.layer addSublayer: metal_layer];

    WGPUSurfaceSourceMetalLayer fromMetalLayer;
    fromMetalLayer.chain.sType = WGPUSType_SurfaceSourceMetalLayer;
    fromMetalLayer.chain.next  = NULL;
    fromMetalLayer.layer       = metal_layer;

    WGPUSurfaceDescriptor surfaceDescriptor;
    surfaceDescriptor.nextInChain = &fromMetalLayer.chain;
    surfaceDescriptor.label       = (WGPUStringView){NULL, WGPU_STRLEN};

    return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
  }
#elif defined(SDL_PLATFORM_LINUX)
  if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
    void *   x11_display = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
    uint64_t x11_window  = SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    if (!x11_display || !x11_window) return NULL;

    WGPUSurfaceSourceXlibWindow fromXlibWindow;
    fromXlibWindow.chain.sType = WGPUSType_SurfaceSourceXlibWindow;
    fromXlibWindow.chain.next  = NULL;
    fromXlibWindow.display     = x11_display;
    fromXlibWindow.window      = x11_window;

    WGPUSurfaceDescriptor surfaceDescriptor;
    surfaceDescriptor.nextInChain = &fromXlibWindow.chain;
    surfaceDescriptor.label       = (WGPUStringView){NULL, WGPU_STRLEN};

    return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
  } else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
    void *wayland_display = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
    void *wayland_surface = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
    if (!wayland_display || !wayland_surface) return NULL;

    WGPUSurfaceSourceWaylandSurface fromWaylandSurface;
    fromWaylandSurface.chain.sType = WGPUSType_SurfaceSourceWaylandSurface;
    fromWaylandSurface.chain.next  = NULL;
    fromWaylandSurface.display     = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
    fromWaylandSurface.surface     = wayland_surface;

    WGPUSurfaceDescriptor surfaceDescriptor;
    surfaceDescriptor.nextInChain = &fromWaylandSurface.chain;
    surfaceDescriptor.label       = (WGPUStringView){NULL, WGPU_STRLEN};

    return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
  }
#elif defined(SDL_PLATFORM_WIN32)
  {
    HWND hwnd = (HWND) SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (!hwnd) return NULL;
    HINSTANCE hinstance = GetModuleHandle(NULL);

    WGPUSurfaceSourceWindowsHWND fromWindowsHWND;
    fromWindowsHWND.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
    fromWindowsHWND.chain.next  = NULL;
    fromWindowsHWND.hinstance   = hinstance;
    fromWindowsHWND.hwnd        = hwnd;

    WGPUSurfaceDescriptor surfaceDescriptor;
    surfaceDescriptor.nextInChain = &fromWindowsHWND.chain;
    surfaceDescriptor.label       = (WGPUStringView){NULL, WGPU_STRLEN};

    return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
  }
#elif defined(__EMSCRIPTEN__)
  {
#  ifdef WEBGPU_BACKEND_EMDAWNWEBGPU
  WGPUEmscriptenSurfaceSourceCanvasHTMLSelector fromCanvasHTMLSelector;
  fromCanvasHTMLSelector.chain.sType = WGPUSType_EmscriptenSurfaceSourceCanvasHTMLSelector;
#  else
  WGPUSurfaceDescriptorFromCanvasHTMLSelector fromCanvasHTMLSelector;
  fromCanvasHTMLSelector.chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector;
#  endif
  fromCanvasHTMLSelector.chain.next = NULL;
  fromCanvasHTMLSelector.selector   = "canvas";

  WGPUSurfaceDescriptor surfaceDescriptor;
  surfaceDescriptor.nextInChain = &fromCanvasHTMLSelector.chain;
  surfaceDescriptor.label       = NULL;

  return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
    }
#else
  // TODO: See SDL_syswm.h for other possible enum values!
#error "Unsupported WGPU_TARGET"
#endif

  UNREACHABLE();
}

namespace Flock::SDL {
  static bool start_sdl() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
      return false;
    }

    return true;
  }

  static void stop_sdl() {
    SDL_Quit();
  }

  Vec<OwnedRef<Event>> Window::poll_events() {
    Vec<OwnedRef<Event>> stack;
    SDL_Event            event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
          stack.push(OwnedRef<Event>::from_value(WindowClosedEvent{event.window.windowID}));
          break;
        default:
          break;
      }
    }

    return stack;
  }

  Maybe<Window> Window::create(WindowConfig config) {
    if (s_window_count_ == 0) {
      ASSERT(start_sdl(), "Failed to initialize SDL");
    }

    SDL_Window *window_ptr = SDL_CreateWindow(
      config.name.first(),
      config.width,
      config.height,
      SDL_WINDOW_RESIZABLE
    );

    if (!window_ptr) {
      return {};
    }

    Window window{};
    window.window_ptr_   = window_ptr;
    window.config_       = config;
    window.should_close_ = false;

    s_window_count_++;
    return window;
  }

  Window::Window(Window &&other) noexcept {
    window_ptr_         = other.window_ptr_;
    config_             = other.config_;
    should_close_       = other.should_close_;
    events_             = std::move(other.events_);
    other.window_ptr_   = nullptr;
    other.config_       = {};
    other.should_close_ = true;
  }

  Window &Window::operator=(Window &&other) noexcept {
    if (this == &other) {
      return *this;
    }

    release();

    window_ptr_         = other.window_ptr_;
    config_             = other.config_;
    should_close_       = other.should_close_;
    events_             = std::move(other.events_);
    other.window_ptr_   = nullptr;
    other.config_       = {};
    other.should_close_ = true;

    return *this;
  }

  Window::~Window() {
    release();
  }

  void Window::release() {
    if (!window_ptr_) {
      return;
    }

    if (s_window_count_ == 0) {
      stop_sdl();
    }

    SDL_DestroyWindow(window_ptr_);
    window_ptr_   = nullptr;
    should_close_ = false;
    s_window_count_--;
  }

  void Window::resize(u16 w, u16 h) {
    SDL_SetWindowSize(window_ptr_, w, h);
  }

  void Window::rename(StringSlice new_name) {
    SDL_SetWindowTitle(window_ptr_, new_name.first());
  }

  u16 Window::width() const {
    i32 w;
    SDL_GetWindowSize(window_ptr_, &w, nullptr);
    return w;
  }

  u16 Window::height() const {
    i32 h;
    SDL_GetWindowSize(window_ptr_, nullptr, &h);
    return h;
  }

  StringSlice Window::title() const {
    const char *c_str = SDL_GetWindowTitle(window_ptr_);
    return StringSlice::create(c_str, strlen(c_str));
  }

  WindowID Window::id() const {
    return SDL_GetWindowID(window_ptr_);
  }

  bool Window::should_close() const {
    return should_close_;
  }

  void *Window::create_wgpu_surface(void *instance) const {
    return SDL_GetWGPUSurface(static_cast<WGPUInstance>(instance), window_ptr_);
  }

  Vec<OwnedRef<Event>> Window::consume_events(Vec<OwnedRef<Event>> &events) {
    Vec<OwnedRef<Event>> new_stack = {};
    while (!events.is_empty()) {
      OwnedRef<Event> event        = events.pop().move();
      const auto *    window_event = static_cast<WindowEvent *>(event.get());
      if (window_event->get_window_id() == id()) {
        if (event->get_type_id() == type_id<WindowClosedEvent>()) {
          should_close_ = true;
        }

        events_.push(std::move(event));
      } else {
        new_stack.push(std::move(event));
      }
    }

    return new_stack;
  }
}
