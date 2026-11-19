#pragma once

#include "type.hpp"

namespace type {
    struct primitive : virtual_type {
        enum data_type { i8, u8, i16, u16, i32, u32, i64, u64 };

        explicit primitive(const data_type data_type)
            : data_type{data_type} {}

        std::string to_string() override;
        match_result match(const std::shared_ptr<virtual_type>& type) override;

        data_type data_type;
    };
}
