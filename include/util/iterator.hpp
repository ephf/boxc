#pragma once

namespace util {
    template<typename T>
    struct iterator {
        virtual ~iterator() = default;

        virtual std::optional<T> next() = 0;
    };
}
