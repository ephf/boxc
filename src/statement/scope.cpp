#include "statement/scope.hpp"

#include <ranges>

namespace statement {
    std::shared_ptr<declaration::virtual_declaration> scope::find(const std::string_view identifier) {
        const auto maybe_declaration = declarations.find(identifier);
        return maybe_declaration == declarations.end() ? nullptr : maybe_declaration->second;
    }

    std::shared_ptr<declaration::virtual_declaration> scope::find_on_stack(std::span<std::weak_ptr<scope>> stack,
        const std::string_view identifier) {
        for(const auto& scope : stack | std::views::reverse) {
            if(const auto& declaration = scope.lock()->find(identifier)) {
                return declaration;
            }
        }
        return nullptr;
    }
}
