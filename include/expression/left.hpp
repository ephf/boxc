#pragma once

#include "expression.hpp"

namespace expression::left {
    std::unique_ptr<virtual_expression> parse();
}
