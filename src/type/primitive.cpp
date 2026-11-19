#include "type/primitive.hpp"

namespace type {
    std::string primitive::to_string() {
        static constexpr std::string_view strings[]{"i8", "u8", "i16", "u16", "i32", "u32", "i64", "u64"};
        return std::string{strings[data_type]};
    }

    match_result primitive::match(const std::shared_ptr<virtual_type>& type) {
        if(const auto& primitive_type = std::dynamic_pointer_cast<primitive>(type)) {
            return {true, primitive_type->data_type == data_type};
        }
        return {false};
    }
}
