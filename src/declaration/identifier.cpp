#include "declaration/identifier.hpp"

#include <format>

#include "expression/reflection.hpp"
#include "parser/parser.hpp"
#include "type/generics.hpp"

namespace declaration::identifier {
    std::unique_ptr<expression::virtual_expression> resolved::expression_value() {
        if(!declaration) return std::make_unique<expression::unresolved>(trace);

        if(!override_type_arguments.empty()) {
            actions.emplace_back(
                std::make_shared<type::action::type_arguments_action>(declaration, std::move(override_type_arguments)));
        }

        return declaration->reference(trace, std::move(actions));
    }

    void resolved::declare(const std::shared_ptr<virtual_declaration>& new_declaration) {
        if(declaration && !override_type_arguments.empty()) {
            new_declaration->declared_in = declaration->declared_in;
            declaration->generic_overrides.emplace_back(std::move(override_type_arguments), new_declaration);
            return;
        }

        if(declaration && scope.get() == opening_scope.get()) {
            diagnostics::print(diagnostics::error, trace, std::format("`{}` is already declared in scope", identifier));
            return;
        }

        new_declaration->declared_in = scope;
        scope->declarations.insert({identifier, new_declaration});
    }

    resolved resolve(const diagnostics::trace& identifier) {
        auto opening_scope = parser::thread_context.stack.back().lock();
        resolved resolved{
                identifier, identifier.source, opening_scope, std::move(opening_scope),
                statement::scope::find_on_stack(parser::thread_context.stack, identifier.source),
        };

        auto generic_resolution = generics::resolve_declaration(resolved.declaration);
        resolved.declaration = generic_resolution.declaration;
        resolved.override_type_arguments = std::move(generic_resolution.type_arguments);

        // TODO: replace with double colon
        while(parser::thread_context.try_next(':')) {
            const auto identifier_token = parser::thread_context.expect(lexer::identifier);

            if(const auto& namespace_declaration = std::dynamic_pointer_cast<statement::namespace_scope>(
                resolved.declaration)) {
                resolved.scope = namespace_declaration;
                resolved.trace = resolved.trace.join(identifier_token);

                generic_resolution = generics::resolve_declaration(
                    namespace_declaration->find(identifier_token.source));

                if(!resolved.override_type_arguments.empty()) {
                    resolved.actions.emplace_back(std::make_shared<type::action::type_arguments_action>(
                        resolved.declaration, std::move(resolved.override_type_arguments)));
                }
                resolved.declaration = generic_resolution.declaration;
                resolved.override_type_arguments = std::move(generic_resolution.type_arguments);
            }

            diagnostics::print(diagnostics::error, resolved.trace, std::format("`{}` is not a namespace",
                resolved.trace.source));
        }

        return resolved;
    }
}
