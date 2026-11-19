#pragma once

#include <string_view>
#include <vector>

namespace diagnostics {
    struct registered_file {
        registered_file(const std::string_view name, const std::string_view data)
            : name{name}, line_start_iterators{data.cbegin()}, end{data.cend()} {}

        std::string_view name;
        std::vector<std::string_view::const_iterator> line_start_iterators;
        std::string_view::const_iterator end;
    };

    struct trace {
        trace() = default;

        trace(registered_file* file, const std::string_view source, const std::size_t row)
            : file{file}, source{source}, row{row} {}

        trace(registered_file* file, const std::string_view source)
            : trace{file, source, file->line_start_iterators.size()} {}

        [[nodiscard]] trace join(const trace& back) const;

        registered_file* file{};
        std::string_view source;
        std::size_t row{};
    };

    enum print_type {
        error,
        warning,
        hint,
    };

    void print(print_type type, const trace& trace, std::string_view message);
}
