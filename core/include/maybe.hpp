#pragma once

#include "common.hpp"

namespace Flock {
    template <typename T>
    struct Maybe {
        T    value     = {};
        bool has_value = false;
    };

    template <typename T>
    Maybe<T> maybe(T value) {
        return {
            .value     = value,
            .has_value = true,
        };
    }

    template <typename T>
    Maybe<T> maybe() {
        return {};
    }
}