#pragma once

#include <functional>

#include "lexer/lexer.hpp"
#include "statement/scope.hpp"

namespace type::action {
    struct virtual_action;
}

namespace expression {
    struct unresolved;
}

namespace parser {
    struct context : lexer::token_lexer {
        context() = default;

        explicit context(diagnostics::registered_file* file)
            : token_lexer{file} {}

        std::vector<std::shared_ptr<type::action::virtual_action>> action_accumulation{};
        std::vector<std::weak_ptr<statement::scope>> stack{};
        std::vector<expression::unresolved*> statement_unresolved_list{};

        template<typename T>
        T reduce(T&& start, const int separator, const int terminator, std::function<T(T&&)>&& reducer) {
            while(current && current != terminator) {
                start = reducer(std::forward<T>(start));
                if(separator && !try_next(separator)) break;
            }
            if(terminator) expect(terminator);
            return start;
        }

        template<typename T>
        std::vector<T> collect(const int separator, const int terminator, std::function<T()>&& collector,
            std::vector<T>&& collection = std::vector<T>{}) {
            return reduce<std::vector<T>>(std::move(collection), separator, terminator,
                [collector = std::move(collector)](auto&& col) {
                    col.emplace_back(collector());
                    return col;
                });
        }

        std::size_t count(const int separator, const int terminator, std::function<void()>&& function) {
            return reduce<std::size_t>(0, separator, terminator, [function = std::move(function)](auto count) {
                function();
                return count + 1;
            });
        }

        void range(const int separator, const int terminator, std::function<void()>&& function) {
            reduce<std::monostate>(std::monostate{}, separator, terminator, [function = std::move(function)](auto) {
                function();
                return std::monostate{};
            });
        }
    };

    // TODO: thread_local for modules, normal for debugging
    extern context thread_context;
}
