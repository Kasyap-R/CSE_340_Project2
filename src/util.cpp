#include "util.h"
#include "lexer.h"
#include <sstream>

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
} // namespace util
