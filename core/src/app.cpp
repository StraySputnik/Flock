#include "app.hpp"

#include "debug/log.hpp"

namespace Flock {
  App App::create(const AppConfig &cfg) {
    if (!Debug::Logger::get("Flock")) {
      Debug::Logger::create("Flock", {});
    }

    App app{};
    app.config_   = cfg;
    app.window_   = SDL::Window::create(cfg.window_config).move();
    app.renderer_ = Sokol::Renderer::create(app.window_).move();
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
    LOG_INF("%s", "Starting Application...");
  }

  void App::end() {
    LOG_INF("%s", "Terminating Application...");
  }

  void App::update() {
    Vec<OwnedRef<Event>> events = SDL::Window::poll_events();
    window_.consume_events(events);
  }

  void App::render() {
    renderer_.render({});
  }
}
