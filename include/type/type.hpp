#pragma once

#include <functional>
#include <memory>
#include <string>

#include "lexer/diagnostics.hpp"
#include "util/iterator.hpp"

namespace expression {
    struct virtual_expression;
}

namespace declaration {
    struct virtual_declaration;
}

namespace type::action {
    struct virtual_action {
        virtual ~virtual_action() = default;

        virtual void open() = 0;
        virtual void close() = 0;
    };

    struct destructor_handler {
        explicit destructor_handler(const std::shared_ptr<virtual_action>& action)
            : action{action} {
            action->open();
        }

        ~destructor_handler() {
            action->close();
        }

        std::shared_ptr<virtual_action> action;
    };
}

namespace type {
    struct match_result {
        // ReSharper disable once CppNonExplicitConversionOperator
        operator bool() const {
            return match;
        }

        bool match;
        bool exact_match{true};
        std::function<std::unique_ptr<expression::virtual_expression>(
            std::unique_ptr<expression::virtual_expression>&&)> transformation{nullptr};
        std::function<void()> display_mismatch_hint{nullptr};
    };

    struct opened_type;

    struct virtual_type : std::enable_shared_from_this<virtual_type> {
        virtual std::shared_ptr<virtual_type> next();
        virtual void traverse(const std::function<void(const std::shared_ptr<virtual_type>&)>& consumer);
        virtual std::string to_string() = 0;
        virtual match_result match(const std::shared_ptr<virtual_type>& type) = 0;

        virtual ~virtual_type() = default;

        opened_type open();
        bool contains(virtual_type* type);
    };

    struct opened_type : std::shared_ptr<virtual_type> {
        opened_type(std::shared_ptr<virtual_type>&& type, std::vector<action::destructor_handler>&& action_handlers)
            : std::shared_ptr<virtual_type>{std::move(type)}, action_handlers{std::move(action_handlers)} {}

        std::vector<action::destructor_handler> action_handlers;
    };

    template<std::size_t...>
    struct enable_priority_match {
        enum : std::size_t {
            default_priority,
            auto_priority,
        };

        virtual ~enable_priority_match() = default;

        virtual constexpr std::size_t priority() = 0;
    };

    template<std::size_t Priority>
    struct enable_priority_match<Priority> : enable_priority_match<> {
        constexpr std::size_t priority() override {
            return Priority;
        }
    };

    struct error_type : virtual_type {
        std::string to_string() override;
        match_result match(const std::shared_ptr<virtual_type>& type) override;
    };

    struct enable_comparable_by_generics {
        virtual std::shared_ptr<declaration::virtual_declaration> comparable_declaration() = 0;

        virtual ~enable_comparable_by_generics() = default;
    };

    struct enable_type_iterator {
        virtual std::unique_ptr<util::iterator<std::shared_ptr<virtual_type>>> type_iter() = 0;

        virtual ~enable_type_iterator() = default;
    };

    match_result open_and_match(const std::shared_ptr<virtual_type>& left, const std::shared_ptr<virtual_type>& right);
    std::unique_ptr<expression::virtual_expression> transform_expression(
        std::unique_ptr<expression::virtual_expression>&& expression, const std::shared_ptr<virtual_type>& type,
        const diagnostics::trace& trace);
    std::shared_ptr<virtual_type> expect_reflection(const std::unique_ptr<expression::virtual_expression>& expression);
    std::shared_ptr<virtual_type> parse();
}
