#include "analysis.h"
#include "util.h"
#include <iostream>
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
            // Skip over whats already nullable
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

    for (const std::string &non_term : grammar.non_term_order) {
        auto first_order =
            util::generate_ordered_vec(first[non_term], grammar.term_order);
        std::string first_order_string =
            util::join_vec_string(first_order, ", ");
        std::cout << "First(" << non_term << ") = { " << first_order_string
                  << " }\n";
    }
}

auto calc_first(const Grammar &grammar) -> FirstMap {
    auto nullable = calc_nullable(grammar);
    FirstMap first;
    // Init Terminals
    for (const std::string &term : grammar.terms) {
        first[term].insert(term);
    }

    // Main Loop
    bool changed = true;
    while (changed) {
        changed = false;

        for (const Rule &rule : grammar.rules) {

            for (int i = rule.rhs.size() - 1; i >= 0; i--) {
            }
        }
    }

    return first;
}

auto all_nullable(const std::vector<std::string> &symbols,
                  const std::unordered_set<std::string> nullable) -> bool {
    for (int i = symbols.size() - 1; i >= 0; i--) {
        if (nullable.count(symbols[i]) == 0) {
            break;
        }
        if (i == 0) {
            return true;
        }
    }

    return false;
}

} // namespace analysis
