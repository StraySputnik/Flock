#pragma once

#include "common.hpp"
#include "ecs/world.hpp"
#include "platform/window.hpp"
#include "platform/sdl/sdl_window.hpp"

namespace Flock {
    struct AppConfig {
        WindowConfig window_config;
    };

    class FLK_API App {
        AppConfig   config_        = {};
        ECS::World  current_world_ = {};
        SDL::Window window_        = {};

    public:
        static App create(const AppConfig &cfg = {});

        void run();

    private:
        bool is_running() const;

        void start();
        void end();
        void update();
        void render();
    };
}
