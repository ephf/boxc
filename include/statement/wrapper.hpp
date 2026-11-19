#pragma once

#include "statement.hpp"
#include "expression/expression.hpp"

namespace statement {
    template<typename T>
    struct keepalive : virtual_statement {
        explicit keepalive(T&& resource) : resource{std::forward<T>(resource)} {}

        T resource;
    };

    struct expression_statement : virtual_statement {
        explicit expression_statement(std::unique_ptr<expression::virtual_expression> expression)
            : expression{std::move(expression)} {}

        std::unique_ptr<expression::virtual_expression> expression;
    };
}
