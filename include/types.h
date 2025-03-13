#pragma once

#include "util.h"
#include <string>
#include <unordered_set>
#include <vector>

using IDList = std::vector<std::string>;

struct Rule {
    std::string lhs;
    IDList rhs;

    // Equality operator
    bool operator==(const Rule &other) const {
        return lhs == other.lhs && rhs == other.rhs;
    }

    Rule(const std::string &non_term, const IDList &symbols) {
        lhs = non_term;
        rhs = symbols;
    }

    auto to_string() const -> std::string {
        return lhs + " -> " + util::join_vec_string(rhs, " ") + " #";
    }

    auto starts_with(const std::string &first_symbol) const -> bool {
        if (rhs.empty()) {
            return false;
        }

        return rhs.at(0) == first_symbol;
    }
};

struct Grammar {
    std::unordered_set<std::string> non_terms;
    std::unordered_set<std::string> terms;
    std::vector<std::string> non_term_order;
    std::vector<std::string> term_order;
    std::vector<Rule> rules;
};

struct RuleHasher {
    auto operator()(const Rule &rule) const -> size_t {
        size_t hash = std::hash<std::string>()(rule.lhs);
        for (const std::string &symbol : rule.rhs) {
            hash ^= std::hash<std::string>()(symbol);
        }
        return hash;
    }
};
