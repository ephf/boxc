#include "type/generics.hpp"

#include "expression/reflection.hpp"

namespace type::generics {
    std::shared_ptr<virtual_type> type_argument_reference::next() {
        const auto& type_argument = declaration->generic_arguments_stack.back()[index];

        if(const auto* reflection = dynamic_cast<expression::reflection*>(type_argument.get())) {
            return reflection->reflected_type();
        }

        return nullptr;
    }

    void type_argument_reference::traverse(const std::function<void(const std::shared_ptr<virtual_type>&)>& consumer) {
        next()->traverse(consumer);
    }

    std::string type_argument_reference::to_string() {
        return open()->to_string();
    }

    match_result type_argument_reference::match(const std::shared_ptr<virtual_type>& type) {
        return open()->match(type);
    }

    std::optional<std::shared_ptr<virtual_type>> generics_iterator::next() {
        return index >= span.size() ? std::nullopt : std::optional{span[index++]->type};
    }
}

namespace type::action {
    void type_arguments_action::open() {
        declaration->generic_arguments_stack.push_back(type_arguments);
    }

    void type_arguments_action::close() {
        declaration->generic_arguments_stack.pop_back();
    }
}
