#pragma once

#include "diagnostics.hpp"

namespace lexer {
    enum {
        none,
        identifier = CHAR_MAX + 1,
        integer,

        MAX,
    };

    struct token : diagnostics::trace {
        token() = default;

        token(const int type, diagnostics::registered_file* file, const std::string_view source)
            : trace{file, source}, type{type} {}

        // ReSharper disable once CppNonExplicitConversionOperator
        operator int() const {
            return type;
        }

        [[nodiscard]] token next_after() const;

        int type{};
    };

    struct token_lexer {
        token_lexer() = default;

        explicit token_lexer(diagnostics::registered_file* file)
            : current{token{none, file, {file->line_start_iterators[0], 0uz}}.next_after()} {}

        token next();
        token expect(int type);
        token try_next(int type);

        token current{};
    };
}
