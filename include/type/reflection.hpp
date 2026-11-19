#pragma once

#include "structure.hpp"
#include "type.hpp"

namespace type {
    struct reflection_type : structure_type {
        template<typename T> requires std::is_convertible_v<T, std::shared_ptr<virtual_type>>
        explicit reflection_type(T&& reflected_type)
            : structure_type{std::make_unique<declaration::structure_declaration>(std::string_view{})},
              reflected_type{std::forward<T>(reflected_type)} {}

        match_result match(const std::shared_ptr<virtual_type>& type) override;

        std::shared_ptr<virtual_type> reflected_type;
    };
}
