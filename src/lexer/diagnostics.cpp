#include "lexer/diagnostics.hpp"

#include <print>
#include <ranges>

namespace diagnostics {
    trace trace::join(const trace& back) const {
        return {file, {source.begin(), back.source.end()}, row};
    }

    void print(const print_type type, const trace& trace, const std::string_view message) {
        const auto line_start = std::ranges::find_if(trace.file->line_start_iterators[trace.row - 1],
            std::unreachable_sentinel, [](auto c) { return !std::isspace(c); });
        const std::string_view line{
                line_start,
                std::ranges::find_if(line_start, trace.file->end, [](auto c) { return c == '\n'; })
        };

        static constexpr int color_codes[]{31, 33, 36};
        static constexpr const char* labels[]{"error", "warning", "hint"};

        std::println("{}:{}:\n{:>4} | {}\n     : \33[{}m{:>{}}\n\33[1m{}:\33[0m {}\n",
            trace.file->name, trace.row,
            trace.row, line, color_codes[type], std::string(trace.source.size(), '~'),
            trace.source.begin() - line.begin() + trace.source.size(), labels[type], message);
    }
}
