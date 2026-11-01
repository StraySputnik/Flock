#pragma once

#include "common.hpp"
#include "collect/string.hpp"
#include "event/event.hpp"

namespace Flock {
  using WindowID = usize;

  class WindowEvent : public Event {
    WindowID window_id_ = 0;

  public:
    EVENT(WindowEvent)

    explicit WindowEvent(WindowID id) : window_id_(id) {
    }

    [[nodiscard]] WindowID get_window_id() const {
      return window_id_;
    }
  };

  class WindowClosedEvent : public WindowEvent {
  public:
    EVENT(WindowClosedEvent)

    explicit WindowClosedEvent(WindowID id) : WindowEvent(id) {
    }
  };

  struct FLK_API WindowConfig {
    String name   = String::from_c_str("Flock");
    u16    width  = 1080;
    u16    height = 720;
  };

  class FLK_API Window {
  public:
    virtual ~Window() = default;

    virtual void resize(u16 w, u16 h) = 0;
    virtual void rename(StringSlice new_name) = 0;

    [[nodiscard]] virtual u16         width() const = 0;
    [[nodiscard]] virtual u16         height() const = 0;
    [[nodiscard]] virtual StringSlice title() const = 0;
    [[nodiscard]] virtual WindowID    id() const = 0;
    [[nodiscard]] virtual bool        should_close() const = 0;
    [[nodiscard]] virtual void *      create_wgpu_surface(void *ctx) const = 0;

    virtual Vec<OwnedRef<Event>> consume_events(Vec<OwnedRef<Event>> &) = 0;
  };
}
