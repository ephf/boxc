#pragma once

#include <string_view>
#include <vector>

#include "generics.hpp"

namespace declaration::generics {
    enum : std::size_t {
        generics_bounded = SIZE_MAX,
    };
}

namespace declaration {
    struct virtual_declaration {
        explicit virtual_declaration(const std::string_view identifier) : identifier{identifier} {}

        virtual std::unique_ptr<expression::virtual_expression> reference(const diagnostics::trace& trace,
            std::vector<std::shared_ptr<type::action::virtual_action>>&& actions) = 0;

        virtual ~virtual_declaration() = default;

        std::string_view identifier;
        std::shared_ptr<statement::scope> declared_in{};

        std::vector<std::vector<util::maybe_ptr<expression::const_expression>>> generic_arguments_stack{};
        std::vector<std::pair<std::vector<util::maybe_ptr<expression::const_expression>>,
            std::shared_ptr<virtual_declaration>>> generic_overrides{};
        std::size_t generics_bound{generics::generics_bounded};
    };
}
