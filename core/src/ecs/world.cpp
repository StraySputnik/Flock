#include "ecs/world.hpp"

namespace Flock::ECS {
    World World::create() {
        World world{};
        return world;
    }

    Entity World::spawn() {
        return registry_.spawn();
    }

    void World::despawn(Entity entity) {
        return registry_.despawn(entity);
    }

    bool World::is_alive(Entity entity) const {
        return registry_.is_alive(entity);
    }

    bool World::is_dead(Entity entity) const {
        return registry_.is_dead(entity);
    }

    u8 World::get_version(EntityID entity) const {
        return registry_.get_version(entity);
    }

    World &World::add_system(Stage stage, const System &system) {
        schedule_.add_system(stage, system);
        return *this;
    }

    World &World::add_system(Stage stage, const MutSystem &system) {
        schedule_.add_system(stage, system);
        return *this;
    }
}
