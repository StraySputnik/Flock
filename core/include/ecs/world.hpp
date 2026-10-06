#pragma once

#include "common.hpp"
#include "registry.hpp"
#include "schedule.hpp"

namespace Flock::ECS {
    class FLK_API World {
        Registry registry_ = {};
        Schedule schedule_ = {};

    public:
        static World create();

        template <typename T>
        void register_comp() {
            registry_.register_comp<T>();
        }

        Entity spawn();
        void   despawn(Entity entity);

        bool is_alive(Entity entity) const;
        bool is_dead(Entity entity) const;
        u8   get_version(EntityID entity) const;

        template <typename T>
        bool add(Entity entity, const T &component) {
            return registry_.add(entity, component);
        }

        template <typename T>
        T *get(Entity entity) {
            return registry_.get<T>(entity);
        }

        template <typename T>
        const T *get(Entity entity) const {
            return registry_.get<T>(entity);
        }

        template <typename T>
        bool has(Entity entity) const {
            return registry_.has<T>(entity);
        }

        template <typename T>
        void remove(Entity entity) {
            return registry_.remove<T>(entity);
        }

        template <typename... Args>
        void add_systems(Args... systems) {
            (add_system(systems), ...);
        }

        World &add_system(Stage stage, const System &system);
        World &add_system(Stage stage, const MutSystem &system);
    };
}
