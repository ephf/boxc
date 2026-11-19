#pragma once

#include <variant>

#include "lexer/diagnostics.hpp"
#include "type/type.hpp"
#include "util/maybe_ptr.hpp"

namespace expression {
    struct const_expression;

    struct virtual_expression {
        template<typename T> requires std::is_convertible_v<T, std::shared_ptr<type::virtual_type>>
        virtual_expression(const diagnostics::trace& trace, T&& type)
            : trace{trace}, type{std::forward<T>(type)} {}

        virtual util::maybe_ptr<const_expression> into_const_expression();

        virtual ~virtual_expression() = default;

        diagnostics::trace trace;
        std::shared_ptr<type::virtual_type> type;
    };

    struct const_expression : virtual_expression {
        template<typename T> requires std::is_convertible_v<T, std::shared_ptr<type::virtual_type>>
        const_expression(const diagnostics::trace& trace, T&& type)
            : virtual_expression{trace, std::forward<T>(type)} {}

        util::maybe_ptr<const_expression> into_const_expression() override;

        static util::maybe_ptr<const_expression> expect(std::unique_ptr<virtual_expression>&& expression);
    };

    struct error_expression : const_expression {
        explicit error_expression(const diagnostics::trace& trace)
            : const_expression{trace, std::make_shared<type::error_type>()} {}
    };

    std::unique_ptr<virtual_expression> parse();
}
