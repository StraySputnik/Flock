#pragma once

#ifdef FLK_PLATFORM_SDL3

#include "common.hpp"
#include "event/event.hpp"
#include "platform/window.hpp"

struct SDL_Window;

namespace Flock::SDL {
    bool init();
    void quit();

    class FLK_API Window : public Flock::Window {
        inline static usize s_window_count_ = 0;

        SDL_Window * window_ptr_   = nullptr;
        WindowConfig config_       = {};
        bool         should_close_ = false;
        EventStack   event_stack_  = {};

    public:
        static Maybe<Window> create(WindowConfig config);

        Window() = default;

        Window(const Window &) = delete;
        Window(Window &&other) noexcept;

        Window &operator=(const Window &) = delete;
        Window &operator=(Window &&other) noexcept;

        ~Window() override;

        void free();

        void resize(u16 w, u16 h) override;
        void rename(StringSlice new_name) override;

        u16         get_width() const override;
        u16         get_height() const override;
        StringSlice get_name() const override;
        WindowID    get_id() const override;

        bool       should_close() const override;
        EventStack consume_events(EventStack &events) override;
    };
}

#endif
