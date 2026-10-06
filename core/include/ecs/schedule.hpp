#pragma once

#include "common.hpp"
#include "collect/vector.hpp"
#include "collect/map.hpp"

namespace Flock::ECS {
    class World;

    using System    = void (*)(const World &world);
    using MutSystem = void (*)(World &world);

    enum class Stage : u8 {
        Start,
        Update,
    };

    class FLK_API Schedule {
        Map<Stage, Vector<System>>    systems_     = {};
        Map<Stage, Vector<MutSystem>> mut_systems_ = {};

    public:
        static Schedule create();

        template <typename... Args>
        void add_systems(Args... systems) {
            (add_system(systems), ...);
        }

        Schedule &add_system(Stage stage, const System &system);
        Schedule &add_system(Stage stage, const MutSystem &system);

        void execute(Stage stage, World &world);
    };
}
