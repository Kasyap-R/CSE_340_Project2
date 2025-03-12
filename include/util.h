#pragma once
#include "lexer.h"
#include <string>
#include <unordered_set>

namespace parser_util {
auto starts_rule(const Token &tok) -> bool;
auto is_follow_id_list(const Token &tok) -> bool;

} // namespace parser_util

namespace util {

auto join_vec_string(const std::vector<std::string> &vec,
                     const std::string &delimiter) -> std::string;

auto generate_ordered_vec(const std::unordered_set<std::string> &set,
                          const std::vector<std::string> &order)
    -> std::vector<std::string>;

auto print_unordered_set(const std::unordered_set<std::string> &set) -> void;

template <typename T>
auto merge_vectors(std::vector<T> &vec1, const std::vector<T> &vec2) -> void {
    vec1.insert(vec1.end(), vec2.begin(), vec2.end());
}

template <typename T>
auto merge_sets(std::unordered_set<T> &set1, const std::unordered_set<T> &set2)
    -> void {
    set1.insert(set2.begin(), set2.end());
}

} // namespace util
