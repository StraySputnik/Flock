#include "app.hpp"

namespace Flock {
    App App::create(const AppConfig &cfg) {
        App app{};
        app.config_ = cfg;
        app.window_ = SDL::Window::create(cfg.window_config).move();
        return app;
    }

    void App::run() {
        start();
        while (is_running()) {
            update();
            render();
        }

        end();
    }

    bool App::is_running() const {
        return !window_.should_close();
    }

    void App::start() {
    }

    void App::end() {
    }

    void App::update() {
        EventStack events = poll_platform_events();
        window_.consume_events(events);
    }

    void App::render() {
    }
}
