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
};

struct Grammar {
    std::unordered_set<std::string> non_terms;
    std::unordered_set<std::string> terms;
    std::vector<std::string> non_term_order;
    std::vector<std::string> term_order;
    std::vector<Rule> rules;
};
