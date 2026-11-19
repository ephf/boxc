#pragma once

#include "type.hpp"

namespace type {
    struct auto_type : virtual_type, enable_priority_match<enable_priority_match<>::auto_priority> {
        template<typename R>
            requires std::is_convertible_v<R, std::shared_ptr<virtual_type>>
        explicit auto_type(R&& reference)
            : reference{std::forward<R>(reference)} {}

        std::shared_ptr<virtual_type> next() override;
        void traverse(const std::function<void(const std::shared_ptr<virtual_type>&)>& consumer) override;
        std::string to_string() override;
        match_result match(const std::shared_ptr<virtual_type>& type) override;

        std::shared_ptr<virtual_type> reference;
        bool locked{false};
        std::vector<std::shared_ptr<virtual_type>> predicates{};
    };

    struct action_list_indirection final : auto_type {
        template<typename R>
            requires std::is_convertible_v<R, std::shared_ptr<virtual_type>>
        action_list_indirection(R&& reference, std::vector<std::shared_ptr<action::virtual_action>>&& actions)
            : auto_type{std::forward<R>(reference)}, actions{std::move(actions)} {}

        static std::shared_ptr<virtual_type> wrap_if_thread_context_action(const std::shared_ptr<virtual_type>& type);

        std::vector<std::shared_ptr<action::virtual_action>> actions;
    };
}
