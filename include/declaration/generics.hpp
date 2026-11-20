#pragma once

#include "expression/expression.hpp"

namespace declaration {
    struct virtual_declaration;
}

namespace statement {
    struct scope;
}

namespace declaration::generics {
    struct generic_resolution {
        std::vector<std::shared_ptr<type::virtual_type>> type_arguments;
        std::shared_ptr<virtual_declaration> declaration;
    };

    void resolve_type_arguments_declaration(const std::shared_ptr<virtual_declaration>& declaration);
    generic_resolution resolve_declaration(const std::shared_ptr<virtual_declaration>& declaration,
        bool parse_type_arguments = true);
    void resolve_previous_generics(const std::shared_ptr<statement::scope>& scope);
    void lock_generics(const std::shared_ptr<virtual_declaration>& declaration);

    template<typename I>
        requires std::is_base_of_v<util::iterator<util::maybe_ptr<expression::const_expression>, I>, I>
    struct iterator : util::iterator_self<I, iterator<I>> {
        explicit iterator(I&& i) : util::iterator_self<I, iterator>{std::forward<I>(i)} {}

        iterator<util::flat_iterator<typename I::Type, expression::const_expression, I>> flatten() {
            return iterator<util::flat_iterator<typename I::Type, expression::const_expression, I>>{
                    util::flat_iterator<typename I::Type, expression::const_expression, I>{std::move(*this)}};
        }

        template<typename J>
        util::zip_iterator<typename I::Type, typename J::Type, I, J> zip(J&& iterator) {
            return util::zip_iterator<typename I::Type, typename J::Type, I, J>{std::move(*this),
                    std::forward<J>(iterator)};
        }
    };

    inline iterator<util::iterate<util::maybe_ptr<expression::const_expression>>> span_iter(
        const std::span<util::maybe_ptr<expression::const_expression>> span) {
        return iterator{util::iterate{span}};
    }
}
