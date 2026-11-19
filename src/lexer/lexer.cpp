#include "lexer/lexer.hpp"

#include <algorithm>
#include <format>
#include <ranges>

namespace lexer {
    token token::next_after() const {
        const auto data = *std::ranges::find_if(std::views::iota(source.cend(), file->end), [&](auto it) {
            if(*it == '\n') file->line_start_iterators.push_back(it);
            return !std::isspace(*it);
        });

        if(data == file->end) {
            return token{none, file, {data, 0uz}};
        }

        if(std::isalpha(*data)) {
            return token{
                identifier, file, {
                    data, std::find_if(data, file->end, [](auto c) {
                        return !std::isalnum(c);
                    })
                }
            };
        }

        if(std::isdigit(*data)) {
            return token{
                integer, file, {
                    data, std::find_if(data, file->end, [](auto c) {
                        return !std::isdigit(c);
                    })
                }
            };
        }

        return token{*data, file, {data, 1uz}};
    }

    token token_lexer::next() {
        if(!current) {
            diagnostics::print(diagnostics::error, current, "expected token at end of file");
        }
        const auto next = current;
        current = current.next_after();
        return next;
    }

    token token_lexer::expect(const int type) {
        if(current != type) {
            diagnostics::print(diagnostics::error, current, std::format("unexpected `{}`", current.source));
        }
        return next();
    }

    token token_lexer::try_next(const int type) {
        return current == type ? next() : token{none, current.file, current.source};
    }
}
