#include "expression/right.hpp"

#include "expression/generics.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

namespace expression::right {
    namespace {
        struct operation {
            // ReSharper disable once CppNonExplicitConversionOperator
            operator int() const noexcept {
                return precedence;
            }

            int precedence;
            std::function<std::unique_ptr<virtual_expression>(std::unique_ptr<virtual_expression>&&)> parse;
        };
    }

    std::unique_ptr<virtual_expression> parse(std::unique_ptr<virtual_expression>&& left, const int precedence) {
        static const auto operation_table = [] constexpr {
            std::array<operation, lexer::MAX> table{};
            // table['~'] = {identifier, generics::generics_spread::parse};
            return table;
        }();

        while(const auto& operation = operation_table[parser::thread_context.current]) {
            if(operation.precedence >= precedence) break;

            const auto* left_compare = left.get();
            left = operation.parse(std::move(left));
            if(left.get() == left_compare) break;
        }

        return left;
    }
}
