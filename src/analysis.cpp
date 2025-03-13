#include "analysis.h"
#include "types.h"
#include "util.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <unordered_set>

namespace analysis {

auto print_nullable(const Grammar &grammar) -> void {
    auto nullable = calc_nullable(grammar);
    std::vector<std::string> nullable_order =
        util::generate_ordered_vec(nullable, grammar.non_term_order);
    std::string nullable_string = util::join_vec_string(nullable_order, ", ");
    std::cout << "Nullable = { " << nullable_string << " }";
}

auto calc_nullable(const Grammar &grammar) -> std::unordered_set<std::string> {
    // Init nullable set
    std::unordered_set<std::string> nullable;
    for (const Rule &rule : grammar.rules) {
        if (rule.rhs.empty()) {
            nullable.insert(rule.lhs);
        }
    }

    // Main Loop
    bool changed = true;
    while (changed) {
        changed = false;
        for (const Rule &rule : grammar.rules) {
            // Skip over what's already nullable
            if (nullable.count(rule.lhs) == 1) {
                continue;
            }

            if (all_nullable(rule.rhs, nullable)) {
                nullable.insert(rule.lhs);
                changed = true;
            }
        }
    }

    return nullable;
}

auto print_first(const Grammar &grammar) -> void {
    auto first = calc_first(grammar);
    print_set_map(first, grammar, "FIRST");
}

auto calc_first(const Grammar &grammar) -> SetMap {
    auto nullable = calc_nullable(grammar);
    SetMap first;

    // Init Terminals
    for (const auto &term : grammar.terms) {
        first[term].insert(term);
    }

    // Main Loop
    bool changed = true;
    while (changed) {
        changed = false;

        // Loop through all rules
        for (const Rule &rule : grammar.rules) {
            const std::string lhs = rule.lhs;
            size_t old_size = first[lhs].size();
            // ...and all symbols in the rhs of those rules
            for (const std::string &symbol : rule.rhs) {

                // If the current symbol is a terminal, add it to the first set
                // of this non_term and break
                if (grammar.terms.count(symbol) == 1) {
                    first[lhs].insert(symbol);
                    break;
                }

                // Otherwise if the symbol is a non-terminal, add its first set
                // to first(lhs)
                util::merge_sets(first[lhs], first[symbol]);

                if (nullable.count(symbol) == 0) {
                    break; // If symbol is not nullable, don't check further
                           // symbols
                }
            }
            // If a first set changed, we need to loop
            if (old_size != first[lhs].size()) {
                changed = true;
            }
        }
    }

    return first;
}

auto print_follow(const Grammar &grammar) -> void {
    auto follow = calc_follow(grammar);
    print_set_map(follow, grammar, "FOLLOW");
}

auto calc_follow(const Grammar &grammar) -> SetMap {
    auto nullable = calc_nullable(grammar);
    auto first = calc_first(grammar);

    SetMap follow;
    follow[grammar.non_term_order[0]].insert("$");

    // Initialization: Rules IV and V
    for (const Rule &rule : grammar.rules) {
        for (size_t i = 0; i < rule.rhs.size(); i++) {
            const std::string symbol = rule.rhs[i];
            if (grammar.non_terms.count(symbol) == 0) {
                continue;
            }
            // Rule IV + V
            util::merge_sets(follow[symbol],
                             first_of_subset(rule.rhs, i + 1, first, nullable));
        }
    }

    // Main Loop: Apply rules II and III until nothing changes

    bool changed = true;
    while (changed) {
        changed = false;
        // Loop through rules
        for (const Rule &rule : grammar.rules) {
            // ...and all symbols in those rules in reverse order
            for (int i = rule.rhs.size() - 1; i >= 0; i--) {
                const std::string symbol = rule.rhs[i];

                // Skip terminals
                if (grammar.terms.count(symbol) == 1) {
                    break;
                }

                size_t old_size = follow[symbol].size();

                // Rule II and III
                util::merge_sets(follow[symbol], follow[rule.lhs]);

                // If set sizes changed, we'll need to loop through all rules
                // again
                if (follow[symbol].size() != old_size) {
                    changed = true;
                }

                // If this isn't nullable, then stop
                if (nullable.count(symbol) == 0) {
                    break;
                }
            }
        }
    }

    return follow;
}

auto print_left_factored(const Grammar &grammar) -> void {
    auto factored_rules = calc_left_factored(grammar);
    print_rules(factored_rules);
}

auto print_left_recurse(const Grammar &grammar) -> void {
    auto rules = calc_left_recursed(grammar);
    print_rules(rules);
}

auto calc_left_factored(Grammar grammar) -> vector<Rule> {}

auto calc_left_recursed(Grammar grammar) -> vector<Rule> {
    // Setup
    RuleMap rule_map = gen_rule_map(grammar.rules);
    std::sort(grammar.non_term_order.begin(), grammar.non_term_order.end());
    const vector<string> &non_terms = grammar.non_term_order;

    for (size_t i = 0; i < non_terms.size(); i++) {
        const string &curr_nt = non_terms[i];

        auto &curr_nt_rules = rule_map.at(curr_nt);

        // Eliminate Indirect Left Recursion (rule for a non_term can't
        // start with a previous non_term)
        for (size_t j = 0; j < i; j++) {

            const string &prev_nt = non_terms[j];
            vector<Rule> rules_to_add;
            vector<Rule> rules_to_remove;
            for (const Rule &curr_rule : curr_nt_rules) {

                if (!curr_rule.starts_with(prev_nt)) {
                    continue;
                }

                // Replace prev_nt in curr_rule with each of prev_nt's rule's
                // rhs
                // (i.e if B -> Ac and A -> d, replace B -> Ac with B -> dc)
                vector<Rule> new_rules = replace_nt_for_rhs(
                    curr_rule, prev_nt, rule_map.at(prev_nt));

                rules_to_add.insert(rules_to_add.end(), new_rules.begin(),
                                    new_rules.end());
                rules_to_remove.push_back(curr_rule);
            }

            for (const Rule &rule : rules_to_remove) {
                curr_nt_rules.erase(rule);
            }
            for (const Rule &rule : rules_to_add) {
                curr_nt_rules.insert(rule);
            }
        }

        // Then Eliminate Direct Left Recursion
        std::pair<vector<Rule>, vector<Rule>> pair =
            split_by_left_recurse(curr_nt_rules);
        auto &recurse_rules = pair.first;
        auto &non_recurse_rules = pair.second;

        if (recurse_rules.empty()) {
            continue;
        }

        string new_nt = curr_nt + "1";
        rule_map[new_nt] =
            generate_recursed_new_nt_rules(recurse_rules, new_nt);

        for (const Rule &recurse_rule : recurse_rules) {
            curr_nt_rules.erase(recurse_rule);
        }

        for (Rule &non_recurse_rule : non_recurse_rules) {
            curr_nt_rules.erase(non_recurse_rule);
            non_recurse_rule.rhs.push_back(new_nt);
            curr_nt_rules.insert(non_recurse_rule);
        }
    }

    return rule_map_to_vec(rule_map);
}

auto generate_recursed_new_nt_rules(const vector<Rule> &recurse_rules,
                                    const string &new_nt)
    -> unordered_set<Rule, RuleHasher> {
    // Transform something like A -> Aabc into A1 -> abcA
    unordered_set<Rule, RuleHasher> new_rules;
    for (const Rule &rule : recurse_rules) {
        Rule new_rule(new_nt, IDList());
        new_rule.rhs.insert(new_rule.rhs.end(), rule.rhs.begin() + 1,
                            rule.rhs.end());
        new_rule.rhs.push_back(new_nt);
        new_rules.insert(new_rule);
    }
    // Add the epsilon rule (A1 -> epsilon) to provide a end point for the
    // derivation
    new_rules.insert(Rule(new_nt, IDList()));
    return new_rules;
}

auto replace_nt_for_rhs(const Rule &rule, const string &nt_to_replace,
                        const unordered_set<Rule, RuleHasher> &nt_rules)
    -> vector<Rule> {

    if (rule.rhs.empty() || rule.rhs[0] != nt_to_replace) {
        throw std::runtime_error("Expected NT to be first symbol in rule");
    }

    vector<Rule> replaced_rules;
    for (const Rule &nt_rule : nt_rules) {
        Rule new_rule{rule.lhs, IDList()};
        // Replace NT with one of its rhs
        new_rule.rhs.insert(new_rule.rhs.end(), nt_rule.rhs.begin(),
                            nt_rule.rhs.end());
        // Add the rest of the OG rule (after the nt_to_replace)
        new_rule.rhs.insert(new_rule.rhs.end(), rule.rhs.begin() + 1,
                            rule.rhs.end());
        replaced_rules.push_back(new_rule);
    }
    return replaced_rules;
}

auto split_by_left_recurse(const unordered_set<Rule, RuleHasher> &rules)
    -> std::pair<vector<Rule>, vector<Rule>> {
    std::pair<vector<Rule>, vector<Rule>> pair;
    for (const Rule &rule : rules) {
        if (!rule.starts_with(rule.lhs)) {
            pair.second.push_back(rule);
        } else {
            pair.first.push_back(rule);
        }
    }
    return pair;
}

//
// ========= Util =========
//

auto print_set_map(const SetMap &map, const Grammar &grammar,
                   const std::string &set_name) -> void {
    for (size_t i = 0; i < grammar.non_term_order.size(); i++) {
        const std::string non_term = grammar.non_term_order[i];
        auto first_order =
            util::generate_ordered_vec(map.at(non_term), grammar.term_order);
        std::string first_order_string =
            util::join_vec_string(first_order, ", ");
        std::cout << set_name << "(" << non_term << ") = { "
                  << first_order_string << " }";
        if (i < grammar.non_term_order.size() - 1) {
            std::cout << "\n";
        }
    }
}

auto all_nullable(const std::vector<std::string> &symbols,
                  const std::unordered_set<std::string> &nullable) -> bool {
    return std::all_of(symbols.begin(), symbols.end(),
                       [&nullable](const std::string &symbol) {
                           return nullable.count(symbol) == 1;
                       });
}

// Returns the First set for a series of symbols (i.e. First(ABCD))
auto first_of_subset(const std::vector<std::string> &symbols,
                     const size_t start_idx, const SetMap &first,
                     const std::unordered_set<std::string> &nullable)
    -> std::unordered_set<std::string> {
    std::unordered_set<std::string> subset_first;

    for (size_t i = start_idx; i < symbols.size(); i++) {
        const std::string &symbol = symbols[i];
        // Add this symbols first to subset_first
        subset_first.insert(first.at(symbol).begin(), first.at(symbol).end());
        // Stop when the symbol is not nullable
        if (nullable.count(symbol) == 0) {
            break;
        }
    }
    return subset_first;
}

auto gen_rule_map(const vector<Rule> &rules) -> RuleMap {
    RuleMap rule_map;
    for (const Rule &rule : rules) {
        rule_map[rule.lhs].insert(rule);
    }
    return rule_map;
}

auto print_rules(vector<Rule> &rules) -> void {
    vector<string> rule_strings(rules.size());
    std::transform(rules.begin(), rules.end(), rule_strings.begin(),
                   [](const Rule &rule) { return rule.to_string(); });
    std::sort(rule_strings.begin(), rule_strings.end());
    std::cout << util::join_vec_string(rule_strings, "\n");
}

auto rule_map_to_vec(const RuleMap &rule_map) -> vector<Rule> {
    vector<Rule> res;
    for (const auto &pair : rule_map) {
        for (const Rule &rule : pair.second) {
            res.push_back(rule);
        }
    }
    return res;
}

} // namespace analysis
