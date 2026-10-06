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

        WindowID get_window_id() const {
            return window_id_;
        }
    };

    class WindowClosedEvent : public WindowEvent {
    public:
        EVENT(WindowClosedEvent)

        explicit WindowClosedEvent(WindowID id) : WindowEvent(id) {
        }
    };

    EventStack poll_platform_events();

    struct FLK_API WindowConfig {
        String name   = String::from("Flock");
        u16    width  = 1080;
        u16    height = 720;
    };

    class FLK_API Window {
    public:
        virtual ~Window() = default;

        virtual void resize(u16 w, u16 h) = 0;
        virtual void rename(StringSlice new_name) = 0;

        virtual u16         get_width() const = 0;
        virtual u16         get_height() const = 0;
        virtual StringSlice get_name() const = 0;
        virtual WindowID    get_id() const = 0;

        virtual bool       should_close() const = 0;
        virtual EventStack consume_events(EventStack &events) = 0;
    };
}
