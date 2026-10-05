#pragma once

#include "common.hpp"
#include "group.hpp"
#include "collect/vector.hpp"

namespace Flock::ECS {
    using EntityVersion = u8;

    struct Entity {
        EntityID id      : 24 = 0;
        EntityID version : 8  = 0;
    };

    class FLK_API Registry {
        struct GroupInterface {
            void (*remove)(Registry &, EntityID);
            bool (*has)(Registry &, EntityID id);
        };

        Vector<usize>          sparse_          = {};
        Vector<void *>         groups_          = {};
        Vector<GroupInterface> interfaces_      = {};
        Vector<EntityVersion>  entity_versions_ = {};
        Vector<EntityID>       dead_entities_   = {};

    public:
        static Registry create() {
            Registry registry{};
            return registry;
        }

        template <typename... Args>
        static Registry create() {
            Registry registry{};
            (registry.register_comp<Args>(), ...);
            return registry;
        }

        template <typename T>
        void register_comp() {
            const TypeID id = type_id<T>();
            if (id >= sparse_.len()) {
                sparse_.resize(id + 1, INVALID);
            }

            sparse_[id]                   = groups_.len();
            void *ptr                     = new Group<T>{};
            *static_cast<Group<T> *>(ptr) = Group<T>::create();

            groups_.push(ptr);
            interfaces_.push({
                .remove = [](Registry &reg, const EntityID entity_id) {
                    static_cast<Group<T> *>(*reg.groups_.last())->remove(entity_id);
                },
                .has = [](Registry &reg, const EntityID entity_id) -> bool {
                    return static_cast<Group<T> *>(*reg.groups_.last())->get(entity_id) != nullptr;
                },
            });
        }

        Entity spawn();
        void   despawn(Entity entity);

        bool is_alive(Entity entity) const;
        bool is_dead(Entity entity) const;
        u8   get_version(EntityID entity) const;

        template <typename T>
        Group<T> &get_group() {
            const TypeID id = type_id<T>();
            ASSERT(id < groups_.len(), "Component is not registered");
            return *static_cast<Group<T> *>(groups_[type_id<T>()]);
        }

        template <typename T>
        bool add(Entity entity, const T &component) {
            if (is_dead(entity)) {
                return false;
            }

            get_group<T>().insert(entity.id, component);
            return true;
        }

        template <typename T>
        T *get(Entity entity) {
            if (is_dead(entity)) {
                return nullptr;
            }

            return get_group<T>().get(entity.id);
        }

        template <typename T>
        const T *get(Entity entity) const {
            if (is_dead(entity)) {
                return nullptr;
            }

            return get_group<T>().get(entity.id);
        }

        template <typename T>
        bool has(Entity entity) const {
            return get<T>(entity) != nullptr;
        }

        template <typename T>
        void remove(Entity entity) {
            if (is_dead(entity)) {
                return;
            }

            get_group<T>().remove(entity.id);
        }
    };
}
