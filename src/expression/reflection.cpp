#include "expression/reflection.hpp"

namespace expression {
    std::shared_ptr<type::virtual_type>& reflection::reflected_type() const {
        return std::static_pointer_cast<type::reflection_type>(type)->reflected_type;
    }

    std::unique_ptr<reflection> reflection::wrap_actions(std::unique_ptr<reflection>&& reflection,
        std::vector<std::shared_ptr<type::action::virtual_action>>&& actions) {
        if(actions.empty()) return reflection;
        auto& type = reflection->reflected_type();
        type = std::make_shared<type::action_list_indirection>(std::move(type), std::move(actions));
        return reflection;
    }
}
