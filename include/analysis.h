#pragma once
#include "types.h"
#include <unordered_map>
#include <unordered_set>

using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

using SetMap = std::unordered_map<std::string, std::unordered_set<std::string>>;
using RuleMap = std::unordered_map<string, unordered_set<Rule, RuleHasher>>;
namespace analysis {
// Task 2
auto print_nullable_set(const Grammar &grammar) -> void;
auto calc_nullable(const Grammar &grammar) -> unordered_set<std::string>;

// Task 3
auto print_first_sets(const Grammar &grammar) -> void;
auto calc_first(const Grammar &grammar) -> SetMap;
auto first_of_subset(const std::vector<std::string> &symbols,
                     const size_t start_idx, const SetMap &first,
                     const std::unordered_set<std::string> &nullable)
    -> std::unordered_set<std::string>;

// Task 4
auto print_follow_sets(const Grammar &grammar) -> void;
auto calc_follow(const Grammar &grammar) -> SetMap;

// Task 5
auto print_left_factored_grammar(const Grammar &grammar) -> void;
auto calc_left_factored(Grammar grammar) -> std::vector<Rule>;
auto longest_shared_prefix(const unordered_set<Rule, RuleHasher> &rules)
    -> vector<string>;
auto postfix_of_rules_with_prefix(const unordered_set<Rule, RuleHasher> &rules,
                                  const vector<string> &prefix)
    -> vector<vector<string>>;
auto all_rules_that_start_with(const vector<string> &prefix,
                               const unordered_set<Rule, RuleHasher> &rules)
    -> vector<Rule>;

// Task 6
auto print_grammar_without_left_recursion(const Grammar &grammar) -> void;
auto eliminate_left_recursion(Grammar grammar) -> vector<Rule>;

auto split_by_left_recurse(const unordered_set<Rule, RuleHasher> &rules)
    -> std::pair<vector<Rule>, vector<Rule>>;
auto replace_nt_for_rhs(const Rule &rule, const string &nt_to_replace,
                        const unordered_set<Rule, RuleHasher> &nt_rules)
    -> vector<Rule>;
auto generate_recursed_new_nt_rules(const vector<Rule> &recurse_rules,
                                    const string &new_nt)
    -> unordered_set<Rule, RuleHasher>;

// Util
auto all_nullable(const std::vector<std::string> &symbols,
                  const std::unordered_set<std::string> &nullable) -> bool;
auto print_set_map(const SetMap &map, const Grammar &grammar,
                   const std::string &set_name) -> void;

auto gen_rule_map(const vector<Rule> &rules) -> RuleMap;
auto print_rules(vector<Rule> &rules) -> void;
auto rule_map_to_vec(const RuleMap &rule_map) -> vector<Rule>;

} // namespace analysis
