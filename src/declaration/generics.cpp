#include "declaration/generics.hpp"
#include "expression/generics.hpp"
#include "type/generics.hpp"

#include <ranges>

#include "declaration/type.hpp"
#include "expression/reflection.hpp"
#include "parser/parser.hpp"

namespace declaration::generics {
    void resolve_type_arguments_declaration(const std::shared_ptr<virtual_declaration>& declaration) {
        if(!parser::thread_context.try_next('<')) return;
        declaration->generic_arguments_stack.emplace_back();

        parser::thread_context.range(',', '>', [&, unbounded = false] mutable {
            auto maybe_argument_type = expression::parse();

            if(dynamic_cast<expression::unresolved*>(maybe_argument_type.get())) {
                const auto identifier_trace = maybe_argument_type->trace;
                auto base_generated_type = std::make_shared<type::auto_type>(nullptr);
                std::shared_ptr<type::virtual_type> generated_type = base_generated_type;

                parser::thread_context.stack.back().lock()->declarations.insert({
                        identifier_trace.source, std::make_shared<type_declaration>(identifier_trace.source,
                            std::make_shared<type::generics::type_argument_reference>(declaration,
                                declaration->generic_arguments_stack[0].size()))
                });

                // TODO: replace with ellipses
                if(parser::thread_context.try_next('~')) {
                    base_generated_type->locked = true;
                    generated_type = std::make_shared<type::generics::unbounded_type>(std::move(generated_type));
                    unbounded = true;
                }

                if(parser::thread_context.try_next(':')) {
                    base_generated_type->predicates =
                            parser::thread_context.try_next('(')
                            ? parser::thread_context.collect<std::shared_ptr<type::virtual_type>>(',', ')', [] {
                                return type::expect_reflection(expression::parse());
                            })
                            : std::vector{type::expect_reflection(expression::parse())};
                }

                if(unbounded) {
                    diagnostics::print(diagnostics::error, maybe_argument_type->trace,
                        "type argument defined after unbounded argument");
                    return;
                }

                declaration->generic_arguments_stack[0].emplace_back(std::make_unique<expression::reflection>(
                    std::move(generated_type), identifier_trace));
                return;
            }

            // TODO: variable generic declarations
        });
    }

    generic_resolution resolve_declaration(const std::shared_ptr<virtual_declaration>& declaration,
        const bool parse_type_arguments) {
        if(!declaration || declaration->generic_arguments_stack.empty()) return {{}, declaration};
        // ReSharper disable once CppUseStructuredBinding
        const auto& default_type_arguments = declaration->generic_arguments_stack[0];

        auto type_arguments =
                parse_type_arguments && parser::thread_context.try_next('<')
                ? parser::thread_context.collect<util::maybe_ptr<expression::const_expression>>(',', '>', [] {
                    return expression::const_expression::expect(expression::parse());
                })
                : std::vector<util::maybe_ptr<expression::const_expression>>{};

        type::generics::zip_iterate(
            type::generics::generics_iterator{default_type_arguments},
            type::generics::generics_iterator{type_arguments},
            [](const auto& default_argument, const auto& type_argument) {});

        for(auto i = 0uz; i < type_arguments.size(); i++) {
            if(i >= default_type_arguments.size()) {
                if(declaration->generics_bound == generics_bounded) {
                    diagnostics::print(diagnostics::warning,
                        type_arguments[i]->trace.join(type_arguments.back()->trace), "extra type arguments");
                }

                break;
            }

            const auto type_argument_trace = type_arguments[i]->trace;
            type_arguments[i] = expression::const_expression::expect(type::transform_expression(
                type_arguments[i].owned(), default_type_arguments[i]->type, type_argument_trace));
        }

        while(type_arguments.size() < default_type_arguments.size()) {
            const auto& default_type_argument = default_type_arguments[type_arguments.size()];

            if(auto* reflection = dynamic_cast<expression::reflection*>(default_type_argument.get())) {
                auto predicate_auto = std::make_shared<type::auto_type>(nullptr);
                predicate_auto->predicates.emplace_back(reflection->reflected_type());
                type_arguments.emplace_back(std::make_unique<expression::reflection>(std::move(predicate_auto),
                    reflection->trace));
                continue;
            }

            type_arguments.emplace_back(std::make_unique<expression::generics::default_type_argument>(declaration,
                type_arguments.size()));
        }

        for(const auto& [override_type_arguments, override_declaration] : declaration->generic_overrides) {
            if(type_arguments.list.size() < override_type_arguments.list.size()) continue;
            auto [resolved_type_arguments, resolved_declaration] = resolve_declaration(override_declaration, false);

            const auto type_arguments_action = std::make_shared<type::action::type_arguments_action>(
                resolved_declaration, std::move(resolved_type_arguments));
            const auto action_handler = type::action::destructor_handler{type_arguments_action};

            for(const auto& [override_type_argument, type_argument] : std::views::zip(override_type_arguments.list,
                    type_arguments.list)) {
                if(const auto match = type::open_and_match(override_type_argument->type, type_argument->type);
                    !match || !match.exact_match) {
                    goto next_override;
                }
            }

            return {std::move(type_arguments_action->type_arguments), std::move(resolved_declaration)};
        next_override:;
        }

        return {std::move(type_arguments), declaration};
    }

    void resolve_previous_generics(const std::shared_ptr<statement::scope>& scope) {
        for(auto& unresolved : parser::thread_context.statement_unresolved_list) {
            if(!unresolved) continue;
            if(const auto& declaration = scope->find(unresolved->trace.source)) {
                const auto& temp_expression = declaration->reference({}, {});
                if(auto* reflection = dynamic_cast<expression::reflection*>(temp_expression.get())) {
                    unresolved->type = std::move(reflection->type);
                    unresolved = nullptr;
                }
            }
        }
    }

    void lock_generics(const std::shared_ptr<virtual_declaration>& declaration) {
        if(declaration->generic_arguments_stack.empty()) return;

        const auto lock_span = [](const std::span<util::maybe_ptr<expression::const_expression>> span) {
            for(const auto& anchor_argument : span) {
                if(const auto* reflection = dynamic_cast<expression::reflection*>(anchor_argument.get())) {
                    reflection->reflected_type()->traverse([](const auto& type) {
                        if(const auto& auto_type = std::dynamic_pointer_cast<type::auto_type>(type)) {
                            auto_type->locked = true;
                        }
                    });
                }
            }
        };

        lock_span(declaration->generic_arguments_stack[0].list);
        for(const std::span unbounded : declaration->generic_arguments_stack[0].unbounded_arguments) {
            lock_span(unbounded);
        }
    }
}
