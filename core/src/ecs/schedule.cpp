#include "ecs/schedule.hpp"

namespace Flock::ECS {
    Schedule Schedule::create() {
        return Schedule{};
    }

    Schedule &Schedule::add_system(Stage stage, const System &system) {
        systems_.get(stage)->push(system);
        return *this;
    }

    Schedule &Schedule::add_system(Stage stage, const MutSystem &system) {
        mut_systems_.get(stage)->push(system);
        return *this;
    }

    void Schedule::execute(Stage stage, World &world) {
        for (const auto &sys : *systems_.get(stage)) {
            sys(world);
        }

        for (const auto &sys : *mut_systems_.get(stage)) {
            sys(world);
        }
    }
}
