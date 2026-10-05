#include "ecs/registry.hpp"

namespace Flock::ECS {
    Entity Registry::spawn() {
        if (!dead_entities_.is_empty()) {
            const EntityID      id      = *dead_entities_.last();
            const EntityVersion version = entity_versions_[id];
            dead_entities_.pop();

            return {.id = id, .version = version};
        }

        entity_versions_.push(0);
        return {
            .id      = static_cast<EntityID>(entity_versions_.len()) - 1,
            .version = 0,
        };
    }

    void Registry::despawn(Entity entity) {
        entity_versions_[entity.id]++;
        dead_entities_.push(entity.id);
    }

    bool Registry::is_alive(Entity entity) const {
        if (entity.id >= entity_versions_.len()) {
            return false;
        }

        return entity.version == entity_versions_[entity.id];
    }

    bool Registry::is_dead(Entity entity) const {
        return !is_alive(entity);
    }

    EntityVersion Registry::get_version(EntityID entity) const {
        return entity_versions_[entity];
    }
}
