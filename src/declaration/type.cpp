#include "declaration/type.hpp"

#include "expression/reflection.hpp"

namespace declaration {
    std::unique_ptr<expression::virtual_expression> type_declaration::reference(const diagnostics::trace& trace,
        std::vector<std::shared_ptr<type::action::virtual_action>>&& actions) {
        return expression::reflection::wrap_actions(std::make_unique<expression::reflection>(type, trace),
            std::move(actions));
    }
}
