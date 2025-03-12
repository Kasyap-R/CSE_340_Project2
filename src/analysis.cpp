#include "analysis.h"
#include "util.h"
#include <algorithm>
#include <iostream>
#include <iterator>
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
            // ...and all symbols in those rules
            for (int i = rule.rhs.size() - 1; i >= 0; i--) {
                const std::string symbol = rule.rhs[i];

                // Skip terminals
                if (grammar.terms.count(symbol) == 1) {
                    break;
                }

                size_t old_size = follow[symbol].size();

                // Rule II and III
                util::merge_sets(follow[symbol], follow[rule.lhs]);

                // If set sizes changed, we'll need to do this again
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
    vector<string> rule_strings(factored_rules.size());
    std::transform(factored_rules.begin(), factored_rules.end(),
                   rule_strings.begin(),
                   [](const Rule &rule) { return rule.to_string(); });
    std::sort(rule_strings.begin(), rule_strings.end());
    std::cout << util::join_vec_string(rule_strings, "\n");
}

auto calc_left_factored(Grammar grammar) -> std::vector<Rule> {
    // Init G'
    std::vector<Rule> factored_rules;

    // Map each (non_term -> vector of its rules)
    RuleMap rule_map = generate_rule_map(grammar.rules);
    std::unordered_map<string, int> factored_count;

    // Loop until we clear all non_terms from G
    while (!grammar.non_terms.empty()) {
        vector<string> non_terms_to_delete;
        for (const std::string &non_term : grammar.non_terms) {
            // Find the longest common prefix of this non_term's rules
            const vector<string> prefix =
                longest_com_prefix(rule_map.at(non_term));

            // If there is no prefix, this non_term has been fully factored,
            // meaning we can add all of its rules to G'
            if (prefix.empty()) {
                for (const Rule &rule : rule_map.at(non_term)) {
                    factored_rules.push_back(rule);
                }
                rule_map.erase(non_term);
                non_terms_to_delete.push_back(non_term);
                continue;
            }

            // Otherwise, we will combine all rules with that prefix into one
            // rule `A -> prefix A_new` Where A_new will represent all the
            // different postfixes that could follow that prefix (i.e. A_new ->
            // postfix1 | postfix2 | postfix3)

            vector<vector<string>> postfixes =
                find_postfixes(prefix, rule_map.at(non_term));
            rule_map.at(non_term) =
                remove_rules_with_prefix(prefix, rule_map.at(non_term));

            // Create our new non term (i.e. A_new)
            factored_count[non_term] = factored_count.count(non_term) == 1
                                           ? factored_count[non_term] + 1
                                           : 1;
            string new_non_term =
                non_term + std::to_string(factored_count[non_term]);

            // The new non_terms rules will have NO overlap (if they did that
            // means we didn't find the longest prefix)
            for (const vector<string> &postfix : postfixes) {
                factored_rules.emplace_back(new_non_term, postfix);
            }

            // Update rule_map to include A -> prefix A_new
            Rule new_rule = Rule{non_term, prefix};
            new_rule.rhs.push_back(new_non_term);
            rule_map[non_term].push_back(new_rule);
        }

        // Remove the non terms that have been fully left factored
        for (const string &non_term : non_terms_to_delete) {
            grammar.non_terms.erase(non_term);
        }
    }

    return factored_rules;
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

auto generate_rule_map(const std::vector<Rule> &rules) -> RuleMap {
    RuleMap rule_map;
    for (const Rule &rule : rules) {
        rule_map[rule.lhs].push_back(rule);
    }
    return rule_map;
}

auto longest_com_prefix(const vector<Rule> &rules) -> vector<string> {
    // SORRY LEETCODE GODS, will be implementing the n^2 * m solution, no tries
    // today

    vector<string> longest_prefix;
    string longest_prefix_string = "";

    for (size_t i = 0; i < rules.size() - 1; i++) {
        for (size_t j = i + 1; j < rules.size(); j++) {
            size_t smallest_rhs =
                std::min(rules[i].rhs.size(), rules[j].rhs.size());
            uint prefix_length = 0;
            auto &vec1 = rules[i].rhs;
            auto &vec2 = rules[j].rhs;
            for (size_t k = 0; k < smallest_rhs; k++) {
                if (vec1.at(k) != vec2.at(k)) {
                    break;
                }
                prefix_length++;
            }

            // Create the candidate prefix
            vector<string> candidate_prefix(vec1.begin(),
                                            vec1.begin() + prefix_length);
            string candidate_prefix_string =
                util::join_vec_string(candidate_prefix, "");

            // Update if longer or equal but lexicographically greater
            if (prefix_length > longest_prefix.size() ||
                (prefix_length == longest_prefix.size() &&
                 candidate_prefix_string > longest_prefix_string)) {
                longest_prefix = std::move(candidate_prefix);
                longest_prefix_string = std::move(candidate_prefix_string);
            }
        }
    }

    return longest_prefix;
}

auto find_postfixes(const vector<string> &prefix, const vector<Rule> &rules)
    -> vector<vector<string>> {
    vector<vector<string>> postfixes;

    for (const Rule &rule : rules) {
        if (rule_has_prefix(rule, prefix)) {
            // Add everything after the prefix as the postfix
            vector<string> postfix(rule.rhs.begin() + prefix.size(),
                                   rule.rhs.end());
            postfixes.push_back(postfix);
        }
    }

    return postfixes;
}

auto remove_rules_with_prefix(const vector<string> &prefix, vector<Rule> &rules)
    -> vector<Rule> {
    vector<Rule> filtered;

    std::copy_if(
        rules.begin(), rules.end(), std::back_inserter(filtered),
        [prefix](const Rule &rule) { return !rule_has_prefix(rule, prefix); });
    return filtered;
}

auto rule_has_prefix(const Rule &rule, const vector<string> &prefix) -> bool {
    if (rule.rhs.size() < prefix.size()) {
        return false;
    }
    for (size_t i = 0; i < prefix.size(); i++) {
        if (prefix[i] != rule.rhs[i]) {
            return false;
        }
    }
    return true;
}

} // namespace analysis
