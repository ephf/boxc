#pragma once

#include "expression/expression.hpp"

namespace declaration {
    struct virtual_declaration;
}

namespace statement {
    struct scope;
}

namespace declaration::generics {
    struct generic_resolution {
        std::vector<std::shared_ptr<type::virtual_type>> type_arguments;
        std::shared_ptr<virtual_declaration> declaration;
    };

    void resolve_type_arguments_declaration(const std::shared_ptr<virtual_declaration>& declaration);
    generic_resolution resolve_declaration(const std::shared_ptr<virtual_declaration>& declaration,
        bool parse_type_arguments = true);
    void resolve_previous_generics(const std::shared_ptr<statement::scope>& scope);
    void lock_generics(const std::shared_ptr<virtual_declaration>& declaration);
}
