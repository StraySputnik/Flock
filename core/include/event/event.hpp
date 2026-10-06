#pragma once

#include "common.hpp"
#include "collect/vector.hpp"
#include "memory/ref.hpp"

namespace Flock {
    class FLK_API Event {
    public:
        virtual        ~Event() = default;
        virtual TypeID get_type_id() = 0;
    };

#define EVENT(class) \
    Flock::TypeID get_type_id() override { return Flock::type_id<class>(); }

    class FLK_API EventStack {
        Vector<OwnedRef<Event>> events_ = {};

    public:
        static EventStack create();

        void            push(OwnedRef<Event> &&event);
        OwnedRef<Event> pop();

        bool is_empty() const;
    };

    using EventCallback = void(*)(Event *event);
    using CallbackID    = usize;

    class FLK_API EventHandler {
        EventStack            event_stack_    = {};
        Vector<EventCallback> callbacks_      = {};
        Vector<CallbackID>    dead_callbacks_ = {};

    public:
        void push(OwnedRef<Event> &&event);
        void dispatch(OwnedRef<Event> &&event);

        CallbackID subscribe(EventCallback callback);
        void       unsubscribe(CallbackID callback_id);

        void handle_events();
    };
}
