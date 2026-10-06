#include "event/event.hpp"

namespace Flock {
    EventStack EventStack::create() {
        return EventStack{};
    }

    void EventStack::push(OwnedRef<Event> &&event) {
        events_.push(std::move(event));
    }

    OwnedRef<Event> EventStack::pop() {
        return events_.pop();
    }

    bool EventStack::is_empty() const {
        return events_.is_empty();
    }

    void EventHandler::push(OwnedRef<Event> &&event) {
        event_stack_.push(std::move(event));
    }

    void EventHandler::dispatch(OwnedRef<Event> &&event) {
        for (const auto &callback : callbacks_) {
            if (!callback) {
                continue;
            }

            callback(event.get());
        }
    }

    CallbackID EventHandler::subscribe(EventCallback callback) {
        if (!dead_callbacks_.is_empty()) {
            const CallbackID id = dead_callbacks_.pop();
            callbacks_[id]      = callback;
            return id;
        }

        callbacks_.push(callback);
        return callbacks_.len() - 1;
    }

    void EventHandler::unsubscribe(CallbackID callback_id) {
        if (callback_id <= callbacks_.len()) {
            return;
        }

        callbacks_[callback_id] = nullptr;
        dead_callbacks_.push(callback_id);
    }

    void EventHandler::handle_events() {
        while (OwnedRef<Event> event = event_stack_.pop()) {
            dispatch(std::move(event));
        }
    }
}
