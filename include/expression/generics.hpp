#pragma once

#include "expression.hpp"
#include "reflection.hpp"
#include "declaration/declaration.hpp"
#include "type/generics.hpp"

namespace expression::generics {
    struct default_type_argument final : const_expression {
        template<typename D> requires std::is_convertible_v<D, std::shared_ptr<declaration::virtual_declaration>>
        default_type_argument(D&& declaration, std::size_t index)
            : const_expression{declaration->generic_arguments_stack[0].list[index]->trace,
                      declaration->generic_arguments_stack[0].list[index]->type},
              declaration{std::forward<D>(declaration)}, index{index} {}

        std::shared_ptr<declaration::virtual_declaration> declaration;
        std::size_t index;
    };

    struct generics_spread final : const_expression {
        generics_spread(const diagnostics::trace& trace, util::maybe_ptr<const_expression>&& value)
            : const_expression{trace, std::make_shared<type::error_type>()},
              value{std::move(value)} {}

        util::maybe_ptr<const_expression> value;
    };
}
