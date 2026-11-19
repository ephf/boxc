#pragma once

#include "declaration.hpp"

namespace declaration {
    struct type_declaration : virtual_declaration {
        template<typename T> requires std::is_convertible_v<T, std::shared_ptr<type::virtual_type>>
        type_declaration(const std::string_view identifier, T&& type)
            : virtual_declaration{identifier}, type{std::forward<T>(type)} {}

        std::unique_ptr<expression::virtual_expression> reference(const diagnostics::trace& trace,
            std::vector<std::shared_ptr<type::action::virtual_action>>&& actions) override;

        std::shared_ptr<type::virtual_type> type;
    };
}
