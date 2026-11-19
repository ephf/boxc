#include <fstream>
#include <sstream>

#include "declaration/type.hpp"
#include "parser/parser.hpp"
#include "statement/statement.hpp"
#include "type/primitive.hpp"

int main() {
    std::ifstream file{"tests/main.bc"};
    if(!file) {
        std::println(stderr, "failed to open file: {}", strerror(errno));
        return 1;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    const auto content = buffer.str();

    const auto registered_file = std::make_unique<diagnostics::registered_file>("tests/main.bc", content);
    parser::thread_context = parser::context{registered_file.get()};

    const auto std_scope = std::make_shared<statement::scope>();
    std::vector<std::string> std_primitive_strings{};

    for(auto i = 0u; i <= type::primitive::u64; i++) {
        auto type = std::make_shared<type::primitive>(static_cast<enum type::primitive::data_type>(i));
        std_primitive_strings.emplace_back(type->to_string());

        std_scope->declarations.insert({std_primitive_strings.back(),
                std::make_shared<declaration::type_declaration>(std_primitive_strings.back(), std::move(type))});
    }

    parser::thread_context.stack.emplace_back(std_scope);

    parser::thread_context.range(lexer::none, lexer::none, statement::parse);
}
