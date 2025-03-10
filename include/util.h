#pragma once
#include "lexer.h"
#include <string>
namespace parser_util {
auto starts_rule(const Token &tok) -> bool;
auto is_follow_id_list(const Token &tok) -> bool;

} // namespace parser_util

namespace util {

auto join_vec_string(const std::vector<std::string> &vec,
                     const std::string &delimiter) -> std::string;

template <typename T>
auto merge_vectors(std::vector<T> &vec1, const std::vector<T> &vec2) -> void {
    vec1.insert(vec1.end(), vec2.begin(), vec2.end());
}
} // namespace util
