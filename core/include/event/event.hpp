#pragma once

#include "common.hpp"
#include "collect/vec.hpp"
#include "memory/ref.hpp"

namespace Flock {
  class FLK_API Event {
  public:
    virtual        ~Event() = default;
    virtual TypeID get_type_id() = 0;
  };

#define EVENT(class) \
    Flock::TypeID get_type_id() override { return Flock::type_id<class>(); }

  using EventCallback = void(*)(Event *event);
  using CallbackID    = usize;

  class FLK_API EventHandler {
    Vec<OwnedRef<Event>> events_         = {};
    Vec<EventCallback>   callbacks_      = {};
    Vec<CallbackID>      dead_callbacks_ = {};

  public:
    void push(OwnedRef<Event> &&event);
    void dispatch(OwnedRef<Event> &&event);

    CallbackID subscribe(EventCallback callback);
    void       unsubscribe(CallbackID callback_id);

    void handle_events();
  };
}
