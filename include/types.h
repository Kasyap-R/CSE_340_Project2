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

    auto starts_with(const std::vector<std::string> &prefix) const -> bool {
        if (rhs.size() < prefix.size()) {
            return false;
        }

        for (size_t i = 0; i < prefix.size(); i++) {
            if (rhs[i] != prefix[i]) {
                return false;
            }
        }
        return true;
    }

    auto longest_prefix_with(const Rule &other) const
        -> std::vector<std::string> {
        size_t smaller_rhs_size =
            rhs.size() > other.rhs.size() ? other.rhs.size() : rhs.size();

        std::vector<std::string> prefix;
        for (size_t i = 0; i < smaller_rhs_size; i++) {
            if (other.rhs[i] != rhs[i]) {
                break;
            }
            prefix.push_back(rhs[i]);
        }
        return prefix;
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
