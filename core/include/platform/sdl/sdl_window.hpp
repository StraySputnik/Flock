#pragma once

#include "common.hpp"
#include "event/event.hpp"
#include "platform/window.hpp"

struct SDL_Window;

namespace Flock::SDL {
  class FLK_API Window : public Flock::Window {
    inline static usize s_window_count_ = 0;

    SDL_Window *         window_ptr_   = nullptr;
    WindowConfig         config_       = {};
    bool                 should_close_ = false;
    Vec<OwnedRef<Event>> events_       = {};

  public:
    static Vec<OwnedRef<Event>> poll_events();

    static Maybe<Window> create(WindowConfig config);

    Window() = default;

    Window(const Window &) = delete;
    Window(Window &&other) noexcept;

    Window &operator=(const Window &) = delete;
    Window &operator=(Window &&other) noexcept;

    ~Window() override;

    void release();

    void resize(u16 w, u16 h) override;
    void rename(StringSlice new_name) override;

    [[nodiscard]] u16         width() const override;
    [[nodiscard]] u16         height() const override;
    [[nodiscard]] StringSlice title() const override;
    [[nodiscard]] WindowID    id() const override;
    [[nodiscard]] bool        should_close() const override;
    [[nodiscard]] void *      create_wgpu_surface(void *instance) const override;

    Vec<OwnedRef<Event>> consume_events(Vec<OwnedRef<Event>> &events) override;
  };
}
