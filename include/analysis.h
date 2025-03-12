#pragma once
#include "types.h"
#include <unordered_map>
#include <unordered_set>

using std::string;
using std::vector;

using SetMap = std::unordered_map<std::string, std::unordered_set<std::string>>;
using RuleMap = std::unordered_map<std::string, vector<Rule>>;
namespace analysis {
// Task 2
auto print_nullable(const Grammar &grammar) -> void;
auto calc_nullable(const Grammar &grammar) -> std::unordered_set<std::string>;

// Task 3
auto print_first(const Grammar &grammar) -> void;
auto calc_first(const Grammar &grammar) -> SetMap;
auto first_of_subset(const std::vector<std::string> &symbols,
                     const size_t start_idx, const SetMap &first,
                     const std::unordered_set<std::string> &nullable)
    -> std::unordered_set<std::string>;

// Task 4
auto print_follow(const Grammar &grammar) -> void;
auto calc_follow(const Grammar &grammar) -> SetMap;

// Task 5
auto print_left_factored(const Grammar &grammar) -> void;
auto calc_left_factored(Grammar grammar) -> std::vector<Rule>;
auto longest_com_prefix(const vector<Rule> &rules) -> vector<string>;
auto generate_rule_map(const std::vector<Rule> &rules) -> RuleMap;
auto find_postfixes(const vector<string> &prefix, const vector<Rule> &rules)
    -> vector<vector<string>>;
auto remove_rules_with_prefix(const vector<string> &prefix, vector<Rule> &rules)
    -> vector<Rule>;
auto rule_has_prefix(const Rule &rule, const vector<string> &prefix) -> bool;

// Util
auto all_nullable(const std::vector<std::string> &symbols,
                  const std::unordered_set<std::string> &nullable) -> bool;
auto print_set_map(const SetMap &map, const Grammar &grammar,
                   const std::string &set_name) -> void;

} // namespace analysis
