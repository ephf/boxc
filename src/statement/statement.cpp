#include "statement/statement.hpp"

#include <format>

#include "declaration/structure.hpp"
#include "expression/reflection.hpp"
#include "parser/parser.hpp"
#include "statement/wrapper.hpp"

namespace statement {
    static std::unique_ptr<virtual_statement> parse_internal() {
        if(parser::thread_context.current == lexer::identifier) {
            static const std::unordered_map<std::string_view,
                std::function<std::unique_ptr<virtual_statement>(lexer::token)>> statement_keywords{
                    {"struct", declaration::structure_declaration::parse},
            };

            if(const auto& keyword_it = statement_keywords.find(parser::thread_context.current.source);
                keyword_it != statement_keywords.end()) {
                return keyword_it->second(parser::thread_context.next());
            }
        }

        auto expression = expression::parse();
        parser::thread_context.expect(';');
        return std::make_unique<expression_statement>(std::move(expression));
    }

    std::unique_ptr<virtual_statement> parse() {
        auto statement = parse_internal();

        for(const auto& unresolved : parser::thread_context.statement_unresolved_list) {
            if(!unresolved) continue;
            diagnostics::print(diagnostics::error, unresolved->trace, std::format("unresolved identifier `{}`",
                unresolved->trace.source));
        }

        parser::thread_context.statement_unresolved_list.resize(0);
        return statement;
    }
}
