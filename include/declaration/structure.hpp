#pragma once

#include <map>

#include "declaration.hpp"
#include "lexer/lexer.hpp"
#include "statement/scope.hpp"
#include "type/type.hpp"

namespace statement {
    struct virtual_statement;
}

namespace declaration {
    struct structure_declaration : virtual_declaration, statement::namespace_scope,
            std::enable_shared_from_this<structure_declaration> {
        explicit structure_declaration(const std::string_view identifier) : virtual_declaration{identifier} {}

        std::unique_ptr<expression::virtual_expression> reference(const diagnostics::trace& trace,
            std::vector<std::shared_ptr<type::action::virtual_action>>&& actions) override;

        static std::unique_ptr<statement::virtual_statement> parse(lexer::token keyword_token);

        std::map<std::string_view, std::shared_ptr<type::virtual_type>> fields{};
    };
}
