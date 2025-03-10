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
    for (size_t i = 0; i < grammar.non_term_order.size(); i++) {
        const std::string non_term = grammar.non_term_order[i];
        auto first_order =
            util::generate_ordered_vec(first[non_term], grammar.term_order);
        std::string first_order_string =
            util::join_vec_string(first_order, ", ");
        std::cout << "FIRST(" << non_term << ") = { " << first_order_string
                  << " }";
        if (i < grammar.non_term_order.size() - 1) {
            std::cout << "\n";
        }
    }
}

auto calc_first(const Grammar &grammar) -> FirstMap {
    auto nullable = calc_nullable(grammar);
    FirstMap first;

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

auto all_nullable(const std::vector<std::string> &symbols,
                  const std::unordered_set<std::string> &nullable) -> bool {
    for (size_t i = symbols.size() - 1; i >= 0; i--) {
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
