#pragma once

#include <memory>
#include <span>
#include <string_view>
#include <unordered_map>

namespace declaration {
    struct virtual_declaration;
}

namespace statement {
    struct scope {
        std::unordered_map<std::string_view, std::shared_ptr<declaration::virtual_declaration>> declarations{};

        std::shared_ptr<declaration::virtual_declaration> find(std::string_view identifier);
        static std::shared_ptr<declaration::virtual_declaration> find_on_stack(std::span<std::weak_ptr<scope>> stack,
            std::string_view identifier);
    };

    struct namespace_scope : scope {};
}
