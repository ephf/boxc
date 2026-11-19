#pragma once

#include "expression.hpp"
#include "parser/parser.hpp"
#include "type/auto.hpp"
#include "type/reflection.hpp"

namespace expression {
    struct reflection : const_expression {
        template<typename T> requires std::is_convertible_v<T, std::shared_ptr<type::virtual_type>>
        reflection(T&& reflected_type, const diagnostics::trace& trace)
            : const_expression{trace, std::make_shared<type::reflection_type>(std::forward<T>(reflected_type))} {}

        [[nodiscard]] std::shared_ptr<type::virtual_type>& reflected_type() const;

        static std::unique_ptr<reflection> wrap_actions(std::unique_ptr<reflection>&& reflection,
            std::vector<std::shared_ptr<type::action::virtual_action>>&& actions);
    };

    struct unresolved : reflection {
        explicit unresolved(const diagnostics::trace& trace)
            : reflection{std::make_shared<type::auto_type>(nullptr), trace},
              list_index{parser::thread_context.statement_unresolved_list.size()} {
            parser::thread_context.statement_unresolved_list.push_back(this);
        }

        ~unresolved() override {
            parser::thread_context.statement_unresolved_list[list_index] = nullptr;
        }

        std::size_t list_index;
    };
}
