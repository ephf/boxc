#include "type/structure.hpp"

#include <format>
#include <ranges>

namespace type {
    void structure_type::traverse(const std::function<void(const std::shared_ptr<virtual_type>&)>& consumer) {
        for(const auto& field_type : declaration->fields | std::views::values) {
            consumer(field_type);
        }
    }

    std::string structure_type::to_string() {
        return std::format("struct {}", declaration->identifier);
    }

    match_result structure_type::match(const std::shared_ptr<virtual_type>& type) {
        return {this == type.get()};
    }

    std::shared_ptr<declaration::virtual_declaration> structure_type::comparable_declaration() {
        return declaration;
    }
}
