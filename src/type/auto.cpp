#include "type/auto.hpp"

#include <format>
#include <ranges>

#include "parser/parser.hpp"

namespace type {
    std::shared_ptr<virtual_type> auto_type::next() {
        return reference;
    }

    void auto_type::traverse(const std::function<void(const std::shared_ptr<virtual_type>&)>& consumer) {
        consumer(open());
    }

    std::string auto_type::to_string() {
        return reference ? open()->to_string() : "auto";
    }

    match_result auto_type::match(const std::shared_ptr<virtual_type>& type) {
        const auto opened = type->open();
        if(this == opened.get()) return {true};

        const auto generate_recursive_hint = [&] {
            return match_result{
                    .match = false,
                    .display_mismatch_hint = [&] {
                        diagnostics::print(diagnostics::hint, {},
                            std::format("auto matched with `{}` contains itself", type->to_string()));
                    }
            };
        };

        if(opened->contains(this)) return generate_recursive_hint();

        for(const auto& predicate : predicates) {
            const auto opened_predicate = predicate->open();
            if(const match_result match = opened_predicate->match(type); !match) {
                return match;
            }

            if(const auto& opened_auto_type = std::dynamic_pointer_cast<auto_type>(opened)) {
                if(opened_predicate.get() == opened_auto_type.get()) continue;
                if(opened_predicate->contains(opened_auto_type.get())) return generate_recursive_hint();
                opened_auto_type->predicates.push_back(predicate);
            }
        }

        if(!locked) reference = action_list_indirection::wrap_if_thread_context_action(type);
        return {true};
    }

    std::shared_ptr<virtual_type> action_list_indirection::wrap_if_thread_context_action(
        const std::shared_ptr<virtual_type>& type) {
        return parser::thread_context.action_accumulation.empty()
               ? type : std::make_shared<action_list_indirection>(type,
                   std::vector{parser::thread_context.action_accumulation});
    }
}
