#include "util.h"
#include "lexer.h"
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
    std::vector<std::string> ordered;

    for (const std::string &item : order) {
        if (set.count(item) == 1) {
            ordered.push_back(item);
        }
    }

    if (ordered.size() != set.size()) {
        throw std::runtime_error(
            "All symbols in set were not present in order\n");
    }

    return ordered;
}

} // namespace util
