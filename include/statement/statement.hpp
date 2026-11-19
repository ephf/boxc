#pragma once

#include <memory>

namespace statement {
    struct virtual_statement {
        ~virtual_statement() = default;
    };

    std::unique_ptr<virtual_statement> parse();
}
