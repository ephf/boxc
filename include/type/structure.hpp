#pragma once

#include "type.hpp"
#include "declaration/structure.hpp"

namespace type {
    struct structure_type : virtual_type, enable_comparable_by_generics {
        template<typename D> requires std::is_convertible_v<D, std::shared_ptr<declaration::structure_declaration>>
        explicit structure_type(D&& declaration) : declaration{std::forward<D>(declaration)} {}

        void traverse(const std::function<void(const std::shared_ptr<virtual_type>&)>& consumer) override;
        std::string to_string() override;
        match_result match(const std::shared_ptr<virtual_type>& type) override;

        std::shared_ptr<declaration::virtual_declaration> comparable_declaration() override;

        std::shared_ptr<declaration::structure_declaration> declaration;
    };
}
