#pragma once

#include "expression.hpp"

namespace expression::right {
    enum {
        postfix = 1,
        multiplicative,
        additive,
        bitwise_shift,
        relational,
        equality,
        bitwise_and,
        bitwise_xor,
        bitwise_or,
        logical_and,
        logical_or,
        ternary,
        assignment,
        identifier,
        none,
    };

    std::unique_ptr<virtual_expression> parse(std::unique_ptr<virtual_expression>&& left, int precedence);
}
