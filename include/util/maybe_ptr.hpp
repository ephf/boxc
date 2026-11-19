#pragma once

#include <memory>

namespace util {
    template<typename T>
    struct maybe_ptr {
        maybe_ptr() = default;
        // ReSharper disable once CppNonExplicitConvertingConstructor
        maybe_ptr(T* pointer) : pointer{pointer} {}
        // ReSharper disable once CppNonExplicitConvertingConstructor
        template<typename K> requires std::is_convertible_v<std::unique_ptr<K>, std::unique_ptr<T>>
        maybe_ptr(std::unique_ptr<K>&& owned_pointer)
            : pointer{owned_pointer.get()}, owned_pointer{std::move(owned_pointer)} {}

        maybe_ptr(maybe_ptr&&) = default;
        maybe_ptr& operator=(maybe_ptr&&) = default;

        maybe_ptr(const maybe_ptr& other)
            : maybe_ptr{other.pointer} {}

        maybe_ptr& operator=(const maybe_ptr& other) {
            if(this != &other) *this = other.pointer;
            return *this;
        }

        T& operator*() const noexcept {
            return *pointer;
        }

        T* operator->() const noexcept {
            return pointer;
        }

        std::unique_ptr<T> owned() {
            return std::move(owned_pointer);
        }

        std::unique_ptr<T> owned_or(std::unique_ptr<T>&& fallback) {
            return !fallback || owned_pointer ? std::move(owned_pointer) : std::move(fallback);
        }

        T* get() const {
            return pointer;
        }

    private:
        T* pointer{};
        std::unique_ptr<T> owned_pointer{};
    };
}
