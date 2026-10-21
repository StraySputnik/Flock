#include "app.hpp"

#include "debug/log.hpp"

namespace Flock {
  App App::create(const AppConfig &cfg) {
    if (!Debug::Logger::get("Flock")) {
      Debug::Logger::create("Flock", {});
    }

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
    Debug::Logger::get("Flock")->log_inf("%s", "Starting Application...");
  }

  void App::end() {
    Debug::Logger::get("Flock")->log_inf("%s", "Terminating Application...");
  }

  void App::update() {
    Vec<OwnedRef<Event>> events = poll_platform_events();
    window_.consume_events(events);
  }

  void App::render() {
  }
}
