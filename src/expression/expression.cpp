#include "expression/expression.hpp"

#include <format>

#include "expression/left.hpp"
#include "expression/right.hpp"

namespace expression {
    util::maybe_ptr<const_expression> virtual_expression::into_const_expression() {
        return nullptr;
    }

    util::maybe_ptr<const_expression> const_expression::into_const_expression() {
        return this;
    }

    util::maybe_ptr<const_expression> const_expression::expect(std::unique_ptr<virtual_expression>&& expression) {
        auto maybe_const_expression = expression->into_const_expression();

        if(!maybe_const_expression.get()) {
            diagnostics::print(diagnostics::error, expression->trace, std::format("`{}` is not a compiler constant",
                expression->trace.source));
            return std::make_unique<error_expression>(expression->trace);
        }

        return maybe_const_expression.owned_or(
            std::unique_ptr<const_expression>{dynamic_cast<const_expression*>(expression.release())});
    }

    std::unique_ptr<virtual_expression> parse() {
        return right::parse(left::parse(), right::none);
    }
}
