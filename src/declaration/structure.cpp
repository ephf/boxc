#include "declaration/structure.hpp"

#include "declaration/identifier.hpp"
#include "expression/reflection.hpp"
#include "parser/parser.hpp"
#include "statement/wrapper.hpp"

namespace declaration {
    std::unique_ptr<expression::virtual_expression> structure_declaration::reference(const diagnostics::trace& trace,
        std::vector<std::shared_ptr<type::action::virtual_action>>&& actions) {
        return expression::reflection::wrap_actions(
            std::make_unique<expression::reflection>(std::make_shared<type::structure_type>(shared_from_this()), trace),
            std::move(actions));
    }

    std::unique_ptr<statement::virtual_statement> structure_declaration::parse(lexer::token) {
        auto resolved_identifier = identifier::resolve(parser::thread_context.expect(lexer::identifier));
        auto declaration = std::make_shared<structure_declaration>(resolved_identifier.identifier);

        resolved_identifier.declare(declaration);
        parser::thread_context.stack.emplace_back(declaration);

        generics::resolve_type_arguments_declaration(declaration);
        generics::resolve_previous_generics(declaration);

        parser::thread_context.expect('{');
        parser::thread_context.range(lexer::none, '}', [&] {
            const auto field_type = type::parse();
            const auto field_identifier = parser::thread_context.expect(lexer::identifier).source;
            declaration->fields.insert({field_identifier, field_type});
            parser::thread_context.expect(';');
        });

        parser::thread_context.stack.pop_back();
        generics::lock_generics(declaration);

        return std::make_unique<statement::keepalive<std::shared_ptr<structure_declaration>>>(std::move(declaration));
    }
}
