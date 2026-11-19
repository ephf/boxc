#pragma once

#include "type.hpp"

#include <span>

#include "declaration/declaration.hpp"
#include "util/iterator.hpp"

namespace type::generics {
    struct type_argument_reference : virtual_type {
        template<typename D> requires std::is_convertible_v<D, std::shared_ptr<declaration::virtual_declaration>>
        type_argument_reference(D&& declaration, const std::size_t index)
            : declaration{std::forward<D>(declaration)}, index{index} {}

        std::shared_ptr<virtual_type> next() override;
        void traverse(const std::function<void(const std::shared_ptr<virtual_type>&)>& consumer) override;
        std::string to_string() override;
        match_result match(const std::shared_ptr<virtual_type>& type) override;

        std::shared_ptr<declaration::virtual_declaration> declaration;
        std::size_t index;
    };

    struct unbounded_type : virtual_type, enable_type_iterator {
        struct iterator : util::iterator<std::shared_ptr<virtual_type>> {
            explicit iterator(const std::shared_ptr<virtual_type>& repeated_type) : repeated_type{repeated_type} {}

            std::optional<std::shared_ptr<virtual_type>> next() override;

            const std::shared_ptr<virtual_type>& repeated_type;
        };

        template<typename R> requires std::is_convertible_v<R, std::shared_ptr<virtual_type>>
        explicit unbounded_type(R&& repeated_type) : repeated_type{std::forward<R>(repeated_type)} {}

        std::string to_string() override;
        match_result match(const std::shared_ptr<virtual_type>& type) override;

        std::unique_ptr<util::iterator<std::shared_ptr<virtual_type>>> type_iter() override;

        std::shared_ptr<virtual_type> repeated_type;
    };

    template<typename T, typename A, typename B> requires std::is_base_of_v<A, util::iterator<T>>
        && std::is_base_of_v<B, util::iterator<T>>
    std::pair<A, B> zip_iterate(A&& a, B&& b, const std::function<
        void(const std::shared_ptr<virtual_type>&, const std::shared_ptr<virtual_type>&)>& function) {
        for(std::optional<std::shared_ptr<virtual_type>> next_a; (next_a = a.next());) {
            if(const auto next_b = b.next()) {
                if(const auto& iterable = std::dynamic_pointer_cast<enable_type_iterator>(*next_a)) {
                    zip_iterate(iterable->type_iter(), b, function);
                    continue;
                }
                if(const auto& iterable = std::dynamic_pointer_cast<enable_type_iterator>(*next_b)) {
                    zip_iterator(a, iterable->type_iter(), function);
                    continue;
                }

                function(*next_a, *next_b);
                continue;
            }
            break;
        }
        return {std::forward<A>(a), std::forward<B>(b)};
    }

    struct generics_iterator : util::iterator<std::shared_ptr<virtual_type>> {
        explicit generics_iterator(const std::span<const util::maybe_ptr<expression::const_expression>> span)
            : span{span} {}

        std::optional<std::shared_ptr<virtual_type>> next() override;

        std::span<const util::maybe_ptr<expression::const_expression>> span;
        std::size_t index{0};
    };
}

namespace type::action {
    struct type_arguments_action : virtual_action {
        template<typename D> requires std::is_convertible_v<D, std::shared_ptr<declaration::virtual_declaration>>
        type_arguments_action(D&& declaration,
            std::vector<util::maybe_ptr<expression::const_expression>>&& type_arguments)
            : declaration{std::forward<D>(declaration)}, type_arguments{std::move(type_arguments)} {}

        void open() override;
        void close() override;

        std::shared_ptr<declaration::virtual_declaration> declaration;
        std::vector<util::maybe_ptr<expression::const_expression>> type_arguments;
    };
}
