#include "expression/left.hpp"

#include <format>

#include "declaration/identifier.hpp"
#include "parser/parser.hpp"

namespace expression::left {
    std::unique_ptr<virtual_expression> parse() {
        const auto token = parser::thread_context.next();

        switch(token) {
            case lexer::identifier:
                return declaration::identifier::resolve(token).expression_value();

            default:
                diagnostics::print(diagnostics::error, token, std::format("unexpected `{}`", token.source));
        }

        return std::make_unique<error_expression>(token);
    }
}
