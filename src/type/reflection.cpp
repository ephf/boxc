#include "type/reflection.hpp"

namespace type {
    match_result reflection_type::match(const std::shared_ptr<virtual_type>& type) {
        if(const auto& reflection = std::dynamic_pointer_cast<reflection_type>(type)) {
            return open_and_match(reflected_type, reflection->reflected_type);
        }
        return {false};
    }
}
