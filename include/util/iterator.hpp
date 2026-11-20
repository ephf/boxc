#pragma once

#include <span>

namespace util {
    template<typename...>
    struct zip_iterator;

    template<typename...>
    struct flat_iterator;

    template<typename>
    struct iterable;

    template<typename T, typename...>
    struct iterator {
        using Type = T;

        virtual ~iterator() = default;

        virtual std::optional<T> next() = 0;
    };

    template<typename T, typename I>
    struct iterator<T, I> : iterator<T> {
        std::optional<T> next() override = 0;

        template<typename K, typename J> requires std::is_base_of_v<iterator<K, J>, J>
        zip_iterator<T, K, I, J> zip(J&& iterator) {
            return zip_iterator<T, K, I, J>{std::move(*static_cast<I*>(this)), std::forward<J>(iterator)};
        }

        template<typename K> requires std::is_convertible_v<T, K*>
        flat_iterator<T, K, I> flatten() {
            return flat_iterator<T, K, I>{std::move(*static_cast<I*>(this))};
        }

        template<typename F>
        void for_each(F&& function) {
            while(auto value = next()) function(std::forward<T>(*value));
        }

        template<typename F>
        void for_each_t(F&& function) {
            while(auto value = next()) std::apply(function, *value);
        }
    };

    template<typename I, typename S, typename... Args>
    struct iterator_self : I, iterator<typename I::Type, S> {
        explicit iterator_self(Args&&... args) : I{std::forward<Args>(args)...} {}
        explicit iterator_self(I&& i) : I{std::forward<I>(i)} {}

        std::optional<typename I::Type> next() override {
            return I::next();
        }
    };

    template<typename T>
    struct dyn_iterable {
        virtual ~dyn_iterable() = default;

        virtual std::unique_ptr<iterator<T>> into_dyn_iterator() = 0;
    };

    template<typename I>
    struct iterable : dyn_iterable<typename I::Type> {
        virtual I into_iterator() = 0;

        std::unique_ptr<iterator<typename I::Type>> into_dyn_iterator() override {
            return std::make_unique<I>(into_iterator());
        }
    };

    template<typename T, typename K, typename I, typename J>
        requires std::is_base_of_v<iterator<T, I>, I> && std::is_base_of_v<iterator<K, J>, J>
    struct zip_iterator<T, K, I, J> : iterator<std::tuple<T, K>, zip_iterator<T, K, I, J>> {
        zip_iterator(I&& i, J&& j) : i{std::move(i)}, j{std::forward<J>(j)} {}

        std::optional<std::tuple<T, K>> next() override {
            auto t = i.next();
            auto k = j.next();
            return t && k ? std::optional<std::tuple<T, K>>{{*t, *k}} : std::nullopt;
        }

        I i;
        J j;
    };

    template<typename T, typename K, typename I>
        requires std::is_base_of_v<iterator<T, I>, I> && std::is_convertible_v<T, K*>
    struct flat_iterator<T, K, I> : iterator<T, flat_iterator<T, K, I>> {
        explicit flat_iterator(I&& i) : i{std::move(i)} {}

        std::optional<T> next() override {
            return j
                   ? j->next().or_else([&] {
                       j = nullptr;
                       return next();
                   })
                   : i.next().and_then([&](const T& value) {
                       if(auto* dyn_iter = dynamic_cast<dyn_iterable<T>*>(static_cast<K*>(value))) {
                           j = dyn_iter->into_dyn_iterator();
                           return next();
                       }
                       return std::optional{value};
                   });
        }

        I i;
        std::unique_ptr<iterator<T>> j{};
    };

    template<typename T>
    struct iterate : iterator<T, iterate<T>> {
        explicit iterate(const std::span<T> span) : begin{span.begin()}, end{span.end()} {}

        std::optional<T> next() override {
            return begin == end ? std::nullopt : std::optional{*begin++};
        }

        std::span<T>::iterator begin, end;
    };
}
