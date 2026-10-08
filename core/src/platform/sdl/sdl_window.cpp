#ifdef FLK_PLATFORM_SDL3

#include "platform/sdl/sdl_window.hpp"

#include <SDL3/SDL.h>

namespace Flock {
    EventStack poll_platform_events() {
        EventStack stack;
        SDL_Event  event;
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
}

namespace Flock::SDL {
    static Vec<SDL_Window *> s_window_ptrs = {};

    bool init() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            return false;
        }

        return true;
    }

    void quit() {
        SDL_Quit();
    }

    Maybe<Window> Window::create(WindowConfig config) {
        if (s_window_count_ == 0) {
            ASSERT(init(), "Failed to initialize SDL");
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

        s_window_ptrs.push(window_ptr);
        return window;
    }

    Window::Window(Window &&other) noexcept {
        window_ptr_         = other.window_ptr_;
        config_             = other.config_;
        should_close_       = other.should_close_;
        event_stack_        = std::move(other.event_stack_);
        other.window_ptr_   = nullptr;
        other.config_       = {};
        other.should_close_ = true;
    }

    Window &Window::operator=(Window &&other) noexcept {
        if (this == &other) {
            return *this;
        }

        free();

        window_ptr_         = other.window_ptr_;
        config_             = other.config_;
        should_close_       = other.should_close_;
        event_stack_        = std::move(other.event_stack_);
        other.window_ptr_   = nullptr;
        other.config_       = {};
        other.should_close_ = true;

        return *this;
    }

    Window::~Window() {
        free();
    }

    void Window::free() {
        if (!window_ptr_) {
            return;
        }

        if (s_window_count_ == 0) {
            quit();
        }

        for (usize i = 0; i < s_window_ptrs.len(); i++) {
            if (s_window_ptrs[i] == window_ptr_) {
                s_window_ptrs.swap_remove(i);
                break;
            }
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

    u16 Window::get_width() const {
        i32 w;
        SDL_GetWindowSize(window_ptr_, &w, nullptr);
        return w;
    }

    u16 Window::get_height() const {
        i32 h;
        SDL_GetWindowSize(window_ptr_, nullptr, &h);
        return h;
    }

    StringSlice Window::get_name() const {
        const char *c_str = SDL_GetWindowTitle(window_ptr_);
        return StringSlice::create(c_str, strlen(c_str));
    }

    WindowID Window::get_id() const {
        return SDL_GetWindowID(window_ptr_);
    }

    bool Window::should_close() const {
        return should_close_;
    }

    EventStack Window::consume_events(EventStack &events) {
        EventStack new_stack = EventStack::create();
        while (!events.is_empty()) {
            OwnedRef<Event> event = events.pop();
            if (static_cast<WindowEvent>(event).get_window_id() == get_id()) {
                event_stack_.push(std::move(event));
                if (event->get_type_id() == type_id<WindowClosedEvent>()) {
                    should_close_ = true;
                }
            } else {
                new_stack.push(std::move(event));
            }
        }

        return new_stack;
    }
}

#endif
