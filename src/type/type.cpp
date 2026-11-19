#include "type/type.hpp"

#include <format>
#include <ranges>

#include "expression/expression.hpp"
#include "expression/left.hpp"
#include "expression/reflection.hpp"
#include "expression/right.hpp"
#include "type/auto.hpp"
#include "type/generics.hpp"

namespace type {
    std::shared_ptr<virtual_type> virtual_type::next() {
        return nullptr;
    }

    void virtual_type::traverse(const std::function<void(const std::shared_ptr<virtual_type>&)>& consumer) {
        consumer(shared_from_this());
    }

    opened_type virtual_type::open() {
        std::vector<action::destructor_handler> action_handlers{};
        std::shared_ptr<virtual_type> next = shared_from_this(), type;

        do {
            type = std::move(next);
            if(const auto& indirection = std::dynamic_pointer_cast<action_list_indirection>(type)) {
                action_handlers.append_range(indirection->actions |
                    std::views::transform([](const auto& action) { return action::destructor_handler{action}; }));
            }
        } while((next = type->next()));

        return opened_type{std::move(type), std::move(action_handlers)};
    }

    bool virtual_type::contains(virtual_type* type) {
        bool contains = false;
        traverse([&](const auto& t) { contains |= t.get() == type; });
        return contains;
    }

    std::string error_type::to_string() {
        return "<error-type>";
    }

    match_result error_type::match(const std::shared_ptr<virtual_type>&) {
        return {true, false};
    }

    match_result open_and_match(const std::shared_ptr<virtual_type>& left, const std::shared_ptr<virtual_type>& right) {
        const auto opened_left = left->open(), opened_right = right->open();

        if(const auto &generics_compare_left = std::dynamic_pointer_cast<enable_comparable_by_generics>(opened_left),
                      &generics_compare_right = std::dynamic_pointer_cast<enable_comparable_by_generics>(opened_right);
            generics_compare_left && generics_compare_right) {
            if(const auto& declaration = generics_compare_left->comparable_declaration();
                declaration.get() == generics_compare_right->comparable_declaration().get()) {
                const auto find_action = [&, empty_type_arguments = declaration::generics::type_arguments{}]
                    (const std::vector<action::destructor_handler>& action_handlers)
                    -> const declaration::generics::type_arguments& {
                    for(const auto& action_handler : action_handlers | std::views::reverse) {
                        if(const auto& type_arguments_action = std::dynamic_pointer_cast<action::type_arguments_action>(
                            action_handler.action)) {
                            if(type_arguments_action->declaration.get() != declaration.get()) continue;
                            return type_arguments_action->type_arguments;
                        }
                    }
                    return empty_type_arguments;
                };

                const auto &left_type_arguments = find_action(opened_left.action_handlers),
                           &right_type_arguments = find_action(opened_right.action_handlers);

                // TODO: match unbounded type arguments
                for(const auto& [left_argument, right_argument] : std::views::zip(left_type_arguments.list,
                        right_type_arguments.list)) {
                    if(const auto match = open_and_match(left_argument->type, right_argument->type); !match) {
                        return match;
                    }
                }

                return {true};
            }
        }

        return std::dynamic_pointer_cast<enable_priority_match>(opened_right) &&
               !std::dynamic_pointer_cast<enable_priority_match>(opened_left)
               ? opened_right->match(opened_left) : opened_left->match(opened_right);
    }

    std::unique_ptr<expression::virtual_expression> transform_expression(
        std::unique_ptr<expression::virtual_expression>&& expression, const std::shared_ptr<virtual_type>& type,
        const diagnostics::trace& trace) {
        const auto match = open_and_match(type, expression->type);

        if(!match) {
            diagnostics::print(diagnostics::error, trace, std::format("type mismatch between `{}` and `{}`",
                expression->type->to_string(), type->to_string()));
            if(match.display_mismatch_hint) match.display_mismatch_hint();
        }

        return match.transformation ? match.transformation(std::move(expression)) : std::move(expression);
    }

    std::shared_ptr<virtual_type> expect_reflection(const std::unique_ptr<expression::virtual_expression>& expression) {
        if(const auto* reflection = dynamic_cast<expression::reflection*>(expression.get())) {
            return reflection->reflected_type();
        }

        diagnostics::print(diagnostics::error, expression->trace, "expected a type here");
        return std::make_shared<error_type>();
    }

    std::shared_ptr<virtual_type> parse() {
        return expect_reflection(expression::right::parse(expression::left::parse(), expression::right::identifier));
    }
}
