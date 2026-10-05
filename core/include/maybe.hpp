#pragma once

#include "common.hpp"

namespace Flock {
    template <typename T>
    class Maybe {
        T    value_     = {};
        bool has_value_ = false;

    public:
        Maybe() = default;

        Maybe(const T &value) : value_(value), has_value_(true) {
        }

        Maybe(T &&value) : value_(std::move(value)), has_value_(true) {
        }

        bool has_value() const {
            return has_value_;
        }

        bool is_empty() const {
            return !has_value_;
        }

        const T &get_value() const {
            ASSERT(has_value_, "Get on empty Maybe");
            return value_;
        }

        T &get_value() {
            ASSERT(has_value_, "Get on empty Maybe");
            return value_;
        }

        T &&move() {
            ASSERT(has_value_, "Get on empty Maybe");
            return std::move(value_);
        }

        operator bool() const {
            return has_value();
        }

        T *operator ->() {
            return &value_;
        }

        const T *operator ->() const {
            return &value_;
        }

        T &operator*() {
            return value_;
        }

        const T &operator*() const {
            return value_;
        }
    };
}
