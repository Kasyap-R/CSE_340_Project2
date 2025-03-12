#include "util.h"
#include "lexer.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace parser_util {
auto starts_rule(const Token &tok) -> bool { return tok.token_type == ID; }
auto is_follow_id_list(const Token &tok) -> bool {
    return (tok.token_type == OR || tok.token_type == STAR);
}
} // namespace parser_util

namespace util {

auto join_vec_string(const std::vector<std::string> &vec,
                     const std::string &delimiter) -> std::string {
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) {
            oss << delimiter; // Add delimiter except for the first element
        }
        oss << vec[i];
    }
    return oss.str();
}

auto generate_ordered_vec(const std::unordered_set<std::string> &set,
                          const std::vector<std::string> &order)
    -> std::vector<std::string> {
    std::vector<std::string> ordered_set;

    // Add dollar sign first even though it is not in the order of terms
    if (set.count("$") == 1) {
        ordered_set.emplace_back("$");
    }

    for (const std::string &item : order) {
        if (set.count(item) == 1) {
            ordered_set.push_back(item);
        }
    }

    return ordered_set;
}

auto print_unordered_set(const std::unordered_set<std::string> &set) -> void {
    std::cout << "{ ";
    for (const auto &elem : set) {
        std::cout << elem << " ";
    }
    std::cout << "}\n";
}

} // namespace util
