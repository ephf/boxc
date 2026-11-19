#pragma once

#include "declaration.hpp"
#include "statement/scope.hpp"

namespace declaration::identifier {
    struct resolved {
        diagnostics::trace trace;
        std::string_view identifier;
        std::shared_ptr<statement::scope> scope;
        std::shared_ptr<statement::scope> opening_scope;
        std::shared_ptr<virtual_declaration> declaration;

        std::vector<std::shared_ptr<type::action::virtual_action>> actions{};
        generics::type_arguments override_type_arguments{};

        std::unique_ptr<expression::virtual_expression> expression_value();
        void declare(const std::shared_ptr<virtual_declaration>& new_declaration);
    };

    resolved resolve(const diagnostics::trace& identifier);
}
